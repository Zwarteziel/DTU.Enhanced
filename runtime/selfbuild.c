/* Undermountain Enhanced - self-building version: prepare the game.

   The program contains no code of the game. At the first start, and again
   when DTUGAME.EXE or this program changes, it translates the player's own
   DTUGAME.EXE (version 1.3) with the tools that come with it, in the folder
   "enhanced" next to the exe:

     enhanced/SR.exe          M-HT's static recompiler (MIT)
     enhanced/nasm.exe        the Netwide Assembler (BSD)
     enhanced/recomp/         our input for SR (the .sci files, SR.cfg, x64)
     enhanced/cache/          the result: dtu-main.obj, stamp.txt, logs

   The steps are those of recomp/build-x64.sh, with its sed and Python steps
   done here:
     1. SR translates DTUGAME.EXE to dtu-main.asm + seg01.inc + seg02.inc;
     2. a few SR output patterns are fixed (loop/rep outsb with "near") and
        long runs of identical "db" or "resb 1" lines are compacted;
     3. nasm assembles a win64 object; short jumps it reports as out of range
        become near jumps, and nasm runs again (at most 6 times).
   coffload.c then loads dtu-main.obj. A small window shows the progress. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <zlib.h>
#ifdef _WIN32
#include <windows.h>
#include <SDL3/SDL.h>
#include "coffload.h"
#else
#include <sys/wait.h>
#endif
#include "dtu.h"
#include "platform.h"
#include "version.h"
#include "selfbuild.h"

#ifdef _WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif

static char logpath[1200];
static FILE *blog;                      /* enhanced/cache/build.log */
static volatile int step;               /* 1..4 for the window, 5 = done */
static volatile int failed;
static char failmsg[1024];

static void say(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt); vsnprintf(buf, sizeof buf, fmt, ap); va_end(ap);
    dtu_log("[build] t=%.1f %s", plat_now(), buf);
    if (blog) { fprintf(blog, "%.1f s: %s\n", plat_now(), buf); fflush(blog); }
}

static void fail(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt); vsnprintf(failmsg, sizeof failmsg, fmt, ap); va_end(ap);
    say("error: %s", failmsg);
    failed = 1;
}

/* ---------- files ---------- */

static char *read_file(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *b = n >= 0 ? malloc((size_t)n + 1) : NULL;
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) { b[n] = 0; *len = (size_t)n; }
    return b;
}

/* growing output buffer */
typedef struct { char *p; size_t n, cap; int bad; } obuf;
static void put(obuf *o, const char *s, size_t n)
{
    if (o->bad) return;
    if (o->n + n + 1 > o->cap) {
        size_t c = o->cap ? o->cap * 2 : 1 << 20;
        while (c < o->n + n + 1) c *= 2;
        char *q = realloc(o->p, c);
        if (!q) { o->bad = 1; return; }
        o->p = q; o->cap = c;
    }
    memcpy(o->p + o->n, s, n);
    o->n += n;
}
static void puts_(obuf *o, const char *s) { put(o, s, strlen(s)); }

static int write_file(const char *path, const obuf *o)
{
    if (o->bad) return 0;
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    int ok = fwrite(o->p, 1, o->n, f) == o->n;
    return fclose(f) == 0 && ok;
}

/* next line of b (LF or CRLF), without the line end; 0 at the end */
static int next_line(const char *b, size_t len, size_t *pos, const char **line, size_t *ll)
{
    if (*pos >= len) return 0;
    const char *s = b + *pos, *e = memchr(s, '\n', len - *pos);
    size_t n = e ? (size_t)(e - s) : len - *pos;
    *pos += n + (e ? 1 : 0);
    if (n && s[n - 1] == '\r') n--;
    *line = s; *ll = n;
    return 1;
}

static int starts(const char *s, size_t n, const char *p)
{
    size_t k = strlen(p);
    return n >= k && !memcmp(s, p, k);
}

