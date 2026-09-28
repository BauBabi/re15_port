#!/usr/bin/env python3
"""Ist die Sicherung im Spiel TEXTURIERT? Farbstatistik der Bildpunkte, die sie beitraegt
(Differenz MIT/OHNE), gegen die Farbstatistik ihrer eingebackenen Textur.

    python .../farbe_im_spiel.py <lauf_mit> <lauf_ohne> <bild>
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import REPO, inc_bytes, tim_lesen, rgb15  # noqa: E402

Z = os.path.join(REPO, "build", "r30_sicherung")
a, b, f = sys.argv[1], sys.argv[2], int(sys.argv[3])
A = np.asarray(Image.open(os.path.join(Z, a, "f_%06d.ppm" % f)).convert("RGB")).astype(int)
B = np.asarray(Image.open(os.path.join(Z, b, "f_%06d.ppm" % f)).convert("RGB")).astype(int)
d = np.abs(A - B).max(2) > 4
px = A[d]
print("%s F%d: %d Bildpunkte der Sicherung (960x720)" % (a, f, len(px)))
if len(px):
    farben = set(map(tuple, px.tolist()))
    print("  Mittel RGB (%.0f,%.0f,%.0f)  Spanne Helligkeit %d..%d  verschiedene Farben %d"
          % (px[:, 0].mean(), px[:, 1].mean(), px[:, 2].mean(),
             px.mean(1).min(), px.mean(1).max(), len(farben)))
w, h, idx, cluts, kopf = tim_lesen(inc_bytes("re15_sicherung_tim"))
t = np.array([[rgb15(cluts[0][idx[y][x]]) for x in range(w)] for y in range(h)])
print("Textur gen/sicherung_prop.inc %dx%d: Mittel RGB (%.0f,%.0f,%.0f), verschiedene Farben %d, Kopf %s"
      % (w, h, t[..., 0].mean(), t[..., 1].mean(), t[..., 2].mean(),
         len(set(map(tuple, t.reshape(-1, 3).tolist()))), kopf))
