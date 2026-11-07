# Aufrufe mit konstantem Zielzeiger a0 unterhalb/auf 0x800cfb74 (Block-Kopie/Loeschung, die das Wort ueberdecken koennte)
import struct, os, glob
REPO = r'C:\workspace\git\reAi_v2'
def bins():
    exe = open(os.path.join(REPO,'info','re2leon','PSX.EXE'),'rb').read()
    taddr, tsize = struct.unpack_from('<II', exe, 0x18)
    yield 'PSX.EXE', exe[0x800:0x800+tsize], taddr
    for p in sorted(glob.glob(os.path.join(REPO,'info','re2leon','COMMON','BIN','*.BIN'))):
        yield os.path.basename(p), open(p,'rb').read(), 0x80100000
def s16(x): return x-0x10000 if x & 0x8000 else x
from collections import Counter
res = []
for name, d, base in bins():
    reg = {}
    n = len(d)//4
    pending = None
    for i in range(n):
        a = base + 4*i
        w = struct.unpack_from('<I', d, 4*i)[0]
        op = w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; imm=w&0xffff
        if pending is not None:
            # delay slot schon ausgefuehrt -> jetzt Werte festhalten (nach Ausfuehrung unten)
            pass
        # execute
        if op == 0x0f: reg[rt] = (imm<<16)&0xffffffff
        elif op == 0x09:
            if rs in reg: reg[rt] = (reg[rs]+s16(imm))&0xffffffff
            else: reg.pop(rt,None)
        elif op == 0x0d:
            if rs in reg: reg[rt] = reg[rs]|imm
            else: reg.pop(rt,None)
        elif op == 0 and (w&63) == 0x21:  # addu rd,rs,rt
            if rs in reg and rt == 0: reg[rd] = reg[rs]
            elif rt in reg and rs == 0: reg[rd] = reg[rt]
            else: reg.pop(rd,None)
        elif op == 0:
            fn = w&63
            if fn not in (0x08,0x0c,0x0d,0x18,0x19,0x1a,0x1b,0x11,0x13): reg.pop(rd,None)
        elif op in (0x28,0x29,0x2b,0x2a,0x2e, 0x04,0x05,0x06,0x07,0x01,0x02,0x03,0x12): pass
        else: reg.pop(rt,None)
        if pending is not None:
            tgt, ja = pending
            a0 = reg.get(4); a1 = reg.get(5); a2 = reg.get(6)
            if a0 is not None and 0x800c0000 <= a0 <= 0x800cfb77:
                res.append((name, ja, tgt, a0, a1, a2))
            for r in (2,3,4,5,6,7,8,9,10,11,12,13,14,15,24,25,31): reg.pop(r,None)
            pending = None
        if op == 0x03:
            tgt = ((a+4)&0xf0000000)|((w&0x3ffffff)<<2)
            pending = (tgt, a)
for r in res:
    name, ja, tgt, a0, a1, a2 = r
    f = lambda x: 'None' if x is None else hex(x)
    print(f'{name:18s} jal@{ja:#010x} -> {tgt:#010x}  a0={a0:#010x} a1={f(a1)} a2={f(a2)}')
print(len(res))
