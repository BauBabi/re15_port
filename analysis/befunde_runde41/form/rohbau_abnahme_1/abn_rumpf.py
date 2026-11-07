#!/usr/bin/env python3
"""Abnahme 1: Rumpf als Kette, Knickwinkel je Pose, Stufe an den Teilgrenzen, Oberarm-Bauch gegen Unterarm, Kastigkeit. Nur aus FORM.EMD. Kein RE-Wert."""
import os, sys, math
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose
k = P.Kandidat(K)
n = netz(k)
aus = []
def sag(t=''):
    print(t); aus.append(t)
# Radius ueber die Knochenachse je Meshes in Bindlage
def profil(b, achse=0, schritte=8):
    v, f = n[b]
    a = v[:, achse]
    q = np.delete(v, achse, axis=1)
    m = q.mean(0)
    r = np.linalg.norm(q - m, axis=1)
    kan = np.linspace(a.min(), a.max(), schritte + 1)
    z = []
    for i in range(schritte):
        s = (a >= kan[i]) & (a <= kan[i + 1])
        z.append(int(r[s].max()) if s.any() else 0)
    return int(a.min()), int(a.max()), z
for b, nm in ((0, 'huefte'), (7, 'wirbel'), (10, 'brust')):
    sag('%s x %s Radius je Abschnitt %s  y %d..%d z %d..%d' % ((nm,) + profil(b)[:1] + (profil(b)[1:],) + (int(n[b][0][:, 1].min()), int(n[b][0][:, 1].max()), int(n[b][0][:, 2].min()), int(n[b][0][:, 2].max()))) if False else '%s  x %d..%d  Radius je Abschnitt %s  y %d..%d  z %d..%d' % (nm, *profil(b), int(n[b][0][:, 1].min()), int(n[b][0][:, 1].max()), int(n[b][0][:, 2].min()), int(n[b][0][:, 2].max())))
for b, nm in ((11, 'oberarm l'), (16, 'oberarm r'), (12, 'unterarm l'), (17, 'unterarm r'), (8, 'oberschenkel l'), (24, 'unterschenkel l'), (25, 'fuss l')):
    a0, a1, z = profil(b, 1)
    sag('%-16s y %d..%d  Radius %s  Bauch/Ende %.2f' % (nm, a0, a1, z, max(z) / max(1, min(z[0], z[-1]) if min(z[0], z[-1]) else 1)))
# Achsen der Rumpfteile je Pose
for clip, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    kf = k.kf(clip, 0)
    R, T = k.mo.pose(kf)
    ach = {}
    for b in (0, 7, 10, 21, 22):
        ach[b] = (R[b].astype(float) / 4096.0)[:, 0]       # X-Achse des Knochens in Aktorraum
    def w(a, b):
        return math.degrees(math.acos(max(-1, min(1, float(ach[a] @ ach[b])))))
    sag('%s: Knick Huefte/Wirbel %.0f, Wirbel/Brust %.0f, Brust/Hals %.0f, Hals/Kopf %.0f; Neigung Brust gegen Boden %.0f, Huefte %.0f' % (nm, w(0, 7), w(7, 10), w(10, 21), w(21, 22), math.degrees(math.asin(-ach[10][1])), math.degrees(math.asin(-ach[0][1]))))
    V, F, B = pose(k, n, clip)
    for b, t in ((0, 'huefte'), (7, 'wirbel'), (10, 'brust'), (22, 'kopf')):
        p = V[np.unique(F[B == b])]
        sag('    %-7s Hoehe %d..%d  x %d..%d' % (t, -p[:, 1].max(), -p[:, 1].min(), p[:, 0].min(), p[:, 0].max()))
with open(os.path.join(HIER, 'mess', 'rumpf.txt'), 'w', encoding='utf-8') as f:
    f.write('\n'.join(aus))
