/* Undermountain Enhanced - VGA/VESA emulation: banked 256-colour modes, DAC, PNG dump */
#include "dtu.h"
#include <stdlib.h>
#include <string.h>
#include "platform.h"

static plat_mutex vlock;   /* video memory is read by the front-end thread */

uint8_t *vga_fb;                 /* whole video memory, 1 MB */
uint8_t vga_pal[768];
int vga_width = 320, vga_height = 200, vga_mode = 3, vga_bank;
uint32_t vga_lfb_addr;           /* 0: no linear frame buffer offered (VBE 1.2) */
unsigned long vga_frames;
static int dac_widx, dac_ridx, dac_comp;
static int vretrace;
static uint32_t display_start;

#define VRAM_SIZE (1u << 20)

typedef struct { uint16_t mode, w, h; } vmode;
static const vmode modes[] = { {0x100, 640, 400}, {0x101, 640, 480}, {0x103, 800, 600}, {0x105, 1024, 768} };
#define NMODES (int)(sizeof modes / sizeof modes[0])

void video_init(void)
{
    plat_mutex_init(&vlock);
    vga_fb = calloc(1, VRAM_SIZE);
    /* default palette: grey ramp so early screenshots are visible */
    for (int i = 0; i < 256; i++) vga_pal[i * 3] = vga_pal[i * 3 + 1] = vga_pal[i * 3 + 2] = (uint8_t)(i >> 2);
    {
        char *s = PTR(ROM_INFO);
        uint16_t *ml = PTR(ROM_INFO + 0x40);
        strcpy(s, "Undermountain Enhanced VGA");
        for (int i = 0; i < NMODES; i++) ml[i] = modes[i].mode;
        ml[NMODES] = 0xFFFF;
    }
}

static int banked(void) { return vga_mode >= 0x100; }

/* copy the 64 KB window at A0000 into video memory for the current bank */
void video_sync_window(void)
{
    if (banked()) memcpy(vga_fb + (vga_bank << 16), PTR(VGA_WINDOW), 65536);
    else if (vga_mode == 0x13) memcpy(vga_fb, PTR(VGA_WINDOW), 64000);
}

unsigned long vga_draws;                 /* switches to bank 0 ~ frames drawn */
static void set_bank(int b)
{
    if (b == 0 && vga_bank != 0) vga_draws++;
    if (b == vga_bank || !banked()) { vga_bank = b; return; }
    plat_lock(&vlock);
    memcpy(vga_fb + (vga_bank << 16), PTR(VGA_WINDOW), 65536);
    vga_bank = b & 15;
    memcpy(PTR(VGA_WINDOW), vga_fb + (vga_bank << 16), 65536);
    plat_unlock(&vlock);
}

/* Current picture as RGBA (R in the lowest byte), for the front end.
   Returns 0 if out is too small. */
int video_snapshot(uint32_t *out, int maxpix, int *w, int *h)
{
    int ww = vga_width, hh = vga_height;
    if (vga_mode == 3 || vga_mode == 2) { ww = 320; hh = 200; }
    if (ww * hh > maxpix) return 0;
    plat_lock(&vlock);
    video_sync_window();
    const uint8_t *src = vga_fb + display_start;
    uint32_t lut[256];
    for (int i = 0; i < 256; i++) {
        const uint8_t *c = vga_pal + i * 3;
        lut[i] = (uint32_t)(c[0] << 2 | c[0] >> 4) | (uint32_t)(c[1] << 2 | c[1] >> 4) << 8 |
                 (uint32_t)(c[2] << 2 | c[2] >> 4) << 16 | 0xFF000000u;
    }
    if (vga_mode == 3 || vga_mode == 2) for (int i = 0; i < ww * hh; i++) out[i] = 0xFF000000u;
    else for (int i = 0; i < ww * hh; i++) out[i] = lut[src[i]];
    plat_unlock(&vlock);
    {   /* mouse pointer of the text menus (menu.c) */
        int x0, y0, cw, ch; const uint32_t *img;
        if (vga_mode >= 0x100 && menu_pointer(&x0, &y0, &img, &cw, &ch))
            for (int y = 0; y < ch; y++) for (int x = 0; x < cw; x++) {
                int sx = x0 + x, sy = y0 + y;
                if (img[y * cw + x] && sx >= 0 && sy >= 0 && sx < ww && sy < hh) out[sy * ww + sx] = img[y * cw + x];
            }
    }
    *w = ww; *h = hh;
    return 1;
}

