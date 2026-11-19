/* Undermountain Enhanced - DPMI host (int 31h) and interrupt dispatch */
#define _GNU_SOURCE
#include "dtu.h"
#include <stdlib.h>
#include <string.h>
#include "platform.h"

static uint32_t pm_vec[256];
extern uint8_t dtu_iret_stub[];

/* ---- selectors: everything is flat, we only remember what was asked ---- */
#define NSEL 256
static struct { int used; uint32_t base, limit; uint16_t access; } sel[NSEL];
#define SEL_FIRST 0x100

extern uint32_t dtu_sel_base[65536];   /* x64/dtu-asm.asm, read by translated FS/GS code */
static void sel_set_base(uint16_t s, uint32_t base) { for (int rpl = 0; rpl < 4; rpl++) dtu_sel_base[(s & ~3) | rpl] = base; }

static int sel_index(uint16_t s) { int i = (s - SEL_FIRST) >> 3; return (s >= SEL_FIRST && i < NSEL) ? i : -1; }

/* ---- extended memory (0501h) below 4 GB ---- */
#define NMEM 512
static struct { uint32_t addr, size; } mem[NMEM];
static uint32_t mem_total;

static uint32_t mem_alloc(uint32_t size)
{
    void *p = plat_alloc32(size);
    if (!p) return 0;
    for (int i = 0; i < NMEM; i++)
        if (!mem[i].addr) { mem[i].addr = ADDR(p); mem[i].size = size; mem_total += size; return ADDR(p); }
    plat_free32(p, size);
    return 0;
}

static int mem_find(uint32_t a) { for (int i = 0; i < NMEM; i++) if (mem[i].addr == a) return i; return -1; }

/* ---- conventional memory (0100h, int 21h/48h): first fit in 0x20000-0x9FFFF ---- */
#define NDOS 64
static struct { uint32_t seg, paras; } dblk[NDOS];

uint32_t dosmem_alloc(uint32_t paras, uint32_t *maxparas)
{
    uint32_t seg = DOSMEM_START >> 4, best = 0;
    for (;;) {
        int moved = 0;
        for (int i = 0; i < NDOS; i++)
            if (dblk[i].paras && seg < dblk[i].seg + dblk[i].paras && seg + paras > dblk[i].seg) {
                seg = dblk[i].seg + dblk[i].paras; moved = 1;
            }
        if (!moved) break;
    }
    if (seg + paras > (DOSMEM_END >> 4)) {
        /* The game checks for ~300 KB of free conventional memory at start
           (a DOS sound-driver requirement) but only allocates a few KB.
           Report a full 512 KB as the largest block. */
        best = (DOSMEM_END >> 4) > seg ? (DOSMEM_END >> 4) - seg : 0;
        if (maxparas) *maxparas = paras >= 0x8000 ? 0x8000 : best;
        if (paras < 0x8000) dtu_log("[dos] conventional memory exhausted (%u paras requested)", paras);
        return 0;
    }
    for (int i = 0; i < NDOS; i++)
        if (!dblk[i].paras) { dblk[i].seg = seg; dblk[i].paras = paras; memset(PTR(seg << 4), 0, paras << 4); return seg; }
    if (maxparas) *maxparas = 0;
    return 0;
}

int dosmem_free(uint32_t seg)
{
    for (int i = 0; i < NDOS; i++) if (dblk[i].paras && dblk[i].seg == seg) { dblk[i].paras = 0; return 1; }
    return 0;
}

void dpmi_init(void)
{
    for (int i = 0; i < 256; i++) pm_vec[i] = ADDR(dtu_iret_stub);
}

uint32_t dpmi_get_vector(int n) { return pm_vec[n & 255]; }

void dpmi_set_vector(int n, uint32_t addr)
{
    dtu_log("[dpmi] set vector %02Xh -> %08X", n & 255, addr);
    pm_vec[n & 255] = addr ? addr : ADDR(dtu_iret_stub);
}

uint32_t dtu_irq_handler(int irq)
{
    uint32_t v = pm_vec[irq < 8 ? 8 + irq : 0x70 + irq - 8];
    return v == ADDR(dtu_iret_stub) ? 0 : v;
}

/* ---- real-mode interrupt simulation (0300h) ---- */
#pragma pack(push, 1)
typedef struct {
    uint32_t edi, esi, ebp, res, ebx, edx, ecx, eax;
    uint16_t flags, es, ds, fs, gs, ip, cs, sp, ss;
} rm_regs;
#pragma pack(pop)

void video_vesa_rm(uint32_t rmaddr);
void mouse_int33(dtu_regs *r);

