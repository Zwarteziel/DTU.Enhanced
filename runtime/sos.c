/* Undermountain Enhanced - HMI SOS digital sound replaced by a software mixer
 *
 * DTU plays its sound through the HMI Sound Operating System (SOS 4): a
 * library linked into the game plus a driver (HMIDRV.386) that it loads from
 * disk at run time and that talks to the sound card. The driver cannot run
 * here, so the library's DIGI functions are replaced (instruction
 * replacements at their entry points, via x86_int 0xEF01xx) by this mixer.
 * The SOS timer service stays as it is: it calls the "fill handler" that
 * sosDIGIInitDriver hands out 120 times per second from the game's timer
 * interrupt.
 *
 * The mixing itself runs on the audio thread (the SDL stream asks for sound
 * when it needs it), so sound keeps playing when the game thread is busy for
 * a while, for instance while a level loads. The game's sample callbacks
 * must run on the game thread: the fill handler calls them. Sample data is
 * copied when a sample starts, so for streamed sounds the callback that asks
 * for the next buffer can come early and the next buffer is already waiting
 * when the current one ends. The music comes in buffers of 186 ms that the
 * game decodes in turn in its callback (0xA43FC: continue with the decoded
 * buffer, decode the next 16 KB into the other one); its callback comes as
 * soon as a buffer starts playing, so 186..372 ms of music are always ready.
 * Other sounds the game continues from its callback get theirs LEAD early.
 * Without a pull-capable audio output (headless tests) the fill handler
 * mixes and queues the sound itself, as before.
 *
 * Music while the game is loading (a level, a saved game, the menu after
 * dying): the game thread then often runs no timer interrupt for longer than
 * the music it has decoded, the music runs dry, and every time the game gets
 * a moment it decodes one more buffer: the music stutters. Now (sos_music_hold)
 * the music fades out in its last 30 ms when no next buffer is there; if it
 * then runs dry while the game thread is stalled (no fill handler call for
 * 100 ms), music holds: stereo streams stay paused (silent, not consumed, so
 * the game decodes nothing new) until the fill handler has run smoothly for
 * 0.5 s, and then fade in over 0.3 s from where they stopped.
 *
 * Sample structure (_SOS_START_SAMPLE, all fields 32 bits):
 *   00 data (offset), 04 selector, 08 size in bytes, 0C loop count,
 *   10 channel, 14 volume (0..7FFF), 18 sample ID, 1C callback (offset),
 *   20 selector, 24 port, 28 flags, 2C byte length, 30 loop point,
 *   34 loop length, 38 pitch add, 3C pitch fraction, 40 pan (0..FFFF). */
#include "dtu.h"
#include "platform.h"
#include <stdlib.h>
#include <string.h>

#define NCH 32
#define RATE 22050
#define LEAD (RATE * 15 / 100)          /* streamed sounds: next buffer 150 ms before the end */
#define FADE_OUT (RATE * 3 / 100)       /* music: fade out over the last 30 ms when no next buffer is there */
#define FADE_IN (RATE * 3 / 10)         /* and back in over 0.3 s */

typedef struct {
    int used, buf;                      /* buf: index in the channel's buffer pool */
    uint32_t frames;
    int loops, vol, pan, stereo;
} segment;

typedef struct {
    int active;                 /* for the game: until the "done" callback */
    int ended;                  /* the mixer reached the end of the data */
    int cb_sent;                /* "processed" callback sent for the current segment */
    int streaming;              /* callback early, see above */
    int continued;              /* sosDIGIContinueSample during a callback */
    segment cur, next;
    uint32_t pos;               /* frame in the current segment */
    int loops, vol, pan, stereo;
    uint32_t cb;                /* callback offset, 0 = none */
    double gain;                /* music fade (stereo streams), 0..1 */
    int id;
    struct { int16_t *p; uint32_t cap; } pool[3];
} channel;

static channel ch[NCH];
static plat_mutex mx;
static int mx_ready, open_ok, pull_ok, started;
static double last_fill = -1, frac;
static unsigned long nlog;
int sos_music_hold = 1;                 /* see above; 0 = original */
static volatile double fill_last = -1, fill_smooth = -1;   /* game thread: last fill handler call, start of the last smooth run */
static int music_held;                  /* (lock held) music paused until the game thread runs smoothly */
static double music_held_t;
static uint32_t stream_cbs[8];          /* callbacks seen continuing a sample */

extern void dtu_c_call_sos(dtu_stack *s, uint32_t handler, uint32_t a, uint32_t b, uint32_t c);
extern uint8_t SR_dtu_sos_fill[];

static uint32_t *stack_args(dtu_regs *r) { return (uint32_t *)r + 11; }   /* above eip (x86_int) and the game's return */
static void lock(void) { if (!mx_ready) { plat_mutex_init(&mx); mx_ready = 1; } plat_lock(&mx); }
static void unlock(void) { plat_unlock(&mx); }

