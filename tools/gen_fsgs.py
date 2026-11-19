#!/usr/bin/env python3
"""Undermountain Enhanced - generate instruction_replacements for FS/GS segment use.

DTU's texture mapper (and some other code) loads its own selectors into FS/GS
(DPMI descriptors whose base points at a texture) and then reads fs:[ecx].
SR drops segment registers, so those reads would go to the flat offset.
Every selector load / store / push / pop and every fs:/gs: memory operand in
translated code is rewritten to use runtime variables:
    dtu_fs_sel, dtu_gs_sel      current selector values
    dtu_fs_base, dtu_gs_base    their base addresses (dtu_sel_base[selector])
    dtu_sel_base[65536]         base of every selector, kept by dpmi.c
Only r9/r10 (scratch registers of the recompiled code) are used and no
instruction that changes the flags is added.

usage: gen_fsgs.py OBJ1.BIN DTUGAME.EXE seg01.inc instruction_replacements.sci
       (rewrites the generated block at the end of the .sci file)
"""
import re, sys, capstone, struct, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE

obj1, exe, seg01, sci = sys.argv[1:5]
code = open(obj1, 'rb').read(); B = 0x10000
le = LE(exe)
bases = [o['base'] for o in le.objs]
fix = {}                                  # linear source address -> target
for oi, off, st, to, toff in le.fixups:
    if oi == 0 and st == 7 and off >= 0: fix[bases[0] + off] = bases[to] + toff

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
src = open(seg01).read()
labels = sorted(int(m.group(1), 16) for m in re.finditer(r'^loc_([0-9A-F]+):$', src, re.M))
# label regions that SR emitted as data are skipped
datalab = set(int(m.group(1), 16) for m in re.finditer(r'^loc_([0-9A-F]+):\n(?:align \d+\n)?(?:db |dd |times )', src, re.M))

STOP = {'ret', 'retf', 'jmp', 'iretd', 'iret', 'ljmp', 'hlt'}

def instrs():
    seen = set()
    for i, a in enumerate(labels):
        if a in datalab or a < B or a >= B + len(code): continue
        end = labels[i + 1] if i + 1 < len(labels) else a + 64
        for ins in md.disasm(code[a - B:min(end, a + 8192) - B], a):
            if ins.address in seen: break
            seen.add(ins.address)
            yield ins
            if ins.mnemonic in STOP: break

R32 = {'esp': 'r11d'}
def reg(r):
    return R32.get(r, r)

def nasm_mem(ins, op, size_kw=True):
    """memory operand -> (expression string, number of registers); fixups become labels"""
    m = op.mem
    parts = []
    nregs = 0
    if m.base:
        parts.append(reg(ins.reg_name(m.base))); nregs += 1
    if m.index:
        s = ins.reg_name(m.index)
        parts.append(reg(s) + ('*%d' % m.scale if m.scale != 1 else '')); nregs += 1
    disp = m.disp & 0xFFFFFFFF
    # a fixup inside the instruction that points at disp -> label
    lab = None
    for a in range(ins.address, ins.address + ins.size - 3):
        if a in fix and fix[a] == disp:
            lab = 'loc_%X' % disp
    if lab: parts.append(lab)
    elif m.disp: parts.append(('0x%x' % m.disp) if m.disp > 0 else ('-0x%x' % -m.disp))
    return ' + '.join(parts) if parts else '0', nregs

SIZE = {1: 'byte', 2: 'word', 4: 'dword', 8: 'qword'}

def seg_of(ins, op):
    if op.type != capstone.x86.X86_OP_MEM: return None
    sg = {capstone.x86.X86_REG_FS: 'fs', capstone.x86.X86_REG_GS: 'gs', capstone.x86.X86_REG_ES: 'es'}.get(op.mem.segment)
    if sg == 'es' and 0x26 not in ins.prefix: return None     # implicit es (string ops) stays flat
    return sg

def load_seg(s, srcexpr):
    """srcexpr: something movzx can read (16-bit reg or word [mem]) -> sets selector and base"""
    return ['movzx r10d, %s' % srcexpr, 'mov [dtu_%s_sel], r10d' % s,
            'mov r10d, [dtu_sel_base + r10*4]', 'mov [dtu_%s_base], r10d' % s]

def opstr(ins, op, memtext=None):
    t = op.type
    if t == capstone.x86.X86_OP_REG: return reg(ins.reg_name(op.reg))
    if t == capstone.x86.X86_OP_IMM:
        v = op.imm & 0xFFFFFFFF
        for a in range(ins.address, ins.address + ins.size - 3):
            if a in fix and fix[a] == v: return 'loc_%X' % v
        return '0x%x' % v
    return '%s [%s]' % (SIZE[op.size], memtext)

