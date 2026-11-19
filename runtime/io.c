/* Undermountain Enhanced - I/O ports, PIT timer, keyboard, mouse, headless driver */
#define _GNU_SOURCE
#include "dtu.h"
#include <stdlib.h>
#include <string.h>
#include "platform.h"

double io_run_seconds = 20.0, io_shot_every = 2.0;
volatile int io_quit_requested;

static double now(void) { return plat_now(); }

/* ---------------- PIT ---------------- */
#define PIT_CLOCK 1193182.0
static uint32_t pit_reload[3] = { 65536, 65536, 65536 };
static int pit_access[3] = { 3, 3, 3 }, pit_wflip[3], pit_rflip[3];
static uint16_t pit_latch[3]; static int pit_latched[3];
static double pit_epoch;          /* time the channel 0 count was (re)loaded */

uint32_t pit_hz(void) { return (uint32_t)(PIT_CLOCK / pit_reload[0]); }

/* PIT channel 0 and DTU's clock.
   The raw count runs freely on wall-clock time, as on real hardware (the
   joystick routine at 0x101B10 times out on it with interrupts disabled).
   DTU's clock (0xDFA7A) latches the count, reads the PIC's IRR and computes
       ticks * reload + (reload - count)      (+ reload if IRR bit 0 is set)
   with ticks counted by its IRQ0 handler (0x149C3B). Our interrupts reach
   the game with a small delay, so plain wall-clock values make that clock
   jump back by a period now and then. When IRR is read right after a latch
   (that pattern), the latched count and the IRR bit are chosen so that the
   result follows wall-clock time, fits the current tick count and never
   decreases. */
static int pic_read_irr, pit0_latch_fresh;
static unsigned long frame_count;
double io_frame_shots[2];         /* headless tests: screenshots at the frame hook between these times */
static int in_frame_hook;           /* dtu_run_timer called from the frame hook */
static double last_frame_hook;      /* time of the last frame hook (main game loop running) */
static int main_loop_running(void);
static double pit0_frame_time;      /* > 0: time for the frame-time reading that follows the frame hook */
static uint32_t pit0_last_ticks;
static int64_t pit0_last;
static int pit0_valid;

static uint16_t pit_count(int ch)
{
    uint32_t r = pit_reload[ch];
    double ticks = (now() - (ch == 0 ? pit_epoch : 0)) * PIT_CLOCK;
    return (uint16_t)(r - (uint32_t)((uint64_t)ticks % r));
}

/* DTU clock pattern: replace the latched count, return the IRR bit.
   The phase is measured from the moment the last delivered tick was due, so
   ticks * reload + phase is wall-clock time as long as interrupts are at
   most one period late (an earlier version used a free offset that drifted
   by up to a period: frame times then alternated, e.g. 6.7/22.6 ms at
   70 fps, and the game's speed limit per frame (0x540F0) halved walking). */
static int pit0_clock_read(void)
{
    int64_t r = pit_reload[0];
    uint32_t ticks = *(volatile uint32_t *)DATA(0x149C3B);
    int64_t base = (int64_t)ticks * r;
    double hz = PIT_CLOCK / r;
    double due = pit_epoch + ((double)dtu_timer_run - 1) / hz;     /* tick n is due at epoch + (n-1)/hz */
    double t = now();
    if (pit0_frame_time > 0) {      /* the game measures its frame time: use the limiter's frame start */
        if (pit0_frame_time < t) t = pit0_frame_time;
        pit0_frame_time = 0;
    }
    int64_t e = (int64_t)((t - due) * PIT_CLOCK);
    if (!pit0_valid || ticks < pit0_last_ticks) { pit0_last = 0; pit0_valid = 1; }   /* first use or counter reset */
    pit0_last_ticks = ticks;
    if (e < 1) e = 1;
    if (e > 2 * r - 1) {                               /* more than one period late: time stands still */
        static int nlog;
        if (nlog++ < 20) dtu_log("[clock] timer interrupts %.1f ms late (IF %u, ticks %u, run %u, target %u)", (e - r) * 1000.0 / PIT_CLOCK, X86_InterruptFlag, ticks, dtu_timer_run, dtu_timer_tick);
        e = 2 * r - 1;
    }
    int64_t v = base + e;
    if (v <= pit0_last) {                              /* never backwards */
        if (pit0_last + 1 - base <= 2 * r - 1) v = pit0_last + 1;
        else v = pit0_last;
        e = v - base;
        if (e < 1) e = 1;
    }
    pit0_last = base + e;
    int irr = e >= r;
    pit_latch[0] = (uint16_t)(irr ? 2 * r - e : r - e);
    return irr;
}

