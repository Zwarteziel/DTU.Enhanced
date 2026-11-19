import re, capstone, sys
code=open('../an/obj1.bin','rb').read(); B=0x10000
md=capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
targets=sorted(set(l.strip() for l in open('invalid.txt') if l.strip()))
src=open('seg01.inc').read().split('\n')
# label -> following line kind
labidx={}
for i,l in enumerate(src):
    m=re.match(r'^(loc_[0-9A-F]+):$',l)
    if m: labidx[m.group(1)]=i
uses={}
pat=re.compile(r'\bloc_[0-9A-F]+\b')
for i,l in enumerate(src):
    if l.startswith(';') or l.endswith(':'): continue
    for m in pat.finditer(l):
        t=m.group(0)
        inbr = '[' in l[:m.start()] and ']' in l[m.end():] and not l.strip().startswith('lea ')
        uses.setdefault(t,[]).append(('mem' if inbr else 'imm', l.strip()))
def plausible(a):
    n=0; bad=0
    for ins in md.disasm(code[a-B:a-B+64],a):
        n+=1
        if ins.mnemonic in ('in','out','insb','insd','outsb','outsd','arpl','hlt','into','aaa','das','daa','aas','bound','les','lds','salc','fwait') : bad+=1
        if n>=8: break
    return n>=6 and bad==0
res=[]
for t in targets:
    a=int(t[4:],16)
    i=labidx.get(t)
    kind='nolabel'
    if i is not None:
        nxt=src[i+1].strip()
        kind='db' if nxt.startswith('db') or nxt.startswith('times') or nxt.startswith('dd') or nxt.startswith('dw') else 'code'
    u=uses.get(t,[])
    imm=sum(1 for k,_ in u if k=='imm'); mem=len(u)-imm
    res.append((t,kind,imm,mem,plausible(a), u[0][1] if u else ''))
for r in res:
    print('%-11s %-7s imm=%-3d mem=%-3d plaus=%d  %s'%r)
