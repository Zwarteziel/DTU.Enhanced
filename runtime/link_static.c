/* Undermountain Enhanced - the recompiled object linked into the program
   (headless Linux build and the Windows build from source): the pointers of
   dtu.h point at its symbols. See coffload.c for the self-building version. */
#include "dtu.h"

extern uint8_t dtu_data_base[], stack_start[];
extern void dtu_cstart(void);

uint8_t *dtu_data_ptr = dtu_data_base, *dtu_stack_top = stack_start;
void *dtu_cstart_ptr = (void *)dtu_cstart;
