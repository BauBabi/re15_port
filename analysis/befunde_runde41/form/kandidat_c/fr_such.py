# -*- coding: utf-8 -*-
"""Pose 2, Bein FR: Der Stand aus Bild 1 faellt in Bild 2 auf sicheren Raum und Boden. Suche die Gelenkwinkel, mit denen das Bein in Feuerzone und Figur bleibt.
Nur Bildmass auf Bild 2. Ergebnis pose2_fr.json. Frei gesetzte Pose, Bild 2 zeigt dieses Bein nicht."""
from bau import *
import form_pruefung as PR
from scipy.optimize import differential_evolution

T = pickle.load(open('teile.pkl', 'rb'))
kn = knochen_tafel(T)
namen = [t['name'] for t in T]
idx = [namen.index(n) for n in ('fr_huefte', 'fr_bein', 'fr_fuss', 'fr_zeh')]
schwarz = ZIEL[2] == 0


def pose(q):
    r1 = dreh('y', q[0]) @ dreh('x', q[1]) @ dreh('z', q[2])
    r2 = dreh('z', q[3])
    return Pose(kn, {1: r1, 2: r2, 3: (r1 @ r2).T})


def kosten(q):
    tr = pose(q).trafo(T)
    r = np.zeros((480, 640), np.uint8)
    lo = -1e9
    for i in idx:
        Q = tr(i, T[i]['V'])
        lo = max(lo, Q[:, 1].max())
        raster(Q, T[i]['F'], S2, r)
    m = r.astype(bool)
    return 3.0 * (m & schwarz).sum() + 12.0 * max(lo, 0.0) + 2.0 * max(-lo - 70.0, 0.0) + 0.2 * sum(abs(a) for a in q)


g = [(-45, 45), (-45, 45), (-45, 45), (-70, 70)]
s = differential_evolution(kosten, g, maxiter=60, popsize=12, seed=5, tol=0.002, polish=False, updating='immediate')
q = s.x
tr = pose(q).trafo(T)
r = np.zeros((480, 640), np.uint8)
for i in idx:
    raster(tr(i, T[i]['V']), T[i]['F'], S2, r)
print('Kosten', round(float(s.fun), 1), np.round(q, 1).tolist(), 'Schwarz', int((r.astype(bool) & schwarz).sum()), 'tiefster Punkt', round(max(tr(i, T[i]['V'])[:, 1].max() for i in idx)))
json.dump(dict(a1y=float(q[0]), a1x=float(q[1]), a1z=float(q[2]), a2z=float(q[3])), open('pose2_fr.json', 'w'), indent=1)