/* the sed rules of build-x64.sh for one line of seg01.inc:
     s/^a32 (near )+loop /LOOP_FAR /; s/^LOOP_FAR (near )+/LOOP_FAR /; s/^(near )+REP_OUTSB/REP_OUTSB/
   result in out (room for n bytes); returns its length */
static size_t fix_line(const char *s, size_t n, char *out)
{
    size_t k;
    if (starts(s, n, "a32 near ")) {
        k = 4;
        while (starts(s + k, n - k, "near ")) k += 5;
        if (starts(s + k, n - k, "loop ")) {
            memmove(out + 9, s + k + 5, n - k - 5);
            memcpy(out, "LOOP_FAR ", 9);
            s = out; n = 9 + n - k - 5;
        }
    }
    if (starts(s, n, "LOOP_FAR near ")) {
        k = 9;
        while (starts(s + k, n - k, "near ")) k += 5;
        memmove(out + 9, s + k, n - k);
        memcpy(out, "LOOP_FAR ", 9);
        return 9 + n - k;
    }
    if (starts(s, n, "near ")) {
        k = 0;
        while (starts(s + k, n - k, "near ")) k += 5;
        if (starts(s + k, n - k, "REP_OUTSB")) { memmove(out, s + k, n - k); return n - k; }
    }
    if (s != out) memcpy(out, s, n);
    return n;
}

/* the sed rules (seg01.inc only) and compact_source.py: a run of identical
   "db ..." lines becomes "times N db ...", a run of "resb 1" lines "resb N";
   runs of two stay as they are */
static int fix_and_compact(const char *path, int sed)
{
    size_t len, pos = 0, ll, rl = 0;
    char *b = read_file(path, &len);
    if (!b) { fail("cannot read %s", path); return 0; }
    obuf o = { 0 };
    char *line = malloc(len + 32), *rep = malloc(len + 32);
    const char *s;
    long num = 0;
    if (!line || !rep) { free(b); free(line); free(rep); fail("out of memory"); return 0; }
    for (;;) {
        int more = next_line(b, len, &pos, &s, &ll);
        size_t n = 0;
        if (more) { if (sed) n = fix_line(s, ll, line); else { memcpy(line, s, ll); n = ll; } }
        if (num) {
            if (more && n == rl && !memcmp(line, rep, n)) { num++; continue; }
            char hdr[64];
            if (num == 1) { put(&o, rep, rl); put(&o, "\n", 1); }
            else if (starts(rep, rl, "resb")) { snprintf(hdr, sizeof hdr, "resb %ld\n", num); puts_(&o, hdr); }
            else if (num == 2) { put(&o, rep, rl); put(&o, "\n", 1); put(&o, rep, rl); put(&o, "\n", 1); }
            else { snprintf(hdr, sizeof hdr, "times %ld ", num); puts_(&o, hdr); put(&o, rep, rl); put(&o, "\n", 1); }
            num = 0;
        }
        if (!more) break;
        size_t a = 0, z = n;                                    /* "resb 1" with any white space around it */
        while (a < z && (line[a] == ' ' || line[a] == '\t')) a++;
        while (z > a && (line[z - 1] == ' ' || line[z - 1] == '\t')) z--;
        if (starts(line, n, "db ") || (z - a == 6 && !memcmp(line + a, "resb 1", 6))) {
            num = 1;
            memcpy(rep, line, n); rl = n;
        } else { put(&o, line, n); put(&o, "\n", 1); }
    }
    free(b); free(line); free(rep);
    int ok = write_file(path, &o);
    free(o.p);
    if (!ok) fail("cannot write %s", path);
    return ok;
}

