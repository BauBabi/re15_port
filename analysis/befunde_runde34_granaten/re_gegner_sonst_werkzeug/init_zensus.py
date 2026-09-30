#!/usr/bin/env python3
"""init_zensus.py — je registriertem Typ/Stage (reg_scan-Logik): Wurzel -> Zustandstabelle (erstes
`lui at / addiu at` nach `lbu vX,4(vY)` + `jalr` in den ersten 120 Instruktionen) -> INIT = Tabelle[0],
HURT = [2], DEATH = [3]. Im INIT (bis jr ra, max 400 Instr.) werden gesucht:
  sw rX,120(..)  -> Hitbox-Zeiger (Wert von rX ueber `lui rX / lw rX,imm(rX)` = *(Adresse) rekonstruiert)
  sh rX,154(..)  -> HP (Konstante, falls rX per ori/addiu zero gesetzt)
  sb rX,147(..)  -> +0x93 (Konstante bzw. 'zero')
Nur Mustersuche — jede Zeile ist mit `re15_disasm.py dis` gegenzupruefen."""
import os, sys, struct, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, '..', '..', '..'))
spec = importlib.util.spec_from_file_location('d', os.path.join(root, '.claude', 'skills', 're15-psx-disasm', 'scripts', 're15_disasm.py'))
D = importlib.util.module_from_spec(spec); spec.loader.exec_module(D)
sys.path.insert(0, here)
def sx(v): return v - 0x10000 if v & 0x8000 else v
def W(data, fo, a):
    try: return struct.unpack_from('<I', data, fo(a))[0]
    except struct.error: return 0
def regs(stage):
    import reg_scan
REGLIST = []
_rs = os.path.join(root, 'build', 'r34g_gegner_sonst', 'reg_scan.txt')
if not os.path.exists(_rs): _rs = os.path.join(here, 'reg_scan.txt')   # Beleg-Kopie neben dem Werkzeug
for line in open(_rs):
    p = line.split()
    if len(p) < 6 or p[5] == '?': continue
    REGLIST.append((p[0], int(p[3], 16), int(p[5], 16)))
def state_table(data, fo, rootp):
    seen4 = False; hi = None
    for i in range(160):
        a = rootp + i*4; w = W(data, fo, a); op = w >> 26
        rs = (w>>21)&31; rt = (w>>16)&31; imm = w & 0xffff
        if op == 0x24 and sx(imm) == 4: seen4 = True
        if seen4 and op == 0x0f and rt == 1: hi = imm << 16
        if seen4 and hi is not None and op == 0x09 and rt == 1 and rs == 1:
            return (hi + sx(imm)) & 0xffffffff
    return None
def konst(data, fo, a, reg):
    # rueckwaerts: ori reg,zero,imm / addiu reg,zero,imm / lui+lw
    for j in range(1, 12):
        w = W(data, fo, a - 4*j); op = w >> 26; rs=(w>>21)&31; rt=(w>>16)&31; imm=w&0xffff
        if rt == reg and op == 0x0d and rs == 0: return imm
        if rt == reg and op == 0x09 and rs == 0: return sx(imm)
        if rt == reg and op == 0x23:  # lw reg, imm(rs)
            for k in range(1, 6):
                w2 = W(data, fo, a - 4*(j+k))
                if (w2>>26) == 0x0f and ((w2>>16)&31) == rs:
                    addr = (((w2 & 0xffff) << 16) + sx(imm)) & 0xffffffff
                    return ('*', addr)
            return None
        if rt == reg and op in (0x09, 0x0d, 0x0f, 0x23, 0x25, 0x24, 0x21): return None
    return None
out = []
for stage, typ, rootp in REGLIST:
    if typ not in (0x1a,0x22,0x23,0x24,0x26,0x27,0x29,0x2b,0x2d,0x30,0x36,0x40,0x42,0x45,0x47,0x49,0x4b,0x4d):
        continue
    data, fo, path = D.load(rootp, stage)
    st = state_table(data, fo, rootp)
    if st is None:
        print(f'{stage} 0x{typ:02x} root 0x{rootp:08x}: Zustandstabelle nicht gefunden'); continue
    ent = [W(data, fo, st + 4*k) for k in range(4)]
    init = ent[0]
    info = []
    if 0x80100000 <= init < 0x80130000:
        for i in range(400):
            a = init + i*4; w = W(data, fo, a); op = w >> 26
            rt = (w>>16)&31; imm = sx(w & 0xffff)
            if w == 0x03e00008: break
            if op == 0x2b and imm == 120:
                v = konst(data, fo, a, rt)
                if isinstance(v, tuple):
                    ptr = W(data, fo, v[1]) if 0x80100000 <= v[1] < 0x80130000 else None
                    box = None
                    if ptr and 0x80100000 <= ptr < 0x80130000:
                        try: box = [sx(struct.unpack_from('<H', data, fo(ptr) + 2*k)[0]) for k in range(6)]
                        except struct.error: box = None
                    info.append(f'+0x78 @0x{a:08x} = *(0x{v[1]:08x}) = {("0x%08x" % ptr) if ptr else "?"} Box {box}')
                else: info.append(f'+0x78 @0x{a:08x} = {v}')
            if op == 0x29 and imm == 154:
                v = konst(data, fo, a, rt); info.append(f'HP @0x{a:08x} = {v if v is not None else "Tabelle/Reg"}')
            if op == 0x28 and imm == 147:
                v = 0 if rt == 0 else konst(data, fo, a, rt); info.append(f'+0x93 @0x{a:08x} = {v}')
    print(f'{stage} Typ 0x{typ:02x} Wurzel 0x{rootp:08x} Tabelle 0x{st:08x}: INIT 0x{ent[0]:08x} HURT 0x{ent[2]:08x} DEATH 0x{ent[3]:08x}')
    for s in info: print('     ', s)
