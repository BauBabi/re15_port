#!/usr/bin/env python3
"""Abnahme 2: Netzpruefung nur aus FORM.EMD. Freiliegende flache Deckel, Dichtheit, Windung. Strahltest in numpy (Moeller-Trumbore). Kein RE-Wert."""
import math, os, sys
import numpy as np
import trimesh
import networkx as nx
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
from a2_netz_lib import netz, NAMEN
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
def treffer(o, d, tri):
    """Strahlparameter t>0 aller Treffer auf tri (N,3,3)."""
    e1 = tri[:, 1] - tri[:, 0]; e2 = tri[:, 2] - tri[:, 0]
    p = np.cross(d, e2)
    det = (e1 * p).sum(1)
    ok = np.abs(det) > 1e-9
    inv = np.zeros_like(det); inv[ok] = 1 / det[ok]
    s = o - tri[:, 0]
    u = (s * p).sum(1) * inv
    q = np.cross(s, e1)
    v = (q * d).sum(1) * inv
    t = (e2 * q).sum(1) * inv
    m = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > 1e-6)
    return t[m]
def innen(p, tri):
    return len(treffer(p, np.array([.4359, .7171, .5436]), tri)) % 2 == 1
print('=== Netz je Knochen (Clip 2)')
M = welt(2)
print('dicht:', sum(m.is_watertight for m in M.values()), 'von', len(M), ' Volumenvorzeichen je Teil:', sorted(set('+' if m.volume > 0 else '-' for m in M.values())))
print('offene Kanten', sum(int(trimesh.grouping.group_rows(m.edges_sorted, require_count=1).size) for m in M.values()))
print()
print('=== Flache Deckel: zusammenhaengende Flaechen, Knick <= 6 Grad, Flaeche >= 0.08 m2, frei = Anteil der Flaechen mit freiem Blick nach aussen')
for clip, name in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    M = welt(clip)
    tri_alle = np.vstack([m.triangles for m in M.values()])
    zeilen = []
    for b, m in M.items():
        g = nx.Graph(); g.add_nodes_from(range(len(m.faces)))
        g.add_edges_from(m.face_adjacency[m.face_adjacency_angles < math.radians(6)])
        for c in nx.connected_components(g):
            c = list(c)
            A = float(m.area_faces[c].sum()) / 1e6
            if A < .08:
                continue
            w = m.area_faces[c][:, None]
            nrm = (m.face_normals[c] * w).sum(0); nrm /= np.linalg.norm(nrm)
            # Windung: alle Volumen negativ, also zeigt die trimesh-Normale nach innen
            aus_n = -nrm
            mitte = (m.triangles_center[c] * w).sum(0) / w.sum()
            frei = 0
            for p in m.triangles_center[c]:
                if not len(treffer(p + aus_n * 8, aus_n, tri_alle)):
                    frei += 1
            pr = mitte + aus_n * 30
            in_t = [NAMEN.get(o, o) for o, mm in M.items() if o != b and innen(pr, mm.triangles)]
            d = 2 * math.sqrt(A / math.pi)
            zeilen.append((A, NAMEN.get(b, b), len(c), d, aus_n, -mitte[1], in_t, frei / len(c)))
    print('---', name)
    for A, nm, cnt, d, nr, h, in_t, fr in sorted(zeilen, key=lambda z: -z[0])[:16]:
        print('  %-13s Flaechen %2d  %.2f m2  Durchm %.2f  aussen (%+.2f %+.2f %+.2f)  Hoehe %.2f  frei %.2f  in %s' % (nm, cnt, A, d, nr[0], -nr[1], nr[2], h / 1000, fr, ','.join(map(str, in_t)) or '-'))
