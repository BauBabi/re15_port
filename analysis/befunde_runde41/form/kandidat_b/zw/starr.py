"""Starre Figur Pose 1 in Bild 2 zeigen: farbig je Gliedgruppe auf Bild 2. Aufruf: starr.py gier x z [fit]"""
import sys, json, math
sys.path.insert(0, '.')
import numpy as np, cv2, torch
from b_geom import *
import b_teile as T
import b_fit as F
import b_mesh as BM
from b_fit import GERAET, ablage_torch

fit = BM.lade(sys.argv[4] if len(sys.argv) > 4 else 'zw/fit2.pt')
g = float(sys.argv[1])
l = np.array([float(sys.argv[2]), 0, float(sys.argv[3])])
GR = {'Kuppel': ['rumpf_m'], 'FR': ['fr_ober', 'fr_unter', 'fr_fuss', 'fr_zeh'], 'FL': ['fl_ober', 'fl_unter', 'fl_fuss', 'fl_zeh'], 'NL': ['nl_ober', 'nl_unter', 'nl_faust', 'nl_innen'], 'NR': ['nr_ober', 'nr_unter', 'nr_faust', 'nr_innen']}
FA = {'Kuppel': (255, 255, 255), 'FR': (0, 255, 0), 'FL': (0, 255, 255), 'NL': (255, 80, 80), 'NR': (0, 120, 255)}
o = cv2.imread(FORM + '/bild2.png')
a = np.clip(o.astype(np.float32) * 1.2, 0, 255).astype(np.uint8)
xs, ys = np.meshgrid(np.arange(150, 500, 2.0), np.arange(120, 470, 2.0))
xy = np.stack([xs.ravel(), ys.ravel()], 1)
e, d = FK.strahl(K2, xy)
d = d / (d @ K2['R'][2])[:, None]
ts = np.linspace(3000, 9000, 70)
for nm, teile in GR.items():
    Fe = BM.Feld(fit, set(teile), 30.0)
    m = np.ones(len(xy)) * 1e9
    for t in ts:
        w = e + d * t
        p = (Y(g).T @ (w - l).T).T
        m = np.minimum(m, Fe(p.astype(np.float32)))
    q = (m.reshape(xs.shape) < 0).astype(np.uint8)
    q = cv2.resize(q, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    v = np.zeros((480, 640), np.uint8)
    h, b = min(q.shape[0], 480 - 120), min(q.shape[1], 640 - 150)
    v[120:120 + h, 150:150 + b] = q[:h, :b]
    k, _ = cv2.findContours(v, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, k, -1, FA[nm], 1)
z = cv2.imread(FORM + '/ziel_bild2.png', 0)
k, _ = cv2.findContours((z == 0).astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
cv2.drawContours(a, k, -1, (255, 0, 255), 1)
print(np.round(FK.projiziere(K2, Y(g) @ np.array(T.KUPPEL_MITTE) + l), 0))
cv2.imwrite('zw/starr_%d.png' % int(g), cv2.resize(a[120:470, 150:500], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))
