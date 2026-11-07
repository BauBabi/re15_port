#!/usr/bin/env python3
"""Rohbau final, Sicht: Tonbild aus Teilen und Pose (ohne EMD). python rf_blick.py [STAND.json] --aus X.png [--posen bind,1,2,ruhe]"""
import argparse
import json
import math
import os

import cv2
import numpy as np
import torch

from rf_kern import HIER, GERAET, Kette, Netz
import rf_plan


def rot(az, el):
    a, e = math.radians(az), math.radians(el)
    Ry = np.array([[math.cos(a), 0, math.sin(a)], [0, 1, 0], [-math.sin(a), 0, math.cos(a)]])
    Rx = np.array([[1, 0, 0], [0, math.cos(e), -math.sin(e)], [0, math.sin(e), math.cos(e)]])
    return Rx @ Ry


def ton(v, tris, az, el, gross=520, s=None, mitte=None):
    M = rot(az, el)
    q = v @ M.T
    lo, hi = q.min(0), q.max(0)
    if mitte is None:
        mitte = (lo + hi) / 2
    if s is None:
        s = (gross - 30) / max(hi[0] - lo[0], hi[1] - lo[1])
    x = (q[:, 0] - mitte[0]) * s + gross / 2
    y = (q[:, 1] - mitte[1]) * s + gross / 2
    z = q[:, 2]
    a, b, c = q[tris[:, 0]], q[tris[:, 1]], q[tris[:, 2]]
    n = np.cross(b - a, c - a)
    n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-9
    bild = np.full((gross, gross, 3), 58, np.uint8)
    zm = z[tris].mean(1)
    # Netz-Windung nach aussen in Knochenraum (Y unten): vom Betrachter (-z) sichtbar sind Flaechen mit n.z * vz > 0, Vorzeichen aus dem Rand bestimmt
    L = np.array([.35, -.65, -.67])
    L /= np.linalg.norm(L)
    pts = np.stack([x, y], 1)
    vz = -1.0 if (n[:, 2] * (zm - zm.mean())).sum() < 0 else 1.0     # vorne liegende Flaechen zeigen zum Betrachter
    n = n * vz
    folge = np.argsort(-zm)
    for i in folge:
        if n[i, 2] > 0.02:
            continue
        h = max(0.0, float(n[i] @ L)) * .75 + .22
        c_ = int(min(255, 255 * h))
        cv2.fillConvexPoly(bild, np.round(pts[tris[i]] * 8).astype(np.int32), (c_, c_, c_), lineType=cv2.LINE_AA, shift=3)
    return bild


def netz_von(T, stand):
    nz = Netz(T)
    lr, c = nz.startwerte()
    if stand:
        for i in nz.meister:
            if str(i) in stand['lr']:
                x = np.array(stand['lr'][str(i)])
                if x.shape == tuple(lr[i].shape):
                    lr[i] = torch.tensor(x, dtype=torch.float32, device=GERAET)
                    c[i] = torch.tensor(stand['c'][str(i)], dtype=torch.float32, device=GERAET)
    return nz, lr, c


def pose_welt(g, nz, v, winkel, wurzel):
    ke = Kette(g)
    vers = torch.tensor([k['versatz'] for k in g['knochen']], dtype=torch.float32, device=GERAET)
    w = torch.tensor(np.array(winkel, float), dtype=torch.float32, device=GERAET)
    wz = torch.tensor(np.array(wurzel, float), dtype=torch.float32, device=GERAET)
    R, T_ = ke(w, vers, wz, 0.0, torch.zeros(3, device=GERAET))
    return nz.welt(v, R, T_)


def main():
    a = argparse.ArgumentParser()
    a.add_argument('stand', nargs='?')
    a.add_argument('--aus', default='zw/blick.png')
    a.add_argument('--posen', default='bind')
    a.add_argument('--nur', default='')
    a.add_argument('--gross', type=int, default=420)
    a = a.parse_args()
    g, T = rf_plan.plan()
    print('Teile', len(T), 'Flaechen', sum(p.flaechen() for p in T))
    gr = {}
    for p in T:
        k = p.name.split('_')[0]
        gr[k] = gr.get(k, 0) + p.flaechen()
    print(gr)
    stand = json.load(open(os.path.join(HIER, a.stand))) if a.stand else None
    nz, lr, c = netz_von(T, stand)
    v, _, _ = nz.ecken(lr, c)
    tris = nz.tris.cpu().numpy()
    if a.nur:
        from rf_kern import namen
        ix = namen(g)
        kn = [ix[x] for x in a.nur.split(',')]
        tris = tris[np.isin(nz.tkn.cpu().numpy(), kn)]
    zeilen = []
    for pn in a.posen.split(','):
        if pn == 'bind':
            w = np.zeros((len(g['knochen']), 3))
            wz = [0, -1500, 0]
        else:
            if pn == 'ruhe':
                p = json.load(open(os.path.join(HIER, 'zw', 'ruhe.json')))
            else:
                p = stand['posen'][pn] if stand and 'posen' in stand and pn in stand['posen'] else g['posen']['bild' + pn]
            w, wz = p['winkel'], p['wurzel']
        q = pose_welt(g, nz, v, w, wz).detach().cpu().numpy()
        sp = []
        sel = np.unique(tris)
        qs = q.copy()
        qs[np.setdiff1d(np.arange(len(q)), sel)] = q[sel].mean(0)
        for az, el in ((90, 0), (-90, 0), (0, 0), (180, 0), (40, 25), (-140, 25), (140, 20)):
            sp.append(ton(qs, tris, az, el, a.gross))
        zeilen.append(np.hstack(sp))
    cv2.imwrite(os.path.join(HIER, a.aus), np.vstack(zeilen))


if __name__ == '__main__':
    main()
