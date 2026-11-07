#!/usr/bin/env python3
"""Abnahme 1: Was fehlt je Teil, als Flecken statt als Mittelwert. Dazu Farbproben im Achselfenster gegen Raum und Figur. Kein RE-Wert."""
import os, sys, json
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
LUM = np.array([.299, .587, .114])
logo = P.logo_maske()
lage = P.teile_lage()
aus = []
def sag(t=''):
    print(t); aus.append(t)
for n in (1, 2):
    m = cv2.imread(os.path.join(HIER, 'bild', 'maske_bild%d.png' % n), 0) > 0
    o = P.original(n).astype(np.float32)
    Y = cv2.GaussianBlur(o @ LUM, (0, 0), 1.0)
    sag('Bild %d: Maskenpixel ohne jedes Modell, Flecken ab 40 px, Y = Mittel im Original' % n)
    for name in sorted(lage):
        if lage[name]['bild'] != n:
            continue
        M = P.teil_maske(name) & ~logo
        R = (M & ~m).astype(np.uint8)
        k, lab, st, ze = cv2.connectedComponentsWithStats(R, connectivity=8)
        for i in np.argsort(-st[1:, cv2.CC_STAT_AREA]) + 1:
            x, y, w, h, a = st[i]
            if a < 40:
                continue
            sag('  %-20s %4d px  x %3d-%3d  y %3d-%3d  Mitte (%3d,%3d)  Y %3.0f' % (name, a, x, x + w, y, y + h, ze[i][0], ze[i][1], Y[lab == i].mean()))
# Farbproben Bild 1
o = P.original(1).astype(np.float32)
def p(t, x0, y0, x1, y1):
    c = o[y0:y1, x0:x1].reshape(-1, 3)
    sag('  %-34s RGB %3.0f %3.0f %3.0f  Y %3.0f' % (t, *c.mean(0), (c @ LUM).mean()))
sag('Farbproben Bild 1')
p('Fenster links (352-362,304-314)', 352, 304, 362, 314)
p('Fenster mitte (362-372,302-312)', 362, 302, 372, 312)
p('Fenster rechts (372-380,304-314)', 372, 304, 380, 314)
p('Band im Fenster (364-376,288-298)', 364, 288, 376, 298)
p('Figur Arm (390-410,310-340)', 390, 310, 410, 340)
p('Figur Schwanz (262-280,280-320)', 262, 280, 280, 320)
p('Raum rechts vom Arm, gleiche Zeilen', 432, 304, 470, 314)
p('Raum rechts, Band y 286-296', 440, 286, 490, 296)
p('Raum links, gleiche Zeilen', 150, 304, 215, 314)
p('Boden im Spalt (360-378,330-345)', 360, 330, 378, 345)
with open(os.path.join(HIER, 'mess', 'teilrest.txt'), 'w', encoding='utf-8') as f:
    f.write('\n'.join(aus))