static int set_mode(int mode)
{
    int m = mode & 0x1FF;
    if (m == 0x13) { vga_width = 320; vga_height = 200; }
    else if (m == 0x03 || m == 0x02) { vga_width = 720; vga_height = 400; }
    else {
        int found = 0;
        for (int i = 0; i < NMODES; i++) if (modes[i].mode == m) { vga_width = modes[i].w; vga_height = modes[i].h; found = 1; }
        if (!found) { dtu_log("[video] set mode %Xh - unsupported", mode); return 0; }
    }
    plat_lock(&vlock);
    vga_mode = m;
    vga_bank = 0;
    display_start = 0;
    if (!(mode & 0x8000)) { memset(vga_fb, 0, VRAM_SIZE); memset(PTR(VGA_WINDOW), 0, 65536); }
    plat_unlock(&vlock);
    dtu_log("[video] mode %Xh %dx%d%s", m, vga_width, vga_height, (mode & 0x4000) ? " (LFB)" : "");
    return 1;
}

/* VBE function with a real-mode style buffer at linear 'buf' */
static int vbe_call(uint16_t ax, uint16_t bx, uint16_t cx, uint16_t dx, uint32_t buf, uint16_t *rbx, uint16_t *rcx, uint16_t *rdx)
{
    switch (ax) {
    case 0x4F00: {
        uint8_t *b = PTR(buf);
        int v2 = !memcmp(b, "VBE2", 4);
        memset(b, 0, v2 ? 512 : 256);
        memcpy(b, "VESA", 4);
        b[4] = 0x02; b[5] = 0x01;                       /* VBE 1.2 */
        *(uint32_t *)(b + 6) = ((ROM_INFO >> 4) << 16) | 0;          /* OEM string */
        *(uint32_t *)(b + 0x0E) = ((ROM_INFO >> 4) << 16) | 0x40;    /* mode list */
        *(uint16_t *)(b + 0x12) = 16;                   /* 1 MB */
        dtu_log("[vesa] controller info (buffer %08X%s)", buf, v2 ? ", VBE2 requested" : "");
        return 0x004F; }
    case 0x4F01: {
        uint8_t *b = PTR(buf);
        int i;
        for (i = 0; i < NMODES; i++) if (modes[i].mode == (cx & 0x1FF)) break;
        if (i == NMODES) { dtu_log("[vesa] mode info %Xh - unsupported", cx); return 0x014F; }
        memset(b, 0, 256);
        *(uint16_t *)(b + 0) = 0x1B;            /* supported, colour, graphics, (no LFB) */
        b[2] = 7; b[3] = 0;                     /* window A rw */
        *(uint16_t *)(b + 4) = 64;              /* granularity KB */
        *(uint16_t *)(b + 6) = 64;              /* window size KB */
        *(uint16_t *)(b + 8) = 0xA000;
        *(uint16_t *)(b + 0x10) = modes[i].w;   /* bytes per line */
        *(uint16_t *)(b + 0x12) = modes[i].w;
        *(uint16_t *)(b + 0x14) = modes[i].h;
        b[0x16] = 8; b[0x17] = 16; b[0x18] = 1; b[0x19] = 8; b[0x1A] = 1; b[0x1B] = 4;
        b[0x1D] = (uint8_t)((VRAM_SIZE / ((uint32_t)modes[i].w * modes[i].h)) - 1);
        b[0x1E] = 1;
        dtu_log("[vesa] mode info %Xh", cx);
        return 0x004F; }
    case 0x4F02: return set_mode(bx) ? 0x004F : 0x014F;
    case 0x4F03: *rbx = (uint16_t)vga_mode; return 0x004F;
    case 0x4F05:
        if ((bx & 0xFF00) == 0) { set_bank(dx); return 0x004F; }
        *rdx = (uint16_t)vga_bank; return 0x004F;
    case 0x4F06:
        if ((bx & 0xFF) == 1 || (bx & 0xFF) == 0) { *rbx = (uint16_t)vga_width; *rcx = (uint16_t)vga_width; *rdx = (uint16_t)(VRAM_SIZE / vga_width); return 0x004F; }
        return 0x014F;
    case 0x4F07:
        if ((bx & 0xFF) == 0 || (bx & 0xFF) == 0x80) { display_start = (uint32_t)dx * vga_width + cx; return 0x004F; }
        *rcx = (uint16_t)(display_start % vga_width); *rdx = (uint16_t)(display_start / vga_width); *rbx = 0; return 0x004F;
    case 0x4F08: *rbx = 0x0600; return 0x004F;
    default:
        dtu_log("[vesa] function %04X not supported", ax);
        return 0x014F;
    }
}

