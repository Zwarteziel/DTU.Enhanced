/* Undermountain Enhanced - accesses to the first 64 KB of the address space.
 *
 * Under DOS/4GW the program runs zero-based and flat: a NULL pointer
 * dereference simply reads the real-mode interrupt table. The game relies
 * on that in places (e.g. reading a field of a not-yet-created object).
 * Modern systems cannot map page 0, so such an access faults. We catch the
 * fault, point the base (or index) register of the faulting instruction at a
 * stand-in buffer, single-step the instruction, and put the register
 * back. Reads see zeros (or what was written there), writes go nowhere.
 * The platform-specific part (signals / vectored exceptions) is in fault.c.
 */
#include "dtu.h"
#include "fault.h"
#include "platform.h"
#include <string.h>

#define FAKE_SIZE LOWMEM_START
static uint32_t fake_low;              /* stand-in for linear 0..9FFFF */
static int fix_reg = -1;               /* register adjusted for the pending step */
static uint64_t fix_delta, fix_expect;
static int fix_string;
static unsigned long fix_count;
static uint32_t last_logged_ip;
#define HOT_N 64
static uint32_t hot_ip[HOT_N];
static unsigned long hot_n[HOT_N];

/* Decode just enough of the instruction at ip to find the registers that
   form its memory address. Register numbers: 0 rax .. 15 r15.
   Returns 0 if not understood, 1 for a ModRM operand, 2 for a string op. */
static int decode_mem(const uint8_t *p, int *base, int *index, int *scale, int *addr32)
{
    int rex = 0, op2 = 0;
    *addr32 = 0; *base = *index = -1; *scale = 1;
    for (;; p++) {
        if (*p == 0x67) *addr32 = 1;
        else if (*p == 0x66 || *p == 0xF2 || *p == 0xF3 || *p == 0xF0 || *p == 0x2E || *p == 0x3E || *p == 0x26 || *p == 0x36 || *p == 0x64 || *p == 0x65) ;
        else break;
    }
    if ((*p & 0xF0) == 0x40) rex = *p++;
    uint8_t op = *p++;
    if (op == 0x0F) { op2 = 1; op = *p++; if (op == 0x38 || op == 0x3A) p++; }
    if (!op2) {
        if ((op >= 0xA4 && op <= 0xA7) || (op >= 0xAC && op <= 0xAD)) { *base = 6; return 2; }
        if (op >= 0xAA && op <= 0xAF) { *base = 7; return 2; }
        if (op >= 0xA0 && op <= 0xA3) return 0;   /* moffs: no register to adjust */
    }
    uint8_t modrm = *p++;
    int mod = modrm >> 6, rm = modrm & 7;
    if (mod == 3) return 0;
    if (rm == 4) {
        uint8_t sib = *p++;
        int ss = sib >> 6, idx = ((sib >> 3) & 7) | ((rex & 2) ? 8 : 0), b = (sib & 7) | ((rex & 1) ? 8 : 0);
        *scale = 1 << ss;
        if (idx != 4) *index = idx;
        if ((sib & 7) == 5 && mod == 0) { *base = -1; return 1; }
        *base = b;
    } else if (rm == 5 && mod == 0) {
        return 0;   /* rip-relative */
    } else {
        *base = rm | ((rex & 1) ? 8 : 0);
    }
    return 1;
}

int lowmem_fault(cpu_ctx *c, uintptr_t addr)
{
    int base, index, scale, a32, kind;

    if (addr >= LOWMEM_START || fix_reg >= 0 || c->rip < 0x10000 || !fake_low) return 0;
    kind = decode_mem((const uint8_t *)(uintptr_t)c->rip, &base, &index, &scale, &a32);
    if (!kind) return 0;

    int reg = base >= 0 ? base : index;
    if (reg < 0) return 0;
    fix_reg = reg;
    fix_string = (kind == 2);
    fix_delta = (uint64_t)fake_low / (base >= 0 ? 1 : scale);
    *c->r[reg] += fix_delta;
    if (a32) *c->r[reg] &= 0xFFFFFFFFull;
    fix_expect = *c->r[reg];
    c->trap = 1;                    /* single-step this one instruction */
    fix_count++;
    {   /* small per-site histogram, printed at exit */
        uint32_t ip = (uint32_t)c->rip; int i;
        for (i = 0; i < HOT_N && hot_ip[i] && hot_ip[i] != ip; i++) {}
        if (i < HOT_N) { hot_ip[i] = ip; hot_n[i]++; }
    }
    if ((uint32_t)c->rip != last_logged_ip && fix_count < 200) {
        last_logged_ip = (uint32_t)c->rip;
        dtu_log("[lowmem] access to %04lX at rip %08X (reads zero page) - #%lu", (unsigned long)addr, (uint32_t)c->rip, fix_count);
    }
    return 1;
}

int lowmem_trap(cpu_ctx *c)
{
    if (fix_reg < 0) return 0;
    uint64_t v = *c->r[fix_reg];
    /* restore unless the instruction overwrote the register itself */
    if (v == fix_expect || fix_string) *c->r[fix_reg] = (v - fix_delta) & 0xFFFFFFFFull;
    fix_reg = -1;
    c->trap = 0;
    return 1;
}

unsigned long lowmem_count(void)
{
    for (int k = 0; k < 8; k++) {           /* log the busiest sites */
        int best = -1;
        for (int i = 0; i < HOT_N; i++) if (hot_n[i] && (best < 0 || hot_n[i] > hot_n[best])) best = i;
        if (best < 0) break;
        dtu_log("[lowmem] hot site rip %08X: %lu accesses", hot_ip[best], hot_n[best]);
        hot_n[best] = 0;
    }
    return fix_count;
}

void lowmem_init(void)
{
    void *p = plat_alloc32(FAKE_SIZE);
    if (!p) dtu_fatal("cannot allocate low-memory stand-in");
    memset(p, 0, FAKE_SIZE);
    fake_low = ADDR(p);
}
