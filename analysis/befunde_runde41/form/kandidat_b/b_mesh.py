#!/usr/bin/env python3
"""Kandidat B: Volumen je Teil -> Dreiecksnetz (Marching Cubes) im Bindraum. Kuppel als ein Stueck, Trennung in Knochen erst nach dem Remesh.
    python b_mesh.py zw/fit1.pt --aus zw/mesh1
"""
import argparse, json, math, os, sys
import numpy as np, torch
from skimage import measure
from b_geom import *
import b_teile as T
from b_fit import GERAET, ablage_torch, abst_kanten, glatt_min


def lade(pfad):
    return torch.load(pfad, map_location='cpu', weights_only=False)


class Feld:
    """Glatte Vereinigung einer Kantenmenge, optional Zusatzfunktionen (Rillen)."""

    def __init__(self, fit, bones, k=30.0, extra=None):
        self.k = k
        ni = {n: i for i, n in enumerate(fit['namen'])}
        K = [(ni[a], ni[b]) for b_, a, b in fit['kanten'] if b_ in bones]
        p, r = fit['pos'], fit['rad']
        self.a = ablage_torch(p[[x for x, _ in K]])
        self.b = ablage_torch(p[[y for _, y in K]])
        self.ra = ablage_torch(r[[x for x, _ in K]])
        self.rb = ablage_torch(r[[y for _, y in K]])
        ids = sorted({i for q in K for i in q})
        self.lo = (p[ids] - r[ids, None]).min(0)
        self.hi = (p[ids] + r[ids, None]).max(0)
        self.extra = extra

    def __call__(self, g):
        P = ablage_torch(g)
        aus = []
        with torch.no_grad():
            for i in range(0, P.shape[0], 150000):
                q = P[i:i + 150000]
                d = glatt_min(abst_kanten(q, self.a, self.b, self.ra, self.rb), self.k)
                if self.extra is not None:
                    d = self.extra(q, d)
                aus.append(d.cpu().numpy())
        return np.concatenate(aus)


def netz(fn, lo, hi, zelle):
    n = np.ceil((hi - lo) / zelle).astype(int) + 1
    xs = [lo[i] + np.arange(n[i]) * zelle for i in range(3)]
    G = np.stack(np.meshgrid(*xs, indexing='ij'), -1).reshape(-1, 3).astype(np.float32)
    f = fn(G).reshape(n)
    v, fa, _, _ = measure.marching_cubes(f, 0.0, spacing=(zelle, zelle, zelle))
    # Nur die groesste zusammenhaengende Schale: Hohlraeume im Volumen ergaeben innere Schalen, QuadriFlow nimmt sonst die falsche
    from scipy.sparse import coo_matrix
    from scipy.sparse.csgraph import connected_components
    e = np.concatenate([fa[:, [0, 1]], fa[:, [1, 2]]])
    g = coo_matrix((np.ones(len(e)), (e[:, 0], e[:, 1])), shape=(len(v), len(v)))
    n, lab = connected_components(g, directed=False)
    gr = np.bincount(lab)
    gut = lab == gr.argmax()
    neu = np.cumsum(gut) - 1
    fa = fa[gut[fa[:, 0]]]
    fa = neu[fa]
    v = v[gut]
    return v + lo, fa[:, ::-1]


def schreibe_obj(pfad, v, f):
    with open(pfad, 'w') as h:
        h.write(''.join('v %.2f %.2f %.2f\n' % tuple(p) for p in v))
        h.write(''.join('f %d %d %d\n' % (t[0] + 1, t[1] + 1, t[2] + 1) for t in f))


def baue(fit, aus, zelle=18.0, k=30.0, kuppel_extra=None, teile_extra=None, k_kuppel=30.0):
    os.makedirs(aus, exist_ok=True)
    info = {}
    # Kuppel
    F = Feld(fit, {'rumpf_m'}, k_kuppel, kuppel_extra)
    v, f = netz(F, F.lo - 3 * zelle - 4 * k_kuppel, F.hi + 3 * zelle + 4 * k_kuppel, zelle * 1.3)
    schreibe_obj(os.path.join(aus, 'kuppel.obj'), v, f)
    info['kuppel'] = len(f)
    for b, (nm, _) in enumerate(T.KNOCHEN):
        if nm in ('rumpf', 'rumpf_m', 'rumpf_f'):
            continue
        ex = (teile_extra or {}).get(nm)
        if ex is not None and 'datei' in ex:
            schreibe_obj(os.path.join(aus, 'k%02d.obj' % b), ex['v'], ex['f'])
            info[nm] = len(ex['f'])
            continue
        if not any(x[0] == nm for x in fit['kanten']):
            continue
        F = Feld(fit, {nm}, k, ex)
        v, f = netz(F, F.lo - 3 * zelle - 4 * k, F.hi + 3 * zelle + 4 * k, zelle)
        schreibe_obj(os.path.join(aus, 'k%02d.obj' % b), v, f)
        info[nm] = len(f)
    print(json.dumps(info))


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('fit')
    ap.add_argument('--aus', default='zw/mesh')
    a = ap.parse_args()
    baue(lade(a.fit), a.aus)
