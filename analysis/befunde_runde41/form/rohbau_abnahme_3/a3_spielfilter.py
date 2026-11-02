#!/usr/bin/env python3
"""Spielbild (echtes Fenster) durch form_video.video_filter neben das Original."""
import os, sys
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_video as FV
for spiel, orig, aus in (('p2_s11.png', 'bild2.png', 'spielfilter_bild2.png'), ('p1_nach_r5080.png', 'bild1.png', 'spielfilter_bild1.png')):
    s = cv2.imread(os.path.join(H, 'spiel', spiel))[..., ::-1].copy()
    f = FV.video_filter(s)[..., ::-1]
    o = cv2.imread(os.path.join(F, orig))
    cv2.imwrite(os.path.join(H, 'bild', aus), np.hstack([o, f]))
print('ok')
