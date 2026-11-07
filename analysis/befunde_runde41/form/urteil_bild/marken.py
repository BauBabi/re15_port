#!/usr/bin/env python3
"""Groesse heller Marken und Helligkeit laengs des Unterarms, Original gegen Auflage. Beleg zum Bildurteil, kein RE-Wert."""
import os, sys
import cv2
import numpy as np
from scipy import ndimage as ndi
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
from einsatz import P

def lade(n):
    d = [('Orig', P.original(n))]
    for r in 'ABC':
        d.append((r, cv2.imread(os.path.join(H, 'auflage_%s_bild%d.png' % (r, n)))[:, :, ::-1]))
    return d

def marke(n, name, box, schwelle):
    x0, y0, x1, y1 = box
    z = []
    for nm, b in lade(n):
        y = cv2.GaussianBlur(b.astype(np.float32) @ P.YK, (0, 0), 1)[y0:y1, x0:x1]
        m = y >= schwelle
        lab, k = ndi.label(m)
        if k == 0:
            z.append('%s -' % nm)
            continue
        g = ndi.sum(m, lab, range(1, k + 1))
        i = int(np.argmax(g)) + 1
        yy, xx = np.nonzero(lab == i)
        z.append('%s %d px, %dx%d, x%d-%d y%d-%d' % (nm, g[i - 1], xx.max() - xx.min() + 1, yy.max() - yy.min() + 1, xx.min() + x0, xx.max() + x0, yy.min() + y0, yy.max() + y0))
    print('%-6s Y>=%d  ' % (name, schwelle), ' | '.join(z))

marke(1, 'Z5', (372, 262, 428, 318), 110)
marke(1, 'Z1', (298, 212, 348, 272), 110)
marke(1, 'Z3/4', (296, 278, 350, 316), 130)
marke(1, 'X', (240, 415, 298, 480), 130)

print('--- Unterarm Bild 2: Streifen um die Linie (333,322)-(268,392), Breite 30 px, Anteil der Laenge mit Y>=70 und R-Mittel')
a, b = np.array([333., 322.]), np.array([268., 392.])
L = np.linalg.norm(b - a)
d = (b - a) / L
q = np.array([-d[1], d[0]])
for nm, im in lade(2):
    im = im.astype(np.float32)
    y = cv2.GaussianBlur(im @ P.YK, (0, 0), 1)
    r = cv2.GaussianBlur(im[..., 0], (0, 0), 1)
    t = np.arange(0, L, 1.0)
    pr = []
    prr = []
    for s in t:
        p = a + d * s
        vs = [p + q * o for o in np.arange(-15, 15.5, 1)]
        ys = [y[int(v[1]), int(v[0])] for v in vs]
        rs = [r[int(v[1]), int(v[0])] for v in vs]
        pr.append(max(ys))
        prr.append(np.mean(rs))
    pr = np.array(pr)
    print('%-5s Laenge mit Y>=70: %.0f %%, obere Haelfte %.0f %%, untere Haelfte %.0f %%, Rot-Mittel %.0f' % (nm, 100 * (pr >= 70).mean(), 100 * (pr[:len(pr) // 2] >= 70).mean(), 100 * (pr[len(pr) // 2:] >= 70).mean(), np.mean(prr)))
