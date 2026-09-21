# -*- coding: utf-8 -*-
"""Messung 9: rechte Kante der Nutzer-Freistellung je Zeile, gegen die
Helligkeitskante des gemalten Gegenstands. Zeigt, WO die Freistellung aufhoert."""
import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)
a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
PX, PY = 0, 123
m = a[..., 3] >= 110

print("y  | PNG-Spalten (opak, absolut)                         | hell(>150) x50..99")
for y in range(123, 170):
    r = y - PY
    if r < 0 or r >= a.shape[0]:
        continue
    xs = np.nonzero(m[r])[0] + PX
    hell = [x for x in range(50, 100) if lum[y, x] > 150]
    sp = "%d..%d (n=%d)" % (xs.min(), xs.max(), len(xs)) if len(xs) else "-"
    print("%3d| %-24s | %s" % (y, sp, ",".join(str(x) for x in hell)))

# Der rechte Rand des PNG: ist er abgeschnitten (Punkte in der letzten Spalte)?
print()
print("PNG-Breite %d, opake Punkte in Spalte %d: %d"
      % (a.shape[1], a.shape[1] - 1, m[:, -1].sum()))
print("opake Punkte in Spalte 0: %d, in Zeile 0: %d, in letzter Zeile: %d"
      % (m[:, 0].sum(), m[0].sum(), m[-1].sum()))
