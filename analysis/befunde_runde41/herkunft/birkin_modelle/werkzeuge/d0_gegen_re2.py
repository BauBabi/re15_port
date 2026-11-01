#!/usr/bin/env python3
"""D0 (Disc) und D36 gegen RE2 G1-G5 (kind 0x30,0x31,0x33,0x34,0x36): stammt etwas aus RE2? Je D0-Mesh: bester RE2-Mesh
nach Anteil der D0-Vertices, die einen RE2-Vertex im Abstand <= 2 haben (ohne Verschiebung, Mesh-lokal).
TIM: RGB-16er-Kacheln an gleicher Stelle (<10 mittlere Abweichung). Dazu D36-Meshes gegen D0 (exakt gleich)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from scipy.spatial import cKDTree
D0 = zerlege(blob('D0')); D36 = zerlege(blob('D36'))
RE = {}
for k in (0x30, 0x31, 0x33, 0x34, 0x36):
    e, *_ = re2_rec(k, 3); t, *_ = re2_rec(k, 2); RE[k] = zerlege(e, t)
def pts(m): return np.array([v[:3] for v in m.tv + (m.qv if m.qv != m.tv else [])], float)
print('== D0-Meshes gegen alle RE2-G-Meshes (Anteil Vertices mit Partner <= 2 Einheiten)')
for i, m in enumerate(D0.meshes):
    A = pts(m); best = (0, None)
    for k, q in RE.items():
        for j, n in enumerate(q.meshes):
            B = pts(n)
            if len(A) == 0 or len(B) == 0: continue
            d, _ = cKDTree(B).query(A)
            f = float((d <= 2).mean())
            if f > best[0]: best = (f, f'RE2 0x{k:02X}[{j}]')
    print(f'  D0[{i:2d}] V{len(A):3d} bester {best[1]} {best[0]*100:.0f}%')
def rgb(q):
    w = q.tex.shape[1] // q.pal.shape[0]
    return np.concatenate([q.pal[k][q.tex[:, k*w:(k+1)*w]] for k in range(q.pal.shape[0])], axis=1).astype(int)
R = rgb(D0)
for k, q in RE.items():
    try: G = rgb(q)
    except Exception as ex: print('TIM', hex(k), ex); continue
    if G.shape != R.shape: print(f'  TIM D0 gegen 0x{k:02X}: Form {G.shape} != {R.shape}'); continue
    n = tot = 0
    for y in range(0, R.shape[0], 16):
        for x in range(0, R.shape[1], 16):
            a = R[y:y+16, x:x+16]
            if a.max() < 16: continue
            tot += 1; n += np.abs(a - G[y:y+16, x:x+16]).mean() < 10
    print(f'  TIM D0 gegen RE2 0x{k:02X}: Kacheln gleicher Stelle <10: {n}/{tot}; Pixelindex gleich {(D0.tex==q.tex).mean()*100:.1f}%')
print('== D36 gegen D0 (Mesh exakt gleich: Vertices, Normalen, Flaechen, UV)')
for i, m in enumerate(D36.meshes):
    gl = [j for j, n in enumerate(D0.meshes) if all(getattr(m, a) == getattr(n, a) for a in ('tv', 'tn', 'tf', 'tuv', 'qv', 'qn', 'qf', 'quv'))]
    gv = [j for j, n in enumerate(D0.meshes) if m.tv == n.tv]
    print(f'  D36[{i:2d}] V{len(m.tv):3d} T{len(m.tf):3d} Q{len(m.qf):3d} gleich D0 {gl} (nur Vertices gleich {gv})')
print('D36 TIM gegen D0: Pixelindex gleich %.1f%%, CLUT gleich %d/768' % ((D36.tex == D0.tex).mean()*100, sum(int((D36.pal[k]==D0.pal[k]).all(1).sum()) for k in range(3))))
print('D36 Skelett', D36.skel.bone_count, D36.skel.parent, 'Rel gleich D0:', D36.skel.rel == D0.skel.rel)
