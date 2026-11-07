#!/usr/bin/env python3
"""Schritt 1: Mittellinie, Breite, Enden, Engstellen je Nutzer-Teil. Masken in Bildgroesse, 640x480."""
import json, os, sys
import numpy as np, cv2
from skimage.morphology import medial_axis
from scipy import ndimage as ndi
from scipy.sparse.csgraph import dijkstra
from scipy.sparse import coo_matrix

F = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
T = os.path.join(F, 'nutzer_teile')
lage = json.load(open(os.path.join(T, 'lage.json'), encoding='utf-8'))


def laengster_pfad(sk, dist):
    pts = np.argwhere(sk)
    idx = -np.ones(sk.shape, int); idx[sk] = np.arange(len(pts))
    r, c, w = [], [], []
    for i, (y, x) in enumerate(pts):
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if dx == 0 and dy == 0:
                    continue
                yy, xx = y + dy, x + dx
                if 0 <= yy < sk.shape[0] and 0 <= xx < sk.shape[1] and idx[yy, xx] >= 0:
                    r.append(i); c.append(idx[yy, xx]); w.append(np.hypot(dx, dy))
    g = coo_matrix((w, (r, c)), shape=(len(pts), len(pts))).tocsr()
    d0, _ = dijkstra(g, indices=0, return_predecessors=True)
    d0[~np.isfinite(d0)] = -1
    a = int(np.argmax(d0))
    d1, p = dijkstra(g, indices=a, return_predecessors=True)
    d1[~np.isfinite(d1)] = -1
    b = int(np.argmax(d1))
    weg = [b]
    while weg[-1] != a:
        weg.append(int(p[weg[-1]]))
    return pts[np.array(weg)][:, ::-1].astype(float), float(d1[b])


erg = {}
for name, l in lage.items():
    m = cv2.imread(os.path.join(T, 'maske_%s.png' % name), cv2.IMREAD_UNCHANGED)
    if m.ndim == 3:
        m = m[..., -1]
    m = (m > 127)
    n, lab = cv2.connectedComponents(m.astype(np.uint8))
    groessen = [int((lab == i).sum()) for i in range(1, n)]
    m1 = lab == (1 + int(np.argmax(groessen)))
    glatt = cv2.morphologyEx(m1.astype(np.uint8), cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))) > 0
    sk, dist = medial_axis(glatt, return_distance=True)
    pfad, laenge = laengster_pfad(sk, dist)
    # glaetten und gleichmaessig abtasten
    kx = ndi.gaussian_filter1d(pfad[:, 0], 4, mode='nearest')
    ky = ndi.gaussian_filter1d(pfad[:, 1], 4, mode='nearest')
    s = np.r_[0, np.cumsum(np.hypot(np.diff(kx), np.diff(ky)))]
    n_ab = max(8, int(s[-1] / 12))
    si = np.linspace(0, s[-1], n_ab)
    px, py = np.interp(si, s, kx), np.interp(si, s, ky)
    br = 2 * dist[np.round(py).astype(int).clip(0, 479), np.round(px).astype(int).clip(0, 639)]
    # Enge Stellen = lokale Minima der Breite; Enden
    br_g = ndi.gaussian_filter1d(br, 1.0, mode='nearest')
    eng = [i for i in range(1, len(br_g) - 1) if br_g[i] <= br_g[i - 1] and br_g[i] <= br_g[i + 1]]
    wulst = [i for i in range(1, len(br_g) - 1) if br_g[i] >= br_g[i - 1] and br_g[i] >= br_g[i + 1]]
    ys, xs = np.nonzero(m1)
    erg[name] = dict(bild=l['bild'], flaeche=int(m1.sum()), komponenten=groessen,
                     bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                     schwerpunkt=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                     laenge_px=round(float(s[-1]), 1),
                     mittellinie=[[round(float(a), 1), round(float(b), 1), round(float(c), 1)] for a, b, c in zip(px, py, br)],
                     engstellen=[[round(float(px[i]), 1), round(float(py[i]), 1), round(float(br[i]), 1)] for i in eng],
                     wuelste=[[round(float(px[i]), 1), round(float(py[i]), 1), round(float(br[i]), 1)] for i in wulst])
    print(name, 'Bild', l['bild'], 'Flaeche', int(m1.sum()), 'Komp', groessen, 'bbox', erg[name]['bbox'], 'Laenge %.0f' % s[-1])
    print('   Mittellinie (x,y,Breite):', [tuple(np.round(q, 0).astype(int).tolist()) for q in zip(px[::2], py[::2], br[::2])])
    print('   eng', erg[name]['engstellen'])
    print('   wulst', erg[name]['wuelste'])
json.dump(erg, open(os.path.join(F, 'gerippe', 'lauf', 'teile_mass.json'), 'w', encoding='utf-8'), indent=1)

# Ueberlappung der Masken
namen = list(lage)
masks = {}
for n in namen:
    m = cv2.imread(os.path.join(T, 'maske_%s.png' % n), cv2.IMREAD_UNCHANGED)
    masks[n] = (m[..., -1] if m.ndim == 3 else m) > 127
for i, a in enumerate(namen):
    for b in namen[i + 1:]:
        if lage[a]['bild'] == lage[b]['bild']:
            u = (masks[a] & masks[b]).sum()
            if u:
                print('Ueberlappung', a, b, int(u))