static int stream_cb(uint32_t cb)
{
    for (int i = 0; i < 8; i++) if (stream_cbs[i] == cb) return 1;
    return 0;
}

/* read a sample structure into segment s of channel c, copying the data (lock held) */
static void load_segment(channel *c, segment *s, const uint8_t *ss, int h)
{
    uint32_t v[17];
    memcpy(v, ss, sizeof v);
    int b;
    for (b = 0; b < 3; b++)
        if (!(c->cur.used && c->cur.buf == b) && !(c->next.used && c->next.buf == b && s != &c->next)) break;
    uint32_t size = v[2];
    if (size > (16u << 20)) size = 0;
    s->stereo = (uint16_t)v[4] == 3;    /* 2: mono, 3: interleaved stereo (music); all DTU sounds are 16-bit */
    s->frames = size / (s->stereo ? 4u : 2u);
    if (c->pool[b].cap < size) {
        int16_t *p = realloc(c->pool[b].p, size);
        if (!p) s->frames = 0; else { c->pool[b].p = p; c->pool[b].cap = size; }
    }
    if (s->frames) memcpy(c->pool[b].p, PTR(v[0]), (size_t)s->frames * (s->stereo ? 4 : 2));
    s->used = 1;
    s->buf = b;
    s->loops = (uint16_t)v[3] == 0xFFFF ? -1 : (int)(uint16_t)v[3];   /* 0xFFFF: loop forever (ambient sounds) */
    s->vol = (int)(uint16_t)v[5];
    s->pan = (int)(uint16_t)v[16];
    c->id = (int)(uint16_t)v[6];
    c->cb = v[7];
    if (nlog < 40) {
        nlog++;
        dtu_log("[sos] %s %d: data %08X size %u loops %d chan %d vol %04X id %d cb %08X flags %04X pan %04X",
                s == &c->next ? "next" : c->active ? "continue" : "start", h, v[0], v[2], s->loops, (int)(uint16_t)v[4],
                s->vol, c->id, c->cb, (uint16_t)v[10], s->pan);
    }
}

static void enter_segment(channel *c)
{
    c->pos = 0;
    c->loops = c->cur.loops; c->vol = c->cur.vol; c->pan = c->cur.pan; c->stereo = c->cur.stereo;
    c->ended = 0;
    c->cb_sent = 0;
}

/* mix n frames of channel c into acc (lock held) */
/* music (stereo stream) with the fade of sos_music_hold: per frame, gain follows
   min(1, frames left / FADE_OUT) when no next buffer is there, else 1, at most
   1/FADE_IN up per frame (lock held) */
static void mix_music(channel *c, int32_t *acc, int n)
{
    double lg0 = c->vol / 32768.0, rg0 = lg0;
    if (c->pan < 0x8000) rg0 *= c->pan / 32768.0; else lg0 *= (0xFFFF - c->pan) / 32767.0;
    for (int i = 0; i < n; i++) {
        if (c->pos >= c->cur.frames) {
            if (c->loops != 0 && c->cur.frames) { if (c->loops > 0) c->loops--; c->pos = 0; i--; continue; }
            if (c->next.used) { c->cur = c->next; c->next.used = 0; enter_segment(c); i--; continue; }
            c->ended = 1;
            c->gain = 0;
            double t = plat_now();
            if (!music_held && fill_last >= 0 && t - fill_last > 0.1) {
                music_held = 1; music_held_t = t;
                if (dtu_verbose >= 2) dtu_log("[sos] t=%.3f music ran dry while the game is busy: held", t);
            }
            return;
        }
        double target = 1;
        if (!c->next.used && c->loops == 0) {
            double left = (double)(c->cur.frames - c->pos) / FADE_OUT;
            if (left < target) target = left;
        }
        if (c->gain > target) c->gain = target;
        else if (c->gain < target) { c->gain += 1.0 / FADE_IN; if (c->gain > target) c->gain = target; }
        const int16_t *d = c->pool[c->cur.buf].p + (size_t)c->pos * 2;
        acc[2 * i] += (int32_t)(d[0] * lg0 * c->gain);
        acc[2 * i + 1] += (int32_t)(d[1] * rg0 * c->gain);
        c->pos++;
    }
}

