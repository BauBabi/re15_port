#!/usr/bin/env python3
"""Mittlere Farbe im Kern der Teile: Original gegen Auflage. Kern = Maske minus 6 px, ohne Logo. Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
from einsatz import P
T = {1: ('tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'), 2: ('face_and_upper_body', 'left_arm_front', 'right_arm_front')}
EXTRA = {1: {'arm_zylinder': (388, 302, 412, 352), 'schwanz_mitte': (258, 300, 286, 370)}, 2: {'kopf_innen': (290, 225, 340, 280), 'hand_mitte': (175, 395, 265, 430), 'armwand': (380, 300, 405, 345)}}
for n in (1, 2):
    o = P.original(n).astype(np.float32)
    logo = cv2.dilate(P.logo_maske().astype(np.uint8), np.ones((7, 7), np.uint8)) > 0
    bilder = [('Orig', o)] + [(r, cv2.imread(os.path.join(H, 'auflage_%s_bild%d.png' % (r, n)))[:, :, ::-1].astype(np.float32)) for r in 'ABC']
    print('Bild', n)
    for t in T[n]:
        w = P.enge(P.teil_roh(t), 6) & ~logo
        print('  %-20s' % t, '  '.join('%s %3d %3d %3d' % (nm, *b[w].mean(0)) for nm, b in bilder))
    for t, (x0, y0, x1, y1) in EXTRA[n].items():
        print('  %-20s' % t, '  '.join('%s %3d %3d %3d' % (nm, *b[y0:y1, x0:x1].reshape(-1, 3).mean(0)) for nm, b in bilder))