/* dtu-main.asm: the code section must be writable (as in build-x64.sh) */
static int fix_main(const char *path)
{
    static const char *from = "section cseg01 progbits alloc exec nowrite";
    size_t len, pos = 0, ll;
    const char *s;
    char *b = read_file(path, &len);
    if (!b) { fail("cannot read %s", path); return 0; }
    obuf o = { 0 };
    while (next_line(b, len, &pos, &s, &ll)) {
        if (starts(s, ll, from)) { puts_(&o, "section cseg01 progbits alloc exec write"); put(&o, s + strlen(from), ll - strlen(from)); }
        else put(&o, s, ll);
        put(&o, "\n", 1);
    }
    free(b);
    int ok = write_file(path, &o);
    free(o.p);
    if (!ok) fail("cannot write %s", path);
    return ok;
}

/* repair_short_jumps.py: for each "FILE:LINE: error: short jump is out of
   range" in nasm's error file, "jmp short X" becomes "jmp X", "jcc short X"
   becomes "jcc near X", anything else gets "near" after its first word.
   Returns the number of lines changed, -1 on an error. */
typedef struct { char file[64]; long line; } jumpfix;
static int cmp_fix(const void *a, const void *b)
{
    const jumpfix *x = a, *y = b;
    int c = strcmp(x->file, y->file);
    return c ? c : (x->line > y->line) - (x->line < y->line);
}
static long repair_short_jumps(const char *dir, const char *errfile)
{
    size_t len, pos = 0, ll;
    const char *s;
    char *e = read_file(errfile, &len);
    if (!e) return 0;
    jumpfix *fx = NULL;
    long nfx = 0, cap = 0;
    while (next_line(e, len, &pos, &s, &ll)) {
        const char *c1 = memchr(s, ':', ll);
        if (!c1 || c1 == s || c1 - s >= 63) continue;
        const char *c2 = memchr(c1 + 1, ':', ll - (size_t)(c1 + 1 - s));
        if (!c2 || !starts(c2 + 1, ll - (size_t)(c2 + 1 - s), " error: short jump is out of range")) continue;
        if (nfx == cap) { cap = cap ? cap * 2 : 256; jumpfix *q = realloc(fx, sizeof *fx * (size_t)cap); if (!q) break; fx = q; }
        memcpy(fx[nfx].file, s, (size_t)(c1 - s)); fx[nfx].file[c1 - s] = 0;
        fx[nfx].line = strtol(c1 + 1, NULL, 10);
        nfx++;
    }
    free(e);
    if (!nfx) { free(fx); return 0; }
    qsort(fx, (size_t)nfx, sizeof *fx, cmp_fix);
    long changed = 0;
    for (long i = 0; i < nfx;) {
        long j = i;
        while (j < nfx && !strcmp(fx[j].file, fx[i].file)) j++;
        if (strchr(fx[i].file, '/') || strchr(fx[i].file, '\\') || strstr(fx[i].file, "..")) { i = j; continue; }
        char path[1300];
        snprintf(path, sizeof path, "%s" SEP "%s", dir, fx[i].file);
        char *b = read_file(path, &len);
        if (!b) { fail("cannot read %s", path); free(fx); return -1; }
        obuf o = { 0 };
        long ln = 0, k = i;
        pos = 0;
        while (next_line(b, len, &pos, &s, &ll)) {
            ln++;
            while (k < j && fx[k].line < ln) k++;
            if (k < j && fx[k].line == ln) {
                changed++;
                if (starts(s, ll, "jmp short ")) { puts_(&o, "jmp"); put(&o, s + 9, ll - 9); }
                else {
                    const char *sp = memchr(s, ' ', ll);
                    size_t first = sp ? (size_t)(sp - s) + 1 : ll;
                    const char *sec = s + first;
                    size_t sl = ll - first;
                    while (sl && (*sec == ' ' || *sec == '\t')) { sec++; sl--; }
                    put(&o, s, first);
                    puts_(&o, "near ");
                    if (s[0] == 'j' && starts(sec, sl, "short ")) put(&o, sec + 6, sl - 6);
                    else put(&o, sec, sl);
                }
            } else put(&o, s, ll);
            put(&o, "\n", 1);
        }
        free(b);
        int ok = write_file(path, &o);
        free(o.p);
        if (!ok) { fail("cannot write %s", path); free(fx); return -1; }
        i = j;
    }
    free(fx);
    return changed;
}

