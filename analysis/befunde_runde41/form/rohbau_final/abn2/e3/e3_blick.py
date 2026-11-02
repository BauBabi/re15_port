#!/usr/bin/env python3
"""E3/E1: Ausschnitt Bild 1 um die Kerbe, Original verstaerkt, Ziel, Nutzermaske right_arm, Renderumriss. python e3_blick.py [form.json] [aus.png]"""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
RF = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(RF, '..', '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
fj = sys.argv[1] if len(sys.argv) > 1 else os.path.join(RF, 'form.json')
aus = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HIER, 'e3_blick.png')
X0, Y0, X1, Y1, S = 370, 228, 450, 318, 8
o = P.original(1).astype(np.float32)
Y = o @ P.YK
k = P.Kandidat(fj)
sauber, alb, m, _ = k.rendern(1)
z = cv2.imread(os.path.join(P.FORM, 'ziel2_bild1.png'), 0)
ra = P.teil_roh('right_arm')
def gross(a):
    return cv2.resize(a[Y0:Y1, X0:X1], ((X1 - X0) * S, (Y1 - Y0) * S), interpolation=cv2.INTER_NEAREST)
def kontur(b, mm, f):
    q = gross(mm.astype(np.uint8))
    cs, _ = cv2.findContours(q, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(b, cs, -1, f, 2)
# Original: Y gedehnt 15..80
v = np.clip((Y - 15) / 65.0 * 255, 0, 255).astype(np.uint8)
b1 = cv2.cvtColor(gross(v), cv2.COLOR_GRAY2BGR)
kontur(b1, ra, (255, 0, 255))
kontur(b1, m, (0, 255, 255))
b2 = gross(sauber[:, :, ::-1].copy())
kontur(b2, ra, (255, 0, 255))
zz = cv2.cvtColor(gross(z), cv2.COLOR_GRAY2BGR)
kontur(zz, m, (0, 200, 255))
for b in (b1, b2, zz):
    for x in range(X0, X1, 10):
        cv2.line(b, ((x - X0) * S, 0), ((x - X0) * S, b.shape[0]), (0, 120, 0), 1)
        cv2.putText(b, str(x), ((x - X0) * S + 2, 12), 0, .4, (0, 255, 0), 1)
    for y in range(Y0 - Y0 % 10 + 10, Y1, 10):
        cv2.line(b, (0, (y - Y0) * S), (b.shape[1], (y - Y0) * S), (0, 120, 0), 1)
        cv2.putText(b, str(y), (2, (y - Y0) * S - 2), 0, .4, (0, 255, 0), 1)
    cv2.rectangle(b, ((424 - X0) * S, (279 - Y0) * S), ((432 - X0) * S, (288 - Y0) * S), (0, 0, 255), 1)
cv2.imwrite(aus, np.hstack([b1, b2, zz]))
# Zahlen: Spalten-Profil Y in Zeilen 270..296 fuer x 410..436
for y in range(268, 298, 2):
    print(y, ' '.join('%3d' % Y[y, x] for x in range(410, 438, 2)), ' | render', ''.join('#' if m[y, x] else '.' for x in range(410, 438, 2)), ' ziel', ''.join({255: 'W', 128: 'g', 0: '.'}.get(int(z[y, x]), '?') for x in range(410, 438, 2)))
