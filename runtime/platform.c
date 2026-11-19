/* Undermountain Enhanced - host platform services (Windows and Linux) */
#define _GNU_SOURCE
#include "dtu.h"
#include "platform.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <mmsystem.h>

static LARGE_INTEGER qpf, qp0;
double plat_now(void)
{
    LARGE_INTEGER t;
    if (!qpf.QuadPart) { QueryPerformanceFrequency(&qpf); QueryPerformanceCounter(&qp0); timeBeginPeriod(1); }
    QueryPerformanceCounter(&t);
    return (double)(t.QuadPart - qp0.QuadPart) / (double)qpf.QuadPart;
}
void plat_sleep_ms(int ms) { Sleep((DWORD)ms); }

typedef struct { void (*fn)(void *); void *arg; } thr_arg;
static DWORD WINAPI thr_main(LPVOID p) { thr_arg a = *(thr_arg *)p; free(p); a.fn(a.arg); return 0; }
int plat_thread(void (*fn)(void *), void *arg, size_t stack)
{
    thr_arg *a = malloc(sizeof *a);
    a->fn = fn; a->arg = arg;
    HANDLE h = CreateThread(NULL, stack, thr_main, a, 0, NULL);
    if (!h) return -1;
    CloseHandle(h);
    return 0;
}

void plat_mutex_init(plat_mutex *m) { InitializeCriticalSection((CRITICAL_SECTION *)m->opaque); }
void plat_lock(plat_mutex *m) { EnterCriticalSection((CRITICAL_SECTION *)m->opaque); }
void plat_unlock(plat_mutex *m) { LeaveCriticalSection((CRITICAL_SECTION *)m->opaque); }

void *plat_alloc_fixed(uint32_t addr, uint32_t size)
{
    return VirtualAlloc((void *)(uintptr_t)addr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

/* memory whose address fits in 31 bits (the game keeps pointers in 32-bit
   registers and the recompiled code sign-extends some displacements) */
static uintptr_t alloc_hint = 0x40000000u;
void *plat_alloc32(uint32_t size)
{
    size = (size + 0xFFFFu) & ~0xFFFFu;
    for (int tries = 0; tries < 4096; tries++) {
        if (alloc_hint + size > 0x7FFF0000u) alloc_hint = 0x20000000u;
        void *p = VirtualAlloc((void *)alloc_hint, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        alloc_hint += size;
        if (p) return p;
        alloc_hint = (alloc_hint + 0xFFFFu) & ~(uintptr_t)0xFFFFu;
    }
    return NULL;
}
void plat_free32(void *p, uint32_t size) { (void)size; VirtualFree(p, 0, MEM_RELEASE); }

int plat_mkdir(const char *path) { return _mkdir(path); }

void plat_exe_dir(char *out, size_t len)
{
    char buf[1024];
    DWORD n = GetModuleFileNameA(NULL, buf, sizeof buf);
    if (n == 0 || n >= sizeof buf) { snprintf(out, len, "."); return; }
    char *s = strrchr(buf, '\\');
    if (s) *s = 0;
    snprintf(out, len, "%s", buf);
}

#else
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

static struct timespec t0;
double plat_now(void)
{
    struct timespec t;
    if (!t0.tv_sec && !t0.tv_nsec) clock_gettime(CLOCK_MONOTONIC, &t0);
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)(t.tv_sec - t0.tv_sec) + (t.tv_nsec - t0.tv_nsec) * 1e-9;
}
void plat_sleep_ms(int ms) { struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L }; nanosleep(&ts, NULL); }

typedef struct { void (*fn)(void *); void *arg; } thr_arg;
static void *thr_main(void *p) { thr_arg a = *(thr_arg *)p; free(p); a.fn(a.arg); return NULL; }
int plat_thread(void (*fn)(void *), void *arg, size_t stack)
{
    pthread_t th;
    pthread_attr_t at;
    thr_arg *a = malloc(sizeof *a);
    a->fn = fn; a->arg = arg;
    pthread_attr_init(&at);
    if (stack) pthread_attr_setstacksize(&at, stack);
    int r = pthread_create(&th, &at, thr_main, a);
    pthread_attr_destroy(&at);
    if (r) return -1;
    pthread_detach(th);
    return 0;
}

void plat_mutex_init(plat_mutex *m) { pthread_mutex_init((pthread_mutex_t *)m->opaque, NULL); }
void plat_lock(plat_mutex *m) { pthread_mutex_lock((pthread_mutex_t *)m->opaque); }
void plat_unlock(plat_mutex *m) { pthread_mutex_unlock((pthread_mutex_t *)m->opaque); }

void *plat_alloc_fixed(uint32_t addr, uint32_t size)
{
    void *p = mmap((void *)(uintptr_t)addr, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    return p == MAP_FAILED || p != (void *)(uintptr_t)addr ? NULL : p;
}
void *plat_alloc32(uint32_t size)
{
    void *p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    return p == MAP_FAILED ? NULL : p;
}
void plat_free32(void *p, uint32_t size) { munmap(p, size); }

int plat_mkdir(const char *path) { return mkdir(path, 0755); }

void plat_exe_dir(char *out, size_t len)
{
    char buf[1024];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof buf - 1);
    if (n <= 0) { snprintf(out, len, "."); return; }
    buf[n] = 0;
    char *s = strrchr(buf, '/');
    if (s) *s = 0;
    snprintf(out, len, "%s", buf);
}
#endif
