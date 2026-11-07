#!/usr/bin/env python3
"""Abnahme 2: Tafel der Spielbilder, dazu Helligkeitsprobe je Bild."""
import cv2, os
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
Z = [['p2_r5090_s11', 'p2_dreh1', 'p2_dreh2', 'p2_dreh3'], ['ruhe_g0', 'ruhe_g1024', 'ruhe_g2048', 'ruhe_g3072'], ['p1_g0', 'p1_g1024', 'p1_g2048', 'p1_g3072'], ['p1_nachstellung', 'p1_r5080_s1', 'ruhe_nachstellung']]
gesamt = []
for zeile in Z:
    r = []
    for n in zeile:
        a = cv2.imread(os.path.join(HIER, 'spiel', n + '.png'))
        print(n, a.shape, round(float(a.mean()), 1))
        a = a.copy()
        cv2.putText(a, n, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .55, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(a, n, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .55, (120, 255, 255), 1, cv2.LINE_AA)
        r.append(a)
    while len(r) < 4:
        r.append(np.zeros_like(r[0]))
    gesamt.append(np.hstack(r))
cv2.imwrite(os.path.join(HIER, 'spiel', 'tafel.png'), np.vstack(gesamt))
