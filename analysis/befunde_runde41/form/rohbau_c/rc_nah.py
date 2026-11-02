#!/usr/bin/env python3
"""Rohbau C, Nahblick auf einen Knochen: python rc_nah.py form.json aus.png clip knochen[,knochen] [gross]."""
import sys

import cv2
import numpy as np

from rc_kern import TOOLS  # noqa: F401
import form_pruefung as P
import form_buehne as FB
import form_emd as FE


def blatt(j, aus, clip, ks, gross=2200):
    k = P.Kandidat(j)
    weiss = FB.Textur(k.tim)
    weiss.lut = weiss.lut.copy()
    weiss.lut[..., :3] = 200
    kf = k.kf(clip, 0)
    zeilen = []
    z = []
    for t, az, el in (('rechts', 0, 6), ('vorn', 90, 6), ('3/4 rechts', 40, 16), ('3/4 links', 140, 16), ('unten vorn', 70, -35), ('oben', 60, 55)):
        tr = P.rundum_dreiecke(k.mo, kf, az, el, gross)
        tk = FB.dreiecke  # noqa: F841
        R, T = k.mo.pose(kf)
        a = np.full((gross, gross, 3), 60, np.uint8)
        FB.rastern(tr, weiss, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
        # Ausschnitt: Bildlage der Knochen ueber dieselbe Abbildung wie rundum_dreiecke
        import math
        A0 = math.radians(az)
        E0 = math.radians(el)
        vor = np.array([-math.sin(A0) * math.cos(E0), math.sin(E0), math.cos(A0) * math.cos(E0)])
        rechts = np.array([math.cos(A0), 0.0, math.sin(A0)])
        unten = np.cross(vor, rechts)
        if unten[1] < 0 and abs(el) < 80:
            unten = -unten
        M = np.stack([rechts, unten, vor])
        alle = P.pose_punkte(k.mo, kf)
        mitte = (alle.max(0) + alle.min(0)) / 2
        rad = float(np.linalg.norm(alle - mitte, axis=1).max())
        s = gross * .46 / rad
        pk = []
        for b in ks:
            v = np.array([x[:3] for x in k.mo.meshes[b].qv], float) @ (R[b].astype(float).T / 4096.0) + T[b].astype(float)
            pk.append(v)
        pk = np.vstack(pk)
        q = (pk - mitte) @ M.T
        sx, sy = gross / 2 + q[:, 0] * s, gross / 2 + q[:, 1] * s
        x0, x1, y0, y1 = int(sx.min()) - 30, int(sx.max()) + 30, int(sy.min()) - 30, int(sy.max()) + 30
        c = a[max(y0, 0):y1, max(x0, 0):x1]
        h = 420
        w = int(c.shape[1] * h / c.shape[0])
        c = cv2.resize(c, (min(w, 640), h), interpolation=cv2.INTER_AREA)
        c = cv2.copyMakeBorder(c, 0, 0, 0, max(0, 640 - c.shape[1]), cv2.BORDER_CONSTANT, value=(60, 60, 60))
        cv2.putText(c, t, (8, 20), 0, .55, (255, 255, 255), 1, cv2.LINE_AA)
        z.append(c)
    zeilen = [np.hstack(z[:3]), np.hstack(z[3:])]
    cv2.imwrite(aus, np.vstack(zeilen)[:, :, ::-1])


if __name__ == '__main__':
    blatt(sys.argv[1], sys.argv[2], int(sys.argv[3]), [int(x) for x in sys.argv[4].split(',')])
