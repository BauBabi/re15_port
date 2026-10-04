#!/usr/bin/env python3
"""ovl_reloc_diff.py - Runde 35 Spur C: relokationsbewusster Wortvergleich zweier Overlay-Regionen.
STAGE*.BIN hat KEINEN Header: file_off = addr - 0x80100000.
Aufruf: python ovl_reloc_diff.py STAGEa.BIN startA STAGEb.BIN startB laenge_bytes
Klassen je Wort: gleich | j/jal (Zieldelta) | imm (gleicher Opcode/Register, nur 16-Bit-Immediate anders
= lui/addiu/lw/sw-Datenrelokation) | ECHT (alles andere)."""
import sys, struct
from collections import Counter
fa, sa, fb, sb, n = sys.argv[1], int(sys.argv[2], 16), sys.argv[3], int(sys.argv[4], 16), int(sys.argv[5], 16)
A = open(fa, "rb").read(); B = open(fb, "rb").read()
oa, ob = sa - 0x80100000, sb - 0x80100000
cls = Counter(); jd = Counter(); echt = []
for i in range(0, n, 4):
    wa = struct.unpack_from("<I", A, oa + i)[0]; wb = struct.unpack_from("<I", B, ob + i)[0]
    if wa == wb: cls["gleich"] += 1; continue
    opa, opb = wa >> 26, wb >> 26
    if opa == opb and opa in (2, 3):
        ta = ((wa & 0x3ffffff) << 2) | 0x80000000; tb = ((wb & 0x3ffffff) << 2) | 0x80000000
        cls["j/jal"] += 1; jd[tb - ta] += 1; continue
    if opa == opb and (wa >> 16) == (wb >> 16):
        cls["imm"] += 1; continue
    cls["ECHT"] += 1; echt.append((sa + i, wa, sb + i, wb))
print("Woerter:", n // 4, dict(cls))
print("j/jal-Zieldeltas:", {hex(k): v for k, v in jd.items()})
for a, wa, b, wb in echt[:40]:
    print("  ECHT A@0x%08x %08x  B@0x%08x %08x" % (a, wa, b, wb))
