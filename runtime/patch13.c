/* Undermountain Enhanced - install the official 1.3 patch (DTU1-3.EXE).

   DTU1-3.EXE is a self-extracting ZIP archive for DOS (an MZ stub followed by
   the archive), which no longer runs on 64-bit Windows. Here its 44 files are
   unpacked into the game folder; the files they replace are moved to a
   backup folder first, so nothing is lost. Only stored and deflated entries
   (all that PKZIP 2 writes) are supported; every file's CRC is checked
   before anything in the game folder changes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif
#include <sys/stat.h>
#include <zlib.h>
#include "platform.h"
#include "patch13.h"
#ifdef _WIN32
#include <io.h>
#define chmod_rw(p) _chmod((p), _S_IREAD | _S_IWRITE)
#define SEP "\\"
#else
#define chmod_rw(p) chmod((p), 0644)
#define SEP "/"
#endif

#define MAXFILES 256

static unsigned rd16(const unsigned char *p) { return p[0] | p[1] << 8; }
static unsigned long rd32(const unsigned char *p) { return (unsigned long)p[0] | (unsigned long)p[1] << 8 | (unsigned long)p[2] << 16 | (unsigned long)p[3] << 24; }

static unsigned char *read_all(const char *path, long *len)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *b = n > 0 ? malloc((size_t)n) : NULL;
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    *len = n;
    return b;
}

/* an existing file in dir whose name matches name without regard to case
   (Windows does that itself; elsewhere: as given, upper or lower case) */
static int find_existing(const char *dir, const char *name, char *out, size_t outlen)
{
    char v[3][128];
    size_t i;
    for (i = 0; name[i] && i < sizeof v[0] - 1; i++) {
        v[0][i] = name[i];
        v[1][i] = (char)toupper((unsigned char)name[i]);
        v[2][i] = (char)tolower((unsigned char)name[i]);
    }
    v[0][i] = v[1][i] = v[2][i] = 0;
    static const int order[3] = { 1, 0, 2 };        /* upper case first: the game's own files are, and Windows keeps that name */
    for (int k = 0; k < 3; k++) {
        snprintf(out, outlen, "%s" SEP "%s", dir, v[order[k]]);
        FILE *f = fopen(out, "rb");
        if (f) { fclose(f); return 1; }
    }
    snprintf(out, outlen, "%s" SEP "%s", dir, v[1]);     /* new file: upper case, as the rest of the folder */
    return 0;
}

int patch13_find(const char *dir, char *out, size_t outlen)
{
    static const char *names[] = { "DTU1-3.EXE", "DTU1-3.ZIP", "DTU1_3.ZIP", "DTU1_3.EXE", "DTU13.EXE", "DTU13.ZIP" };   /* DTU1_3.ZIP: the download of patches-scrolls.de */
    for (unsigned k = 0; k < sizeof names / sizeof names[0]; k++)
        if (find_existing(dir, names[k], out, outlen)) return 1;
    return 0;
}

