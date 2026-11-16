"""Pruefer P5 R2 (D): Gesichts-Bildpunkte (Mesh-9-V7-Flaechen mit Schwerpunkt x9 >= 150) in ALLEN 177 Ansichten (Maler-Reihenfolge,
Variante 0): Bezug-Gesicht, das im Kandidaten nicht Gesicht ist, und welcher Knochen/welche Flaeche dort liegt. Bezug V7 und R6."""
import sys, os, numpy as np
from multiprocessing import Pool
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as Vn
def arbeit(a):
    kn, ref, i0, i1 = a
    K, B = Vn.lade(kn), Vn.lade(ref); V7 = Vn.lade('V7'); n7t, n7q = len(V7.meshes[9]['tf']), len(V7.meshes[9]['qf'])
    gK, gB = Vn.gesicht_flaechen(K, n7t, n7q), Vn.gesicht_flaechen(B, n7t, n7q); out = []
    for nm, pose in L.ansichten()[i0:i1]:
        rk, rb = Vn.render(K, pose), Vn.render(B, pose)
        m = lambda r, s: np.array([k in s for k in r['key'][:-1]] + [False])[r['face']]
        v = m(rb, gB) & ~m(rk, gK)
        if v.any():
            wer = {}
            for f in rk['face'][v]:
                k = rk['key'][f] if f >= 0 else None; wer[str(k)] = wer.get(str(k), 0) + 1
            out.append((nm, int(v.sum()), wer))
    return out
if __name__ == '__main__':
    kn, ref = sys.argv[1], sys.argv[2]; n = len(L.ansichten()); j = 8; k = (n + j - 1) // j
    with Pool(j) as p:
        res = [x for t in p.map(arbeit, [(kn, ref, i, min(i + k, n)) for i in range(0, n, k)]) for x in t]
    print('%s gegen %s, 177 Ansichten: %d Gesichts-Bildpunkte in %d Ansichten' % (kn, ref, sum(r[1] for r in res), len(res)), res)
