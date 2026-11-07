#!/usr/bin/env python3
"""Abnahme 2: Maengelblatt auf einen Blick."""
import cv2, os
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
B = os.path.join(HIER, 'bild'); N = os.path.join(HIER, 'netz'); S = os.path.join(HIER, 'spiel')
def lies(p):
    a = cv2.imread(p); assert a is not None, p; return a
def fit(a, w, h):
    s = min(w / a.shape[1], h / a.shape[0])
    r = cv2.resize(a, None, fx=s, fy=s, interpolation=cv2.INTER_AREA)
    t = np.full((h, w, 3), 24, np.uint8)
    y, x = (h - r.shape[0]) // 2, (w - r.shape[1]) // 2
    t[y:y + r.shape[0], x:x + r.shape[1]] = r
    return t
def tx(a, s, f=(120, 255, 255)):
    cv2.putText(a, s, (6, 19), cv2.FONT_HERSHEY_SIMPLEX, .44, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, s, (6, 19), cv2.FONT_HERSHEY_SIMPLEX, .44, f, 1, cv2.LINE_AA)
    return a
def spalte(b, x0, x1):
    return b[:, x0:x1]
W, H = 480, 400
g1 = lies(os.path.join(B, 'ganz_bild1.png')); g2 = lies(os.path.join(B, 'ganz_bild2.png'))
sp = lies(os.path.join(S, 'hell_tafel.png'))
nw = lies(os.path.join(B, 'nwurz0.png'))
pv = lies(os.path.join(B, 'probe_vergleich.png'))
nf = lies(os.path.join(B, 'nfuss0.png')); nf2 = lies(os.path.join(B, 'nfuss2.png')); nh = lies(os.path.join(B, 'nhand0.png'))
kt = lies(os.path.join(B, 'kamton_bild2.png'))
tr = [tx(fit(g1, W * 2, 360), 'Bild 1: Original links, Render gefiltert rechts'), tx(fit(g2, W * 2, 360), 'Bild 2: Original links, Render gefiltert rechts')]
z1 = np.hstack(tr)
a = tx(fit(nw[560:1120, 0:560], W, H), 'M1 Ton Pose 1 az 270: Schwanzwurzel abgesaegt')
b = tx(fit(nw[0:560, 560:1120], W, H), 'M1 Ton Pose 1, az 45: Deckel 0,68 m')
c = tx(fit(sp[0:420, 0:640], W, H), 'M1 echtes Spiel, aufgehellt: Stufe in der Silhouette')
d = tx(fit(np.vstack([pv[110:370, 0:560], pv[590:850, 0:560]]), W, H), 'M1 Weg: oben vorher, unten Probe, 1 Ecke versetzt')
z2 = np.hstack([a, b, c, d])
e = tx(fit(nf[:, 560:1120], W, H), 'M2 Ton Pose 1 az 180: Deckel 0,61 m, Blick wie Bild 1')
f = tx(fit(nf2[:, 1120:1680], W, H), 'M2 Ton Ruhe az 225: Hinterbein als Klotzstapel')
g = tx(fit(nh[:, 1120:1680], W, H), 'M3 Ton Pose 1 az 270: Hand auf der Kante')
h = tx(fit(lies(os.path.join(B, 'ton_ruhe_el35.png'))[0:560, 0:560], W, H), 'M4 Ton Ruhe von vorn oben: Haende 1,5 zu 1')
z3 = np.hstack([e, f, g, h])
w = max(z1.shape[1], z2.shape[1])
z1 = cv2.copyMakeBorder(z1, 0, 0, 0, w - z1.shape[1], cv2.BORDER_CONSTANT, value=(24, 24, 24))
cv2.imwrite(os.path.join(B, 'maengel_blatt.png'), np.vstack([z1, z2, z3]))
print('ok')
