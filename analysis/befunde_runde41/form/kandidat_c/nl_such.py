# -*- coding: utf-8 -*-
"""Pose 2, Bein NL: Bild 2 zeigt links unter U keinen Dunkelkoerper (Lichtband (210-250,376-384), Feuer). Suche die Gelenkwinkel, mit denen das Bein in U bleibt.
Nur Bildrand-Mass auf Bild 2. Ergebnis pose2_nl.json. Frei gesetzte Pose, Bild 2 zeigt das Bein selbst nicht."""
from bau import *
import form_pruefung as PR
from scipy.optimize import differential_evolution

T = pickle.load(open('teile.pkl', 'rb'))
kn = knochen_tafel(T)
namen = [t['name'] for t in T]
idx = [namen.index(n) for n in ('nl_ober', 'nl_unter', 'nl_kugel_o', 'nl_kugel_u')]
idx_nl = idx
z = ZIEL[2]
schwarz = z == 0
band = PR.rechteck(205, 372, 262, 392)
links = PR.rechteck(150, 340, 228, 460)
fuss = PR.rechteck(150, 395, 330, 470)
v = np.zeros((480, 640), bool)
v_pts = [(int(x), int(round(347 - (x - 232) / 53.0))) for x in range(234, 286)]
rest = np.zeros((480, 640), np.uint8)
p0 = Pose(kn)
tr0 = p0.trafo(T)
for i, t in enumerate(T):
    if i not in idx_nl and not t['name'].startswith('kiefer'):
        raster(tr0(i, t['V']), t['F'], S2, rest)
rest = rest.astype(bool)


def pose(q):
    return Pose(kn, {16: dreh('x', q[0]) @ dreh('z', q[1]) @ dreh('y', 45.0), 17: dreh('z', q[2]), 18: dreh('z', q[3]), 19: dreh('x', q[4]) @ dreh('z', q[5])})


def kosten(q):
    tr = pose(q).trafo(T)
    r = np.zeros((480, 640), np.uint8)
    lo = -1e9
    for i in idx_nl:
        Q = tr(i, T[i]['V'])
        lo = max(lo, Q[:, 1].max())
        raster(Q, T[i]['F'], S2, r)
    m = r.astype(bool)
    deck = np.mean([(m | rest)[y, x] for x, y in v_pts])
    return (3.0 * (m & schwarz).sum() + 2.0 * (m & band).sum() + 1.5 * (m & links).sum() + 2.0 * (m & fuss).sum() + 3000.0 * (1 - deck) + 2.0 * max(lo + 40.0, 0.0)
            + 0.15 * sum(abs(a) for a in q))


g = [(-35, 35), (-20, 75), (-90, 90), (-90, 90), (-40, 40), (-90, 90)]
s = differential_evolution(kosten, g, maxiter=70, popsize=10, seed=3, tol=0.002, polish=False, updating='immediate')
q = s.x
print('Kosten', round(float(s.fun), 1), np.round(q, 1).tolist())
json.dump(dict(a16x=float(q[0]), a16z=float(q[1]), a17z=float(q[2]), a18z=float(q[3]), a19x=float(q[4]), a19z=float(q[5])), open('pose2_nl.json', 'w'), indent=1)
