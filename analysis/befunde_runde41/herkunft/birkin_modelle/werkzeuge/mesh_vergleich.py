#!/usr/bin/env python3
"""Je Mesh von M1 (Mod CDEMD1 Typ 0x30): bestes Gegenstueck in D0, RE2 G1..G5, nach exakter Vertex-Uebereinstimmung.
Vertex = (x,y,z) aus MD1 (Tri-Block, Quad-Block getrennt betrachtet: bei EM30 teilen beide einen Block)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from collections import Counter
Q = {}
Q['M1'] = zerlege(blob('M1'))
Q['D0'] = zerlege(blob('D0'))
Q['D36'] = zerlege(blob('D36'))
for kind in (0x30, 0x31, 0x33, 0x34, 0x36):
    e, *_ = re2_rec(kind, 3); t, *_ = re2_rec(kind, 2)
    Q[f'R{kind:02X}'] = zerlege(e, t)

def vs(m):
    a = [tuple(v[:3]) for v in m.tv]
    b = [tuple(v[:3]) for v in m.qv]
    return a if a == b or not b else a + b
def faces(m):
    return len(m.tf), len(m.qf)

ziel = Q['M1']
print('M1-Mesh: Verts Tri Quad | bestes D0 (Treffer%) | bestes RE2 je kind (Treffer%)')
for i, m in enumerate(ziel.meshes):
    v = set(vs(m))
    z = f'{i:2d}: V={len(vs(m)):3d} T={len(m.tf):3d} Q={len(m.qf):3d} |'
    for name in ('D0', 'R30', 'R31', 'R33', 'R34', 'D36'):
        best = (0, -1, 0)
        for j, n in enumerate(Q[name].meshes):
            w = set(vs(n))
            if not w: continue
            s = len(v & w)
            if s > best[0]:
                best = (s, j, len(w))
        p = 100*best[0]/max(1, len(v))
        z += f' {name}[{best[1]:2d}]{p:5.1f}%'
    print(z)
print()
print('RE2 G1 Meshes:')
for j, n in enumerate(Q['R30'].meshes):
    print(f'  {j:2d}: V={len(vs(n)):3d} T={len(n.tf):3d} Q={len(n.qf):3d}')
print('D0 Meshes:')
for j, n in enumerate(Q['D0'].meshes):
    print(f'  {j:2d}: V={len(vs(n)):3d} T={len(n.tf):3d} Q={len(n.qf):3d}')
