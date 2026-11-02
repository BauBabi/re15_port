#!/usr/bin/env python3
"""Rohbau final, Lagebild: Kennbild je Knochen auf dem Original mit Nutzer-Umrissen. python rf_auflage.py STAND.json --aus zw/x.png"""
import argparse
import json
import os

import cv2
import numpy as np
import torch

from rf_kern import E, GERAET, HIER, TEILE, Kamera, kennbild, masken
import rf_plan
from rf_fit import Passung

FARBE = {'huefte': (120, 80, 200), 'wirbel': (150, 110, 230), 'brust': (190, 150, 250), 'hals': (90, 200, 200), 'kopf': (60, 255, 255), 'kiefer': (40, 180, 255), 'oberarm_l': (200, 255, 200), 'unterarm_l': (60, 220, 60),
         'hand_l': (30, 170, 30), 'oberarm_r': (255, 120, 255), 'unterarm_r': (255, 150, 60), 'hand_r': (220, 90, 20), 'oberschenkel_l': (0, 160, 255), 'unterschenkel_l': (0, 120, 220), 'fuss_l': (0, 80, 180),
         'oberschenkel_r': (230, 230, 0), 'unterschenkel_r': (190, 190, 0), 'fuss_r': (150, 150, 0)}


def farbe(nm):
    if nm in FARBE:
        return FARBE[nm]
    if nm.startswith('schwanz'):
        i = int(nm[-1])
        return (60, 200 - 12 * i, 80 + 20 * i)
    if nm.startswith('klaue_r'):
        return (250, 60 + 30 * int(nm[-1]), 0)
    if nm.startswith('klaue_l'):
        return (0, 120 + 30 * int(nm[-1]), 0)
    return (128, 128, 128)


def blatt(pa, aus, s=2):
    import form_pruefung as P
    m, logo = masken()
    v, _, _ = pa.netz.ecken(pa.lr, pa.c)
    nm = [k['name'] for k in pa.g['knochen']]
    bl = []
    for n in (1, 2):
        R, T = pa.kette_pose(n)
        w = pa.netz.welt(v, R, T)
        xy = pa.kam[n](w).detach().cpu().numpy()
        z = pa.kam[n].tiefe(w).detach().cpu().numpy()
        bild = kennbild(xy, z, pa.netz.tris.cpu().numpy(), pa.netz.tkn.cpu().numpy(), 480 * s, 640 * s, 1.0 / s)
        o = P.original(n)[:, :, ::-1]
        o = cv2.resize(o, None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
        c = (o.astype(np.float32) * .55 + 40).clip(0, 255).astype(np.uint8)
        r = np.full(c.shape, 30, np.uint8)
        for i, nn in enumerate(nm):
            q = bild == i
            if q.any():
                r[q] = farbe(nn)
        kant = np.zeros(bild.shape, np.uint8)
        for i in range(len(nm)):
            q = (bild == i).astype(np.uint8)
            kant |= cv2.morphologyEx(q, cv2.MORPH_GRADIENT, np.ones((3, 3), np.uint8))
        r[kant > 0] = (r[kant > 0] * .6).astype(np.uint8)
        for t in TEILE[n]:
            ct, _ = cv2.findContours(cv2.resize(m[t].astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(c, ct, -1, (255, 255, 255), 1)
            cv2.drawContours(r, ct, -1, (255, 255, 255), 1)
        x0, y0, x1, y1 = pa.box[n]
        x0, y0, x1, y1 = max(x0 - 30, 0) * s, max(y0 - 30, 0) * s, min(x1 + 30, 640) * s, y1 * s
        bl.append(np.hstack([c[y0:y1, x0:x1], r[y0:y1, x0:x1]]))
    w_ = max(b.shape[1] for b in bl)
    bl = [np.pad(b, ((0, 0), (0, w_ - b.shape[1]), (0, 0))) for b in bl]
    cv2.imwrite(aus, np.vstack(bl))


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('stand')
    a.add_argument('--aus', default='zw/auflage.png')
    a = a.parse_args()
    g, t = rf_plan.plan()
    pa = Passung(g, t, json.load(open(os.path.join(HIER, a.stand))))
    blatt(pa, os.path.join(HIER, a.aus))
