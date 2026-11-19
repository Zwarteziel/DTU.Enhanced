/* Undermountain Enhanced - DOS services (int 21h, int 2Fh) on top of the host file system */
#define _GNU_SOURCE
#include "dtu.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include "platform.h"
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define localtime_r(t, tm) localtime_s(tm, t)
#define ftruncate _chsize
#define mkdir(p, m) _mkdir(p)
#define OPEN_FLAGS O_BINARY
#else
#define OPEN_FLAGS 0
#endif

#define MAXH 64
static int htab[MAXH];             /* DOS handle -> host fd (-1 = free) */
static char hname[MAXH][64];
static int cur_drive = 2;          /* C: */
static char cwd[26][128];          /* per drive, DOS style without drive, e.g. "\" */
static uint32_t dta_addr;
static int cd_letter = 3;          /* D: */

/* findfirst state, kept per DTA */
typedef struct { uint32_t dta; char dir[1024]; char pat[16]; int attr; DIR *d; } find_state;
static find_state fs[8];

void dos_init(void)
{
    for (int i = 0; i < MAXH; i++) htab[i] = -1;
    htab[0] = 0; htab[1] = 1; htab[2] = 2; htab[3] = -1; htab[4] = -1;
    strcpy(hname[0], "stdin"); strcpy(hname[1], "stdout"); strcpy(hname[2], "stderr");
    for (int i = 0; i < 26; i++) strcpy(cwd[i], "\\");
    dta_addr = (PSP_SEG << 4) + 0x80;
}

static int dos_err(dtu_regs *r, int code)
{
    EAX = (EAX & 0xffff0000u) | (uint16_t)code;
    SET_CF();
    return 0;
}

static int errno_to_dos(int e)
{
    switch (e) {
    case ENOENT: return 2;
    case ENOTDIR: return 3;
    case EMFILE: return 4;
    case EACCES: case EPERM: case EROFS: return 5;
    case EEXIST: return 0x50;
    default: return 0x1F;
    }
}

/* case-insensitive lookup of one name inside a host directory */
static int find_ci(const char *dir, const char *name, char *out, size_t outlen)
{
    DIR *d = opendir(dir);
    struct dirent *de;
    if (!d) return 0;
    while ((de = readdir(d))) {
        if (!strcasecmp(de->d_name, name)) {
            snprintf(out, outlen, "%s", de->d_name);
            closedir(d);
            return 1;
        }
    }
    closedir(d);
    return 0;
}

/* DOS path -> host path. Drive C: is the game directory, the cd drive is the
   cd directory. must_exist=0 allows a missing last component (create). */
int dos_host_path(const char *dospath, char *out, size_t outlen, int must_exist)
{
    char full[260], comp[260], name[260];
    int drive = cur_drive;
    const char *p = dospath;
    const char *root;

    if (p[0] && p[1] == ':') { drive = toupper((unsigned char)p[0]) - 'A'; p += 2; }
    if (*p == '\\' || *p == '/') snprintf(full, sizeof full, "%s", p);
    else snprintf(full, sizeof full, "%s%s%s", cwd[drive], strcmp(cwd[drive], "\\") ? "\\" : "", p);

    if (drive == cd_letter && dtu_cd_dir[0]) root = dtu_cd_dir;
    else if (drive == 2) root = dtu_game_dir;
    else return 0;

    snprintf(out, outlen, "%s", root);
    {
        char *s = full;
        while (*s) {
            size_t n = 0;
            while (*s == '\\' || *s == '/') s++;
            if (!*s) break;
            while (*s && *s != '\\' && *s != '/' && n < sizeof comp - 1) comp[n++] = *s++;
            comp[n] = 0;
            if (!strcmp(comp, ".")) continue;
            if (!strcmp(comp, "..")) { char *q = strrchr(out, '/'); if (q && q > out + strlen(root) - 1) *q = 0; continue; }
            /* "C:\DTU\..." : the game may name its own install dir; the root already is it */
            if (find_ci(out, comp, name, sizeof name)) {
                size_t l = strlen(out);
                snprintf(out + l, outlen - l, "/%s", name);
            } else {
                int last = (*s == 0) || (s[strspn(s, "\\/")] == 0);
                if (must_exist || !last) {
                    /* tolerate a leading install-dir name such as \DTU on C: */
                    if (drive == 2 && !strcasecmp(comp, "DTU") && strlen(out) == strlen(root)) continue;
                    return 0;
                }
                size_t l = strlen(out);
                for (char *c = comp; *c; c++) *c = toupper((unsigned char)*c);
                snprintf(out + l, outlen - l, "/%s", comp);
            }
        }
    }
    return 1;
}

