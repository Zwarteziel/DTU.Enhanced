#!/bin/sh
# Undermountain Enhanced - headless Linux test build (no network needed).
#
# Builds SR from $TOOLS/SR-master.zip (with the DTU patch), recompiles
# DTUGAME.EXE to ELF64 and links runtime/ into "dtu-headless". That binary runs
# the game without a window, logs every DOS/DPMI/VESA call and writes PNG
# screenshots - used to develop the platform layer before the SDL3 window.
#
# Usage: TOOLS=/path/to/_tools GAME=/path/to/DtU sh build-headless.sh
# Run:   $PREFIX/headless/dtu-headless --game DIR --cd CDDIR --out OUT --seconds 15 -- -notitles -nosound
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TOOLS=${TOOLS:?set TOOLS to the folder with the toolchain archives (see README.md)}
GAME=${GAME:?set GAME to your Descent to Undermountain 1.3 folder (with DTUGAME.EXE)}
PREFIX=${PREFIX:-$HOME/dtu-build}
mkdir -p "$PREFIX" && cd "$PREFIX"

if [ ! -x SR/SR.exe ]; then
  rm -rf SR-master SR && unzip -q "$TOOLS/SR-master.zip"
  mv SR-master/SR SR && rm -rf SR-master
  (cd SR && patch -p0 < "$HERE/third_party/SR/sr-dtu.patch")
  (cd SR && mkdir -p obj && for f in *.c udis86-1.6/libudis86/*.c; do
     gcc -O2 -w -Iudis86-1.6 -c "$f" -o obj/$(basename "$f" .c).o; done &&
   g++ -O2 -std=c++20 -w -Iudis86-1.6 -c container_helpers.cpp -o obj/container_helpers.o &&
   g++ obj/*.o -o SR.exe)
fi

rm -rf recomp-elf && mkdir recomp-elf && cp -r "$HERE"/recomp/* recomp-elf/
cp SR/SR.exe "$GAME/DTUGAME.EXE" recomp-elf/
(cd recomp-elf && NASMFMT=elf64 OBJEXT=o sh build-x64.sh)
RECOMP="$PREFIX/recomp-elf" OUT="$PREFIX/headless" sh "$HERE/runtime/build-linux.sh"
