import sys, capstone
code=open('obj1.bin','rb').read(); BASE=0x10000
md=capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a=int(sys.argv[1],16); n=int(sys.argv[2]) if len(sys.argv)>2 else 60
for i in md.disasm(code[a-BASE:a-BASE+n*8], a):
    print('%06X: %-24s %s %s'%(i.address, i.bytes.hex(), i.mnemonic, i.op_str)); n-=1
    if n==0: break
