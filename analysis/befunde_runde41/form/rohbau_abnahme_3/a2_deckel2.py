#!/usr/bin/env python3
"""Abnahme 2: nur echte Deckel. Flache Inseln mit scharfem Rand (mittlerer Knick am Rand >= 30 Grad), die nach aussen frei liegen. Kein RE-Wert."""
import math, os, sys
import numpy as np
import trimesh
import networkx as nx
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
from a2_netz_lib import netz, NAMEN
from a2_deckel_kern import treffer
K = os.path.join(FORM, 'rohbau_final', 'form.json')
k = P.Kandidat(K)
n = netz(k)
def welt(clip):
    R, T = k.mo.pose(k.kf(clip, 0))
    aus = {}
    for b, (v, f) in enumerate(n):
        if len(v):
            q = v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float)
            m = trimesh.Trimesh(vertices=q, faces=f, process=False)
            m.merge_vertices()
            aus[b] = m
    return aus
def deckel(M, tri_alle):
    z = []
    for b, m in M.items():
        g = nx.Graph(); g.add_nodes_from(range(len(m.faces)))
        ang = m.face_adjacency_angles
        adj = m.face_adjacency
        g.add_edges_from(adj[ang < math.radians(8)])
        for c in nx.connected_components(g):
            c = set(c)
            A = float(m.area_faces[list(c)].sum()) / 1e6
            if A < .03 or len(c) < 4:
                continue
            rand = [(a, np.linalg.norm(m.vertices[e[0]] - m.vertices[e[1]])) for (i, j), e, a in zip(adj, m.face_adjacency_edges, ang) if (i in c) != (j in c)]
            if not rand:
                continue
            w = sum(l for _, l in rand)
            knick = math.degrees(sum(a * l for a, l in rand) / w)
            if knick < 30:
                continue
            li = list(c)
            wf = m.area_faces[li][:, None]
            nr = (m.face_normals[li] * wf).sum(0); nr /= np.linalg.norm(nr); nr = -nr
            mitte = (m.triangles_center[li] * wf).sum(0) / wf.sum()
            frei = sum(1 for p in m.triangles_center[li] if not len(treffer(p + nr * 8, nr, tri_alle))) / len(li)
            z.append((A, NAMEN.get(b, b), len(li), knick, nr, -mitte[1], frei))
    return z
for clip, name in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    M = welt(clip)
    tri = np.vstack([m.triangles for m in M.values()])
    print('---', name)
    ges = 0
    for A, nm, cnt, kn, nr, h, fr in sorted(deckel(M, tri), key=lambda z: -z[0]):
        ges += A * fr
        print('  %-13s Flaechen %2d  %.3f m2  Durchm %.2f  Randknick %3.0f  aussen (%+.2f %+.2f %+.2f)  Hoehe %.2f  frei %.2f' % (nm, cnt, A, 2 * math.sqrt(A / math.pi), kn, nr[0], -nr[1], nr[2], h / 1000, fr))
    print('  freie Deckelflaeche gesamt %.2f m2' % ges)
