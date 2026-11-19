/* Undermountain Enhanced - replacements for renderer code that the recompiler cannot run as is */
#include "dtu.h"
#include <string.h>

/* Scaled row of a bitmap with a transparent colour (0x104968, set up by 0x1059F4).
   The original speeds this up with self-modifying code: the setup routine
   patches an unrolled loop of 319 steps (0x1049A5..0x1059E5, 13 bytes each),
   writing "mov ecx, ebx" or "mov ecx, edx" into every step so that each source
   pixel gets the short or the long run length (Bresenham). In the recompiled
   code those bytes are x64 code, the check for the 0x8B opcode fails (int 3 at
   0x105A73) and the patches never happen, so every pixel got the short run.
   Here the setup stores the pattern and the draw routine applies it.

   Variables (all at 0x1512xx): 00 transparent colour (byte), 01 error term,
   05 first run, 09 error step, 0D error correction, 11 last run, 15 number of
   middle pixels, 19 short run, 1D source, 21 destination. */
static int32_t rd32(uint32_t a) { int32_t v; memcpy(&v, DATA(a), 4); return v; }
static void wr32(uint32_t a, int32_t v) { memcpy(DATA(a), &v, 4); }

static struct { int32_t err, step, corr, run; uint32_t count; } pat;

void render_scale_setup(void)
{
    pat.err = rd32(0x151201);
    pat.step = rd32(0x151209);
    pat.corr = rd32(0x15120D);
    pat.run = rd32(0x151219);
    pat.count = (uint32_t)rd32(0x151215);
    if (pat.count > 319) pat.count = 319;            /* the unrolled loop has 319 steps */
    wr32(0x151229, pat.run);                          /* as the original leaves them */
    wr32(0x15122D, pat.run + 1);
}

void render_scale_draw(void)
{
    const uint8_t *src = PTR((uint32_t)rd32(0x15121D));
    uint8_t *dst = PTR((uint32_t)rd32(0x151221));
    uint8_t t = *(uint8_t *)DATA(0x151200), px;
    int32_t first = rd32(0x151205), last = rd32(0x151211), err = pat.err;

    px = *src++;
    if (px != t && first > 0) memset(dst, px, (size_t)first);
    dst += first;
    for (uint32_t i = 0; i < pat.count; i++) {
        int32_t run = pat.run;
        px = *src++;
        err += pat.step;
        if (err > 0) { run++; err -= pat.corr; }
        if (px != t && run > 0) memset(dst, px, (size_t)run);
        dst += run;
    }
    px = *src;
    if (px != t && last > 0) memset(dst, px, (size_t)last);
}

/* Smooth model animation. Polygon models (creatures, and in the first-person
   view your own model with the weapon) have key frames: per frame one set of
   angles per submodel (pm+0x5C, pm+0 submodels) and for some models a root
   offset (pm+0x84, flag pm+0xB4 bit 0). The game steps the frame number
   (object +0x172, sequence type +0x170, ranges in the table at 0x13D9E0) 15
   times per second or slower and draws each frame unchanged until the next,
   so movements go in steps. Here the drawing (draw_polygon_model 0x727A5,
   called from the object renderer 0x6746A, which hands us the object in
   dtu_anim_obj) gets angles and offset between the current frame and the next
   one of the same sequence, by the time since the frame started relative to
   the measured interval between frames. The game logic itself is unchanged. */
#include "platform.h"
extern uint32_t dtu_anim_obj, dtu_smooth_anim;

#define OBJ_FIRST 0x2B0614
#define OBJ_SIZE 0x1DF
#define MAXOBJ 1024
/* Large tables go to a section of their own: in the Windows build everything in
   the standard sections must stay below 0xA0000 (the DOS area, x64/dosmem.asm). */
#define BIG __attribute__((section("dtubig")))
static struct { uint32_t pm; int32_t frame; int type, steps, fresh, loops; double t, period; } an[MAXOBJ] BIG;

static int32_t g32(uint32_t a) { int32_t v; memcpy(&v, PTR(a), 4); return v; }
static int seq_bound(int type, int end) { int16_t v; memcpy(&v, DATA(0x13D9E0 + 2 * type + 2 * end), 2); return v; }

/* fraction (0..1) of the way from frame to frame + 1 for the object being drawn, 0 = none.
   Only for objects seen stepping (or just starting a new sequence): some stand still on
   a frame for a long time, and the next frame must not be shown before it comes. */
