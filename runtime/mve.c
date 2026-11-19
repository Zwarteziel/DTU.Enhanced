/* Undermountain Enhanced - Interplay MVE movie player
   Replaces the game's MVE library (MPLAY32, 0x96439): the 8-bit video codec
   (opcode 0x11 with a 4-bit decoding map), 8-bit PCM and 16-bit DPCM sound.
   Format and block opcodes as documented by the ffmpeg (interplayvideo.c,
   ipmovie.c) and d2x (mvelib) projects; the DPCM delta table is read from the
   game's own data (0x14C864). */
#include "dtu.h"
#include "platform.h"
#include <stdlib.h>
#include <string.h>

#define DPCM_TABLE 0x14C864u

typedef struct {
    int w, h, bw, bh;           /* picture size in pixels and in 8x8 blocks */
    uint8_t *cur, *prev;        /* target buffer (holds frame n-2) and frame n-1 */
    uint8_t *map; int map_len;  /* 4-bit opcode per block */
    uint8_t pal[768];
    int pal_dirty;
    /* sound */
    int a_rate, a_ch, a_16, a_dpcm, a_open, a_started;
    const int16_t *delta;
} mve;

/* ---------------- video ---------------- */
typedef struct { const uint8_t *p, *end; } stream;
static int rd(stream *s) { return s->p < s->end ? *s->p++ : 0; }
static unsigned rd16(stream *s) { unsigned a = rd(s); return a | (unsigned)rd(s) << 8; }
static uint32_t rd32(stream *s) { uint32_t a = rd16(s); return a | (uint32_t)rd16(s) << 16; }
static uint64_t rd64(stream *s) { uint64_t a = rd32(s); return a | (uint64_t)rd32(s) << 32; }

/* copy an 8x8 block from buffer src at (x+dx, y+dy) to dst at (x, y) */
static void copy_block(mve *m, uint8_t *dst, const uint8_t *src, int x, int y, int dx, int dy)
{
    int sx = x + dx, sy = y + dy;
    if (sx < 0 || sy < 0 || sx + 8 > m->w || sy + 8 > m->h) return;
    for (int r = 0; r < 8; r++)
        memmove(dst + (size_t)(y + r) * m->w + x, src + (size_t)(sy + r) * m->w + sx, 8);
}

