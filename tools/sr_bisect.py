import subprocess, sys
L=[l.strip() for l in open('fixup_interpret_as_code.sci') if l.strip()]
def ok(n):
    open('fixup_interpret_as_code.sci','w').write('\n'.join(L[:n])+'\n')
    r=subprocess.run(['./SR.exe','DTUGAME.EXE','x.asm'],capture_output=True,text=True)
    return r.returncode==0
lo,hi=0,len(L)   # ok(lo) assumed true, ok(hi) false
while hi-lo>1:
    mid=(lo+hi)//2
    if ok(mid): lo=mid
    else: hi=mid
bad=L[hi-1]
print('culprit',bad)
open('fixup_interpret_as_code.sci','w').write('\n'.join(x for x in L if x!=bad)+'\n')
open('data_in_code.txt','a').write(bad+'\n')
