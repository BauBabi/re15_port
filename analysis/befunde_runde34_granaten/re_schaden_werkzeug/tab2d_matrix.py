#!/usr/bin/env python3
"""tab2d_matrix.py - druckt eine 2D-Zeigertabelle eines Overlays als Matrix Zeile(+0x5) x Spalte(+0x6).
Index = base + zeile*32 + spalte*4 (8 Spalten), wie im Zombie-Dispatch
(`sll v1,v1,5` / `sll v0,v0,2` @0x80105af8-b00 bzw. @0x80106be8-f0). Runde 34 / re_schaden_resolver.
Aufruf: python tab2d_matrix.py <STAGEn.BIN> <base_hex> <zeilen>"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
binname, base, rows = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3])
b = open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", binname), "rb").read()
print(f"{binname} @0x{base:08x}  Zeile = +0x5, Spalte = +0x6 (0..7), '--------' = NULL")
print("Zeile  " + " ".join(f"Sp{c}      " for c in range(8)))
for r in range(rows):
    ws = [struct.unpack_from("<I", b, base + r * 32 + c * 4 - 0x80100000)[0] for c in range(8)]
    print(f"{r:5d}  " + " ".join(("--------" if w == 0 else f"{w:08x}") + "  " for w in ws))
