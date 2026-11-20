/* Undermountain Enhanced - core: memory setup, DOS environment, running the game */
#define _GNU_SOURCE
#include "dtu.h"
#include "fault.h"
#include "platform.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

FILE *dtu_logf;
int dtu_verbose = 1;
char dtu_game_dir[1024] = ".", dtu_cd_dir[1024] = "", dtu_out_dir[1024] = ".";
dtu_stack dtu_stk;
uint32_t dtu_env_base, dtu_psp_base;
uint32_t X86_InterruptFlag;

static void *exit_env[5];      /* __builtin_setjmp buffer: no SEH unwinding on Windows */
static int exit_code;
static int shot_count;
volatile int dtu_running;

void dtu_log(const char *fmt, ...)
{
    va_list ap;
    if (!dtu_logf) dtu_logf = stderr;
    va_start(ap, fmt);
    vfprintf(dtu_logf, fmt, ap);
    va_end(ap);
    fputc('\n', dtu_logf);
    fflush(dtu_logf);
}

void dtu_fatal(const char *fmt, ...)
{
    char msg[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);
    dtu_log("FATAL: %s", msg);
    dtu_screenshot("fatal");
    dtu_crashed(msg);
}

/* last resort: tell the user and stop the process */
void dtu_crashed(const char *what)
{
#ifdef _WIN32
    char msg[700];
    snprintf(msg, sizeof msg, "Undermountain Enhanced stopped: %s\n\nDetails are in dtu-enhanced.log.", what);
    MessageBoxA(NULL, msg, "Undermountain Enhanced", MB_ICONERROR | MB_OK);
    ExitProcess(3);
#else
    fprintf(stderr, "FATAL: %s\n", what);
    _exit(3);
#endif
}

void dtu_screenshot(const char *tag)
{
    char path[1200];
    if (!vga_fb) return;
    snprintf(path, sizeof path, "%s/shot_%03d_%s.png", dtu_out_dir, shot_count++, tag);
    if (video_write_png(path) == 0) dtu_log("[shot] %s (mode %xh %dx%d)", path, vga_mode, vga_width, vga_height);
}

void dtu_exit(int code)
{
    exit_code = code;
    __builtin_longjmp(exit_env, 1);
}

/* reads through a segment prefix with a constant offset (only in unused
   extender paths of cstart); logged so we notice if something else uses it */
uint32_t dtu_seg_read(uint32_t ofs, uint32_t size)
{
    dtu_log("[seg] read %u bytes at seg:%08X -> 0", size, ofs);
    return 0;
}

static void build_psp_env(int argc, char **argv)
{
    uint8_t *psp = PTR(PSP_SEG << 4);
    char tail[128] = "";
    size_t n = 0;
    for (int i = 0; i < argc; i++) {
        size_t l = strlen(argv[i]);
        if (n + l + 2 >= 126) break;
        tail[n++] = ' ';
        memcpy(tail + n, argv[i], l);
        n += l;
    }
    memset(psp, 0, 256);
    psp[0] = 0xCD; psp[1] = 0x20;
    psp[0x80] = (uint8_t)n;
    memcpy(psp + 0x81, tail, n);
    psp[0x81 + n] = 0x0D;
    dtu_psp_base = PSP_SEG << 4;

    /* DOS environment block, then a word count and the program path */
    static const char *env[] = { "PATH=C:\\DTU", "COMSPEC=C:\\COMMAND.COM", "DOS4G=QUIET", NULL };
    char *e = PTR(ENV_ADDR);
    for (int i = 0; env[i]; i++) { strcpy(e, env[i]); e += strlen(env[i]) + 1; }
    *e++ = 0;
    *e++ = 1; *e++ = 0;
    strcpy(e, "C:\\DTU\\DTUGAME.EXE");
    dtu_env_base = ENV_ADDR;
}

/* Conventional memory and the VGA window at their DOS linear addresses.
   Call this first thing: graphics drivers like to put things down there. */
#ifdef _WIN32
extern uint8_t dtu_dos_area[], dtu_dos_area_end[];
#endif
int dtu_reserve_lowmem(void)
{
    static int done;
    if (done) return 1;
#ifdef _WIN32
    /* part of our own image: see x64/dosmem.asm and build-windows.sh */
    if (ADDR(dtu_dos_area) > LOWMEM_START || ADDR(dtu_dos_area_end) < LOWMEM_END) return 0;
#else
    if (!plat_alloc_fixed(LOWMEM_START, LOWMEM_END - LOWMEM_START)) return 0;
#endif
    memset(PTR(LOWMEM_START), 0, LOWMEM_END - LOWMEM_START);
    done = 1;
    return 1;
}

/* Prepare memory and devices; gargc/gargv are the DOS command-line arguments. */
void dtu_core_init(int gargc, char **gargv)
{
    if (!dtu_reserve_lowmem())
        dtu_fatal("cannot reserve the DOS memory area %X-%X", LOWMEM_START, LOWMEM_END - 1);
    fault_init();
    lowmem_init();
    build_psp_env(gargc, gargv);
    dos_init();
    dpmi_init();
    video_init();
    io_init();
    menu_init();
    dtu_log("[main] game=%s cd=%s data=%p stack_top=%p", dtu_game_dir, dtu_cd_dir, (void *)dtu_data_ptr, (void *)dtu_stack_top);
}

/* Run the game until it exits (or dtu_exit is called); returns the exit code. */
static int run_inner(void) __attribute__((noinline));
static int run_inner(void)
{
    if (__builtin_setjmp(exit_env) == 0) {
        dtu_c_start(&dtu_stk);
        dtu_log("[main] cstart returned");
    }
    return exit_code;
}

int dtu_core_run(void)
{
    dtu_stk.esp = ADDR(dtu_stack_top);
    X86_InterruptFlag = 1;
    dtu_running = 1;
    timer_start();
    int code = run_inner();
    dtu_running = 0;
    { extern unsigned long lowmem_count(void), vga_draws; extern uint32_t dtu_sprite_culls; dtu_log("[main] exit code %d after %lu game frames (%lu bank-0 switches), %lu low-memory accesses, %u sprites at the camera moved away", code, io_frame_count(), vga_draws, lowmem_count(), dtu_sprite_culls); }
    return code;
}