static void decode_block(mve *m, int op, stream *s, int x, int y)
{
    const int st = m->w;
    uint8_t *px = m->cur + (size_t)y * st + x;
    uint8_t P[8];
    int b, dx, dy;
    switch (op) {
    case 0x0: copy_block(m, m->cur, m->prev, x, y, 0, 0); break;
    case 0x1: break;                                   /* unchanged: frame n-2 is already there */
    case 0x2:                                          /* from frame n-2 (this buffer), right/below */
        b = rd(s);
        if (b < 56) { dx = 8 + b % 7; dy = b / 7; } else { dx = -14 + (b - 56) % 29; dy = 8 + (b - 56) / 29; }
        copy_block(m, m->cur, m->cur, x, y, dx, dy);
        break;
    case 0x3:                                          /* from this frame, up/left */
        b = rd(s);
        if (b < 56) { dx = -(8 + b % 7); dy = -(b / 7); } else { dx = -(-14 + (b - 56) % 29); dy = -(8 + (b - 56) / 29); }
        copy_block(m, m->cur, m->cur, x, y, dx, dy);
        break;
    case 0x4:                                          /* from frame n-1, small offset */
        b = rd(s);
        copy_block(m, m->cur, m->prev, x, y, -8 + (b & 15), -8 + (b >> 4));
        break;
    case 0x5:                                          /* from frame n-1, signed offset */
        dx = (int8_t)rd(s); dy = (int8_t)rd(s);
        copy_block(m, m->cur, m->prev, x, y, dx, dy);
        break;
    case 0x6: break;                                   /* not used in 8-bit movies */
    case 0x7:                                          /* 2 colours */
        P[0] = (uint8_t)rd(s); P[1] = (uint8_t)rd(s);
        if (P[0] <= P[1]) {
            for (int r = 0; r < 8; r++) {
                int f = rd(s);
                for (int c = 0; c < 8; c++, f >>= 1) px[r * st + c] = P[f & 1];
            }
        } else {
            unsigned f = rd16(s);
            for (int r = 0; r < 8; r += 2)
                for (int c = 0; c < 8; c += 2, f >>= 1)
                    px[r * st + c] = px[r * st + c + 1] = px[(r + 1) * st + c] = px[(r + 1) * st + c + 1] = P[f & 1];
        }
        break;
    case 0x8:                                          /* 2 colours per quadrant or half */
        P[0] = (uint8_t)rd(s); P[1] = (uint8_t)rd(s);
        if (P[0] <= P[1]) {
            /* quadrants in the order top-left, bottom-left, top-right, bottom-right */
            uint8_t *q = px;
            unsigned f = 0;
            for (int r = 0; r < 16; r++) {
                if (!(r & 3)) {
                    if (r) { P[0] = (uint8_t)rd(s); P[1] = (uint8_t)rd(s); }
                    f = rd16(s);
                }
                for (int c = 0; c < 4; c++, f >>= 1) q[c] = P[f & 1];
                q += st;
                if (r == 7) q = px + 4;
            }
        } else {
            uint32_t f = rd32(s);
            P[2] = (uint8_t)rd(s); P[3] = (uint8_t)rd(s);
            if (P[2] <= P[3]) {                        /* left and right halves */
                uint8_t *q = px;
                for (int r = 0; r < 16; r++) {
                    for (int c = 0; c < 4; c++, f >>= 1) q[c] = P[f & 1];
                    q += st;
                    if (r == 7) { q = px + 4; P[0] = P[2]; P[1] = P[3]; f = rd32(s); }
                }
            } else {                                   /* top and bottom halves */
                for (int r = 0; r < 8; r++) {
                    if (r == 4) { P[0] = P[2]; P[1] = P[3]; f = rd32(s); }
                    for (int c = 0; c < 8; c++, f >>= 1) px[r * st + c] = P[f & 1];
                }
            }
        }
        break;
    case 0x9:                                          /* 4 colours */
        for (int i = 0; i < 4; i++) P[i] = (uint8_t)rd(s);
        if (P[0] <= P[1]) {
            if (P[2] <= P[3]) {
                for (int r = 0; r < 8; r++) {
                    unsigned f = rd16(s);
                    for (int c = 0; c < 8; c++, f >>= 2) px[r * st + c] = P[f & 3];
                }
            } else {
                uint32_t f = rd32(s);
                for (int r = 0; r < 8; r += 2)
                    for (int c = 0; c < 8; c += 2, f >>= 2)
                        px[r * st + c] = px[r * st + c + 1] = px[(r + 1) * st + c] = px[(r + 1) * st + c + 1] = P[f & 3];
            }
        } else {
            uint64_t f = rd64(s);
            if (P[2] <= P[3]) {
                for (int r = 0; r < 8; r++)
                    for (int c = 0; c < 8; c += 2, f >>= 2) px[r * st + c] = px[r * st + c + 1] = P[f & 3];
            } else {
                for (int r = 0; r < 8; r += 2)
                    for (int c = 0; c < 8; c++, f >>= 2) px[r * st + c] = px[(r + 1) * st + c] = P[f & 3];
            }
        }
        break;
    case 0xA:                                          /* 4 colours per quadrant or half */
        for (int i = 0; i < 4; i++) P[i] = (uint8_t)rd(s);
        if (P[0] <= P[1]) {
            uint8_t *q = px;
            uint32_t f = 0;
            for (int r = 0; r < 16; r++) {
                if (!(r & 3)) {
                    if (r) for (int i = 0; i < 4; i++) P[i] = (uint8_t)rd(s);
                    f = rd32(s);
                }
                for (int c = 0; c < 4; c++, f >>= 2) q[c] = P[f & 3];
                q += st;
                if (r == 7) q = px + 4;
            }
        } else {
            uint64_t f = rd64(s);
            for (int i = 4; i < 8; i++) P[i] = (uint8_t)rd(s);
            int vert = P[4] <= P[5];
            uint8_t *q = px;
            for (int r = 0; r < 16; r++) {
                for (int c = 0; c < 4; c++, f >>= 2) q[c] = P[f & 3];
                if (vert) { q += st; if (r == 7) q = px + 4; }
                else { q += 4; if (r & 1) q += st - 8; }
                if (r == 7) { memcpy(P, P + 4, 4); f = rd64(s); }
            }
        }
        break;
    case 0xB:                                          /* raw 8x8 */
        for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) px[r * st + c] = (uint8_t)rd(s);
        break;
    case 0xC:                                          /* raw 4x4 of 2x2 */
        for (int r = 0; r < 8; r += 2)
            for (int c = 0; c < 8; c += 2) {
                uint8_t v = (uint8_t)rd(s);
                px[r * st + c] = px[r * st + c + 1] = px[(r + 1) * st + c] = px[(r + 1) * st + c + 1] = v;
            }
        break;
    case 0xD:                                          /* one colour per quadrant */
        for (int r = 0; r < 8; r++) {
            if (!(r & 3)) { P[0] = (uint8_t)rd(s); P[1] = (uint8_t)rd(s); }
            memset(px + r * st, P[0], 4);
            memset(px + r * st + 4, P[1], 4);
        }
        break;
    case 0xE:                                          /* one colour */
        b = rd(s);
        for (int r = 0; r < 8; r++) memset(px + r * st, b, 8);
        break;
    case 0xF:                                          /* checkerboard */
        P[0] = (uint8_t)rd(s); P[1] = (uint8_t)rd(s);
        for (int r = 0; r < 8; r++)
            for (int c = 0; c < 8; c++) px[r * st + c] = P[(r ^ c) & 1];
        break;
    }
}

