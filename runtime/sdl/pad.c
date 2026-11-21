/* Undermountain Enhanced - game controller support (SDL gamepad -> keyboard and mouse)
 *
 * DTU only knows the DOS joystick (two sticks with four axes in total, four
 * buttons, calibration with F4). A modern controller is better passed on as
 * what the game already handles well: the left stick and the buttons as keys,
 * the right stick as mouse motion (turning and looking, or the cursor and the
 * hand pointer in menus), the triggers as mouse buttons. Every button can be
 * set in the [controller] section of the INI file:
 *   key:XX    scan code XX (hexadecimal, 1XX = with E0 prefix, e.g. 148 = arrow up)
 *   mouse:N   mouse button N (1 left, 2 right, 3 middle)
 *   none
 * Test hook: DTU_PADTEST="t:what:value;..." attaches a virtual controller and
 * plays the script (what = a button name or an axis name, value 0/1 or -1..1). */
#include "pad.h"
#include "../dtu.h"
#include "../platform.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ACT_NONE, ACT_KEY, ACT_MOUSE, ACT_GAME };

/* game:NAME = whatever key the game uses for that function right now (its own
   key table, also after changing keys with F2), see io_game_key */
static const struct { const char *name; int slot, fallback; } game_fn[] = {
    { "forward", 24, 0x148 }, { "back", 26, 0x150 }, { "turn_left", 4, 0x14B }, { "turn_right", 6, 0x14D },
    { "slide_left", 10, 0x2C }, { "slide_right", 12, 0x2D }, { "jump", 2, 0x39 },
};
#define NGAME (int)(sizeof game_fn / sizeof game_fn[0])
static int game_held[NGAME];            /* the key sent for a held game function */
typedef struct { int type, code; } action;

/* buttons: the SDL gamepad buttons, then the two triggers */
#define NB (SDL_GAMEPAD_BUTTON_COUNT + 2)
#define TRIG_L SDL_GAMEPAD_BUTTON_COUNT
#define TRIG_R (SDL_GAMEPAD_BUTTON_COUNT + 1)
static const char *bnames[NB] = {
    "a", "b", "x", "y", "back", "guide", "start", "left_stick", "right_stick", "left_shoulder", "right_shoulder",
    "dpad_up", "dpad_down", "dpad_left", "dpad_right", "misc1", "paddle_r1", "paddle_l1", "paddle_r2", "paddle_l2",
    "touchpad", "misc2", "misc3", "misc4", "misc5", "misc6", "left_trigger", "right_trigger" };
static action map[NB];
/* left stick: forward, back, left, right */
static action stick[4];
static const char *snames[4] = { "left_stick_up", "left_stick_down", "left_stick_left", "left_stick_right" };

static int enabled = 1;
static float deadzone = 0.25f, look_speed = 900, look_vertical = 0.7f;
static int invert_y;

static SDL_Gamepad *pad;
static int bdown[NB], sdown[4];
static double repeat_at[NB];
static float frac_x, frac_y;
static int mouse_bits;
static pad_key_fn key_out;
static pad_mouse_fn mouse_out;

static action parse(const char *v)
{
    action a = { ACT_NONE, 0 };
    if (!SDL_strncasecmp(v, "key:", 4)) { a.type = ACT_KEY; a.code = (int)strtol(v + 4, NULL, 16); }
    else if (!SDL_strncasecmp(v, "game:", 5)) { for (int i = 0; i < NGAME; i++) if (!SDL_strcasecmp(v + 5, game_fn[i].name)) { a.type = ACT_GAME; a.code = i; } }
    else if (!SDL_strncasecmp(v, "mouse:", 6)) { int b = atoi(v + 6); if (b >= 1 && b <= 3) { a.type = ACT_MOUSE; a.code = b == 1 ? 1 : b == 2 ? 2 : 4; } }
    return a;
}

