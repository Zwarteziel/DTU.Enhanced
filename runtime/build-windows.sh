#!/bin/sh
# Build DTU-Enhanced.exe (Windows x64, SDL3) from the recompiled object.
#   RECOMP = directory with dtu-main.obj (win64) and the x64/*.inc includes
#   MW     = MSYS2 mingw64 tree (headers, CRT, libgcc)
#   SDL3   = SDL3-3.4.x/x86_64-w64-mingw32 from SDL3-devel-*-mingw.tar.gz
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
RECOMP=${RECOMP:-$HERE/../recomp}
OUT=${OUT:-$HERE/build-win}
MW=${MW:-$HOME/dtu-build/mingw64}
SDL3=${SDL3:-$HOME/dtu-build/SDL3-3.4.20/x86_64-w64-mingw32}
GCCLIB=$(ls -d "$MW"/lib/gcc/x86_64-w64-mingw32/* | head -1)
WCC="clang -Wno-unused-command-line-argument --target=x86_64-w64-mingw32 -fuse-ld=lld -isystem $MW/include -L$MW/lib -L$GCCLIB -B$MW/lib --sysroot=$MW"
mkdir -p "$OUT"
BUILD=${BUILD:-$(date -u "+%Y-%m-%d %H:%M UTC")}
llvm-rc -I "$HERE" -D "DTU_BUILD=\"$BUILD\"" -fo "$OUT/dtu.res" "$HERE/sdl/dtu.rc"
for a in dosmem dtu-asm asm_int asm_inout; do
  nasm -fwin64 -Ox -w+orphan-labels -I"$RECOMP/x64/" "$HERE/x64/$a.asm" -o "$OUT/$a.obj"
done
# SELFBUILD=1: the self-building version - no game code in the exe; it
# recompiles the player's DTUGAME.EXE at first start and loads the result
# (selfbuild.c, coffload.c). Otherwise the recompiled object is linked in.
if [ -n "$SELFBUILD" ]; then
  python3 "$HERE/gen_symtab.py" "$RECOMP/x64" "$OUT/symtab.c" >/dev/null
  GAMESRC="coffload selfbuild"; GAMEOBJ="$OUT/coffload.o $OUT/selfbuild.o $OUT/symtab.o"; DEFS="-DDTU_SELFBUILD"
  $WCC -O2 -c "$OUT/symtab.c" -o "$OUT/symtab.o"
else
  GAMESRC="link_static"; GAMEOBJ="$OUT/link_static.o $RECOMP/dtu-main.obj"; DEFS=""
fi
for c in core fault lowmem platform dos dpmi video io mve sos render menu patch13 sdl/crt_gl sdl/pad sdl/sdl_main $GAMESRC; do
  $WCC -O2 -g -Wall -Wno-unused-function $DEFS -I"$SDL3/include" -DDTU_BUILD="\"$BUILD\"" -c "$HERE/$c.c" -o "$OUT/$(basename $c).o"
done
$WCC -o "$OUT/DTU-Enhanced.exe" \
  "$OUT"/dosmem.obj "$OUT"/core.o "$OUT"/fault.o "$OUT"/lowmem.o "$OUT"/platform.o "$OUT"/dos.o "$OUT"/dpmi.o \
  "$OUT"/video.o "$OUT"/io.o "$OUT"/mve.o "$OUT"/sos.o "$OUT"/render.o "$OUT"/menu.o "$OUT"/patch13.o "$OUT"/crt_gl.o "$OUT"/pad.o "$OUT"/sdl_main.o \
  "$OUT"/dtu-asm.obj "$OUT"/asm_int.obj "$OUT"/asm_inout.obj $GAMEOBJ "$OUT/dtu.res" \
  -L"$SDL3/lib" -lSDL3 "$MW/lib/libz.a" -lwinmm -mwindows \
  -Wl,--image-base,0x20000,--disable-dynamicbase,--disable-reloc-section,--disable-high-entropy-va
# the DOS area must cover A0000-BFFFF (see x64/dosmem.asm)
python3 - "$OUT/DTU-Enhanced.exe" <<'PYEOF'
import struct, sys
d = open(sys.argv[1], 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]
n = struct.unpack_from('<H', d, pe + 6)[0]
opt = struct.unpack_from('<H', d, pe + 20)[0]
base = struct.unpack_from('<Q', d, pe + 24 + 24)[0]
for i in range(n):
    o = pe + 24 + opt + 40 * i
    name = d[o:o + 8].rstrip(b'\0').decode()
    vsize, va = struct.unpack_from('<II', d, o + 8)
    if name == '.dosmem':
        s, e = base + va, base + va + vsize
        print('.dosmem at %x-%x' % (s, e))
        sys.exit(0 if s <= 0xA0000 and e >= 0xC0000 else 'error: .dosmem does not cover A0000-BFFFF')
sys.exit('error: no .dosmem section')
PYEOF
cp "$SDL3/bin/SDL3.dll" "$OUT/"
echo "built $OUT/DTU-Enhanced.exe (version $(sed -n 's/#define DTU_VERSION "\(.*\)"/\1/p' "$HERE/version.h"), build $BUILD)"