static void decode_frame(mve *m, const uint8_t *data, int len)
{
    if (len < 14 || !m->cur) return;
    unsigned flags = data[12] | data[13] << 8;
    if (flags & 1) { uint8_t *t = m->cur; m->cur = m->prev; m->prev = t; }
    stream s = { data + 14, data + len };
    int n = m->bw * m->bh;
    if (n > m->map_len * 2) n = m->map_len * 2;
    for (int i = 0; i < n; i++) {
        int op = (m->map[i >> 1] >> ((i & 1) * 4)) & 15;
        decode_block(m, op, &s, (i % m->bw) * 8, (i / m->bw) * 8);
    }
}

/* ---------------- sound ---------------- */
static void audio_frame(mve *m, const uint8_t *d, int len, int silence)
{
    if (len < 6 || !m->a_open) return;
    unsigned mask = d[2] | d[3] << 8, out = d[4] | d[5] << 8;
    if (!(mask & 1)) return;
    int n = m->a_16 ? (int)out / 2 : (int)out;      /* samples, all channels */
    if (n <= 0) return;
    int16_t *buf = calloc((size_t)n, sizeof *buf);
    if (!buf) return;
    if (!silence) {
        const uint8_t *p = d + 6, *end = d + len;
        if (m->a_dpcm) {
            int16_t pred[2] = { 0, 0 };
            int i = 0;
            for (int c = 0; c < m->a_ch && i < n && p + 2 <= end; c++, i++, p += 2) buf[i] = pred[c] = (int16_t)(p[0] | p[1] << 8);
            for (int c = 0; i < n && p < end; i++, p++, c ^= (m->a_ch - 1)) {
                pred[c] = (int16_t)(uint16_t)((uint16_t)pred[c] + (uint16_t)m->delta[*p]);   /* wraps, as the original */
                buf[i] = pred[c];
            }
        } else if (m->a_16) {
            for (int i = 0; i < n && p + 2 <= end; i++, p += 2) buf[i] = (int16_t)(p[0] | p[1] << 8);
        } else {
            for (int i = 0; i < n && p < end; i++, p++) buf[i] = (int16_t)((*p - 128) << 8);
        }
    }
    fe_audio_queue(FE_MOVIE, buf, n);
    free(buf);
}

/* ---------------- player ---------------- */
static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t u32(const uint8_t *p) { return u16(p) | (uint32_t)u16(p + 2) << 16; }

