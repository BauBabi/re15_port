#!/usr/bin/env python3
"""Abnahme 1: echtes Fensterbild durch den Videofilter neben das Original. Bild 2: Schnitt 11 ist die Kamera des Screenshots. Bild 1: Cut 1 mit nachgestelltem Blick, Raum anders. Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
import form_video as FV
from abn_bild import schreib, schnitt, speichere, AUS
def rgb(p):
    return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)
s = rgb(os.path.join(HIER, 'spiel', 'p2_r5090_s11.png'))
print(s.shape)
g = FV.video_filter(s, bild=2, entzerren=True)
o = P.original(2)
cv2.imwrite(os.path.join(AUS, 'spiel_gefiltert_bild2.png'), g[:, :, ::-1])
speichere([schreib(o.copy(), 'Original Bild 2'), schreib(g.copy(), 'echtes Spielbild durch Filter')], 'spiel_ganz_bild2.png')
for t, box, f in (('figur', (135, 140, 480, 480), 3),):
    speichere([schreib(schnitt(o, box, f), 'Original'), schreib(schnitt(g, box, f), 'Spiel gefiltert')], 'spiel_nah_b2_%s.png' % t)
    speichere([schreib(schnitt(o, box, f, (2.4, .7)), 'Original hell'), schreib(schnitt(g, box, f, (2.4, .7)), 'Spiel gefiltert hell')], 'spiel_nah_b2_%s_hell.png' % t)
s1 = rgb(os.path.join(HIER, 'spiel2', 'b1blick_pose1.png'))
g1 = FV.video_filter(s1, bild=1, entzerren=False)
o1 = P.original(1)
speichere([schreib(o1.copy(), 'Original Bild 1'), schreib(g1.copy(), 'Spiel Cut 1, Blick nachgestellt, Filter')], 'spiel_ganz_bild1.png')
