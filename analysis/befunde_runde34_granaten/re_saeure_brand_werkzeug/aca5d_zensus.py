#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: Zensus aller Leser der ausgeruesteten Waffe 0x800ACA5D
(`lbu rX,-13731(rY)` mit rY aus `lui rY,0x800b`) in RE1.5 PSX.EXE + DEBUG.BIN + STAGE*/TITLE.BIN.

Je Fundstelle werden die naechsten 12 Instruktionen auf Vergleichs-Konstanten untersucht
(ori rZ,zero,K / addiu rZ,zero,K / sltiu / slti / xori / beq/bne gegen rZ). Ausgabe: alle
Konstanten je Leser, und am Ende die Liste der Leser, die 10/11 (0x0A/0x0B) oder 16/17
(0x10/0x11) vergleichen. Zweck: belegen, ob irgendein Code Acid/Incendiary (0x0A/0x0B)
gesondert behandelt.

Zusaetzlich: indizierte Tabellen-Zugriffe (sll + addiu at,at,LO + addu at,at,rX) nach dem
Leser -> Tabellenbasis ausgeben (dann liest der Code eine per-Waffe-Tabelle, dort koennten
0x0A/0x0B eigene Eintraege haben).
"""
import os, sys, struct, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R
from jal_xref import images

TARGET = 0x800ACA5D
LO = TARGET & 0xFFFF
LO_S = LO - 0x10000 if LO & 0x8000 else LO
HI = ((TARGET - LO_S) >> 16) & 0xFFFF


def main():
    re2 = "--re2" in sys.argv
    tot = 0
    special = []
    for name, base, img in images(re2):
        n = len(img) // 4
        ws = struct.unpack_from("<%dI" % n, img, 0)
        hi = {}
        for i, w in enumerate(ws):
            op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31
            imm = w & 0xFFFF; simm = imm - 0x10000 if imm & 0x8000 else imm
            if op == 0xF:
                hi[rt] = (imm, i)
                continue
            if op in (0x24, 0x20) and rs in hi and hi[rs][0] == HI and simm == LO_S and i - hi[rs][1] < 12:
                tot += 1
                addr = base + 4 * i
                consts = []
                tabs = []
                for j in range(i + 1, min(n, i + 14)):
                    w2 = ws[j]
                    op2 = w2 >> 26; rs2 = (w2 >> 21) & 31; rt2 = (w2 >> 16) & 31
                    imm2 = w2 & 0xFFFF; simm2 = imm2 - 0x10000 if imm2 & 0x8000 else imm2
                    if op2 == 0xD and rs2 == 0:
                        consts.append(("ori", imm2))
                    elif op2 == 9 and rs2 == 0:
                        consts.append(("li", simm2))
                    elif op2 == 0xB:
                        consts.append(("sltiu", imm2))
                    elif op2 == 0xA:
                        consts.append(("slti", simm2))
                    elif op2 == 0xE:
                        consts.append(("xori", imm2))
                    elif op2 == 9 and rs2 == 1 and rt2 == 1 and j > 0 and (ws[j - 1] >> 26) == 0xF:
                        tabs.append(((((ws[j - 1] & 0xFFFF) << 16) + simm2) & 0xFFFFFFFF))
                vals = [c[1] for c in consts]
                line = "%-10s @0x%08x  const=%s  tab=%s" % (name, addr,
                        ",".join("%s:%d" % c for c in consts),
                        ",".join("0x%08x" % t for t in tabs))
                print(line)
                if any(v in (10, 11, 16, 17) for v in vals):
                    special.append(line)
    print("; %d Leser von 0x800ACA5D" % tot)
    print("; Leser mit Vergleichskonstante 10/11/16/17 in den naechsten 13 Instruktionen:")
    for s in special:
        print("   " + s)


if __name__ == "__main__":
    main()