static void pit_write(int ch, uint8_t v)
{
    static uint16_t lo[3];
    int acc = pit_access[ch];
    if (acc == 1) pit_reload[ch] = v ? v : 65536;
    else if (acc == 2) pit_reload[ch] = v ? (uint32_t)v << 8 : 65536;
    else {
        if (!pit_wflip[ch]) { lo[ch] = v; pit_wflip[ch] = 1; return; }
        pit_wflip[ch] = 0;
        uint32_t n = lo[ch] | ((uint32_t)v << 8);
        pit_reload[ch] = n ? n : 65536;
    }
    if (ch == 0) {
        pit_epoch = now();
        dtu_log("[pit] channel 0 reload %u -> %.2f Hz", pit_reload[0], PIT_CLOCK / pit_reload[0]);
    }
}

static uint8_t pit_read(int ch)
{
    if (ch == 0) pit0_latch_fresh = 0;
    uint16_t c = pit_latched[ch] ? pit_latch[ch] : pit_count(ch);
    int acc = pit_access[ch];
    if (acc == 1) { pit_latched[ch] = 0; return (uint8_t)c; }
    if (acc == 2) { pit_latched[ch] = 0; return (uint8_t)(c >> 8); }
    if (!pit_rflip[ch]) { pit_rflip[ch] = 1; return (uint8_t)c; }
    pit_rflip[ch] = 0; pit_latched[ch] = 0;
    return (uint8_t)(c >> 8);
}

static void pit_control(uint8_t v)
{
    int ch = v >> 6;
    if (ch == 3) return;
    if (((v >> 4) & 3) == 0) { pit_latch[ch] = pit_count(ch); pit_latched[ch] = 1; pit_rflip[ch] = 0; if (ch == 0) pit0_latch_fresh = 1; return; }
    pit_access[ch] = (v >> 4) & 3;
    pit_wflip[ch] = pit_rflip[ch] = 0;
}

/* ---------------- keyboard ---------------- */
/* scancode bytes (XT set 1, E0 prefixes included) from the front end or a
   test script; one byte is handed to the game's IRQ1 handler per timer tick */
#define KQ 256
static volatile uint8_t kq[KQ];
static volatile unsigned kq_head, kq_tail;     /* single producer, single consumer */
static uint8_t kbd_data, kbd_pending;
static uint16_t bios_buf[32]; static int bios_head, bios_tail;

#define NKEYS 512
static struct { double t; int sc; int done; } keys[NKEYS];
static struct { double t; int x, y, b, done; } mice[NKEYS];
static int nmice;
void io_add_mouse(double t, int x, int y, int b)
{
    if (nmice >= NKEYS) return;
    mice[nmice].t = t; mice[nmice].x = x; mice[nmice].y = y; mice[nmice].b = b; nmice++;
}
static int nkeys;

void io_key_byte(uint8_t b)
{
    unsigned t = kq_tail;
    if (((t + 1) & (KQ - 1)) == (kq_head & (KQ - 1))) return;   /* full: drop */
    kq[t & (KQ - 1)] = b;
    __atomic_store_n(&kq_tail, t + 1, __ATOMIC_RELEASE);
}

/* sc: XT scan code; 0x1xx = with E0 prefix (arrows etc.); held for dur seconds */
void io_add_key(double t, int sc, double dur)
{
    if (nkeys + 2 > NKEYS) return;
    keys[nkeys].t = t; keys[nkeys].sc = sc; nkeys++;
    /* typematic repeat as on Windows: after 0.5 s, about 30 per second */
    for (double r = t + 0.5; r < t + dur && nkeys + 2 < NKEYS; r += 1.0 / 30) { keys[nkeys].t = r; keys[nkeys].sc = sc; nkeys++; }
    keys[nkeys].t = t + dur; keys[nkeys].sc = sc | 0x80; nkeys++;
}

static const char sc_ascii[128] =
    "\0\x1b" "1234567890-=\b\tqwertyuiop[]\r\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 ";

int kbd_bios_get(int peek, uint16_t *key)
{
    if (bios_head == bios_tail) return 0;
    *key = bios_buf[bios_head];
    if (!peek) bios_head = (bios_head + 1) & 31;
    return 1;
}

/* ---------------- mouse (int 33h) ----------------
   Works like a DOS mouse driver: the front end captures the pointer and
   reports relative motion in 640x480 game pixels. DTU reads that motion
   (function 0Bh, 1 mickey = 1 pixel) to move its own cursor or to turn the
   view (immersive mode, key V), and receives button presses through an
   event handler (function 0Ch). The driver also keeps an absolute position
   (functions 03h/04h) within the range set by 07h/08h. */
static volatile int mouse_buttons, mouse_mx, mouse_my;
static int mouse_x = 320, mouse_y = 240;
static int mouse_minx = 0, mouse_maxx = 639, mouse_miny = 0, mouse_maxy = 479;
static uint32_t mouse_handler, mouse_mask;
static unsigned mouse_presses[3], mouse_releases[3];
static int mouse_px[3][2], mouse_rel_pos[3][2];

#define MQ 64
static struct { uint16_t mask, buttons; } mq[MQ];
static volatile unsigned mq_head, mq_tail;

