#!/usr/bin/env python3
# Runde 41 Spur H, Nachbesserung 1: wer setzt/loescht die Gate-Bits von FUN_8002aec4 im Entity-Wort +0x0?
# Gates (selbst disassembliert): 8002af04 or/andi 0x2 (an einem), 8002af14 and/andi 0x1000 (an beiden),
# 8002af24 andi 0x4 (am Geschobenen a1). Muster SET: `ori rC,rA,IMM` (rA != zero, IMM & BITS) und innerhalb von
# 4 Instruktionen `sw/sh/sb rC,0|1(rB)`; Muster CLR: `addiu rM,zero,NEG` (~NEG & BITS), `and`, Speicher auf 0|1.
# Ueber PSX.EXE (text) und STAGE1..6.BIN (roh @0x80100000). Aufruf: python nb1_flagscan.py <bits>  (z.B. 0x2 / 0x4 / 0x1000)
import os, struct, sys
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as D
BITS = int(sys.argv[1], 0) if len(sys.argv) > 1 else 0x1006
def regions():
    exe = open(os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
    taddr, tsize = struct.unpack_from("<II", exe, 0x18)
    yield "EXE", exe[0x800:0x800 + tsize], taddr
    for n in range(1, 7):
        yield "STAGE%d" % n, open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "STAGE%d.BIN" % n), "rb").read(), 0x80100000
for name, data, base in regions():
    n = len(data) // 4
    ws = struct.unpack_from("<%dI" % n, data, 0)
    for i in range(n):
        w = ws[i]; op = w >> 26
        if op == 0xd:
            imm = w & 0xffff; rt = (w >> 16) & 31; rs = (w >> 21) & 31
            if rs == 0 or not (imm & BITS) or imm > 0x1fff: continue
            for j in range(i + 1, min(n, i + 5)):
                w2 = ws[j]
                if (w2 >> 26) in (0x2b, 0x29, 0x28) and ((w2 >> 16) & 31) == rt:
                    if (w2 & 0xffff) in (0, 1):
                        a = base + 4 * i
                        print("%s SET  0x%08x: %-26s -> 0x%08x: %s" % (name, a, D.dis_one(w, a)[0], base + 4 * j, D.dis_one(w2, base + 4 * j)[0]))
                    break
        if op == 0x9 and ((w >> 21) & 31) == 0 and (w & 0x8000) and ((~w) & BITS & 0xffff):
            rx = (w >> 16) & 31
            for j in range(i + 1, min(n, i + 5)):
                w2 = ws[j]
                if (w2 >> 26) == 0 and (w2 & 63) == 0x24 and rx in (((w2 >> 21) & 31), ((w2 >> 16) & 31)):
                    rd = (w2 >> 11) & 31
                    for k in range(j + 1, min(n, j + 4)):
                        w3 = ws[k]
                        if (w3 >> 26) in (0x2b, 0x29, 0x28) and ((w3 >> 16) & 31) == rd and (w3 & 0xffff) in (0, 1):
                            a = base + 4 * i
                            print("%s CLR  0x%08x: %-26s / 0x%08x %s -> 0x%08x: %s" % (name, a, D.dis_one(w, a)[0], base + 4 * j,
                                  D.dis_one(w2, base + 4 * j)[0], base + 4 * k, D.dis_one(w3, base + 4 * k)[0]))
                            break
                    break
