#!/usr/bin/env python3
"""Fingerwurzeln und -spitzen in beiden Bildern, Hoehe, Gelenkwinkel. python rf_spitzen.py STAND.json"""
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
nm = ['klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'kralle_r', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4', 'kralle_l']
ix = pa.ix
for n in (1, 2):
    R, T = pa.kette_pose(n)
    w = pa.netz.welt(v, R, T)
    xy = pa.kam[n](w).detach().cpu().numpy()
    zz = pa.kam[n].tiefe(w).detach().cpu().numpy()
    wn = w.detach().cpu().numpy()
    print('Bild', n)
    for b in ('hand_r', 'hand_l'):
        i = ix[b]
        q = pa.kam[n](T[i:i + 1]).detach().cpu().numpy()[0]
        print('  Gelenk %-9s (%3d,%3d) H %5d' % (b, q[0], q[1], -T[i, 1]), 'Winkel', np.round(pa.winkel(n)[i].detach().cpu().numpy() * 360 / 4096))
    for m in nm:
        i = next(i for i, p in enumerate(t) if p.name == m)
        p = t[i]
        a = pa.netz.offs[i]
        w0 = a
        k1 = a + p.T * p.n + 1
        print('  %-9s Wurzelring (%3d,%3d)  Spitze (%3d,%3d) H %5d Tiefe %d  Winkel %s' % (m, xy[w0:w0 + p.n, 0].mean(), xy[w0:w0 + p.n, 1].mean(), xy[k1, 0], xy[k1, 1], -wn[k1, 1], zz[k1],
                                                                                 np.round(pa.winkel(n)[p.knochen].detach().cpu().numpy() * 360 / 4096)))
