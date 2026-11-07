#!/usr/bin/env python3
"""M1: Kegelspitze der Schwanzwurzel (Knochen 1, fernste Ecke vor Ring 0) in jedem Clip: liegt sie im Rumpf (Strahlparitaet gegen 0, 7, 10)? Dazu Hoehe der Kegeloberkante."""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
A2 = os.path.dirname(HIER)
RF = os.path.dirname(A2)
sys.path.insert(0, os.path.normpath(os.path.join(RF, '..', '..', '..', '..', 're15_port', 'tools')))
sys.path.insert(0, A2)
import form_pruefung as P
from a2_netz_lib import netz
from a2_deckel_kern import treffer
k = P.Kandidat(os.path.join(RF, 'form.json'))
n = netz(k)
v1 = n[1][0]
# Spitze = Ecke mit groesstem x im Knochenraum (Kegel zeigt nach vorn, +x laengs)
i = int(v1[:, 0].argmax())
print('Spitze Knochenraum', v1[i].round())
for clip, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    R, T = k.mo.pose(k.kf(clip, 0))
    W = {b: n[b][0] @ (R[b].astype(float).T / 4096.0) + T[b].astype(float) for b in (0, 1, 7, 10)}
    p = W[1][i]
    drin = {}
    for b in (0, 7, 10):
        tri = W[b][n[b][1]]
        z = [len(treffer(p, np.array(d, float), tri)) % 2 for d in ((0, 1, 0), (0, -1, 0), (1, 0, 0), (-1, 0, 0), (0, 0, 1))]
        drin[b] = sum(z) >= 3
    print('%-6s Spitze Welt %s  im Rumpf %s (je Teil 0/7/10 %s)  Hoehe %.0f' % (nm, p.round(), 'ja' if any(drin.values()) else 'NEIN', [int(x) for x in drin.values()], -p[1]))
