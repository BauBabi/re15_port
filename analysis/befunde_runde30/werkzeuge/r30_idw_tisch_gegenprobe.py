# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: GEGENPROBE der Tischhoehe ueber ein EINZELMERKMAL.

Das gemalte Klemmbrett (heller Block) ist in Cut 2 UND Cut 6 zu sehen. Sein
Helligkeits-Schwerpunkt wird in beiden Bildern bestimmt, beide Sehstrahlen werden
geschnitten (kleinste Quadrate). Unabhaengig vom NCC-Verfahren in r30_idw_tischhoehe.py:
dort Flaechen-Korrelation ueber die ganze Platte, hier zwei Strahlen auf EIN Merkmal.

Die Schwelle wird NICHT gewaehlt, sondern durchgefahren; ausgegeben wird die Hoehe je
Schwelle, damit man sieht, ob das Ergebnis an der Schwelle haengt.
"""
import os, sys
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_idw_geom import lade_kameras, strahl, trianguliere, projiziere, AUS


def schwerpunkt(A, box, schwelle):
    x0, x1, y0, y1 = box
    L = A[y0:y1, x0:x1].astype(float).sum(axis=2) / 3.0
    m = L >= schwelle
    if m.sum() < 4:
        return None
    ys, xs = np.nonzero(m)
    w = L[m]
    return (x0 + (xs * w).sum() / w.sum() + 0.5, y0 + (ys * w).sum() / w.sum() + 0.5, int(m.sum()),
            x0 + xs.min(), x0 + xs.max(), y0 + ys.min(), y0 + ys.max())


def main():
    cams = lade_kameras()
    A2 = np.array(Image.open(os.path.join(AUS, 'bg', 'ROOM11502.png')).convert('RGB'))
    A6 = np.array(Image.open(os.path.join(AUS, 'bg', 'ROOM11506.png')).convert('RGB'))
    # Fenster: um die Projektion des NCC-Punkts (Klemmbrettmitte B) herum
    b2 = (144, 162, 118, 132)
    b6 = (170, 225, 95, 150)
    for box, A, n in ((b2, A2, 'Cut 2'), (b6, A6, 'Cut 6')):
        L = A[box[2]:box[3], box[0]:box[1]].astype(float).sum(axis=2) / 3.0
        print('%s Fenster x %d..%d y %d..%d: Helligkeit min %.0f max %.0f Median %.0f'
              % (n, box[0], box[1] - 1, box[2], box[3] - 1, L.min(), L.max(), np.median(L)))
    for s in (110, 120, 130, 140, 150, 160):
        c2 = schwerpunkt(A2, b2, s); c6 = schwerpunkt(A6, b6, s)
        if not c2 or not c6:
            print('Schwelle %d: zu wenig Pixel' % s); continue
        o2, d2 = strahl(cams[2], c2[0], c2[1]); o6, d6 = strahl(cams[6], c6[0], c6[1])
        p, rest = trianguliere([(o2, d2), (o6, d6)])
        r2 = projiziere(cams[2], p); r6 = projiziere(cams[6], p)
        print('Schwelle %3d: Cut2 Schwerpunkt (%.2f,%.2f) n=%d Huelle x%d..%d y%d..%d | Cut6 (%.2f,%.2f) n=%d Huelle x%d..%d y%d..%d'
              % (s, c2[0], c2[1], c2[2], c2[3], c2[4], c2[5], c2[6], c6[0], c6[1], c6[2], c6[3], c6[4], c6[5], c6[6]))
        print('              -> Schnittpunkt (%.0f, %.0f, %.0f), Strahlabstand %.1f / %.1f Einheiten; '
              'zurueck: Cut2 (%.2f,%.2f) Cut6 (%.2f,%.2f)'
              % (p[0], p[1], p[2], rest[0], rest[1], r2[0], r2[1], r6[0], r6[1]))


if __name__ == '__main__':
    main()
