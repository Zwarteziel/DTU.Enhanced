#!/usr/bin/env python3
"""Map host addresses (from a crash log) to the nearest preceding label of the recompiled code."""
import sys, bisect, subprocess
binp = sys.argv[1]
syms = []
for l in subprocess.run(['nm', '-n', binp], capture_output=True, text=True).stdout.splitlines():
    p = l.split()
    if len(p) == 3 and p[1] in 'tTdDbB' and not p[2].startswith('..@'):
        syms.append((int(p[0], 16), p[2]))
addrs = [s[0] for s in syms]
for a in sys.argv[2:]:
    v = int(a, 16); i = bisect.bisect_right(addrs, v) - 1
    print('%s  %s+%#x' % (a, syms[i][1], v - syms[i][0]) if i >= 0 else a)