/* ---------- running SR and nasm ---------- */

/* run exe in dir with args, stdout and stderr to logfile; returns the exit code or -1 */
static int run(const char *dir, const char *exe, const char *args, const char *logfile)
{
#ifdef _WIN32
    char cmd[4096];
    snprintf(cmd, sizeof cmd, "\"%s\" %s", exe, args);
    SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
    HANDLE h = CreateFileA(logfile, GENERIC_WRITE, FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return -1;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = NULL; si.hStdOutput = h; si.hStdError = h;
    BOOL ok = CreateProcessA(exe, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, dir, &si, &pi);
    CloseHandle(h);
    if (!ok) return -1;
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return (int)code;
#else
    char cmd[4096];
    snprintf(cmd, sizeof cmd, "cd '%s' && '%s' %s >'%s' 2>&1", dir, exe, args, logfile);
    int r = system(cmd);
    return r == -1 ? -1 : WIFEXITED(r) ? WEXITSTATUS(r) : -1;
#endif
}

static void remove_in(const char *dir, const char *name)
{
    char p[1300];
    snprintf(p, sizeof p, "%s" SEP "%s", dir, name);
    remove(p);
}

static char e_dir[1100], r_dir[1150], c_dir[1150], game_exe[1200], obj_path[1200], stamp_path[1200], stamp_text[512];
static unsigned long game_size;
#define DTUGAME_13_SIZE 1760375ul       /* DTUGAME.EXE of version 1.3, the one the files in recomp describe */

/* the build (a worker thread on Windows) */
static int build(void *unused)
{
    char p[1300], q[1300], args[1400], tool[1200], nlog[1200];
    (void)unused;
    say("translating %s", game_exe);
    step = 1;
    snprintf(tool, sizeof tool, "%s" SEP "SR.exe", e_dir);
    snprintf(args, sizeof args, "\"%s\" dtu-main.asm", game_exe);
    snprintf(p, sizeof p, "%s" SEP "sr.log", c_dir);
    int r = run(r_dir, tool, args, p);
    if (r != 0) { fail("SR could not translate DTUGAME.EXE (exit code %d); see %s", r, p); return 0; }

    step = 2;
    say("preparing the translation");
    snprintf(p, sizeof p, "%s" SEP "dtu-main.asm", r_dir);
    if (!fix_main(p)) return 0;
    snprintf(q, sizeof q, "%s" SEP "seg02.inc", r_dir);
    if (!fix_and_compact(q, 0)) return 0;
    snprintf(p, sizeof p, "%s" SEP "seg01.inc", r_dir);
    if (!fix_and_compact(p, 1)) return 0;

    step = 3;
    snprintf(tool, sizeof tool, "%s" SEP "nasm.exe", e_dir);
    snprintf(args, sizeof args, "-fwin64 -O1 -w+orphan-labels -w-number-overflow -w-prefix-seg -ix64/ -Z a.a dtu-main.asm -o dtu-main.obj");
    snprintf(q, sizeof q, "%s" SEP "a.a", r_dir);
    snprintf(nlog, sizeof nlog, "%s" SEP "nasm.log", c_dir);
    for (int pass = 1; ; pass++) {
        say("assembling (pass %d)", pass);
        r = run(r_dir, tool, args, nlog);
        if (r == 0) break;
        long n = pass < 6 ? repair_short_jumps(r_dir, q) : 0;
        if (n < 0) return 0;
        if (n == 0) {
            size_t len;
            char *e = read_file(q, &len), first[300] = "";
            if (e) { size_t k = 0; while (k < len && k < sizeof first - 1 && e[k] != '\n' && e[k] != '\r') { first[k] = e[k]; k++; } first[k] = 0; free(e); }
            fail("nasm could not assemble the translation (exit code %d): %s", r, first[0] ? first : "see nasm.log");
            return 0;
        }
        say("%ld short jumps made near", n);
        if (!fix_and_compact(p, 1)) return 0;               /* the sed rules again, as build-x64.sh does */
    }

    step = 4;
    snprintf(p, sizeof p, "%s" SEP "dtu-main.obj", r_dir);
    remove(obj_path);
    if (rename(p, obj_path) != 0) { fail("cannot move dtu-main.obj to %s", c_dir); return 0; }
    FILE *f = fopen(stamp_path, "wb");
    if (!f || fputs(stamp_text, f) < 0) { if (f) fclose(f); fail("cannot write %s", stamp_path); return 0; }
    fclose(f);
    remove_in(r_dir, "dtu-main.asm"); remove_in(r_dir, "seg01.inc"); remove_in(r_dir, "seg02.inc"); remove_in(r_dir, "a.a");
    say("done");
    step = 5;
    return 1;
}

static int same_stamp(void)
{
    size_t len;
    char *s = read_file(stamp_path, &len);
    int same = s && !strcmp(s, stamp_text);
    free(s);
    FILE *f = fopen(obj_path, "rb");
    if (!f) return 0;
    fclose(f);
    return same;
}

/* the paths, and the stamp: this program (version, build time) and DTUGAME.EXE (size, CRC) */
static int make_paths(const char *game_dir)
{
    snprintf(e_dir, sizeof e_dir, "%s" SEP "enhanced", game_dir);
    snprintf(r_dir, sizeof r_dir, "%s" SEP "recomp", e_dir);
    snprintf(c_dir, sizeof c_dir, "%s" SEP "cache", e_dir);
    snprintf(game_exe, sizeof game_exe, "%s" SEP "DTUGAME.EXE", game_dir);
    snprintf(obj_path, sizeof obj_path, "%s" SEP "dtu-main.obj", c_dir);
    snprintf(stamp_path, sizeof stamp_path, "%s" SEP "stamp.txt", c_dir);
    snprintf(logpath, sizeof logpath, "%s" SEP "build.log", c_dir);
    plat_mkdir(c_dir);
    size_t len;
    char *x = read_file(game_exe, &len);
    if (!x) return 0;
    unsigned long crc = crc32(0L, (const Bytef *)x, (uInt)len);
    free(x);
    game_size = (unsigned long)len;
    snprintf(stamp_text, sizeof stamp_text, "Undermountain Enhanced %s, build %s\nDTUGAME.EXE %lu bytes, CRC %08lX\n",
             DTU_VERSION, DTU_BUILD, (unsigned long)len, crc);
    return 1;
}

/* test (Linux): build in game_dir/enhanced without a window */
int selfbuild_run(const char *game_dir)
{
    if (!make_paths(game_dir)) { fprintf(stderr, "no DTUGAME.EXE in %s\n", game_dir); return 0; }
    blog = fopen(logpath, "w");
    int ok = build(NULL);
    if (blog) { fclose(blog); blog = NULL; }
    if (!ok) fprintf(stderr, "%s\n", failmsg);
    return ok;
}

#ifdef _WIN32
/* a small window with the progress while the worker thread builds */
static void show_progress(SDL_Thread *t)
{
    SDL_Window *w = SDL_CreateWindow("Undermountain Enhanced", 720, 220, 0);
    SDL_Renderer *rr = w ? SDL_CreateRenderer(w, NULL) : NULL;
    double t0 = plat_now();
    static const char *steps[] = { "", "translating DTUGAME.EXE (SR)", "preparing the translation", "assembling (nasm)", "finishing", "done" };
    while (step < 5 && !failed) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {}                         /* closing the window waits for the build */
        if (rr) {
            char line[128];
            int s = step < 1 ? 1 : step > 4 ? 4 : step;
            SDL_SetRenderScale(rr, 1.0f, 1.0f);
            SDL_SetRenderDrawColor(rr, 28, 22, 16, 255);
            SDL_RenderClear(rr);
            SDL_SetRenderScale(rr, 2.0f, 2.0f);
            SDL_SetRenderDrawColor(rr, 232, 196, 104, 255);
            SDL_RenderDebugText(rr, 14, 14, "Undermountain Enhanced " DTU_VERSION);
            SDL_SetRenderDrawColor(rr, 220, 216, 208, 255);
            SDL_RenderDebugText(rr, 14, 34, "Preparing the game from your DTUGAME.EXE.");
            SDL_RenderDebugText(rr, 14, 46, "This takes less than a minute, only once.");
            snprintf(line, sizeof line, "Step %d of 4: %s", s, steps[s]);
            SDL_RenderDebugText(rr, 14, 70, line);
            snprintf(line, sizeof line, "%.0f s", plat_now() - t0);
            SDL_SetRenderDrawColor(rr, 150, 146, 140, 255);
            SDL_RenderDebugText(rr, 14, 84, line);
            SDL_RenderPresent(rr);
        }
        SDL_Delay(50);
    }
    int status;
    SDL_WaitThread(t, &status);
    if (rr) SDL_DestroyRenderer(rr);
    if (w) SDL_DestroyWindow(w);
}

