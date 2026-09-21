"""Die Bilder des Dossiers: ganze Abzuege vorher/nachher und eine Karte der 187 Punkte
des Nutzers auf dem Hintergrund (gruen = seine Marke, weiss = seine alte Freistellung).
"""
import os

import numpy as np
from PIL import Image

Z = "analysis/befunde_2026-09-21/pri-runde19c-saum"
Image.open("build/r19c/abzug_vorher/bild000260.png").save(os.path.join(Z, "40_abzug_vorher_ganz.png"))
Image.open("build/r19c/abzug_nachher/bild000260.png").save(os.path.join(Z, "41_abzug_nachher_ganz.png"))

bg = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB")).astype(np.uint8)
alt = np.load("build/r19c/alt.npy")
neu = np.load("build/r19c/neu.npy")
saum = neu & ~alt
k = bg.copy()
k[alt] = (k[alt] // 2 + np.array([110, 110, 110], np.uint8) // 2)      # alte Freistellung heller
k[saum] = [40, 230, 90]                                               # die 187 Marken des Nutzers
x0, x1, y0, y1 = 0, 80, 118, 240
F = 10
Image.fromarray(k[y0:y1, x0:x1]).resize(((x1 - x0) * F, (y1 - y0) * F), Image.NEAREST).save(
    os.path.join(Z, "42_saum_auf_hintergrund.png"))
print("42_saum_auf_hintergrund.png: %d alte Punkte (hell), %d neue (gruen), %d-fach"
      % (int(alt.sum()), int(saum.sum()), F))
