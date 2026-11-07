#!/usr/bin/env python3
"""Bildlage der Ringecken eines Teils. python rf_ring.py STAND.json teil,teil"""
import json, sys
import numpy as np
import torch
from rf_kern import *
import rf_plan
from rf_fit import Passung
g, t = rf_plan.plan()
s = json.load(open(sys.argv[1]))
pa = Passung(g, t, s)
v, L, C = pa.netz.ecken(pa.lr, pa.c)
names = sys.argv[2].split(',')
np.set_printoptions(linewidth=200, suppress=True, precision=0)
for n in (1, 2):
    R, T = pa.kette_pose(n)
    w = pa.netz.welt(v, R, T)
    xy = pa.kam[n](w).detach().cpu().numpy()
    w = w.detach().cpu().numpy()
    for nm in names:
        i = next(i for i, p in enumerate(t) if p.name == nm)
        p = t[i]
        a = pa.netz.offs[i]
        print('Bild', n, nm)
        for r in range(p.T):
            q = a + r * p.n
            print('  Ring', r, 't', p.t[r], 'xy', ' '.join('%d:(%d,%d)' % (j, xy[q + j, 0], xy[q + j, 1]) for j in range(0, p.n)), ' H j5', int(-w[q + p.n // 2, 1]))
