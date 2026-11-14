"""Pruefer P5: Spitzen der Oberkontur (Mesh 8+9, Klassen 1..5) in echten Dumps. Buckel-Mass je Spalte x: b(x) = h(x) - (h(x-12)+h(x+12))/2
(h = Hoehe der obersten Figurzeile, 960x720); Spitze = groesstes b in den Spalten Kopf +-90 px, aber nicht ueber dem Kopf selbst? -> beides:
'ueber Kopf' (Spalten mit Kopfklasse) und 'neben Kopf'."""
import numpy as np, json
import szene as S
N = ['V7', 'R4', 'R6', 'R2', 'R5D', 'R3']
K = {n: np.load(S.L_ + n + '_kl.npz')['K'] for n in N}
RAND = 40
def mass(k, d=6):
    F = k >= 1; H = k >= 3
    ys, xs = np.nonzero(H); x0, x1 = max(xs.min() - RAND, d), min(xs.max() + RAND, 959 - d)
    top = np.array([np.nonzero(F[:, x])[0].min() if F[:, x].any() else 720 for x in range(960)], float); h = -top
    hk = np.array([np.nonzero(H[:, x])[0].min() if H[:, x].any() else 720 for x in range(960)])
    best = (0, None, None)
    for x in range(x0, x1 + 1):
        if top[x] >= 720 or top[x - d] >= 720 or top[x + d] >= 720: continue
        b = h[x] - (h[x - d] + h[x + d]) / 2
        if b > best[0]: best = (b, x, 'Kopf' if hk[x] == top[x] else 'Rumpf')
    return round(best[0], 1), best[1], best[2]
res = {}
for lo, hi in ((262, 313), (344, 378)):
    print('F%d-F%d: groesste Spitze b (px 960x720, Bild, x, oberste Klasse)' % (lo, hi))
    for n in N:
        z = [(mass(K[n][f - S.F0]), f) for f in range(lo, hi + 1)]
        res['%s_%d' % (n, lo)] = [(f, m) for m, f in z]
        m = max(z, key=lambda q: q[0][0])
        rumpf = [q for q in z if q[0][2] == 'Rumpf']
        mr = max(rumpf, key=lambda q: q[0][0]) if rumpf else ((0, None, None), None)
        print('   %-4s max %s bei F%d | max Rumpf-Spitze %s bei F%s | F278 %s F311 %s F354 %s F362 %s' % (n, m[0], m[1], mr[0], mr[1],
              *[dict((f, q) for q, f in z).get(ff, '-') for ff in (278, 311, 354, 362)]))
json.dump(res, open(S.L_ + 'spitze_d6.json', 'w'))