static int new_handle(int fd, const char *name)
{
    for (int i = 5; i < MAXH; i++)
        if (htab[i] < 0) { htab[i] = fd; snprintf(hname[i], sizeof hname[i], "%s", name); return i; }
    return -1;
}

static void dos_time(time_t t, uint16_t *dtime, uint16_t *ddate)
{
    struct tm tmv;
    localtime_r(&t, &tmv);
    *dtime = (uint16_t)((tmv.tm_hour << 11) | (tmv.tm_min << 5) | (tmv.tm_sec / 2));
    *ddate = (uint16_t)(((tmv.tm_year - 80) << 9) | ((tmv.tm_mon + 1) << 5) | tmv.tm_mday);
}

static int wild_match(const char *pat, const char *name)
{
    /* DOS 8.3 matching: compare base and extension separately */
    char pb[16] = "", pe[8] = "", nb[256] = "", ne[256] = "";
    const char *d;
    d = strchr(pat, '.');
    if (d) { snprintf(pb, sizeof pb, "%.*s", (int)(d - pat), pat); snprintf(pe, sizeof pe, "%s", d + 1); }
    else { snprintf(pb, sizeof pb, "%s", pat); strcpy(pe, ""); }
    d = strrchr(name, '.');
    if (d && d != name) { snprintf(nb, sizeof nb, "%.*s", (int)(d - name), name); snprintf(ne, sizeof ne, "%s", d + 1); }
    else { snprintf(nb, sizeof nb, "%s", name); ne[0] = 0; }
    if (strlen(nb) > 8 || strlen(ne) > 3) return 0;
    for (int part = 0; part < 2; part++) {
        const char *p = part ? pe : pb, *n = part ? ne : nb;
        if (part && !d && !strcmp(pe, "*")) continue;
        while (*p) {
            if (*p == '*') { n = ""; p = ""; break; }
            if (*p == '?') { p++; if (*n) n++; continue; }
            if (toupper((unsigned char)*p) != toupper((unsigned char)*n)) return 0;
            p++; n++;
        }
        if (*n) return 0;
    }
    return 1;
}

static int find_next(find_state *f, dtu_regs *r)
{
    struct dirent *de;
    uint8_t *dta = PTR(f->dta);
    while (f->d && (de = readdir(f->d))) {
        char hp[1400];
        struct stat st;
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;
        if (!wild_match(f->pat, de->d_name)) continue;
        snprintf(hp, sizeof hp, "%s/%s", f->dir, de->d_name);
        if (stat(hp, &st)) continue;
        if (S_ISDIR(st.st_mode) && !(f->attr & 0x10)) continue;
        uint16_t t, dd;
        dos_time(st.st_mtime, &t, &dd);
        dta[0x15] = S_ISDIR(st.st_mode) ? 0x10 : 0x20;
        memcpy(dta + 0x16, &t, 2);
        memcpy(dta + 0x18, &dd, 2);
        uint32_t sz = (uint32_t)st.st_size;
        memcpy(dta + 0x1A, &sz, 4);
        memset(dta + 0x1E, 0, 13);
        for (int i = 0; i < 12 && de->d_name[i]; i++) dta[0x1E + i] = (uint8_t)toupper((unsigned char)de->d_name[i]);
        CLR_CF();
        return 1;
    }
    if (f->d) { closedir(f->d); f->d = NULL; }
    dos_err(r, 0x12);
    return 0;
}

static void dos_int21_call(dtu_regs *r);

/* DOS calls run on the game thread; a slow one (a disk or virus scanner that
   takes its time) holds up the game, so those are logged */
void dos_int21(dtu_regs *r)
{
    static int nlog;
    uint8_t ah = AH;
    double t = plat_now();
    dos_int21_call(r);
    t = plat_now() - t;
    if (t > 0.05 && nlog < 30) { nlog++; dtu_log("[dos] function %02Xh took %.0f ms", ah, t * 1000); }
}

