#!/usr/bin/env python3
"""Abnahme 2: Spaltenprofile Original gegen gefiltertes Render, Nahbild 6-fach um Fenster und Raute."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
def Y(a):
    return (.299 * a[..., 0] + .587 * a[..., 1] + .114 * a[..., 2]).astype(np.float32)
o = P.original(1)
g = cv2.imread(os.path.join(HIER, 'bild', 'gef_bild1.png'))[:, :, ::-1]
m = np.load(os.path.join(HIER, 'bild', 'maske_bild1.npy'))
oy, gy = cv2.GaussianBlur(Y(o), (0, 0), .8), cv2.GaussianBlur(Y(g), (0, 0), .8)
print('Zeile:  ' + ' '.join('%4d' % y for y in range(296, 372, 4)))
for x in (292, 296, 300, 345, 350, 355, 360, 365, 370, 375, 380):
    print('x%3d O ' % x + ' '.join('%4d' % oy[y, x] for y in range(296, 372, 4)))
    print('     R ' + ' '.join('%4d' % gy[y, x] for y in range(296, 372, 4)))
    print('     M ' + ' '.join('%4d' % m[y, x] for y in range(296, 372, 4)))
# Farbe Fenster gegen Boden
for name, (x0, y0, x1, y1) in {'Fenster links (343-352,322-345)': (343, 322, 352, 345), 'Fenster rechts (366-378,320-332)': (366, 320, 378, 332), 'Mitte frei (352-366,345-360)': (352, 345, 366, 360), 'Boden rechts des Arms (430-460,330-350)': (430, 330, 460, 350), 'Schatten SCH (346-376,364-400)': (346, 364, 376, 400)}.items():
    c = o[y0:y1, x0:x1].reshape(-1, 3).mean(0)
    d = g[y0:y1, x0:x1].reshape(-1, 3).mean(0)
    print('%-42s Original RGB %3d %3d %3d  Render %3d %3d %3d' % (name, *c, *d))
s = 6
def z(a, box):
    x0, y0, x1, y1 = box
    return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
box = (280, 295, 395, 372)
t = np.hstack([z(o, box), np.full((77 * s, 6, 3), 255, np.uint8), z(g, box)])
cv2.imwrite(os.path.join(HIER, 'bild', 'fenster_nah.png'), t[:, :, ::-1])
hell = np.clip(255 * np.power(np.clip(o.astype(np.float32) / 255 * 3.0, 0, 1), .6), 0, 255).astype(np.uint8)
gh = np.clip(255 * np.power(np.clip(g.astype(np.float32) / 255 * 3.0, 0, 1), .6), 0, 255).astype(np.uint8)
t = np.hstack([z(hell, box), np.full((77 * s, 6, 3), 255, np.uint8), z(gh, box)])
cv2.imwrite(os.path.join(HIER, 'bild', 'fenster_nah_hell.png'), t[:, :, ::-1])
