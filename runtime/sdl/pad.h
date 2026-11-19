/* Undermountain Enhanced - game controller support */
#ifndef DTU_PAD_H
#define DTU_PAD_H
#include <stdio.h>
#include <SDL3/SDL_events.h>

typedef void (*pad_key_fn)(int code, int down);              /* XT scan code, 0x100 = E0 prefix */
typedef void (*pad_mouse_fn)(int dx, int dy, int buttons);   /* motion in game pixels, button bits */

int  pad_config(const char *key, const char *value);         /* 1 = a controller setting */
void pad_write_config(FILE *f);
void pad_init(pad_key_fn k, pad_mouse_fn m);
void pad_event(const SDL_Event *ev);
void pad_update(double dt);
int  pad_mouse_buttons(void);
void pad_shutdown(void);
#endif
