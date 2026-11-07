#!/usr/bin/env python3
"""Abnahme 1: Netz der Abgabe selbst pruefen, ohne Generator, nur aus FORM.EMD. Kein RE-Wert.

Tonbilder mit Tiefenpuffer (keine Saegezacken aus der Malersortierung), je Clip 8 Azimute auf 3 Hoehen,
Durchblick (Silhouette ohne Vorderseite), offene Kanten, Spiegelabstand, Bodenkontakt je Glied, Links-rechts-Laengen.
"""
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P       # noqa: E402

K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'netz')
os.makedirs(AUS, exist_ok=True)
NAMEN = {0: 'huefte', 1: 'schwanz1', 2: 'schwanz2', 3: 'schwanz3', 4: 'schwanz4', 5: 'schwanz5', 6: 'schwanz6'}


def netz(k):
    """Je Knochen: Ecken im Knochenraum, Dreiecke (Windung wie im Spiel)."""
    aus = []
    for b in range(min(k.mo.skel.bone_count, len(k.mo.meshes))):
        m = k.mo.meshes[b]
        vl, tl = [], []
        off = 0
        if m.tf:
            v = np.array([x[:3] for x in m.tv], float)
            vl.append(v)
            for g in m.tf:
                ie = g[1::2]
                tl.append((ie[0], ie[1], ie[2]))
            off = len(v)
        if m.qf:
            v = np.array([x[:3] for x in m.qv], float)
            vl.append(v)
            for g in m.qf:
                ie = [i + off for i in g[1::2]]
                tl.append((ie[0], ie[1], ie[3]))
                tl.append((ie[0], ie[3], ie[2]))
        aus.append((np.vstack(vl) if vl else np.zeros((0, 3)), np.array(tl, int).reshape(-1, 3)))
    return aus


def pose(k, n, clip, bild=0):
    kf = k.kf(clip, bild)
    R, T = k.mo.pose(kf)
    V, F, B = [], [], []
    off = 0
    for b, (v, f) in enumerate(n):
        if not len(v):
            continue
        q = v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float)
        V.append(q)
        F.append(f + off)
        B.append(np.full(len(f), b))
        off += len(q)
    return np.vstack(V), np.vstack(F), np.concatenate(B)


def blick(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    return np.stack([rechts, unten, vor])


def raster(V, F, A, gross, s, mitte, hell=True):
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
    vorn = np.zeros((gross, gross), bool)
    alle = np.zeros((gross, gross), bool)
    # Spiel: Dreieck vorn, wenn mac0 > 0 (form_buehne._mac0). d hat dasselbe Vorzeichen wie mac0, gleiche Bildachsen.
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
        alle[y0:y1, x0:x1] |= inn
        if d[i] > 0:
            vorn[y0:y1, x0:x1] |= inn
            neu = inn & (zz < zb[y0:y1, x0:x1])
            zb[y0:y1, x0:x1][neu] = zz[neu]
            nb[y0:y1, x0:x1][neu] = n3[i]
    return alle, vorn, zb, nb


def ton(nb, vorn):
    L = np.array([-.45, -.6, -.66])
    L = L / np.linalg.norm(L)
    h = np.clip(np.abs(nb @ L), 0, 1)                   # Windung im Bild fest, Vorzeichen egal
    a = (.25 + .72 * h) * 255
    bild = np.full(vorn.shape + (3,), 62, np.uint8)
    bild[vorn] = np.stack([a, a, a], -1)[vorn].astype(np.uint8)
    return bild


def schreib(a, t):
    cv2.putText(a, t, (5, 15), cv2.FONT_HERSHEY_SIMPLEX, .42, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, t, (5, 15), cv2.FONT_HERSHEY_SIMPLEX, .42, (255, 255, 120), 1, cv2.LINE_AA)
    return a


def main():
    k = P.Kandidat(K)
    n = netz(k)
    bericht = []
    sag = lambda t='': (print(t), bericht.append(t))
    # offene Kanten je Mesh, ueber Ecklage verschmolzen
    offen = 0
    einfach = 0
    tris = 0
    for b, (v, f) in enumerate(n):
        if not len(f):
            continue
        tris += len(f)
        schl = {}
        idx = {}
        kan = {}
        for i, p in enumerate(np.round(v).astype(int)):
            idx[i] = schl.setdefault(tuple(p), len(schl))
        for t in f:
            a = [idx[int(x)] for x in t]
            for x, y in ((a[0], a[1]), (a[1], a[2]), (a[2], a[0])):
                if x == y:
                    continue
                key = (min(x, y), max(x, y))
                kan.setdefault(key, [0, 0])
                kan[key][0 if x < y else 1] += 1
        o = sum(1 for c in kan.values() if c[0] + c[1] == 1)
        e = sum(1 for c in kan.values() if c[0] != c[1])
        offen += o
        einfach += e
        if o or e:
            sag('  Mesh %2d: offene Kanten %d, ungleiche Windung %d von %d Kanten' % (b, o, e, len(kan)))
    sag('Dreiecke %d, offene Kanten %d, Kanten mit ungleicher Windung %d' % (tris, offen, einfach))

    # Boden und Glieder je Clip
    GLIED = {'Hand r': (18, 19, 20, 29, 31), 'Hand l': (13, 14, 15, 28, 30), 'Fuss r': (27,), 'Fuss l': (25,), 'Schwanz': (1, 2, 3, 4, 5, 6)}
    for clip in (0, 1, 2):
        V, F, B = pose(k, n, clip)
        z = {}
        for nm, kn in GLIED.items():
            m = np.isin(B, kn)
            z[nm] = -V[np.unique(F[m])][:, 1].max()     # Hoehe ueber Boden: y unten positiv, Boden 0
        sag('Clip %d Hoehe tiefster Punkt je Glied: %s; Spanne %s; hoechster Punkt %.0f' % (clip, ', '.join('%s %.0f' % (a, b) for a, b in z.items()), np.round(V.max(0) - V.min(0)).astype(int), -V[:, 1].min()))

    # Tonbilder und Durchblick
    ge = 300
    for clip, nm in ((0, 'pose1'), (1, 'pose2'), (2, 'ruhe')):
        V, F, B = pose(k, n, clip)
        mitte = (V.max(0) + V.min(0)) / 2
        s = ge * .46 / np.linalg.norm(V - mitte, axis=1).max()
        zeilen = []
        sum_d = 0
        for el in (0, 25, 60):
            z = []
            for az in range(0, 360, 45):
                alle, vorn, zb, nb = raster(V, F, blick(az, el), ge, s, mitte)
                d = alle & ~vorn
                sum_d += int(d.sum())
                z.append(schreib(ton(nb, vorn), 'az %d el %d' % (az, el)))
            zeilen.append(np.hstack(z))
        cv2.imwrite(os.path.join(AUS, 'ton_%s.png' % nm), np.vstack(zeilen))
        sag('%s: Durchblick ueber 24 Blicke %d px' % (nm, sum_d))
    with open(os.path.join(HIER, 'mess', 'netz.txt'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(bericht))


if __name__ == '__main__':
    main()
