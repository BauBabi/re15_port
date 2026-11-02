#!/usr/bin/env python3
"""Abnahme 2: Oberkante rechts Bild 1, Zeilen- und Spaltenprofile, Nahbild ohne Masken."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
def Y(a):
    return (.299 * a[..., 0] + .587 * a[..., 1] + .114 * a[..., 2]).astype(np.float32)
o = P.original(1)
g = cv2.imread(os.path.join(HIER, 'bild', 'gef_bild1.png'))[:, :, ::-1]
m = np.load(os.path.join(HIER, 'bild', 'maske_bild1.npy'))
oy, gy = cv2.GaussianBlur(Y(o), (0, 0), .8), cv2.GaussianBlur(Y(g), (0, 0), .8)
print('Spalten (y aufsteigend), O Original, R Render, Modell-Oberkante je Spalte')
ys = list(range(186, 262, 4))
print('Zeile  ' + ' '.join('%4d' % y for y in ys))
for x in (335, 345, 355, 365, 375, 385, 395, 405, 415, 425):
    top = np.where(m[:, x])[0]
    print('x%3d O ' % x + ' '.join('%4d' % oy[y, x] for y in ys))
    print('     R ' + ' '.join('%4d' % gy[y, x] for y in ys) + '   Modell ab y=%d' % (top.min() if len(top) else -1))
for name, (x0, y0, x1, y1) in {'Band ueber Armkuppe (380-405,232-239)': (380, 232, 405, 239), 'Band ueber Flanke (352-372,205-216)': (352, 205, 372, 216), 'Arm innen (385-400,250-270)': (385, 250, 400, 270), 'Wand hinter Arm (432-460,236-250)': (432, 236, 460, 250), 'Lampe (372-398,205-226)': (372, 205, 398, 226)}.items():
    print('%-42s Original RGB %3d %3d %3d  Render %3d %3d %3d' % (name, *o[y0:y1, x0:x1].reshape(-1, 3).mean(0), *g[y0:y1, x0:x1].reshape(-1, 3).mean(0)))
s = 5
box = (325, 180, 445, 300)
def z(a):
    x0, y0, x1, y1 = box
    return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
a = z(o); b = z(g)
ko, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
a2 = a.copy()
for c in ko:
    c = ((c - np.array([[box[0], box[1]]])) * s + s // 2).astype(np.int32)
    cv2.polylines(a2, [c], True, (0, 255, 0), 1)
cv2.imwrite(os.path.join(HIER, 'bild', 'oberkante_nah.png'), np.hstack([a, np.full((a.shape[0], 6, 3), 255, np.uint8), a2, np.full((a.shape[0], 6, 3), 255, np.uint8), b])[:, :, ::-1])