void real_mode_int(int n, uint32_t rmaddr)
{
    rm_regs *rm = PTR(rmaddr);
    dtu_regs t;
    memset(&t, 0, sizeof t);
    t.eax.e = rm->eax; t.ebx.e = rm->ebx; t.ecx.e = rm->ecx; t.edx.e = rm->edx;
    t.esi.e = rm->esi; t.edi.e = rm->edi;
    switch (n) {
    case 0x10:
        if ((rm->eax & 0xff00) == 0x4f00) { video_vesa_rm(rmaddr); return; }
        video_int10(&t);
        break;
    case 0x2F:
        {
            uint16_t f = (uint16_t)rm->eax;
            int ptr = (f == 0x1501 || f == 0x150D || f == 0x1510);
            if (ptr) t.ebx.e = ((uint32_t)rm->es << 4) + (uint16_t)rm->ebx;   /* ES:BX buffer -> linear */
            dos_int2f(&t);
            if (ptr) t.ebx.e = rm->ebx;
        }
        break;
    case 0x33:
        mouse_int33(&t);
        break;
    case 0x21:
        /* real-mode DOS call: translate ds:dx style pointers to linear */
        t.edx.e = ((uint32_t)rm->ds << 4) + (uint16_t)rm->edx;
        dos_int21(&t);
        break;
    default:
        dtu_log("[dpmi] real-mode int %02Xh AX=%04X not handled", n, (uint16_t)rm->eax);
        rm->flags |= 1;
        return;
    }
    rm->eax = t.eax.e; rm->ebx = t.ebx.e; rm->ecx = t.ecx.e; rm->edx = t.edx.e;
    rm->esi = t.esi.e; rm->edi = t.edi.e;
    rm->flags = (uint16_t)((rm->flags & ~1) | (t.eflags.e & 1));
}