static int clampi(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }
static volatile unsigned mouse_clicks[2];      /* presses of the left and right button, ever */

/* for the menu pointer (menu.c): all motion so far and the number of presses */
void io_mouse_totals(int *mx, int *my, unsigned clicks[2])
{
    *mx = __atomic_load_n(&mouse_mx, __ATOMIC_RELAXED);
    *my = __atomic_load_n(&mouse_my, __ATOMIC_RELAXED);
    clicks[0] = __atomic_load_n(&mouse_clicks[0], __ATOMIC_RELAXED);
    clicks[1] = __atomic_load_n(&mouse_clicks[1], __ATOMIC_RELAXED);
}

/* from the front end (or the headless script): relative motion in game
   pixels and button bits (1 left, 2 right, 4 middle) */
void io_mouse(int dx, int dy, int buttons)
{
    int old = mouse_buttons;
    __atomic_add_fetch(&mouse_mx, dx, __ATOMIC_RELAXED);
    __atomic_add_fetch(&mouse_my, dy, __ATOMIC_RELAXED);
    mouse_buttons = buttons;
    if (buttons != old) {
        uint16_t m = 0;
        for (int b = 0; b < 3; b++) {
            int bit = 1 << b;
            if ((buttons & bit) && !(old & bit)) { m |= 2 << (2 * b); if (b < 2) __atomic_add_fetch(&mouse_clicks[b], 1, __ATOMIC_RELAXED); }   /* pressed */
            if (!(buttons & bit) && (old & bit)) m |= 4 << (2 * b);       /* released */
        }
        unsigned t = mq_tail;
        if (((t + 1) & (MQ - 1)) != (mq_head & (MQ - 1))) {
            mq[t & (MQ - 1)].mask = m; mq[t & (MQ - 1)].buttons = (uint16_t)buttons;
            __atomic_store_n(&mq_tail, t + 1, __ATOMIC_RELEASE);
        }
    }
}

/* game thread: apply motion to the driver position */
static void mouse_move(void)
{
    int dx = __atomic_load_n(&mouse_mx, __ATOMIC_RELAXED), dy = __atomic_load_n(&mouse_my, __ATOMIC_RELAXED);
    static int seen_x, seen_y;
    mouse_x = clampi(mouse_x + dx - seen_x, mouse_minx, mouse_maxx);
    mouse_y = clampi(mouse_y + dy - seen_y, mouse_miny, mouse_maxy);
    seen_x = dx; seen_y = dy;
}

/* game thread: count presses/releases and call the game's event handler */
static void mouse_deliver(dtu_stack *s)
{
    mouse_move();
    while (mq_head != __atomic_load_n(&mq_tail, __ATOMIC_ACQUIRE)) {
        unsigned i = mq_head & (MQ - 1);
        uint16_t m = mq[i].mask, bt = mq[i].buttons;
        int x = mouse_x, y = mouse_y;
        mq_head++;
        for (int b = 0; b < 3; b++) {
            if (m & (2 << (2 * b))) { mouse_presses[b]++; mouse_px[b][0] = x; mouse_px[b][1] = y; }
            if (m & (4 << (2 * b))) { mouse_releases[b]++; mouse_rel_pos[b][0] = x; mouse_rel_pos[b][1] = y; }
        }
        if (mouse_handler && (m & mouse_mask)) {
            static int logged;
            if (!logged++) dtu_log("[mouse] first event %04X -> handler %08X", m & mouse_mask, mouse_handler);
            X86_InterruptFlag = 0;
            dtu_c_call_mouse(s, mouse_handler, (uint32_t)(m & mouse_mask) | ((uint32_t)bt << 16),
                             (uint32_t)x | ((uint32_t)y << 16));
            X86_InterruptFlag = 1;
        }
    }
}

