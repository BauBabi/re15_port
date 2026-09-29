# -*- coding: utf-8 -*-
"""R32: Ende der Fahrt nach No/No (beide bleiben liegen) gegen einen Lauf OHNE beide (Flags 9:53,9:56),
ausgerichtet am ersten Abfahrt-Bild (Plattform verlaesst -1205) aus RE15_HEBETISCH_LOG.
Zaehlt je Bild die abweichenden Punkte: solange die Faecher sichtbar sind, sieht man die Gegenstaende;
sobald der Tisch in den Schreibtisch faehrt, muessen es 0 sein (kein Durchstoss/kein Durchscheinen).
ende_vergleich.py <mit-ordner> <ohne-ordner> <aus.txt>"""
import os, sys
import numpy as np
from PIL import Image
mit, ohne, aus = sys.argv[1:4]


def hl(d):
    out = {}
    for l in open(os.path.join(d, 'hebetisch.log')):
        p = l.split()
        if p and p[0].startswith('F'):
            out[int(p[0][1:])] = int(p[1][2:])
    return out


def abfahrt(h):
    for f in sorted(h):
        if f > 300 and h.get(f - 1) == -1205 and -300 > h[f] > -1205:
            return f


def park(h, f0):
    for f in sorted(h):
        if f > f0 and h[f] < -5000:
            return f


def L(d, f):
    p = os.path.join(d, 'f_%06d.ppm' % f)
    return np.asarray(Image.open(p).convert('RGB'), np.int16) if os.path.exists(p) else None


a, b = hl(mit), hl(ohne)
fa, fb = abfahrt(a), abfahrt(b)
pa, pb = park(a, fa), park(b, fb)
off = fa - fb
z = ['Abfahrt MIT F%d, OHNE F%d; Parklage MIT F%d, OHNE F%d; Versatz %d / %d' % (fa, fb, pa, pb, off, pa - pb)]
for f in range(fa - 6, pa + 6):
    x, y = L(mit, f), L(ohne, f - off)
    if x is None or y is None:
        continue
    m = np.abs(x - y).max(-1) > 0
    s = 'MIT F%d (y=%s) gegen OHNE F%d (y=%s): %d Punkte' % (f, a.get(f), f - off, b.get(f - off), int(m.sum()))
    if m.sum():
        ys, xs = np.nonzero(m)
        s += ' bbox x%d..%d y%d..%d' % (xs.min(), xs.max(), ys.min(), ys.max())
    z.append(s)
open(aus, 'w').write('\n'.join(z) + '\n')
print('\n'.join(z))
