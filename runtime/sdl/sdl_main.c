/* Undermountain Enhanced - SDL3 front end: window, VGA CRT shader, keyboard and mouse.
 *
 * Threads: the main thread owns the window (events, drawing); the recompiled
 * game runs in its own thread and is driven by the runtime's timer. Keys go
 * to the game as XT scancodes through its own IRQ1 handler; the mouse is
 * served through int 33h. */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../dtu.h"
#include "../platform.h"
#include "crt_gl.h"
#include "pad.h"
#include "../patch13.h"
#ifdef DTU_SELFBUILD
#include "../selfbuild.h"
#endif
#include "../version.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define APP_NAME "Undermountain Enhanced " DTU_VERSION " (build " DTU_BUILD ")"
#define INI_NAME "dtu-enhanced.ini"

typedef struct {
    int fullscreen, width, height, shader, scale, aspect, vsync;
    float mouse_speed, mouse_vertical;
    int menu_pointer;
    int volume;
    int fps_limit, show_fps, physics_fps, menu_key_delay, ladder_fix, sprite_fix, persp_objects, far_detail, weapon_light, smooth_anim, dialog_keys, full_3d, creature_floor, step_gap, hit_sounds, music_hold;
    char args[256], cd_path[1024];
} Config;

static Config cfg = { 0, 1280, 960, SHADER_CRT_AUTO, 2, 1, 1, 1.0f, 1.25f, 1, 100, 60, 1, 20, 300, 1, 1, 1, 1, 1, 1, 1, 1, 1, 200, 1, 1, "-nosound", "" };
static SDL_Window *win;
static CrtGL *gl;
static SDL_Renderer *ren;          /* fallback without OpenGL 3.3 */
static SDL_Texture *ren_tex;
static SDL_FRect pic = { 0, 0, 640, 480 };
static int game_w = 640, game_h = 480;
static volatile int game_done;
static int game_code;

static const char *INI_TEXT =
    "; Undermountain Enhanced settings\n"
    "[video]\n"
    "; 1 = start in full screen (Alt+Enter or F11 switches while playing)\n"
    "fullscreen = %d\n"
    "; window size in pixels\n"
    "width = %d\n"
    "height = %d\n"
    "; crt-auto, crt-1080p, crt-1440p, crt-4k or none (VGA monitor emulation, from DOSBox Staging)\n"
    "shader = %s\n"
    "; without CRT shader: sharp, linear or nearest\n"
    "scale = %s\n"
    "; 1 = 4:3 picture as on a CRT monitor\n"
    "aspect = %d\n"
    "vsync = %d\n"
    "; frames per second of the game: 0 = no limit. DTU was made for at most\n"
    "; 150 fps; above that its clock runs too fast (turning, movement)\n"
    "fps_limit = %d\n"
    "; 1 = show the frame rate in the window title\n"
    "show_fps = %d\n"
    "[mouse]\n"
    "; the window captures the mouse (click in it); Ctrl+F10 releases it\n"
    "; speed: 1.0 = one game pixel per window pixel\n"
    "speed = %.2f\n"
    "; looking up and down in the game: vertical motion x this (1.0 = as horizontal)\n"
    "vertical = %.2f\n"
    "; 1 = a pointer in the text menus (main menu, Load Game, Options): point and\n"
    ";     click, right button = back; 0 = original (mouse motion moves the selection)\n"
    "menu_pointer = %d\n"
    "[sound]\n"
    "; volume 0-100 (game sound and movies)\n"
    "volume = %d\n"
    "[game]\n"
    "; DTU slows the player down per frame when not walking forward (strafing),\n"
    "; and falling and the hop at the top of a ladder depend on the frame rate.\n"
    "; Movement as at this frame rate, at any frame rate: 20 = as in DOSBox on a\n"
    "; fast PC, 0 = original\n"
    "physics_fps = %d\n"
    "; 1 = if you stay hanging at the top edge of a ladder, hop again (up to 3\n"
    ";     times); 0 = original\n"
    "ladder_fix = %d\n"
    "; game functions that open a screen or toggle something (automap, inventory,\n"
    "; statistics, rest, ...; also after changing keys with F2): a held key does not\n"
    "; repeat, and a second press within this many milliseconds is ignored, so a\n"
    "; screen does not flash open and closed. Movement keeps repeating. 0 = as in DOS\n"
    "menu_key_delay = %d\n"
    "; 1 = a sprite right at the camera (water splashes at your feet) is drawn a\n"
    ";     little further away, in front of you; in the original it filled the whole\n"
    ";     view for a moment, in DOS as well; 0 = original\n"
    "sprite_fix = %d\n"
    "; 1 = textures on objects (tables, beds, creatures) with perspective correction\n"
    ";     and per-vertex lighting, as on walls; 0 = original (they shift with the\n"
    ";     viewing angle)\n"
    "perspective_objects = %d\n"
    "; 1 = full drawing quality up to the view distance; 0 = original (walls a few\n"
    ";     rooms away use a simpler, darker drawing method, so their lighting jumps\n"
    ";     when you walk towards them)\n"
    "far_detail = %d\n"
    "; 1 = your weapon no longer turns black for a moment while you attack (its\n"
    ";     light overflowed, in DOS as well); 0 = original\n"
    "weapon_light = %d\n"
    "; 1 = creatures and your weapon move smoothly between the key frames of\n"
    ";     their animations (15 per second); 0 = original (in steps)\n"
    "smooth_animation = %d\n"
    "; 1 = choose answers in conversations with the keys 1..9 and 0; 0 = original\n"
    "dialog_keys = %d\n"
    "; 1 = the 3D view uses all 480 lines of the screen (sharper); 0 = original\n"
    ";     (half the lines, each one drawn twice). Only with Hires Dungeon Graphics\n"
    ";     on (in the game's options)\n"
    "full_lines_3d = %d\n"
    "; 1 = creatures are drawn with their feet on the floor (in the original they\n"
    ";     often walk in the air, in DOS as well); 0 = original\n"
    "creature_floor = %d\n"
    "; shortest time between two footsteps of one creature, in ms; at 60 fps the\n"
    ";     game started each footstep about four times in a row; 0 = original\n"
    "footstep_gap = %d\n"
    "; 1 = sounds that follow a creature (the weapon's hit sound, its cry of pain)\n"
    ";     are also heard when all 16 sound places are taken by ambient sounds; in\n"
    ";     the original they were then left out (so a killing blow was sometimes\n"
    ";     heard without its hit sound); 0 = original\n"
    "hit_sounds = %d\n"
    "; 1 = while the game loads (a level, a saved game, the menu after dying) the\n"
    ";     music fades out and waits, and fades back in when loading is done,\n"
    ";     instead of stuttering; 0 = original\n"
    "music_hold = %d\n"
    "; arguments for the game (DTU -help lists them)\n"
    "args = %s\n"
    "; folder with the contents of the CD-ROM (the CD root or its folder DTU).\n"
    "; Empty: _CD\\DTU\\_CDFILES next to this program, the CDPath of DTU.CFG,\n"
    "; or any CD drive (also a mounted ISO/CUE image) that holds the game's CD.\n"
    "cd_path = %s\n";

