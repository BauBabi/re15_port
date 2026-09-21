# -*- coding: utf-8 -*-
"""Messung 17: die NAHT. Je Punkt: Hintergrund-Helligkeit, Render-Helligkeit,
Maskentiefe. Zeigt Spalte fuer Spalte, wo der gemalte Kasten im Render endet."""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import maskenbild                               # noqa: E402

MASKS = "re15_port/shared_assets/PSX/MASKS"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)
blob = open(os.path.join(MASKS, "ROOM10D0.MSK"), "rb").read()
ms = maskenbild.masken(blob, 7) or []
idx = maskenbild.lies_tim(os.path.join(MASKS, "ROOM10D0_PRI07.TIM"))[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)

X0, X1 = 54, 74
print("HG = Hintergrund (r+g+b), RD = Render, T = Maskentiefe (- = ungedeckt)")
for y in range(126, 160):
    z = []
    for x in range(X0, X1):
        hg = BG[y, x].sum(); rd = REN[y, x].sum()
        tt = str(tief[y, x]) if deck[y, x] else "-"
        z.append("%3d/%3d/%-3s" % (hg, rd, tt))
    print("y%3d " % y + " ".join(z))

print()
print("Spalte | Kasten-Punkte(HG b-r>=0 & hell>=120) | davon gedeckt | Median-Tiefe")
br = BG[..., 2] - BG[..., 0]
lum = BG.sum(2)
for x in range(54, 76):
    m = (br[124:160, x] >= 0) & (lum[124:160, x] >= 120)
    g = m & deck[124:160, x]
    td = tief[124:160, x][g]
    print("  %3d  | %3d | %3d | %s" % (x, m.sum(), g.sum(),
                                       ("%d" % np.median(td)) if g.sum() else "-"))
