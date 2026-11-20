/* Undermountain Enhanced - load the recompiled game at start-up (self-building
   Windows version).

   The program itself contains no code of the game. At the first start
   (selfbuild.c) SR translates the player's DTUGAME.EXE and nasm assembles the
   result into a win64 COFF object, dtu-main.obj. Here that object is loaded
   into memory and linked with the program, as the linker does in the build
   from source:

   - its sections (cseg01 code, dseg02 data, .pdata/.xdata) are placed in one
     block below 2 GB: the translated code works with 32-bit addresses, both
     for data and for the return addresses it keeps on the game's stack;
   - the symbols it needs from the program (x86_int, SR_CheckTimer, the dtu_*
     switches, ...) come from the table in symtab.c, generated from the
     extern lines in the .inc files of recomp/x64;
   - relocations: ADDR64, ADDR32, ADDR32NB (relative to the block) and REL32
     (and REL32_1..5); a section with more than 65535 relocations has the
     real count in its first relocation (IMAGE_SCN_LNK_NRELOC_OVFL);
   - dtu_cstart, dtu_data_base and stack_start set the pointers of dtu.h;
   - the unwind table in .pdata is registered with RtlAddFunctionTable. */
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dtu.h"
#include "coffload.h"

uint8_t *dtu_data_ptr, *dtu_stack_top;
void *dtu_cstart_ptr;

struct symtab_entry { const char *name; void *addr; };
extern const struct symtab_entry dtu_symtab[];

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

static void *find_runtime_symbol(const char *name)
{
    for (const struct symtab_entry *e = dtu_symtab; e->name; e++)
        if (!strcmp(e->name, name)) return e->addr;
    return NULL;
}