void video_vesa_rm(uint32_t rmaddr)
{
    uint32_t *d = PTR(rmaddr);           /* edi esi ebp res ebx edx ecx eax */
    uint16_t *w = PTR(rmaddr + 0x20);    /* flags es ds ... */
    uint16_t bx = (uint16_t)d[4], cx = (uint16_t)d[6], dx = (uint16_t)d[5];
    uint32_t buf = ((uint32_t)w[1] << 4) + (uint16_t)d[0];
    uint16_t ax = vbe_call((uint16_t)d[7], bx, cx, dx, buf, &bx, &cx, &dx);
    d[7] = (d[7] & 0xFFFF0000u) | ax;
    d[4] = (d[4] & 0xFFFF0000u) | bx; d[6] = (d[6] & 0xFFFF0000u) | cx; d[5] = (d[5] & 0xFFFF0000u) | dx;
    w[0] &= ~1;
}

void video_int10(dtu_regs *r)
{
    if (AH == 0x4F) {
        uint16_t bx = BX, cx = CX, dx = DX;
        AX = (uint16_t)vbe_call(AX, bx, cx, dx, EDI, &bx, &cx, &dx);
        BX = bx; CX = cx; DX = dx;
        return;
    }
    switch (AH) {
    case 0x00: set_mode(AL | ((AL & 0x80) ? 0x8000 : 0)); break;
    case 0x0F: AL = (uint8_t)(vga_mode < 0x100 ? vga_mode : 0x13); AH = 80; BH = 0; break;
    case 0x10:
        if (AL == 0x12) { /* set block of DAC registers, es:edx */
            uint8_t *p = PTR(EDX);
            for (int i = 0; i < CX && BX + i < 256; i++) memcpy(vga_pal + (BX + i) * 3, p + i * 3, 3);
        } else if (AL == 0x10) { vga_pal[BX * 3] = DH; vga_pal[BX * 3 + 1] = CH; vga_pal[BX * 3 + 2] = CL; }
        else if (AL == 0x17) { memcpy(PTR(EDX), vga_pal + BX * 3, CX * 3u); }
        break;
    case 0x11: case 0x12: case 0x01: case 0x02: case 0x03: case 0x05: case 0x06: case 0x07: case 0x08: case 0x09: case 0x0E:
        break;
    case 0x1A: AL = 0x1A; BL = 8; break;
    default:
        dtu_log("[video] int 10h AX=%04X BX=%04X not handled", AX, BX);
        break;
    }
}

uint32_t video_port_in(uint16_t port)
{
    switch (port) {
    case 0x3DA: case 0x3BA:
        vretrace = (vretrace + 1) & 3;
        if (vretrace == 2) vga_frames++;
        return vretrace >= 2 ? 0x09 : 0x00;
    case 0x3C9: {
        uint8_t v = vga_pal[dac_ridx * 3 + dac_comp];
        if (++dac_comp == 3) { dac_comp = 0; dac_ridx = (dac_ridx + 1) & 255; }
        return v; }
    case 0x3C7: return 3;
    case 0x3C8: return (uint32_t)dac_widx;
    case 0x3CC: return 0xE3;
    default: return 0xFF;
    }
}

void video_port_out(uint16_t port, uint8_t val)
{
    switch (port) {
    case 0x3C8: dac_widx = val; dac_comp = 0; break;
    case 0x3C7: dac_ridx = val; dac_comp = 0; break;
    case 0x3C9:
        vga_pal[dac_widx * 3 + dac_comp] = val & 63;
        if (++dac_comp == 3) { dac_comp = 0; dac_widx = (dac_widx + 1) & 255; }
        break;
    default: break;   /* sequencer/CRTC/graphics controller writes: ignored */
    }
}

/* ---- minimal PNG writer (stored deflate blocks, no dependencies) ---- */
static uint32_t crc_tab[256];
static uint32_t crc(uint32_t c, const uint8_t *p, size_t n)
{
    if (!crc_tab[1]) for (uint32_t i = 0; i < 256; i++) { uint32_t k = i; for (int j = 0; j < 8; j++) k = (k & 1) ? 0xEDB88320u ^ (k >> 1) : k >> 1; crc_tab[i] = k; }
    c ^= 0xFFFFFFFFu;
    while (n--) c = crc_tab[(c ^ *p++) & 255] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}
static void be32(uint8_t *p, uint32_t v) { p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v; }
static void chunk(FILE *f, const char *type, const uint8_t *data, uint32_t len)
{
    uint8_t h[8]; be32(h, len); memcpy(h + 4, type, 4);
    fwrite(h, 1, 8, f); fwrite(data, 1, len, f);
    uint32_t c = crc(crc(0, (const uint8_t *)type, 4), data, len);
    be32(h, c); fwrite(h, 1, 4, f);
}

