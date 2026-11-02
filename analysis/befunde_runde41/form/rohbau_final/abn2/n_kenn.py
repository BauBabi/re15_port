#!/usr/bin/env python3
"""Welche Knochen und Teile stehen an einer Bildstelle vorn. python abn2/n_kenn.py STAND bild x0 y0 x1 y1 [schritt]"""
import json
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import HIER, kennbild
import rf_plan
from rf_fit import Passung

g, t = rf_plan.plan()
s = json.load(open(os.path.join(HIER, sys.argv[1])))
n, x0, y0, x1, y1 = [int(v) for v in sys.argv[2:7]]
sw = int(sys.argv[7]) if len(sys.argv) > 7 else 2
pa = Passung(g, t, s)
v, L, C = pa.netz.ecken(pa.lr, pa.c)
R, T = pa.kette_pose(n)
q = pa.netz.welt(v, R, T)
xy = pa.kam[n](q).detach().cpu().numpy()
z = pa.kam[n].tiefe(q).detach().cpu().numpy()
tr = pa.netz.tris.cpu().numpy()
# Teil je Dreieck
tt = []
for i, p in enumerate(t):
    tt += [i] * ((p.T - 1) * p.n * 2 + 2 * p.n)
tt = np.array(tt)
b = kennbild(xy, z, tr, tt, 480, 640)
nam = {}
for y in range(y0, y1, sw):
    z_ = ''
    for x in range(x0, x1, sw):
        i = b[y, x]
        if i < 0:
            z_ += ' .'
        else:
            nam.setdefault(i, chr(97 + len(nam)) if len(nam) < 26 else chr(65 + len(nam) - 26))
            z_ += ' ' + nam[i]
    print('%3d %s' % (y, z_))
print({v_: t[k].name for k, v_ in nam.items()})
