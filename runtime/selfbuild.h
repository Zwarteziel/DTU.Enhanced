/* Undermountain Enhanced - self-building version (see selfbuild.c) */
#ifndef SELFBUILD_H
#define SELFBUILD_H
/* make sure the recompiled game exists (prepare it if needed) and load it; 0 = stop */
int selfbuild_prepare(void);
/* test: prepare game_dir/enhanced/cache/dtu-main.obj without a window; 1 = done */
int selfbuild_run(const char *game_dir);
#endif
