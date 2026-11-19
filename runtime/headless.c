/* Undermountain Enhanced - headless test driver: no window, logs and PNG screenshots */
#include "dtu.h"
#include "platform.h"
#include "version.h"
#include <stdlib.h>
#include <string.h>

extern double io_run_seconds, io_shot_every;
void io_add_key(double t, int sc, double dur);

static void usage(void)
{
    fprintf(stderr,
        "usage: dtu-headless [options] [-- game arguments]\n"
        "  --game DIR     installed game directory (DTU.CFG, data files)\n"
        "  --cd DIR       cd-rom contents (the directory that holds DTU\\)\n"
        "  --out DIR      where logs and screenshots go\n"
        "  --seconds N    stop after N seconds (default 20)\n"
        "  --shots N      screenshot every N seconds when the screen changed (default 2)\n"
        "  --key T:SC[:D] press scan code SC (hex, 1xx = E0 prefix) at T seconds for D seconds (0.15); repeatable\n"
        "  --fps N        frame limit (default 60, 0 = none)\n"
        "  --mouse T:X,Y,B  move the mouse to X,Y (640x480) with buttons B (1 left, 2 right) at T seconds\n"
        "  --no-sprite-fix  draw sprites at the camera where they are (original behaviour)\n"
        "  --affine-objects  objects without perspective correction (original behaviour)\n"
        "  --orig-weapon-light  weapon light during attacks as in DOS (overflows: black weapon)\n"
        "  --frame-shots T0:T1  a screenshot of every complete frame between T0 and T1 seconds\n"
        "  --no-menu-pointer  text menus without a mouse pointer (original behaviour)\n"
        "  --no-dialog-keys  no number keys for conversation answers (original behaviour)\n"
        "  --steps        model animations in steps (original behaviour)\n"
        "  --near-detail  original detail distances (linear/flat drawing further away)\n"
        "  --persp-depth N  perspective drawing up to N segments deep (test; default 99)\n"
        "  --quiet        less logging\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int gi = argc;
    char logpath[1200];
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--")) { gi = i + 1; break; }
        else if (!strcmp(argv[i], "--game") && i + 1 < argc) snprintf(dtu_game_dir, sizeof dtu_game_dir, "%s", argv[++i]);
        else if (!strcmp(argv[i], "--cd") && i + 1 < argc) snprintf(dtu_cd_dir, sizeof dtu_cd_dir, "%s", argv[++i]);
        else if (!strcmp(argv[i], "--out") && i + 1 < argc) snprintf(dtu_out_dir, sizeof dtu_out_dir, "%s", argv[++i]);
        else if (!strcmp(argv[i], "--seconds") && i + 1 < argc) io_run_seconds = atof(argv[++i]);
        else if (!strcmp(argv[i], "--shots") && i + 1 < argc) io_shot_every = atof(argv[++i]);
        else if (!strcmp(argv[i], "--fps") && i + 1 < argc) io_fps_limit = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--physics-fps") && i + 1 < argc) io_physics_fps = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--menu-key-delay") && i + 1 < argc) io_menu_key_delay = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--mouse") && i + 1 < argc) {
            double t; int x, y, b;
            if (sscanf(argv[++i], "%lf:%d,%d,%d", &t, &x, &y, &b) == 4) io_add_mouse(t, x, y, b); else usage();
        }
        else if (!strcmp(argv[i], "--key") && i + 1 < argc) {
            double t; int sc;
            double d = 0.15;
            if (sscanf(argv[++i], "%lf:%x:%lf", &t, &sc, &d) >= 2) io_add_key(t, sc, d); else usage();
        }
        else if (!strcmp(argv[i], "--quiet")) dtu_verbose = 0;
        else if (!strcmp(argv[i], "--verbose") && i + 1 < argc) dtu_verbose = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--persp-depth") && i + 1 < argc) { extern uint32_t dtu_far_detail; dtu_far_detail = (uint32_t)atoi(argv[++i]); }
        else if (!strcmp(argv[i], "--near-detail")) { extern uint32_t dtu_far_detail; dtu_far_detail = 0; }
        else if (!strcmp(argv[i], "--frame-shots") && i + 1 < argc) { extern double io_frame_shots[2]; if (sscanf(argv[++i], "%lf:%lf", &io_frame_shots[0], &io_frame_shots[1]) != 2) usage(); }
        else if (!strcmp(argv[i], "--no-menu-pointer")) { extern uint32_t dtu_menu_mouse; dtu_menu_mouse = 0; }
        else if (!strcmp(argv[i], "--no-ladder-fix")) io_ladder_fix = 0;
        else if (!strcmp(argv[i], "--orig-music")) { extern int sos_music_hold; sos_music_hold = 0; }
        else if (!strcmp(argv[i], "--orig-hit-sounds")) { extern uint32_t dtu_sound_fallback; dtu_sound_fallback = 0; }
        else if (!strcmp(argv[i], "--step-gap") && i + 1 < argc) { extern uint32_t dtu_step_gap; dtu_step_gap = (uint32_t)atoi(argv[++i]); }
        else if (!strcmp(argv[i], "--orig-creatures")) { extern uint32_t dtu_creature_floor; dtu_creature_floor = 0; }
        else if (!strcmp(argv[i], "--orig-lines")) { extern uint32_t dtu_double_lines; dtu_double_lines = 1; }
        else if (!strcmp(argv[i], "--no-dialog-keys")) { extern uint32_t dtu_dialog_keys; dtu_dialog_keys = 0; }
        else if (!strcmp(argv[i], "--steps")) { extern uint32_t dtu_smooth_anim; dtu_smooth_anim = 0; }
        else if (!strcmp(argv[i], "--orig-weapon-light")) { extern uint32_t dtu_weapon_light; dtu_weapon_light = 0; }
        else if (!strcmp(argv[i], "--affine-objects")) { extern uint32_t dtu_obj_affine; dtu_obj_affine = 1; }
        else if (!strcmp(argv[i], "--no-sprite-fix")) { extern uint32_t dtu_sprite_cull_on; dtu_sprite_cull_on = 0; }
        else usage();
    }
    plat_mkdir(dtu_out_dir);
    snprintf(logpath, sizeof logpath, "%s/dtu.log", dtu_out_dir);
    dtu_logf = fopen(logpath, "w");
    if (!dtu_logf) dtu_logf = stderr;
    dtu_log("Undermountain Enhanced %s (headless), build %s", DTU_VERSION, DTU_BUILD);

    dtu_core_init(argc - gi, argv + gi);
    int code = dtu_core_run();
    dtu_screenshot("exit");
    if (dtu_logf != stderr) fclose(dtu_logf);
    return code;
}

