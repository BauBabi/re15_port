# -*- coding: utf-8 -*-
"""Versatz zwischen einem MIT-Lauf (zusaetzliches Granaten-Modal friert die Szene ein) und dem
OHNE-Lauf: das Delta, bei dem die meisten Bilder ausserhalb eines Fensters bildgleich sind.
versatz.py <mit> <ohne> <von> <bis> <dmin> <dmax>"""
import os, sys
import numpy as np
from PIL import Image
a, b = sys.argv[1], sys.argv[2]
von, bis, dmin, dmax = (int(x) for x in sys.argv[3:7])
cache = {}
def lade(o, f):
    k = (o, f)
    if k not in cache:
        p = os.path.join(o, 'f_%06d.ppm' % f)
        cache[k] = np.asarray(Image.open(p).convert('RGB'), np.int16) if os.path.exists(p) else None
    return cache[k]
best = None
for d in range(dmin, dmax + 1):
    summe = 0; n = 0
    for f in range(von, bis + 1, 5):
        x, y = lade(a, f), lade(b, f - d)
        if x is None or y is None: continue
        summe += int((np.abs(x - y).max(-1) > 0).sum()); n += 1
    if n and (best is None or summe < best[0]): best = (summe, d, n)
print('bester Versatz %d: Summe %d abweichende Punkte ueber %d Bilder' % (best[1], best[0], best[2]))
