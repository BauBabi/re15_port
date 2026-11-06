"""Texel je Mesh (eindeutig, mit der CLUT-Zeile, die die Flaeche nutzt) + Dreiecksdaten (UV-Flaeche, 3D-Flaeche)."""
import numpy as np
import fl


def sammeln(mo):
    F = fl.flaechen(mo)
    h, w = mo.tex.shape
    per = {}
    tri = []
    feld = np.full((h, w), -1, int)
    for b, P, T, z in F:
        xs, ys, uvA = fl.texel_der_flaeche(T, w, h)
        A3 = 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[2] - P[0]))
        tri.append((b, uvA, A3))
        if len(xs) == 0:
            continue
        s = per.setdefault(b, {})
        for x, y in zip(xs.tolist(), ys.tolist()):
            s[(x, y)] = z
        feld[ys, xs] = b
    out = {}
    for b, s in per.items():
        k = np.array(list(s.keys()), int)
        z = np.array(list(s.values()), int)
        rgb = mo.pal[z, mo.tex[k[:, 1], k[:, 0]]]
        idx = mo.tex[k[:, 1], k[:, 0]]
        out[b] = dict(xy=k, zeile=z, rgb=rgb, idx=idx)
    return out, tri, feld


def atlasbild(mo, per):
    h, w = mo.tex.shape
    a = np.zeros((h, w, 3), np.uint8)
    m = np.zeros((h, w), bool)
    for b, d in per.items():
        a[d['xy'][:, 1], d['xy'][:, 0]] = d['rgb']
        m[d['xy'][:, 1], d['xy'][:, 0]] = True
    return a, m
