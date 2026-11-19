/* Undermountain Enhanced - CPU faults of the recompiled code */
#ifndef DTU_FAULT_H
#define DTU_FAULT_H
#include <stdint.h>

/* view on the interrupted thread's registers (pointers into the OS context) */
typedef struct {
    uint64_t *r[16];      /* rax rcx rdx rbx rsp rbp rsi rdi r8..r15 */
    uint64_t rip;
    int trap;             /* in/out: trap flag for single-stepping */
} cpu_ctx;

int  lowmem_fault(cpu_ctx *c, uintptr_t addr);
int  lowmem_trap(cpu_ctx *c);
void lowmem_init(void);
void fault_init(void);    /* install the OS handlers (fault.c) */

#endif
