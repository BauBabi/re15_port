# -*- coding: utf-8 -*-
"""Messung 13: Silhouette der fehlenden Stuecke aus dem HINTERGRUND gewinnen.

Fenster = rechts der bisherigen Deckung. Regel wird GEMESSEN, nicht geraten:
ausgegeben werden die Helligkeits-Histogramme von Gegenstand und Boden, und fuer
jede Schwelle die Trennguete gegen die Bodenflaeche (die rechts davon liegt).
"""
import numpy as np
from PIL import Image, ImageDraw

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)

# Fenster, das die verlorenen Punkte und den Gegenstand umschliesst
X0, X1, Y0, Y1 = 52, 84, 116, 168
print("Helligkeit (r+g+b), Fenster x%d..%d y%d..%d" % (X0, X1 - 1, Y0, Y1 - 1))
print("     " + "".join("%4d" % x for x in range(X0, X1)))
for y in range(Y0, Y1):
    print("%3d  " % y + "".join("%4d" % lum[y, x] for x in range(X0, X1)))

print()
print("Blauueberschuss b-r")
print("     " + "".join("%4d" % x for x in range(X0, X1)))
for y in range(Y0, Y1):
    print("%3d  " % y + "".join("%4d" % (BG[y, x, 2] - BG[y, x, 0]) for x in range(X0, X1)))

# Farbbild mit Gitter zum Nachsehen
im = Image.fromarray((np.clip((BG / 255.0) ** 0.42, 0, 1) * 255).astype(np.uint8))
im = im.crop((X0, Y0, X1, Y1))
S = 26
im = im.resize(((X1 - X0) * S, (Y1 - Y0) * S), Image.NEAREST)
dr = ImageDraw.Draw(im)
for x in range(X0, X1):
    dr.line([(x - X0) * S, 0, (x - X0) * S, im.height], fill=(70, 70, 70))
    if x % 2 == 0:
        dr.text(((x - X0) * S + 2, 2), str(x), fill=(255, 255, 0))
for y in range(Y0, Y1):
    dr.line([0, (y - Y0) * S, im.width, (y - Y0) * S], fill=(70, 70, 70))
    if y % 2 == 0:
        dr.text((2, (y - Y0) * S + 2), str(y), fill=(255, 255, 0))
im.save(OUT + "39_fenster_gitter.png")
print("39_fenster_gitter.png x%d..%d y%d..%d S=%d" % (X0, X1 - 1, Y0, Y1 - 1, S))
