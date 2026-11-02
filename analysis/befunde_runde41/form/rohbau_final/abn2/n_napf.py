#!/usr/bin/env python3
"""Was steht in Ruhe an der Stelle des Napfs (Blick von hinten, Hoehe rund 1270, z rund -690)? Tiefenpuffer von hinten mit Teilekennung, Faechermitten der Beinteile in Welt. python abn2/n_napf.py STAND"""
import json
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import HIER
import rf_plan
import rf_bau
from rf_blick import netz_von, pose_welt
from rf_nah import blick, raster

g, T = rf_plan.plan()
st = json.load(open(os.path.join(HIER, sys.argv[1])))
nz, lr, c = netz_von(T, st)
v, _, _ = nz.ecken(lr, c)
p = json.load(open(os.path.join(HIER, 'zw', 'ruhe.json')))
q = pose_welt(g, nz, v, p['winkel'], p['wurzel']).detach().cpu().numpy()
tris = nz.tris.cpu().numpy()
tt = []
for i, t in enumerate(T):
    tt += [i] * ((t.T - 1) * t.n * 2 + 2 * t.n)
tt = np.array(tt)
A = blick(-90, 12)       # rf_nah: az wie im Ton-Skript verschoben, hier nur Seite von hinten, unten geprueft
for nm in ('oberschenkel_r', 'unterschenkel_r', 'fuss_r', 'huefte'):
    i = next(i for i, t in enumerate(T) if t.name == nm)
    t = T[i]
    a = nz.offs[i]
    print(nm)
    for r in range(t.T):
        w = q[a + r * t.n:a + (r + 1) * t.n]
        m = w.mean(0)
        print('  Ring %d t %5.0f Mitte x %6.0f Hoehe %6.0f z %6.0f  Hoehenspanne %5.0f..%5.0f' % (r, t.t[r], m[0], -m[1], m[2], (-w[:, 1]).min(), (-w[:, 1]).max()))
    k0, k1 = q[a + t.T * t.n], q[a + t.T * t.n + 1]
    print('  Kappe0 x %6.0f Hoehe %6.0f z %6.0f | Kappe1 x %6.0f Hoehe %6.0f z %6.0f' % (k0[0], -k0[1], k0[2], k1[0], -k1[1], k1[2]))
# Tiefenpuffer von hinten: Blick entlang +x, Bild rechts = -z. Wer steht bei Hoehe 1270, z -690 vorn?
gross, s = 700, .1
mitte = np.array([0, -1000, -600.0])
V = np.array([[0, 0, -1.0], [0, 1.0, 0], [1.0, 0, 0]])
zb, nb, idb = raster(q, tris, V, gross, s, mitte)
ks = {}
for h in range(1000, 1460, 40):
    z_ = ''
    for zz in range(-1000, -360, 40):
        px = int((-(zz - mitte[2])) * s + gross / 2)
        py = int((-h - mitte[1]) * s + gross / 2)
        i = idb[py, px]
        nm = '.' if i < 0 else T[tt[i]].name
        if nm not in ks:
            ks[nm] = chr(97 + len(ks))
        z_ += ' ' + ks[nm]
    print('%5d %s' % (h, z_))
print(ks)
