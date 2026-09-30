#!/usr/bin/env python3
"""Gegenpruefung: alle lui/addiu|ori-Paare, die eine Basis im Bereich [lo, hi] bilden (RE1.5 alle Binaerdateien,
--re2 fuer RE2), plus die naechste Lade-/Speicher-Instruktion ueber das Basisregister (auch nach addu-Index).
Aufruf: basis_xref.py 0x80074da8 0x80074db4 [--re2]"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde34_granaten", "re_saeure_brand_werkzeug"))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R
from jal_xref import images
def s16(x): return x - 0x10000 if x & 0x8000 else x
lo = int(sys.argv[1], 0); hi = int(sys.argv[2], 0); re2 = "--re2" in sys.argv
tot = 0
for name, base, img in images(re2):
    n = len(img) // 4
    ws = struct.unpack_from("<%dI" % n, img, 0)
    lui = {}
    for i, w in enumerate(ws):
        op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xFFFF
        if op == 0xF:
            lui[rt] = (imm << 16, i); continue
        if op in (9, 0xD) and rs in lui and i - lui[rs][1] < 16:
            v = (lui[rs][0] + (s16(imm) if op == 9 else 0) + (imm if op == 0xD else 0)) & 0xFFFFFFFF
            if lo <= v <= hi:
                tot += 1
                a = base + 4 * i
                print("== %s @0x%08x  Basis 0x%08x (Reg %s)" % (name, a, v, R.REGS[rt]))
                for j in range(i - 2, min(n, i + 6)):
                    s, _ = R.dis_one(ws[j], base + 4 * j)
                    print("    %08x: %s" % (base + 4 * j, s))
        # memory ops using a lui-reg directly with offset
        if op in (0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2b) and rs in lui and i - lui[rs][1] < 16:
            v = (lui[rs][0] + s16(imm)) & 0xFFFFFFFF
            if lo <= v <= hi:
                tot += 1
                s, _ = R.dis_one(w, base + 4 * i)
                print("== %s @0x%08x  direkt 0x%08x: %s" % (name, base + 4 * i, v, s))
print("; %d Fundstellen fuer Basis in [0x%08x,0x%08x]" % (tot, lo, hi))
