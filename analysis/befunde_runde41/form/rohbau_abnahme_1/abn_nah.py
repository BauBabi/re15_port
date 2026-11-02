#!/usr/bin/env python3
"""Abnahme 1: grosse Tonbilder aus FORM.EMD, Tiefenpuffer, je Knochen eine Grenzlinie. Kein RE-Wert."""
import os, sys, math
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose, blick, raster, ton, schreib

AUS = os.path.join(HIER, 'netz')
ge = 640
k = P.Kandidat(K)
n = netz(k)


def blatt(name, clip, ansichten, zoom=1.0, ziel=None, nur=None):
    V, F, B = pose(k, n, clip)
    if nur is not None:
        m = np.isin(B, nur)
        F, B = F[m], B[m]
    if ziel is None:
        idx = np.unique(F)
        mitte = (V[idx].max(0) + V[idx].min(0)) / 2
    else:
        kn, off = ziel
        idx = np.unique(F[np.isin(B, kn)])
        mitte = V[idx].mean(0) + np.array(off, float)
    s0 = ge * .46 / np.linalg.norm(V[np.unique(F)] - mitte, axis=1).max()
    z = []
    for az, el in ansichten:
        alle, vorn, zb, nb = raster(V, F, blick(az, el), ge, s0 * zoom, mitte)
        z.append(schreib(ton(nb, vorn), '%s az %d el %d' % (name, az, el)))
    zeilen = [np.hstack(z[i:i + 3]) for i in range(0, len(z), 3)]
    w = max(x.shape[1] for x in zeilen)
    zeilen = [cv2.copyMakeBorder(x, 0, 0, 0, w - x.shape[1], cv2.BORDER_CONSTANT, value=(40, 40, 40)) for x in zeilen]
    cv2.imwrite(os.path.join(AUS, 'nah_%s.png' % name), np.vstack(zeilen))


blatt('pose2_gesamt', 1, [(0, 10), (180, 10), (90, 10), (270, 10), (45, 25), (225, 25)], 1.25)
blatt('pose2_ruecken', 1, [(0, 5), (180, 5), (270, 30), (0, 40), (180, 40), (315, 15)], 2.2, ziel=((10, 7), (0, -300, 0)))
blatt('pose1_gesamt', 0, [(0, 10), (180, 10), (90, 10), (270, 10), (45, 25), (225, 25)], 1.25)
blatt('ruhe_gesamt', 2, [(0, 10), (180, 10), (90, 10), (270, 10), (45, 25), (225, 25)], 1.25)
blatt('kopf_ruhe', 2, [(90, 0), (90, 25), (60, 10), (0, 0), (180, 0), (120, 10)], 3.0, ziel=((22, 23), (0, 0, 0)))
blatt('hand_r_pose2', 1, [(0, 20), (90, 20), (180, 20), (270, 20), (45, 50), (225, 50)], 3.0, ziel=((18, 19, 20, 29, 31), (0, 0, 0)))
blatt('hand_l_pose2', 1, [(0, 20), (90, 20), (180, 20), (270, 20), (45, 50), (225, 50)], 3.5, ziel=((13, 14, 15, 28, 30), (0, 0, 0)))
blatt('schulter_pose1', 0, [(270, 0), (270, 20), (300, 10), (240, 10), (0, 0), (180, 0)], 2.4, ziel=((10, 16), (0, 0, 0)))
print('fertig')
