#!/usr/bin/env python3
"""Spalt am Kiefer Bild 2: sichtbare Pixel des rechten Oberarms je Kopf-/Halsdrehung. Nur Messung, keine Datei wird geaendert. python abn2/n_spalt.py STAND"""
import json
import math
import os
import sys

import numpy as np
import torch

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import E, HIER, kennbild
import rf_plan
from rf_fit import Passung
from rf_a1 import euler_mat, mat_euler, kette_R

g, t = rf_plan.plan()
s = json.load(open(os.path.join(HIER, sys.argv[1])))
pa = Passung(g, t, s)
v, L, C = pa.netz.ecken(pa.lr, pa.c)
tr = pa.netz.tris.cpu().numpy()
tk = pa.netz.tkn.cpu().numpy()
ix = pa.ix
w0 = np.array(pa.pose[2]['w0'].cpu().numpy(), float)


def zaehle(w):
    pa.pose[2]['w0'][:] = torch.tensor(w, dtype=torch.float32, device=pa.dev)
    R, T = pa.kette_pose(2)
    q = pa.netz.welt(v, R, T)
    xy = pa.kam[2](q).detach().cpu().numpy()
    z = pa.kam[2].tiefe(q).detach().cpu().numpy()
    b = kennbild(xy, z, tr, tk, 480, 640)
    aus = {k: int((b == ix[k]).sum()) for k in ('oberarm_r', 'kopf', 'kiefer')}
    k = ((b >= 0)).astype(np.uint8)
    return aus, b


def dreh(w, name, achse, grad):
    w = w.copy()
    i = ix[name]
    e = g['knochen'][i]['eltern']
    R = kette_R(g, w.tolist())
    tt = math.radians(grad)
    c, sn = math.cos(tt), math.sin(tt)
    a = np.array(achse, float)
    a /= np.linalg.norm(a)
    K = np.array([[0, -a[2], a[1]], [a[2], 0, -a[0]], [-a[1], a[0], 0]])
    D = np.eye(3) * c + sn * K + (1 - c) * np.outer(a, a)
    M = R[e].T @ D @ R[e] @ euler_mat(w[i])
    w[i] = mat_euler(M)
    return w


print('Basis', zaehle(w0)[0])
for nm in ('hals', 'kopf'):
    for ax, tx in (((0, 1, 0), 'um y'), ((0, 0, 1), 'um z'), ((1, 0, 0), 'um x')):
        for d in (-6, -3, 3, 6):
            print(nm, tx, d, zaehle(dreh(w0, nm, ax, d))[0])
