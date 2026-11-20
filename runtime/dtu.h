/* Undermountain Enhanced - platform layer for the recompiled DTUGAME.EXE */
#ifndef DTU_H
#define DTU_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/* registers as pushed by PUSHFD+PUSHAD in x86_int (see asm_int.asm) */
typedef union {
    uint32_t e;
    struct { uint16_t x, x2; } w;
    struct { uint8_t l, h, l2, h2; } b;
} dtu_reg;

typedef struct {
    dtu_reg edi, esi, ebp, esp, ebx, edx, ecx, eax, eflags, eip;
} dtu_regs;

#define EAX (r->eax.e)
#define EBX (r->ebx.e)
#define ECX (r->ecx.e)
#define EDX (r->edx.e)
#define ESI (r->esi.e)
#define EDI (r->edi.e)
#define EBP (r->ebp.e)
#define AX (r->eax.w.x)
#define BX (r->ebx.w.x)
#define CX (r->ecx.w.x)
#define DX (r->edx.w.x)
#define SI (r->esi.w.x)
#define DI (r->edi.w.x)
#define AL (r->eax.b.l)
#define AH (r->eax.b.h)
#define BL (r->ebx.b.l)
#define BH (r->ebx.b.h)
#define CL (r->ecx.b.l)
#define CH (r->ecx.b.h)
#define DL (r->edx.b.l)
#define DH (r->edx.b.h)
#define SET_CF() (r->eflags.e |= 1)
#define CLR_CF() (r->eflags.e &= ~1u)
#define SET_ZF() (r->eflags.e |= 0x40)
#define CLR_ZF() (r->eflags.e &= ~0x40u)

/* 32-bit game address <-> host pointer */
#define PTR(a) ((void *)(uintptr_t)(uint32_t)(a))
#define ADDR(p) ((uint32_t)(uintptr_t)(p))

/* original data object (0x120000...) -> recompiled location. The recompiled
   object is either linked in (link_static.c) or loaded at start-up
   (coffload.c, the self-building Windows version); these three pointers are
   set by one of the two: the data object, the top of the game's stack and
   the game's entry point (Watcom cstart_). */
extern uint8_t *dtu_data_ptr, *dtu_stack_top;
extern void *dtu_cstart_ptr;
#define DATA(a) ((void *)(dtu_data_ptr + ((a) - 0x120000)))

/* Fixed DOS memory: the VGA window at A0000 (the game writes there directly)
   and, in the unused monochrome area at B0000, the PSP, environment, DOS
   memory blocks for real-mode calls, the VESA strings and a CD-ROM device
   header. On Windows this range lies inside the exe image (section .dosmem,
   image base 0x20000), so no other allocation can take it. */
#define LOWMEM_START 0xA0000u
#define LOWMEM_END   0xC0000u
#define VGA_WINDOW   0xA0000u
#define PSP_SEG      0xB000u     /* PSP at linear 0xB0000 */
#define ENV_ADDR     0xB0200u
#define DOSMEM_START 0xB1000u
#define DOSMEM_END   0xBE000u
#define ROM_INFO     0xBE000u    /* VESA OEM string and mode list */
#define CDDEV_ADDR   0xBF000u    /* fake MSCDEX device driver header */

/* x86 stack frame handed between C and recompiled code */
typedef struct { uint32_t esp; uint32_t pad; } dtu_stack;
extern dtu_stack dtu_stk;

/* asm glue (x64/dtu-asm.asm) */
extern int  dtu_c_start(dtu_stack *s);
extern void dtu_c_call_irq(dtu_stack *s, uint32_t handler);
/* calls a far mouse event handler: AX = condition, BX = buttons, CX = x, DX = y */
extern void dtu_c_call_mouse(dtu_stack *s, uint32_t handler, uint32_t ax_bx, uint32_t cx_dx);
extern volatile uint32_t dtu_timer_tick, dtu_timer_run;
extern uint32_t X86_InterruptFlag;
extern uint32_t dtu_env_base, dtu_psp_base;

/* logging */
extern FILE *dtu_logf;
extern int dtu_verbose;
void dtu_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void dtu_fatal(const char *fmt, ...) __attribute__((format(printf, 1, 2), noreturn));

/* core.c */
extern char dtu_game_dir[1024], dtu_cd_dir[1024], dtu_out_dir[1024];
extern volatile int dtu_running;
int  dtu_reserve_lowmem(void);
void dtu_core_init(int gargc, char **gargv);
int  dtu_core_run(void);
void dtu_exit(int code) __attribute__((noreturn));
void dtu_crashed(const char *what) __attribute__((noreturn));
void dtu_screenshot(const char *tag);

