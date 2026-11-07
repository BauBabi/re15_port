#!/usr/bin/env python3
"""Kopfdrehung Pose 2: sichtbarer Oberarm gegen Lage der Wangenspitze (Ziel P 366,304) und Bogen. Nur Messung. python abn2/n_spalt2.py STAND"""
import json
import math
import os
import sys

import numpy as np
import torch

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import HIER, kennbild
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
    w_ = xy[pa.kappe['wange_l']]
    return int((b == ix['oberarm_r']).sum()), (round(float(w_[0])), round(float(w_[1]))), round(math.hypot(w_[0] - 366, w_[1] - 304), 1)


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


print('Basis', zaehle(w0))
for ax, tx in (((0, 1, 0), 'y'), ((0, 0, 1), 'z')):
    for d in (1, 2, 3, 4):
        print('kopf um', tx, d, zaehle(dreh(w0, 'kopf', ax, d)))
for dy, dz in ((1, 2), (2, 2), (1, 3), (2, 1)):
    print('kopf y', dy, 'z', dz, zaehle(dreh(dreh(w0, 'kopf', (0, 1, 0), dy), 'kopf', (0, 0, 1), dz)))
for d in (-3, -6):
    print('kiefer z', d, zaehle(dreh(w0, 'kiefer', (0, 0, 1), d)))
