#!/usr/bin/env python3
"""Abnahme 3: Netz nur aus FORM.EMD (ueber form_pruefung.Kandidat), je Clip OBJ mit Objekt je Knochen. Kein RE-Wert."""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'netz')

def netz(k):
    aus = []
    for b in range(min(k.mo.skel.bone_count, len(k.mo.meshes))):
        m = k.mo.meshes[b]
        vl, tl, ql = [], [], []
        off = 0
        if m.tf:
            v = np.array([x[:3] for x in m.tv], float); vl.append(v)
            for g in m.tf:
                ie = g[1::2]; tl.append((ie[0], ie[1], ie[2]))
            off = len(v)
        if m.qf:
            v = np.array([x[:3] for x in m.qv], float); vl.append(v)
            for g in m.qf:
                ie = [i + off for i in g[1::2]]
                ql.append((ie[0], ie[1], ie[3], ie[2]))
        aus.append((np.vstack(vl) if vl else np.zeros((0, 3)), tl, ql))
    return aus

def pose(k, n, clip):
    R, T = k.mo.pose(k.kf(clip, 0))
    return [v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float) if len(v) else v for b, (v, tl, ql) in enumerate(n)]

def obj(n, G, pfad):
    with open(pfad, 'w') as f:
        off = 1
        for b, ((v, tl, ql), q) in enumerate(zip(n, G)):
            f.write('o b%02d\n' % b)
            for p in q:
                # PSX (x, y unten, z) -> Blender (x, z, -y): Drehung, det +1, keine Spiegelung
                f.write('v %.3f %.3f %.3f\n' % (p[0] / 1000, p[2] / 1000, -p[1] / 1000))
            for t in tl:
                f.write('f %d %d %d\n' % (t[0] + off, t[1] + off, t[2] + off))
            for t in ql:
                f.write('f %d %d %d %d\n' % (t[0] + off, t[1] + off, t[2] + off, t[3] + off))
            off += len(q)

k = P.Kandidat(K)
n = netz(k)
print('Knochen', len(n), 'Dreiecke', sum(len(t) for _, t, _ in n), 'Vierecke', sum(len(q) for _, _, q in n))
info = {}
for c in (0, 1, 2):
    G = pose(k, n, c)
    A = np.vstack([g for g in G if len(g)])
    print('Clip', c, 'min', A.min(0).round(), 'max', A.max(0).round(), 'Ausdehnung', (A.max(0) - A.min(0)).round())
    obj(n, G, os.path.join(AUS, 'clip%d.obj' % c))
    info[c] = {'min': A.min(0).tolist(), 'max': A.max(0).tolist(), 'mitte': {b: (G[b].mean(0).tolist() if len(G[b]) else None) for b in range(len(G))}}
json.dump(info, open(os.path.join(AUS, 'info.json'), 'w'))