int mve_play(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) { dtu_log("[mve] cannot open %s", path); return 0; }
    uint8_t hdr[26];
    if (fread(hdr, 1, 26, f) != 26 || memcmp(hdr, "Interplay MVE File\x1A", 19)) {
        dtu_log("[mve] %s: not an MVE file", path);
        fclose(f);
        return 0;
    }
    mve m;
    memset(&m, 0, sizeof m);
    m.delta = DATA(DPCM_TABLE);
    double frame_time = 1.0 / 15, t0 = 0;
    long frames = 0;
    int aborted = 0, done = 0;
    uint8_t *chunk = NULL;
    size_t chunk_cap = 0;
    video_movie_begin();

    while (!done) {
        uint8_t ch[4];
        if (fread(ch, 1, 4, f) != 4) break;
        size_t clen = u16(ch);
        if (clen > chunk_cap) { free(chunk); chunk_cap = clen + 65536; chunk = malloc(chunk_cap); if (!chunk) break; }
        if (fread(chunk, 1, clen, f) != clen) break;
        for (size_t q = 0; q + 4 <= clen && !done; ) {
            int olen = u16(chunk + q), op = chunk[q + 2], ver = chunk[q + 3];
            const uint8_t *d = chunk + q + 4;
            q += 4;
            if (q + (size_t)olen > clen) { done = 1; break; }
            q += (size_t)olen;
            switch (op) {
            case 0x00: done = 1; break;                              /* end of stream */
            case 0x02:                                               /* timer */
                if (olen >= 6) frame_time = u32(d) * (double)u16(d + 4) / 1e6;
                if (frame_time <= 0 || frame_time > 1) frame_time = 1.0 / 15;
                break;
            case 0x03:                                               /* sound setup */
                if (olen < 6) break;
                {
                    unsigned fl = u16(d + 2);
                    m.a_rate = u16(d + 4);
                    m.a_ch = (fl & 1) + 1;
                    m.a_16 = (fl >> 1) & 1;
                    m.a_dpcm = ver >= 1 && (fl & 4);
                    if (m.a_dpcm) m.a_16 = 1;
                    if (!m.a_open && m.a_rate > 0) m.a_open = fe_audio_open(FE_MOVIE, m.a_rate, m.a_ch);
                    dtu_log("[mve] sound %d Hz, %s, %s", m.a_rate, m.a_ch == 2 ? "stereo" : "mono",
                            m.a_dpcm ? "16-bit DPCM" : m.a_16 ? "16-bit" : "8-bit");
                }
                break;
            case 0x04:                                               /* start sound */
                if (m.a_open && !m.a_started) { fe_audio_start(FE_MOVIE); m.a_started = 1; }
                break;
            case 0x05:                                               /* video buffers */
                if (olen < 4) break;
                m.bw = u16(d); m.bh = u16(d + 2);
                if (ver >= 2 && olen >= 8 && u16(d + 6)) { dtu_log("[mve] true-colour movie not supported"); done = 1; break; }
                m.w = m.bw * 8; m.h = m.bh * 8;
                free(m.cur); free(m.prev);
                m.cur = calloc((size_t)m.w * m.h, 1);
                m.prev = calloc((size_t)m.w * m.h, 1);
                if (!m.cur || !m.prev) done = 1;
                break;
            case 0x07:                                               /* show frame */
                if (!m.cur) break;
                if (frames == 0) {
                    t0 = plat_now();
                    dtu_log("[mve] %s: %dx%d, %.1f frames/s", path, m.w, m.h, 1.0 / frame_time);
                } else {
                    double due = t0 + frames * frame_time;
                    for (;;) {
                        double left = due - plat_now();
                        if (left <= 0) break;
                        plat_sleep_ms(left > 0.004 ? (int)(left * 1000) - 2 : 1);
                        if (left > 0.004 && io_movie_poll()) { aborted = 1; break; }
                    }
                }
                video_movie_frame(m.cur, m.w, m.h, m.pal_dirty ? m.pal : NULL);
                m.pal_dirty = 0;
                frames++;
                break;
            case 0x08: audio_frame(&m, d, olen, 0); break;
            case 0x09: audio_frame(&m, d, olen, 1); break;
            case 0x0C:                                               /* palette */
                if (olen >= 4) {
                    int start = u16(d), count = u16(d + 2);
                    if (start + count > 256) count = 256 - start;
                    if (count > 0 && olen >= 4 + 3 * count) { memcpy(m.pal + start * 3, d + 4, (size_t)count * 3); m.pal_dirty = 1; }
                }
                break;
            case 0x0F:                                               /* decoding map */
                free(m.map);
                m.map = malloc((size_t)olen + 1);
                if (m.map) { memcpy(m.map, d, (size_t)olen); m.map_len = olen; }
                break;
            case 0x11: if (m.map) decode_frame(&m, d, olen); break;  /* video data */
            default: break;                                          /* 0x01 end of chunk, 0x0A mode, 0x13, 0x15 */
            }
            if (aborted) done = 1;
        }
        if (io_movie_poll()) aborted = 1;
        if (aborted) done = 1;
    }
    /* let the sound finish */
    if (m.a_open) {
        while (!aborted && m.a_started && fe_audio_queued(FE_MOVIE) > 0.01) {
            plat_sleep_ms(10);
            if (io_movie_poll()) aborted = 1;
        }
        fe_audio_close(FE_MOVIE);
    }
    dtu_log("[mve] %ld frames%s", frames, aborted ? ", stopped by a key" : "");
    fclose(f);
    free(chunk); free(m.cur); free(m.prev); free(m.map);
    return aborted;
}

/* replacement for the game's movie function 0x96439 (eax = file name):
   via x86_int 0xEF0002 from the translated code */
void mve_play_guest(dtu_regs *r)
{
    char hp[1024];
    const char *name = PTR(EAX);
    *(uint32_t *)DATA(0x142258) = 0;
    if (!dos_host_path(name, hp, sizeof hp, 1)) { dtu_log("[mve] %s not found", name); return; }
    dtu_log("[mve] playing %s", name);
    if (mve_play(hp)) *(uint32_t *)DATA(0x142254) = 0;   /* a key skips the rest of the intro, as in the original */
}
