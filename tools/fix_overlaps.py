#!/usr/bin/env python3
"""Iteratively run SR and auto-generate replacements for Watcom 'skip-bytes' overlapping instructions."""
import subprocess, re, sys, os
os.chdir(os.path.expanduser('~/dtu/sr'))
code = open(os.path.expanduser('~/dtu/work/obj1.bin'),'rb').read()
BASE = 0x10000
def ndis(addr, n=16):
    off = addr-BASE
    open('/tmp/nd.bin','wb').write(code[off:off+n])
    out = subprocess.run(['ndisasm','-b','32','-o',str(addr),'/tmp/nd.bin'],capture_output=True,text=True).stdout
    res=[]
    for line in out.splitlines():
        m=re.match(r'([0-9A-F]{8})\s+([0-9A-F]+)\s+(.*)',line)
        if m: res.append((int(m.group(1),16), len(m.group(2))//2, m.group(3).strip()))
    return res
REP='instruction_replacements.sci'
for it in range(int(sys.argv[1]) if len(sys.argv)>1 else 50):
    r = subprocess.run(['./SR.exe','DTUGAME.EXE','dtu-main.asm'],capture_output=True,text=True)
    u = re.search(r'unknown instruction - 0 - (\d+) - lsl (\w+), (\w+)', r.stderr)
    if u:
        a=int(u.group(1))+BASE; ins=ndis(a,8)[0]
        with open(REP,'a') as f: f.write('loc_%X,%d,mov %s, 0FFFFFFFFh ; lsl: segment limit 4 GB\n'%(a,ins[1],u.group(2)))
        print('lsl at %X'%a); continue
    m = re.search(r'output not found - 0 - 0x([0-9a-f]+)', r.stderr)
    if not m:
        print('SR exit', r.returncode); print(r.stderr[-1500:]); break
    L = int(m.group(1),16)+BASE
    found=None
    for k in (2,1,3,4,5):
        ins = ndis(L-k,16)[0]
        if ins[1] > k:
            inner = ndis(L, 16)
            tot=0; lst=[]
            for a,ln,t in inner:
                lst.append(t); tot+=ln
                if L+tot >= L-k+ins[1]: break
            if L+tot == L-k+ins[1]:
                found=(L-k,k,ins,lst,tot); break
    if not found:
        print('cannot resolve at %X'%L); 
        for k in range(1,8): print(k, ndis(L-k,12)[:3])
        break
    S,k,ins,lst,tot = found
    with open(REP,'a') as f:
        f.write('loc_%X,%d,%s|jmp skp_%X\n'%(S,k,ins[2],S))
        f.write('loc_%X,%d,%s|skp_%X:\n'%(L,tot,'|'.join(lst),S))
    print('fixed %X: %s  / inner %s'%(S,ins[2],lst), flush=True)
