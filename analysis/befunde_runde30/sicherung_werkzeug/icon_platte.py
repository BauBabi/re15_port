#!/usr/bin/env python3
"""HINTERGRUNDPLATTE der Inventar-Icons (ITEMALL.PIX), Fortsetzungs-Agent.

Je Bildpunkt das haeufigste HINTERGRUND-Index ueber alle 72 Tiles. Hintergrund-Indizes sind
die, deren Farbe in DATA/ST_00.TIM CLUT-Zeile 0 blau-dominant ist UND die in den vier Ecken
der Tiles vorkommen (dort liegt nie ein Gegenstand).

Ausgabe: build/r30_sicherung/icon_platte_idx.npy, icon_platte_8x.png

    python analysis/befunde_runde30/sicherung_werkzeug/icon_platte.py
"""
import os
import struct

import numpy as np
from PIL import Image

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
ZIEL = os.path.join(REPO, "build", "r30_sicherung")

pix = open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read()
st00 = open(os.path.join(PSX, "DATA", "ST_00.TIM"), "rb").read()
cl = np.array(struct.unpack_from("<256H", st00, 20), np.uint16)
rgb = np.stack([(cl & 31) << 3, ((cl >> 5) & 31) << 3, ((cl >> 10) & 31) << 3], -1).astype(int)
t = np.frombuffer(pix, np.uint8).reshape(-1, 30, 40)
ecken = np.concatenate([t[:, :3, :3].ravel(), t[:, :3, -3:].ravel(),
                        t[:, -3:, :3].ravel(), t[:, -3:, -3:].ravel()])
w_, z_ = np.unique(ecken, return_counts=True)
print("Indizes in den vier 3x3-Ecken aller %d Tiles:" % t.shape[0])
hg = []
for i, z in sorted(zip(w_, z_), key=lambda a: -a[1]):
    r, g, b = rgb[i]
    blau = b > r + 25 and b > g + 25
    print("   Index 0x%02X x%4d  CLUT-Wort 0x%04X  RGB (%d,%d,%d)  %s"
          % (i, z, cl[i], r, g, b, "HINTERGRUND" if blau and z >= 20 else ""))
    if blau and z >= 20:
        hg.append(int(i))
print("Hintergrund-Indizes:", " ".join("0x%02X" % i for i in hg))
pl = np.zeros((30, 40), np.uint8)
stz = np.zeros((30, 40), int)
for y in range(30):
    for x in range(40):
        best = (-1, 0)
        for i in hg:
            c = int((t[:, y, x] == i).sum())
            if c > best[0]:
                best = (c, i)
        stz[y, x], pl[y, x] = best
print("Stuetze je Bildpunkt (nur Hintergrund-Indizes): min %d, Median %d, max %d"
      % (stz.min(), int(np.median(stz)), stz.max()))
print("Bildpunkte mit Stuetze < 3: %d" % int((stz < 3).sum()))
np.save(os.path.join(ZIEL, "icon_platte_idx.npy"), pl)
np.save(os.path.join(ZIEL, "icon_platte_stuetze.npy"), stz)
Image.fromarray(rgb[pl].astype(np.uint8)).resize((320, 240), Image.NEAREST).save(
    os.path.join(ZIEL, "icon_platte_8x.png"))
for y in range(0, 30, 3):
    print("   y=%2d  %s" % (y, "".join({hg[0]: "a", hg[1]: "b"}.get(int(v), "c") if len(hg) > 1 else "a"
                                       for v in pl[y])))
# Gegenprobe: wie viele Bildpunkte von Tile 0x40 ausserhalb des Gegenstands stimmen mit der Platte?
t40 = t[0x40]
ist_hg = np.isin(t40, hg)
print("Tile 0x40: %d Hintergrundpunkte, davon %d gleich der Platte"
      % (int(ist_hg.sum()), int((t40[ist_hg] == pl[ist_hg]).sum())))
