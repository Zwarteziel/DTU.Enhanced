/* Undermountain Enhanced - mouse pointer in the text menus
 *
 * The main menu, Load Game and the other text menus use the Descent menu
 * code (newmenu, 0x63FB2). DTU turns mouse motion there into arrow keys
 * (0x66CCF: 16 pixels sideways or 32 up or down = one key) and a button into
 * "choose", without a pointer. Here (a replacement at 0x64AC9, where the
 * menu loop calls 0x66CCF) the menu gets a pointer instead: the item under it
 * becomes the selected one (by arrow keys, so the game redraws and plays its
 * sounds as usual), the left button chooses it (check boxes toggle, sliders
 * follow the pointer while the button is held), the right button is Escape.
 * The pointer is the game's own grey arrow (the one of the Waterdeep map, with
 * a dark grey outline instead of the gold one,
 * sequence 0 of FESEQ.DAT, read from the game folder at start-up; a plain
 * grey arrow drawn here if that file cannot be read), drawn by the
 * front end on top of the picture (video.c), so the menu drawing itself is
 * not disturbed.
 *
 * Menu item (0x5A bytes): 00 type (0 menu, 1 input, 2 check, 3 radio,
 * 4 text, 5 number, 6 input menu, 7 slider), 18 text, 1C x, 1E y, 20 w,
 * 22 h (16 bits, relative to the menu window). Menu loop frame: [esp] number
 * of items, +6C window y, +70 window x; esi = selected item, edi = items. */
#include "dtu.h"
#include "platform.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32_t dtu_menu_mouse;
static int px = 320, py = 240;          /* pointer in 640x480 */
static double active_t = -1;            /* last call of the menu loop */
static int shown;                       /* hidden again after a key press, until the mouse moves */
static int target = -1, click;          /* item to walk to; choose it when there */
static int seen_x, seen_y, seen_init;

static void item_get(uint32_t items, int i, int32_t v[4], int16_t g[4])
{
    const uint8_t *it = PTR(items + (uint32_t)i * 0x5A);
    memcpy(v, it, 16);                  /* type, value, min, max */
    memcpy(g, it + 0x1C, 8);            /* x, y, w, h */
}

/* slider value under the pointer: the bar (left end, max - min + 1 steps, right
   end) is drawn at the right end of the item, about 9.8 pixels per character */
static int slider_value(const uint32_t *f, const int32_t v[4], const int16_t g[4])
{
    int steps = v[3] - v[2] + 1;
    if (steps < 1 || steps > 100) return v[1];
    double cw = 108.0 / 11, x1 = (int)f[0x70 / 4] + g[0] + g[2], x0 = x1 - (steps + 2) * cw;
    int k = (int)((px - (x0 + cw)) / cw);
    if (k < 0) k = 0;
    if (k > steps - 1) k = steps - 1;
    return v[2] + k;
}

static int item_at(const uint32_t *f, uint32_t items, int n)
{
    int wx = (int)f[0x70 / 4], wy = (int)f[0x6C / 4];
    for (int i = 0; i < n; i++) {
        const uint8_t *it = PTR(items + (uint32_t)i * 0x5A);
        uint32_t type; int16_t g[4];
        memcpy(&type, it, 4);
        memcpy(g, it + 0x1C, 8);
        if (type == 4 || g[2] <= 0 || g[3] <= 0) continue;        /* plain text: not selectable */
        int x0 = wx + g[0] - 4, x1 = wx + g[0] + g[2] + 4;
        int y0 = wy + g[1] - 1, y1 = wy + g[1] + g[3] + 1;
        if (px >= x0 && px < x1 && py >= y0 && py < y1) return i;
    }
    return -1;
}

