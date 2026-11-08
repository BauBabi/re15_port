"""Zahlen-Pruefer V4 r2: Spiellauf sfv4/r/V4r2 - Kettengelenke (debug.log, Welt) in die Spielkamera 50A0/12 projizieren und mit
derselben Abbildung wie b2 (BB.abbilden, Homographie M) ins Videobild 640x480 legen; Abstand der Gelenke 3 (Biegung), 5 (Knick),
7 (Ende vor dem Dorn) zu den Video-Punkten (Biegung = Nutzer-Beginn + Messblatt-Relativmass, Knick/Ende = Nutzer-Linienenden), in H2 = 253.
Ausserdem dieselben Gelenke der Ruhe-Pose (Keyframe ohne Feder) in posen_v4 "85" als Bezug."""
import sys, os, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
from pz2_lauf import K, B
r, kam, M, li = b2.raum()
def video(P):
    q = np.asarray(P, float) @ (kam.V.astype(float) / 4096.0).T + kam.t.astype(float)
    x = BB.OFX + q[:, 0] * kam.h / q[:, 2]; y = BB.OFY + q[:, 1] * kam.h / q[:, 2]
    X, Y = BB.abbilden(x + BB.PHASE, y + BB.PHASE, M)
    return np.stack([X, Y], 1)
ZIEL = {3: (357 - 0.245 * 253, 243 + 0.036 * 253), 5: (290, 308), 7: (295, 343)}
H2 = 253.0
print('h    clip bild gier | Gelenk3 (Biegung) dx/dy H | Gelenk5 (Knick) | Gelenk7 (Ende) | max')
zeilen = []
for h in sorted(K):
    if h < 80: continue
    p = video(K[h]); b = B.get(h, {})
    d = {j: ((p[j][0] - ZIEL[j][0]) / H2, (p[j][1] - ZIEL[j][1]) / H2) for j in ZIEL}
    mx = max(np.hypot(*v) for v in d.values())
    zeilen.append((h, mx))
    if h in (80, 85, 90, 95, 100, 105, 110, 115, 118, 120, 125, 130, 135):
        print('h%3d %2s %3s %4s | %+.3f %+.3f | %+.3f %+.3f | %+.3f %+.3f | %.3f  Punkte %s' % (h, b.get('clip'), b.get('bild'), b.get('gier'),
              *d[3], *d[5], *d[7], mx, np.round(p[[3, 5, 7]]).astype(int).tolist()))
z = np.array(zeilen)
print('Takte h80-h135 mit allen drei Gelenken <= 0,06 H (Betrag):', [int(h) for h, m in zeilen if m <= 0.06])
