/* Undermountain Enhanced - OS fault handling: low-memory accesses and crash reports */
#define _GNU_SOURCE
#include "dtu.h"
#include "fault.h"
#include <string.h>
#include <stdlib.h>

static void crash_report(const char *what, uint64_t rip, uintptr_t addr, uint64_t *const r[16])
{
    FILE *f = dtu_logf ? dtu_logf : stderr;
    fprintf(f, "\n*** %s at rip=%016llx addr=%p\n"
               "    eax=%08x ebx=%08x ecx=%08x edx=%08x esi=%08x edi=%08x ebp=%08x esp(r11)=%08x r8=%08x r9=%08x r10=%08x\n",
            what, (unsigned long long)rip, (void *)addr,
            (uint32_t)*r[0], (uint32_t)*r[3], (uint32_t)*r[1], (uint32_t)*r[2], (uint32_t)*r[6], (uint32_t)*r[7],
            (uint32_t)*r[5], (uint32_t)*r[11], (uint32_t)*r[8], (uint32_t)*r[9], (uint32_t)*r[10]);
    /* x86 stack: likely return addresses into recompiled code */
    uint32_t esp = (uint32_t)*r[11];
    if (esp >= 0x10000000u && esp < 0x80000000u) {
        fprintf(f, "    stack:");
        for (int i = 0; i < 24; i++) fprintf(f, " %08x", ((uint32_t *)PTR(esp))[i]);
        fprintf(f, "\n");
    }
    fflush(f);
    dtu_screenshot("crash");
}

/* Divide error (#DE) in the recompiled code: divide by zero or a quotient that does not fit.
   The DOS game would end with an exception dump. We saturate the quotient instead (like a
   fixdiv with overflow check), set the remainder to 0 and continue after the instruction.
   Returns the instruction length, or 0 if the instruction is not a div/idiv we understand. */
