#!/bin/sh
# Undermountain Enhanced - build DTU-Enhanced.exe (Windows x64, SDL3) on Linux.
# No network needed: everything comes from the archives in $TOOLS.
#
# Needs: clang, lld, nasm, python3, gcc/g++ (for SR) and in $TOOLS:
#   mingw64.zip                      MSYS2 mingw64 tree (headers, CRT, libgcc)
#   SDL3-devel-3.4.20-mingw.tar.gz   SDL3 for mingw (import library + SDL3.dll)
#   SR-master.zip                    M-HT/SR static recompiler
# Usage: TOOLS=/path/to/_tools GAME=/path/to/DtU sh build-windows.sh
# Result: $PREFIX/win/DTU-Enhanced.exe and SDL3.dll (copy both into the DtU folder)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TOOLS=${TOOLS:?set TOOLS to the folder with the toolchain archives (see README.md)}
GAME=${GAME:?set GAME to your Descent to Undermountain 1.3 folder (with DTUGAME.EXE)}
PREFIX=${PREFIX:-$HOME/dtu-build}
mkdir -p "$PREFIX" && cd "$PREFIX"

[ -d mingw64 ] || { mkdir mingw64 && unzip -q "$TOOLS/mingw64.zip" -d mingw64; }
[ -d SDL3-3.4.20 ] || tar xzf "$TOOLS/SDL3-devel-3.4.20-mingw.tar.gz"

if [ ! -x SR/SR.exe ]; then
  rm -rf SR-master SR && unzip -q "$TOOLS/SR-master.zip"
  mv SR-master/SR SR && rm -rf SR-master
  (cd SR && patch -p0 < "$HERE/third_party/SR/sr-dtu.patch")
  (cd SR && mkdir -p obj && for f in *.c udis86-1.6/libudis86/*.c; do
     gcc -O2 -w -Iudis86-1.6 -c "$f" -o obj/$(basename "$f" .c).o; done &&
   g++ -O2 -std=c++20 -w -Iudis86-1.6 -c container_helpers.cpp -o obj/container_helpers.o &&
   g++ obj/*.o -o SR.exe)
fi

# recompile DTUGAME.EXE (v1.3) to win64 COFF
rm -rf recomp-win && mkdir recomp-win && cp -r "$HERE"/recomp/* recomp-win/
cp SR/SR.exe "$GAME/DTUGAME.EXE" recomp-win/
(cd recomp-win && sh build-x64.sh)

RECOMP="$PREFIX/recomp-win" OUT="$PREFIX/win" MW="$PREFIX/mingw64" SDL3="$PREFIX/SDL3-3.4.20/x86_64-w64-mingw32" \
  sh "$HERE/runtime/build-windows.sh"