/* dos.c */
void dos_init(void);
void dos_int21(dtu_regs *r);
void dos_int2f(dtu_regs *r);
int  dos_host_path(const char *dospath, char *out, size_t outlen, int must_exist);

/* dpmi.c */
void dpmi_init(void);
void dpmi_int31(dtu_regs *r);
uint32_t dpmi_get_vector(int n);
void dpmi_set_vector(int n, uint32_t addr);
uint32_t dosmem_alloc(uint32_t paras, uint32_t *maxparas);
int dosmem_free(uint32_t seg);
void real_mode_int(int n, uint32_t rmregs_addr);

/* video.c */
extern uint8_t *vga_fb;         /* 1 MB host framebuffer (all banks) */
extern uint8_t vga_pal[768];    /* 6-bit DAC values */
extern int vga_width, vga_height, vga_mode, vga_bank;
extern uint32_t vga_lfb_addr;
extern unsigned long vga_frames;
void video_init(void);
void video_int10(dtu_regs *r);
void video_vesa_rm(uint32_t rm);   /* int 10h 4Fxx via DPMI 0300 */
void video_sync_window(void);
uint32_t video_port_in(uint16_t port);
void video_port_out(uint16_t port, uint8_t val);
int  video_write_png(const char *path);
int  video_snapshot(uint32_t *out, int maxpix, int *w, int *h);
void video_movie_begin(void);
void video_movie_frame(const uint8_t *pic, int w, int h, const uint8_t *pal);

/* mve.c: Interplay MVE movies */
int  mve_play(const char *hostpath);   /* 1 = stopped by a key */
void mve_play_guest(dtu_regs *r);     /* the game's movie function (0x96439) */

/* front end: sound output (16-bit signed, interleaved); stubs in the headless build */
/* stream 0: movies, stream 1: game sound (sos.c) */
#define FE_MOVIE 0
#define FE_GAME 1
int  fe_audio_open(int st, int rate, int channels);   /* paused until fe_audio_start */
void fe_audio_queue(int st, const int16_t *samples, int count);
void fe_audio_start(int st);
double fe_audio_queued(int st);                 /* seconds still to play */
void fe_audio_close(int st);
/* stream whose sound the audio thread asks pull() for (frames of `channels` samples); 0: not available */
int  fe_audio_open_pull(int st, int rate, int channels, void (*pull)(int16_t *out, int frames));

/* render.c: renderer routines that use self-modifying code */
void render_scale_setup(void);
void render_anim_angles(dtu_regs *r);
void render_anim_offset(dtu_regs *r);
void render_floor_point(dtu_regs *r);
void render_floor_note(dtu_regs *r);
void render_floor_draw(dtu_regs *r);
void render_sprite_check(dtu_regs *r);
void render_step_sound(dtu_regs *r);
void render_sound_fallback(dtu_regs *r);
void render_sound_at_pos(dtu_regs *r);
void menu_mouse(dtu_regs *r);
void menu_version(dtu_regs *r);
void menu_init(void);
int  menu_pointer(int *x, int *y, const uint32_t **img, int *w, int *h);
void io_mouse_totals(int *mx, int *my, unsigned clicks[2]);
void render_scale_draw(void);

/* sos.c: HMI SOS digital sound replaced by a mixer */
void sos_call(int fn, dtu_regs *r);
int  io_movie_poll(void);
void io_idle_poll(void);            /* a busy-wait poll outside the main loop: throttled */

/* io.c */
void io_init(void);
void timer_start(void);
uint32_t pit_hz(void);
void dtu_run_timer(dtu_stack *s);
void io_key_byte(uint8_t b);
void io_mouse(int dx, int dy, int buttons);
void io_add_mouse(double t, int x, int y, int b);
extern volatile int io_fps_limit;        /* frames per second, 0 = no limit */
extern volatile int io_physics_fps, io_ladder_fix;
extern volatile float io_mouse_vertical;  /* vertical mouse motion in the game x this */
extern volatile int io_menu_key_delay;   /* ms between presses of a menu function, 0 = off */      /* movement damping as at this frame rate, 0 = original */
extern volatile float io_fps;            /* measured game frames per second */
unsigned long io_frame_count(void);
extern volatile int io_quit_requested;
extern double io_run_seconds, io_shot_every;

#endif