static void dos_int21_call(dtu_regs *r)
{
    char hp[1200];
    uint8_t ah = AH;
    CLR_CF();
    switch (ah) {
    case 0x02: /* char out */
        dtu_log("[dos] putc '%c'", DL);
        break;
    case 0x09: { /* string out, $-terminated */
        char *s = PTR(EDX), buf[256]; int n = 0;
        while (s[n] != '$' && n < 255) { buf[n] = s[n]; n++; }
        buf[n] = 0;
        dtu_log("[dos] print \"%s\"", buf);
        break; }
    case 0x0E: cur_drive = DL; AL = 26; break;
    case 0x19: AL = (uint8_t)cur_drive; break;
    case 0x1A: dta_addr = EDX; break;
    case 0x2F: EBX = dta_addr; break;
    case 0x25: dpmi_set_vector(AL, EDX); break;
    case 0x35: EBX = dpmi_get_vector(AL); break;
    case 0x2A: { time_t t = time(NULL); struct tm tm; localtime_r(&t, &tm);
        CX = (uint16_t)(tm.tm_year + 1900); DH = (uint8_t)(tm.tm_mon + 1); DL = (uint8_t)tm.tm_mday; AL = (uint8_t)tm.tm_wday; break; }
    case 0x2C: { io_idle_poll(); time_t t = time(NULL); struct tm tm; localtime_r(&t, &tm);
        double f = plat_now();
        CH = (uint8_t)tm.tm_hour; CL = (uint8_t)tm.tm_min; DH = (uint8_t)tm.tm_sec; DL = (uint8_t)((int)(f * 100) % 100); break; }
    case 0x2B: case 0x2D: AL = 0; break;
    case 0x30: EAX = 0x00001606; /* DOS 6.22 */ EBX = 0; ECX = 0; break;
    case 0x33: DL = 0; break;
    case 0x36: AX = 64; BX = 0x7FFF; CX = 512; DX = 0xFFFF; break;
    case 0x3B: { /* chdir */
        const char *p = PTR(EDX);
        int dr = cur_drive;
        if (p[0] && p[1] == ':') { dr = toupper((unsigned char)p[0]) - 'A'; p += 2; }
        if (dr < 0 || dr >= 26 || strlen(p) > 100) { dos_err(r, 3); break; }
        if (*p == '\\') snprintf(cwd[dr], sizeof cwd[dr], "%s", p);
        else if (*p) { char t[400]; snprintf(t, sizeof t, "%s%s%s", cwd[dr], strcmp(cwd[dr], "\\") ? "\\" : "", p); snprintf(cwd[dr], sizeof cwd[dr], "%.120s", t); }
        dtu_log("[dos] chdir %s -> %c:%s", (char *)PTR(EDX), 'A' + dr, cwd[dr]);
        break; }
    case 0x39: case 0x3A:
        if (!dos_host_path(PTR(EDX), hp, sizeof hp, ah == 0x3A)) { dos_err(r, 3); break; }
        if ((ah == 0x39 ? mkdir(hp, 0755) : rmdir(hp)) != 0 && errno != EEXIST) dos_err(r, errno_to_dos(errno));
        dtu_log("[dos] %s %s", ah == 0x39 ? "mkdir" : "rmdir", (char *)PTR(EDX));
        break;
    case 0x47: { /* getcwd */
        int dr = DL ? DL - 1 : cur_drive;
        snprintf(PTR(ESI), 64, "%s", cwd[dr] + 1);
        break; }
    case 0x3C: case 0x3D: case 0x5B: {
        const char *name = PTR(EDX);
        int flags, fd, h;
        if (ah == 0x3D) {
            int m = AL & 3;
            /* HMI SOS sound drivers are 16/32-bit DOS code loaded at run time;
               they cannot run natively. Without them the game runs silently. */
            {
                const char *b = strrchr(name, '\\'); b = b ? b + 1 : name;
                if (!strncasecmp(b, "HMI", 3) && strlen(b) > 4 && !strcasecmp(b + strlen(b) - 4, ".386")) {
                    dtu_log("[dos] open %s -> hidden (sound drivers are not supported yet)", name);
                    dos_err(r, 2);
                    break;
                }
            }
            flags = m == 0 ? O_RDONLY : m == 1 ? O_WRONLY : O_RDWR;
            if (!dos_host_path(name, hp, sizeof hp, 1)) { dtu_log("[dos] open %s (mode %d) -> not found", name, AL); dos_err(r, 2); break; }
        } else {
            flags = O_RDWR | O_CREAT | (ah == 0x5B ? O_EXCL : O_TRUNC);
            if (!dos_host_path(name, hp, sizeof hp, 0)) { dtu_log("[dos] create %s -> no path", name); dos_err(r, 3); break; }
        }
        fd = open(hp, flags | OPEN_FLAGS, 0644);
        if (fd < 0) { dtu_log("[dos] %s %s -> errno %d", ah == 0x3D ? "open" : "create", name, errno); dos_err(r, errno_to_dos(errno)); break; }
        h = new_handle(fd, name);
        if (h < 0) { close(fd); dos_err(r, 4); break; }
        dtu_log("[dos] %s %s -> handle %d (%s)", ah == 0x3D ? "open" : "create", name, h, hp);
        EAX = (uint32_t)h;
        break; }
    case 0x6C: { /* extended open/create */
        const char *name = PTR(ESI);
        int fd, h, act = DL, flags;
        int m = BL & 3;
        flags = m == 0 ? O_RDONLY : m == 1 ? O_WRONLY : O_RDWR;
        int exists = dos_host_path(name, hp, sizeof hp, 1);
        if (!exists) {
            if (!(act & 0x10) || !dos_host_path(name, hp, sizeof hp, 0)) { dos_err(r, 2); break; }
            flags |= O_CREAT; ECX = 2;
        } else if ((act & 3) == 2) { flags |= O_TRUNC; ECX = 3; }
        else ECX = 1;
        fd = open(hp, flags | OPEN_FLAGS, 0644);
        if (fd < 0) { dos_err(r, errno_to_dos(errno)); break; }
        h = new_handle(fd, name);
        dtu_log("[dos] extopen %s -> handle %d", name, h);
        EAX = (uint32_t)h;
        break; }
    case 0x3E:
        if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
        if (BX > 4) { close(htab[BX]); htab[BX] = -1; }
        break;
    case 0x3F: {
        if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
        if (BX == 0) { EAX = 0; break; }
        ssize_t n = read(htab[BX], PTR(EDX), ECX);
        if (n < 0) { dos_err(r, 5); break; }
        if (dtu_verbose > 1) dtu_log("[dos] read %d (%s) %u -> %zd", BX, hname[BX], ECX, n);
        EAX = (uint32_t)n;
        break; }
    case 0x40: {
        if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
        if (BX == 1 || BX == 2) {
            char buf[512]; size_t n = ECX < 511 ? ECX : 511;
            memcpy(buf, PTR(EDX), n); buf[n] = 0;
            while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r')) buf[--n] = 0;
            dtu_log("[dos] %s: %s", BX == 1 ? "stdout" : "stderr", buf);
            EAX = ECX; break;
        }
        if (ECX == 0) { /* truncate at current position */
            off_t pos = lseek(htab[BX], 0, SEEK_CUR);
            if (ftruncate(htab[BX], pos)) {}
            EAX = 0; break;
        }
        ssize_t n = write(htab[BX], PTR(EDX), ECX);
        if (n < 0) { dos_err(r, 5); break; }
        EAX = (uint32_t)n;
        break; }
    case 0x41:
        if (!dos_host_path(PTR(EDX), hp, sizeof hp, 1) || unlink(hp)) { dos_err(r, 2); break; }
        dtu_log("[dos] delete %s", (char *)PTR(EDX));
        break;
    case 0x42: {
        if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
        off_t o = (off_t)(int32_t)(((uint32_t)CX << 16) | DX);
        off_t res = lseek(htab[BX], o, AL == 0 ? SEEK_SET : AL == 1 ? SEEK_CUR : SEEK_END);
        if (res < 0) { dos_err(r, 0x19); break; }
        AX = (uint16_t)res; DX = (uint16_t)(res >> 16);
        break; }
    case 0x43:
        if (!dos_host_path(PTR(EDX), hp, sizeof hp, 1)) { dtu_log("[dos] attr %s -> not found", (char *)PTR(EDX)); dos_err(r, 2); break; }
        if (AL == 0) { struct stat st; stat(hp, &st); CX = S_ISDIR(st.st_mode) ? 0x10 : 0x20; }
        break;
    case 0x44:
        switch (AL) {
        case 0x00:
            if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
            DX = BX <= 2 ? (BX == 0 ? 0x80C1 : 0x80C2) : (uint16_t)cur_drive;
            break;
        case 0x01: break;
        case 0x08: AX = (BL == cd_letter + 1) ? 0 : 1; break;
        case 0x09: DX = (BL == cd_letter + 1) ? 0x1000 : 0; break;
        case 0x0E: AL = 0; break;
        default: dtu_log("[dos] ioctl %02X", AL); dos_err(r, 1); break;
        }
        break;
    case 0x45: {
        if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
        int h = new_handle(dup(htab[BX]), hname[BX]);
        EAX = (uint32_t)h; break; }
    case 0x4E: {
        const char *spec = PTR(EDX);
        char dirpart[260], *bs;
        find_state *f = NULL;
        for (int i = 0; i < 8; i++) if (fs[i].dta == dta_addr) { f = &fs[i]; break; }
        if (!f) for (int i = 0; i < 8; i++) if (!fs[i].d) { f = &fs[i]; break; }
        if (!f) { f = &fs[0]; }
        if (f->d) { closedir(f->d); f->d = NULL; }
        snprintf(dirpart, sizeof dirpart, "%s", spec);
        bs = strrchr(dirpart, '\\');
        if (!bs) bs = strrchr(dirpart, ':');
        if (bs) { snprintf(f->pat, sizeof f->pat, "%s", bs + 1); bs[1] = 0; }
        else { snprintf(f->pat, sizeof f->pat, "%s", dirpart); strcpy(dirpart, "."); }
        f->dta = dta_addr; f->attr = CX;
        if (!dos_host_path(dirpart, f->dir, sizeof f->dir, 1)) { dtu_log("[dos] findfirst %s -> no dir", spec); dos_err(r, 3); break; }
        f->d = opendir(f->dir);
        if (find_next(f, r)) dtu_log("[dos] findfirst %s -> %s", spec, (char *)PTR(dta_addr + 0x1E));
        else { dtu_log("[dos] findfirst %s -> none", spec); dos_err(r, 2); }
        break; }
    case 0x4F: {
        find_state *f = NULL;
        for (int i = 0; i < 8; i++) if (fs[i].dta == dta_addr) { f = &fs[i]; break; }
        if (!f) { dos_err(r, 0x12); break; }
        find_next(f, r);
        break; }
    case 0x56: {
        char hp2[1200];
        if (!dos_host_path(PTR(EDX), hp, sizeof hp, 1) || !dos_host_path(PTR(EDI), hp2, sizeof hp2, 0) || rename(hp, hp2)) { dos_err(r, 2); break; }
        dtu_log("[dos] rename %s -> %s", (char *)PTR(EDX), (char *)PTR(EDI));
        break; }
    case 0x57:
        if (BX >= MAXH || htab[BX] < 0) { dos_err(r, 6); break; }
        if (AL == 0) { struct stat st; uint16_t t, d; fstat(htab[BX], &st); dos_time(st.st_mtime, &t, &d); CX = t; DX = d; }
        break;
    case 0x48: dtu_log("[dos] alloc %u paras", BX); { uint32_t mx; uint32_t s = dosmem_alloc(BX, &mx); if (s) AX = (uint16_t)s; else { BX = (uint16_t)mx; dos_err(r, 8); } } break;
    case 0x49: break;
    case 0x4A: break;
    case 0x4C:
        dtu_log("[dos] exit(%d)", AL);
        dtu_exit(AL);
        break;
    case 0x62: EBX = 0x24; break;
    case 0x68: break;
    case 0xFF:
        if (AL == 0x00 && DX == 0x0078) { EAX = 0xFFFF3447; break; } /* DOS/4GW present */
        /* fallthrough */
    default:
        dtu_log("[dos] unhandled int 21h AX=%04X BX=%04X CX=%04X DX=%04X", AX, BX, CX, DX);
        dos_err(r, 1);
        break;
    }
}

