#!/usr/bin/env python3
"""tab2d_suche.py - sucht in allen STAGEn.BIN das 2D-Dispatch-Muster der Humanoid-Wurzeln
    lui a0,0x8012|0x8011 ; addiu a0,a0,X ; lbu v1,5(a1) ; lbu v0,6(a1) ; sll v1,v1,5 ; addu v1,v1,a0 ; sll v0,v0,2
und druckt Tabellenbasis, Zeile 9 (Explosion, Reaktion DAT_8006f430[2]) und Zeile 3 (Referenz).
Runde 34 / re_schaden_resolver."""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
def w(b, a): return struct.unpack_from("<I", b, a - 0x80100000)[0]
for n in range(1, 7):
    f = f"STAGE{n}.BIN"
    b = open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", f), "rb").read()
    end = 0x80100000 + len(b)
    for a in range(0x80100000, end - 32, 4):
        x = w(b, a)
        # sll v1,v1,5  = 0x00031940 ; addu v1,v1,a0 = 0x00641821 ; sll v0,v0,2 = 0x00021080
        if x == 0x00031940 and w(b, a + 4) == 0x00641821 and w(b, a + 8) == 0x00021080:
            # rueckwaerts lui/addiu a0 suchen
            base = None
            for k in range(1, 8):
                y = w(b, a - 4 * k)
                if (y >> 16) == 0x2484:  # addiu a0,a0,imm
                    imm = y & 0xffff; imm = imm - 0x10000 if imm & 0x8000 else imm
                    z = w(b, a - 4 * k - 4)
                    if (z >> 16) == 0x3c04:
                        base = ((z & 0xffff) << 16) + imm
                    break
            if base is None or not (0x80100000 <= base < end):
                print(f"{f} @0x{a:08x}: Muster, Basis unbekannt"); continue
            def row(r):
                return " ".join(("--" if w(b, base + r * 32 + c * 4) == 0 else f"{w(b, base + r*32 + c*4):08x}") for c in range(8))
            print(f"{f} Muster @0x{a:08x} Basis 0x{base:08x}\n   Zeile 3: {row(3)}\n   Zeile 9: {row(9)}")
