"""Zahlen-Pruefer: Schwanzkette des Laufs (debug.log, Weltkoordinaten) in die FRAMEDUMP-Bilder projizieren (Port-Schirm 320x240 x3)."""
import re, sys, os, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
from pz_lauf import K, B
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
r, kam, M, li = b2.raum()
def schirm(P):
    q = np.asarray(P, float) @ (kam.V.astype(float) / 4096.0).T + kam.t.astype(float)
    return np.stack([BB.OFX + q[:, 0] * kam.h / q[:, 2], BB.OFY + q[:, 1] * kam.h / q[:, 2]], 1)
kacheln = []
for h in (20, 40, 85, 90, 100, 110, 118, 125, 135):
    F = B[h]['F']
    im = cv2.imread(S + 'r/V4/fd/f%06d.ppm' % F)
    p = schirm(K[h]) * 3
    for i in range(len(p) - 1):
        cv2.line(im, tuple(np.round(p[i]).astype(int)), tuple(np.round(p[i + 1]).astype(int)), (0, 255, 255), 1)
    for i, q in enumerate(p):
        cv2.circle(im, tuple(np.round(q).astype(int)), 3, (0, 0, 255) if i in (3, 5) else (0, 255, 0), -1)
    x0 = int(max(0, p[:, 0].min() - 120)); y0 = int(max(0, p[:, 1].min() - 120))
    crop = im[y0:y0 + 420, x0:x0 + 420]
    crop = cv2.copyMakeBorder(crop, 0, 420 - crop.shape[0], 0, 420 - crop.shape[1], cv2.BORDER_CONSTANT)
    cv2.putText(crop, 'h%d F%d clip%d' % (h, F, B[h]['clip']), (5, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
    kacheln.append(crop)
    print('h', h, 'Schirm x3 Punkte:', np.round(p).astype(int).tolist())
z = [np.hstack(kacheln[i:i + 3]) for i in range(0, 9, 3)]
cv2.imwrite(S + 'pz/lauf_kette.png', np.vstack(z))
