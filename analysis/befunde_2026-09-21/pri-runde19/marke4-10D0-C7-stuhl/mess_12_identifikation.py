# -*- coding: utf-8 -*-
"""Messung 12: Was ist der helle Gegenstand bei x50..70? Farbe messen, nicht raten."""
import numpy as np
from PIL import Image

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)

def stat(tag, x0, x1, y0, y1, thr=140):
    w = BG[y0:y1, x0:x1]
    l = w.sum(2)
    m = l > thr
    if m.sum() == 0:
        print("%-28s keine Punkte > %d" % (tag, thr))
        return
    c = w[m]
    print("%-28s n=%4d  rgb Median (%3d,%3d,%3d)  b-r Median %+d  Helligkeit %d..%d"
          % (tag, m.sum(), np.median(c[:, 0]), np.median(c[:, 1]), np.median(c[:, 2]),
             np.median(c[:, 2].astype(int) - c[:, 0].astype(int)), l[m].min(), l[m].max()))

stat("heller Block x50..70 y126..160", 50, 70, 126, 160)
stat("Kegel rechts x72..100 y112..155", 72, 100, 112, 155)
stat("Chromrohr Stuhl x0..30 y118..150", 0, 30, 118, 150)
stat("Chromrohr vorn x10..50 y190..230", 10, 50, 190, 230)
stat("Tischplatte x5..45 y150..175", 5, 45, 150, 175)

# Bild in Farbe, ohne Gamma, gross
box = (38, 106, 104, 172)
S = 16
im = Image.fromarray(BG.astype(np.uint8)).crop(box)
im.resize(((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.LANCZOS).save(
    OUT + "38_gegenstand_farbe.png")
print("38_gegenstand_farbe.png box", box)
