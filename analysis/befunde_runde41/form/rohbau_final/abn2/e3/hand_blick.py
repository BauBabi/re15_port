#!/usr/bin/env python3
"""Bild 2, rechte Hand: Original (Y gedehnt), Render, Kennbild; Nutzermaske right_arm_front magenta, Render gelb. python hand_blick.py [form.json] [aus.png]"""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
RF = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, os.path.normpath(os.path.join(RF, '..', '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P
fj = sys.argv[1] if len(sys.argv) > 1 else os.path.join(RF, 'form.json')
aus = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HIER, 'hand_blick.png')
X0, Y0, X1, Y1, S = 130, 330, 300, 490, 4
o = P.original(2).astype(np.float32)
Y = o @ P.YK
k = P.Kandidat(fj)
sauber, alb, m, _ = k.rendern(2)
zu = P.teil_zuordnung(k, 2)
zu = zu[0] if isinstance(zu, tuple) else zu
kn = P.teilbild(k, 2, zu)['kn']
ra = P.teil_roh('right_arm_front')
logo = P.logo_maske()
def gross(a):
    return cv2.resize(a[Y0:Y1, X0:X1], ((X1 - X0) * S, (Y1 - Y0) * S), interpolation=cv2.INTER_NEAREST)
def kontur(b, mm, f):
    q = gross(mm.astype(np.uint8))
    cs, _ = cv2.findContours(q, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(b, cs, -1, f, 2)
v = np.clip((Y - 10) / 90.0 * 255, 0, 255).astype(np.uint8)
b1 = cv2.cvtColor(gross(v), cv2.COLOR_GRAY2BGR)
kontur(b1, ra, (255, 0, 255)); kontur(b1, m, (0, 255, 255)); kontur(b1, logo, (0, 140, 0))
rng = np.random.RandomState(3)
pal = rng.randint(60, 255, (40, 3)).astype(np.uint8)
kb = np.zeros(kn.shape + (3,), np.uint8)
kb[kn >= 0] = pal[kn[kn >= 0]]
b3 = gross(kb)
kontur(b3, ra, (255, 0, 255))
for b_ in (b1, b3):
    for x in range(X0, X1, 10):
        cv2.line(b_, ((x - X0) * S, 0), ((x - X0) * S, b_.shape[0]), (0, 90, 0), 1)
        if x % 20 == 0: cv2.putText(b_, str(x), ((x - X0) * S + 2, 12), 0, .4, (0, 255, 0), 1)
    for y in range(Y0, Y1, 10):
        cv2.line(b_, (0, (y - Y0) * S), (b_.shape[1], (y - Y0) * S), (0, 90, 0), 1)
        if y % 20 == 0: cv2.putText(b_, str(y), (2, (y - Y0) * S - 2), 0, .4, (0, 255, 0), 1)
# Knochennummern an Schwerpunkten
for b in np.unique(kn[Y0:Y1, X0:X1]):
    if b < 0: continue
    ys, xs = np.nonzero(kn[Y0:Y1, X0:X1] == b)
    cv2.putText(b3, str(b), (int(xs.mean() * S), int(ys.mean() * S)), 0, .6, (0, 0, 0), 2)
    cv2.putText(b3, str(b), (int(xs.mean() * S), int(ys.mean() * S)), 0, .6, (255, 255, 255), 1)
cv2.imwrite(aus, np.hstack([b1, b3]))
leer = ra & ~m & ~logo
print('Maske ohne Render (ohne Logo): %d px' % leer.sum())
k_, lab, st, _ = cv2.connectedComponentsWithStats(leer.astype(np.uint8))
for i in np.argsort(-st[1:, 4])[:4] + 1:
    print('  Fleck %d px, x %d-%d, y %d-%d, Y-Mittel %.0f' % (st[i, 4], st[i, 0], st[i, 0] + st[i, 2] - 1, st[i, 1], st[i, 1] + st[i, 3] - 1, Y[lab == i].mean()))
