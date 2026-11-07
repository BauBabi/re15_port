#!/usr/bin/env python3
"""Abnahme 2: Probe im Speicher, FORM.EMD bleibt unberuehrt. Schwanzwurzel laeuft nach vorn spitz aus, Faechermitte Unterschenkel herausgezogen.
Misst: Maskenunterschied Bild 1 und 2, Rueckenlinie, schreibt OBJ fuer Ton. Kein RE-Wert."""
import math, os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
from a2_netz_lib import netz, NAMEN
K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'mess')
ST = {60: .85, 260: .58, 470: .22}          # Ringstation x -> Radiusfaktor, Vorgabe der Probe, keine Messung
def wurzel(p):
    x, y, z = p[0], p[1], p[2]
    # Ringe x 60/260/470: Mitte und Radius, Faecher 500 -> Spitze 680
    if x == 500:
        return (680, 170, 0, 0) if len(p) > 3 else (680, 170, 0)
    return p
def probe(k):
    m = k.mo.meshes[1]
    for name in ('tv', 'qv'):
        v = getattr(m, name)
        a = np.array([x[:3] for x in v], float)
        neu = []
        # Ringe gruppieren ueber x, Toleranz 12
        for p in v:
            x = p[0]
            f = None
            for s, fa in ST.items():
                if abs(x - s) <= 12:
                    f = (s, fa)
            if f:
                r = [q for q in a if abs(q[0] - f[0]) <= 12]
                c = np.mean(r, axis=0)
                q = c + (np.array(p[:3], float) - c) * f[1]
                neu.append((int(round(q[0])), int(round(q[1])), int(round(q[2])), p[3]))
            else:
                neu.append(wurzel(p))
        setattr(m, name, neu)
    for b in (24, 26):
        m = k.mo.meshes[b]
        for name in ('tv', 'qv'):
            v = getattr(m, name)
            neu = []
            for p in v:
                # Faechermitte unten am Unterschenkel 170 Einheiten herausziehen. Ausgangsstation 930 ist je Seite gleich
                if abs(p[1] - 930) <= 12:
                    neu.append((p[0], p[1] + 170, p[2], p[3]))
                else:
                    neu.append(p)
            setattr(m, name, neu)
k0 = P.Kandidat(K)
k1 = P.Kandidat(K)
probe(k1)
print('Rumpf/Schwanz-Rueckenlinie Pose 1 und Ruhe nach Probe')
n1 = netz(k1)
def pose(k, n, clip):
    R, T = k.mo.pose(k.kf(clip, 0))
    return {b: (v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float), f) for b, (v, f) in enumerate(n)}
for clip, nm in ((0, 'Pose 1'), (2, 'Ruhe')):
    G = pose(k1, n1, clip)
    s = np.vstack([G[b][0] for b in (1, 2)])
    r = np.vstack([G[b][0] for b in (0, 7, 10)])
    z = []
    for x in range(-600, 1000, 200):
        a = -s[np.abs(s[:, 0] - x) < 130, 1]; b = -r[np.abs(r[:, 0] - x) < 130, 1]
        z.append('%d: %s/%s' % (x, '%.0f' % a.max() if len(a) else '-', '%.0f' % b.max() if len(b) else '-'))
    print('  ', nm, '  '.join(z))
for n in (1, 2):
    _, _, m0, _ = k0.rendern(n)
    s1, alb, m1, _ = k1.rendern(n)
    d = m0 ^ m1
    ys, xs = np.where(d)
    print('Bild %d: Maske alt %d neu %d, verschieden %d px' % (n, m0.sum(), m1.sum(), d.sum()), ('Lage x %d-%d y %d-%d' % (xs.min(), xs.max(), ys.min(), ys.max())) if d.any() else '')
    nl, lab, st, _ = cv2.connectedComponentsWithStats(d.astype(np.uint8), connectivity=8)
    for i in np.argsort(-st[1:, 4])[:5] + 1:
        print('    Fleck %d px x %d-%d y %d-%d, alt Modell %s' % (st[i, 4], st[i, 0], st[i, 0] + st[i, 2], st[i, 1], st[i, 1] + st[i, 3], bool(m0[lab == i].mean() > .5)))
    # gegen Ziele
    z = P.ziel_laden(n)
    for nmx, mm in (('alt', m0), ('neu', m1)):
        print('   ', nmx, 'Ueberdeckung', P.ueberdeckung(mm, z))
for c in (0, 1, 2):
    G = pose(k1, n1, c)
    with open(os.path.join(AUS, 'probe%d.obj' % c), 'w') as f:
        off = 1
        for b, (q, fa) in G.items():
            f.write('o b%02d\n' % b)
            for p in q:
                f.write('v %.3f %.3f %.3f\n' % (p[0] / 1000, p[2] / 1000, -p[1] / 1000))
            for t in fa:
                f.write('f %d %d %d\n' % (t[0] + off, t[1] + off, t[2] + off))
            off += len(q)
print('fertig')