void mouse_int33(dtu_regs *r)
{
    static unsigned char seen[64];
    if (AX < 64 && !seen[AX]) { seen[AX] = 1; dtu_log("[mouse] first int 33h AX=%04X BX=%04X CX=%04X DX=%04X", AX, BX, CX, DX); }
    static int mick_x, mick_y;           /* motion already reported through 0Bh */
    int b, fn = AX;
    mouse_move();
    int x = mouse_x, y = mouse_y;
    switch (fn) {
    case 0x00: case 0x21:
        AX = 0xFFFF; BX = 3;
        if (fn == 0) { mouse_handler = 0; mouse_mask = 0; }
        break;
    case 0x01: case 0x02: break;            /* the game draws its own cursor */
    case 0x03: BX = (uint16_t)mouse_buttons; CX = (uint16_t)x; DX = (uint16_t)y; break;
    case 0x04:
        mouse_x = clampi((int16_t)CX, mouse_minx, mouse_maxx); mouse_y = clampi((int16_t)DX, mouse_miny, mouse_maxy);
        break;
    case 0x05: case 0x06:
        b = BX < 3 ? BX : 0;
        AX = (uint16_t)mouse_buttons;
        if (fn == 5) { BX = (uint16_t)mouse_presses[b]; mouse_presses[b] = 0; CX = (uint16_t)mouse_px[b][0]; DX = (uint16_t)mouse_px[b][1]; }
        else { BX = (uint16_t)mouse_releases[b]; mouse_releases[b] = 0; CX = (uint16_t)mouse_rel_pos[b][0]; DX = (uint16_t)mouse_rel_pos[b][1]; }
        break;
    case 0x07: mouse_minx = (int16_t)CX; mouse_maxx = (int16_t)DX; mouse_x = clampi(mouse_x, mouse_minx, mouse_maxx); break;
    case 0x08: mouse_miny = (int16_t)CX; mouse_maxy = (int16_t)DX; mouse_y = clampi(mouse_y, mouse_miny, mouse_maxy); break;
    case 0x0B: {                            /* motion since the last call, in mickeys (= pixels) */
        io_idle_poll();
        int mx = __atomic_load_n(&mouse_mx, __ATOMIC_RELAXED), my = __atomic_load_n(&mouse_my, __ATOMIC_RELAXED);
        int dy = my - mick_y;
        CX = (uint16_t)(mx - mick_x);
        mick_x = mx; mick_y = my;
        if (io_mouse_vertical != 1.0f && main_loop_running()) {
            /* in the game (looking up and down) the vertical motion can be scaled */
            static float frac;
            float v = dy * io_mouse_vertical + frac;
            dy = (int)v;
            frac = v - dy;
        }
        DX = (uint16_t)dy;
        break;
    }
    case 0x0C:
        mouse_handler = EDX; mouse_mask = CX;
        dtu_log("[mouse] event handler %08X mask %04X", EDX, CX);
        break;
    case 0x0F: case 0x1A: case 0x1D: break;
    case 0x20: AX = 0xFFFF; break;          /* enable driver: success */
    case 0x24: BX = 0x0700; CX = 0x0400; break;
    default: dtu_log("[mouse] int 33h AX=%04X", AX); break;
    }
}

/* ---------------- port dispatch (called from asm_inout.asm) ---------------- */
uint32_t X86_InPortProcedure(uint16_t port, uint32_t size)
{
    static int logged[1024];
    switch (port) {
    case 0x40: case 0x41: case 0x42: return pit_read(port - 0x40);
    case 0x60: kbd_pending = 0; return kbd_data;
    case 0x61: return 0x20;
    case 0x64: return kbd_pending ? 0x1D : 0x1C;
    case 0x20:
        if (!pic_read_irr) return 0;
        if (pit0_latch_fresh && pit_latched[0] && pit_reload[0] < 65536) { pit0_latch_fresh = 0; return (uint8_t)pit0_clock_read(); }
        return dtu_timer_tick != dtu_timer_run;        /* IRR bit 0: timer interrupt pending */
    case 0xA0: return 0;
    case 0x21: case 0xA1: return 0;
    case 0x201: return 0xFF;          /* no joystick */
    default:
        if (port >= 0x3B0 && port <= 0x3DF) return video_port_in(port);
        if (port < 1024 && !logged[port]++) dtu_log("[port] in %03Xh (%u bytes) -> FF", port, size);
        return size == 1 ? 0xFF : size == 2 ? 0xFFFF : 0xFFFFFFFF;
    }
}

void X86_OutPortProcedure(uint16_t port, uint32_t size, uint32_t val)
{
    static int logged[1024];
    switch (port) {
    case 0x40: case 0x41: case 0x42: pit_write(port - 0x40, (uint8_t)val); return;
    case 0x43: pit_control((uint8_t)val); return;
    case 0x20:
        if ((val & 0x18) == 0x08 && (val & 2)) pic_read_irr = !(val & 1);   /* OCW3: read IRR (0Ah) or ISR (0Bh) */
        return;
    case 0xA0: case 0x21: case 0xA1: case 0x61: case 0x80: return;
    case 0x201: return;
    default:
        if (port >= 0x3B0 && port <= 0x3DF) {
            video_port_out(port, (uint8_t)val);
            if (size == 2) video_port_out(port + 1, (uint8_t)(val >> 8));
            return;
        }
        if (port < 1024 && !logged[port]++) dtu_log("[port] out %03Xh <- %X (%u bytes)", port, val, size);
    }
}

/* ---------------- timer thread and IRQs ---------------- */
volatile int io_stop;

static void timer_thread(void *arg)
{
    (void)arg;
    while (!io_stop) {
        double t = now();
        double hz = PIT_CLOCK / pit_reload[0];
        uint32_t want = (uint32_t)((t - pit_epoch) * hz) + 1;
        dtu_timer_tick = want ? want : 1;
        plat_sleep_ms(1);
    }
}

/* ---------------- frame limiter and fps ----------------
   SR_FrameHook calls this once per game frame, just before the game measures
   its frame time (0x40E91). DTU limits that frame time to 1/150 s .. 1/5 s,
   so above 150 fps the game runs faster than real time. */
