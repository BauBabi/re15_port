#!/usr/bin/env python3
"""M1 gegen RE2 G2: Paarvergleich je Mesh. 1) exakte Vertices 2) Lage nach Abzug einer konstanten Verschiebung
3) Flaechen-Topologie 4) UV-Tupel je Flaeche (u,v ohne Seite)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from collections import Counter
M1 = zerlege(blob('M1'))
D0 = zerlege(blob('D0'))
e, *_ = re2_rec(0x31, 3); t, *_ = re2_rec(0x31, 2)
G2 = zerlege(e, t)
e, *_ = re2_rec(0x30, 3); t, *_ = re2_rec(0x30, 2)
G1 = zerlege(e, t)
def V(m):
    return [tuple(v[:3]) for v in m.tv]
def uvs(m):
    a = [((u[0], u[1]), (u[3], u[4]), (u[6], u[7])) for u in m.tuv]
    b = [((u[0], u[1]), (u[3], u[4]), (u[6], u[7]), (u[9], u[10])) for u in m.quv]
    return a, b
def uvsatz(m):
    s = set()
    for a in uvs(m):
        for f in a:
            s.add(tuple(sorted(f)))
    return s
def paar(a, b, name):
    va, vb = V(a), V(b)
    sa, sb = set(va), set(vb)
    h = len(sa & sb)
    # beste konstante Verschiebung: Mehrheits-Differenz
    c = Counter()
    for x in va:
        for y in vb:
            c[(y[0]-x[0], y[1]-x[1], y[2]-x[2])] += 1
    d, n = c.most_common(1)[0]
    ua, ub = uvsatz(a), uvsatz(b)
    print(f'  {name}: V {len(va)}/{len(vb)} exakt {h}, beste Verschiebung {d} deckt {n}; Flaechen T {len(a.tf)}/{len(b.tf)} Q {len(a.qf)}/{len(b.qf)}; UV-Flaechen gleich {len(ua&ub)}/{len(ua)}')
print('Knochen-Paare M1 <-> RE2 G2 (Beine: RE2 G2 mesh 12-17)')
for i, j in ((1,12),(2,13),(3,14),(4,15),(5,16),(6,17)):
    print(f'M1 mesh {i} gegen G2 mesh {j}')
    paar(M1.meshes[i], G2.meshes[j], 'G2')
print()
print('G2 Meshes:', [(j, len(m.tv), len(m.tf), len(m.qf)) for j, m in enumerate(G2.meshes)])
print()
print('Roh M1 mesh 3 / G2 mesh 14, sortiert:')
print(sorted(V(M1.meshes[3])))
print(sorted(V(G2.meshes[14])))
print()
print('UV-Treffer M1-Mesh gegen ALLE Meshes von G1/G2/D0 (nur >=30%):')
for i, m in enumerate(M1.meshes):
    ua = uvsatz(m)
    z = []
    for nm, q in (('D0', D0), ('G1', G1), ('G2', G2)):
        for j, n in enumerate(q.meshes):
            ub = uvsatz(n)
            if ua and len(ua & ub) / len(ua) >= 0.3:
                z.append(f'{nm}[{j}] {len(ua&ub)}/{len(ua)}')
    print(f'  {i:2d} ({len(ua)} UV-Flaechen): ' + ', '.join(z))
