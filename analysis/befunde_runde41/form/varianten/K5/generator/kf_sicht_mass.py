"""Sicht-Mass P2 (wie Sicht-Pruefer v2 r1 Abschnitt 2): dunkle duenne Linien im Render (Black-Top-Hat 5x5 > 6 L* auf den Figurpixeln, Erosion 3x3),
Anteil %, davon in elongierten Komponenten (>= 12 px, Elongation >= 3) %, Zahl der elongierten Komponenten. Kopf: Mesh 21-23 Clip 1, 300 px,
3 Winkel wie Tafel 06; G4: Mesh 26 Clip 0."""
import json
import math
import os
import sys
import numpy as np, cv2
G = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, G)
import kf_tafeln as KT
import fl, render as R

def linien(img, msk):
    L = np.zeros(msk.shape, np.float32)
    L[msk] = KT.PL.lab(img[msk].astype(np.float32))[:, 0]
    v = cv2.erode(msk.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    bh = cv2.morphologyEx(L, cv2.MORPH_BLACKHAT, np.ones((5, 5), np.uint8))
    m = (bh > 6) & v
    n, lb, st, _ = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    el_px, el_n = 0, 0
    for i in range(1, n):
        if st[i, 4] < 12:
            continue
        ys, xs = np.nonzero(lb == i)
        ev = np.linalg.eigvalsh(np.cov(np.stack([xs, ys]).astype(float)))
        if math.sqrt(ev[1] / max(ev[0], 0.05)) >= 3:
            el_px += st[i, 4]; el_n += 1
    return round(100 * m.sum() / v.sum(), 1), round(100 * el_px / v.sum(), 1), el_n

out = {}
for p in sys.argv[1:]:
    if p == 'G4':
        mo, c, teile = KT.re2_mo(0x34), 0, (26,)
    else:
        mo, c, teile = fl.lade_var(p), 1, (21, 22, 23)
    r = []
    for wt, az, el in (('3/4 vorn', 60, 10), ('Seite', 0, 5), ('unten vorn', 80, -30)):
        img, msk = KT.blick_teile(mo, R.kf(mo, c), az, el, 300, teile)
        r.append(linien(img, msk))
    k = []
    if p != 'G4':
        for _wt, az, el in R.WINKEL:
            img, msk = R.blick(mo, R.kf(mo, 2), az, el, 380, None)
            k.append(linien(img, msk))
    out[os.path.basename(p)] = dict(kopf=r, koerper=k)
    print(os.path.basename(p), 'Kopf', r, 'Koerper', k, flush=True)