void dpmi_int31(dtu_regs *r)
{
    CLR_CF();
    switch (AX) {
    case 0x0000: { /* allocate descriptors */
        int n = CX, start = -1;
        for (int i = 0; i + n <= NSEL && start < 0; i++) {
            int ok = 1;
            for (int j = 0; j < n; j++) if (sel[i + j].used) { ok = 0; break; }
            if (ok) start = i;
        }
        if (start < 0 || n == 0) { AX = 0x8011; SET_CF(); break; }
        for (int j = 0; j < n; j++) { sel[start + j].used = 1; sel[start + j].base = 0; sel[start + j].limit = 0; sel[start + j].access = 0xF2; sel_set_base((uint16_t)(SEL_FIRST + (start + j) * 8), 0); }
        AX = (uint16_t)(SEL_FIRST + start * 8);
        dtu_log("[dpmi] alloc %d selector(s) -> %04X", n, AX);
        break; }
    case 0x0001: { int i = sel_index(BX); if (i >= 0) sel[i].used = 0; break; }
    case 0x0002: AX = (uint16_t)(0x8000 | ((BX >> 1) & 0x7FF8)); sel_set_base(AX, (uint32_t)BX << 4); break;
    case 0x0003: AX = 8; break;
    case 0x0006: { int i = sel_index(BX); uint32_t b = i >= 0 ? sel[i].base : 0; CX = (uint16_t)(b >> 16); DX = (uint16_t)b; break; }
    case 0x0007: { int i = sel_index(BX); if (i >= 0) sel[i].base = ((uint32_t)CX << 16) | DX;
        sel_set_base(BX, ((uint32_t)CX << 16) | DX);
        dtu_log("[dpmi] set base %04X = %08X", BX, ((uint32_t)CX << 16) | DX); break; }
    case 0x0008: { int i = sel_index(BX); if (i >= 0) sel[i].limit = ((uint32_t)CX << 16) | DX; break; }
    case 0x0009: { int i = sel_index(BX); if (i >= 0) sel[i].access = CX; break; }
    case 0x000A: { int i = -1; for (int k = 0; k < NSEL; k++) if (!sel[k].used) { i = k; break; }
        if (i < 0) { SET_CF(); break; } sel[i].used = 1; AX = (uint16_t)(SEL_FIRST + i * 8);
        sel_set_base(AX, dtu_sel_base[BX]); break; }
    case 0x000B: case 0x000C: break;
    case 0x0100: {
        uint32_t mx = 0, seg = dosmem_alloc(BX, &mx);
        if (!seg) { AX = 8; BX = (uint16_t)mx; SET_CF(); break; }
        AX = (uint16_t)seg;
        for (int i = 0; i < NDOS; i++) if (dblk[i].paras && dblk[i].seg == seg) DX = (uint16_t)(0x4000 + i * 8);
        sel_set_base(DX, seg << 4);
        { uint32_t *st = PTR(r->esp.e); dtu_log("[dpmi] dos alloc %u paras -> seg %04X (stack %08X %08X %08X %08X %08X %08X %08X %08X)", BX, seg, st[0], st[1], st[2], st[3], st[4], st[5], st[6], st[7]); }
        break; }
    case 0x0101: { int i = (DX - 0x4000) >> 3; if (DX >= 0x4000 && i < NDOS) dblk[i].paras = 0; break; }
    case 0x0102: break;
    case 0x0200: CX = 0; DX = 0; break;
    case 0x0201: break;
    case 0x0202: CX = 0x18; EDX = ADDR(dtu_iret_stub); break;
    case 0x0203: break;
    case 0x0204: CX = 0x18; EDX = dpmi_get_vector(BL); break;
    case 0x0205: dpmi_set_vector(BL, EDX); break;
    case 0x0300: dtu_log("[dpmi] real-mode int %02Xh AX=%04X from %08X", BL, *(uint16_t *)PTR(EDI + 0x1C), r->eip.e); real_mode_int(BL, EDI); break;
    case 0x0301: case 0x0302:
        dtu_log("[dpmi] call real-mode procedure (ignored)"); break;
    case 0x0303: CX = 0xF000; DX = 0x0100; break;
    case 0x0304: break;
    case 0x0400: AX = 0x005A; BX = 0x0005; CL = 5; DX = 0x0870; break;
    case 0x0500: {
        uint32_t *info = PTR(EDI);
        memset(info, 0xFF, 0x30);
        info[0] = 64u << 20;            /* largest free block */
        info[1] = (64u << 20) >> 12;    /* max unlocked pages */
        info[2] = (64u << 20) >> 12;    /* max locked pages */
        info[3] = (128u << 20) >> 12;   /* linear space pages */
        info[4] = (64u << 20) >> 12;    /* total unlocked */
        info[5] = (64u << 20) >> 12;    /* free pages */
        info[6] = (128u << 20) >> 12;   /* total physical */
        info[7] = (128u << 20) >> 12;
        info[8] = 0;
        break; }
    case 0x0501: {
        uint32_t size = ((uint32_t)BX << 16) | CX;
        uint32_t a = mem_alloc(size);
        dtu_log("[dpmi] alloc %u bytes -> %08X (total %u KB)", size, a, mem_total >> 10);
        if (!a) { AX = 0x8012; SET_CF(); break; }
        BX = (uint16_t)(a >> 16); CX = (uint16_t)a; SI = (uint16_t)(a >> 16); DI = (uint16_t)a;
        break; }
    case 0x0502: {
        uint32_t a = ((uint32_t)SI << 16) | DI;
        int i = mem_find(a);
        if (i >= 0) { plat_free32(PTR(a), mem[i].size); mem_total -= mem[i].size; mem[i].addr = 0; }
        break; }
    case 0x0503: {
        uint32_t a = ((uint32_t)SI << 16) | DI, size = ((uint32_t)BX << 16) | CX;
        int i = mem_find(a);
        if (i < 0) { AX = 0x8023; SET_CF(); break; }
        if (size <= mem[i].size) { BX = (uint16_t)(a >> 16); CX = (uint16_t)a; break; }
        uint32_t n = mem_alloc(size);
        if (!n) { AX = 0x8012; SET_CF(); break; }
        memcpy(PTR(n), PTR(a), mem[i].size);
        plat_free32(PTR(a), mem[i].size); mem_total -= mem[i].size; mem[i].addr = 0;
        BX = (uint16_t)(n >> 16); CX = (uint16_t)n; SI = (uint16_t)(n >> 16); DI = (uint16_t)n;
        dtu_log("[dpmi] resize %08X -> %08X (%u bytes)", a, n, size);
        break; }
    case 0x0600: case 0x0601: case 0x0602: case 0x0603: case 0x0702: case 0x0703: break;
    case 0x0604: BX = 0; CX = 4096; break;
    case 0x0800: {
        uint32_t phys = ((uint32_t)BX << 16) | CX;
        uint32_t lin = phys;
        if (phys == vga_lfb_addr && vga_lfb_addr) lin = ADDR(vga_fb);
        else if (phys < LOWMEM_END) lin = phys;
        else dtu_log("[dpmi] map physical %08X (size %08X) - unknown", phys, ((uint32_t)SI << 16) | DI);
        BX = (uint16_t)(lin >> 16); CX = (uint16_t)lin;
        break; }
    case 0x0801: break;
    case 0x0900: AL = (uint8_t)(X86_InterruptFlag ? 1 : 0); X86_InterruptFlag = 0; break;
    case 0x0901: AL = (uint8_t)(X86_InterruptFlag ? 1 : 0); X86_InterruptFlag = 1; break;
    case 0x0902: AL = (uint8_t)(X86_InterruptFlag ? 1 : 0); break;
    case 0x0A00: SET_CF(); break;
    case 0x0E00: case 0x0E01: AX = 0x4D; break;
    default:
        dtu_log("[dpmi] unhandled int 31h AX=%04X BX=%04X CX=%04X DX=%04X", AX, BX, CX, DX);
        SET_CF(); AX = 0x8001;
        break;
    }
}