int selfbuild_prepare(void)
{
    char err[600], msg[1800];
    if (!make_paths(dtu_game_dir)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Undermountain Enhanced",
                                 "DTUGAME.EXE was not found next to this program.\n\nPut DTU-Enhanced.exe, SDL3.dll and the folder \"enhanced\" in the folder of Descent to Undermountain.", NULL);
        return 0;
    }
    if (game_size != DTUGAME_13_SIZE) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Undermountain Enhanced",
                                 "Undermountain Enhanced needs DTUGAME.EXE of version 1.3 of the game. Install the 1.3 patch first.", NULL);
        return 0;
    }
    if (!same_stamp()) {
        static const char *need[] = { "SR.exe", "nasm.exe", "recomp" SEP "SR.cfg", "recomp" SEP "instruction_replacements.sci", "recomp" SEP "x64" SEP "extern.inc" };
        for (unsigned i = 0; i < sizeof need / sizeof need[0]; i++) {
            char p[1300];
            snprintf(p, sizeof p, "%s" SEP "%s", e_dir, need[i]);
            FILE *f = fopen(p, "rb");
            if (f) { fclose(f); continue; }
            snprintf(msg, sizeof msg, "%s is missing.\n\nUnpack the whole Undermountain Enhanced zip file (with the folder \"enhanced\") into the folder of the game.", p);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Undermountain Enhanced", msg, NULL);
            return 0;
        }
        blog = fopen(logpath, "w");
        if (blog) fputs(stamp_text, blog);
        dtu_log("[build] the game has to be prepared");
        SDL_Thread *t = SDL_CreateThread(build, "build", NULL);
        if (!t) build(NULL);
        else show_progress(t);
        if (blog) { fclose(blog); blog = NULL; }
        if (failed) {
            snprintf(msg, sizeof msg, "Undermountain Enhanced could not prepare the game.\n\n%s\n\nDetails are in %s.", failmsg, logpath);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Undermountain Enhanced", msg, NULL);
            return 0;
        }
    }
    if (!coff_load(obj_path, err, sizeof err)) {
        remove(stamp_path);                                   /* prepare it again next time */
        dtu_log("[build] cannot load %s: %s", obj_path, err);
        snprintf(msg, sizeof msg, "Undermountain Enhanced could not load the prepared game:\n\n%s\n\nIt will be prepared again at the next start.", err);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Undermountain Enhanced", msg, NULL);
        return 0;
    }
    return 1;
}
#endif