static const char *shader_names[] = { "none", "crt-auto", "crt-1080p", "crt-1440p", "crt-4k" };
/* ---------------- sound ----------------
   One SDL audio stream, opened by the movie player (mve.c) on the game thread;
   SDL streams are thread-safe. */
static SDL_AudioStream *astream[2];
static int a_rate[2], a_ch[2];
int fe_audio_open(int st, int rate, int channels)
{
    SDL_AudioSpec spec = { SDL_AUDIO_S16, channels, rate };
    if (astream[st]) fe_audio_close(st);
    astream[st] = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!astream[st]) { dtu_log("[sound] cannot open audio: %s", SDL_GetError()); return 0; }
    SDL_SetAudioStreamGain(astream[st], cfg.volume / 100.0f);
    a_rate[st] = rate; a_ch[st] = channels;
    return 1;
}
void fe_audio_queue(int st, const int16_t *samples, int count)
{
    if (astream[st]) SDL_PutAudioStreamData(astream[st], samples, count * 2);
}
void fe_audio_start(int st) { if (astream[st]) SDL_ResumeAudioStreamDevice(astream[st]); }
double fe_audio_queued(int st)
{
    if (!astream[st]) return 0;
    int b = SDL_GetAudioStreamQueued(astream[st]);
    return b > 0 ? b / (2.0 * a_ch[st] * a_rate[st]) : 0;
}
/* streams that mix on the audio thread: SDL asks for more sound when it needs it */
static void (*a_pull[2])(int16_t *, int);
static void SDLCALL pull_cb(void *ud, SDL_AudioStream *as, int additional, int total)
{
    int st = (int)(intptr_t)ud;
    int16_t buf[2 * 1024];
    int frames = additional / (2 * a_ch[st]);
    (void)total;
    while (frames > 0) {
        int maxf = 2048 / a_ch[st], k = frames > maxf ? maxf : frames;
        a_pull[st](buf, k);
        SDL_PutAudioStreamData(as, buf, k * 2 * a_ch[st]);
        frames -= k;
    }
}
int fe_audio_open_pull(int st, int rate, int channels, void (*pull)(int16_t *out, int frames))
{
    SDL_AudioSpec spec = { SDL_AUDIO_S16, channels, rate };
    if (astream[st]) fe_audio_close(st);
    a_pull[st] = pull; a_rate[st] = rate; a_ch[st] = channels;
    astream[st] = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, pull_cb, (void *)(intptr_t)st);
    if (!astream[st]) { dtu_log("[sound] cannot open audio: %s", SDL_GetError()); return 0; }
    SDL_SetAudioStreamGain(astream[st], cfg.volume / 100.0f);
    return 1;
}
void fe_audio_close(int st)
{
    if (astream[st]) { SDL_DestroyAudioStream(astream[st]); astream[st] = NULL; }
}

static const char *scale_names[] = { "nearest", "linear", "sharp" };

static void trim(char *s)
{
    char *e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1])) *--e = 0;
    char *b = s;
    while (*b && isspace((unsigned char)*b)) b++;
    memmove(s, b, strlen(b) + 1);
}

