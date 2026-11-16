"""Pruefer P5 R2 (G): (1) Umriss R2 gegen R6 in allen 177 Ansichten (Variante 0, Tiefenpuffer, alle Knochen): hinzugekommene / weggefallene
Bildpunkte, davon ausserhalb Kopffenster (Kopf R6 +-60 px), groesster Abstand zur R6-Figur (EDT). (2) S9-Ersatz: Oberkontur Knochen 0-15
x 395-515 (Wortlaut riegel_v8 Kopf Z. 32) in P1, F344, F354, F364 gegen V7 und R6: groesste Hoehe ueber V7 (px, 640x480)."""
import sys, os, json, numpy as np
from scipy import ndimage
from multiprocessing import Pool
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as Vn


def arbeit(a):
    kn, i0, i1 = a
    K, R6 = Vn.lade(kn), Vn.lade('R6'); out = []
    for nm, pose in L.ansichten()[i0:i1]:
        L.setvar(L.VAR[0])
        a_ = pl.raster(K, pl.world_tris(K, pose))['bone']; b_ = pl.raster(R6, pl.world_tris(R6, pose))['bone']
        A, B = a_ >= 0, b_ >= 0; H = b_ == 9; W = np.zeros_like(A)
        if H.any():
            ys, xs = np.nonzero(H); W[max(ys.min() - 60, 0):ys.max() + 61, max(xs.min() - 60, 0):xs.max() + 61] = True
        neu, weg = A & ~B, B & ~A
        dn = ndimage.distance_transform_edt(~B)[neu]; dw = ndimage.distance_transform_edt(~A)[weg]
        out.append((nm, int(neu.sum()), int(weg.sum()), int((neu & ~W).sum()), int((weg & ~W).sum()),
                    round(float(dn.max()), 1) if len(dn) else 0.0, round(float(dw.max()), 1) if len(dw) else 0.0,
                    round(float(ndimage.distance_transform_edt(~B)[neu & ~W].max()), 1) if (neu & ~W).any() else 0.0,
                    round(float(ndimage.distance_transform_edt(~A)[weg & ~W].max()), 1) if (weg & ~W).any() else 0.0))
    return out


def oben(E, pose):
    L.setvar(L.VAR[0]); b = pl.raster(E, pl.world_tris(E, pose, bones=list(range(16))))['bone'] >= 0
    return {x: int(np.nonzero(b[:, x])[0].min()) for x in range(395, 516) if b[:, x].any()}


if __name__ == '__main__':
    kn = sys.argv[1]; n = len(L.ansichten()); j = 8; k = (n + j - 1) // j
    with Pool(j) as p:
        res = [x for t in p.map(arbeit, [(kn, i, min(i + k, n)) for i in range(0, n, k)]) for x in t]
    json.dump(res, open(os.path.dirname(os.path.abspath(__file__)) + '/../vorn/g_rest_%s.json' % kn, 'w'))
    print('(1) %s gegen R6, 177 Ansichten: neu %d px, weg %d px | ausserhalb Kopffenster neu %d weg %d | max Abstand neu %.1f weg %.1f | ausserhalb Fenster max neu %.1f weg %.1f' % (
        kn, sum(r[1] for r in res), sum(r[2] for r in res), sum(r[3] for r in res), sum(r[4] for r in res), max(r[5] for r in res), max(r[6] for r in res),
        max(r[7] for r in res), max(r[8] for r in res)))
    print('   PV:', [r for r in res if r[0] == 'PV'], '| P1:', [r for r in res if r[0] == 'P1'], '| P2:', [r for r in res if r[0] == 'P2'])
    print('   groesste ausserhalb Fenster:', sorted(res, key=lambda r: -(r[3] + r[4]))[:8])
    A = dict(L.ansichten()); V7, R6, K = Vn.lade('V7'), Vn.lade('R6'), Vn.lade(kn)
    for nm in ('P1', 'F344', 'F354', 'F364'):
        o7, o6, ok = oben(V7, A[nm]), oben(R6, A[nm]), oben(K, A[nm])
        u = lambda o: max([o7[x] - o[x] for x in o if x in o7], default=0)
        print('(2) %s Oberkontur Knochen 0-15 x395-515 hoechstens ueber V7: %s %d px | R6 %d px | %s gegen R6 max Unterschied %d px' % (
            nm, kn, u(ok), u(o6), kn, max([abs(ok[x] - o6[x]) for x in ok if x in o6], default=0)))
