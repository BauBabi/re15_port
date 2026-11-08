"""Runde 41 Spur SF N1.4: Spider-Man-Bahn im Ansprung (h0..h16), Video 4 gegen Port-Aufnahmen.

    python re15_port/tools/r41_sichel/bahn.py <laufordner>@<p0> [...]

Mitte/Kasten des groessten Flecks der Differenz gegen ein Bild ohne Figur links (h112), Bereich x < 165 (PSX).
"""
import os, sys, numpy as np
from PIL import Image
from scipy import ndimage
# Spider-Man-Bahn h0..16: Differenz gegen ein Bild ohne Figur links (h110), Bereich x < 165 (PSX), groesster Fleck.
V = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
def video(h):
    v = Image.open(V % (1350 + h)).convert('RGB').resize((320, 240), Image.BILINEAR)
    p = Image.new('RGB', (320, 240)); p.paste(v, (-1, -3)); return np.asarray(p, dtype=float)
def port(d, p0, h):
    return np.asarray(Image.open('%s/gd_%04d.png' % (d, p0 + h)).convert('RGB').resize((320, 240)), dtype=float)
def bahn(get, bg, hs, xmax=165):
    out = {}
    for h in hs:
        a = get(h); m = np.abs(a - bg).sum(2) > 60
        m[:, xmax:] = False; m[:20] = False; m[205:] = False
        m = ndimage.binary_opening(m, iterations=1)
        lab, n = ndimage.label(m)
        if n == 0: out[h] = None; continue
        s = ndimage.sum(m, lab, range(1, n + 1)); k = int(np.argmax(s)) + 1
        ys, xs = np.where(lab == k)
        out[h] = (xs.mean(), ys.mean(), xs.min(), xs.max(), ys.min(), ys.max(), len(xs))
    return out
hs = list(range(0, 17))
vb = bahn(video, video(112), hs)
res = [('video', vb)]
for arg in sys.argv[1:]:
    d, p0 = arg.split('@'); p0 = int(p0)
    res.append((d[-12:], bahn(lambda h: port(d, p0, h), port(d, p0, 112), hs)))
for h in hs:
    print('h%2d ' % h + ' | '.join('%s c(%5.1f,%5.1f) box x%3d-%3d y%3d-%3d n%4d' % ((nm,) + r[h]) if r[h] else nm + ' -' for nm, r in res))
