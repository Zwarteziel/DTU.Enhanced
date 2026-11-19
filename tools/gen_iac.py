import os, capstone
code=open('../an/obj1.bin','rb').read(); B=0x10000
md=capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
out=set(l.strip() for l in open('fixup_interpret_as_code.sci')) if os.path.exists('fixup_interpret_as_code.sci') else set()
out.discard('')
bad=set(l.strip() for l in open('data_in_code.txt')) if os.path.exists('data_in_code.txt') else set()
added=[]
for l in open('../an/classified.txt'):
    p=l.split()
    if p[1]!='db' or 'imm=0' in l or 'mem=0' not in l or 'plaus=1' not in l: continue
    if p[0] in bad or p[0] in out: continue
    a=int(p[0][4:],16); b=code[a-B:a-B+8]
    if b[:4]==b'\0\0\0\0': continue
    if sum(1 for c in b if 65<=c<=90 or 97<=c<=122 or c in b' .\n')>=7: continue
    ins=list(md.disasm(code[a-B:a-B+24],a))[:4]
    if not ins: continue
    m=ins[0].mnemonic; o=ins[0].op_str
    # data tables tend to decode as add/adc/sbb/and with register-indirect operands
    if m in ('add','adc','sbb','and','or') and ('[e' in o.split(',')[0] or o.startswith('al') or o.startswith('eax')): continue
    if any(i.mnemonic=='add' and i.op_str=='byte ptr [eax], al' for i in ins): continue
    out.add(p[0]); added.append(p[0])
open('fixup_interpret_as_code.sci','w').write('\n'.join(sorted(out))+'\n')
print('added',len(added),'total',len(out), ' '.join(added[:12]))
