#!/usr/bin/env python3
"""A: Stege an der NL-Hand aus anderer Sicht. Neigung des Rumpfs je Pose. Rumpfrisse bei 10 Grad. Fremde Malerei als Bild."""
import math, os, sys
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
sys.path.insert(0, G.TOOLS)
import form_pruefung as P
import form_buehne as FB
import geo2 as G2
import geo3 as G3
import geo4 as G4
ZW = os.path.join(G.FORM, 'bewertung_mass_zw')


def nah(k, clip, sichten, box, ziel, gross_=1100, gamma=.55):
    K = G3.kand(k)
    kf = K.kf(clip, 0)
    z = []
    for az, el in sichten:
        a = np.full((gross_, gross_, 3), 92, np.uint8)
        m = FB.rastern(P.rundum_dreiecke(K.mo, kf, az, el, gross_), K.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
        a[m] = np.clip(np.power(a[m] / 255.0, gamma) * 255, 0, 255).astype(np.uint8)
        x0, y0, x1, y1 = box if box else (0, 0, gross_, gross_)
        z.append(a[y0:y1, x0:x1])
    cv2.imwrite(os.path.join(ZW, ziel), cv2.cvtColor(np.hstack(z), cv2.COLOR_RGB2BGR))


def neigung():
    for k in 'abc':
        f, _ = G.lade(k)
        nm = G.NAMEN[k]
        rumpf = 10
        z = []
        for ci in (0, 1, 2):
            L, R, T = G.lokale(f, ci)
            up = R[0] @ np.array([0, -1.0, 0])
            kipp0 = math.degrees(math.acos(np.clip(-up[1] * -1, -1, 1)))
            up10 = R[rumpf] @ np.array([0, -1.0, 0])
            kipp10 = math.degrees(math.acos(np.clip(up10 @ np.array([0, -1.0, 0]), -1, 1)))
            kipp0 = math.degrees(math.acos(np.clip(up @ np.array([0, -1.0, 0]), -1, 1)))
            fw = R[0] @ np.array([1.0, 0, 0])
            z.append(dict(clip=ci, becken_kipp=round(kipp0, 1), rumpf_kipp=round(kipp10, 1), becken_gier=round(math.degrees(math.atan2(-fw[2], fw[0])), 1)))
        print(k, z)


def rumpfriss(k, namen, w=10):
    f, _ = G.lade(k)
    nm = G.NAMEN[k]
    N = G4.Netz(f)
    R0, T0 = G4.pose_frei(f, 2)
    V, _, _ = N.alles(R0, T0)
    m = (V.max(0) + V.min(0)) / 2
    s = G4.GROSS * 0.46 / float(np.linalg.norm(V - m, axis=1).max())
    bl = None
    best = -1
    for n in namen:
        b = nm.index(n)
        for ax in ((1, 0, 0), (0, 0, 1)):
            for sg in (1, -1):
                R, T = G4.pose_frei(f, 2, {b: G4.rot(ax, sg * w)})
                g, z, je, q = G4.rundblick(N, R, T, s, m)
                if g > best:
                    best, bl, je_b = g, q, je
    G4.tafel(bl, os.path.join(ZW, 'riss_%s_%dgrad.png' % (k, w)))
    print(k, w, 'Grad: Lochpixel', best, 'je Richtung', je_b, 'groesste Einzelsicht in Einheiten^2: %.0f' % (max(je_b) / s / s))


if __name__ == '__main__':
    neigung()
    nah('a', 2, [(90, 0), (45, 22), (130, 10)], (0, 450, 1100, 1100), 'nah_a_stege.png')
    nah('c', 2, [(0, 0), (180, 0), (90, 0)], (0, 300, 1100, 1100), 'nah_c_beine.png')
    rumpfriss('b', ['rumpf_m', 'rumpf'])
    rumpfriss('c', ['rumpf_m', 'rumpf_v'])
    rumpfriss('a', ['rumpf_mitte', 'rumpf'])
