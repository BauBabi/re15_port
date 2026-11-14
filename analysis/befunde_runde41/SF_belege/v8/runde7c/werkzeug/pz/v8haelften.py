"""Pruefer P5 R2: Viereck qf8 (Mesh 8, Ecken 30,0,50,49) und seine Ergaenzungs-Dreiecke tf77/tf78. Je Ansicht (177, Variante 0, Maler):
gezeichnet? (Port-Culling quad (e0,e1,e2) m<0, tri m<=0, wie pl.world_tris), sichtbare Bildpunkte von tf77/78, davon dort, wo qf8 NICHT
gezeichnet ist, und davon ausserhalb (8|9) V7 / R6 (+1 px). Vorderansicht = vorn.cosvorn >= 0,5. Aufruf: python v8haelften.py"""
import sys, os, json
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as Vn
KN, QI, TA, TB = os.environ.get('KN', 'R2'), int(os.environ.get('QI', 8)), int(os.environ.get('TA', 77)), int(os.environ.get('TB', 78))


def arbeit(a):
    i0, i1 = a
    K, V7, R6 = Vn.lade(KN), Vn.lade('V7'), Vn.lade('R6')
    out = []
    for nm, pose in L.ansichten()[i0:i1]:
        L.setvar(L.VAR[0])
        tris = pl.world_tris(K, pose); r = pl.raster_maler(K, tris)
        keys = [(t[0], t[1], t[2]) for t in tris]
        q8 = any(k == (8, 'q', QI) for k in keys); t77 = any(k == (8, 't', TA) for k in keys); t78 = any(k == (8, 't', TB) for k in keys)
        lut = np.array([k in ((8, 't', TA), (8, 't', TB)) for k in keys] + [False]); sh = lut[r['face']]
        r7 = pl.raster_maler(V7, pl.world_tris(V7, pose)); r6 = pl.raster_maler(R6, pl.world_tris(R6, pose))
        S7 = ndimage.binary_dilation((r7['bone'] == 8) | (r7['bone'] == 9), np.ones((3, 3), bool))
        S6 = ndimage.binary_dilation((r6['bone'] == 8) | (r6['bone'] == 9), np.ones((3, 3), bool))
        cv = Vn.cosvorn(V7, pose)
        out.append(dict(nm=nm, cos=round(cv, 3), q8=q8, t77=t77, t78=t78, sicht=int(sh.sum()), aussen_v7=int((sh & ~S7).sum()),
                        aussen_r6=int((sh & ~S6).sum()), aussen_beide=int((sh & ~S7 & ~S6).sum())))
    return out


if __name__ == '__main__':
    from multiprocessing import Pool
    n = len(L.ansichten()); j = 8; k = (n + j - 1) // j
    with Pool(j) as p:
        res = [x for t in p.map(arbeit, [(i, min(i + k, n)) for i in range(0, n, k)]) for x in t]
    json.dump(res, open(os.path.dirname(os.path.abspath(__file__)) + '/../vorn/haelften_%s_q%d.json' % (KN, QI), 'w'))
    oq = [r for r in res if not r['q8']]
    print('Ansichten', len(res), '| qf8 verworfen in', len(oq), '| davon tf77 oder tf78 gezeichnet', sum(1 for r in oq if r['t77'] or r['t78']))
    for grp, sel in (('alle', res), ('vorn', [r for r in res if r['cos'] >= 0.5]), ('seite/hinten', [r for r in res if r['cos'] < 0.5])):
        print('  %-12s Ansichten %d | tf77/78 sichtbar px %d in %d | ausserhalb V7 %d in %d | ausserhalb R6 %d | ausserhalb V7 UND R6 %d in %d' % (
            grp, len(sel), sum(r['sicht'] for r in sel), sum(1 for r in sel if r['sicht']), sum(r['aussen_v7'] for r in sel),
            sum(1 for r in sel if r['aussen_v7']), sum(r['aussen_r6'] for r in sel), sum(r['aussen_beide'] for r in sel), sum(1 for r in sel if r['aussen_beide'])))
    print('  groesste aussen V7 UND R6:', sorted([(r['nm'], r['aussen_beide']) for r in res if r['aussen_beide']], key=lambda q: -q[1])[:12])
