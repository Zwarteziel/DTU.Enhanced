/* Undermountain Enhanced - install the official 1.3 patch (see patch13.c) */
#ifndef PATCH13_H
#define PATCH13_H
#include <stddef.h>

/* where the patch has been available for years (dtu1_3.zip, the same 44 files as DTU1-3.EXE) */
#define PATCH13_URL "https://www.patches-scrolls.de/patch/1215/7/41518/download"

/* the patch archive in dir (DTU1-3.EXE, DTU1_3.ZIP and the like, any case): 1 and its path, or 0 */
int patch13_find(const char *dir, char *out, size_t outlen);
/* unpack it into dir (old files to dir/BEFORE-1.3): number of files, or -1; msg says what happened */
int patch13_apply(const char *dir, const char *archive, char *msg, size_t msglen);

#endif