static void mix_channel(channel *c, int32_t *acc, int n)
{
    for (int i = 0; i < n;) {
        if (c->pos >= c->cur.frames) {
            if (c->loops != 0 && c->cur.frames) { if (c->loops > 0) c->loops--; c->pos = 0; continue; }
            if (c->next.used) { c->cur = c->next; c->next.used = 0; enter_segment(c); continue; }
            c->ended = 1;
            return;
        }
        double lg = c->vol / 32768.0, rg = lg;
        if (c->pan < 0x8000) rg *= c->pan / 32768.0; else lg *= (0xFFFF - c->pan) / 32767.0;
        int k = (int)(c->cur.frames - c->pos);
        if (k > n - i) k = n - i;
        const int16_t *d = c->pool[c->cur.buf].p + (size_t)c->pos * (c->stereo ? 2 : 1);
        int32_t *o = acc + 2 * i;
        if (c->stereo) for (int j = 0; j < k; j++) { o[2 * j] += (int32_t)(d[2 * j] * lg); o[2 * j + 1] += (int32_t)(d[2 * j + 1] * rg); }
        else for (int j = 0; j < k; j++) { o[2 * j] += (int32_t)(d[j] * lg); o[2 * j + 1] += (int32_t)(d[j] * rg); }
        c->pos += (uint32_t)k;
        i += k;
    }
}

/* n stereo frames of mixed sound (audio thread, or the fill handler without one) */
static void sos_mix(int16_t *out, int n)
{
    static int32_t acc[2 * 2048];
    while (n > 0) {
        int k = n > 2048 ? 2048 : n;
        memset(acc, 0, (size_t)k * 2 * sizeof *acc);
        lock();
        if (music_held) {                       /* release once the game thread runs smoothly again */
            double t = plat_now();
            if (fill_last >= 0 && t - fill_last < 0.1 && fill_smooth >= 0 && t - fill_smooth >= 0.5 && t - music_held_t >= 0.5) {
                music_held = 0;
                if (dtu_verbose >= 2) dtu_log("[sos] t=%.3f music resumes after %.2f s", t, t - music_held_t);
            }
        }
        for (int i = 0; i < NCH; i++) {
            channel *c = &ch[i];
            if (!c->active || c->ended || !c->cur.used) continue;
            if (sos_music_hold && c->stereo && c->streaming) { if (!music_held) mix_music(c, acc, k); }
            else mix_channel(c, acc, k);
        }
        unlock();
        for (int i = 0; i < k * 2; i++) out[i] = (int16_t)(acc[i] > 32767 ? 32767 : acc[i] < -32768 ? -32768 : acc[i]);
        out += 2 * k; n -= k;
    }
}

/* the game's callbacks: "processed" (0) when a sample ended or, for streamed
   sounds, LEAD before; "done" (2) when it ended without being continued */
static void callbacks(dtu_stack *st)
{
    for (int i = 0; i < NCH; i++) {
        channel *c = &ch[i];
        lock();
        if (!c->active) { unlock(); continue; }
        if (!c->cb) { if (c->ended) c->active = 0; unlock(); continue; }
        uint32_t cb = c->cb;
        if (c->ended) {
            if (!c->cb_sent) {
                c->cb_sent = 1; c->continued = 0;
                unlock();
                dtu_c_call_sos(st, cb, 0, 0, (uint32_t)i);
                lock();
                if (c->continued && !c->streaming && !stream_cb(cb)) {
                    for (int k = 0; k < 8; k++) if (!stream_cbs[k]) { stream_cbs[k] = cb; break; }
                }
                if (c->continued) c->streaming = 1;
            }
            int done = c->active && c->ended;
            if (done) c->active = 0;
            unlock();
            if (done) dtu_c_call_sos(st, cb, 0, 2, (uint32_t)i);
            continue;
        }
        if (c->streaming && !c->cb_sent && !c->next.used && c->loops == 0 && (c->stereo || c->cur.frames - c->pos < LEAD)) {
            c->cb_sent = 1; c->continued = 0;
            unlock();
            dtu_c_call_sos(st, cb, 0, 0, (uint32_t)i);
            continue;
        }
        unlock();
    }
}

static void fill(dtu_regs *r)
{
    dtu_stack st = { (uint32_t)(uintptr_t)r, 0 };
    {
        double t = plat_now();
        if (fill_last < 0 || t - fill_last > 0.1) fill_smooth = t;      /* a stall ends: a new smooth run starts */
        fill_last = t;
    }
    callbacks(&st);
    if (pull_ok) return;
    /* no audio thread: mix here for the time since the last call */
    double t = plat_now();
    if (last_fill < 0) last_fill = t;
    double el = t - last_fill;
    last_fill = t;
    if (el > 0.1) el = 0.1;
    double want = el * RATE + frac;
    if (open_ok) {
        double q = fe_audio_queued(FE_GAME);
        if (q < 0.03) want += (0.06 - q) * RATE;        /* keep about 60 ms queued */
    }
    int n = (int)want;
    frac = want - n;
    if (n <= 0) return;
    if (n > RATE / 5) n = RATE / 5;
    int16_t *out = malloc((size_t)n * 4);
    if (!out) return;
    sos_mix(out, n);
    /* without a sound device the samples still run (the game waits for some to end) */
    if (open_ok) {
        fe_audio_queue(FE_GAME, out, n * 2);
        if (!started) { fe_audio_start(FE_GAME); started = 1; }
    }
    free(out);
}

