# -*- coding: utf-8 -*-
"""Differenz MIT/OHNE Granate je Bild (Framedumps, 320x240): Anzahl abweichender Punkte + bbox.
differenz.py <ordner_mit> <ordner_ohne> <von> <bis> [schritt] [versatz_ohne]
versatz_ohne: Bild f des MIT-Laufs wird mit Bild f-versatz des OHNE-Laufs verglichen
(fuer Laeufe, in denen ein zusaetzliches Modal die Szene um eine feste Bildzahl verschiebt)."""
import os, sys
import numpy as np
from PIL import Image
a, b = sys.argv[1], sys.argv[2]
von, bis = int(sys.argv[3]), int(sys.argv[4])
st = int(sys.argv[5]) if len(sys.argv) > 5 else 1
vs = int(sys.argv[6]) if len(sys.argv) > 6 else 0
for f in range(von, bis + 1, st):
    pa = os.path.join(a, 'f_%06d.ppm' % f); pb = os.path.join(b, 'f_%06d.ppm' % (f - vs))
    if not (os.path.exists(pa) and os.path.exists(pb)):
        continue
    x = np.asarray(Image.open(pa).convert('RGB'), np.int16)
    y = np.asarray(Image.open(pb).convert('RGB'), np.int16)
    d = np.abs(x - y).max(-1) > 0
    n = int(d.sum())
    if n:
        ys, xs = np.nonzero(d)
        print('F%d: %5d Punkte  bbox x%d..%d y%d..%d' % (f, n, xs.min(), xs.max(), ys.min(), ys.max()))
    else:
        print('F%d: %5d Punkte' % (f, n))
