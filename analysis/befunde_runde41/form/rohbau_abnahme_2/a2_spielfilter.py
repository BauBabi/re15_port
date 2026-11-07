#!/usr/bin/env python3
"""Abnahme 2: Spielbild durch video_filter neben das Original. Kein Einsetzen."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
import form_video as FV
import inspect
print(inspect.signature(FV.video_filter))
for name, n in (('p2_r5090_s11', 2), ('p1_nachstellung', 1)):
    a = cv2.imread(os.path.join(HIER, 'spiel', name + '.png'))[:, :, ::-1]
    g = FV.video_filter(a.copy(), bild=n, entzerren=(n == 2))
    o = P.original(n)
    t = np.hstack([o, g])
    for tx, xy in (('Original Bild %d' % n, (6, 18)), ('Spiel %s durch video_filter' % name, (646, 18))):
        cv2.putText(t, tx, xy, cv2.FONT_HERSHEY_SIMPLEX, .55, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(t, tx, xy, cv2.FONT_HERSHEY_SIMPLEX, .55, (255, 255, 90), 1, cv2.LINE_AA)
    cv2.imwrite(os.path.join(HIER, 'bild', 'spiel_neben_%d.png' % n), t[:, :, ::-1])
print('ok')
