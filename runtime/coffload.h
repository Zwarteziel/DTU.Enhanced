/* Undermountain Enhanced - load the recompiled game (see coffload.c) */
#ifndef COFFLOAD_H
#define COFFLOAD_H
#include <stddef.h>
/* load dtu-main.obj (win64 COFF from nasm) and link it with the program: 1, or 0 and err says why */
int coff_load(const char *path, char *err, size_t errlen);
#endif
