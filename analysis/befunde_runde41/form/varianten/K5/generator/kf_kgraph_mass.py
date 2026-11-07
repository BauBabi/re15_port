"""Versuch 2 Runde 2, Unabhaengigkeitsnachweis K_GRAPH (Dossier FARBE.md V2r2):  Abdeckung der Atlas-8-Nachbarschaft (gleiches Mesh) durch die k naechsten 3D-Nachbarn, Anteil Kanten zu Texeln, die im Atlas
NICHT 8-benachbart sind und mehr als 2 Texelabstaende entfernt liegen (Querverbindungen), Zusammenhangskomponenten des Graphen."""
import sys, json
import numpy as np
import os
G_ = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, G_)
import kf_mal3 as M3
from scipy.spatial import cKDTree
from scipy.sparse import coo_matrix
from scipy.sparse.csgraph import connected_components
G = M3.oberflaeche()
ys, xs, mesh, gen, idx, P = G['ys'], G['xs'], G['mesh'], G['genutzt'], G['idx'], G['P']
h, w = gen.shape
paare = set()
for dy, dx in ((0, 1), (1, 0), (1, 1), (1, -1)):
    y2, x2 = ys + dy, xs + dx
    ok = (y2 >= 0) & (y2 < h) & (x2 >= 0) & (x2 < w)
    ok[ok] &= gen[y2[ok], x2[ok]]
    ok[ok] &= mesh[y2[ok], x2[ok]] == mesh[ys[ok], xs[ok]]
    a = np.nonzero(ok)[0]; b = idx[y2[ok], x2[ok]]
    paare |= set(zip(np.minimum(a, b).tolist(), np.maximum(a, b).tolist()))
baum = cKDTree(P)
out = {}
for k in (4, 6, 8, 10, 12, 16):
    d, nb = baum.query(P, k=k + 1)
    r = np.repeat(np.arange(len(P)), k); c = nb[:, 1:].ravel()
    kn = set(zip(np.minimum(r, c).tolist(), np.maximum(r, c).tolist()))
    abdeck = len(paare & kn) / len(paare)
    fremd = kn - paare
    fa = np.array(list(fremd)) if fremd else np.zeros((0, 2), int)
    weit = 0
    if len(fa):
        da = np.abs(ys[fa[:, 0]] - ys[fa[:, 1]]) + np.abs(xs[fa[:, 0]] - xs[fa[:, 1]])
        andere = (mesh[ys[fa[:, 0]], xs[fa[:, 0]]] != mesh[ys[fa[:, 1]], xs[fa[:, 1]]]) | (da > 2)
        weit = int(andere.sum())
    A = coo_matrix((np.ones(len(r)), (r, c)), shape=(len(P), len(P)))
    nk, _ = connected_components(A, directed=False)
    out[k] = dict(abdeckung_8nachbarn=round(abdeck, 3), kanten=len(kn), quer_kanten_anteil=round(weit / len(kn), 3), komponenten_nur_knn=int(nk))
    print(k, out[k], flush=True)
json.dump(out, open(sys.argv[1], 'w'), indent=1)