int video_write_png(const char *path)
{
    int w = vga_width, h = vga_height;
    if (vga_mode == 3 || vga_mode == 2) { w = 320; h = 200; }
    plat_lock(&vlock);
    video_sync_window();
    plat_unlock(&vlock);
    size_t raw_len = (size_t)(w * 3 + 1) * h;
    uint8_t *raw = malloc(raw_len), *z;
    const uint8_t *src = vga_fb + display_start;
    int x0 = 0, y0 = 0, cw = 0, ch = 0; const uint32_t *img = NULL;
    int ptr = vga_mode >= 0x100 && menu_pointer(&x0, &y0, &img, &cw, &ch);
    for (int y = 0; y < h; y++) {
        uint8_t *row = raw + (size_t)y * (w * 3 + 1);
        row[0] = 0;
        for (int x = 0; x < w; x++) {
            const uint8_t *c = vga_pal + src[(size_t)y * w + x] * 3;
            row[1 + x * 3] = (uint8_t)(c[0] << 2 | c[0] >> 4);
            row[2 + x * 3] = (uint8_t)(c[1] << 2 | c[1] >> 4);
            row[3 + x * 3] = (uint8_t)(c[2] << 2 | c[2] >> 4);
            if (ptr && x >= x0 && y >= y0 && x < x0 + cw && y < y0 + ch && img[(y - y0) * cw + (x - x0)]) {
                uint32_t p = img[(y - y0) * cw + (x - x0)];
                row[1 + x * 3] = (uint8_t)p; row[2 + x * 3] = (uint8_t)(p >> 8); row[3 + x * 3] = (uint8_t)(p >> 16);
            }
        }
    }
    size_t nblk = (raw_len + 65534) / 65535;
    z = malloc(2 + raw_len + nblk * 5 + 4);
    size_t zl = 0;
    z[zl++] = 0x78; z[zl++] = 0x01;
    uint32_t a = 1, b = 0;
    for (size_t off = 0; off < raw_len; off += 65535) {
        size_t n = raw_len - off < 65535 ? raw_len - off : 65535;
        z[zl++] = (off + n == raw_len) ? 1 : 0;
        z[zl++] = (uint8_t)n; z[zl++] = (uint8_t)(n >> 8);
        z[zl++] = (uint8_t)~n; z[zl++] = (uint8_t)(~n >> 8);
        memcpy(z + zl, raw + off, n); zl += n;
    }
    for (size_t i = 0; i < raw_len; i++) { a = (a + raw[i]) % 65521; b = (b + a) % 65521; }
    be32(z + zl, (b << 16) | a); zl += 4;
    FILE *f = fopen(path, "wb");
    if (!f) { free(raw); free(z); return -1; }
    static const uint8_t sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    uint8_t ihdr[13];
    be32(ihdr, (uint32_t)w); be32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8; ihdr[9] = 2; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
    fwrite(sig, 1, 8, f);
    chunk(f, "IHDR", ihdr, 13);
    chunk(f, "IDAT", z, (uint32_t)zl);
    chunk(f, "IEND", NULL, 0);
    fclose(f);
    free(raw); free(z);
    return 0;
}

/* ---------------- movies (mve.c) ---------------- */
/* 640x480 screen, black palette and picture */
void video_movie_begin(void)
{
    if (vga_mode != 0x101) set_mode(0x101);
    plat_lock(&vlock);
    memset(vga_fb, 0, VRAM_SIZE);
    memset(PTR(VGA_WINDOW), 0, 65536);
    memset(vga_pal, 0, sizeof vga_pal);
    display_start = 0;
    plat_unlock(&vlock);
}

/* show a w x h 8-bit picture centred on the 640x480 screen; pal: 768 6-bit values or NULL */
void video_movie_frame(const uint8_t *pic, int w, int h, const uint8_t *pal)
{
    int x0 = (640 - w) / 2, y0 = (480 - h) / 2;
    if (x0 < 0 || y0 < 0) return;
    plat_lock(&vlock);
    if (pal) memcpy(vga_pal, pal, 768);
    for (int y = 0; y < h; y++) memcpy(vga_fb + (size_t)(y0 + y) * 640 + x0, pic + (size_t)y * w, (size_t)w);
    if (banked()) memcpy(PTR(VGA_WINDOW), vga_fb + (vga_bank << 16), 65536);
    vga_frames++;
    plat_unlock(&vlock);
}
