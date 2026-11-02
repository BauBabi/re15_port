#!/usr/bin/env python3
"""Dunkle Keile in Bild 1 ausserhalb der Nutzerteile und Dunkelzone Bild 2: wieviel deckt das Modell. Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
from einsatz import P

def anteil(n, box, name, schwelle=110):
    y = cv2.GaussianBlur(P.original(n).astype(np.float32) @ P.YK, (0, 0), 1)
    u = P.nutzer_union(n)
    logo = cv2.dilate(P.logo_maske().astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    x0, y0, x1, y1 = box
    w = np.zeros(y.shape, bool)
    w[y0:y1, x0:x1] = True
    w &= ~u & ~logo & (y < schwelle)
    z = []
    for r in 'ABC':
        m = cv2.imread(os.path.join(H, 'maske_%s_bild%d.png' % (r, n)), 0) > 0
        z.append('%s %d von %d (%.0f %%)' % (r, (m & w).sum(), w.sum(), 100 * (m & w).sum() / max(1, w.sum())))
    print(name, '  '.join(z))

anteil(1, (215, 345, 258, 448), 'Keil links Bild 1 (dunkel, in keinem Teil)')
anteil(1, (283, 360, 305, 435), 'Keil rechts Bild 1')
anteil(2, (300, 300, 400, 385), 'Dunkelzone Bild 2 zwischen den Armen', 90)