out = []; skipped = []
existing = set(int(l.split(',')[0][4:], 16) for l in open(sci) if l.startswith('loc_') and not l.rstrip().endswith('; fsgs'))
for ins in instrs():
    if ins.address in existing: continue
    m = ins.mnemonic; ops = ins.operands
    text = None
    regs_used = [ins.reg_name(r) for r in ins.regs_access()[0] + ins.regs_access()[1]]
    segregs = [o for o in ops if o.type == capstone.x86.X86_OP_REG and ins.reg_name(o.reg) in ('fs', 'gs', 'es')]
    memseg = [o for o in ops if seg_of(ins, o)]
    if m in ('push', 'pop') and segregs:
        s = ins.reg_name(segregs[0].reg)
        if m == 'push': text = ['mov r10d, [dtu_%s_sel]' % s, 'PUSH32 r10d']
        else: text = load_seg(s, 'word [r11d]') + ['lea r11d, [r11d+4]']
    elif m == 'mov' and segregs and ops[0].type == capstone.x86.X86_OP_REG and ins.reg_name(ops[0].reg) in ('fs', 'gs', 'es'):
        s = ins.reg_name(ops[0].reg)
        o = ops[1]
        if o.type == capstone.x86.X86_OP_REG:
            r = ins.reg_name(o.reg)
            r16 = {'eax': 'ax', 'ebx': 'bx', 'ecx': 'cx', 'edx': 'dx', 'esi': 'si', 'edi': 'di', 'ebp': 'bp'}.get(r, r)
            text = load_seg(s, r16)
        else:
            e, n = nasm_mem(ins, o)
            text = load_seg(s, 'word [%s]' % e)
    elif m == 'mov' and segregs:            # mov r/m, fs  (store selector)
        s = ins.reg_name(segregs[0].reg)
        d = ops[0]
        if d.type == capstone.x86.X86_OP_REG:
            text = ['movzx %s, word [dtu_%s_sel]' % (reg(ins.reg_name(d.reg)) if d.size == 4 else 'r10d', s)]
            if d.size == 2: text += ['mov %s, r10w' % ins.reg_name(d.reg)]
        else:
            e, n = nasm_mem(ins, d)
            text = ['mov r10d, [dtu_%s_sel]' % s, 'mov word [%s], r10w' % e]
    elif m in ('lfs', 'lgs', 'les'):
        s = m[1:]
        dst = reg(ins.reg_name(ops[0].reg))
        e, n = nasm_mem(ins, ops[1])
        sel = load_seg(s, 'word [%s + 4]' % e)
        if ins.reg_name(ops[0].reg) in [ins.reg_name(x) for x in (ops[1].mem.base, ops[1].mem.index) if x]:
            text = ['mov r9d, dword [%s]' % e] + sel + ['mov %s, r9d' % dst]
        else:
            text = ['mov %s, dword [%s]' % (dst, e)] + sel
    elif memseg and m in ('call', 'push', 'jmp'):
        skipped.append(ins); continue
    elif memseg and m not in ('cmpsd', 'cmpsb', 'cmpsw', 'movsd', 'movsb', 'movsw', 'lodsb', 'lodsd', 'lodsw', 'scasb', 'stosb', 'arpl'):
        o = memseg[0]; s = seg_of(ins, o)
        e, n = nasm_mem(ins, o)
        names = set(ins.reg_name(r) for r in list(ins.regs_access()[0]) + list(ins.regs_access()[1]))
        if names & {'ah', 'bh', 'ch', 'dh'}:
            # high-byte registers cannot be encoded together with r9/r10:
            # borrow a legacy register that the instruction does not use
            fam = {'eax': {'eax', 'ax', 'al', 'ah'}, 'ebx': {'ebx', 'bx', 'bl', 'bh'}, 'ecx': {'ecx', 'cx', 'cl', 'ch'},
                   'edx': {'edx', 'dx', 'dl', 'dh'}, 'esi': {'esi', 'si'}, 'edi': {'edi', 'di'}}
            t = next(r for r in ('esi', 'edi', 'edx', 'ecx', 'ebx', 'eax') if not (fam[r] & names))
            t64 = 'r' + t[1:]
            args = [opstr(ins, x, t) if x is o else opstr(ins, x) for x in ops]
            out.append('loc_%X,%d,%s ; fsgs' % (ins.address, ins.size, '|'.join(
                ['mov r9, %s' % t64, 'mov %s, [dtu_%s_base]' % (t, s), 'lea %s, [%s + %s]' % (t, t, e),
                 '%s %s' % (m, ', '.join(args)), 'mov %s, r9' % t64])))
            continue
        if n <= 1:
            pre = ['mov r10d, [dtu_%s_base]' % s]; newe = 'r10d + ' + e
        else:
            pre = ['lea r10d, [%s]' % e, 'mov r9d, [dtu_%s_base]' % s, 'lea r10d, [r10d + r9d]']; newe = 'r10d'
        args = [opstr(ins, x, newe if x is o else None) if x.type != capstone.x86.X86_OP_MEM or x is o else None for x in ops]
        if None in args: skipped.append(ins); continue
        mn = m
        text = pre + ['%s %s' % (mn, ', '.join(args))]
    elif memseg or segregs:
        skipped.append(ins); continue
    if text:
        out.append('loc_%X,%d,%s ; fsgs' % (ins.address, ins.size, '|'.join(text)))

lines = [l for l in open(sci) if not l.rstrip().endswith('; fsgs')]
open(sci, 'w').write(''.join(lines).rstrip('\n') + '\n' + '\n'.join(out) + '\n')
print('generated %d replacements, skipped %d:' % (len(out), len(skipped)))
for i in skipped: print('  %X %s %s' % (i.address, i.mnemonic, i.op_str))
