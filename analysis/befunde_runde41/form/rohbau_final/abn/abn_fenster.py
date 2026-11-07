#!/usr/bin/env python3
"""Abnahme 1: Formabweichung ohne Nutzermasken. A = Original dunkel, Render hell (Modell fehlt). B = Original hell, Render dunkel (Modell zu viel). Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
LUM = np.array([.299, .587, .114])
for n in (1, 2):
    o = P.original(n).astype(np.float32)
    Y = cv2.GaussianBlur(o @ LUM, (0, 0), 1.2)
    gef = cv2.cvtColor(cv2.imread(os.path.join(HIER, 'bild', 'gefiltert_bild%d.png' % n)), cv2.COLOR_BGR2RGB).astype(np.float32)
    Yg = cv2.GaussianBlur(gef @ LUM, (0, 0), 1.2)
    m = cv2.imread(os.path.join(HIER, 'bild', 'maske_bild%d.png' % n), 0) > 0
    if n == 1:
        Ad, Ah, Bd, Bh = 70, 130, 70, 130
    else:                                   # Raum heller Boden, dunkle Tuer; Feuer und Frau stehen nur im Original
        Ad, Ah, Bd, Bh = 60, 110, 60, 110
    A = (Y < Ad) & (Yg > Ah)
    B = (Y > Bh) & (Yg < Bd)
    logo = P.logo_maske()
    A &= ~logo; B &= ~logo
    box = np.zeros(A.shape, bool)
    box[140:480, 130:480] = True
    print('Bild', n)
    karte = (0.45 * o).astype(np.uint8)
    for name, Q, f in (('A Modell fehlt (Original dunkel, Render hell)', A & box, (0, 255, 255)), ('B Modell zu viel oder Raum anders (Original hell, Render dunkel)', B & box, (255, 60, 60))):
        Q = cv2.morphologyEx(Q.astype(np.uint8), cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
        k, lab, st, ze = cv2.connectedComponentsWithStats(Q, connectivity=8)
        ord_ = np.argsort(-st[1:, cv2.CC_STAT_AREA]) + 1
        print(' ', name, 'Summe', int(Q.sum()))
        for i in ord_[:10]:
            x, y, w, h, a = st[i]
            if a < 25: continue
            innen = float((m & (lab == i)).sum()) / a
            print('    %5d px  x %3d-%3d  y %3d-%3d  Mitte (%3d,%3d)  im Modell %.2f' % (a, x, x + w, y, y + h, ze[i][0], ze[i][1], innen))
        karte[Q > 0] = f
    k2 = cv2.resize(karte[140:480, 130:480], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
    cv2.imwrite(os.path.join(HIER, 'bild', 'fenster_bild%d.png' % n), k2[:, :, ::-1])
