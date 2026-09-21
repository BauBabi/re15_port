# -*- coding: utf-8 -*-
"""Messung 24: Welche Punkte der Nutzer-Freistellung gehoeren zum KASTEN?

Der Kasten steht nachweislich hinter dem Spieler (Sockel Bildzeile 154..158 gegen
Fuss 179; Kamera-z 7450..7790 gegen 6052). Die Freistellung 07_01.png nimmt seinen
hellen linken Rand mit und gibt ihm die Stuhltiefe 53..55.
Hier: die Punkte zaehlen, ihre Farbe pruefen, und einen Schnitt-Kasten belegen.
"""
import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)
br = BG[..., 2] - BG[..., 0]
a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
png = np.zeros((240, 320), bool)
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

print("Punkte der Freistellung je Kasten-Kandidat (x0,y0,x1,y1 halboffen):")
for (x0, y0, x1, y1) in ((58, 133, 63, 142), (57, 132, 64, 143), (56, 126, 68, 160),
                         (58, 126, 68, 160)):
    m = np.zeros((240, 320), bool)
    m[y0:y1, x0:x1] = True
    s = png & m
    if s.sum() == 0:
        print("   x%d..%d y%d..%d: 0 Punkte" % (x0, x1 - 1, y0, y1 - 1))
        continue
    ys, xs = np.nonzero(s)
    b = br[ys, xs]
    l = lum[ys, xs]
    print("   x%d..%d y%d..%d: %d Punkte | b-r %+d..%+d (Median %+.0f) | Helligkeit %d..%d (Median %d)"
          % (x0, x1 - 1, y0, y1 - 1, s.sum(), b.min(), b.max(), np.median(b),
             l.min(), l.max(), np.median(l)))
    print("        davon blau&hell (b-r>=5 und hell>=120): %d, Rest: %d"
          % (((b >= 5) & (l >= 120)).sum(), (~((b >= 5) & (l >= 120))).sum()))

print()
print("Vergleich: die Freistellung INSGESAMT")
ys, xs = np.nonzero(png)
b = br[ys, xs]; l = lum[ys, xs]
print("   %d Punkte | b-r Median %+.0f | Helligkeit Median %d" % (png.sum(), np.median(b), np.median(l)))
print("   blau&hell (b-r>=5 und hell>=120) in der GANZEN Freistellung: %d"
      % ((b >= 5) & (l >= 120)).sum())
ysb, xsb = np.nonzero(png & (br >= 5) & (lum >= 120))
if len(ysb):
    print("   deren Lage: x%d..%d y%d..%d" % (xsb.min(), xsb.max(), ysb.min(), ysb.max()))
    from collections import Counter
    c = Counter(zip(ysb // 10 * 10, xsb // 10 * 10))
    print("   Haeufung je 10x10-Feld (y,x):", sorted(c.items(), key=lambda kv: -kv[1])[:12])
