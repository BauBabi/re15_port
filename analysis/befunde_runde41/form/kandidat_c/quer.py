# Wo landen die Marken im jeweils anderen Bild? Plus Triangulation moeglicher Paare.
from bau import *
import form_pruefung as PR
T = pickle.load(open('teile.pkl', 'rb'))
kn = knochen_tafel(T)
pz1, pz2, pzr = posen_definieren(kn)
tr2 = pz2.trafo(T)
tr1 = pz1.trafo(T)
Z1 = z_puffer(T, S1, tr1)
Z2 = z_puffer(T, S2, tr2)
Yb = {n: (BILD[n].astype(np.float32) @ np.array([.299, .587, .114], np.float32)) for n in (1, 2)}
def tri(xy1, xy2, S2x):
    a1, r1 = FK.strahl(S1.k, np.array(xy1, float)); a2, r2 = FK.strahl(S2x.k, np.array(xy2, float))
    A1 = S1.aktor(a1); D1 = r1 @ S1.R
    A2 = S2x.aktor(a2); D2 = r2 @ S2x.R
    t = np.linalg.lstsq(np.array([D1, -D2]).T, A2 - A1, rcond=None)[0]
    P1 = A1 + D1 * t[0]; P2 = A2 + D2 * t[1]
    return (P1 + P2) / 2, float(np.linalg.norm(P1 - P2))
o = BILD[1].copy()
cols = {'Q1': (0, 255, 0), 'Q2': (255, 255, 0), 'Q3': (0, 255, 255), 'Q4': (255, 0, 255), 'Q5': (255, 128, 0), 'P': (255, 255, 255), 'G': (255, 0, 0)}
r = T[0]
for s in PR.stellen():
    if s['bild'] != 2:
        continue
    m = np.zeros((480, 640), bool)
    for q in s['polygone']:
        m |= PR.poly_maske(q)
    ys, xs = np.nonzero(m)
    zz = Z2[ys, xs]; ok = np.isfinite(zz)
    P = S2.auf_tiefe(np.c_[xs[ok], ys[ok]].astype(float), zz[ok])
    p1 = S1.bild(P)
    d1 = S1.tiefe(P)
    xi = np.clip(np.round(p1[:, 0]).astype(int), 0, 639); yi = np.clip(np.round(p1[:, 1]).astype(int), 0, 479)
    sicht = d1 <= Z1[yi, xi] + 120
    print(s['id'], 'Bild1 Mitte', np.round(np.nanmean(p1, 0)), 'sichtbar', round(float(sicht.mean()), 2), 'Video-Y dort', round(float(Yb[1][yi[sicht], xi[sicht]].mean()) if sicht.any() else -1, 0), 'Aktor', np.round(P.mean(0)))
    for a, b in zip(xi[sicht], yi[sicht]):
        o[b, a] = cols[s['id']]
cv2.imwrite('quer_b1.png', cv2.resize(o[175:340, 230:440], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)[:, :, ::-1])
for g in (1081, 1166, 1252, 1338):
    Sx = Stand(2, L2, g)
    for nm, a, b in (('A36/B01', (340, 200), (286, 182)), ('A35/B02', (336, 226), (275, 207)), ('A10/B03', (291, 208), (327, 169))):
        p, d = tri(a, b, Sx)
        print(g, nm, np.round(p), 'Rest', round(d), 'Abstand Rumpfmitte', round(float(np.linalg.norm(p - np.array([0, -1950, 0])))))
for t in T:
    if t['name'] in ('nr_unter', 'nr_hand'):
        R = np.array(t['ringe'])
        print(t['name'], 'Pol0', np.round(t['V'][t['pole'][0]]), 'Pol1', np.round(t['V'][t['pole'][1]]), 'Ring0', np.round(t['V'][R[0]].mean(0)), 'RingN', np.round(t['V'][R[-1]].mean(0)))
