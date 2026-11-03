#!/usr/bin/env python3
"""Variante K: nur Bild 1 neu legen (Hals, Kopf, Kiefer) fuer ein fertiges kopf.json, mit den Grenzen aus k_kopf_fit (Kiefer 0..600). python k_bild1.py kopf.json"""
import json
import sys

import numpy as np
from scipy.optimize import minimize

import k_kern as KK
import k_form as KF
import k_kopf_fit as F

pf = sys.argv[1]
j = json.load(open(pf, encoding='utf-8'))
form, mo, fj = KK.lade()
b1 = F.Buehne(1, form, mo, fj)
p = dict(KF.VORGABE)
p.update(j['form'])
geo = F.geometrie(p, b1.hals, b1.hals_f)
w1 = {b: list(b1.pool[b]) for b in F.KOPF}


def zerl(x):
    return ([w1[21][i] + 200 * x[i] for i in range(3)], [w1[22][i] + 200 * x[3 + i] for i in range(3)],
            [w1[23][0], w1[23][1], min(max(w1[23][2] + 200 * x[6], 0.0), 600.0)])


def f1(x):
    _t, idb = b1.rendern(geo, *zerl(x))
    return -b1.mass(idb)['umriss'] + 0.002 * float(np.sum(np.asarray(x[:6]) ** 2))


best = None
for st in range(3):
    x0 = np.zeros(7) if st == 0 else np.random.RandomState(st).normal(0, 0.5, 7)
    r = minimize(f1, x0, method='Powell', options=dict(maxfev=500, xtol=1e-2, ftol=1e-5))
    if best is None or r.fun < best.fun:
        best = r
W = [[int(round(v)) for v in w] for w in zerl(best.x)]
_t, idb = b1.rendern(geo, *W)
m = b1.mass(idb)
j['bild1'] = dict(winkel={21: W[0], 22: W[1], 23: W[2]}, mass=m)
json.dump(j, open(pf, 'w', encoding='utf-8'), indent=1)
np.save(pf.replace('.json', '_id1.npy'), idb)
print(pf, 'Bild 1', m, W)
