"""Blut-Flaeche RE2 gegen Port, gleiche Masszahl wie die Autoren (blutflaeche.py: rein rote Pixel R>=30, G,B <= 0,12 R + 3, im
Band ohne Kinobalken, abzueglich der schon vorher roten Pixel), aber jedes Bild statt Stichproben.
blut2.py <re2-dir mit re2_<clipzeit>.png> <re2-basiszeit> <port-laufdir> <port-basisbild> <port-von> <port-bis>"""
import sys, glob, os, re
import numpy as np
from PIL import Image
def maske(p, band):
    a = np.asarray(Image.open(p).convert('RGB')).astype(int)[band[0]:band[1]]
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    return (r >= 30) & (g <= 0.12 * r + 3) & (b <= 0.12 * r + 3)
r2, r2b, lauf, pb, pv, pbis = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6])
m0 = maske(os.path.join(r2, 're2_%s.png' % r2b), (32, 208))
zs = sorted(float(re.search(r're2_([\d.]+)\.png', p).group(1)) for p in glob.glob(os.path.join(r2, 're2_*.png')))
t0 = None
for t in zs:
    if t <= float(r2b): continue
    v = float((maske(os.path.join(r2, 're2_%.2f.png' % t), (32, 208)) & ~m0).mean())
    print('RE2  Clip %.2f s (Aufnahme %.2f s)  %5.1f %%' % (t, t + 1.0, 100 * v))
m0 = maske(os.path.join(lauf, 'f%06d.ppm' % pb), (48, 432))
for f in range(pv, pbis + 1):
    v = float((maske(os.path.join(lauf, 'f%06d.ppm' % f), (48, 432)) & ~m0).mean())
    print('Port F%d (+%.2f s nach dem Treffer F%d)  %5.1f %%' % (f, (f - pv) / 30.0, pv, 100 * v))