int patch13_apply(const char *dir, const char *archive, char *msg, size_t msglen)
{
    long len = 0;
    unsigned char *z = read_all(archive, &len);
    struct { char name[64]; unsigned char *data; unsigned long size; } f[MAXFILES];
    int n = 0, ok = 0, failed = 1;
    if (!z) { snprintf(msg, msglen, "Cannot read %s.", archive); return -1; }
    memset(f, 0, sizeof f);

    /* end of central directory: the last "PK\5\6" within the last 64 KB */
    long e = -1;
    for (long i = len - 22; i >= 0 && i >= len - 22 - 65535; i--)
        if (z[i] == 'P' && z[i + 1] == 'K' && z[i + 2] == 5 && z[i + 3] == 6) { e = i; break; }
    if (e < 0) { snprintf(msg, msglen, "%s is not a ZIP archive.", archive); goto out; }
    unsigned entries = rd16(z + e + 10);
    unsigned long cdsize = rd32(z + e + 12), cdofs = rd32(z + e + 16);
    long base = e - (long)cdsize - (long)cdofs;     /* offsets count from the start of the archive, after the DOS stub */
    if (base < 0 || entries > MAXFILES) { snprintf(msg, msglen, "Unexpected layout of %s.", archive); goto out; }

    long c = base + (long)cdofs;
    for (unsigned k = 0; k < entries; k++) {
        if (c + 46 > len || rd32(z + c) != 0x02014b50) { snprintf(msg, msglen, "Damaged directory in %s.", archive); goto out; }
        unsigned method = rd16(z + c + 10), nlen = rd16(z + c + 28), xlen = rd16(z + c + 30), clen = rd16(z + c + 32);
        unsigned long crc = rd32(z + c + 16), csize = rd32(z + c + 20), usize = rd32(z + c + 24), lofs = rd32(z + c + 42);
        char name[64];
        if (nlen == 0 || nlen >= sizeof name) { snprintf(msg, msglen, "Unexpected file name in %s.", archive); goto out; }
        memcpy(name, z + c + 46, nlen);
        name[nlen] = 0;
        c += 46 + nlen + xlen + clen;
        if (name[nlen - 1] == '/' || name[nlen - 1] == '\\') continue;      /* a folder */
        if (strchr(name, '/') || strchr(name, '\\') || strchr(name, ':') || strstr(name, "..")) {
            snprintf(msg, msglen, "Unexpected file name %s in %s.", name, archive); goto out;
        }
        long l = base + (long)lofs;
        if (l + 30 > len || rd32(z + l) != 0x04034b50) { snprintf(msg, msglen, "Damaged entry %s in %s.", name, archive); goto out; }
        long d = l + 30 + rd16(z + l + 26) + rd16(z + l + 28);
        if (d + (long)csize > len || usize > 64ul << 20) { snprintf(msg, msglen, "Damaged entry %s in %s.", name, archive); goto out; }
        unsigned char *u = malloc(usize ? usize : 1);
        if (!u) { snprintf(msg, msglen, "Out of memory."); goto out; }
        if (method == 0 && csize == usize) memcpy(u, z + d, usize);
        else if (method == 8) {
            z_stream s;
            memset(&s, 0, sizeof s);
            s.next_in = z + d; s.avail_in = (uInt)csize;
            s.next_out = u; s.avail_out = (uInt)usize;
            int r = inflateInit2(&s, -MAX_WBITS);
            if (r == Z_OK) r = inflate(&s, Z_FINISH);
            inflateEnd(&s);
            if (r != Z_STREAM_END || s.total_out != usize) { free(u); snprintf(msg, msglen, "Cannot unpack %s from %s.", name, archive); goto out; }
        } else { free(u); snprintf(msg, msglen, "%s in %s uses an unsupported compression method (%u).", name, archive, method); goto out; }
        if (crc32(0L, u, (uInt)usize) != crc) { free(u); snprintf(msg, msglen, "%s in %s is damaged (CRC).", name, archive); goto out; }
        snprintf(f[n].name, sizeof f[n].name, "%s", name);
        f[n].data = u; f[n].size = usize;
        n++;
    }
    if (!n) { snprintf(msg, msglen, "%s contains no files.", archive); goto out; }
    {   /* the right patch: its DTUGAME.EXE is the one of version 1.3 */
        int found = 0;
        for (int k = 0; k < n; k++)
            if (!strcasecmp(f[k].name, "DTUGAME.EXE") && f[k].size == 1760375ul) found = 1;
        if (!found) { snprintf(msg, msglen, "%s is not the 1.3 patch of Descent to Undermountain (no DTUGAME.EXE of version 1.3 in it).", archive); goto out; }
    }

    /* all files are fine: move the old ones aside and write the new ones */
    char bdir[1100], path[1200], bpath[1200];
    snprintf(bdir, sizeof bdir, "%s" SEP "BEFORE-1.3", dir);
    plat_mkdir(bdir);
    for (int k = 0; k < n; k++) {
        if (find_existing(dir, f[k].name, path, sizeof path)) {
            const char *base_name = strrchr(path, SEP[0]) + 1;
            snprintf(bpath, sizeof bpath, "%s" SEP "%s", bdir, base_name);
            FILE *t = fopen(bpath, "rb");
            if (t) fclose(t);
            else {                                         /* keep the first original, also when this runs twice */
                chmod_rw(path);
                if (rename(path, bpath) != 0) { snprintf(msg, msglen, "Cannot move %s to %s.", path, bdir); goto out; }
            }
            chmod_rw(path);
            remove(path);
        }
        FILE *w = fopen(path, "wb");
        if (!w || fwrite(f[k].data, 1, f[k].size, w) != f[k].size) {
            if (w) fclose(w);
            snprintf(msg, msglen, "Cannot write %s.", path); goto out;
        }
        fclose(w);
        ok++;
    }
    snprintf(msg, msglen, "Installed %d files of the 1.3 patch. The files they replaced are in %s.", ok, bdir);
    failed = 0;
out:
    for (int k = 0; k < n; k++) free(f[k].data);
    free(z);
    return failed ? -1 : ok;
}
