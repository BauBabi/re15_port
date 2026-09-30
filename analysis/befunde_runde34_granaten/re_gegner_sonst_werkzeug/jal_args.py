#!/usr/bin/env python3
"""jal_args.py <re2:BIN|STAGEn.BIN|PSX.EXE> <von_hex> <n_instr> — listet jeden `jal` mit den zuletzt
gesetzten Konstanten in a0..a3 (lui/ori/addiu zero/addiu reg,reg), inkl. Delay-Slot. Mustersuche,
jede Zeile mit dis gegenpruefen."""
import sys, os, struct, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, '..', '..', '..'))
def modload(name):
    p = os.path.join(root, '.claude', 'skills', 're15-psx-disasm', 'scripts', name)
    spec = importlib.util.spec_from_file_location(name[:-3], p)
    m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m); return m
b = sys.argv[1]; lo = int(sys.argv[2], 16); n = int(sys.argv[3])
if b.startswith('re2:'): m = modload('re2_disasm.py'); b = b[4:]
else: m = modload('re15_disasm.py'); b = None if b == 'PSX.EXE' else b
data, fo, path = m.load(lo, b)
def sx(v): return v - 0x10000 if v & 0x8000 else v
regs = {}
for i in range(n):
    a = lo + 4*i
    try: w = struct.unpack_from('<I', data, fo(a))[0]
    except struct.error: break
    op = w >> 26; rs = (w>>21)&31; rt = (w>>16)&31; imm = w & 0xffff
    def apply(w):
        op = w >> 26; rs = (w>>21)&31; rt = (w>>16)&31; imm = w & 0xffff
        if op == 0x0f: regs[rt] = (imm << 16) & 0xffffffff
        elif op == 0x0d: regs[rt] = ((regs.get(rs, 0) if rs else 0) | imm) & 0xffffffff if (rs == 0 or rs in regs) else None
        elif op == 0x09:
            if rs == 0: regs[rt] = sx(imm) & 0xffffffff
            elif rs in regs and regs[rs] is not None: regs[rt] = (regs[rs] + sx(imm)) & 0xffffffff
            else: regs[rt] = None
        elif op in (0x23,0x21,0x25,0x20,0x24): regs[rt] = None
        elif op == 0 and (w & 63) in (0x21, 0x25) and ((w>>11)&31):
            rd = (w>>11)&31; a1_ = regs.get(rs) if rs else 0; b1_ = regs.get(rt) if rt else 0
            regs[rd] = (a1_ + b1_) & 0xffffffff if (a1_ is not None and b1_ is not None and (w&63)==0x21) else None
    if op == 0x03:
        tgt = ((a + 4) & 0xf0000000) | ((w & 0x3ffffff) << 2)
        # Delay-Slot anwenden
        w2 = struct.unpack_from('<I', data, fo(a + 4))[0]; apply(w2)
        args = ' '.join(f'a{k}={("0x%08x" % regs[4+k]) if regs.get(4+k) is not None else "?"}' for k in range(4))
        print(f'  {a:08x}: jal 0x{tgt:08x}   {args}')
        regs = {k: v for k, v in regs.items() if k >= 16}
        continue
    apply(w)
