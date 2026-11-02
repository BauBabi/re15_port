#!/usr/bin/env python3
"""Abnahme 1: Aussenkante linkes Bein je Zeile. Original, Nutzermaske, Modell. Zeigt Lappen und Kerbe. Kein RE-Wert."""
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
M = P.teil_maske('left_leg')
aus = []
L, Ls = [], []
for y in range(336, 436, 8):
    z = np.where(Y[y, 180:262] < 90)[0]
    a = 180 + z[0] if len(z) else -1
    b = np.where(M[y])[0]
    c = np.where(m[y, 150:300])[0]
    t = 'y %3d  Original dunkel ab x %3d  Nutzermaske ab x %3d  Modell ab x %3d' % (y, a, b[0] if len(b) else -1, 150 + c[0] if len(c) else -1)
    print(t); aus.append(t)
    L.append(150 + c[0] if len(c) else np.nan); Ls.append(a)
L, Ls = np.array(L, float), np.array(Ls, float)
# Abweichung von der Geraden = Lappentiefe
for nm, v in (('Original', Ls), ('Modell', L)):
    x = np.arange(len(v)); ok = np.isfinite(v) & (v > 0)
    p = np.polyfit(x[ok], v[ok], 1)
    r = v[ok] - np.polyval(p, x[ok])
    t = '%s Aussenkante: Abweichung von der Geraden %.1f bis %.1f, Hub %.1f px' % (nm, r.min(), r.max(), r.max() - r.min())
    print(t); aus.append(t)
open(os.path.join(HIER, 'mess', 'lappen.txt'), 'w', encoding='utf-8').write('\n'.join(aus))