static void load_config(const char *path)
{
    FILE *f = fopen(path, "r");
    char line[1200];
    if (!f) {
        f = fopen(path, "w");
        if (f) {
            fprintf(f, INI_TEXT, cfg.fullscreen, cfg.width, cfg.height, shader_names[cfg.shader],
                    scale_names[cfg.scale], cfg.aspect, cfg.vsync, cfg.fps_limit, cfg.show_fps, cfg.mouse_speed, cfg.mouse_vertical, cfg.menu_pointer, cfg.volume, cfg.physics_fps, cfg.ladder_fix, cfg.menu_key_delay, cfg.sprite_fix, cfg.persp_objects, cfg.far_detail, cfg.weapon_light, cfg.smooth_anim, cfg.dialog_keys, cfg.full_3d, cfg.creature_floor, cfg.step_gap, cfg.hit_sounds, cfg.music_hold, cfg.args, cfg.cd_path);
            pad_write_config(f);
            fclose(f);
        }
        return;
    }
    int seen_menu_pointer = 0, seen_weapon_light = 0, seen_smooth = 0, seen_dialog = 0, seen_full = 0, seen_ladder = 0, seen_floor = 0, seen_step = 0, seen_hit = 0, seen_music = 0, seen_pad = 0;
    while (fgets(line, sizeof line, f)) {
        char *eq = strchr(line, '=');
        if (line[0] == ';' || line[0] == '[' || !eq) continue;
        *eq = 0;
        char *k = line, *v = eq + 1;
        trim(k); trim(v);
        if (!strcmp(k, "menu_pointer")) seen_menu_pointer = 1;
        if (!strcmp(k, "weapon_light")) seen_weapon_light = 1;
        if (!strcmp(k, "smooth_animation")) seen_smooth = 1;
        if (!strcmp(k, "controller")) seen_pad = 1;
        if (!strcmp(k, "dialog_keys")) seen_dialog = 1;
        if (!strcmp(k, "full_lines_3d")) seen_full = 1;
        if (!strcmp(k, "ladder_fix")) seen_ladder = 1;
        if (!strcmp(k, "creature_floor")) seen_floor = 1;
        if (!strcmp(k, "footstep_gap")) seen_step = 1;
        if (!strcmp(k, "hit_sounds")) seen_hit = 1;
        if (!strcmp(k, "music_hold")) seen_music = 1;
        if (pad_config(k, v)) continue;
        if (!strcmp(k, "fullscreen")) cfg.fullscreen = atoi(v);
        else if (!strcmp(k, "width")) cfg.width = atoi(v) >= 320 ? atoi(v) : cfg.width;
        else if (!strcmp(k, "height")) cfg.height = atoi(v) >= 200 ? atoi(v) : cfg.height;
        else if (!strcmp(k, "aspect")) cfg.aspect = atoi(v);
        else if (!strcmp(k, "vsync")) cfg.vsync = atoi(v);
        else if (!strcmp(k, "fps_limit")) cfg.fps_limit = atoi(v) > 0 ? atoi(v) : 0;
        else if (!strcmp(k, "show_fps")) cfg.show_fps = atoi(v);
        else if (!strcmp(k, "physics_fps")) cfg.physics_fps = atoi(v) > 0 ? atoi(v) : 0;
        else if (!strcmp(k, "far_detail")) cfg.far_detail = atoi(v) != 0;
        else if (!strcmp(k, "weapon_light")) cfg.weapon_light = atoi(v) != 0;
        else if (!strcmp(k, "smooth_animation")) cfg.smooth_anim = atoi(v) != 0;
        else if (!strcmp(k, "dialog_keys")) cfg.dialog_keys = atoi(v) != 0;
        else if (!strcmp(k, "full_lines_3d")) cfg.full_3d = atoi(v) != 0;
        else if (!strcmp(k, "ladder_fix")) cfg.ladder_fix = atoi(v) != 0;
        else if (!strcmp(k, "creature_floor")) cfg.creature_floor = atoi(v) != 0;
        else if (!strcmp(k, "footstep_gap")) { int g = atoi(v); cfg.step_gap = g < 0 ? 0 : g > 2000 ? 2000 : g; }
        else if (!strcmp(k, "hit_sounds")) cfg.hit_sounds = atoi(v) != 0;
        else if (!strcmp(k, "music_hold")) cfg.music_hold = atoi(v) != 0;
        else if (!strcmp(k, "perspective_objects")) cfg.persp_objects = atoi(v) != 0;
        else if (!strcmp(k, "sprite_fix")) cfg.sprite_fix = atoi(v) != 0;
        else if (!strcmp(k, "menu_key_delay")) cfg.menu_key_delay = atoi(v) > 0 ? atoi(v) : 0;
        else if (!strcmp(k, "volume")) { int vol = atoi(v); cfg.volume = vol < 0 ? 0 : vol > 100 ? 100 : vol; }
        else if (!strcmp(k, "menu_pointer")) cfg.menu_pointer = atoi(v) != 0;
        else if (!strcmp(k, "vertical")) { float sp = (float)atof(v); if (sp > 0.05f && sp < 20) cfg.mouse_vertical = sp; }
        else if (!strcmp(k, "speed")) { float sp = (float)atof(v); if (sp > 0.05f && sp < 20) cfg.mouse_speed = sp; }
        else if (!strcmp(k, "args")) snprintf(cfg.args, sizeof cfg.args, "%s", v);
        else if (!strcmp(k, "cd_path")) snprintf(cfg.cd_path, sizeof cfg.cd_path, "%s", v);
        else if (!strcmp(k, "shader")) { for (int i = 0; i < 5; i++) if (!SDL_strcasecmp(v, shader_names[i])) cfg.shader = i; }
        else if (!strcmp(k, "scale")) { for (int i = 0; i < 3; i++) if (!SDL_strcasecmp(v, scale_names[i])) cfg.scale = i; }
    }
    fclose(f);
    /* settings of newer versions that this INI file does not have yet: added at the end */
    if (!seen_menu_pointer || !seen_weapon_light || !seen_smooth || !seen_dialog || !seen_full || !seen_ladder || !seen_floor || !seen_step || !seen_hit || !seen_music || !seen_pad) {
        f = fopen(path, "a");
        if (!f) return;
        fprintf(f, "\n; added by version %s\n", DTU_VERSION);
        if (!seen_menu_pointer)
            fprintf(f, "[mouse]\n; 1 = a pointer in the text menus (main menu, Load Game, Options): point and\n"
                       ";     click, right button = back; 0 = original (mouse motion moves the selection)\nmenu_pointer = %d\n", cfg.menu_pointer);
        if (!seen_weapon_light || !seen_smooth || !seen_dialog || !seen_full || !seen_ladder || !seen_floor || !seen_step || !seen_hit || !seen_music) fprintf(f, "[game]\n");
        if (!seen_weapon_light)
            fprintf(f, "; 1 = your weapon no longer turns black for a moment while you attack (its\n"
                       ";     light overflowed, in DOS as well); 0 = original\nweapon_light = %d\n", cfg.weapon_light);
        if (!seen_smooth)
            fprintf(f, "; 1 = creatures and your weapon move smoothly between the key frames of\n"
                       ";     their animations (15 per second); 0 = original (in steps)\nsmooth_animation = %d\n", cfg.smooth_anim);
        if (!seen_dialog)
            fprintf(f, "; 1 = choose answers in conversations with the keys 1..9 and 0; 0 = original\ndialog_keys = %d\n", cfg.dialog_keys);
        if (!seen_full)
            fprintf(f, "; 1 = the 3D view uses all 480 lines of the screen (sharper); 0 = original\n"
                       ";     (half the lines, each one drawn twice). Only with Hires Dungeon Graphics\n"
                       ";     on (in the game's options)\nfull_lines_3d = %d\n", cfg.full_3d);
        if (!seen_ladder)
            fprintf(f, "; 1 = if you stay hanging at the top edge of a ladder, hop again (up to 3\n"
                       ";     times); 0 = original\nladder_fix = %d\n", cfg.ladder_fix);
        if (!seen_floor)
            fprintf(f, "; 1 = creatures are drawn with their feet on the floor (in the original they\n"
                       ";     often walk in the air, in DOS as well); 0 = original\ncreature_floor = %d\n", cfg.creature_floor);
        if (!seen_step)
            fprintf(f, "; shortest time between two footsteps of one creature, in ms; at 60 fps the\n"
                       ";     game started each footstep about four times in a row; 0 = original\nfootstep_gap = %d\n", cfg.step_gap);
        if (!seen_hit)
            fprintf(f, "; 1 = sounds that follow a creature (the weapon's hit sound, its cry of pain)\n"
                       ";     are also heard when all 16 sound places are taken by ambient sounds; in\n"
                       ";     the original they were then left out (so a killing blow was sometimes\n"
                       ";     heard without its hit sound); 0 = original\nhit_sounds = %d\n", cfg.hit_sounds);
        if (!seen_music)
            fprintf(f, "; 1 = while the game loads (a level, a saved game, the menu after dying) the\n"
                       ";     music fades out and waits, and fades back in when loading is done,\n"
                       ";     instead of stuttering; 0 = original\nmusic_hold = %d\n", cfg.music_hold);
        if (!seen_pad) pad_write_config(f);
        fclose(f);
        dtu_log("[sdl] new settings added to %s", path);
    }
}

