# Bildvergleich zweier Framedump-Serien (master-Kopie vs Integrations-Kopie), gleiche Bildnummern.
# Aufruf: python ppmdiff.py <dirA> <dirB> [von bis] [--thr N]
# Ausgabe je Bild: Pixel mit Abweichung (max |d| ueber RGB > thr), Box, mittlere Helligkeit in der Box A/B.
import sys, os, glob, re
import numpy as np
from PIL import Image
def load(p):
    return np.asarray(Image.open(p).convert('RGB')).astype(np.int16)
def main():
    a, b = sys.argv[1], sys.argv[2]
    thr = 0
    args = [x for x in sys.argv[3:]]
    if '--thr' in args:
        i = args.index('--thr'); thr = int(args[i+1]); del args[i:i+2]
    fa = sorted(glob.glob(os.path.join(a, 'f_*.ppm')))
    nums = [int(re.search(r'f_(\d+)\.ppm', f).group(1)) for f in fa]
    if len(args) >= 2:
        lo, hi = int(args[0]), int(args[1])
        nums = [n for n in nums if lo <= n <= hi]
    tot = 0
    for n in nums:
        pa = os.path.join(a, 'f_%06d.ppm' % n); pb = os.path.join(b, 'f_%06d.ppm' % n)
        if not os.path.exists(pb):
            print('F%d fehlt in B' % n); continue
        A = load(pa); B = load(pb)
        if A.shape != B.shape:
            print('F%d Groesse %s != %s' % (n, A.shape, B.shape)); continue
        d = np.abs(A - B).max(axis=2)
        m = d > thr
        c = int(m.sum()); tot += c
        if c == 0:
            print('F%d 0' % n); continue
        ys, xs = np.nonzero(m)
        x0, x1, y0, y1 = xs.min(), xs.max(), ys.min(), ys.max()
        la = A[m].mean(axis=0); lb = B[m].mean(axis=0)
        print('F%d %d px box x%d..%d y%d..%d maxd %d  meanA(%d,%d,%d) meanB(%d,%d,%d)' % (
            n, c, x0, x1, y0, y1, d.max(), la[0], la[1], la[2], lb[0], lb[1], lb[2]))
    print('SUMME %d' % tot)
main()
