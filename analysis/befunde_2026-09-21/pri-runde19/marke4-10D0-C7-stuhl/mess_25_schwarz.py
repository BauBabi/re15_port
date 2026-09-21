# -*- coding: utf-8 -*-
"""Messung 25: der pechschwarze Fleck im Render — Ausdehnung und was er verdeckt."""
import numpy as np
from PIL import Image

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)

s = REN.sum(2) == 0
print("Punkte mit Render == (0,0,0): %d" % s.sum())
ys, xs = np.nonzero(s)
if len(ys):
    print("Lage x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
    print("Hintergrund dort: Helligkeit %d..%d (Median %d)"
          % (BG[ys, xs].sum(1).min(), BG[ys, xs].sum(1).max(), np.median(BG[ys, xs].sum(1))))
    # wie viele davon liegen auf HELLEM Hintergrund (>=100)?
    hell = BG[ys, xs].sum(1) >= 60
    print("davon auf Hintergrund >= 60: %d" % hell.sum())
    print("Zeilenweise:")
    for y in range(ys.min(), ys.max() + 1):
        xx = np.nonzero(s[y])[0]
        if len(xx):
            print("  y%3d  x%3d..%3d (%d)" % (y, xx.min(), xx.max(), len(xx)))
# Wie viele schwarze Punkte hat der Hintergrund selbst?
print()
print("Hintergrund mit (0,0,0): %d Punkte" % (BG.sum(2) == 0).sum())
