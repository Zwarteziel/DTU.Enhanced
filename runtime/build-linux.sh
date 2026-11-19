#!/bin/sh
# Build the headless Linux test binary (dtu-headless) from the recompiled object.
#   RECOMP = directory with dtu-main.o (elf64) and the x64/*.inc includes
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
RECOMP=${RECOMP:-$HERE/../recomp}
OUT=${OUT:-$HERE/build}
mkdir -p "$OUT"
for a in dtu-asm asm_int asm_inout; do
  nasm -felf64 -Ox -w+orphan-labels -I"$RECOMP/x64/" "$HERE/x64/$a.asm" -o "$OUT/$a.o"
done
for c in core headless fault lowmem platform dos dpmi video io mve sos render menu; do
  gcc -O2 -g -Wall -Wno-unused-function -fno-pie -c "$HERE/$c.c" -o "$OUT/$c.o"
done
gcc -no-pie -Wl,-Ttext-segment,0x10000000 -o "$OUT/dtu-headless" \
  "$OUT"/core.o "$OUT"/headless.o "$OUT"/fault.o "$OUT"/lowmem.o "$OUT"/platform.o \
  "$OUT"/dos.o "$OUT"/dpmi.o "$OUT"/video.o "$OUT"/io.o "$OUT"/mve.o "$OUT"/sos.o "$OUT"/render.o "$OUT"/menu.o \
  "$OUT"/dtu-asm.o "$OUT"/asm_int.o "$OUT"/asm_inout.o "$RECOMP/dtu-main.o" -lpthread
nm -n "$OUT/dtu-headless" | grep -E " [tT] (loc_|dtu_|SR_|x86_)" > "$OUT/dtu-headless.map"
echo "built $OUT/dtu-headless"