static double anim_fraction(uint32_t pm, int32_t frame, int32_t *next)
{
    uint32_t obj = dtu_anim_obj, first = (uint32_t)(uintptr_t)DATA(OBJ_FIRST);
    if (!dtu_smooth_anim || !obj || frame < 0 || obj < first || (obj - first) % OBJ_SIZE) return 0;
    uint32_t idx = (obj - first) / OBJ_SIZE;
    if (idx >= MAXOBJ) return 0;
    int32_t fr = g32(obj + 0x172);
    int type = (int16_t)(g32(obj + 0x170) & 0xFFFF);
    if (fr != frame || type < 0 || type > 18) return 0;
    double t = plat_now();
    __typeof__(an[0]) *s = &an[idx];
    int start = seq_bound(type, 0), end = seq_bound(type, 1);
    if (s->pm != pm) {                                  /* another object in this slot */
        s->pm = pm; s->type = type; s->frame = fr; s->t = t; s->steps = 0; s->fresh = 0; s->loops = 0;
    } else if (s->type == type && fr == start && s->frame == end - 1 && fr != s->frame) {   /* a looping sequence wrapped */
        double iv = t - s->t;
        if (iv > 0.02 && iv < 0.5) s->period = s->period * 0.6 + iv * 0.4;
        s->frame = fr; s->t = t; s->steps++; s->loops = 1;
    } else if (s->type != type || (fr != s->frame && fr != s->frame + 1)) {   /* new sequence */
        s->fresh = s->type != type && fr == start;
        if (s->type != type) s->loops = 0;
        s->type = type; s->frame = fr; s->t = t; s->steps = 0;
        s->period = 1.0 / 15;                           /* most sequences step 15 times per second */
    } else if (fr == s->frame + 1) {
        double iv = t - s->t;
        if (iv > 0.02 && iv < 0.5) s->period = s->steps || s->period > 0 ? s->period * 0.6 + iv * 0.4 : iv;
        s->frame = fr; s->t = t; s->steps++;
    }
    if (s->period <= 0) s->period = 1.0 / 15;
    if (!s->steps && !s->fresh) return 0;
    *next = fr + 1;
    if (*next >= end) {
        if (!s->loops) return 0;                        /* last frame of the sequence: what follows is not known */
        *next = start;                                  /* seen looping: back to its first frame */
    }
    if (*next >= g32(pm + 0x58)) return 0;
    double f = (t - s->t) / s->period;
    if (f > 2) return 0;                                /* stopped stepping */
    return f <= 0 ? 0 : f >= 1 ? 1 : f;
}

static uint8_t *anim_buf(size_t size)
{
    static uint8_t *ring[8]; static size_t cap[8]; static int k;
    k = (k + 1) & 7;
    if (cap[k] < size) { ring[k] = plat_alloc32((uint32_t)(size + 4095) & ~4095u); cap[k] = ring[k] ? (size + 4095) & ~4095u : 0; }
    return ring[k];
}

/* 0x72A51: angles for frame EDX of model EAX (original: pm+0x5C + frame * submodels * 6) */
void render_anim_angles(dtu_regs *r)
{
    uint32_t pm = EAX;
    int32_t frame = (int32_t)EDX, n = g32(pm);
    uint32_t base = (uint32_t)g32(pm + 0x5C) + (uint32_t)(frame * n * 6);
    EDX = (uint32_t)(frame * n * 6);
    EAX = base;
    int32_t next;
    double f = anim_fraction(pm, frame, &next);
    if (f <= 0 || n <= 0 || n > 4096) return;
    uint8_t *out = anim_buf((size_t)n * 6);
    if (!out) return;
    const int16_t *a = PTR(base), *b = PTR((uint32_t)g32(pm + 0x5C) + (uint32_t)(next * n * 6));
    int16_t *o = (int16_t *)out;
    for (int i = 0; i < n * 3; i++) o[i] = (int16_t)(a[i] + (int)((int16_t)(b[i] - a[i]) * f));   /* angles wrap at 16 bits */
    EAX = (uint32_t)(uintptr_t)out;
}

/* 0x729F3: root offset of the model, EAX = pm+0x84 table, EDX = (frame + 1) * 12; returns the vector in EAX.
   The model is at [esp+0x54] of draw_polygon_model (the guest stack starts after the saved registers). */
