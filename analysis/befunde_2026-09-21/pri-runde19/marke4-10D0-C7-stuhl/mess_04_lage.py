# -*- coding: utf-8 -*-
"""Messung 4: sitzt die Nutzer-Freistellung an der eingetragenen Lage?

Die Freistellung traegt die HINTERGRUND-Pixel ihrer Silhouette. Damit ist die
Lage MESSBAR: man schiebt das PNG ueber alle (dx,dy) und vergleicht die RGB der
opaken Punkte mit dem Hintergrundbild. Nullmodell: die Verteilung ALLER Lagen,
nicht nur der Bestwert.
"""
import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
H, W = BG.shape[:2]

for nm, lage in (("07_01.png", (0, 123)), ("07_02.png", (0, 146))):
    a = np.asarray(Image.open("pri/STAGE1/10D0/" + nm).convert("RGBA"), int)
    h, w = a.shape[:2]
    m = a[..., 3] >= 110
    rgb = a[..., :3]
    ys, xs = np.nonzero(m)
    n = len(ys)
    erg = []
    for oy in range(0, H - h + 1):
        for ox in range(0, W - w + 1):
            diff = np.abs(BG[ys + oy, xs + ox] - rgb[ys, xs]).sum()
            erg.append((diff / float(n), ox, oy))
    erg.sort()
    best = erg[0]
    ein = [e for e in erg if (e[1], e[2]) == lage][0]
    werte = np.array([e[0] for e in erg])
    print("%s  %dx%d, %d opake Punkte" % (nm, w, h, n))
    print("   BESTE Lage      x=%3d y=%3d  mittl. |dRGB| = %.3f" % (best[1], best[2], best[0]))
    print("   eingetragen     x=%3d y=%3d  mittl. |dRGB| = %.3f   (Rang %d von %d)"
          % (lage[0], lage[1], ein[0], erg.index(ein) + 1, len(erg)))
    print("   Nullmodell: Median aller Lagen %.3f, 1%%-Quantil %.3f, Minimum %.3f"
          % (np.median(werte), np.percentile(werte, 1), werte.min()))
    print("   naechste 5 Lagen:", [(e[1], e[2], round(e[0], 3)) for e in erg[1:6]])
    # Trefferquote: Anteil Punkte mit |dRGB| <= 3 an bester und eingetragener Lage
    for tag, (ox, oy) in (("beste", (best[1], best[2])), ("eingetragen", lage)):
        dd = np.abs(BG[ys + oy, xs + ox] - rgb[ys, xs]).sum(1)
        print("   %-12s bitgleich %.1f%%  |dRGB|<=3 %.1f%%"
              % (tag, 100.0 * (dd == 0).mean(), 100.0 * (dd <= 3).mean()))
