# -*- coding: utf-8 -*-
"""Runde 31 / H — sieht man die liegen gebliebene Sicherung durch die GESCHLOSSENE Kuppel am Ende
einer Fahrt nach "No"? Vergleich Bild f+versatz eines Laufs MIT Sicherung gegen Bild f eines Laufs
OHNE beide Gegenstaende (Versatz = Parkbild MIT - Parkbild OHNE aus dem Mess-Protokoll).
kuppel_zu_ende.py <mit> <ohne> <versatz> <von> <bis> [schritt]"""
import os, sys
import numpy as np
from PIL import Image
a, b, vs, von, bis = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
st = int(sys.argv[6]) if len(sys.argv) > 6 else 2
def L(o, f):
    p = os.path.join(o, 'f_%06d.ppm' % f)
    return np.asarray(Image.open(p).convert('RGB'), np.int16) if os.path.exists(p) else None
for f in range(von, bis + 1, st):
    x, y = L(a, f + vs), L(b, f)
    if x is None or y is None:
        continue
    d = np.abs(x - y).max(-1) > 0
    ys, xs = np.nonzero(d)
    print('MIT Bild %d gegen OHNE Bild %d: %4d Punkte%s' % (f + vs, f, d.sum(),
          ' bbox x%d..%d y%d..%d' % (xs.min(), xs.max(), ys.min(), ys.max()) if d.sum() else ''))
