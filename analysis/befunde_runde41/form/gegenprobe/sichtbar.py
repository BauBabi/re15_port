#!/usr/bin/env python3
"""Sichtbare Pixel je Knochen im Kamerabild (Maler aus render.py). python sichtbar.py [gerippe.json]"""
import sys
import numpy as np
from render import *

g = lade(sys.argv[1] if len(sys.argv) > 1 else None)
m = masken()
for nr in (1, 2):
    kn = kamerabild(g, nr)
    v = np.zeros_like(kn, bool)
    for t in TEILE[nr]:
        v |= m[t]
    print('Bild', nr, 'Figur px', int((kn >= 0).sum()), 'ausserhalb aller Teile', int(((kn >= 0) & ~v).sum()))
    for k in g['knochen']:
        n = int((kn == k['index']).sum())
        au = int(((kn == k['index']) & ~v).sum())
        if n:
            print('   %-16s %6d  davon ausserhalb Teile %5d' % (k['name'], n, au))