static unsigned div_faults;
static int divide_fault(cpu_ctx *c, const uint8_t *ip)
{
    const uint8_t *p = ip;
    int opsize = 32, addr32 = 0, rex = 0;
    for (;; p++) {
        if (*p == 0x66) opsize = 16;
        else if (*p == 0x67) addr32 = 1;
        else break;
    }
    if ((*p & 0xF0) == 0x40) rex = *p++;
    if (rex & 8) opsize = 64;
    if (*p == 0xF6) opsize = 8;
    else if (*p != 0xF7) return 0;
    p++;
    uint8_t modrm = *p++;
    int mod = modrm >> 6, reg = (modrm >> 3) & 7, rm = modrm & 7;
    if (reg != 6 && reg != 7) return 0;
    int is_signed = reg == 7;
    int64_t divisor = 0;
    if (mod == 3) {
        int r = rm | ((rex & 1) << 3);
        uint64_t v = *c->r[r];
        if (opsize == 8 && !rex && r >= 4) v >>= 8;  /* ah ch dh bh */
        divisor = (int64_t)v;
    } else {
        uint64_t a = 0;
        if (rm == 4) {
            uint8_t sib = *p++;
            int base = (sib & 7) | ((rex & 1) << 3), idx = ((sib >> 3) & 7) | ((rex & 2) << 2);
            if (idx != 4) a += *c->r[idx] << (sib >> 6);
            if ((sib & 7) == 5 && mod == 0) { a += (uint64_t)(int64_t)*(const int32_t *)p; p += 4; }
            else a += *c->r[base];
        } else if (rm == 5 && mod == 0) {
            int32_t d = *(const int32_t *)p; p += 4;
            a = (uint64_t)(p - ip) + c->rip + (int64_t)d;     /* rip-relative: from the next instruction */
            a = (uint64_t)((int64_t)a);
        } else {
            a = *c->r[rm | ((rex & 1) << 3)];
        }
        if (mod == 1) { a += (uint64_t)(int64_t)*(const int8_t *)p; p += 1; }
        else if (mod == 2) { a += (uint64_t)(int64_t)*(const int32_t *)p; p += 4; }
        if (addr32) a = (uint32_t)a;
        const void *m = (const void *)(uintptr_t)a;
        divisor = opsize == 8 ? *(const int8_t *)m : opsize == 16 ? *(const int16_t *)m :
                  opsize == 32 ? *(const int32_t *)m : *(const int64_t *)m;
    }
    int len = (int)(p - ip);
    /* sign of the dividend (high half) and of the divisor */
    uint64_t *rax = c->r[0], *rdx = c->r[2];
    int neg;
    switch (opsize) {
    case 8:
        neg = ((int16_t)*rax < 0) ^ ((int8_t)divisor < 0);
        *rax = (*rax & ~0xFFFFull) | (is_signed ? (neg ? 0x80 : 0x7F) : 0xFF);   /* al = q, ah = 0 */
        break;
    case 16:
        neg = ((int16_t)*rdx < 0) ^ ((int16_t)divisor < 0);
        *rax = (*rax & ~0xFFFFull) | (is_signed ? (neg ? 0x8000 : 0x7FFF) : 0xFFFF);
        *rdx &= ~0xFFFFull;
        break;
    case 32:
        neg = ((int32_t)*rdx < 0) ^ ((int32_t)divisor < 0);
        *rax = is_signed ? (neg ? 0x80000000u : 0x7FFFFFFFu) : 0xFFFFFFFFu;
        *rdx = 0;
        break;
    default:
        neg = ((int64_t)*rdx < 0) ^ (divisor < 0);
        *rax = is_signed ? (neg ? 0x8000000000000000ull : 0x7FFFFFFFFFFFFFFFull) : ~0ull;
        *rdx = 0;
        break;
    }
    if (++div_faults <= 10 || (div_faults & (div_faults - 1)) == 0) {
        FILE *f = dtu_logf ? dtu_logf : stderr;
        fprintf(f, "[fault] divide error at rip %08llx (%s, %d-bit) - quotient saturated (#%u)\n",
                (unsigned long long)c->rip, is_signed ? "idiv" : "div", opsize, div_faults);
        fflush(f);
    }
    return len;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static void ctx_view(CONTEXT *x, cpu_ctx *c)
{
    DWORD64 *regs[16] = { &x->Rax, &x->Rcx, &x->Rdx, &x->Rbx, &x->Rsp, &x->Rbp, &x->Rsi, &x->Rdi,
                          &x->R8, &x->R9, &x->R10, &x->R11, &x->R12, &x->R13, &x->R14, &x->R15 };
    for (int i = 0; i < 16; i++) c->r[i] = (uint64_t *)regs[i];
    c->rip = x->Rip;
    c->trap = (x->EFlags & 0x100) != 0;
}

static LONG CALLBACK veh(EXCEPTION_POINTERS *ep)
{
    EXCEPTION_RECORD *er = ep->ExceptionRecord;
    cpu_ctx c;
    ctx_view(ep->ContextRecord, &c);
    if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2) {
        if (lowmem_fault(&c, (uintptr_t)er->ExceptionInformation[1])) {
            ep->ContextRecord->EFlags |= 0x100;
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    } else if (er->ExceptionCode == EXCEPTION_SINGLE_STEP) {
        if (lowmem_trap(&c)) {
            ep->ContextRecord->EFlags &= ~0x100u;
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        return EXCEPTION_CONTINUE_SEARCH;
    } else if (er->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO || er->ExceptionCode == EXCEPTION_INT_OVERFLOW) {
        int len = divide_fault(&c, (const uint8_t *)(uintptr_t)c.rip);
        if (len) {
            ep->ContextRecord->Rip += len;
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    /* only report faults in our own image (recompiled code and runtime) */
    if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION || er->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION ||
        er->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO || er->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION ||
        er->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
        char what[64];
        snprintf(what, sizeof what, "exception %08lX", (unsigned long)er->ExceptionCode);
        crash_report(what, c.rip, er->NumberParameters >= 2 ? (uintptr_t)er->ExceptionInformation[1] : 0, c.r);
        dtu_crashed(what);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void fault_init(void)
{
    AddVectoredExceptionHandler(1, veh);
}

#else
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

static void ctx_view(ucontext_t *uc, cpu_ctx *c)
{
    greg_t *g = uc->uc_mcontext.gregs;
    static const int map[16] = { REG_RAX, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP, REG_RSI, REG_RDI,
                                 REG_R8, REG_R9, REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15 };
    for (int i = 0; i < 16; i++) c->r[i] = (uint64_t *)&g[map[i]];
    c->rip = (uint64_t)g[REG_RIP];
    c->trap = (g[REG_EFL] & 0x100) != 0;
}

static void on_trap(int sig, siginfo_t *si, void *uc_)
{
    ucontext_t *uc = uc_;
    cpu_ctx c;
    (void)sig; (void)si;
    ctx_view(uc, &c);
    lowmem_trap(&c);
    uc->uc_mcontext.gregs[REG_EFL] &= ~0x100ll;
}

static void on_signal(int sig, siginfo_t *si, void *uc_)
{
    ucontext_t *uc = uc_;
    cpu_ctx c;
    ctx_view(uc, &c);
    if (sig == SIGSEGV && lowmem_fault(&c, (uintptr_t)si->si_addr)) {
        uc->uc_mcontext.gregs[REG_EFL] |= 0x100;
        return;
    }
    if (sig == SIGFPE && (si->si_code == FPE_INTDIV || si->si_code == FPE_INTOVF)) {
        int len = divide_fault(&c, (const uint8_t *)(uintptr_t)c.rip);
        if (len) {
            uc->uc_mcontext.gregs[REG_RIP] += len;
            return;
        }
    }
    char what[32];
    snprintf(what, sizeof what, "signal %d", sig);
    crash_report(what, c.rip, (uintptr_t)si->si_addr, c.r);
    _exit(4);
}

void fault_init(void)
{
    static char altstack[1 << 16];
    stack_t ss = { .ss_sp = altstack, .ss_size = sizeof altstack, .ss_flags = 0 };
    struct sigaction sa;
    sigaltstack(&ss, NULL);
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_signal;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sa.sa_sigaction = on_trap;
    sigaction(SIGTRAP, &sa, NULL);
}
#endif
