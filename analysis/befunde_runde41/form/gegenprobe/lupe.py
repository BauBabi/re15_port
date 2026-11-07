#!/usr/bin/env python3
"""Lupe: Ausschnitt eines Bildes vergroessert, Gamma, Gitter in Originalpixeln. python lupe.py nr x0 y0 x1 y1 skala gamma aus.png [nutzer]"""
import sys, os, cv2, numpy as np, glob
F = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
nr, x0, y0, x1, y1, s = [int(a) for a in sys.argv[1:7]]
gam = float(sys.argv[7]); aus = sys.argv[8]; nut = len(sys.argv) > 9
v = cv2.imread(os.path.join(F, 'bild%d.png' % nr)).astype(np.float32) / 255
c = v[y0:y1, x0:x1]
c = np.clip(c, 0, 1) ** gam
g = cv2.resize(c, None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
g = (np.clip(g, 0, 1) * 255).astype(np.uint8)
for x in range((x0 // 20 + 1) * 20, x1, 20):
    X = (x - x0) * s
    cv2.line(g, (X, 0), (X, 6), (255, 255, 255), 1)
    cv2.line(g, (X, g.shape[0] - 7), (X, g.shape[0] - 1), (255, 255, 255), 1)
    cv2.putText(g, str(x), (X + 2, 14), 0, .35, (255, 255, 255), 1, cv2.LINE_AA)
for y in range((y0 // 20 + 1) * 20, y1, 20):
    Y = (y - y0) * s
    cv2.line(g, (0, Y), (6, Y), (255, 255, 255), 1)
    cv2.line(g, (g.shape[1] - 7, Y), (g.shape[1] - 1, Y), (255, 255, 255), 1)
    cv2.putText(g, str(y), (8, Y + 4), 0, .35, (255, 255, 255), 1, cv2.LINE_AA)
if nut:
    for p in glob.glob(os.path.join(F, 'nutzer_teile', 'maske_*.png')):
        m = cv2.imread(p, 0)
        if m.shape[0] != v.shape[0]:
            continue
        name = os.path.basename(p)[6:-4]
        if (nr == 1) != (name in ('tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body')):
            continue
        kk, _ = cv2.findContours((m > 127).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        for k in kk:
            k = ((k.reshape(-1, 2) - [x0, y0]) * s + s // 2).astype(np.int32)
            cv2.polylines(g, [k], True, (0, 255, 255), 1, cv2.LINE_AA)
cv2.imwrite(aus, g)
