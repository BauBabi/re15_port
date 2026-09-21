# -*- coding: utf-8 -*-
"""Silhouette des blaugrauen Kastens am Tischende aus dem HINTERGRUND gewinnen.

Regel (GEMESSEN, mess_14_regel.py):
  Fenster  x46..72 / y124..159 (umschliesst den Gegenstand; rechts davon nur Boden)
  Kern     b-r >= 0  UND  (r+g+b) >= 120
           -> Boden-Fehlalarm 0 von 609 Referenzpunkten (x72..100 / y150..170),
              Treffer 69,1 % der Gegenstandsreferenz
  groesste 8er-Komponente, 3x3 geschlossen, Loecher gefuellt
Der Kasten ist ein SOLIDER Kasten - seine Silhouette IST eine gefuellte Flaeche,
das Fuellen erfindet also keine Form, es stellt sie wieder her (vgl. Huelle der
Pflanze in ROOM10D0 C6).

Ausgabe: pri/STAGE1/10D0/07_03.png (RGB aus dem Hintergrund, Alpha = Silhouette)
plus die Messzahlen.
"""
import os

import numpy as np
from PIL import Image
from scipy import ndimage as nd

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)
br = BG[..., 2] - BG[..., 0]
X0, X1, Y0, Y1 = 46, 73, 124, 160

kern = np.zeros((240, 320), bool)
kern[Y0:Y1, X0:X1] = (br[Y0:Y1, X0:X1] >= 0) & (lum[Y0:Y1, X0:X1] >= 120)
print("Kern: %d Punkte" % kern.sum())

lab, n = nd.label(kern, np.ones((3, 3), bool))
gr = np.bincount(lab.ravel())
gr[0] = 0
K = lab == gr.argmax()
print("groesste Komponente: %d Punkte (von %d Komponenten, Groessen %s)"
      % (K.sum(), n, sorted(gr[1:], reverse=True)[:6]))

H = nd.binary_closing(K, np.ones((3, 3), bool))
H = nd.binary_fill_holes(H)
H[:Y0] = False
H[Y1:] = False
H[:, :X0] = False
H[:, X1:] = False
ys, xs = np.nonzero(H)
print("Huelle: %d Punkte, x%d..%d y%d..%d" % (H.sum(), xs.min(), xs.max(), ys.min(), ys.max()))
print("   davon mit b-r < -4 (bodenartig): %d  (%.1f %%)"
      % (((br[ys, xs] < -4)).sum(), 100.0 * (br[ys, xs] < -4).mean()))
print("   Helligkeit Median %d, b-r Median %+.0f" % (np.median(lum[ys, xs]), np.median(br[ys, xs])))

# Nullmodell fuer die Fuellung: wieviel Flaeche kommt durch Schliessen/Fuellen dazu?
print("   durch Schliessen+Fuellen dazugekommen: %d Punkte (%.0f %% der Huelle)"
      % (H.sum() - (K & H).sum(), 100.0 * (H.sum() - (K & H).sum()) / H.sum()))

# PNG schreiben: Lage = bbox, RGB aus dem Hintergrund (damit maske_aus_png/Lagemessung passt)
x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
out = np.zeros((y1 - y0, x1 - x0, 4), np.uint8)
out[..., :3] = BG[y0:y1, x0:x1].astype(np.uint8)
out[..., 3] = np.where(H[y0:y1, x0:x1], 255, 0)
p = "pri/STAGE1/10D0/07_03.png"
Image.fromarray(out, "RGBA").save(p)
print("geschrieben: %s  %dx%d, Lage x=%d y=%d, %d opake Punkte"
      % (p, x1 - x0, y1 - y0, x0, y0, (out[..., 3] > 0).sum()))

# Ueberlappung mit der Nutzer-Freistellung
a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
png = np.zeros((240, 320), bool)
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110
print("Ueberlappung Huelle & Nutzer-Freistellung: %d Punkte" % (H & png).sum())
oy, ox = np.nonzero(H & png)
if len(oy):
    print("   Lage x%d..%d y%d..%d, Helligkeit Median %d, b-r Median %+.0f"
          % (ox.min(), ox.max(), oy.min(), oy.max(),
             np.median(lum[oy, ox]), np.median(br[oy, ox])))
    print("   davon hell (>=120): %d" % (lum[oy, ox] >= 120).sum())
np.save(os.path.join("analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl",
                     "huelle.npy"), H)