void render_anim_offset(dtu_regs *r)
{
    uint32_t tab = EAX, ofs = EDX;
    const uint32_t *stk = (const uint32_t *)r + 10;
    uint32_t pm = stk[0x54 / 4];
    int32_t frame = (int32_t)(ofs / 12) - 1;
    EAX = tab + ofs;
    EDX = (uint32_t)(frame + 1);
    int32_t next;
    double f = anim_fraction(pm, frame, &next);
    if (f <= 0 || next < frame) return;                 /* not across the wrap of a loop: the offset starts over there */
    const int32_t *a = PTR(tab + ofs), *b = PTR(tab + (uint32_t)(next + 1) * 12);
    int32_t *o = (int32_t *)anim_buf(12);
    if (!o) return;
    for (int i = 0; i < 3; i++) o[i] = a[i] + (int32_t)((double)(b[i] - a[i]) * f);
    EAX = (uint32_t)(uintptr_t)o;
}

/* ---- creatures on the floor ----
   The engine comes from Descent, where everything flies. Undermountain keeps a
   creature up by a ray down to the floor (0x6C30E) against a "bottom" per
   animation frame (pm+0x9C): when the bottom is below the floor the creature is
   pushed up at once (0x6C7AA), above it it only falls slowly and from 1.0 on,
   and its collision sphere (radius about 10 for a kobold, its feet only 3..6
   below the centre) often holds it up. While walking the bottom changes from
   frame to frame, so the creature stays at the height of the frame with the
   lowest feet and the frames with lifted legs are drawn in the air; and the
   bottom values do not match the drawn model well (up to about 3 off).
   Here only the drawing is moved: the lowest point of the creature as drawn in
   the previous frame (measured on the rotated points, 0xF080C) is put on the
   floor that the game's own ray found (0x6C742). Game logic and collisions are
   unchanged. Not for flying creatures, nor far above the floor (jumps, falls). */
extern uint32_t dtu_creature_floor;
static struct { int32_t floor, dist; int has_floor; double min, shift, posy, depth; int measuring, has_depth; } ft[MAXOBJ] BIG;

static int creature_index(uint32_t obj)
{
    uint32_t first = (uint32_t)(uintptr_t)DATA(OBJ_FIRST);
    if (!obj || obj < first || (obj - first) % OBJ_SIZE) return -1;
    uint32_t idx = (obj - first) / OBJ_SIZE;
    if (idx >= MAXOBJ || *(const uint8_t *)PTR(obj + 4) != 2) return -1;
    return (int)idx;
}

/* 0x6C742: EBP = object, EAX = height of its bottom above the floor (from 0x6C30E) */
void render_floor_note(dtu_regs *r)
{
    int idx = creature_index(EBP);
    if (idx < 0) return;
    uint32_t pm = (uint32_t)g32((uint32_t)(uintptr_t)DATA(0x3880D0) + 4 * (uint32_t)g32(EBP + 0x162));
    int32_t fr = (int16_t)(g32(EBP + 0x172) & 0xFFFF);
    if (!pm || fr < 0 || fr >= g32(pm + 0x58)) { ft[idx].has_floor = 0; return; }
    int32_t dist = (int32_t)EAX, bottom = g32((uint32_t)g32(pm + 0x9C) + 4u * (uint32_t)fr);
    ft[idx].dist = dist;
    ft[idx].has_floor = dist > -0x40000 && dist < 0x70000;          /* 8.0 = nothing hit */
    ft[idx].floor = g32(EBP + 0x18) - dist - bottom;
}

static int creature_walks(uint32_t obj)
{
    if (g32(obj + 0x88) & 0x200880) return 0;                       /* exempt from gravity (as 0x6C77A) */
    if ((int16_t)(g32(obj + 0x170) & 0xFFFF) != 9) {                 /* flying kind (as 0x6C770) */
        uint32_t id = *(const uint8_t *)PTR(obj + 5);
        if (*(const uint8_t *)DATA(0x38E3DC + id * 242) & 0x20) return 0;
    }
    return g32(obj + 0x30) > 0xF000;                                  /* upright: up vector nearly (0, 1, 0) */
}

