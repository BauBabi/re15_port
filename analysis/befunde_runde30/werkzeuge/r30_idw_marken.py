# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: die zwei Nutzer-Marken in "irons items.png" messen.

Kein Farb-Schwellwert nach Gefuehl: das Nutzerbild wird gegen JEDEN der neun
Hintergruende von ROOM1150 gestellt (Differenzbild). Der Cut mit der kleinsten
Abweichung AUSSERHALB der Marken ist der Cut des Bildes; die Marken sind die
Pixel, die vom Hintergrund abweichen UND reines Rot / reines Blau tragen.

    python r30_idw_marken.py
"""
import os, sys
import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
AUS = os.path.join(REPO, 'build', 'r30_irons-diary-welt')


def main():
    n = Image.open(os.path.join(REPO, 'irons items.png')).convert('RGB')
    print('Nutzerbild: %dx%d' % n.size)
    N = np.array(n).astype(int)
    for cut in range(9):
        p = os.path.join(AUS, 'bg', 'ROOM115%02d.png' % cut)
        B = np.array(Image.open(p).convert('RGB')).astype(int)
        if B.shape != N.shape:
            print('Cut %d: Groesse %s passt nicht' % (cut, B.shape)); continue
        D = np.abs(N - B).sum(axis=2)
        print('Cut %d: mittlere Abweichung %.2f | Pixel mit Abweichung > 0: %d | Median %d'
              % (cut, D.mean(), int((D > 0).sum()), int(np.median(D))))
    B = np.array(Image.open(os.path.join(AUS, 'bg', 'ROOM11502.png')).convert('RGB')).astype(int)
    D = np.abs(N - B).sum(axis=2)
    # Farbzensus der abweichenden Pixel
    ys, xs = np.nonzero(D > 0)
    print('abweichende Pixel gegen Cut 2: %d, Huelle x %d..%d y %d..%d'
          % (len(xs), xs.min(), xs.max(), ys.min(), ys.max()))
    import collections
    c = collections.Counter(tuple(N[y, x]) for y, x in zip(ys, xs))
    print('Farben der abweichenden Pixel (haeufigste 12):')
    for f, k in c.most_common(12):
        print('   RGB %s : %d' % (f, k))
    for name, test in (('ROT', lambda r, g, b: r > 200 and g < 80 and b < 80),
                       ('BLAU', lambda r, g, b: b > 200 and r < 80)):
        pts = [(x, y) for y, x in zip(ys, xs) if test(*N[y, x])]
        X = np.array([p[0] for p in pts]); Y = np.array([p[1] for p in pts])
        print('%s: %d Pixel, x %d..%d, y %d..%d, Flaeche der Huelle %d, Schwerpunkt (%.2f, %.2f), '
              'Huellenmitte (Pixelmitten) (%.1f, %.1f)'
              % (name, len(pts), X.min(), X.max(), Y.min(), Y.max(),
                 (X.max() - X.min() + 1) * (Y.max() - Y.min() + 1),
                 X.mean() + 0.5, Y.mean() + 0.5,
                 (X.min() + X.max() + 1) / 2.0, (Y.min() + Y.max() + 1) / 2.0))
        cf = collections.Counter(tuple(N[y, x]) for x, y in pts)
        print('   Farben:', cf.most_common(4))
        # Zeilenweise Ausdehnung
        for y in range(Y.min(), Y.max() + 1):
            xx = X[Y == y]
            print('   y=%d: x %d..%d (%d)' % (y, xx.min(), xx.max(), len(xx)))


if __name__ == '__main__':
    main()
