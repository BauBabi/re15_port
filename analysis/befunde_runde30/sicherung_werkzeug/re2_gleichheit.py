#!/usr/bin/env python3
"""Stichprobe (Fortsetzungs-Agent): Ist das RE1.5-Icon 0x40 'Fuse' bytegleich mit einem
RE2-Icon?  Vergleicht ITEMALL.PIX Tile 0x40/0x41 (RE1.5) gegen ALLE RE2-Tiles.

    python analysis/befunde_runde30/sicherung_werkzeug/re2_gleichheit.py
"""
import os
import struct

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
A = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "DATA", "ITEMALL.PIX"), "rb").read()
B = open(os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ITEMALL.PIX"), "rb").read()
na, nb = len(A) // 1200, len(B) // 1200
print("RE1.5 ITEMALL.PIX %d B = %d Tiles | RE2 ITEMALL.PIX %d B = %d Tiles" % (len(A), na, len(B), nb))
for t in (0x3F, 0x40, 0x41):
    a = A[t * 1200:(t + 1) * 1200]
    best = None
    for k in range(nb):
        b = B[k * 1200:(k + 1) * 1200]
        gleich = sum(1 for i in range(1200) if a[i] == b[i])
        if best is None or gleich > best[0]:
            best = (gleich, k)
    print("RE1.5 Tile 0x%02X @Datei 0x%05X: bestes RE2-Tile = %d (0x%02X) @Datei 0x%05X, %d von 1200 Indexbytes gleich"
          % (t, t * 1200, best[1], best[1], best[1] * 1200, best[0]))
