#!/usr/bin/env python3
"""Nahblatt: Ruhepose gross von vorn rechts, Kopf von vorn und schraeg, Hand. python nah.py"""
import os
import numpy as np, cv2
import bau, ton, entwurf, ruhe
from kn import knochen, lade_posen, fk, HIER, INDEX
import netz as NZ


def einheit(v):
    v = np.array(v, float)
    return v / np.linalg.norm(v)


def tafel(Q, F, N, blick, oben, W, H, mitte, sk, titel):
    ez = einheit(blick)
    u = -np.array(oben, float)
    ey = einheit(u - ez * (u @ ez))
    ex = np.cross(ey, ez)
    return ton.tafel(Q, F, N, ex, ey, ez, W, H, sk, mitte, titel)


def main():
    ks = knochen()
    N = NZ.netze(entwurf.alle(), bau.korr_laden())
    P = lade_posen()
    P['ruhe'] = ruhe.pose(ks, P)
    R, T, _a, _b = fk(ks, P['ruhe'])
    Q, F = ton.weltquads(N, R, T)
    pts = Q.reshape(-1, 3)
    mitte = 0.5 * (pts.min(0) + pts.max(0))
    W, H = 760, 560
    sk = 0.082
    o = (0, -1, 0)
    z = []
    z.append(tafel(Q, F, N, (-1, 0.25, -0.75), o, W, H, mitte, sk, 'Ruhe, vorn rechts'))
    z.append(tafel(Q, F, N, (1, 0.25, -0.75), o, W, H, mitte, sk, 'Ruhe, hinten rechts'))
    k = INDEX['kopf']
    kopf = T[k] + R[k] @ np.array([700.0, 120, 0])
    z2 = []
    z2.append(tafel(Q, F, N, R[k] @ np.array([-1.0, 0.1, 0.0]), o, W, H, kopf, 0.26, 'Kopf von vorn'))
    z2.append(tafel(Q, F, N, R[k] @ np.array([-0.7, 0.2, -0.7]), o, W, H, kopf, 0.26, 'Kopf schraeg rechts'))
    cv2.imwrite(os.path.join(HIER, 'zw', 'nah.png'), np.vstack([np.hstack(z), np.hstack(z2)]))


if __name__ == '__main__':
    main()
