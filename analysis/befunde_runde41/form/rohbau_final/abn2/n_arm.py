#!/usr/bin/env python3
"""Welche Ecken des rechten Oberarms stecken in Pose 2 in Kopf, Kiefer, Hals. python abn2/n_arm.py STAND"""
import json
import os
import sys

import numpy as np
import torch

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import HIER, innen_tiefe
import rf_plan
from rf_fit import Passung

g, t = rf_plan.plan()
s = json.load(open(os.path.join(HIER, sys.argv[1])))
pa = Passung(g, t, s)
v, L, C = pa.netz.ecken(pa.lr, pa.c)
for n in (2,):
    R, T = pa.kette_pose(n)
    w = pa.netz.welt(v, R, T)
    i = next(i for i, p in enumerate(t) if p.name == 'oberarm_r')
    p = t[i]
    a = pa.netz.offs[i]
    sel = torch.arange(a, a + p.T * p.n, device=pa.dev)
    q = w[sel]
    for nm in ('kopf', 'kiefer', 'hals', 'brust'):
        j = next(k for k, x in enumerate(t) if x.name == nm)
        d = innen_tiefe(q, t[j], pa.netz.pl[j], L[j], C[j], R, T).detach().cpu().numpy().reshape(p.T, p.n)
        print(nm, 'Tiefe je Ring/Ecke (>0 drin)')
        print(np.round(np.maximum(d, -99)).astype(int))
    xy = pa.kam[n](q).detach().cpu().numpy().reshape(p.T, p.n, 2)
    z = pa.kam[n].tiefe(q).detach().cpu().numpy().reshape(p.T, p.n)
    print('Bildlage Ring 2:', np.round(xy[2]).astype(int).tolist(), 'Tiefe', np.round(z[2]).astype(int).tolist())
    print('Bildlage Ring 4:', np.round(xy[4]).astype(int).tolist(), 'Tiefe', np.round(z[4]).astype(int).tolist())
    for nm in ('hals', 'kopf', 'kiefer', 'oberarm_r', 'unterarm_r', 'hand_r'):
        b = pa.ix[nm]
        print(nm, 'Bild', np.round(pa.kam[n](T[b:b + 1])[0].detach().cpu().numpy()), 'Tiefe', round(float(pa.kam[n].tiefe(T[b:b + 1])[0])), 'Hoehe', round(-float(T[b, 1])))