/* replaces the call of 0x66CCF in the menu loop: EAX = key (0 = none), ECX -> "chosen" flag; returns the key */
void menu_mouse(dtu_regs *r)
{
    const uint32_t *f = (const uint32_t *)r + 10;
    int n = (int)f[0], cur = (int)ESI, key = (int)EAX;
    int mx, my; unsigned c[2];
    io_mouse_totals(&mx, &my, c);
    double t = plat_now();
    if (!seen_init || t - active_t > 0.5) {               /* a new menu: forget old motion and clicks */
        seen_init = 1; seen_x = mx; seen_y = my;
        target = -1; click = 0;
    }
    active_t = t;
    int dx = mx - seen_x, dy = my - seen_y;
    seen_x = mx; seen_y = my;
    if (dx || dy) {
        px = px + dx < 0 ? 0 : px + dx > 639 ? 639 : px + dx;
        py = py + dy < 0 ? 0 : py + dy > 479 ? 479 : py + dy;
        shown = 1;
    }
    /* buttons: the game's own press counters (0x46B2AC + 4 * button, counted by its
       mouse event handler 0xE84D0, read and cleared by 0xE8AA8 and for joysticks
       0xDF3DC). Taking the presses from there keeps the menu's "wait until released"
       loop after a choice (0x64AF6) from finding one and waiting for ever. */
    uint32_t *cnt = DATA(0x46B2AC);
    int left = cnt[0] != 0, right = cnt[1] != 0;
    cnt[0] = cnt[1] = cnt[2] = 0;
    (void)c;
    if (key) { shown = 0; target = -1; click = 0; return; }   /* the keyboard goes first */
    if (right) { EAX = 0x01; active_t = -1; return; }          /* Escape (the menu may close: no pointer until it runs again) */
    if (n <= 0 || n > 100) return;
    int hit = shown ? item_at(f, EDI, n) : -1;
    int32_t v[4]; int16_t g[4];
    if (hit >= 0) item_get(EDI, hit, v, g);
    int held = *(uint8_t *)DATA(0x46B248);                      /* left button down (game's mouse handler) */
    if (hit >= 0 && v[0] == 7 && (left || held) && hit == cur) {
        /* slider: walk the value to the pointer with the left/right keys */
        int want = slider_value(f, v, g);
        target = -1; click = 0;
        if (want != v[1]) EAX = want < v[1] ? 0xCB : 0xCD;
        return;
    }
    if ((dx || dy) && hit >= 0) target = hit;
    if (left && hit >= 0) { target = hit; click = v[0] != 7; }   /* a slider is not "chosen" (that would close the menu) */
    if (target >= 0 && target != cur && cur >= 0) {
        EAX = target < cur ? 0xC8 : 0xD0;                       /* up / down */
        return;
    }
    if (target == cur && click) {
        uint32_t one = 1;
        memcpy(PTR(ECX), &one, 4);                               /* choose, as a mouse button does in 0x66CCF */
        click = 0;
        active_t = -1;                                           /* the menu may close (a check box keeps it open) */
    }
    if (target == cur) target = -1;
}

/* The front end cursors are IFF ANIM files (Deluxe Paint PBM: BMHD, CMAP,
   BODY, uncompressed or ByteRun1) in FESEQ.DAT, with offsets and sizes in
   FESEQ.JRG (count, then offset/size pairs). Sequence 0 is the grey arrow of
   the Waterdeep map: 20x12, colour 255 transparent, tip at the top left. */
#define ARROW_MAX 64
#define ARROW_LIGHT 40                  /* percent */
static uint32_t *arrow;                 /* malloc: the Windows build has little room for static data below 0xA0000 */
static int arrow_w, arrow_h;

static FILE *open_game_file(const char *name)
{
    char path[1200], low[64];
    size_t i;
    for (i = 0; name[i] && i < sizeof low - 1; i++) low[i] = (char)(name[i] >= 'A' && name[i] <= 'Z' ? name[i] + 32 : name[i]);
    low[i] = 0;
    snprintf(path, sizeof path, "%s/%s", dtu_game_dir, name);
    FILE *f = fopen(path, "rb");
    if (!f) { snprintf(path, sizeof path, "%s/%s", dtu_game_dir, low); f = fopen(path, "rb"); }
    return f;
}

static uint32_t be32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }

/* chunk id inside [p, end) of an IFF FORM body, descending into FORMs */
static const uint8_t *iff_find(const uint8_t *p, const uint8_t *end, const char *id, uint32_t *len)
{
    while (p + 8 <= end) {
        uint32_t n = be32(p + 4);
        if (n > (uint32_t)(end - p - 8)) return NULL;
        if (!memcmp(p, id, 4)) { *len = n; return p + 8; }
        if (!memcmp(p, "FORM", 4) && n >= 4) {
            const uint8_t *q = iff_find(p + 12, p + 8 + n, id, len);
            if (q) return q;
        }
        p += 8 + n + (n & 1);
    }
    return NULL;
}