/* fake CD-ROM device driver header, reachable from real mode */
static void mscdex_init(void)
{
    static int done;
    uint8_t *h = PTR(CDDEV_ADDR);
    if (done) return;
    done = 1;
    memset(h, 0, 32);
    *(uint32_t *)h = 0xFFFFFFFFu;
    *(uint16_t *)(h + 4) = 0xC800;
    memcpy(h + 10, "MSCD001 ", 8);
    h[20] = (uint8_t)(cd_letter + 1);
    h[21] = 1;
}

/* device driver request (int 2Fh AX=1510h), request header at linear 'rq' */
static void mscdex_request(uint32_t rq)
{
    uint8_t *h = PTR(rq);
    uint8_t cmd = h[2];
    uint16_t status = 0x0100;                 /* done */
    dtu_log("[mscdex] request cmd %02X len %d sub %d ctl %d", cmd, h[0], h[1], (cmd == 3 || cmd == 12) ? *(uint8_t *)PTR((((*(uint32_t *)(h + 14)) >> 16) << 4) + ((*(uint32_t *)(h + 14)) & 0xFFFF)) : -1);
    if (cmd == 3 || cmd == 12) {              /* IOCTL input / output */
        uint32_t fp = *(uint32_t *)(h + 14);
        uint8_t *cb = PTR(((fp >> 16) << 4) + (fp & 0xFFFF));
        switch (cb[0]) {
        case 0: *(uint32_t *)(cb + 1) = ((CDDEV_ADDR >> 4) << 16); break;   /* device header address */
        case 6: *(uint32_t *)(cb + 1) = 0x00000202u; break;                 /* door closed, data+audio, no disc change */
        case 7: cb[1] = 0; *(uint16_t *)(cb + 2) = 2048; break;              /* sector size */
        case 8: *(uint32_t *)(cb + 1) = 300000; break;                       /* volume size */
        case 9: cb[1] = 1; break;                                            /* media not changed */
        case 10: cb[1] = 1; cb[2] = 1; *(uint32_t *)(cb + 3) = 0x00203000; break; /* audio disk info: 1 data track */
        case 11: *(uint32_t *)(cb + 2) = 0x00000200; cb[6] = 0x40; break;     /* track info: data track */
        case 12: memset(cb + 1, 0, 10); break;
        case 15: memset(cb + 1, 0, 10); break;                               /* audio status */
        default: dtu_log("[mscdex] ioctl control code %d", cb[0]); break;
        }
    } else if (cmd == 0x84 || cmd == 0x85 || cmd == 0x88 || cmd == 13 || cmd == 14 || cmd == 7) {
        /* play/stop/resume audio, open/close: accepted */
    } else {
        dtu_log("[mscdex] device command %02X", cmd);
        status = 0x8103;                      /* unknown command */
    }
    *(uint16_t *)(h + 3) = status;
}

