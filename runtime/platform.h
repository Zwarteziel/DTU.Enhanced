/* Undermountain Enhanced - host platform services */
#ifndef DTU_PLATFORM_H
#define DTU_PLATFORM_H
#include <stdint.h>
#include <stddef.h>

typedef struct { _Alignas(16) unsigned char opaque[64]; } plat_mutex;

double plat_now(void);                 /* seconds since first call, monotonic */
void plat_sleep_ms(int ms);
int  plat_thread(void (*fn)(void *), void *arg, size_t stack);
void plat_mutex_init(plat_mutex *m);
void plat_lock(plat_mutex *m);
void plat_unlock(plat_mutex *m);
void *plat_alloc_fixed(uint32_t addr, uint32_t size);   /* at exactly addr */
void *plat_alloc32(uint32_t size);                      /* below 2 GB */
void plat_free32(void *p, uint32_t size);
int  plat_mkdir(const char *path);
void plat_exe_dir(char *out, size_t len);

#endif
