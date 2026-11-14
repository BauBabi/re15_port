"""Pruefer P5 (C) helle Bildpunkte, Oberkontur-Spitzen; (F) Kippen; echte Dumps (eigene Laeufe). Aufruf: python szene2.py"""
import sys, os, json
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import szene as S
L_ = S.L_
N = ['R3', 'R2', 'V7', 'R6', 'R4', 'R5D']
K = {n: np.load(L_ + n + '_kl.npz')['K'] for n in N}
out = {}
# (C1) helle Bildpunkte im Kasten x 420..640, y 200..400 (960x720), Regel B >= R-6, B > 120, R > 110 (Pruefer r6 c6c.py, Bauer-Regel)
hell = {n: [] for n in N}
for f in range(344, 379):
    for n in N:
        im = S.ppm(n, f); k = im[200:400, 420:640]; r, g, b = k[..., 0], k[..., 1], k[..., 2]
        hell[n].append(int(((b >= r - 6) & (b > 120) & (r > 110)).sum()))
print('(C1) helle Bildpunkte im Kasten F344-F378:')
for n in N:
    a = np.array(hell[n]); print('   %-4s min %d Median %d max %d | ueber 87: %d Bilder %s' % (n, a.min(), np.median(a), a.max(), (a > 87).sum(), [344 + i for i in np.nonzero(a > 87)[0]][:12]))
out['hell'] = hell


# (C2) Oberkontur ueber/neben dem Kopf (Figur = Klassen 1..5 = Mesh 8 und 9), F344-F378, Spalten Kopf +-90 px (960x720)
def kontur(k):
    F = (k >= 1); H = k >= 3
    if not H.any(): return None
    ys, xs = np.nonzero(H); x0, x1 = max(xs.min() - 90, 0), min(xs.max() + 90, 959)
    top = {}
    for x in range(x0, x1 + 1):
        c = np.nonzero(F[:, x])[0]
        if len(c): top[x] = int(c.min())
    return top


def spitzen(top, w=24):
    """Spitze = Spalte, deren Oberkante hoeher liegt als beide Seiten (Minimum der Hoehe je Seite im Fenster w); Prominenz in px."""
    xs = sorted(top); h = np.array([-top[x] for x in xs], float); res = []
    for i in range(len(xs)):
        if i < 2 or i > len(xs) - 3: continue
        if h[i] < h[i - 1] or h[i] < h[i + 1]: continue
        l = h[max(i - w, 0):i]; r = h[i + 1:i + 1 + w]
        if len(l) < w // 2 or len(r) < w // 2: continue
        if h[i] < l.max() or h[i] < r.max(): continue
        p = h[i] - max(l.min(), r.min())
        res.append((xs[i], round(float(p), 1)))
    stufe = max([abs(h[i + 1] - h[i]) for i in range(len(h) - 1)], default=0)
    return res, stufe


print('(C2) Oberkontur (Mesh 8+9) Spitzen mit Prominenz >= 6 px (960x720 = 4 px in 640x480) und groesste Stufe je Bild:')
sp = {}
for n in N:
    z = []
    for f in range(344, 379):
        top = kontur(K[n][f - S.F0])
        if top is None: z.append((f, [], 0)); continue
        s, st = spitzen(top); z.append((f, [x for x in s if x[1] >= 6], int(st)))
    sp[n] = z
    print('   %-4s F354 %s Stufe %d | F362 %s Stufe %d | Bilder mit Spitze >= 6: %d | max Prominenz %.1f | max Stufe %d' % (
        n, z[10][1], z[10][2], z[18][1], z[18][2], sum(1 for q in z if q[1]), max([p for q in z for _, p in q[1]], default=0), max(q[2] for q in z)))
out['spitzen'] = {n: [(f, s, st) for f, s, st in sp[n]] for n in N}

# (F) Kippen A-B-A ueber drei Folgebilder (Mesh-8/9-Klassen 1..5 und Hintergrund -1) im Fenster Kopf +-60 px, F226-F449
print('(F) Kippen Kopf/Ruecken A-B-A (Klassen -1,1..5; Fenster Kopf +-60 px; 960x720):')
kp = {}
for n in N:
    Kn = K[n]; tot = 0; je = []; mit5 = 0
    for f in range(226, 450):
        a, b, c = Kn[f - 1 - S.F0], Kn[f - S.F0], Kn[f + 1 - S.F0]
        H = b >= 3
        if not H.any(): je.append(0); continue
        ys, xs = np.nonzero(H); W = np.zeros(b.shape, bool); W[max(ys.min() - 60, 0):ys.max() + 61, max(xs.min() - 60, 0):xs.max() + 61] = True
        rel = lambda k: (k >= 1) | (k == -1)
        fl = W & (a == c) & (b != a) & rel(a) & rel(b) & ((a >= 1) | (b >= 1))
        je.append(int(fl.sum())); tot += int(fl.sum()); mit5 += int((fl & ((a == 5) | (b == 5))).sum())
    kp[n] = je
    top = sorted(range(len(je)), key=lambda i: -je[i])[:6]
    print('   %-4s Summe %d (davon mit Kopf-Anhang %d) | groesste: %s' % (n, tot, mit5, ', '.join('F%d %d' % (226 + i, je[i]) for i in top)))
out['kippen'] = kp
json.dump(out, open(L_ + 'szene2.json', 'w'))
