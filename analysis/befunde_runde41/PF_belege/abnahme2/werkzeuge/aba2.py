import numpy as np, sys, bilder, panel_inc
import scipy.ndimage as nd
lauf, welche = sys.argv[1], sys.argv[2]
G = panel_inc.grund('panel_%s.inc' % welche).astype(np.int16)
ev = bilder.ereignisse(lauf, ('[panel2030]',))
t0 = next(t for t, s in ev if 'offen:' in s); t1 = next(t for t, s in ev if 'GELOEST: A' in s)
seq = []
for i, t, b in bilder.bilder(lauf):
    if t0 + 0.2 < t < t1 - 0.1:
        l = bilder.logisch(b)
        if (np.abs(l.astype(np.int16) - G).max(-1) == 0).mean() > 0.6:
            seq.append((t, l.copy()))
u = [seq[0]]
for t, l in seq[1:]:
    if not np.array_equal(l, u[-1][1]): u.append((t, l))
acc = np.zeros((240, 320), int); zeiten = []
for k in range(1, len(u) - 1):
    m = (u[k][1] != u[k - 1][1]).any(-1) & (u[k - 1][1] == u[k + 1][1]).all(-1)
    if m.any():
        acc += m; zeiten.append(u[k][0])
lab, n = nd.label(acc > 0, np.ones((3, 3)))
for s in nd.find_objects(lab):
    k = int((acc[s] > 0).sum())
    if k >= 4: print('  Bereich x %d..%d y %d..%d, %d Texel, max %d mal' % (s[1].start, s[1].stop - 1, s[0].start, s[0].stop - 1, k, acc[s].max()))
z = np.array(zeiten); print('  Zeiten %.2f..%.2f, Abstaende Median %.3f s' % (z.min(), z.max(), np.median(np.diff(z))))
