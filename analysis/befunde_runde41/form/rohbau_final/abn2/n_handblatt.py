#!/usr/bin/env python3
"""Ausschnitt Bild 1 um die rechte Hand: Original, Original aufgehellt, ziel2 (weiss Figur, schwarz nicht Figur, grau offen), Maske right_arm. Faktor 6."""
import os, sys
import cv2, numpy as np
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import *
import form_pruefung as P
HIER2 = os.path.dirname(os.path.abspath(__file__))
def blatt(n, box, aus, s=6, teile=()):
    x0, y0, x1, y1 = box
    o = P.original(n)
    z = cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
    he = np.clip(255 * np.power(np.clip(o.astype(np.float32) / 255 * 2.2, 0, 1), .7), 0, 255).astype(np.uint8)
    def gr(a, st=cv2.INTER_CUBIC):
        return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=st)
    a = gr(o)[:, :, ::-1].copy(); b = gr(he)[:, :, ::-1].copy()
    c = cv2.cvtColor(gr(z, cv2.INTER_NEAREST), cv2.COLOR_GRAY2BGR)
    m, _ = masken()
    for t in teile:
        q = cv2.resize(m[t][y0:y1, x0:x1].astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
        ko, _x = cv2.findContours(q, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        for im in (a, b, c):
            cv2.drawContours(im, ko, -1, (0, 255, 255), 1)
    for im in (a, b, c):
        for x in range(x0 - x0 % 10 + 10, x1, 10):
            cv2.line(im, ((x - x0) * s, 0), ((x - x0) * s, 6), (0, 255, 0), 1)
            cv2.putText(im, str(x), ((x - x0) * s + 2, 16), 0, .35, (0, 255, 0), 1, cv2.LINE_AA)
        for y in range(y0 - y0 % 10 + 10, y1, 10):
            cv2.line(im, (0, (y - y0) * s), (6, (y - y0) * s), (0, 255, 0), 1)
            cv2.putText(im, str(y), (8, (y - y0) * s + 4), 0, .35, (0, 255, 0), 1, cv2.LINE_AA)
    cv2.imwrite(aus, np.hstack([a, b, c]))
blatt(1, (360, 300, 450, 392), os.path.join(HIER2, 'bild', 'hand_b1.png'), 6, ('right_arm',))
blatt(2, (130, 340, 300, 480), os.path.join(HIER2, 'bild', 'hand_b2.png'), 4, ('right_arm_front',))