/* multiplex: MSCDEX, Windows/DPMI checks. Pointers (ES:BX) arrive as linear in EBX. */
void dos_int2f(dtu_regs *r)
{
    mscdex_init();
    dtu_log("[mscdex] int 2Fh AX=%04X BX=%04X CX=%04X", AX, BX, CX);
    switch (AX) {
    case 0x1501: { uint8_t *b = PTR(EBX); b[0] = 0; *(uint32_t *)(b + 1) = (CDDEV_ADDR >> 4) << 16; break; }
    case 0x1510: mscdex_request(EBX); break;
    case 0x1500: BX = dtu_cd_dir[0] ? 1 : 0; CX = (uint16_t)cd_letter; break;
    case 0x150B: AX = (CX == cd_letter && dtu_cd_dir[0]) ? 0x5AD8 : 0; BX = 0xADAD; break;
    case 0x150C: BX = 0x0215; break;
    case 0x150D: *(uint8_t *)PTR(EBX) = (uint8_t)cd_letter; break;
    case 0x1600: AL = 0; break;
    case 0x1680: AL = 0; break;
    case 0x1687: AX = 1; break;
    default:
        dtu_log("[dos] unhandled int 2Fh AX=%04X BX=%04X CX=%04X", AX, BX, CX);
        break;
    }
}