/* a block of size bytes below 2 GB (read, write, execute) */
static uint8_t *alloc_low(size_t size)
{
    for (uintptr_t a = 0x20000000u; a + size < 0x7FFF0000u; a += 0x01000000u) {
        void *p = VirtualAlloc((void *)a, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (p) return p;
    }
    return NULL;
}

int coff_load(const char *path, char *err, size_t errlen)
{
    FILE *f = fopen(path, "rb");
    uint8_t *b = NULL, *mem = NULL;
    long len = 0;
    if (!f) { snprintf(err, errlen, "cannot open %s", path); return 0; }
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len < 20 || !(b = malloc((size_t)len)) || fread(b, 1, (size_t)len, f) != (size_t)len) {
        fclose(f); free(b); snprintf(err, errlen, "cannot read %s", path); return 0;
    }
    fclose(f);
    if (rd16(b) != 0x8664) { snprintf(err, errlen, "%s is not an x64 COFF object", path); goto fail; }
    unsigned nsec = rd16(b + 2);
    uint32_t symoff = rd32(b + 8), nsym = rd32(b + 12);
    unsigned opt = rd16(b + 16);
    const uint8_t *sh = b + 20 + opt, *syms = b + symoff, *strtab = syms + (size_t)nsym * 18;
    if (nsec > 64 || (size_t)(sh - b) + nsec * 40u > (size_t)len || symoff + (size_t)nsym * 18 + 4 > (size_t)len) {
        snprintf(err, errlen, "%s: damaged header", path); goto fail;
    }

    /* layout: every section that takes memory, aligned to its own alignment
       (at least 16; code and data on a page of their own) */
    size_t off[65] = { 0 }, total = 0;
    int use[65] = { 0 };
    for (unsigned i = 0; i < nsec; i++) {
        const uint8_t *s = sh + 40 * i;
        uint32_t ch = rd32(s + 36), size = rd32(s + 16);
        if (ch & 0x00000800 || ch & 0x02000000 || !size) continue;          /* LNK_REMOVE, MEM_DISCARDABLE, empty */
        if (!(ch & 0x40000000)) continue;                                     /* not readable at run time (.note) */
        size_t al = size >= 4096 ? 4096 : 16;
        total = (total + al - 1) & ~(al - 1);
        off[i + 1] = total; use[i + 1] = 1;
        total += size;
    }
    if (!(mem = alloc_low(total + 4096))) { snprintf(err, errlen, "no room below 2 GB for %zu bytes", total); goto fail; }
    for (unsigned i = 0; i < nsec; i++) {
        const uint8_t *s = sh + 40 * i;
        uint32_t size = rd32(s + 16), raw = rd32(s + 20);
        if (!use[i + 1]) continue;
        if (raw && raw + (size_t)size <= (size_t)len) memcpy(mem + off[i + 1], b + raw, size);   /* else zero (uninitialised data) */
    }

    /* symbol addresses */
    uint64_t *addr = calloc(nsym ? nsym : 1, sizeof *addr);
    if (!addr) { snprintf(err, errlen, "out of memory"); goto fail; }
    for (uint32_t k = 0; k < nsym; k++) {
        const uint8_t *e = syms + (size_t)k * 18;
        char name[256];
        if (rd32(e) == 0) snprintf(name, sizeof name, "%s", (const char *)strtab + rd32(e + 4));
        else { memcpy(name, e, 8); name[8] = 0; }
        int16_t secno = (int16_t)rd16(e + 12);
        uint32_t value = rd32(e + 8);
        uint8_t cls = e[16], aux = e[17];
        if (secno > 0 && secno <= (int)nsec) addr[k] = use[secno] ? (uint64_t)(uintptr_t)(mem + off[secno]) + value : 0;
        else if (secno == 0 && cls == 2) {                                    /* external, defined by the program */
            void *p = find_runtime_symbol(name);
            if (!p) { snprintf(err, errlen, "%s needs %s, which this version of the program does not have", path, name); free(addr); goto fail; }
            addr[k] = (uint64_t)(uintptr_t)p;
        } else if (secno == -1) addr[k] = value;                              /* absolute */
        if (secno > 0 && cls == 2) {
            if (!strcmp(name, "dtu_cstart")) dtu_cstart_ptr = (void *)(uintptr_t)addr[k];
            else if (!strcmp(name, "dtu_data_base")) dtu_data_ptr = (uint8_t *)(uintptr_t)addr[k];
            else if (!strcmp(name, "stack_start")) dtu_stack_top = (uint8_t *)(uintptr_t)addr[k];
        }
        k += aux;
    }
    if (!dtu_cstart_ptr || !dtu_data_ptr || !dtu_stack_top) { snprintf(err, errlen, "%s lacks dtu_cstart, dtu_data_base or stack_start", path); free(addr); goto fail; }

    /* relocations */
    unsigned long nrel_total = 0;
    for (unsigned i = 0; i < nsec; i++) {
        const uint8_t *s = sh + 40 * i;
        uint32_t ch = rd32(s + 36), roff = rd32(s + 24);
        uint32_t nrel = rd16(s + 32);
        if (!use[i + 1] || !nrel) continue;
        const uint8_t *r = b + roff;
        if ((ch & 0x01000000) && nrel == 0xFFFF) { nrel = rd32(r) - 1; r += 10; }   /* LNK_NRELOC_OVFL */
        if (roff + (size_t)nrel * 10 > (size_t)len) { snprintf(err, errlen, "%s: damaged relocations", path); free(addr); goto fail; }
        uint8_t *base = mem + off[i + 1];
        for (uint32_t k = 0; k < nrel; k++, r += 10) {
            uint32_t va = rd32(r), si = rd32(r + 4);
            uint16_t type = rd16(r + 8);
            if (si >= nsym) { snprintf(err, errlen, "%s: bad relocation", path); free(addr); goto fail; }
            uint8_t *p = base + va;
            uint64_t S = addr[si];
            int64_t v;
            switch (type) {
            case 1: { uint64_t a; memcpy(&a, p, 8); a += S; memcpy(p, &a, 8); break; }            /* ADDR64 */
            case 2: { uint32_t a; memcpy(&a, p, 4); uint64_t t = S + a;                           /* ADDR32 */
                      if (t > 0xFFFFFFFFu) { snprintf(err, errlen, "address above 4 GB"); free(addr); goto fail; }
                      a = (uint32_t)t; memcpy(p, &a, 4); break; }
            case 3: { uint32_t a; memcpy(&a, p, 4); a += (uint32_t)(S - (uint64_t)(uintptr_t)mem); memcpy(p, &a, 4); break; }   /* ADDR32NB */
            case 4: case 5: case 6: case 7: case 8: case 9: {                                       /* REL32, REL32_1..5 */
                int32_t a; memcpy(&a, p, 4);
                v = (int64_t)S + a - ((int64_t)(uintptr_t)p + 4 + (type - 4));
                if (v < INT32_MIN || v > INT32_MAX) { snprintf(err, errlen, "relative jump out of range"); free(addr); goto fail; }
                a = (int32_t)v; memcpy(p, &a, 4); break; }
            default: snprintf(err, errlen, "%s: relocation type %u not supported", path, type); free(addr); goto fail;
            }
        }
        nrel_total += nrel;
    }
    free(addr);

    /* unwind information for the translated code */
    for (unsigned i = 0; i < nsec; i++) {
        const uint8_t *s = sh + 40 * i;
        if (use[i + 1] && !memcmp(s, ".pdata\0", 7)) {
            DWORD n = rd32(s + 16) / 12;
            if (n && !RtlAddFunctionTable((PRUNTIME_FUNCTION)(mem + off[i + 1]), n, (DWORD64)(uintptr_t)mem))
                dtu_log("[load] RtlAddFunctionTable failed");
        }
    }
    FlushInstructionCache(GetCurrentProcess(), mem, total);
    dtu_log("[load] %s: %zu bytes at %p, %lu relocations, data at %p", path, total, (void *)mem, nrel_total, (void *)dtu_data_ptr);
    free(b);
    return 1;
fail:
    free(b);
    if (mem) VirtualFree(mem, 0, MEM_RELEASE);
    return 0;
}
#endif