/* 0x72A0F (draw_polygon_model): [esp+0x30] is where the model is drawn, [esp+0x54] the model */
void render_floor_draw(dtu_regs *r)
{
    uint32_t *stk = (uint32_t *)r + 10, obj = dtu_anim_obj;
    int idx = creature_index(obj);
    if (idx < 0 || !dtu_creature_floor) return;
    uint32_t pm = stk[0x54 / 4];
    if (pm != (uint32_t)g32((uint32_t)(uintptr_t)DATA(0x3880D0) + 4 * (uint32_t)g32(obj + 0x162))) return;
    __typeof__(ft[0]) *s = &ft[idx];
    if (s->measuring && s->min < 1e8) { s->depth = s->posy - (s->min + s->shift); s->has_depth = 1; }
    s->measuring = 0;
    double posy = g32(obj + 0x18) / 65536.0, shift = 0;
    if (creature_walks(obj) && s->has_floor && s->dist < 0x40000) {
        if (s->has_depth) {
            shift = (posy - s->depth) - s->floor / 65536.0;          /* > 0: lowest point above the floor */
            if (shift > 8) shift = 8;
            if (shift < -3) shift = -3;
        }
        int32_t *y = (int32_t *)&stk[0x34 / 4];
        *y -= (int32_t)(shift * 65536);
        s->measuring = 1;
    } else s->has_depth = 0;
    s->shift = shift; s->posy = posy; s->min = 1e9;
}

/* 0xF080C: a point of a model was rotated, EDI = the point (view coordinates) */
void render_floor_point(dtu_regs *r)
{
    int idx = creature_index(dtu_anim_obj);
    if (idx < 0 || !ft[idx].measuring) return;
    const int32_t *p = PTR(EDI), *pos, *m;
    if (*(const int32_t *)DATA(0x14C1D0) > 0) { pos = (const int32_t *)DATA(0x14BE10); m = pos + 3; }   /* camera: instance stack level 0 */
    else { pos = (const int32_t *)DATA(0x14D3AC); m = (const int32_t *)DATA(0x14D3E0); }
    double wy = pos[1] / 65536.0;
    for (int i = 0; i < 3; i++) {
        double rx = m[i * 3] / 65536.0, ry = m[i * 3 + 1] / 65536.0, rz = m[i * 3 + 2] / 65536.0, l2 = rx * rx + ry * ry + rz * rz;
        if (l2 > 0) wy += p[i] / 65536.0 / l2 * ry;
    }
    if (wy < ft[idx].min) ft[idx].min = wy;
}

/* ---- sprites at the camera ----
   0xEAA01 (g3_draw_bitmap, after the point is rotated and projected): ESI = the
   point (view coordinates x, y, z at +0, +4, +8; screen position at +0x18, +0x1C),
   EBX = half width and ECX = half height of the sprite in the world. Returns EAX = 1
   to skip it.
   Water splashes are spawned at your feet, right at the camera (depth about 1 for
   a sprite 30 wide): drawn there, one texel filled the whole view for a frame (in
   DOS as well) or nothing of it could be seen. Version 0.2.16 skipped every sprite
   closer than half its width, so walking forward showed no splashes at all. Now
   such a sprite is drawn as if it were a little further away along the view
   (SPRITE_NEAR times its half width), at the same height and side: the splash
   appears in the lower part of the view, in front of your feet. */
#define SPRITE_NEAR 0.6
void render_sprite_check(dtu_regs *r)
{
    int32_t *q = (int32_t *)PTR(ESI);
    double z = q[2] / 65536.0, w = (int32_t)EBX / 65536.0, nz = w * SPRITE_NEAR;
    EAX = 0;
    if (w <= 0 || z <= 0 || z >= nz) return;
    double cw2 = g32((uint32_t)(uintptr_t)DATA(0x14D424)) / 65536.0, ch2 = g32((uint32_t)(uintptr_t)DATA(0x14D428)) / 65536.0;
    double scx = g32((uint32_t)(uintptr_t)DATA(0x14D410)) / 65536.0, scy = g32((uint32_t)(uintptr_t)DATA(0x14D414)) / 65536.0;
    double x = q[0] / 65536.0, y = q[1] / 65536.0;
    q[2] = (int32_t)(nz * 65536);
    q[6] = (int32_t)((cw2 + x * cw2 * scx / nz) * 65536);
    q[7] = (int32_t)((ch2 - y * ch2 * scy / nz) * 65536);
    { extern uint32_t dtu_sprite_culls; dtu_sprite_culls++; }
}

/* ---- footsteps of creatures ----
   0x9FD68 (object movement, 0x9F...): a walking or running creature (sequence 2
   or 5) plays a footstep each time the movement code runs while its animation
   frame is the sequence's sound frame (table at 0x13D9DE). The animation steps
   about 15 times per second, so at 60 fps that frame lasts about four game frames
   and the footstep was started four times in a row (with a random variant each
   time); at DOS frame rates (about 20 fps) once. Sequences 6 and 7 already have
   a timer of their own. Here a creature's footstep is skipped if its previous one
   started less than dtu_step_gap ms ago. EBP = the object; EAX = 1 to skip. */
