# -*- coding: utf-8 -*-
"""Verdeckt die Granate die Sicherung? Je Bild: Sicherungs-Maske = Punkte, in denen sich der Lauf
OHNE Granate (nur Sicherung) vom Lauf OHNE BEIDE unterscheidet; gezaehlt wird, wie viele dieser
Punkte der Lauf MIT Granate veraendert.
sicherung_frei.py <mit> <ohne_granate> <ohne_beide> <von> <bis> [schritt]"""
import os, sys
import numpy as np
from PIL import Image
m, o, ob = sys.argv[1:4]
von, bis = int(sys.argv[4]), int(sys.argv[5]); st = int(sys.argv[6]) if len(sys.argv) > 6 else 1
L = lambda d, f: np.asarray(Image.open(os.path.join(d, 'f_%06d.ppm' % f)).convert('RGB'), np.int16)
for f in range(von, bis + 1, st):
    try:
        a, b, c = L(m, f), L(o, f), L(ob, f)
    except FileNotFoundError:
        continue
    sich = np.abs(b - c).max(-1) > 0
    gr = np.abs(a - b).max(-1) > 0
    print('F%d: Sicherung %4d Punkte, davon von der Granate veraendert %3d | Granate %4d Punkte' %
          (f, int(sich.sum()), int((sich & gr).sum()), int(gr.sum())))
