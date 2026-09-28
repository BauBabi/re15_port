#!/usr/bin/env python3
"""Zerlegt das ausgelieferte Item-Bild 0x40 in RAHMEN / HINTERGRUND / GEGENSTAND und zaehlt.

    python analysis/befunde_runde30/sicherung_werkzeug/itps_hintergrund_40.py
"""
import os
import struct

import numpy as np
from PIL import Image

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
ZIEL = os.path.join(REPO, "build", "r30_sicherung")
S, W, H = 0x3000, 112, 72
d = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
b = d[0x40 * S:0x41 * S]
clut = np.array(struct.unpack_from("<256H", b, 0x14), np.uint16)
idx = np.frombuffer(b, np.uint8, W * H, 0x220).reshape(H, W)
w = clut[idx]
r, g, bl = ((w & 31) << 3).astype(int), (((w >> 5) & 31) << 3).astype(int), (((w >> 10) & 31) << 3).astype(int)
print("Zeile 0..3 und Spalte 0..3 (Farbworte) = Rahmen:")
for y in range(4):
    print("   y=%d: %s" % (y, " ".join("%04X" % v for v in w[y, :6])))
print("   letzte Zeilen: %s" % " | ".join(" ".join("%04X" % v for v in w[y, :4]) for y in range(68, 72)))
# Rahmenbreite: erste Zeile/Spalte, in der das haeufigste Wort 0x1C00 ist
for y in range(6):
    print("   Zeile %d: Anteil 0x1C00 = %d von %d" % (y, int((w[y] == 0x1C00).sum()), W))
for x in range(6):
    print("   Spalte %d: Anteil 0x1C00 = %d von %d" % (x, int((w[:, x] == 0x1C00).sum()), H))
innen = np.zeros((H, W), bool)
innen[2:-2, 2:-2] = True
flach = (w == 0x1C00) & innen
blau = (bl > r + 25) & (bl > g + 25) & innen & ~flach
rest = innen & ~flach & ~blau
print("Innenflaeche %d px: flach 0x1C00 %d | anderes Blau %d | Rest (Gegenstand + Schatten) %d"
      % (int(innen.sum()), int(flach.sum()), int(blau.sum()), int(rest.sum())))
wb, zb = np.unique(w[blau], return_counts=True)
print("'anderes Blau': %d Worte: %s" % (len(wb), ", ".join("%04X x%d" % (a, c) for a, c in zip(wb, zb))))
ys, xs = np.nonzero(blau)
print("   Lage: x %d..%d, y %d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
bild = np.zeros((H, W, 3), np.uint8)
bild[..., 0], bild[..., 1], bild[..., 2] = r, g, bl
bild[rest] = (255, 0, 255)
bild[blau] = (0, 255, 0)
Image.fromarray(bild).resize((W * 4, H * 4), Image.NEAREST).save(os.path.join(ZIEL, "itps_40_zerlegt_4x.png"))
