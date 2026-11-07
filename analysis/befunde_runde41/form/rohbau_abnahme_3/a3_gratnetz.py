#!/usr/bin/env python3
"""Probe nur im Speicher: s1-Ecken vor Pose-1-Modell-x CUT um SENK senken (Knochen-lokal), dann OBJ je Clip nach netz_probe/. Aendert keine Abgabedatei."""
import os, sys
import numpy as np
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
sys.argv = sys.argv[:1] + sys.argv[1:]
CUT = int(sys.argv[1]); SENK = int(sys.argv[2])
import form_pruefung as P
src = open(os.path.join(H, 'a3_netz.py')).read().split('k = P.Kandidat(K)')[0]
exec(src)                                   # netz, pose, obj
k = P.Kandidat(K)
R, T = k.mo.pose(k.kf(*k.stand(1)[2:]))
Rf = R[1].astype(float) / 4096.0; Tf = T[1].astype(float)
m = k.mo.meshes[1]
for liste in (m.tv, m.qv):
    for i, e in enumerate(liste):
        p = Rf @ np.array(e[:3], float) + Tf
        if p[0] > CUT:
            p[1] += SENK; v2 = np.linalg.solve(Rf, p - Tf)
            liste[i] = type(e)([int(round(x)) for x in v2] + list(e[3:]))
n = netz(k)
for c in (0, 1, 2):
    obj(n, pose(k, n, c), os.path.join(H, 'netz_probe', 'clip%d.obj' % c))
print('ok')