/* ---- keyboard: SDL scancode -> XT set 1 (0x100 = E0 prefix) ---- */
static int xt_code(SDL_Scancode s)
{
    if (s >= SDL_SCANCODE_A && s <= SDL_SCANCODE_Z) {
        static const unsigned char az[26] = { 0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,0x32,
                                              0x31,0x18,0x19,0x10,0x13,0x1F,0x14,0x16,0x2F,0x11,0x2D,0x15,0x2C };
        return az[s - SDL_SCANCODE_A];
    }
    if (s >= SDL_SCANCODE_1 && s <= SDL_SCANCODE_9) return 0x02 + (s - SDL_SCANCODE_1);
    if (s >= SDL_SCANCODE_F1 && s <= SDL_SCANCODE_F10) return 0x3B + (s - SDL_SCANCODE_F1);
    switch (s) {
    case SDL_SCANCODE_0: return 0x0B;
    case SDL_SCANCODE_RETURN: return 0x1C;
    case SDL_SCANCODE_ESCAPE: return 0x01;
    case SDL_SCANCODE_BACKSPACE: return 0x0E;
    case SDL_SCANCODE_TAB: return 0x0F;
    case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_MINUS: return 0x0C;
    case SDL_SCANCODE_EQUALS: return 0x0D;
    case SDL_SCANCODE_LEFTBRACKET: return 0x1A;
    case SDL_SCANCODE_RIGHTBRACKET: return 0x1B;
    case SDL_SCANCODE_BACKSLASH: return 0x2B;
    case SDL_SCANCODE_NONUSBACKSLASH: return 0x56;
    case SDL_SCANCODE_SEMICOLON: return 0x27;
    case SDL_SCANCODE_APOSTROPHE: return 0x28;
    case SDL_SCANCODE_GRAVE: return 0x29;
    case SDL_SCANCODE_COMMA: return 0x33;
    case SDL_SCANCODE_PERIOD: return 0x34;
    case SDL_SCANCODE_SLASH: return 0x35;
    case SDL_SCANCODE_CAPSLOCK: return 0x3A;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    case SDL_SCANCODE_SCROLLLOCK: return 0x46;
    case SDL_SCANCODE_PAUSE: return 0x45;
    case SDL_SCANCODE_INSERT: return 0x152;
    case SDL_SCANCODE_HOME: return 0x147;
    case SDL_SCANCODE_PAGEUP: return 0x149;
    case SDL_SCANCODE_DELETE: return 0x153;
    case SDL_SCANCODE_END: return 0x14F;
    case SDL_SCANCODE_PAGEDOWN: return 0x151;
    case SDL_SCANCODE_RIGHT: return 0x14D;
    case SDL_SCANCODE_LEFT: return 0x14B;
    case SDL_SCANCODE_DOWN: return 0x150;
    case SDL_SCANCODE_UP: return 0x148;
    case SDL_SCANCODE_NUMLOCKCLEAR: return 0x45;
    case SDL_SCANCODE_KP_DIVIDE: return 0x135;
    case SDL_SCANCODE_KP_MULTIPLY: return 0x37;
    case SDL_SCANCODE_KP_MINUS: return 0x4A;
    case SDL_SCANCODE_KP_PLUS: return 0x4E;
    case SDL_SCANCODE_KP_ENTER: return 0x11C;
    case SDL_SCANCODE_KP_1: return 0x4F;
    case SDL_SCANCODE_KP_2: return 0x50;
    case SDL_SCANCODE_KP_3: return 0x51;
    case SDL_SCANCODE_KP_4: return 0x4B;
    case SDL_SCANCODE_KP_5: return 0x4C;
    case SDL_SCANCODE_KP_6: return 0x4D;
    case SDL_SCANCODE_KP_7: return 0x47;
    case SDL_SCANCODE_KP_8: return 0x48;
    case SDL_SCANCODE_KP_9: return 0x49;
    case SDL_SCANCODE_KP_0: return 0x52;
    case SDL_SCANCODE_KP_PERIOD: return 0x53;
    case SDL_SCANCODE_LCTRL: return 0x1D;
    case SDL_SCANCODE_LSHIFT: return 0x2A;
    case SDL_SCANCODE_LALT: return 0x38;
    case SDL_SCANCODE_RCTRL: return 0x11D;
    case SDL_SCANCODE_RSHIFT: return 0x36;
    case SDL_SCANCODE_RALT: return 0x138;
    default: return 0;
    }
}

