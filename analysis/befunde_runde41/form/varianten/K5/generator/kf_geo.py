#!/usr/bin/env python3
"""K5c Farbe Runde 2: Texel-Geometrie. Je genutztem Atlastexel die Weltlage und die Normale in der Ruhepose (Clip 2, Bild 0, wie die
Familientafel 03 und Riegel N1/N2), aus den Flaechen der Variante (Texelzentrum im UV-Dreieck, baryzentrisch interpoliert; Ecknormalen wie
render.blick, Gouraud). Kein RE-Wert: Messwerkzeug, damit Fleischflaechen als zusammenhaengende Koerperpartien (Bauch/Unterseite wie RE2-G4,
Sicht-Pruefung r1 P1) statt als Atlasflecken gewaehlt werden.

    python kf_geo.py <Variantenordner> <aus.npz>
Ausgabe: pos (h, w, 3), nrm (h, w, 3) normiert, gesetzt (h, w) bool; PSX-Koordinaten (y nach unten)."""
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'farbe_lesung', 'B_skripte'))
import fl  # noqa: E402
import render as R  # noqa: E402


def geometrie(vdir, clip=2, bild=0):
    mo = fl.lade_var(vdir)
    Rm, Tm = mo.pose(R.kf(mo, clip, bild))
    h, w = mo.T.px.shape if hasattr(mo.T, 'px') else mo.tex.shape
    pos = np.zeros((h, w, 3), np.float32)
    nrm = np.zeros((h, w, 3), np.float32)
    ges = np.zeros((h, w), bool)
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        Wm = Rm[b].astype(float) / 4096.0
        t = Tm[b].astype(float)
        for verts, norms, faces, uvs, q in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            V = (np.array([x[:3] for x in verts], float) @ Wm.T) + t
            N = (np.array([x[:3] for x in norms], float) / 4096.0) @ Wm.T
            for g, uv in zip(faces, uvs):
                ie, inn = list(g[1::2]), list(g[0::2])
                po = (uv[5] & 0xF) * 128
                if q:
                    T = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    tris = ((0, 1, 2), (1, 3, 2))       # wie p_lade.flaechen (Texelsammlung der Riegel)
                else:
                    T = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tris = ((0, 1, 2),)
                for a, bb, c in tris:
                    u = np.array([T[a][0], T[bb][0], T[c][0]], float)
                    v = np.array([T[a][1], T[bb][1], T[c][1]], float)
                    A2 = (u[1] - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (v[1] - v[0])
                    if abs(A2) < 1e-9:
                        continue
                    x0, x1, y0, y1 = int(np.floor(u.min())), int(np.ceil(u.max())), int(np.floor(v.min())), int(np.ceil(v.max()))
                    X, Y = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
                    w1 = ((X - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (Y - v[0])) / A2
                    w2 = ((u[1] - u[0]) * (Y - v[0]) - (X - u[0]) * (v[1] - v[0])) / A2
                    w0 = 1 - w1 - w2
                    inn_ = (w1 >= -1e-6) & (w2 >= -1e-6) & (w0 >= -1e-6)
                    xs = np.clip(np.floor(X[inn_]).astype(int), 0, w - 1)
                    ys = np.clip(np.floor(Y[inn_]).astype(int), 0, h - 1)
                    neu = ~ges[ys, xs]                      # erste Flaeche je Texel (wie p_lade.sammeln)
                    ww = np.stack([w0[inn_], w1[inn_], w2[inn_]], -1)
                    P = ww @ V[[ie[a], ie[bb], ie[c]]]
                    Nn = ww @ N[[inn[a], inn[bb], inn[c]]]
                    pos[ys[neu], xs[neu]] = P[neu]
                    nrm[ys[neu], xs[neu]] = Nn[neu]
                    ges[ys[neu], xs[neu]] = True
    nrm /= np.maximum(np.linalg.norm(nrm, axis=-1, keepdims=True), 1e-9)
    return pos, nrm, ges


if __name__ == '__main__':
    p, n, g = geometrie(sys.argv[1])
    np.savez_compressed(sys.argv[2], pos=p, nrm=n, gesetzt=g)
    print('Texel', int(g.sum()), 'Normale y Mittel', float(n[..., 1][g].mean()))
