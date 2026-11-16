"""Orange Bildpunkte (Maske des Sicht-Pruefers R3: HSV H 16-40 Grad, S > 0,43, V > 0,59) im Kasten x 420-760 / y 180-420 der echten
Bilder (960x720), F225-F450; je Bild Zahl und groesstes zusammenhaengendes Stueck (8er-Nachbarschaft).  python orange.py N [REF]"""
import sys, numpy as np
from scipy import ndimage
import szene as SZ
def maske(a):
    a = a[180:420, 420:760].astype(float) / 255.0
    mx, mn = a.max(-1), a.min(-1); d = mx - mn + 1e-9
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    h = np.where(mx == r, (g - b) / d % 6, np.where(mx == g, (b - r) / d + 2, (r - g) / d + 4)) * 60
    s = np.where(mx > 0, d / (mx + 1e-9), 0)
    return (h >= 16) & (h <= 40) & (s > 0.43) & (mx > 0.59)
n = sys.argv[1]; ref = sys.argv[2] if len(sys.argv) > 2 else None
tot = 0; bilder = 0; neu_b = 0; zeilen = []
for f in range(225, 451):
    a = SZ.ppm(n, f)
    if a is None: continue
    m = maske(a); k = int(m.sum()); tot += k; bilder += k > 0
    lab, nn = ndimage.label(m, np.ones((3, 3)))
    gr = max(ndimage.sum(m, lab, range(1, nn + 1))) if nn else 0
    nz = None
    if ref:
        b = SZ.ppm(ref, f); mr = maske(b)
        nz = int((m & ~ndimage.binary_dilation(mr, np.ones((3, 3)), iterations=2)).sum())
        neu_b += nz >= 3
    zeilen.append((f, k, int(gr), nz))
print(n, 'orange px F225-F450: %d in %d Bildern; groesstes Stueck max %d' % (tot, bilder, max(z[2] for z in zeilen)),
      ('| neu gegen %s (nicht in 2 px Umkreis): Bilder mit >= 3 neuen px %d, Summe %d' % (ref, neu_b, sum(z[3] for z in zeilen))) if ref else '')
print('  je Bild (F, px, groesstes, neu):', [z for z in zeilen if z[1]][:40])
