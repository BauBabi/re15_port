#!/usr/bin/env python3
"""Rohbau final, Sicht mit Tiefenpuffer, direkt aus Stand und Plan, ohne EMD. Netz- und Rasterweg wie Abnahme 1 (abn_netz.py).
python rf_nah.py STAND.json --aus X.png --pose 2 [--blick 0,10;180,10;...] [--ziel brust,wirbel] [--zoom 1.2] [--gross 560]"""
import argparse
import json
import math
import os

import cv2
import numpy as np

from rf_kern import HIER, namen
from rf_blick import netz_von, pose_welt
import rf_plan


def blick(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    return np.stack([rechts, unten, vor])


def raster(V, F, A, gross, s, mitte):
    q = (V - mitte) @ A.T
    px = np.stack([q[:, 0] * s + gross / 2, q[:, 1] * s + gross / 2], 1)
    z = q[:, 2]
    tri = px[F]
    tz = z[F]
    d = (tri[:, 1, 0] - tri[:, 0, 0]) * (tri[:, 2, 1] - tri[:, 0, 1]) - (tri[:, 2, 0] - tri[:, 0, 0]) * (tri[:, 1, 1] - tri[:, 0, 1])
    n3 = np.cross(q[F[:, 1]] - q[F[:, 0]], q[F[:, 2]] - q[F[:, 0]])
    n3 /= np.linalg.norm(n3, axis=1, keepdims=True) + 1e-9
    zb = np.full((gross, gross), 1e18)
    nb = np.zeros((gross, gross, 3))
    idb = np.full((gross, gross), -1, int)
    for i in range(len(F)):
        if d[i] == 0:
            continue
        t = tri[i]
        x0 = max(int(math.floor(t[:, 0].min())), 0); x1 = min(int(math.ceil(t[:, 0].max())) + 1, gross)
        y0 = max(int(math.floor(t[:, 1].min())), 0); y1 = min(int(math.ceil(t[:, 1].max())) + 1, gross)
        if x0 >= x1 or y0 >= y1:
            continue
        X, Y = np.meshgrid(np.arange(x0, x1) + .5, np.arange(y0, y1) + .5)
        w1 = ((X - t[0, 0]) * (t[2, 1] - t[0, 1]) - (t[2, 0] - t[0, 0]) * (Y - t[0, 1])) / d[i]
        w2 = ((t[1, 0] - t[0, 0]) * (Y - t[0, 1]) - (X - t[0, 0]) * (t[1, 1] - t[0, 1])) / d[i]
        w0 = 1 - w1 - w2
        inn = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        if not inn.any():
            continue
        zz = w0 * tz[i, 0] + w1 * tz[i, 1] + w2 * tz[i, 2]
        neu = inn & (zz < zb[y0:y1, x0:x1])
        zb[y0:y1, x0:x1][neu] = zz[neu]
        nb[y0:y1, x0:x1][neu] = n3[i]
        idb[y0:y1, x0:x1][neu] = i
    return zb, nb, idb


def bild(nb, idb, tk):
    L = np.array([-.45, -.6, -.66])
    L = L / np.linalg.norm(L)
    h = np.clip(np.abs(nb @ L), 0, 1)
    b = (45 + 195 * h ** .9)
    b[idb < 0] = 40
    a = np.repeat(b[..., None], 3, 2).astype(np.uint8)
    kn = np.where(idb >= 0, tk[np.clip(idb, 0, None)], -1)
    r = np.zeros(kn.shape, bool)
    r[1:] |= kn[1:] != kn[:-1]
    r[:, 1:] |= kn[:, 1:] != kn[:, :-1]
    a[r] = (20, 150, 230)
    return a


def welt_von(g, T, stand, pn, nz, v):
    if pn.startswith('ruhe'):
        p = json.load(open(os.path.join(HIER, 'zw', 'ruhe.json' if pn == 'ruhe' else pn + '.json')))
        wz = p['wurzel']
    else:
        p = stand['posen'][pn]
        wz = list(p['wurzel'])
        wz[1] += p['lage'][1]
    return pose_welt(g, nz, v, p['winkel'], wz).detach().cpu().numpy()


def main():
    a = argparse.ArgumentParser()
    a.add_argument('stand')
    a.add_argument('--aus', default='zw/nah.png')
    a.add_argument('--pose', default='2')
    a.add_argument('--blick', default='0,10;180,10;90,10;270,10;45,25;225,25')
    a.add_argument('--ziel', default='')
    a.add_argument('--zoom', type=float, default=1.2)
    a.add_argument('--gross', type=int, default=560)
    a.add_argument('--spalten', type=int, default=3)
    a = a.parse_args()
    g, T = rf_plan.plan()
    stand = json.load(open(os.path.join(HIER, a.stand)))
    nz, lr, c = netz_von(T, stand)
    v, _, _ = nz.ecken(lr, c)
    tris = nz.tris.cpu().numpy()
    tk = nz.tkn.cpu().numpy()
    z = []
    for pn in a.pose.split(','):
        q = welt_von(g, T, stand, pn, nz, v)
        if a.ziel:
            ix = namen(g)
            kn = [ix[x] for x in a.ziel.split(',')]
            idx = np.unique(tris[np.isin(tk, kn)])
            mitte = q[idx].mean(0)
        else:
            mitte = (q.max(0) + q.min(0)) / 2
        s0 = a.gross * .46 / np.linalg.norm(q - mitte, axis=1).max()
        for t in a.blick.split(';'):
            az, el = [float(x) for x in t.split(',')]
            zb, nb, idb = raster(q, tris, blick(az, el), a.gross, s0 * a.zoom, mitte)
            b = bild(nb, idb, tk)
            cv2.putText(b, 'pose %s az %g el %g' % (pn, az, el), (6, 16), cv2.FONT_HERSHEY_SIMPLEX, .42, (120, 255, 255), 1, cv2.LINE_AA)
            z.append(b)
    n = a.spalten
    while len(z) % n:
        z.append(np.full_like(z[0], 40))
    cv2.imwrite(os.path.join(HIER, a.aus), np.vstack([np.hstack(z[i:i + n]) for i in range(0, len(z), n)]))


if __name__ == '__main__':
    main()
