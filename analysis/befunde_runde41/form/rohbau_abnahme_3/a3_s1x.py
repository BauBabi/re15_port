#!/usr/bin/env python3
"""Abnahme 3: Welche Modell-x der Schwanzknochen liegen in Bild 1 auf der Oberkante (y < 200)? Kein RE-Wert."""
import os, sys
import numpy as np
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P, form_buehne as FB, emd_ansichtsblatt as EA
k = P.Kandidat(os.path.join(F, 'rohbau_final', 'form.json'))
lage, gier, clip, bild = k.stand(1); kam = P.kamera(1); kf = k.kf(clip, bild)
s, c = EA.sin_q12(gier), EA.cos_q12(gier)
Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
R, T = k.mo.pose(kf)
L = np.array(lage, dtype=np.int64)
out = []
for b in (0, 7, 1, 2):
    m = k.mo.meshes[b]
    v = np.array([x[:3] for x in (m.tv + m.qv)], dtype=np.int64)
    mod = (v @ R[b].T >> 12) + T[b]                 # Modellraum (Pose)
    W = (Y @ R[b]) >> 12; t = ((Y @ T[b]) >> 12) + L
    VW = (kam.V @ W) >> 12; vT = ((kam.V @ t) >> 12) + kam.t
    q = (VW @ v.T) >> 12
    sx, sy = FB._gte(q[0] + vT[0], q[1] + vT[1], q[2] + vT[2], kam.h)
    x, y = FB.abbilden(np.array(sx, float) + FB.PHASE, np.array(sy, float) + FB.PHASE, kam.abbildung)
    oben = y < 200
    out.append('Knochen %d: %d Ecken, Bild y min %.0f; Ecken mit y<200: %d, Modell-x %s, Hoehe %s' % (b, len(v), y.min(), oben.sum(),
               ('%d..%d' % (mod[oben, 0].min(), mod[oben, 0].max())) if oben.any() else '-', ('%d..%d' % (-mod[oben, 1].max(), -mod[oben, 1].min())) if oben.any() else '-'))
    for lo, hi in ((-1000, -600), (-600, -200), (-200, 200), (200, 600), (600, 1400)):
        s_ = (mod[:, 0] >= lo) & (mod[:, 0] < hi)
        if s_.any():
            out.append('    Modell-x %5d..%5d: Bild y %.0f..%.0f, x %.0f..%.0f' % (lo, hi, y[s_].min(), y[s_].max(), x[s_].min(), x[s_].max()))
open(os.path.join(H, 'mess', 's1x.txt'), 'w').write('\n'.join(out) + '\n'); print('\n'.join(out))
