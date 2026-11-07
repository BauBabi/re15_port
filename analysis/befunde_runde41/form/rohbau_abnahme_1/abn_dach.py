#!/usr/bin/env python3
"""Abnahme 1: Oberkante Bild 1 je Spalte, Original gegen Modell. Unabhaengig von den Nutzermasken. Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
o = P.original(1).astype(np.float32)
Y = cv2.GaussianBlur(o @ np.array([.299, .587, .114]), (0, 0), 1.0)
m = cv2.imread(os.path.join(HIER, 'bild', 'maske_bild1.png'), 0) > 0
print('Spalte: erste Zeile unter y=175 mit Y<70 im Original (stabil 6 px), oberste Modellzeile, Differenz')
z = []
for x in range(300, 436, 5):
    sp = Y[175:330, x] < 70
    y0 = None
    for i in range(len(sp) - 6):
        if sp[i:i + 6].all():
            y0 = 175 + i
            break
    ym = np.where(m[175:330, x])[0]
    ym = 175 + ym[0] if len(ym) else None
    z.append((x, y0, ym))
    print('x %3d  Original %s  Modell %s  %s' % (x, y0, ym, '' if y0 is None or ym is None else '%+d' % (ym - y0)))
# Zeilen rechts: letzte dunkle Spalte links der Wand je Zeile, Original gegen Modell
print('Zeile: rechte Kante der dunklen Masse, Original gegen Modell (x 380-445)')
for y in range(236, 306, 6):
    z = Y[y, 380:446] < 70
    xs = np.where(z)[0]
    # zusammenhaengend von links: bis zur ersten Luecke ueber 4 px
    r = None
    if len(xs):
        r = xs[0]
        for a in xs:
            if a - r <= 4:
                r = a
            else:
                break
        r += 380
    mx = np.where(m[y, 380:446])[0]
    print('y %3d  Original %s  Modell %s' % (y, r, (380 + mx[-1]) if len(mx) else None))
