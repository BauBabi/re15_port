# -*- coding: utf-8 -*-
"""Messung 8: wo endet der GEMALTE Gegenstand, wo endet die Deckung?

Zeile fuer Zeile: rechte Kante der PBM-Deckung gegen die rechte Kante des hellen
Gegenstands im Hintergrund. Der Gegenstand ist hier ueber die Helligkeit trennbar
(die Lehne/der Bezug liegt weit ueber Boden und Schatten) - die Schwelle wird
mitgemessen, nicht geraten: ausgegeben werden 3 Schwellen.
"""
import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
COV = ~np.asarray(Image.open("re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM"))
lum = BG.sum(2)

print("Hintergrund-Helligkeit (r+g+b) im Fenster x50..84, y118..166")
print("     " + "".join("%4d" % x for x in range(50, 85, 2)))
for y in range(118, 167, 2):
    print("%3d  " % y + "".join("%4d" % lum[y, x] for x in range(50, 85, 2)))

print()
print("y   Deckung-xmax | Gegenstand-xmax bei Schwelle 150/200/260 (zusammenhaengend ab x=50)")
for y in range(118, 170):
    xs = np.nonzero(COV[y])[0]
    cm = xs.max() if len(xs) else -1
    aus = []
    for thr in (150, 200, 260):
        xm = -1
        for x in range(50, 100):
            if lum[y, x] > thr:
                xm = x
        aus.append(xm)
    print("%3d  %4d  |  %4d %4d %4d" % (y, cm, aus[0], aus[1], aus[2]))
