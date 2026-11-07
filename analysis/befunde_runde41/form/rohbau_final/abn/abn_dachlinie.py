#!/usr/bin/env python3
"""Nacharbeit: Rueckenlinie im Seitenblick je Clip, Rumpf 0, 7, 10, mit Kennung, welcher Knochen oben liegt. Aus FORM.EMD. Kein RE-Wert."""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose
k = P.Kandidat(K)
n = netz(k)
for clip_, nm in ((1, 'Pose 2'), (2, 'Ruhe'), (0, 'Pose 1')):
    V2, F2, B2 = pose(k, n, clip_)
    m = np.isin(B2, (0, 7, 10))
    q = V2[:, [0, 1]]
    t = q[F2[m]]
    bb = B2[m]
    x0, x1 = t[:, :, 0].min(), t[:, :, 0].max()
    zeile = []
    alt = None
    for xx in np.arange(int(x0) + 40, int(x1) - 40, 120):
        beste, kn = None, None
        sel = (t[:, :, 0].min(1) <= xx) & (t[:, :, 0].max(1) >= xx)
        for tri, b in zip(t[sel], bb[sel]):
            for a, c in ((0, 1), (1, 2), (2, 0)):
                xa, ya = tri[a]; xc, yc = tri[c]
                if (xa - xx) * (xc - xx) <= 0 and xa != xc:
                    y = ya + (yc - ya) * (xx - xa) / (xc - xa)
                    if beste is None or y < beste:
                        beste, kn = y, b
        zeile.append('%d:%d(%d)' % (xx, -beste, kn))
    print(nm, ' '.join(zeile))
