#!/usr/bin/env python3
"""Abnahme 2: Netz nur aus FORM.EMD, ohne Generator. Je Clip OBJ (Gruppe je Knochen) fuer Blender, dazu Grundmasse. Kein RE-Wert."""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'mess')

def netz(k):
    aus = []
    for b in range(min(k.mo.skel.bone_count, len(k.mo.meshes))):
        m = k.mo.meshes[b]
        vl, tl, ql = [], [], []
        off = 0
        if m.tf:
            v = np.array([x[:3] for x in m.tv], float)
            vl.append(v)
            for g in m.tf:
                ie = g[1::2]
                tl.append((ie[0], ie[1], ie[2]))
            off = len(v)
        if m.qf:
            v = np.array([x[:3] for x in m.qv], float)
            vl.append(v)
            for g in m.qf:
                ie = [i + off for i in g[1::2]]
                ql.append((ie[0], ie[1], ie[3], ie[2]))    # PSX-Viereck: Reihenfolge 0,1,3,2 als Umlauf
        aus.append((np.vstack(vl) if vl else np.zeros((0, 3)), tl, ql))
    return aus

def pose(k, n, clip):
    R, T = k.mo.pose(k.kf(clip, 0))
    G = []
    for b, (v, tl, ql) in enumerate(n):
        q = v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float) if len(v) else v
        G.append(q)
    return G

def obj(n, G, pfad):
    with open(pfad, 'w') as f:
        off = 1
        for b, ((v, tl, ql), q) in enumerate(zip(n, G)):
            f.write('o b%02d\n' % b)
            for p in q:
                # PSX (x rechts, y unten, z vorn) rechtshaendig, Blender auch: (x, z, -y) ist Drehung, Determinante +1. Sonst waeren links und rechts vertauscht.
                f.write('v %.3f %.3f %.3f\n' % (p[0] / 1000, p[2] / 1000, -p[1] / 1000))
            for t in tl:
                f.write('f %d %d %d\n' % (t[0] + off, t[1] + off, t[2] + off))
            for t in ql:
                f.write('f %d %d %d %d\n' % (t[0] + off, t[1] + off, t[2] + off, t[3] + off))
            off += len(q)

k = P.Kandidat(K)
n = netz(k)
print('Knochen', len(n), 'Dreiecke', sum(len(t) for _, t, _ in n), 'Vierecke', sum(len(q) for _, _, q in n))
sk = k.mo.skel
print([a for a in dir(sk) if not a.startswith('_')][:30])
for c in (0, 1, 2):
    G = pose(k, n, c)
    A = np.vstack([g for g in G if len(g)])
    print('Clip', c, 'min', A.min(0).round(), 'max', A.max(0).round(), 'Ausdehnung', (A.max(0) - A.min(0)).round())
    obj(n, G, os.path.join(AUS, 'clip%d.obj' % c))

# Mitte jedes Teils je Clip in Blender-Koordinaten fuer Nahbilder
NM = {0:'huefte',1:'s1',2:'s2',3:'s3',4:'s4',5:'s5',6:'s6',7:'wirbel',8:'oschenkel_l',9:'oschenkel_r',10:'brust',11:'oarm_l',12:'uarm_l',13:'hand_l',14:'f14',15:'f15',16:'oarm_r',17:'uarm_r',18:'hand_r',19:'f19',20:'f20',21:'hals',22:'kopf',23:'kiefer',24:'uschenkel_l',25:'fuss_l',26:'uschenkel_r',27:'fuss_r',28:'f28',29:'f29',30:'k30',31:'k31'}
L = []
for c in (0, 1, 2):
    G = pose(k, n, c)
    for b, q in enumerate(G):
        if len(q):
            m = (q.max(0) + q.min(0)) / 2
            L.append('c%d %-12s %2d  mitte %.2f,%.2f,%.2f  groesse %.2f %.2f %.2f  ecken %d flaechen %d/%d' % (c, NM.get(b, '?'), b, m[0]/1000, m[2]/1000, -m[1]/1000, *((q.max(0)-q.min(0))/1000), len(q), len(n[b][1]), len(n[b][2])))
open(os.path.join(AUS, 'teilmitten.txt'), 'w').write('\n'.join(L))