static void send_key(SDL_Scancode s, int down)
{
    int c = xt_code(s);
    if (!c) return;
    if (c & 0x100) io_key_byte(0xE0);
    io_key_byte((uint8_t)((c & 0x7F) | (down ? 0 : 0x80)));
}

/* ---- mouse: captured (relative mode), motion in game pixels ----
   DTU draws its own cursor and only reads relative motion, as with a DOS
   mouse driver, so the window captures the pointer like DOSBox does. */
static int mouse_buttons, mouse_captured;
static float mouse_frac_x, mouse_frac_y;

static void mouse_capture(int on)
{
    if (on == mouse_captured) return;
    mouse_captured = on;
    SDL_SetWindowRelativeMouseMode(win, on);
    if (!on && mouse_buttons) { mouse_buttons = 0; io_mouse(0, 0, pad_mouse_buttons()); }
    dtu_log("[sdl] mouse %s", on ? "captured" : "released");
}

static void mouse_motion(float dx, float dy)
{
    if (!mouse_captured) return;
    /* one game pixel per window pixel of the picture, times the speed setting */
    float sx = pic.w > 0 ? game_w / pic.w : 1, sy = pic.h > 0 ? game_h / pic.h : 1;
    if (game_w < 640) { sx *= 640.0f / game_w; sy *= 480.0f / game_h; }
    mouse_frac_x += dx * sx * cfg.mouse_speed; mouse_frac_y += dy * sy * cfg.mouse_speed;
    int mx = (int)mouse_frac_x, my = (int)mouse_frac_y;
    mouse_frac_x -= mx; mouse_frac_y -= my;
    if (mx || my) io_mouse(mx, my, mouse_buttons | pad_mouse_buttons());
}

/* the controller (pad.c) sends keys and mouse input like the keyboard and mouse do */
static void pad_key(int code, int down)
{
    if (code & 0x100) io_key_byte(0xE0);
    io_key_byte((uint8_t)((code & 0x7F) | (down ? 0 : 0x80)));
}
static void pad_mouse(int dx, int dy, int buttons) { io_mouse(dx, dy, mouse_buttons | buttons); }

static void toggle_fullscreen(void)
{
    cfg.fullscreen = !cfg.fullscreen;
    SDL_SetWindowFullscreen(win, cfg.fullscreen);
}

static void screenshot(void)
{
    char path[1200];
    for (int i = 0; i < 1000; i++) {
        snprintf(path, sizeof path, "%s/dtu-shot-%03d.png", dtu_out_dir, i);
        FILE *f = fopen(path, "rb");
        if (f) { fclose(f); continue; }
        if (video_write_png(path) == 0) dtu_log("[shot] %s", path);
        return;
    }
}

static void game_thread(void *arg)
{
    (void)arg;
    game_code = dtu_core_run();
    game_done = 1;
}

static void present(void)
{
    static uint32_t *frame;
    if (!frame) frame = malloc(1024 * 768 * 4);
    int w, h;
    if (!video_snapshot(frame, 1024 * 768, &w, &h)) return;
    game_w = w; game_h = h;
    if (gl) {
        crtgl_frame(gl, frame, w, h, cfg.shader, cfg.scale, cfg.aspect, 0, &pic);
        crtgl_swap(gl);
        return;
    }
    if (!ren_tex || ren_tex->w != w || ren_tex->h != h) {
        if (ren_tex) SDL_DestroyTexture(ren_tex);
        ren_tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        SDL_SetTextureScaleMode(ren_tex, cfg.scale == 0 ? SDL_SCALEMODE_NEAREST : SDL_SCALEMODE_LINEAR);
    }
    SDL_UpdateTexture(ren_tex, NULL, frame, w * 4);
    int ow, oh;
    SDL_GetCurrentRenderOutputSize(ren, &ow, &oh);
    float ar = cfg.aspect ? 4.0f / 3.0f : (float)w / h;
    float dh = (float)oh, dw = dh * ar;
    if (dw > ow) { dw = (float)ow; dh = dw / ar; }
    pic.x = (ow - dw) / 2; pic.y = (oh - dh) / 2; pic.w = dw; pic.h = dh;
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderTexture(ren, ren_tex, NULL, &pic);
    SDL_RenderPresent(ren);
}


/* ---- version 1.3 check ----
   The recompiled code is DTUGAME.EXE 1.3; the data files in the game folder
   must come from the 1.3 patch (DTU1-3.EXE, a self-extracting ZIP archive)
   as well: with those of the CD (1.0 .. 1.2a) texts, items and levels do not
   match. Files of the patch: DTUGAME.EXE 1,760,375 bytes, GABE.DAT 369,929. */
static long game_file_size(const char *name)
{
    char path[1200], low[64];
    size_t i;
    for (i = 0; name[i] && i < sizeof low - 1; i++) low[i] = (char)tolower((unsigned char)name[i]);
    low[i] = 0;
    const char *tries[2] = { name, low };
    for (int k = 0; k < 2; k++) {
        snprintf(path, sizeof path, "%s/%s", dtu_game_dir, tries[k]);
        FILE *f = fopen(path, "rb");
        if (!f) continue;
        fseek(f, 0, SEEK_END);
        long n = ftell(f);
        fclose(f);
        return n;
    }
    return -1;
}

static const char *game_version_problem(char *detail, size_t len)
{
    long exe = game_file_size("DTUGAME.EXE"), gab = game_file_size("GABE.DAT");
    const char *what = NULL;
    if (exe >= 0 && exe != 1760375) { what = "DTUGAME.EXE"; snprintf(detail, len, "DTUGAME.EXE is %ld bytes; the one of version 1.3 is 1760375 bytes", exe); }
    else if (exe < 0 && gab >= 0 && gab != 369929) { what = "GABE.DAT"; snprintf(detail, len, "GABE.DAT is %ld bytes; the one of version 1.3 is 369929 bytes", gab); }
    dtu_log("[sdl] game files: DTUGAME.EXE %ld bytes, GABE.DAT %ld bytes (-1 = not there)%s", exe, gab, what ? ": not version 1.3" : "");
    return what;
}