volatile int io_fps_limit = 60;
/* DTU damps the player's velocity by 1/3 per frame when there is no forward
   input (0x2C02E), e.g. while strafing. That makes strafing slower at higher
   frame rates. With io_physics_fps > 0 the damping is scaled with the frame
   time so that it behaves as at that frame rate (0 = original, 1/3 per frame):
   steady state v = a*dt/(1-f) equals the one at dt0 = 1/io_physics_fps when
   f = 1 - dt/(1.5*dt0). */
volatile int io_physics_fps = 20;
volatile int io_ladder_fix = 1;     /* see dtu_frame_hook */
extern int32_t dtu_friction, dtu_phys_scale;
volatile float io_fps;

static double work_sum, work_max, hook_exit;   /* game work per frame (excluding the limiter wait) */
extern uint32_t dtu_c_call_proc3(dtu_stack *s, uint32_t proc, uint32_t eax_, uint32_t edx_, uint32_t ebx_);
void dtu_frame_hook(dtu_stack *s)
{
    static double deadline, fps_t0;
    {   /* test: DTU_CALLTEST=hostaddr:frame:eax:edx:ebx calls a game function once (headless experiments) */
        static int done = -1;
        static const char *e;
        if (done < 0) { e = getenv("DTU_CALLTEST"); done = !e; }
        if (done) e = NULL;
        unsigned a, f, x, y, z;
        if (e && sscanf(e, "%x:%u:%x:%x:%x", &a, &f, &x, &y, &z) == 5 && frame_count >= f) {
            done = 1;
            uint32_t ret = dtu_c_call_proc3(s, a, x, y, z);
            dtu_log("[test] called %08X(%X, %X, %X) -> %X", a, x, y, z, ret);
        }
    }
    if (io_frame_shots[1] > 0 && now() >= io_frame_shots[0] && now() < io_frame_shots[1]) dtu_screenshot("frame");   /* complete frames (tests) */
    {   /* test: DTU_STALLTEST=frame:ms:every[:count] keeps the game thread busy (as a slow disk would) */
        static const char *e; static int init;
        static unsigned stalls;
        unsigned f, ms, every, count = 0;
        if (!init) { init = 1; e = getenv("DTU_STALLTEST"); }
        if (e && sscanf(e, "%u:%u:%u:%u", &f, &ms, &every, &count) >= 3 && frame_count >= f && every && (frame_count - f) % every == 0 && (!count || stalls < count)) {
            stalls++;
            dtu_log("[test] game thread busy for %u ms at frame %lu", ms, frame_count);
            plat_sleep_ms((int)ms);
        }
    }
    {
        double w = now() - hook_exit;
        if (hook_exit > 0 && w < 1.0) { work_sum += w; if (w > work_max) work_max = w; }
    }
    static unsigned long fps_n0;
    double t = now();
    int lim = io_fps_limit;
    if (lim > 0) {
        double period = 1.0 / lim;
        deadline += period;
        if (deadline < t - period || deadline > t + period) deadline = t;   /* fell behind or limit changed */
        double left = deadline - t;
        if (left > 0.002) plat_sleep_ms((int)((left - 0.0015) * 1000));
        while (now() < deadline) {}
        t = now();
    }
    double frame_start = lim > 0 ? deadline : t;
    frame_count++;
    {
        double f = 1.0 / 3;
        int32_t gdt = *(volatile int32_t *)DATA(0x13CD2C);       /* last frame time, 1/65536 s */
        if (io_physics_fps > 0 && gdt > 0) {
            f = 1.0 - (gdt / 65536.0) * io_physics_fps / 1.5;
            if (f < 1.0 / 3) f = 1.0 / 3;
            if (f > 0.98) f = 0.98;
        }
        dtu_friction = (int32_t)(f * 65536 + 0.5);
        /* the physics (gravity, drag) integrate at most 2/64 s per frame
           (0x9DD4F), so at DOS frame rates (about 20 fps) a jump or the hop
           at the top of a ladder goes higher than at 60 fps, where the hop
           is often too low to get over the edge. With io_physics_fps > 0 the
           integration time is scaled so that it behaves as at that frame rate. */
        dtu_phys_scale = io_physics_fps > 0 && io_physics_fps < 32 ? io_physics_fps * 2048 : 65536;
    }
    if (io_ladder_fix) {
        /* At the top of a ladder the game ends the climb with a small hop over
           the edge. If the hop is too low the player stays hanging in the air
           at the edge: "falling" (flag 0x20) but not moving at all, and walking
           does nothing (in DOS as well, now and then). Then give the same hop
           again, at most 3 times; walking backwards still drops you down. */
        static int32_t last[3]; static double since; static int hops;
        uint32_t po = *(volatile uint32_t *)DATA(0x30DF14);
        if (po) {
            uint8_t *b = (uint8_t *)PTR(po);
            int32_t *pos = (int32_t *)(b + 0x14), *vy = (int32_t *)(b + 0x100);
            int same = pos[0] == last[0] && pos[1] == last[1] && pos[2] == last[2];
            if ((b[0x1ae] & 0xA0) != 0x20 || !same) { since = t; if (!same) hops = 0; }
            else if (*vy < -0x80000 && t - since > 0.15 && hops < 3) {
                *vy = 0x208000;
                hops++; since = t;
                dtu_log("[ladder] hanging at the edge: hop %d", hops);
            }
            memcpy(last, pos, sizeof last);
        }
    }
    {   /* frame time statistics: the game's frame time (1/65536 s) of the last frame */
        static double stat_t; static unsigned long n; static double sum; static int32_t mx, mn = 0x7fffffff; static unsigned long bad;
        int32_t raw = *(volatile int32_t *)DATA(0x13CD2C), used = *(volatile int32_t *)DATA(0x1C7FD0);
        (void)used;
        n++; sum += raw; if (raw > mx) mx = raw; if (raw < mn) mn = raw;
        if (raw <= 0 || raw > 0x10000) bad++;
        if (t >= stat_t) {
            if (stat_t > 0 && n) dtu_log("[frame] %lu frames, frame time min %.1f avg %.1f max %.1f ms, %lu out of range; game work avg %.2f max %.1f ms", n, mn * 1000.0 / 65536, sum / n * 1000.0 / 65536, mx * 1000.0 / 65536, bad, work_sum / n * 1000, work_max * 1000);
            stat_t = t + 5; n = 0; sum = 0; mx = -0x7fffffff; mn = 0x7fffffff; bad = 0; work_sum = 0; work_max = 0;
        }
    }
    if (t - fps_t0 >= 0.5) {
        io_fps = (float)((frame_count - fps_n0) / (t - fps_t0));
        fps_t0 = t; fps_n0 = frame_count;
    }
    last_frame_hook = now();
    in_frame_hook = 1;
    dtu_run_timer(s);
    in_frame_hook = 0;
    /* delivering the ticks runs game code (its timer callbacks) for a varying
       time; the frame time the game measures next should not include that,
       because it caps the walking speed per frame at the frame time (0x540F0) */
    pit0_frame_time = frame_start;
    hook_exit = now();
}

