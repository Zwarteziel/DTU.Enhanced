import re, subprocess, sys
def entries(): return [l.strip() for l in open('fixup_interpret_as_code.sci') if l.strip()]
for it in range(60):
    r=subprocess.run(['./SR.exe','DTUGAME.EXE','x.asm'],capture_output=True,text=True)
    log=r.stdout+r.stderr
    if r.returncode==0: print('SR ok after',it,'fixes'); sys.exit(0)
    m=re.search(r'decoded length: \d+ \(0 - 0x([0-9a-f]+)\)',log) or re.search(r'fixup mismatch - 0 - (\d+)',log) or re.search(r'output not found - 0 - 0x([0-9a-f]+)',log)
    if not m: print(log[-500:]); sys.exit(1)
    s=m.group(1); off=int(s,16) if ('0x' in m.group(0) or 'length' in m.group(0)) else int(s)
    a=off+0x10000
    L=entries(); cand=[x for x in L if a-0x800<=int(x[4:],16)<=a]
    if not cand: print('no candidate for %X'%a, log[-300:]); sys.exit(1)
    bad=max(cand,key=lambda x:int(x[4:],16))
    print('error at %X -> drop %s'%(a,bad))
    open('fixup_interpret_as_code.sci','w').write('\n'.join(x for x in L if x!=bad)+'\n')
    open('data_in_code.txt','a').write(bad+'\n')
print('gave up'); sys.exit(1)