static int have_defaults;
static void defaults(void)
{
    if (have_defaults) return;
    have_defaults = 1;
    static const struct { int b; const char *v; } d[] = {
        { SDL_GAMEPAD_BUTTON_SOUTH, "key:1C" },          /* Enter */
        { SDL_GAMEPAD_BUTTON_EAST, "key:01" },           /* Escape */
        { SDL_GAMEPAD_BUTTON_WEST, "key:2F" },           /* V: immersive mode on/off */
        { SDL_GAMEPAD_BUTTON_NORTH, "key:17" },          /* I: inventory */
        { SDL_GAMEPAD_BUTTON_BACK, "key:3D" },           /* F3: interface on/off */
        { SDL_GAMEPAD_BUTTON_START, "key:3C" },          /* F2: options */
        { SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, "key:0F" },  /* Tab: automap */
        { SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, "key:3A" }, /* Caps Lock: auto run */
        { SDL_GAMEPAD_BUTTON_DPAD_UP, "key:148" },
        { SDL_GAMEPAD_BUTTON_DPAD_DOWN, "key:150" },
        { SDL_GAMEPAD_BUTTON_DPAD_LEFT, "key:14B" },
        { SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "key:14D" },
        { TRIG_L, "mouse:2" },
        { TRIG_R, "mouse:1" },
    };
    for (size_t i = 0; i < sizeof d / sizeof d[0]; i++) map[d[i].b] = parse(d[i].v);
    stick[0] = parse("game:forward"); stick[1] = parse("game:back");
    stick[2] = parse("game:slide_left"); stick[3] = parse("game:slide_right");
}

static void fmt(char *out, size_t n, action a)
{
    if (a.type == ACT_KEY) snprintf(out, n, "key:%02X", a.code);
    else if (a.type == ACT_GAME) snprintf(out, n, "game:%s", game_fn[a.code].name);
    else if (a.type == ACT_MOUSE) snprintf(out, n, "mouse:%d", a.code == 1 ? 1 : a.code == 2 ? 2 : 3);
    else snprintf(out, n, "none");
}

int pad_config(const char *k, const char *v)
{
    defaults();
    if (!strcmp(k, "controller")) { enabled = atoi(v) != 0; return 1; }
    if (!strcmp(k, "deadzone")) { float f = (float)atof(v); if (f >= 0 && f < 0.9f) deadzone = f; return 1; }
    if (!strcmp(k, "look_speed")) { float f = (float)atof(v); if (f > 0 && f < 10000) look_speed = f; return 1; }
    if (!strcmp(k, "look_vertical")) { float f = (float)atof(v); if (f > 0 && f < 10) look_vertical = f; return 1; }
    if (!strcmp(k, "invert_y")) { invert_y = atoi(v) != 0; return 1; }
    for (int i = 0; i < 4; i++) if (!strcmp(k, snames[i])) {
        /* the fixed keys of versions up to 1.1.1 (arrows, and A/D, keys of one player's own
           layout) become the game functions, whatever keys the game uses for them */
        static const char *old[4] = { "key:148", "key:150", "key:1E", "key:20" };
        stick[i] = !SDL_strcasecmp(v, old[i]) ? (i == 0 ? parse("game:forward") : i == 1 ? parse("game:back") : i == 2 ? parse("game:slide_left") : parse("game:slide_right")) : parse(v);
        return 1;
    }
    for (int i = 0; i < NB; i++) if (!strcmp(k, bnames[i])) { map[i] = parse(v); return 1; }
    return 0;
}

void pad_write_config(FILE *f)
{
    char s[32];
    defaults();
    fprintf(f, "[controller]\n"
               "; game controller (Xbox, PlayStation, ...): 1 = on\n"
               "controller = %d\n"
               "; the right stick turns and looks (or moves the pointer) like the mouse:\n"
               "; pixels per second at full tilt, vertical speed x this, 1 = up is down\n"
               "look_speed = %.0f\nlook_vertical = %.2f\ninvert_y = %d\n"
               "; stick dead zone 0..0.9\n"
               "deadzone = %.2f\n"
               "; what each button does: key:XX = scan code XX in hex (1XX = with E0 prefix,\n"
               "; e.g. 148 arrow up, 150 down, 14B left, 14D right, 1C Enter, 01 Escape,\n"
               "; 39 space, 0F Tab, 3A Caps Lock, 3B..44 F1..F10, 02..0B 1..0, 1D Ctrl,\n"
               "; 38 Alt, 2A Shift), mouse:1 / mouse:2 / mouse:3 = mouse button,\n"
               "; game:forward, back, turn_left, turn_right, slide_left, slide_right, jump =\n"
               "; the key the game uses for that (also after changing keys with F2), none\n",
            enabled, look_speed, look_vertical, invert_y, deadzone);
    for (int i = 0; i < 4; i++) { fmt(s, sizeof s, stick[i]); fprintf(f, "%s = %s\n", snames[i], s); }
    for (int i = 0; i < NB; i++) {
        if (i == SDL_GAMEPAD_BUTTON_GUIDE || (i >= SDL_GAMEPAD_BUTTON_MISC1 && i < SDL_GAMEPAD_BUTTON_COUNT)) continue;
        fmt(s, sizeof s, map[i]); fprintf(f, "%s = %s\n", bnames[i], s);
    }
}

