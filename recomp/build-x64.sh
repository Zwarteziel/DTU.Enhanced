#!/bin/sh
# Recompile DTUGAME.EXE (v1.3) to x64 assembly and assemble it (win64 COFF).
set -e
cd "$(dirname "$0")"
./SR.exe DTUGAME.EXE dtu-main.asm >sr.log 2>&1 || { tail -20 sr.log; exit 1; }
sed -i -E 's/^a32 (near )+loop /LOOP_FAR /; s/^LOOP_FAR (near )+/LOOP_FAR /; s/^(near )+REP_OUTSB/REP_OUTSB/' seg01.inc
sed -i 's/^section cseg01 progbits alloc exec nowrite/section cseg01 progbits alloc exec write/' dtu-main.asm
python3 compact_source.py
for i in 1 2 3 4 5 6; do
  if nasm -f${NASMFMT:-win64} -O1 -w+orphan-labels -w-number-overflow -w-prefix-seg -ix64/ dtu-main.asm -o dtu-main.${OBJEXT:-obj} 2>a.a; then echo "assembled (pass $i)"; exit 0; fi
  if ! grep -q "short jump is out of range" a.a; then break; fi
  python3 repair_short_jumps.py
  sed -i -E 's/^a32 (near )+loop /LOOP_FAR /; s/^LOOP_FAR (near )+/LOOP_FAR /; s/^(near )+REP_OUTSB/REP_OUTSB/' seg01.inc
done
grep -v warning a.a | head -60; exit 1
