#!/usr/bin/env python
"""Runde 30 karten-marken, BAU: abweichende Punkte eigener Abzug (320x240) gegen Nutzer-Abzug
(960x720) nach ORT klassifizieren (benannte Kaesten). Aufruf:
  python r30_bau_diff_klassen.py <eigen.bmp> <nutzer.png> <blatt> "<name>:x,y,w,h;..."
Ausgabe je Kasten: Punktzahl und die haeufigsten Farbwechsel eigen<-nutzer; REST = ausserhalb."""
import sys, struct
import numpy as np
from PIL import Image
from collections import Counter
a = np.array(Image.open(sys.argv[1]).convert('RGB'))
b = np.array(Image.open(sys.argv[2]).convert('RGB'))[::3, ::3]
seite = int(sys.argv[3])
kaesten = []
for teil in (sys.argv[4].split(';') if len(sys.argv) > 4 and sys.argv[4] else []):
    nm, xywh = teil.split(':'); x, y, w, h = map(int, xywh.split(','))
    kaesten.append((nm, x, y, w, h))
d = np.any(a != b, axis=2)
ys, xs = np.nonzero(d)
k = Counter(); farben = {}
for y, x in zip(ys, xs):
    nm = 'REST'
    for (n, kx, ky, kw, kh) in kaesten:
        if kx <= x < kx + kw and ky <= y < ky + kh: nm = n; break
    k[nm] += 1
    farben.setdefault(nm, Counter())[(tuple(a[y, x]), tuple(b[y, x]))] += 1
print('abweichend gesamt', int(d.sum()))
for nm, n in k.most_common():
    print('  %-14s %5d' % (nm, n), '  ', ', '.join('%s<-%s:%d' % (e, u, c) for (e, u), c in farben[nm].most_common(4)))
if 'REST' in farben:
    rys = [y for y, x in zip(ys, xs)]
    pts = [(x, y) for y, x in zip(ys, xs) if not any(kx <= x < kx + kw and ky <= y < ky + kh for (_, kx, ky, kw, kh) in kaesten)]
    print('  REST-Punkte (bis 40):', pts[:40])
