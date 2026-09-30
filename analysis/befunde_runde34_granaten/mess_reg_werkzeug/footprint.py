# Diff-Pixel ausserhalb der Effekt-Fussabdruecke (Vereinigung beider Logs, Bild F-1..F+1, Box sx +- k*w).
# python footprint.py <mstdir> <intdir> <anker> <start> <von> <bis> [scale=3] [k=1.0]
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
import numpy as np
from PIL import Image
from fxframes import parse
md, idr, anker, start, lo, hi = sys.argv[1:7]
start, lo, hi = int(start), int(lo), int(hi)
sc = int(sys.argv[7]) if len(sys.argv) > 7 else 3
k = float(sys.argv[8]) if len(sys.argv) > 8 else 1.0
I = parse(os.path.join(idr, 'fx.log'))
M = parse(os.path.join(md, 'fx.log'), anker=anker, start=start) if anker != '-' else {}
def boxes(F):
    out = []
    for src in (I, M):
        for f in (F - 1, F, F + 1):
            for d in src.get(f, []):
                if not d.get('drawn'): continue
                w = max(1, d['w16'] >> 16)
                out.append((d['sx'] - k * w - 2, d['sy'] - k * w - 2, d['sx'] + k * w + 2, d['sy'] + k * w + 2))
    return out
for F in range(lo, hi + 1):
    pa = os.path.join(md, 'f_%06d.ppm' % F); pb = os.path.join(idr, 'f_%06d.ppm' % F)
    if not (os.path.exists(pa) and os.path.exists(pb)): continue
    A = np.asarray(Image.open(pa).convert('RGB')).astype(np.int16)
    B = np.asarray(Image.open(pb).convert('RGB')).astype(np.int16)
    d = np.abs(A - B).max(axis=2) > 0
    fp = np.zeros_like(d)
    for (x0, y0, x1, y1) in boxes(F):
        X0 = max(0, int(x0 * sc)); Y0 = max(0, int(y0 * sc)); X1 = min(d.shape[1], int(x1 * sc) + 1); Y1 = min(d.shape[0], int(y1 * sc) + 1)
        if X1 > X0 and Y1 > Y0: fp[Y0:Y1, X0:X1] = True
    inn = int((d & fp).sum()); aus = int((d & ~fp).sum())
    s = ''
    if aus:
        ys, xs = np.nonzero(d & ~fp); s = 'aussen-box x%d..%d y%d..%d' % (xs.min(), xs.max(), ys.min(), ys.max())
    print('F%d diff %d: in Fussabdruck %d, ausserhalb %d %s' % (F, int(d.sum()), inn, aus, s))
