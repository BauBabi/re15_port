# -*- coding: utf-8 -*-
"""Messung 23: Der helle Strang, der VOR dem Spieler gezeichnet wird — was ist er?

Alle gedeckten Punkte im Fenster x54..72 / y126..160 mit RGB, b-r und Maskentiefe.
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import maskenbild                               # noqa: E402

MASKS = "re15_port/shared_assets/PSX/MASKS"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
blob = open(os.path.join(MASKS, "ROOM10D0.MSK"), "rb").read()
ms = maskenbild.masken(blob, 7) or []
idx = maskenbild.lies_tim(os.path.join(MASKS, "ROOM10D0_PRI07.TIM"))[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)

print("gedeckte Punkte x54..72 / y126..160 mit Helligkeit >= 120:")
print("  x   y   r   g   b   b-r  hell  Tiefe")
n = 0
for y in range(126, 161):
    for x in range(54, 73):
        if deck[y, x] and BG[y, x].sum() >= 120:
            r, g, b = BG[y, x]
            print("%3d %3d %3d %3d %3d  %+4d  %4d   %3d" % (x, y, r, g, b, b - r, r + g + b, tief[y, x]))
            n += 1
print("%d Punkte" % n)

print()
print("Zum Vergleich — das Chromrohr des VORDEREN Stuhls (gedeckt, y190..230):")
sel = []
for y in range(190, 235):
    for x in range(0, 60):
        if deck[y, x] and BG[y, x].sum() >= 200:
            r, g, b = BG[y, x]
            sel.append((b - r, r + g + b))
if sel:
    a = np.array(sel)
    print("  n=%d  b-r Median %+.0f (25..75%%: %+.0f..%+.0f)  Helligkeit Median %d"
          % (len(a), np.median(a[:, 0]), np.percentile(a[:, 0], 25), np.percentile(a[:, 0], 75),
             np.median(a[:, 1])))
