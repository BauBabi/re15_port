#!/usr/bin/env python3
"""Fit gegen Pruefrender: oberste Modellzeile je Spalte, Kanten je Zeile. Versatz = form_emd.boden_nachfuehren hebt die Figur um den tiefsten Punkt. python rf_verg.py STAND.json (FORM.EMD muss zum Stand gehoeren)."""
import json, os, sys
import numpy as np, cv2
import torch
from rf_kern import *
import rf_plan
from rf_fit import Passung
import form_pruefung as P
g, t = rf_plan.plan()
s = json.load(open(sys.argv[1]))
pa = Passung(g, t, s)
k = P.Kandidat('form.json')
sa, alb, m, _ = k.rendern(1)
v, L, C = pa.netz.ecken(pa.lr, pa.c)
R, T = pa.kette_pose(1)
w = pa.netz.welt(v, R, T)
xy = pa.kam[1](w).detach().cpu().numpy()
z = pa.kam[1].tiefe(w).detach().cpu().numpy()
b = kennbild(xy, z, pa.netz.tris.cpu().numpy(), pa.netz.tkn.cpu().numpy(), 480, 640)
for x in (300, 305, 311, 316, 322, 330, 340, 350, 400):
    a = np.where(m[:, x])[0]
    c = np.where(b[:, x] >= 0)[0]
    print(x, 'Render oben', a.min() if len(a) else None, 'Fit oben', c.min() if len(c) else None)
for y in (260, 300, 340):
    a = np.where(m[y])[0]
    c = np.where(b[y] >= 0)[0]
    print('y', y, 'Render', a.min(), a.max(), 'Fit', c.min(), c.max())
print('Kandidat stand', k.stand(1))
print('stand', s['posen']['1']['lage'], s['posen']['1']['gier'], s['posen']['1']['wurzel'])
print('nach Passung', pa.pose[1]['lage0'], pa.pose[1]['gier0'], pa.pose[1]['wz0'])