static int ask(SDL_MessageBoxFlags kind, const char *title, const char *msg, int n, const SDL_MessageBoxButtonData *buttons, int fallback)
{
    const SDL_MessageBoxData box = { kind, NULL, title, msg, n, buttons, NULL };
    int choice = fallback;
    if (!SDL_ShowMessageBox(&box, &choice)) { dtu_log("[sdl] message box: %s", SDL_GetError()); choice = fallback; }
    return choice;
}

static void check_game_version(void)
{
    char detail[200], msg[1600], patch[1200];
    if (!game_version_problem(detail, sizeof detail)) return;
    const char *title = "Undermountain Enhanced - version 1.3 needed";
    int have_patch = patch13_find(dtu_game_dir, patch, sizeof patch);
#ifdef _WIN32
    if (!have_patch && getenv("USERPROFILE")) {                     /* or where the browser put the download */
        char dl[1100];
        snprintf(dl, sizeof dl, "%s\\Downloads", getenv("USERPROFILE"));
        have_patch = patch13_find(dl, patch, sizeof patch);
    }
#endif
    if (have_patch) {
        dtu_log("[sdl] patch found: %s", patch);
        snprintf(msg, sizeof msg,
            "The game files in this folder are from an older version of Descent to Undermountain "
            "(%s).\n\n"
            "Undermountain Enhanced is built on version 1.3. With the files of an older version "
            "(such as 1.2 from the CD), texts, items and levels do not match what the program expects.\n\n"
            "The official 1.3 patch was found:\n%s\n\n"
            "Install it now? Its files replace the old ones; those are kept in the folder BEFORE-1.3.",
            detail, patch);
        const SDL_MessageBoxButtonData b[] = {
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 2, "Install patch" },
            { 0, 1, "Start anyway" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Quit" },
        };
        int choice = ask(SDL_MESSAGEBOX_WARNING, title, msg, 3, b, 1);
        if (choice == 0) { dtu_log("[sdl] stopped: not version 1.3"); exit(0); }
        if (choice == 1) { dtu_log("[sdl] started anyway"); return; }
        char result[1400];
        int n = patch13_apply(dtu_game_dir, patch, result, sizeof result);
        dtu_log("[sdl] patch: %d, %s", n, result);
        int still = game_version_problem(detail, sizeof detail) != NULL;
        const SDL_MessageBoxButtonData ok[] = { { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "OK" } };
        if (n > 0 && !still) {
            ask(SDL_MESSAGEBOX_INFORMATION, "Undermountain Enhanced - patch 1.3 installed", result, 1, ok, 1);
            return;
        }
        snprintf(msg, sizeof msg, "The patch could not be installed completely:\n%s\n\nThe game will start anyway.", result);
        ask(SDL_MESSAGEBOX_ERROR, title, msg, 1, ok, 1);
        return;
    }
    snprintf(msg, sizeof msg,
        "The game files in this folder are from an older version of Descent to Undermountain "
        "(%s).\n\n"
        "Undermountain Enhanced is built on version 1.3. With the files of an older version "
        "(such as 1.2 from the CD), texts, items and levels do not match what the program expects.\n\n"
        "The official 1.3 patch can be downloaded from\npatches-scrolls.de/patch/1215/7/41518/download\n\n"
        "Leave the downloaded file (dtu1_3.zip) in your Downloads folder or put it in this folder, "
        "and start Undermountain Enhanced again: it will offer to install the patch.\n\n"
        "Folder: %s", detail, dtu_game_dir);
    const SDL_MessageBoxButtonData b[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 2, "Open download page" },
        { 0, 1, "Start anyway" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Quit" },
    };
    int choice = ask(SDL_MESSAGEBOX_WARNING, title, msg, 3, b, 1);
    if (choice == 2) {
        dtu_log("[sdl] opening " PATCH13_URL);
        if (!SDL_OpenURL(PATCH13_URL)) dtu_log("[sdl] SDL_OpenURL: %s", SDL_GetError());
        exit(0);
    }
    if (choice != 1) { dtu_log("[sdl] stopped: not version 1.3"); exit(0); }
    dtu_log("[sdl] started anyway");
}

/* ---------------- the CD ----------------
   The levels (*.COW, *.HEN), fonts and some other files are read from the CD,
   in its folder DTU. The CD root (the folder that holds DTU) is, in order:
   cd_path in the ini file (the CD root or its DTU folder), _CD\DTU\_CDFILES
   next to the exe, the CDPath of DTU.CFG if it exists on this PC, the game
   folder itself if it holds the CD files, and any CD drive (also a mounted
   ISO/CUE image) that holds DTU\DTU.INI. Each may also be the CD's folder DTU
   itself; dos.c then takes D:\DTU as that folder. */
static int has_file(const char *dir, const char *name)
{
    char p[1300];
    snprintf(p, sizeof p, "%s/%s", dir, name);
    FILE *f = fopen(p, "rb");
    if (f) fclose(f);
    return f != NULL;
}
static int is_cd_root(const char *dir) { return has_file(dir, "DTU/DTU.INI") || has_file(dir, "DTU/HMC.COW"); }
static int use_cd(const char *dir, const char *how)
{
    char d[1100];
    snprintf(d, sizeof d, "%s", dir);
    size_t n = strlen(d);
    while (n > 3 && (d[n - 1] == '/' || d[n - 1] == '\\')) d[--n] = 0;   /* keep "E:\" */
    if (is_cd_root(d)) { snprintf(dtu_cd_dir, sizeof dtu_cd_dir, "%s", d); dtu_log("[cd] %s: %s", how, d); return 1; }
    if (has_file(d, "DTU.INI") || has_file(d, "HMC.COW")) {                 /* the DTU folder itself: take its parent */
        char *q = d + n;
        while (q > d && q[-1] != '/' && q[-1] != '\\') q--;
        if (q > d) { q[-1] = 0; if (strlen(d) == 2) strcat(d, "\\"); }
        if (is_cd_root(d)) { snprintf(dtu_cd_dir, sizeof dtu_cd_dir, "%s", d); dtu_log("[cd] %s (its parent folder): %s", how, d); return 1; }
        snprintf(d, sizeof d, "%s", dir);                                    /* a folder with a copy of DTU: that folder */
        d[n] = 0;
        snprintf(dtu_cd_dir, sizeof dtu_cd_dir, "%s", d); dtu_log("[cd] %s (the folder DTU of the CD): %s", how, d); return 1;
    }
    return 0;
}
static void find_cd(void)
{
    char p[1200], line[256];
#ifdef _WIN32
    UINT old_mode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);   /* no "insert a disk" boxes */
