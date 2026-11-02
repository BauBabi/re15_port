#!/usr/bin/env python3
"""Rohbau final: Gelenktafel einer Pose. python rf_gelenke.py STAND.json"""
import json, os, sys
import numpy as np
import torch
from rf_kern import HIER
import rf_plan
from rf_fit import Passung
g, t = rf_plan.plan()
pa = Passung(g, t, json.load(open(os.path.join(HIER, sys.argv[1]))) if len(sys.argv) > 1 else None)
for n in (1, 2):
    R, T = pa.kette_pose(n)
    q = pa.kam[n](T).detach().cpu().numpy()
    z = pa.kam[n].tiefe(T).detach().cpu().numpy()
    p = pa.pose[n]
    Y = R[0].detach().cpu().numpy()
    a = (p['gier0'] + float(p['dg'][0])) * 2 * np.pi / 4096
    Yg = np.array([[np.cos(a), 0, np.sin(a)], [0, 1, 0], [-np.sin(a), 0, np.cos(a)]])
    lage = (p['lage0'] + p['dl']).detach().cpu().numpy()
    print('Bild', n, 'Lage', np.round(lage), 'Gier', round(p['gier0'] + float(p['dg'][0])), 'Wurzel', np.round((p['wz0'] + p['dwz']).detach().cpu().numpy()))
    w = pa.winkel(n).detach().cpu().numpy() * 360 / 4096
    for k in g['knochen']:
        i = k['index']
        f = Yg.T @ (T[i].detach().cpu().numpy() - lage)
        print('  %2d %-16s Bild (%4.0f,%4.0f) H %5.0f Tiefe %5.0f  Figur (%6.0f,%6.0f)  Winkel %s' % (i, k['name'], q[i, 0], q[i, 1], -T[i, 1], z[i], f[0], f[2], np.round(w[i]).astype(int).tolist()))