/* ---- int 16h: BIOS keyboard (no keys in headless mode unless queued) ---- */
int kbd_bios_get(int peek, uint16_t *key);
static void bios_int16(dtu_regs *r)
{
    uint16_t k;
    switch (AH) {
    case 0x00: case 0x10: if (kbd_bios_get(0, &k)) AX = k; else AX = 0; break;
    case 0x01: case 0x11: if (kbd_bios_get(1, &k)) { AX = k; CLR_ZF(); } else SET_ZF(); break;
    case 0x02: case 0x12: AL = 0; break;
    default: dtu_log("[bios] int 16h AH=%02X", AH); break;
    }
}

/* ---- dispatcher, called from asm_int.asm ---- */
/* Watcom _EFG_Format (0x1151E0): formats one double for printf's %e/%f/%g.
   eax = buffer (at least 256 bytes), edx = pointer to the va_list pointer,
   ebx = format specs (+8 precision, -1 = default; +0x14 flags, bit 0 = '#';
   +0x15 conversion character). Returns the string in eax (edx = ds); printf then
   pads it to the field width. The precision is reset to -1, as in the original. */
extern uint32_t dtu_efg_ds;
static void crt_efg_format(dtu_regs *r)
{
    char *buf = PTR(EAX);
    uint32_t *ap = PTR(EDX);
    uint8_t *specs = PTR(EBX);
    int32_t prec;
    double v;
    char conv = (char)specs[0x15], f[16];
    memcpy(&prec, specs + 8, 4);
    memset(specs + 0xC, 0, 4);
    memcpy(&v, PTR(*ap), 8);
    *ap += 8;
    if (prec < 0) prec = 6;
    if (!strchr("eEfgG", conv) || !conv) conv = 'f';
    snprintf(f, sizeof f, "%%%s.%d%c", (specs[0x14] & 1) ? "#" : "", prec > 100 ? 100 : prec, conv);
    snprintf(buf, 256, f, v);
    prec = -1;
    memcpy(specs + 8, &prec, 4);
    EDX = dtu_efg_ds;
}

void X86_InterruptProcedure(uint32_t num, dtu_regs *r)
{
    if (num == 0xEF0000) { crt_efg_format(r); return; }
    if (num == 0xEF0002) { mve_play_guest(r); return; }
    if (num == 0xEF0020) { render_scale_draw(); return; }
    if (num == 0xEF0021) { render_scale_setup(); return; }
    if (num == 0xEF0022) { render_anim_angles(r); return; }
    if (num == 0xEF0023) { render_anim_offset(r); return; }
    if (num == 0xEF0024) { menu_mouse(r); return; }
    if (num == 0xEF0025) { render_floor_point(r); return; }
    if (num == 0xEF0026) { render_floor_note(r); return; }
    if (num == 0xEF0027) { render_floor_draw(r); return; }
    if (num == 0xEF0028) { render_sprite_check(r); return; }
    if (num == 0xEF0029) { render_step_sound(r); return; }
    if (num == 0xEF002A) { render_sound_fallback(r); return; }
    if (num == 0xEF002B) { render_sound_at_pos(r); return; }
    if (num == 0xEF002C) { menu_version(r); return; }
    if ((num & 0xFFFF00) == 0xEF0100) { sos_call((int)(num & 0xFF), r); return; }
    if (num >= 0x1000000) {
        dtu_log("[far] call into runtime-loaded driver at %06X (AX=%04X) - not supported, returning error", num & 0xFFFFFF, AX);
        EAX = 0xFFFFFFFF;
        SET_CF();
        return;
    }
    switch (num) {
    case 0x21: dos_int21(r); break;
    case 0x31: dpmi_int31(r); break;
    case 0x10: video_int10(r); break;
    case 0x2F: dos_int2f(r); break;
    case 0x33: mouse_int33(r); break;
    case 0x16: bios_int16(r); break;
    case 0x1A: { /* BIOS ticks */
        extern unsigned long io_bios_ticks(void);
        unsigned long t = io_bios_ticks(); CX = (uint16_t)(t >> 16); DX = (uint16_t)t; AL = 0; break; }
    case 0x03: {    /* debug asserts in the game, e.g. a frame time of 0 at 0x40EFD: ignored */
        static unsigned long n;
        if (++n <= 10) dtu_log("[int] breakpoint (int 3) at %08X%s", r->eip.e, n == 10 ? " (no further messages)" : "");
        break; }
    case 0x08: case 0x09: case 0x1C: {
        dtu_log("[int] software int %02Xh", num);
        break; }
    default:
        dtu_log("[int] unhandled int %02Xh AX=%04X BX=%04X CX=%04X DX=%04X", num, AX, BX, CX, DX);
        SET_CF();
        break;
    }
}
