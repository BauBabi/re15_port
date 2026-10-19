"""Vollbild-Vergleich Sweep-Abzug <-> Soll (320x240, Klassenbild aus komp.farbe_klasse).
Aufruf: python vgl_voll.py <sweepordner> [x0 y0 x1 y1]  (Default: ganzer Schirm)
Aktuell (C) zaehlt als Koerper (B), Mischfarbe aktuell-ueber-Wand (CW) als X; der Spieler-Marker (O) ist ausgenommen."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from komp import farbe_klasse, KL, NAME
import numpy as np
from PIL import Image
from collections import Counter
NAMEN = {2: 'map1F', 3: 'map2F', 4: 'map3F', 1: 'mapB2'}
sd = sys.argv[1]
box = list(map(int, sys.argv[2:6])) if len(sys.argv) >= 6 else [0, 0, 320, 240]
bd = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'G_karten_bilder')
ges = 0
for seite, name in NAMEN.items():
    a = farbe_klasse(np.array(Image.open(f'{sd}/s{seite:02d}.bmp').convert('RGB')))
    s = farbe_klasse(np.array(Image.open(f'{bd}/soll_{name}.png').convert('RGB')))
    for k in (a, s):
        k[k == KL['C']] = KL['B']; k[k == KL['CW']] = KL['X']
    m = np.zeros(a.shape, bool); m[box[1]:box[3], box[0]:box[2]] = True
    d = (a != s) & m & (s != KL['O']) & (a != KL['O'])
    ys, xs = np.nonzero(d)
    print(name, 'Abweichungen:', int(d.sum()), dict(Counter((NAME[a[y, x]], NAME[s[y, x]]) for y, x in zip(ys, xs))),
          ('x %d..%d y %d..%d' % (xs.min(), xs.max(), ys.min(), ys.max())) if len(xs) else '')
    ges += int(d.sum())
print('GESAMT', ges)
