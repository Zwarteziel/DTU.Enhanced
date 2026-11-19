#!/bin/sh
# recompile + assemble (elf64) + link the headless binary
set -e
cd "$(dirname "$0")/recomp"
NASMFMT=elf64 OBJEXT=o sh build-x64.sh > /tmp/rebuild.out 2>&1 || { tail -5 /tmp/rebuild.out; exit 1; }; tail -1 /tmp/rebuild.out
cd ..
sh runtime/build-linux.sh 2>&1 | grep -E " error|built|undefined" || true