static void reset_channels(void)
{
    lock();
    for (int i = 0; i < NCH; i++) {
        channel *c = &ch[i];
        c->active = c->ended = c->cb_sent = c->streaming = c->continued = 0;
        c->cur.used = c->next.used = 0;
        c->cb = 0;
    }
    unlock();
}

void sos_call(int fn, dtu_regs *r)
{
    switch (fn) {
    case 0x01: EAX = 0; break;                                   /* sosDIGIInitSystem */
    case 0x02: EAX = 0; break;                                   /* sosDIGIUnInitSystem */
    case 0x03: {                                                 /* sosDIGIInitDriver(id, hw, init, handle) */
        uint32_t *a = stack_args(r);
        uint8_t *init = PTR(a[0]);
        uint32_t stub = (uint32_t)(uintptr_t)SR_dtu_sos_fill;
        uint32_t sel = 0x18;
        memcpy(init + 0x1C, &stub, 4);                           /* fill handler for the timer service */
        memcpy(init + 0x20, &sel, 4);
        *(uint32_t *)PTR(a[2]) = 0;                              /* driver handle 0 */
        fe_audio_close(FE_GAME);
        reset_channels();
        started = 0; last_fill = -1; frac = 0;
        pull_ok = fe_audio_open_pull(FE_GAME, RATE, 2, sos_mix);
        open_ok = pull_ok || fe_audio_open(FE_GAME, RATE, 2);
        if (pull_ok) { fe_audio_start(FE_GAME); started = 1; }
        dtu_log("[sos] driver %04X initialised: software mixer, %d Hz stereo%s", EAX, RATE,
                pull_ok ? ", mixed on the audio thread" : open_ok ? "" : " (no audio output)");
        EAX = 0;
        break; }
    case 0x04:                                                   /* sosDIGIUnInitDriver */
        fe_audio_close(FE_GAME);
        reset_channels();
        open_ok = pull_ok = 0;
        EAX = 0;
        break;
    case 0x05: {                                                 /* sosDIGIStartSample(drv, sample far) */
        int h;
        lock();
        for (h = 0; h < NCH && ch[h].active; h++) {}
        if (h == NCH) { unlock(); EAX = 0xFFFFFFFF; break; }
        channel *c = &ch[h];
        c->cur.used = c->next.used = 0;
        load_segment(c, &c->cur, PTR(EBX), h);
        enter_segment(c);
        c->streaming = c->cur.stereo || stream_cb(c->cb);
        c->continued = 0;
        c->gain = music_held ? 0 : 1;   /* music: starts at full volume, or fades in after a hold */
        c->active = 1;
        unlock();
        EAX = (uint32_t)h;
        break; }
    case 0x06:                                                   /* sosDIGIStopSample(drv, h) */
        if (EDX < NCH) { lock(); ch[EDX].active = 0; ch[EDX].cur.used = ch[EDX].next.used = 0; unlock(); }
        EAX = 0;
        break;
    case 0x07:                                                   /* sosDIGIContinueSample(drv, h, sample far) */
        if (EDX < NCH) {
            channel *c = &ch[EDX];
            lock();
            if (c->active && !c->ended && c->cur.used) load_segment(c, &c->next, PTR(EBX), (int)EDX);   /* queued after the current data */
            else { c->next.used = 0; load_segment(c, &c->cur, PTR(EBX), (int)EDX); enter_segment(c); c->active = 1; }
            c->continued = 1;
            unlock();
        }
        EAX = 0;
        break;
    case 0x08:                                                   /* sosDIGISetSampleVolume(drv, h, vol) */
        if (EDX < NCH) { lock(); EAX = (uint32_t)ch[EDX].vol; ch[EDX].vol = (uint16_t)EBX; unlock(); } else EAX = 0;
        break;
    case 0x09:                                                   /* sosDIGISetPanLocation(drv, h, pan) */
        if (EDX < NCH) { lock(); EAX = (uint32_t)ch[EDX].pan; ch[EDX].pan = (uint16_t)EBX; unlock(); } else EAX = 0;
        break;
    case 0x0A: {                                                 /* sosDIGIGetSampleHandle(drv, id) */
        EAX = 0xFFFFFFFF;
        for (int h = 0; h < NCH; h++) if (ch[h].active && ch[h].id == (int)(uint16_t)EDX) { EAX = (uint32_t)h; break; }
        break; }
    case 0x0B:                                                   /* sosDIGISampleDone(drv, h) */
        EAX = EDX >= NCH ? 0xA : ch[EDX].active ? 0 : 1;
        break;
    case 0x10: fill(r); break;                                   /* fill handler (timer event) */
    default: dtu_log("[sos] unknown function %d", fn); break;
    }
}