/* sound: written to <out>/audio-N.wav (movies) and <out>/game.wav, so the
   decoders and the mixer can be checked; playback runs in real time */
typedef struct { FILE *f; int rate, ch; uint32_t bytes; double t0; } wavout;
static wavout wv[2];
static int wav_n;
static void wav_header(wavout *w)
{
    uint8_t h[44];
    uint32_t v;
    memcpy(h, "RIFF", 4); v = 36 + w->bytes; memcpy(h + 4, &v, 4); memcpy(h + 8, "WAVEfmt ", 8);
    v = 16; memcpy(h + 16, &v, 4);
    uint16_t s = 1; memcpy(h + 20, &s, 2); s = (uint16_t)w->ch; memcpy(h + 22, &s, 2);
    v = (uint32_t)w->rate; memcpy(h + 24, &v, 4); v = (uint32_t)(w->rate * w->ch * 2); memcpy(h + 28, &v, 4);
    s = (uint16_t)(w->ch * 2); memcpy(h + 32, &s, 2); s = 16; memcpy(h + 34, &s, 2);
    memcpy(h + 36, "data", 4); memcpy(h + 40, &w->bytes, 4);
    fseek(w->f, 0, SEEK_SET); fwrite(h, 1, 44, w->f); fseek(w->f, 0, SEEK_END);
}
int fe_audio_open(int st, int rate, int channels)
{
    char p[1200];
    wavout *w = &wv[st];
    if (st == FE_MOVIE) snprintf(p, sizeof p, "%s/audio-%d.wav", dtu_out_dir, wav_n++);
    else snprintf(p, sizeof p, "%s/game.wav", dtu_out_dir);
    w->f = fopen(p, "wb");
    if (!w->f) return 0;
    w->rate = rate; w->ch = channels; w->bytes = 0; w->t0 = -1;
    wav_header(w);
    return 1;
}
void fe_audio_queue(int st, const int16_t *samples, int count)
{
    wavout *w = &wv[st];
    if (!w->f) return;
    fwrite(samples, 2, (size_t)count, w->f);
    w->bytes += (uint32_t)count * 2;
    if (st == FE_GAME && (w->bytes & 0xFFFFF) < (uint32_t)count * 2) wav_header(w);   /* keep the file readable */
}
void fe_audio_start(int st) { wv[st].t0 = plat_now(); }
double fe_audio_queued(int st)
{
    wavout *w = &wv[st];
    if (!w->f || w->t0 < 0) return 0;
    double q = w->bytes / (2.0 * w->ch * w->rate) - (plat_now() - w->t0);
    return q > 0 ? q : 0;
}
/* a stand-in for an audio device that asks for sound: a thread that takes it in real time, 20 ms ahead */
static void (*pull_fn)(int16_t *, int);
static volatile int pull_run, pull_alive;
static void pull_thread(void *arg)
{
    wavout *w = &wv[FE_GAME];
    uint64_t done = 0;
    static int16_t buf[2 * 1024];
    (void)arg;
    while (pull_run) {
        double t0 = w->t0;
        if (t0 >= 0) {
            uint64_t want = (uint64_t)((plat_now() - t0 + 0.02) * w->rate);
            while (done < want && pull_run) {
                int k = want - done > 1024 ? 1024 : (int)(want - done);
                pull_fn(buf, k);
                fe_audio_queue(FE_GAME, buf, k * w->ch);
                done += (uint64_t)k;
            }
        }
        plat_sleep_ms(5);
    }
    pull_alive = 0;
}
int fe_audio_open_pull(int st, int rate, int channels, void (*pull)(int16_t *out, int frames))
{
    if (st != FE_GAME || !fe_audio_open(st, rate, channels)) return 0;
    pull_fn = pull; pull_run = 1; pull_alive = 1;
    plat_thread(pull_thread, NULL, 256 * 1024);
    return 1;
}
void fe_audio_close(int st)
{
    wavout *w = &wv[st];
    if (st == FE_GAME && pull_alive) { pull_run = 0; while (pull_alive) plat_sleep_ms(1); }
    if (w->f) { wav_header(w); fclose(w->f); w->f = NULL; }
}