unsigned long io_frame_count(void) { return frame_count; }

unsigned long io_bios_ticks(void) { return (unsigned long)(now() * 18.2065); }

void io_init(void) { now(); }

void timer_start(void)
{
    pit_epoch = now();
    dtu_timer_run = dtu_timer_tick = 0;
    plat_thread(timer_thread, NULL, 64 * 1024);
}

uint32_t dtu_irq_handler(int irq);

static uint32_t fb_hash(void)
{
    uint32_t h = 2166136261u;
    video_sync_window();
    for (size_t i = 0; i < (size_t)vga_width * vga_height && i < (1u << 20); i += 7) h = (h ^ vga_fb[i]) * 16777619u;
    for (int i = 0; i < 768; i++) h = (h ^ vga_pal[i]) * 16777619u;
    return h;
}

/* ---------------- menu key filter ----------------
   Automap, inventory, statistics, rest etc. are modes of the main game loop:
   every frame the loop checks whether the key of such a function is down
   and switches if it is (e.g. automap 0x53DAE -> 0x408F5). A key that is down
   during several frames switches back and forth (typematic repeats add to
   that). That happened in DOS too, but shows more at high frame rates.
   So, per function (the game's live binding table at 0x13C1A5, 22 bytes per
   slot, also after changing keys with F2), while the main loop runs:
   - the press is handed to the game at the start of a frame (frame hook)
     and released at the start of the next one: the game sees it in exactly
     one frame and switches once;
   - repeats are dropped, and a new press of the same function within
     io_menu_key_delay ms is dropped (with its release).
   Movement, turning, looking, strafing and modifier functions, keys bound to
   no function, and everything outside the main loop (front-end menus,
   character generation, typing names) are passed on unchanged. */
volatile int io_menu_key_delay = 300;
#define BIND_SLOTS 64
static int slot_is_movement(int slot)
{
    /* 4-7 turn, 8-9 Alt, 10-13 strafe, 14-17 PgUp/PgDn, 18-21 Ctrl/...,
       24-27 forward/back, 28-37 up/down/look, 41-42 Shift */
    return (slot >= 4 && slot <= 21) || (slot >= 24 && slot <= 37) || slot == 41 || slot == 42;
}

static double auto_release_at[256];       /* > 0: release this key (fallback time) */
static unsigned long auto_release_frame[256];   /* release at the frame hook of this frame */
static unsigned char auto_released[256];  /* the game already got a release: drop the real one */

static int main_loop_running(void) { return now() - last_frame_hook < 0.15; }

/* Menus, the automap-free front end and loading screens wait in a busy loop
   that polls the mouse (int 33h 0Bh) and the time (int 21h 2Ch) hundreds of
   thousands of times per second, which keeps a CPU core at 100%. Outside the
   main game loop such polls are spaced at least 1 ms apart. */
