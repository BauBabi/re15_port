#!/usr/bin/env python3
"""Gegenpruefung re_saeure_brand §1.2: BREITER Zensus der Leser der ausgeruesteten Waffe 0x800ACA5D.

Der Ermittler-Zensus (aca5d_zensus.py) fand nur `lui rY,0x800b` + `lb/lbu -13731(rY)`.
Hier zusaetzlich (einfache Vorwaerts-Konstantenpropagation je Register, geloescht bei jr/j/Branch-Ziel-
unabhaengig nach 96 Instruktionen und bei jedem anderen Schreiben des Registers):
  * Byte-Lesen mit effektiver Adresse 0x800ACA5D ueber JEDE Basis (z.B. Spielerbasis 0x800ACA54 + 9),
  * Halbwort-Lesen 0x800ACA5C (aca5c|aca5d<<8) und Wort-Lesen 0x800ACA5C,
  * unaligned lwl/lwr ueber 0x800ACA5C..5F.
Je Fundstelle: Konstanten der naechsten 40 Instruktionen; Markierung, wenn 10/11/12 oder 16/17 vorkommen.
Abbildung wie jal_xref.images() (PSX.EXE via Header, DEBUG.BIN @0x800C0000, *.BIN roh @0x80100000).
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde34_granaten", "re_saeure_brand_werkzeug"))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R
from jal_xref import images

LOADS = {0x20: ("lb", 1), 0x24: ("lbu", 1), 0x21: ("lh", 2), 0x25: ("lhu", 2), 0x23: ("lw", 4),
         0x22: ("lwl", 4), 0x26: ("lwr", 4)}

def s16(x): return x - 0x10000 if x & 0x8000 else x

def main():
    re2 = "--re2" in sys.argv
    target = 0x800ACA5D
    tot = 0; flagged = []
    for name, base, img in images(re2):
        n = len(img) // 4
        ws = struct.unpack_from("<%dI" % n, img, 0)
        reg = {}
        for i, w in enumerate(ws):
            op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; rd = (w >> 11) & 31
            fn = w & 63; imm = w & 0xFFFF; si = s16(imm)
            # expire
            for r in list(reg):
                if i - reg[r][1] > 96: del reg[r]
            hit = None
            if op in LOADS and rs in reg:
                ea = (reg[rs][0] + si) & 0xFFFFFFFF
                nm, sz = LOADS[op]
                if ea <= target < ea + sz:
                    hit = (nm, ea)
                elif nm in ("lwl",) and ea - 3 <= target <= ea:
                    hit = (nm, ea)
            # reg update
            if op == 0xF:
                reg[rt] = ((imm << 16) & 0xFFFFFFFF, i)
            elif op == 9 and rs in reg:
                reg[rt] = ((reg[rs][0] + si) & 0xFFFFFFFF, i)
            elif op == 9 and rs == 0:
                reg[rt] = (si & 0xFFFFFFFF, i)
            elif op == 0xD and rs in reg:
                reg[rt] = (reg[rs][0] | imm, i)
            elif op == 0 and fn == 0x21 and (rs == 0 or rt == 0) and ((rt if rs == 0 else rs) in reg):
                src = rt if rs == 0 else rs
                reg[rd] = (reg[src][0], i)
            else:
                # invalidate written register
                if op == 0 and fn not in (8, 0x18, 0x19, 0x1a, 0x1b, 0x11, 0x13, 0xc, 0xd):
                    reg.pop(rd, None)
                elif op == 3 or (op == 0 and fn == 9):
                    # call clobbers caller-saved
                    for r in (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,24,25,31):
                        reg.pop(r, None)
                elif op in (0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf) or op in LOADS or op in (0x32,):
                    reg.pop(rt, None)
            if op == 0 and fn == 8:  # jr -> function end-ish (delay slot still ok)
                pass
            if hit:
                tot += 1
                a = base + 4 * i
                consts = []
                for j in range(i + 1, min(n, i + 41)):
                    w2 = ws[j]; op2 = w2 >> 26; rs2 = (w2 >> 21) & 31
                    im2 = w2 & 0xFFFF; si2 = s16(im2)
                    if op2 == 0xD and rs2 == 0: consts.append(im2)
                    elif op2 == 9 and rs2 == 0: consts.append(si2)
                    elif op2 in (0xA, 0xB): consts.append(("<", si2))
                    elif op2 == 0xE: consts.append(("^", im2))
                    elif op2 == 9 and rs2 != 0 and rs2 != 29 and -32 < si2 < 0: consts.append(("+", si2))
                flat = [c if isinstance(c, int) else c[1] for c in consts]
                mark = any(v in (10, 11, 12, 16, 17, -10, -11) for v in flat)
                line = "%-10s @0x%08x %-4s EA=0x%08x  consts=%s%s" % (name, a, hit[0], hit[1], consts, "   <== 10/11/12/16/17" if mark else "")
                print(line)
                if mark: flagged.append(line)
    print("; %d Leser (breit) von 0x800ACA5D" % tot)
    print("; markiert (Konstante 10/11/12/16/17 in 40 Folge-Instruktionen):")
    for f in flagged: print("   " + f)

if __name__ == "__main__":
    main()
