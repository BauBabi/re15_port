#!/usr/bin/env python3
"""Umriss des Renders auf dem Original, aufgehellt, stark vergroessert. Nutzerumriss nur duenn grau. Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
from einsatz import P, schnitt, beschriften

FARBE = dict(A=(255, 90, 90), B=(90, 255, 120), C=(255, 240, 60))
BOXEN = {1: (('figur', (185, 175, 445, 480), 3), ('unten', (185, 320, 350, 480), 5)), 2: (('figur', (135, 140, 480, 480), 3), ('hand', (138, 310, 345, 480), 5), ('arm', (355, 250, 480, 425), 5))}
for n in (1, 2):
    o = P.original(n)
    u = P.nutzer_union(n)
    for t, box, s in BOXEN[n]:
        tafeln = []
        for r in 'ABC':
            m = cv2.imread(os.path.join(H, 'maske_%s_bild%d.png' % (r, n)), 0)
            g = schnitt(o, box, s, ton=(1.8, .75))
            x0, y0, x1, y1 = box
            k = cv2.resize(m[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
            c, _ = cv2.findContours(k, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
            ku = cv2.resize(u[y0:y1, x0:x1].astype(np.uint8) * 255, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
            cu, _ = cv2.findContours(ku, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(g, cu, -1, (150, 150, 150), 1, cv2.LINE_AA)
            cv2.drawContours(g, c, -1, FARBE[r], 1, cv2.LINE_AA)
            tafeln.append(beschriften(g, r + ' Umriss Render, grau Nutzer', FARBE[r]))
        z = np.hstack([cv2.copyMakeBorder(x, 2, 2, 2, 2, cv2.BORDER_CONSTANT, value=(16, 17, 20)) for x in tafeln])
        cv2.imwrite(os.path.join(H, 'kontur_b%d_%s.png' % (n, t)), z[:, :, ::-1])
print('fertig')
