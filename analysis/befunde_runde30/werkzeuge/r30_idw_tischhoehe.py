# -*- coding: utf-8 -*-
"""Tischplatten-Hoehe in ROOM1150 aus ZWEI Hintergrundbildern (Cut 2 und Cut 6).

Verfahren (Bildkonsistenz): jeder Bildpunkt der Tischplatte in Cut 2 wird ueber seinen
Sehstrahl auf die waagerechte Ebene y=h gelegt und von dort in Cut 6 abgebildet. Stimmt
h, zeigen beide Bildpunkte dieselbe Stelle der Platte -> hohe Korrelation.
Kein Schwellwert, keine Farbtrennung; das Mass hat einen Gipfel oder es hat keinen.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_idw_geom import *
from PIL import Image, ImageFilter
import numpy as np

def bilin(A, x, y):
    x0 = int(np.floor(x)); y0 = int(np.floor(y)); fx = x - x0; fy = y - y0
    if x0 < 0 or y0 < 0 or x0 + 1 >= A.shape[1] or y0 + 1 >= A.shape[0]:
        return None
    return (A[y0, x0]*(1-fx)*(1-fy) + A[y0, x0+1]*fx*(1-fy)
            + A[y0+1, x0]*(1-fx)*fy + A[y0+1, x0+1]*fx*fy)

def messe(x0, x1, y0, y1, hs, glatt=1.2):
    cams = lade_kameras()
    A2 = np.array(Image.open(os.path.join(AUS, 'bg', 'ROOM11502.png')).convert('RGB')).astype(float)
    A6 = np.array(Image.open(os.path.join(AUS, 'bg', 'ROOM11506.png')).convert('RGB')
                  .filter(ImageFilter.BoxBlur(glatt))).astype(float)
    out = []
    for h in hs:
        a = []; b = []
        for sy in range(y0, y1):
            for sx in range(x0, x1):
                p, s = auf_ebene(cams[2], sx + 0.5, sy + 0.5, h)
                r = projiziere(cams[6], p)
                if r is None: continue
                v = bilin(A6, r[0] - 0.5, r[1] - 0.5)
                if v is None: continue
                a.append(A2[sy, sx]); b.append(v)
        a = np.array(a); b = np.array(b)
        if len(a) < 100: continue
        ncc = np.corrcoef(a.mean(axis=1), b.mean(axis=1))[0, 1]
        mad = np.abs(a - b).mean()
        out.append((h, len(a), ncc, mad))
    return out

if __name__ == '__main__':
    for name, (x0, x1, y0, y1) in (('ganze Platte  x100..212 y121..131', (100, 213, 121, 132)),
                                   ('um die Marken x125..170 y121..131', (125, 171, 121, 132)),
                                   ('linke Haelfte x100..150 y121..131', (100, 151, 121, 132)),
                                   ('rechte Haelfte x150..212 y121..131', (150, 213, 121, 132))):
        r = messe(x0, x1, y0, y1, range(-1700, -1340, 10))
        bn = max(r, key=lambda t: t[2]); bm = min(r, key=lambda t: t[3])
        zweit = sorted(r, key=lambda t: -t[2])
        print('%s: NCC-Gipfel h=%d (NCC %.4f, n=%d) | MAD-Tal h=%d (MAD %.2f)' % (name, bn[0], bn[2], bn[1], bm[0], bm[3]))
        print('    NCC-Verlauf:', ' '.join('%d:%.3f' % (t[0], t[2]) for t in r))
