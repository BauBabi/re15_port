#!/usr/bin/env python3
"""Abnahme 1: Luecken und Dachlinie Bild 1 direkt am Bild messen. Figurmaske Original = dunkel gegen Platte, nur als Anhalt, Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
o = P.original(1).astype(np.float32)
Y = o @ np.array([.299, .587, .114])
m = cv2.imread(os.path.join(HIER, 'bild', 'maske_bild1.png'), 0) > 0
gef = cv2.cvtColor(cv2.imread(os.path.join(HIER, 'bild', 'gefiltert_bild1.png')), cv2.COLOR_BGR2RGB).astype(np.float32)
Yg = gef @ np.array([.299, .587, .114])
print('Spaltenprofile Bild 1: Y Original / Y gefiltert / Maske, von oben nach unten')
for x in (362, 368, 374, 380):
    print('x=%d' % x)
    for y in range(256, 336, 6):
        print('  y=%3d  Y %5.0f  Yg %5.0f  m %d' % (y, Y[y-1:y+2, x-1:x+2].mean(), Yg[y-1:y+2, x-1:x+2].mean(), m[y, x]))
print('Zeilenprofile Unterkante Raute: x-Spannen Maske frei')
for y in (340, 346, 352, 358, 364, 370, 376):
    z = ~m[y, 270:335]
    xs = np.where(z)[0] + 270
    hell = np.where(Y[y-1:y+2, 270:335].mean(0) > 150)[0] + 270
    print('  y=%3d  Modell frei %s  Original hell(>150) %s' % (y, (xs.min(), xs.max(), len(xs)) if len(xs) else '-', (hell.min(), hell.max(), len(hell)) if len(hell) else '-'))