static void do_action(action a, int down)
{
    if (a.type == ACT_KEY) key_out(a.code, down);
    else if (a.type == ACT_GAME) {
        if (down) {
            game_held[a.code] = io_game_key(game_fn[a.code].slot, game_fn[a.code].fallback);
            dtu_log("[pad] game:%s -> key %03X", game_fn[a.code].name, game_held[a.code]);
        }
        if (game_held[a.code]) key_out(game_held[a.code], down);
        if (!down) game_held[a.code] = 0;
    }
    else if (a.type == ACT_MOUSE) {
        if (down) mouse_bits |= a.code; else mouse_bits &= ~a.code;
        mouse_out(0, 0, mouse_bits);
    }
}

static void press(int b, int down)
{
    if (b < 0 || b >= NB || bdown[b] == down) return;
    bdown[b] = down;
    if (dtu_verbose > 1) dtu_log("[pad] %s %s", bnames[b], down ? "down" : "up");
    do_action(map[b], down);
    repeat_at[b] = down ? plat_now() + 0.4 : 0;
}

static void open_pad(SDL_JoystickID id)
{
    if (pad || !enabled) return;
    pad = SDL_OpenGamepad(id);
    if (pad) dtu_log("[pad] controller connected: %s", SDL_GetGamepadName(pad));
    else dtu_log("[pad] cannot open controller: %s", SDL_GetError());
}

static void release_all(void)
{
    for (int i = 0; i < NB; i++) press(i, 0);
    for (int i = 0; i < 4; i++) if (sdown[i]) { sdown[i] = 0; do_action(stick[i], 0); }
}

/* ---- test script ---- */
static SDL_JoystickID vid;
static SDL_Joystick *vjoy;
static struct { double t; char what[24]; float v; int done; } script[64];
static int nscript;

static void test_init(void)
{
    const char *e = getenv("DTU_PADTEST");
    if (!e) return;
    SDL_VirtualJoystickDesc d;
    SDL_INIT_INTERFACE(&d);
    d.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    d.naxes = SDL_GAMEPAD_AXIS_COUNT;
    d.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
    d.button_mask = (1u << SDL_GAMEPAD_BUTTON_COUNT) - 1;
    d.axis_mask = (1u << SDL_GAMEPAD_AXIS_COUNT) - 1;
    d.name = "Test controller";
    vid = SDL_AttachVirtualJoystick(&d);
    vjoy = vid ? SDL_OpenJoystick(vid) : NULL;
    dtu_log("[pad] test controller %s", vjoy ? "attached" : SDL_GetError());
    char buf[2048];
    snprintf(buf, sizeof buf, "%s", e);
    for (char *t = strtok(buf, ";"); t && nscript < 64; t = strtok(NULL, ";")) {
        double tt; char w[24]; float v;
        if (sscanf(t, "%lf:%23[^:]:%f", &tt, w, &v) == 3) { script[nscript].t = tt; snprintf(script[nscript].what, sizeof script[nscript].what, "%s", w); script[nscript].v = v; nscript++; }
    }
}

static void test_run(void)
{
    static const char *axes[SDL_GAMEPAD_AXIS_COUNT] = { "leftx", "lefty", "rightx", "righty", "lt", "rt" };
    if (!vjoy) return;
    double t = plat_now();
    for (int i = 0; i < nscript; i++) {
        if (script[i].done || script[i].t > t) continue;
        script[i].done = 1;
        dtu_log("[pad] test %s = %.2f at %.2f s", script[i].what, script[i].v, t);
        for (int a = 0; a < SDL_GAMEPAD_AXIS_COUNT; a++)
            if (!strcmp(script[i].what, axes[a])) SDL_SetJoystickVirtualAxis(vjoy, a, (Sint16)(script[i].v * 32767));
        for (int b = 0; b < SDL_GAMEPAD_BUTTON_COUNT; b++)
            if (!strcmp(script[i].what, bnames[b])) SDL_SetJoystickVirtualButton(vjoy, b, script[i].v != 0);
    }
}

