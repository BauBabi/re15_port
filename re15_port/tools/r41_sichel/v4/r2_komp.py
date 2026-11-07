"""SF V4 Runde 2: helle Komponenten im Kopf (p_blob-Schwelle) je Ansicht - Lage und Groesse (Fehlersuche K1/S1)."""
import sys
import v4_gemein as G
import numpy as np, b2, riegel_v4 as RV
from scipy import ndimage as nd
mo = b2.modell(sys.argv[1]); P = G.posen()
for fn in (RV.kopf_vorn, RV.kopf_seite):
    Z, info, (img, ids, V, b) = fn(mo, P)
    L = G.lab(V)[..., 0]
    lb, k = nd.label((L > b['thr']) & (ids == 9))
    for i in range(1, k + 1):
        ys, xs = np.nonzero(lb == i)
        if len(xs) >= 2:
            print(fn.__name__, len(xs), 'x', xs.min(), xs.max(), 'y', ys.min(), ys.max(), 'Lmax %.1f' % L[lb == i].max())
    print('  thr %.1f ext %.1f umg %.1f kb %s' % (b['thr'], b['ext'], b['umg'], info['kopfbox']))
