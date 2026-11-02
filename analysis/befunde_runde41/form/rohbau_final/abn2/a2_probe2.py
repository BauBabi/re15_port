#!/usr/bin/env python3
"""Abnahme 2: Probe 2 im Speicher, FORM.EMD bleibt unberuehrt. Nur je eine Faechermitte versetzt: Schwanzwurzel zum langen Kegel in den Ruecken, Ferse zum flachen Kegel.
Misst Maskenunterschied, Ueberdeckung gegen ziel2, Rueckenlinie, Deckelfreiheit. Kein RE-Wert."""
import math, os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
from a2_netz_lib import netz, NAMEN
K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'mess')
k0 = P.Kandidat(K)
def setze(m, alt, neu):
    n = 0
    for name in ('tv', 'qv'):
        v = list(getattr(m, name))
        for i, p in enumerate(v):
            if p[0] == alt[0] and p[1] == alt[1] and p[2] == alt[2]:
                v[i] = (neu[0], neu[1], neu[2], p[3]); n += 1
        setattr(m, name, v)
    return n
def kandidat(sx, sy, zug):
    k = P.Kandidat(K)
    m = k.mo.meshes[1]
    a = [p for p in m.tv if 495 <= p[0] <= 505]
    assert len(a) == 1, a
    setze(m, a[0][:3], (sx, sy, a[0][2]))
    for b in (24, 26):
        m = k.mo.meshes[b]
        a = [p for p in m.tv if abs(p[1] - 930) <= 12]
        assert len(a) == 1, a
        setze(m, a[0][:3], (a[0][0], a[0][1] + zug, a[0][2]))
    return k
def pose(k, n, clip):
    R, T = k.mo.pose(k.kf(clip, 0))
    return {b: (v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float), f) for b, (v, f) in enumerate(n)}, R, T
# Wo landet die Kegelspitze? Bone-1-Raum nach Welt, fuer die drei Clips
n0 = netz(k0)
for sx, sy in ((1240, 547),):
    z = []
    for c, nm in ((0, 'P1'), (2, 'Ruhe'), (1, 'P2')):
        G, R, T = pose(k0, n0, c)
        w = np.array([sx, sy, 0.]) @ (R[1].astype(float).T / 4096.0) + T[1].astype(float)
        r = np.vstack([G[b][0] for b in (0, 7, 10)])
        o = -r[np.abs(r[:, 0] - w[0]) < 130, 1]
        z.append('%s Spitze x %.0f Hoehe %.0f Rumpf dort %s' % (nm, w[0], -w[1], '%.0f' % o.max() if len(o) else '-'))
    print('Spitze (%d,%d): ' % (sx, sy) + ' | '.join(z))
sx, sy, zug = 1240, 547, 95
k1 = kandidat(sx, sy, zug)
n1 = netz(k1)
for clip, nm in ((0, 'Pose 1'), (2, 'Ruhe')):
    G, _, _ = pose(k1, n1, clip)
    s = np.vstack([G[b][0] for b in (1, 2)])
    r = np.vstack([G[b][0] for b in (0, 7, 10)])
    z = []
    for x in range(0, 1400, 200):
        a = -s[np.abs(s[:, 0] - x) < 130, 1]; b = -r[np.abs(r[:, 0] - x) < 130, 1]
        z.append('%d: %s/%s' % (x, '%.0f' % a.max() if len(a) else '-', '%.0f' % b.max() if len(b) else '-'))
    print('Rueckenlinie', nm, 'Schwanz/Rumpf  ' + '  '.join(z))
for n in (1, 2):
    _, _, m0, _ = k0.rendern(n)
    _, _, m1, _ = k1.rendern(n)
    d = m0 ^ m1
    print('Bild %d: Maske alt %d neu %d, verschieden %d px' % (n, m0.sum(), m1.sum(), d.sum()))
    nl, lab, st, _ = cv2.connectedComponentsWithStats(d.astype(np.uint8), connectivity=8)
    for i in np.argsort(-st[1:, 4])[:6] + 1:
        print('    Fleck %d px x %d-%d y %d-%d, %s' % (st[i, 4], st[i, 0], st[i, 0] + st[i, 2], st[i, 1], st[i, 1] + st[i, 3], 'weggefallen' if m0[lab == i].mean() > .5 else 'neu'))
    z = P.ziel_laden(n)[1]
    for nmx, mm in (('alt', m0), ('neu', m1)):
        u = P.ueberdeckung(mm, z)
        print('   ', nmx, {a: u[a] for a in ('iou', 'weiss_gedeckt')})
for c in (0, 1, 2):
    G, _, _ = pose(k1, n1, c)
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
