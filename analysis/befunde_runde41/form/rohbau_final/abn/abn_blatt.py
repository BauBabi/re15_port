#!/usr/bin/env python3
"""Abnahme 1: Belegblaetter. Maengel markiert auf Original und gefiltertem Render, Spieltafel. Kein RE-Wert."""
import glob, os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose, schreib
from abn_kamton import welt, bildpunkte

def rgb(p):
    return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)

def tafel(a, box, s, marken, titel):
    x0, y0, x1, y1 = box
    c = cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC).copy()
    for (ax, ay, bx, by, f, t) in marken:
        cv2.rectangle(c, ((ax - x0) * s, (ay - y0) * s), ((bx - x0) * s, (by - y0) * s), f, 2)
        cv2.putText(c, t, ((ax - x0) * s + 2, (ay - y0) * s - 4), cv2.FONT_HERSHEY_SIMPLEX, .5, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(c, t, ((ax - x0) * s + 2, (ay - y0) * s - 4), cv2.FONT_HERSHEY_SIMPLEX, .5, f, 1, cv2.LINE_AA)
    return schreib(c, titel)

G, C, Y, M = (120, 255, 120), (90, 230, 255), (255, 240, 60), (255, 120, 255)
m1 = [(340, 284, 381, 321, Y, 'M1 Fenster'), (405, 255, 431, 300, C, 'M2 O1'), (324, 193, 340, 212, G, 'Kerbe')]
m2 = [(143, 350, 205, 445, Y, 'M3 Sporn')]
o1, g1 = P.original(1), rgb(os.path.join(HIER, 'bild', 'gefiltert_bild1.png'))
o2, g2 = P.original(2), rgb(os.path.join(HIER, 'bild', 'gefiltert_bild2.png'))
b1 = (180, 170, 445, 480); b2 = (130, 140, 480, 480)
z1 = np.hstack([tafel(o1, b1, 2, m1, 'Bild 1 Original'), tafel(g1, b1, 2, m1, 'Render gefiltert')])
z2 = np.hstack([tafel(o2, b2, 2, m2, 'Bild 2 Original'), tafel(g2, b2, 2, m2, 'Render gefiltert')])
w = max(z1.shape[1], z2.shape[1])
z1 = cv2.copyMakeBorder(z1, 0, 0, 0, w - z1.shape[1], cv2.BORDER_CONSTANT, value=(20, 20, 24))
z2 = cv2.copyMakeBorder(z2, 0, 0, 0, w - z2.shape[1], cv2.BORDER_CONSTANT, value=(20, 20, 24))
cv2.imwrite(os.path.join(HIER, 'bild', 'maengel_blatt.png'), np.vstack([z1, z2])[:, :, ::-1])

# Spieltafel: alle Laeufe halb gross
bilder = sorted(glob.glob(os.path.join(HIER, 'spiel', '*.png'))) + sorted(glob.glob(os.path.join(HIER, 'spiel2', '*.png')))
kl = []
for p in bilder:
    a = cv2.resize(cv2.imread(p), (320, 240), interpolation=cv2.INTER_AREA)
    cv2.putText(a, os.path.basename(p)[:-4], (5, 14), cv2.FONT_HERSHEY_SIMPLEX, .4, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, os.path.basename(p)[:-4], (5, 14), cv2.FONT_HERSHEY_SIMPLEX, .4, (120, 255, 255), 1, cv2.LINE_AA)
    kl.append(a)
sp = 5
while len(kl) % sp:
    kl.append(np.full((240, 320, 3), 24, np.uint8))
cv2.imwrite(os.path.join(HIER, 'spiel_tafel.png'), np.vstack([np.hstack(kl[i:i + sp]) for i in range(0, len(kl), sp)]))
print(len(bilder), 'Spielbilder')

# Welcher Knochen traegt die hellen Striche in Bild 1, und ist es Zeichnung aus Bild 2?
k = P.Kandidat(K)
n = netz(k)
lage, gier, clip, bild = k.stand(1)
kam = P.kamera(1)
V, F, B = pose(k, n, clip, bild)
px, z = bildpunkte(welt(V, lage, gier), kam)
a = px[F]
m0 = (a[:, 1, 0] - a[:, 0, 0]) * (a[:, 2, 1] - a[:, 0, 1]) - (a[:, 2, 0] - a[:, 0, 0]) * (a[:, 1, 1] - a[:, 0, 1])
for q in ((351, 227), (366, 223)):
    best = None
    for i in np.where(m0 > 0)[0]:
        t = a[i]
        d = m0[i]
        w1 = ((q[0] - t[0, 0]) * (t[2, 1] - t[0, 1]) - (t[2, 0] - t[0, 0]) * (q[1] - t[0, 1])) / d
        w2 = ((t[1, 0] - t[0, 0]) * (q[1] - t[0, 1]) - (q[0] - t[0, 0]) * (t[1, 1] - t[0, 1])) / d
        if w1 >= 0 and w2 >= 0 and w1 + w2 <= 1:
            zz = (1 - w1 - w2) * z[F[i, 0]] + w1 * z[F[i, 1]] + w2 * z[F[i, 2]]
            if best is None or zz < best[0]:
                best = (zz, int(B[i]))
    print('Strich bei', q, 'Knochen', best)