#endif
    if (cfg.cd_path[0] && use_cd(cfg.cd_path, "cd_path in the ini file")) goto done;
    if (cfg.cd_path[0]) dtu_log("[cd] cd_path %s does not hold the CD (no DTU\\DTU.INI)", cfg.cd_path);
    snprintf(p, sizeof p, "%s/_CD/DTU/_CDFILES", dtu_game_dir);
    if (use_cd(p, "next to the program")) goto done;
    snprintf(p, sizeof p, "%s/DTU.CFG", dtu_game_dir);
    FILE *f = fopen(p, "rb");
    if (f) {
        while (fgets(line, sizeof line, f))
            if (!strncmp(line, "CDPath=", 7) && line[8] == ':') {
                line[7 + strcspn(line + 7, "\r\n")] = 0;
                if (use_cd(line + 7, "CDPath of DTU.CFG")) { fclose(f); goto done; }
            }
        fclose(f);
    }
    if (use_cd(dtu_game_dir, "in the game folder")) goto done;          /* the game installed into a copy of the CD */
#ifdef _WIN32
    for (char l = 'D'; l <= 'Z'; l++) {
        char root[4] = { l, ':', '\\', 0 };
        if (GetDriveTypeA(root) != DRIVE_CDROM) continue;
        if (use_cd(root, "CD drive")) goto done;
    }
#endif
    dtu_log("[cd] the CD was not found");
    {
        const SDL_MessageBoxButtonData b[] = {
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Quit" },
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Start anyway" },
        };
        const SDL_MessageBoxData box = { SDL_MESSAGEBOX_WARNING, NULL, APP_NAME,
            "The CD of Descent to Undermountain was not found. The game reads its levels from it.\n\n"
            "Insert the CD or mount its image (ISO or CUE), or copy the contents of the CD to the folder "
            "_CD\\DTU\\_CDFILES next to DTU-Enhanced.exe, or set cd_path in dtu-enhanced.ini to the folder "
            "with the contents of the CD.", 2, b, NULL };
        int choice = 0;
        SDL_Init(SDL_INIT_VIDEO);
        if (!SDL_ShowMessageBox(&box, &choice)) choice = 1;
        if (choice != 1) { dtu_log("[cd] stopped"); exit(0); }
    }
done:
#ifdef _WIN32
    SetErrorMode(old_mode);
#endif
    return;
}

int main(int argc, char **argv)
{
    char path[1200], err[512];
    (void)argc; (void)argv;
    int lowmem_ok = dtu_reserve_lowmem();   /* before any DLL can take it */

    plat_exe_dir(dtu_game_dir, sizeof dtu_game_dir);
    snprintf(dtu_out_dir, sizeof dtu_out_dir, "%s", dtu_game_dir);
    snprintf(path, sizeof path, "%s/dtu-enhanced.log", dtu_out_dir);
    dtu_logf = fopen(path, "w");
    dtu_log("Undermountain Enhanced %s, build %s", DTU_VERSION, DTU_BUILD);
    snprintf(path, sizeof path, "%s/" INI_NAME, dtu_game_dir);
    load_config(path);
    if (!lowmem_ok) dtu_fatal("the DOS memory area A0000-BFFFF is not inside the program image (build error)");

    find_cd();
    io_run_seconds = 0;     /* no time limit or periodic screenshots */
    io_shot_every = 0;

    /* the program's icon (resource 101 in dtu.rc) for the window and the taskbar */
    SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
    SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "101");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) dtu_fatal("SDL_Init: %s", SDL_GetError());
    check_game_version();
#ifdef DTU_SELFBUILD
    if (!selfbuild_prepare()) return 0;     /* recompile the player's DTUGAME.EXE if needed, and load it */