unsigned long io_idle_sleeps;
volatile float io_mouse_vertical = 1.25f;
void io_idle_poll(void)
{
    static double last;
    if (main_loop_running()) return;
    double t = now();
    if (t - last < 0.001) { plat_sleep_ms(1); io_idle_sleeps++; t = now(); }
    last = t;
}

/* 1 = deliver now, 0 = drop, 2 = keep it queued until the next frame hook */
static int menu_key_filter(int code, int make)
{
    static unsigned char down[256], dropped[256];
    static double last_slot[BIND_SLOTS];
    if (io_menu_key_delay <= 0) return 1;
    if (!make && (dropped[code] || auto_released[code])) {     /* releases of filtered presses */
        down[code] = 0; auto_release_at[code] = 0;
        dropped[code] = auto_released[code] = 0;
        return 0;
    }
    if (!main_loop_running()) { down[code] = make; return 1; }
    const uint8_t *tbl = DATA(0x13C1A5);
    int slot = -1;
    for (int i = 0; i < BIND_SLOTS; i++)
        if (tbl[i * 22] == code) { slot = i; if (slot_is_movement(i)) return down[code] = make, 1; }
    if (slot < 0) { down[code] = make; return 1; }       /* not bound to a game function */
    int fn = slot & ~1;                                  /* primary and secondary key share a timer */
    if (!make) { down[code] = 0; auto_release_at[code] = 0; return 1; }
    if (down[code]) return 0;                            /* typematic repeat */
    if (!in_frame_hook) return 2;                        /* hand it over at the start of a frame */
    down[code] = 1;
    double t = now();
    if (t - last_slot[fn] < io_menu_key_delay / 1000.0) { dropped[code] = 1; return 0; }
    last_slot[fn] = t;
    auto_release_at[code] = t + 0.25;                    /* fallback if the main loop stops */
    auto_release_frame[code] = frame_count + 1;
    auto_released[code] = 0;
    return 1;
}

static void kbd_deliver(dtu_stack *s, const uint8_t *bytes, int n)
{
    for (int i = 0; i < n; i++) {
        uint8_t c = bytes[i];
        kbd_data = c; kbd_pending = 1;
        if (n == 1 && c < 0x80) {
            uint16_t k = (uint16_t)((c << 8) | (uint8_t)sc_ascii[c & 127]);
            bios_buf[bios_tail] = k; bios_tail = (bios_tail + 1) & 31;
        }
        uint32_t h1 = dtu_irq_handler(1);
        if (h1) { X86_InterruptFlag = 0; dtu_c_call_irq(s, h1); X86_InterruptFlag = 1; }
        kbd_pending = 0;
    }
}

static void script_keys(double t)
{
    /* scripted key presses (headless tests) */
    for (int i = 0; i < nkeys; i++) {
        if (keys[i].done || keys[i].t > t) continue;
        keys[i].done = 1;
        if (keys[i].sc & 0x100) io_key_byte(0xE0);
        io_key_byte((uint8_t)keys[i].sc);
        if (nkeys < 40) dtu_log("[key] scancode %02X at %.2fs", keys[i].sc, t);
    }
}

static unsigned long irq_calls;
static void housekeeping(double t)
{
    static double next_shot = 1.0;
    static uint32_t last_hash;
    if (io_quit_requested) {
        dtu_log("[main] quit requested by the host");
        dtu_exit(0);
    }
    if (io_shot_every > 0 && t >= next_shot) {
        uint32_t h = fb_hash();
        next_shot = t + io_shot_every;
        if (h != last_hash) {
            char tag[32];
            snprintf(tag, sizeof tag, "t%05.1f", t);
            last_hash = h;
            dtu_screenshot(tag);
        }
    }
    if (io_run_seconds > 0 && t >= io_run_seconds) {
        dtu_log("[main] time limit %.1fs reached (%lu timer interrupts, %lu vretrace frames)", io_run_seconds, irq_calls, vga_frames);
        dtu_exit(0);
    }
}