extern uint32_t dtu_step_gap;
static double step_time[MAXOBJ] BIG;
void render_step_sound(dtu_regs *r)
{
    uint32_t first = (uint32_t)(uintptr_t)DATA(OBJ_FIRST), obj = EBP;
    EAX = 0;
    if (obj < first || (obj - first) % OBJ_SIZE || (obj - first) / OBJ_SIZE >= MAXOBJ) return;
    uint32_t idx = (obj - first) / OBJ_SIZE;
    double t = plat_now();
    if (t - step_time[idx] < dtu_step_gap / 1000.0 && t >= step_time[idx]) {
        EAX = 1;
        if (dtu_verbose >= 2) dtu_log("[step] t=%.3f object %u: footstep skipped", t, idx);
        return;
    }
    if (dtu_verbose >= 2) dtu_log("[step] t=%.3f object %u: footstep", t, idx);
    step_time[idx] = t;
}

/* digi_link_sound_to_object (0x2E631) plays a sound that follows an object
   through a "sound object", of which there are only 16 (41 bytes each from
   0x1B7AEC, byte +2 = flags, 0 = free). Looping ambient sounds (water, torches,
   wind) keep sound objects for as long as they are in the level, so in many
   places all 16 are taken and every sound linked to an object is silently
   dropped: the weapon's hit sound and the creature's pain sound in melee
   (0x278B5) are linked to the creature, while its death sound is also played
   at a position, which needs no sound object. That is why a killing blow was
   sometimes heard with its hit sound and sometimes without, depending on the
   place (in DOS as well). Here a one-shot sound (EBX = 0) for a valid object
   that would find no free sound object is played at the object's position
   instead (0x2E957, as digi_link_sound_to_pos does): dtu_sound_fb_now = 1.
   The melee code plays the death sound both at the position and linked to the
   creature; if that same sound has just been started at a position, the linked
   copy is left out (dtu_sound_fb_now = 2) rather than played twice at once. The
   same goes for the same sound for the same object within 0.15 s: right after
   a killing blow the swing still touches the creature for a few frames (more
   of them at 60 fps than in DOS), and each time its hit sounds were started.
   EAX = sound, EDX = object number. */
extern uint32_t dtu_sound_fb_now;
static int last_pos_sound = -1;
static struct { int snd, obj; double t; } fb_recent[8];     /* the last sounds played this way */
static unsigned fb_next;
static double last_pos_time;
/* 0x2E957 (play a sound at a position) is starting sound EAX */
void render_sound_at_pos(dtu_regs *r)
{
    last_pos_sound = (int)(int16_t)EAX;
    last_pos_time = plat_now();
}
#define SNDOBJ_FIRST 0x1B7AEC
#define SNDOBJ_SIZE 41
#define SNDOBJ_MAX 16
void render_sound_fallback(dtu_regs *r)
{
    int16_t obj = (int16_t)EDX;
    dtu_sound_fb_now = 0;
    if (EBX != 0 || obj < 0 || obj > *(int32_t *)DATA(0x13D324) || obj >= MAXOBJ) return;     /* looping, or no such object */
    for (int i = 0; i < SNDOBJ_MAX; i++)
        if (*(uint8_t *)DATA(SNDOBJ_FIRST + SNDOBJ_SIZE * i + 2) == 0) return;               /* a free sound object: as before */
    double t = plat_now();
    int snd = (int)(int16_t)EAX;
    int again = snd == last_pos_sound && t - last_pos_time < 0.05 && t >= last_pos_time;
    for (int i = 0; i < 8; i++)
        if (fb_recent[i].t > 0 && fb_recent[i].snd == snd && fb_recent[i].obj == obj && t - fb_recent[i].t < 0.15 && t >= fb_recent[i].t) again = 1;
    if (again) {
        dtu_sound_fb_now = 2;
        if (dtu_verbose >= 2) dtu_log("[snd] t=%.3f sound %d for object %d: no free sound object, just started already", t, snd, obj);
        return;
    }
    fb_recent[fb_next % 8].snd = snd; fb_recent[fb_next % 8].obj = obj; fb_recent[fb_next % 8].t = t;
    fb_next++;
    dtu_sound_fb_now = 1;
    if (dtu_verbose >= 2) dtu_log("[snd] t=%.3f sound %d for object %d: no free sound object, played at its position", t, snd, obj);
}