#endif
    SDL_WindowFlags wf = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | (cfg.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    crtgl_set_attributes();
    win = SDL_CreateWindow(APP_NAME, cfg.width, cfg.height, wf | SDL_WINDOW_OPENGL);
    if (win) {
        gl = crtgl_create(win, err, sizeof err);
        if (!gl) { dtu_log("[video] OpenGL 3.3 not available (%s), using the SDL renderer", err); SDL_DestroyWindow(win); win = NULL; }
        else dtu_log("[video] %s", crtgl_info(gl));
        if (gl) SDL_GL_SetSwapInterval(cfg.vsync ? 1 : 0);
    }
    if (!win) {
        win = SDL_CreateWindow(APP_NAME, cfg.width, cfg.height, wf);
        if (!win) dtu_fatal("SDL_CreateWindow: %s", SDL_GetError());
        ren = SDL_CreateRenderer(win, NULL);
        if (!ren) dtu_fatal("SDL_CreateRenderer: %s", SDL_GetError());
        SDL_SetRenderVSync(ren, cfg.vsync ? 1 : 0);
    }
    mouse_capture(1);

    /* the DOS command line */
    static char argbuf[256];
    char *gargv[32];
    int gargc = 0;
    snprintf(argbuf, sizeof argbuf, "%s", cfg.args);
    for (char *t = strtok(argbuf, " \t"); t && gargc < 31; t = strtok(NULL, " \t")) gargv[gargc++] = t;

    io_fps_limit = cfg.fps_limit;
    io_physics_fps = cfg.physics_fps;
    io_ladder_fix = cfg.ladder_fix;
    io_menu_key_delay = cfg.menu_key_delay;
    io_mouse_vertical = cfg.mouse_vertical;
    { extern uint32_t dtu_sprite_cull_on, dtu_obj_affine; dtu_sprite_cull_on = (uint32_t)cfg.sprite_fix; dtu_obj_affine = !cfg.persp_objects; }
    { extern uint32_t dtu_far_detail; dtu_far_detail = cfg.far_detail ? 99 : 0; }
    { extern uint32_t dtu_weapon_light; dtu_weapon_light = (uint32_t)cfg.weapon_light; }
    { extern uint32_t dtu_smooth_anim; dtu_smooth_anim = (uint32_t)cfg.smooth_anim; }
    { extern uint32_t dtu_menu_mouse; dtu_menu_mouse = (uint32_t)cfg.menu_pointer; }
    { extern uint32_t dtu_dialog_keys; dtu_dialog_keys = (uint32_t)cfg.dialog_keys; }
    { extern uint32_t dtu_double_lines; dtu_double_lines = !cfg.full_3d; }
    { extern uint32_t dtu_creature_floor; dtu_creature_floor = (uint32_t)cfg.creature_floor; }
    { extern uint32_t dtu_step_gap; dtu_step_gap = (uint32_t)cfg.step_gap; }
    { extern uint32_t dtu_sound_fallback; dtu_sound_fallback = (uint32_t)cfg.hit_sounds; }
    { extern int sos_music_hold; sos_music_hold = cfg.music_hold; }
    if (getenv("DTU_VERBOSE")) dtu_verbose = atoi(getenv("DTU_VERBOSE"));
    dtu_log("[sdl] fps limit %d (0 = none), physics as at %d fps (0 = original), mouse speed %.2f (vertical x%.2f), menu key delay %d ms", cfg.fps_limit, cfg.physics_fps, cfg.mouse_speed, cfg.mouse_vertical, cfg.menu_key_delay);
    dtu_core_init(gargc, gargv);
    pad_init(pad_key, pad_mouse);
    if (plat_thread(game_thread, NULL, 8u << 20)) dtu_fatal("cannot start the game thread");

    double quit_at = 0, title_at = 0, last_loop = plat_now();
    while (!game_done) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            pad_event(&ev);
            switch (ev.type) {
            case SDL_EVENT_QUIT:
                io_quit_requested = 1;
                if (!quit_at) quit_at = plat_now() + 3.0;
                break;
            case SDL_EVENT_KEY_DOWN: {
                SDL_Keycode k = ev.key.key;
                int alt = (ev.key.mod & SDL_KMOD_ALT) != 0, ctrl = (ev.key.mod & SDL_KMOD_CTRL) != 0;
                if ((k == SDLK_RETURN && alt) || (k == SDLK_F11 && !ctrl)) { if (!ev.key.repeat) toggle_fullscreen(); break; }
                if (k == SDLK_F12 && ctrl) { if (!ev.key.repeat) screenshot(); break; }
                if (k == SDLK_F10 && ctrl) { if (!ev.key.repeat) mouse_capture(!mouse_captured); break; }
                if (dtu_verbose > 1) dtu_log("[sdl] key down %s%s", SDL_GetScancodeName(ev.key.scancode), ev.key.repeat ? " (repeat)" : "");
                send_key(ev.key.scancode, 1);
                break; }
            case SDL_EVENT_KEY_UP:
                if (ev.key.key == SDLK_F11 || ((ev.key.key == SDLK_F12 || ev.key.key == SDLK_F10) && (ev.key.mod & SDL_KMOD_CTRL))) break;
                if (dtu_verbose > 1) dtu_log("[sdl] key up %s", SDL_GetScancodeName(ev.key.scancode));
                send_key(ev.key.scancode, 0);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                { static int n; if (n++ < 3) dtu_log("[sdl] mouse motion %.1f,%.1f (captured %d)", ev.motion.xrel, ev.motion.yrel, mouse_captured); }
                mouse_motion(ev.motion.xrel, ev.motion.yrel);
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                dtu_log("[sdl] window lost focus");
                mouse_capture(0);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                { static int n; if (n++ < 6) dtu_log("[sdl] mouse button %d %s (captured %d)", ev.button.button, ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? "down" : "up", mouse_captured); }
                int bit = ev.button.button == SDL_BUTTON_LEFT ? 1 : ev.button.button == SDL_BUTTON_RIGHT ? 2 : ev.button.button == SDL_BUTTON_MIDDLE ? 4 : 0;
                if (!mouse_captured) {      /* the first click only captures the mouse */
                    if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) mouse_capture(1);
                    break;
                }
                if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) mouse_buttons |= bit; else mouse_buttons &= ~bit;
                io_mouse(0, 0, mouse_buttons | pad_mouse_buttons());
                break; }
            default: break;
            }
        }
        {
            double tn = plat_now();
            pad_update(tn - last_loop);
            last_loop = tn;
        }
        present();
        if (cfg.show_fps && plat_now() >= title_at) {
            /* only while the 3D view runs (the menus do not pass the frame hook) */
            static unsigned long last_frames;
            static int shown = 1;
            unsigned long fr = io_frame_count();
            char title[160];
            title_at = plat_now() + 0.5;
            if (fr == last_frames) {
                if (shown) SDL_SetWindowTitle(win, APP_NAME);
                shown = 0;
            } else {
                last_frames = fr; shown = 1;
                if (cfg.fps_limit > 0) snprintf(title, sizeof title, "%s - %.0f fps (limit %d)", APP_NAME, io_fps, cfg.fps_limit);
                else snprintf(title, sizeof title, "%s - %.0f fps", APP_NAME, io_fps);
                SDL_SetWindowTitle(win, title);
            }
        }
        if (!cfg.vsync) SDL_Delay(10);
        if (quit_at && plat_now() > quit_at) { dtu_log("[main] game did not stop, exiting"); break; }
    }
    dtu_log("[main] game ended with code %d", game_code);
    pad_shutdown();
    if (gl) crtgl_destroy(gl);
    SDL_DestroyWindow(win);
    SDL_Quit();
    if (dtu_logf) fclose(dtu_logf);
    return game_code;
}
