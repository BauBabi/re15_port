#!/usr/bin/env python3
"""Form bemalt, Sichtblatt: grosse Ansichten aus FORM.EMD/TIM wie das Rundumblatt der Pruefung (Gouraud je Ecke, Textur, Malerfolge wie der Port).
python fb_blick.py form.json --clip 2 --aus blick.png [--ton] [--gamma] [--gross 640] [--blicke "az,el;az,el"]
--ton: Textur grau (nur Form und Normalen). Kein RE-Wert."""
import argparse
import os
import sys

import numpy as np

from rf_kern import TOOLS
import form_pruefung as P
import form_buehne as FB

a = argparse.ArgumentParser()
a.add_argument('form')
a.add_argument('--clip', type=int, default=2)
a.add_argument('--aus', default='zw/blick.png')
a.add_argument('--ton', action='store_true')
a.add_argument('--gamma', action='store_true')
a.add_argument('--gross', type=int, default=600)
a.add_argument('--nur', default='')
a.add_argument('--blicke', default='90,0;45,22;0,0;-45,22;-90,0;-135,22;180,0;0,60')
a = a.parse_args()
k = P.Kandidat(a.form)
tex = k.tex
if a.ton:
    tex.lut = tex.lut.copy()
    tex.lut[..., :3] = np.where(tex.lut[..., 3:4] > 0, 112, 0)
kf = k.kf(a.clip, 0)
if a.nur:
    import copy
    keep = {int(x) for x in a.nur.split(',')}
    ms = []
    for i, m in enumerate(k.mo.meshes):
        if i not in keep:
            m = copy.copy(m)
            m.tf, m.qf = [], []
        ms.append(m)
    k.mo.meshes = ms
bilder = []
for b in a.blicke.split(';'):
    az, el = (float(x) for x in b.split(','))
    G = int(a.gross * 1.9)
    bild = np.full((G, G, 3), 92, np.uint8)
    m = FB.rastern(P.rundum_dreiecke(k.mo, kf, az, el, G), tex, bild, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    ys, xs = np.nonzero(m)
    cx, cy = (xs.min() + xs.max()) // 2, (ys.min() + ys.max()) // 2
    hw = max(xs.max() - xs.min(), ys.max() - ys.min()) // 2 + 12
    x0, y0 = max(cx - hw, 0), max(cy - hw, 0)
    import cv2
    bild = cv2.resize(bild[y0:y0 + 2 * hw, x0:x0 + 2 * hw], (a.gross, a.gross), interpolation=cv2.INTER_AREA)
    m = cv2.resize(m[y0:y0 + 2 * hw, x0:x0 + 2 * hw].astype(np.uint8), (a.gross, a.gross), interpolation=cv2.INTER_NEAREST) > 0
    if a.gamma:
        bild[m] = np.clip(np.power(bild[m] / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
    bilder.append(('az %g el %g' % (az, el), bild))
FB.montage(bilder, min(4, len(bilder)), a.aus, '%s Clip %d %s%s' % (os.path.basename(os.path.dirname(os.path.abspath(a.form))), a.clip, 'Ton' if a.ton else 'Textur', ', Gamma 0,5' if a.gamma else ''))
print(a.aus)
