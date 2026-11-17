import sys, os, numpy as np
from PIL import Image
from scipy import ndimage as ndi
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rand as R
L = sys.argv[1]
acc = {}
for f in range(R.F0, R.F1 + 1):
    s_ = R.bild(L + '/s', f); mk = R.bild(L + '/mk_klasse', f)
    T_ = np.abs(mk - s_).sum(2) > 0
    kl = R.muster(mk, R.MUSTER6, T_)
    bauch = (kl == 0) | (kl == 1); flanke = (kl == 2) | (kl == 3)
    d_fl = ndi.distance_transform_edt(~flanke)
    for s in sys.argv[2:]:
        n = R.bild(L + '/n_' + s, f); nh = R.bild(L + '/naht_' + s, f)
        naht = T_ & (np.abs(nh - n).sum(2) > 0)
        lab = R.G.lab(n.astype(float))
        for dd in (1, 2, 3, 4, 6, 8):
            ring = bauch & (d_fl > dd - 1) & (d_fl <= dd)
            a = acc.setdefault((s, dd), [[], []])
            a[0].append(lab[ring & naht]); a[1].append(lab[ring & ~naht])
for s in sys.argv[2:]:
    print('== %s: Bauch-Bildpunkte nach Abstand zur Flanke (px, 960x720): Anteil Naht-Texel u 54-55 | L* Naht | L* sonst (Median, 10. Perzentil) | a*' % s)
    for dd in (1, 2, 3, 4, 6, 8):
        a, b = (np.concatenate(x) for x in acc[(s, dd)])
        n_ = len(a) + len(b)
        q = lambda x: '%5.1f %5.1f' % (np.median(x[:, 0]), np.percentile(x[:, 0], 10)) if len(x) else '    -     -'
        alle = np.concatenate([a, b])
        print('  d %d: %6d px, Naht %5.1f %%, L* Naht %s, L* sonst %s, alle L* %s a* %4.1f' % (dd, n_, 100.0 * len(a) / max(n_, 1), q(a), q(b), q(alle), np.median(alle[:, 1])))
