"""Pruefer P5 R2: helle Bildpunkte im Kasten (420,200)-(640,400) F344-F378 (Regel wie szene2: B >= R-6, B > 120, R > 110) nach Klasse
des Markerlaufs (welches Mesh steht dort?) + Kippen je Bild gegen R6/V7 (aus szene2.json)."""
import json, numpy as np
import szene as S
N = ['R3', 'R2', 'V7', 'R6', 'R4']
K = {n: np.load(S.L_ + n + '_kl.npz')['K'] for n in N}
NM = {-1: 'Hintergrund/Leon/Ada', 0: 'Rest(andere Meshes)', 1: 'M8 unten', 2: 'M8 oben', 3: 'Kopf', 4: 'Gesicht', 5: 'Anhang'}
for n in N:
    tot = {}
    for f in range(344, 379):
        im = S.ppm(n, f); k = im[200:400, 420:640]; r, g, b = k[..., 0], k[..., 1], k[..., 2]
        m = (b >= r - 6) & (b > 120) & (r > 110); kl = K[n][f - S.F0][200:400, 420:640][m]
        for c in np.unique(kl): tot[NM[int(c)]] = tot.get(NM[int(c)], 0) + int((kl == c).sum())
    print('%-3s helle px F344-F378 nach Klasse (Summe 35 Bilder):' % n, tot)
sz = json.load(open(S.L_ + 'szene2.json'))['kippen']
ex = set(range(313, 316))
for n in N:
    a = sz[n]; print('Kippen %s ohne F313-F315: %d' % (n, sum(v for i, v in enumerate(a) if 226 + i not in ex)))
for kn in ('R3', 'R2'):
  for ref in ('R6', 'V7', 'R2'):
    if ref == kn: continue
    d = [(226 + i, sz[kn][i], sz[ref][i]) for i in range(len(sz[kn])) if sz[kn][i] > sz[ref][i] + 100 and 226 + i not in ex]
    print('Bilder %s > %s + 100:' % (kn, ref), d)
