"""Minimal LE (DOS4GW) loader: extract objects and apply internal fixups."""
import struct, sys, json
class LE:
    def __init__(s, path):
        d = s.d = open(path,'rb').read()
        s.le = le = struct.unpack_from('<I', d, 0x3c)[0]
        g = lambda o, f='<I': struct.unpack_from(f, d, le+o)[0]
        s.npages=g(0x14); s.eip_obj=g(0x18); s.eip=g(0x1c); s.esp_obj=g(0x20); s.esp=g(0x24)
        s.pgsz=g(0x28); s.lastpg=g(0x2c)
        objtab=g(0x40); nobj=g(0x44); s.optab=le+g(0x48)
        s.fixpg=le+g(0x68); s.fixrec=le+g(0x6c); s.datapages=g(0x80)
        s.objs=[]
        for i in range(nobj):
            vs,base,fl,pti,npe,_=struct.unpack_from('<6I', d, le+objtab+i*24)
            s.objs.append(dict(vsize=vs,base=base,flags=fl,pti=pti,npe=npe))
        s.load()
    def page_file_off(s, p):  # p: 1-based page index
        b = s.d[s.optab+(p-1)*4: s.optab+(p-1)*4+4]
        num = (b[0]<<16)|(b[1]<<8)|b[2]
        return s.datapages+(num-1)*s.pgsz
    def load(s):
        s.mem=[]
        for o in s.objs:
            buf=bytearray(o['vsize'] + 0x10)
            for k in range(o['npe']):
                p=o['pti']+k
                off=s.page_file_off(p)
                n = s.lastpg if p==s.npages else s.pgsz
                buf[k*s.pgsz:k*s.pgsz+n]=s.d[off:off+n]
            s.mem.append(buf)
        s.fixups=[]  # (obj, offset_in_obj, type, target_obj, target_off)
        d=s.d
        for oi,o in enumerate(s.objs):
            for k in range(o['npe']):
                p=o['pti']+k
                a,b=struct.unpack_from('<II', d, s.fixpg+(p-1)*4)
                i=s.fixrec+a; end=s.fixrec+b
                while i<end:
                    src=d[i]; flg=d[i+1]; i+=2
                    if src & 0x20:
                        cnt=d[i]; i+=1; offs=None
                    else:
                        so=struct.unpack_from('<h',d,i)[0]; i+=2; cnt=None
                    if flg&3!=0: raise Exception('non-internal fixup %x'%flg)
                    if flg&0x40: tobj=struct.unpack_from('<H',d,i)[0]; i+=2
                    else: tobj=d[i]; i+=1
                    st=src&0xf
                    toff=0
                    if st!=2:
                        if flg&0x10: toff=struct.unpack_from('<I',d,i)[0]; i+=4
                        else: toff=struct.unpack_from('<H',d,i)[0]; i+=2
                    if cnt is not None:
                        lst=[struct.unpack_from('<h',d,i+2*j)[0] for j in range(cnt)]; i+=2*cnt
                    else: lst=[so]
                    for so in lst:
                        s.fixups.append((oi, k*s.pgsz+so, st, tobj-1, toff))
    def apply(s, bases):
        mem=[bytearray(m) for m in s.mem]
        for oi,off,st,to,toff in s.fixups:
            if off<0: continue
            v=bases[to]+toff
            if st==7: struct.pack_into('<I',mem[oi],off,v&0xffffffff)
            elif st==8:  # 32-bit self-relative
                struct.pack_into('<I',mem[oi],off,(v-(bases[oi]+off+4))&0xffffffff)
            elif st==2: pass  # selector
            elif st==5: struct.pack_into('<H',mem[oi],off,v&0xffff)
            else: pass
        return mem
if __name__=='__main__':
    le=LE(sys.argv[1])
    from collections import Counter
    print('objects', [(hex(o['base']),hex(o['vsize'])) for o in le.objs])
    print('entry', le.eip_obj, hex(le.eip), 'stack', le.esp_obj, hex(le.esp))
    print('fixups', len(le.fixups), Counter((f[2],f[0],f[3]) for f in le.fixups))
    bases=[o['base'] for o in le.objs]
    mem=le.apply(bases)
    for i,m in enumerate(mem):
        open(sys.argv[2]+'/obj%d.bin'%(i+1),'wb').write(m[:le.objs[i]['vsize']])