static int load_arrow(void)
{
    FILE *j = open_game_file("FESEQ.JRG"), *d = open_game_file("FESEQ.DAT");
    uint8_t idx[12], *b = NULL;
    int ok = 0;
    if (!j || !d || fread(idx, 1, 12, j) != 12) goto out;
    uint32_t off = idx[4] | idx[5] << 8 | idx[6] << 16 | (uint32_t)idx[7] << 24;
    uint32_t size = idx[8] | idx[9] << 8 | idx[10] << 16 | (uint32_t)idx[11] << 24;
    if (size < 64 || size > 65536 || !(b = malloc(size)) || fseek(d, (long)off, SEEK_SET) || fread(b, 1, size, d) != size) goto out;
    uint32_t lh, lc, lb;
    const uint8_t *bmhd = iff_find(b, b + size, "BMHD", &lh), *cmap = iff_find(b, b + size, "CMAP", &lc), *body = iff_find(b, b + size, "BODY", &lb);
    if (!bmhd || lh < 20 || !cmap || lc < 768 || !body) goto out;
    int w = bmhd[0] << 8 | bmhd[1], h = bmhd[2] << 8 | bmhd[3], comp = bmhd[10];
    int tr = bmhd[12] << 8 | bmhd[13];
    if (w <= 0 || h <= 0 || w > ARROW_MAX || h > ARROW_MAX) goto out;
    uint8_t px[ARROW_MAX * ARROW_MAX];
    int n = 0;
    if (!comp) { if (lb < (uint32_t)(w * h)) goto out; memcpy(px, body, (size_t)(w * h)); n = w * h; }
    else for (uint32_t i = 0; i < lb && n < w * h;) {                  /* ByteRun1 */
        int c = body[i++];
        if (c < 128) { for (int k = 0; k <= c && i < lb && n < w * h; k++) px[n++] = body[i++]; }
        else if (c > 128 && i < lb) { for (int k = 0; k < 257 - c && n < w * h; k++) px[n++] = body[i]; i++; }
    }
    if (n < w * h || !(arrow = malloc(sizeof *arrow * (size_t)(w * h)))) goto out;
    /* The arrow is made ARROW_LIGHT % lighter (it is fairly dark on the menu
       scrolls). The outline (the colour of the tip, top left) is gold in this
       palette; it becomes a grey a little darker than the arrow itself: 70%
       of the average of the other (lightened) colours. */
    int edge = px[0], sum[3] = { 0, 0, 0 }, cnt = 0;
    uint8_t rgb[256][3];
    for (int i = 0; i < 256; i++)
        for (int c = 0; c < 3; c++) { int v = cmap[3 * i + c] * (100 + ARROW_LIGHT) / 100; rgb[i][c] = (uint8_t)(v > 255 ? 255 : v); }
    for (int i = 0; i < w * h; i++)
        if (px[i] != tr && px[i] != edge) { for (int c = 0; c < 3; c++) sum[c] += rgb[px[i]][c]; cnt++; }
    if (edge != tr && cnt) for (int c = 0; c < 3; c++) rgb[edge][c] = (uint8_t)(sum[c] * 7 / (10 * cnt));
    for (int i = 0; i < w * h; i++)
        arrow[i] = px[i] == tr ? 0 : 0xFF000000u | (uint32_t)rgb[px[i]][2] << 16 | (uint32_t)rgb[px[i]][1] << 8 | rgb[px[i]][0];
    arrow_w = w; arrow_h = h;
    ok = 1;
out:
    free(b);
    if (j) fclose(j);
    if (d) fclose(d);
    return ok;
}

/* a plain arrow in the same greys, for when FESEQ.DAT cannot be read: the
   usual pointer shape, tip at the top left, outline around a lighter body */
static void plain_arrow(void)
{
    static const char *shape[] = {
        "X           ", "XX          ", "X#X         ", "X##X        ", "X###X       ", "X####X      ",
        "X#####X     ", "X######X    ", "X#######X   ", "X########X  ", "X#####XXXXX ", "X##X##X     ",
        "X#X X##X    ", "XX  X##X    ", "X    X##X   ", "     X##X   ", "      XX    " };
    static uint32_t img[12 * 17];
    for (int y = 0; y < 17; y++)
        for (int x = 0; x < 12; x++) {
            char c = shape[y][x];
            img[y * 12 + x] = c == '#' ? 0xFF816D62u : c == 'X' ? 0xFF5A4C45u : 0;     /* RGB 98,109,129 and 69,76,90 */
        }
    arrow = img; arrow_w = 12; arrow_h = 17;
}

/* at start-up (not from a hook of the game: file functions may set errno) */
void menu_init(void)
{
    if (load_arrow()) dtu_log("[menu] pointer: the grey arrow from FESEQ.DAT (%dx%d)", arrow_w, arrow_h);
    else { plain_arrow(); dtu_log("[menu] pointer: FESEQ.DAT not readable, using a plain arrow"); }
}

/* for the front end: where to draw the pointer (0 = not now) */
int menu_pointer(int *x, int *y, const uint32_t **img, int *w, int *h)
{
    if (!dtu_menu_mouse || !shown || active_t < 0 || plat_now() - active_t > 0.25) return 0;
    if (!arrow_w) return 0;
    *x = px; *y = py; *img = arrow; *w = arrow_w; *h = arrow_h;     /* tip at the top left */
    return 1;
}

/* The main menu (0x58118) writes the copyright, two lines from Text_string[964]
   (DTUE.TEX), centred at the bottom of the screen: y = height - (font height +
   15), in colour 2, through 0x80620. Below it there is no room for another line,
   so the copyright moves up one line and "Undermountain Enhanced <version>" goes
   under it, in the same font and colour. Called just before that print with EDX
   = its y; sets EDX and dtu_menu_ver_str / dtu_menu_ver_y for the second print. */
#include "version.h"
extern uint32_t dtu_menu_ver_str, dtu_menu_ver_y;
void menu_version(dtu_regs *r)
{
    static char text[] = "Undermountain Enhanced " DTU_VERSION;
    uint32_t font = *(uint32_t *)DATA(0x13E384);
    int h = font ? *(int16_t *)PTR(font + 2) : 8;
    if (h <= 0 || h > 40) h = 8;
    int pitch = h + 1;
    EDX = (uint32_t)((int)EDX - pitch);
    dtu_menu_ver_y = (uint32_t)((int)EDX + 2 * pitch);
    dtu_menu_ver_str = (uint32_t)(uintptr_t)text;
    if (dtu_verbose >= 2) dtu_log("[menu] version line at y %d (font height %d)", (int)dtu_menu_ver_y, h);
}