/* ---- interface ---- */
void pad_init(pad_key_fn k, pad_mouse_fn m)
{
    defaults();
    key_out = k; mouse_out = m;
    if (!enabled) { dtu_log("[pad] controller support off"); return; }
    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) { dtu_log("[pad] SDL gamepad: %s", SDL_GetError()); enabled = 0; return; }
    test_init();
    int n = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&n);
    if (ids && n > 0) open_pad(ids[0]);
    SDL_free(ids);
}

int pad_mouse_buttons(void) { return mouse_bits; }

void pad_event(const SDL_Event *ev)
{
    if (!enabled) return;
    switch (ev->type) {
    case SDL_EVENT_GAMEPAD_ADDED: open_pad(ev->gdevice.which); break;
    case SDL_EVENT_GAMEPAD_REMOVED:
        if (pad && SDL_GetGamepadID(pad) == ev->gdevice.which) {
            release_all();
            SDL_CloseGamepad(pad); pad = NULL;
            dtu_log("[pad] controller disconnected");
            int n = 0;
            SDL_JoystickID *ids = SDL_GetGamepads(&n);
            if (ids && n > 0) open_pad(ids[0]);
            SDL_free(ids);
        }
        break;
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN: case SDL_EVENT_GAMEPAD_BUTTON_UP:
        if (pad && ev->gbutton.which == SDL_GetGamepadID(pad)) press(ev->gbutton.button, ev->gbutton.down);
        break;
    default: break;
    }
}

static float shaped(float v)
{
    float a = v < 0 ? -v : v;
    if (a <= deadzone) return 0;
    a = (a - deadzone) / (1 - deadzone);
    if (a > 1) a = 1;
    a = a * a;                                  /* fine control near the centre */
    return v < 0 ? -a : a;
}

/* once per front-end loop: sticks, triggers, repeats */
void pad_update(double dt)
{
    test_run();
    if (!pad) return;
    if (dt > 0.1) dt = 0.1;
    float ax[SDL_GAMEPAD_AXIS_COUNT];
    for (int a = 0; a < SDL_GAMEPAD_AXIS_COUNT; a++) ax[a] = SDL_GetGamepadAxis(pad, (SDL_GamepadAxis)a) / 32767.0f;
    /* left stick: keys, with a little hysteresis */
    float lv[4] = { -ax[SDL_GAMEPAD_AXIS_LEFTY], ax[SDL_GAMEPAD_AXIS_LEFTY], -ax[SDL_GAMEPAD_AXIS_LEFTX], ax[SDL_GAMEPAD_AXIS_LEFTX] };
    for (int i = 0; i < 4; i++) {
        float on = deadzone + 0.15f, off = deadzone + 0.05f;
        int d = sdown[i] ? lv[i] > off : lv[i] > on;
        if (d != sdown[i]) { sdown[i] = d; do_action(stick[i], d); }
    }
    /* triggers as buttons */
    press(TRIG_L, bdown[TRIG_L] ? ax[SDL_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.3f : ax[SDL_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.5f);
    press(TRIG_R, bdown[TRIG_R] ? ax[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.3f : ax[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.5f);
    /* right stick: mouse motion */
    float rx = shaped(ax[SDL_GAMEPAD_AXIS_RIGHTX]), ry = shaped(ax[SDL_GAMEPAD_AXIS_RIGHTY]);
    if (invert_y) ry = -ry;
    frac_x += rx * look_speed * (float)dt;
    frac_y += ry * look_speed * look_vertical * (float)dt;
    int mx = (int)frac_x, my = (int)frac_y;
    frac_x -= mx; frac_y -= my;
    if (mx || my) mouse_out(mx, my, mouse_bits);
    /* the d-pad repeats like a held key (menus, dialogs) */
    double t = plat_now();
    for (int b = SDL_GAMEPAD_BUTTON_DPAD_UP; b <= SDL_GAMEPAD_BUTTON_DPAD_RIGHT; b++)
        if (bdown[b] && map[b].type == ACT_KEY && repeat_at[b] > 0 && t >= repeat_at[b]) {
            key_out(map[b].code, 1);
            repeat_at[b] = t + 0.1;
        }
}

void pad_shutdown(void)
{
    if (pad) { release_all(); SDL_CloseGamepad(pad); pad = NULL; }
    if (vjoy) { SDL_CloseJoystick(vjoy); vjoy = NULL; }
    if (vid) { SDL_DetachVirtualJoystick(vid); vid = 0; }
}