void dtu_run_timer(dtu_stack *s)
{
    static int in_irq;
    uint32_t target = dtu_timer_tick;
    double t;

    if (in_irq) return;
    in_irq = 1;
    {   /* diagnostics: the game thread did not come by for a long time (sound depends on it) */
        static double last; static int nlog;
        double tn = now();
        if (last > 0 && tn - last > 0.15 && irq_calls && nlog < 30) { nlog++; dtu_log("[irq] no timer check for %.0f ms", (tn - last) * 1000); }
        last = tn;
    }
    {
        int32_t lag = (int32_t)(target - dtu_timer_run);
        if (lag < 0) dtu_timer_run = target;           /* the PIT was reprogrammed: count restarts */
        else if (lag > 60) {
            static int nlog;
            if (nlog++ < 20) {
                uint32_t *sp = (uint32_t *)(uintptr_t)s->esp;
                dtu_log("[irq] %d timer ticks late, %d dropped (IF %u, stack %08X %08X %08X)", lag, lag - 60, X86_InterruptFlag, sp[0], sp[1], sp[2]);
            }
            dtu_timer_run = target - 60;
        }
    }

    {   /* diagnostics: interrupts disabled for a long time */
        static double if0_since; static int nlog;
        if (X86_InterruptFlag) if0_since = 0;
        else if (if0_since == 0) if0_since = now();
        else if (now() - if0_since > 0.5 && nlog < 5) {
            uint32_t *sp = (uint32_t *)(uintptr_t)s->esp;
            nlog++; if0_since = now();
            dtu_log("[irq] interrupts disabled for 0.5 s (stack %08X %08X %08X %08X %08X %08X %08X %08X)", sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]);
        }
    }
    if (X86_InterruptFlag) {
        uint32_t h0 = dtu_irq_handler(0);
        while (dtu_timer_run != target) {
            dtu_timer_run++;
            if (h0) {
                X86_InterruptFlag = 0;
                dtu_c_call_irq(s, h0);
                X86_InterruptFlag = 1;
                if (++irq_calls == 1) dtu_log("[irq] first timer interrupt -> %08X", h0);
            }
        }
    }
    /* with interrupts disabled the ticks stay pending (as in the PIC) and
       are delivered at the next check point after sti */

    t = now();
    /* scripted mouse actions (headless tests) */
    for (int i = 0; i < nmice; i++) {
        if (mice[i].done || mice[i].t > t) continue;
        mice[i].done = 1;
        static int sx = 320, sy = 240;      /* the script gives absolute positions */
        io_mouse(mice[i].x - sx, mice[i].y - sy, mice[i].b);
        sx = mice[i].x; sy = mice[i].y;
        dtu_log("[mouse] script %d,%d buttons %d at %.2fs", mice[i].x, mice[i].y, mice[i].b, t);
    }
    if (X86_InterruptFlag) mouse_deliver(s);
    script_keys(t);
    /* hand all waiting scancode bytes to the game's keyboard interrupt, one
       interrupt per byte (one byte per call let held keys queue up: Windows
       repeats a held key ~30 times per second, two bytes each for arrows).
       An E0 prefix is handled together with the byte after it. */
    while (X86_InterruptFlag && !kbd_pending && kq_head != __atomic_load_n(&kq_tail, __ATOMIC_ACQUIRE)) {
        uint8_t b = kq[kq_head & (KQ - 1)], b2 = 0;
        int n = 1;
        if (b == 0xE0) {
            if (kq_head + 1 == __atomic_load_n(&kq_tail, __ATOMIC_ACQUIRE)) break;   /* wait for the second byte */
            b2 = kq[(kq_head + 1) & (KQ - 1)];
            n = 2;
        }
        int code = n == 2 ? (0x80 | (b2 & 0x7F)) : (b & 0x7F);
        int make = !((n == 2 ? b2 : b) & 0x80);
        int f = menu_key_filter(code, make);
        if (f == 2) break;                               /* wait for the frame hook */
        kq_head += n;
        if (!f) { if (dtu_verbose > 1) dtu_log("[key] %02X %s dropped (menu key filter)", code, make ? "press" : "release"); continue; }
        if (dtu_verbose > 1) dtu_log("[key] %02X %s at %.3f", code, make ? "press" : "release", now());
        uint8_t bytes[2] = { b, b2 };
        kbd_deliver(s, bytes, n);
    }
    /* automatic releases of held menu-function keys */
    if (X86_InterruptFlag && !kbd_pending) {
        double tn = now();
        for (int c = 0; c < 256; c++) {
            if (auto_release_at[c] <= 0 || (tn < auto_release_at[c] && frame_count < auto_release_frame[c])) continue;
            auto_release_at[c] = 0; auto_released[c] = 1;
            if (dtu_verbose > 1) dtu_log("[key] %02X automatic release at %.3f", c, tn);
            uint8_t bytes[2];
            int n = 0;
            if (c & 0x80) bytes[n++] = 0xE0;
            bytes[n++] = (uint8_t)(0x80 | (c & 0x7F));
            kbd_deliver(s, bytes, n);
        }
    }
    in_irq = 0;
    housekeeping(t);
}

/* While a movie plays (mve.c) the game's interrupts do not run: scripted keys,
   screenshots and the time limit are handled here. Returns 1 if a key was
   pressed since the last call; all waiting keys are consumed. */
int io_movie_poll(void)
{
    double t = now();
    int pressed = 0;
    script_keys(t);
    while (kq_head != __atomic_load_n(&kq_tail, __ATOMIC_ACQUIRE)) {
        uint8_t b = kq[kq_head & (KQ - 1)];
        kq_head++;
        if (b != 0xE0 && !(b & 0x80)) pressed = 1;
    }
    housekeeping(t);
    return pressed;
}
