#!/usr/bin/env python3
"""Nachpruefung (Neustart 17:3x) der Kernaussagen zu M1 mit eigenen, kleinen Vergleichen:
1 Abschnitts-md5 je Fassung, 2 Mesh-Rohbytes M1 gegen D0 (Vertices, Normalen, Flaechen, UV), 3 Beine M1[1..6] gegen
RE2 G2 (kind 0x31) Mesh 12..17 als Punktmengen (bestes 1:1 nach Sortierung + Abstand), 4 EMR-Kopf/Knochen und
Keyframe-Herkunft, 5 TIM: Pixel-/CLUT-Gleichheit, RGB-Kachelvergleich 16x16 gegen G2 und D0."""
import sys, os, hashlib
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
import numpy as np
from scipy.optimize import linear_sum_assignment
M1 = zerlege(blob('M1')); D0 = zerlege(blob('D0')); D1 = zerlege(blob('D1')); M0x = zerlege(blob('M0x'))
e, s2, l2 = re2_rec(0x31, 3); t, st, lt = re2_rec(0x31, 2); G2 = zerlege(e, t)
print('RE2 G2: EMD Sektor', s2, l2, 'TIM Sektor', st, lt)
print('== 1 Abschnitte (md5[:8], Laenge)')
for nm, q in (('D0', D0), ('D1', D1), ('M1', M1), ('M0x', M0x)):
    print(' ', nm, ' '.join(f'{i}:{len(q.sek[i])}:{hashlib.md5(q.sek[i]).hexdigest()[:8]}' for i in range(q.n)))
print('== 2 Meshes M1 gegen D0 (gleich = Vertices+Normalen+Flaechen+UV identisch)')
for i in range(len(M1.meshes)):
    a = M1.meshes[i]; b = D0.meshes[i] if i < len(D0.meshes) else None
    gl = b is not None and all(getattr(a, k) == getattr(b, k) for k in ('tv', 'tn', 'tf', 'tuv', 'qv', 'qn', 'qf', 'quv'))
    print(f'  M1[{i:2d}] V{len(a.tv):3d} T{len(a.tf):3d} Q{len(a.qf):3d} | D0[{i}] V{len(b.tv) if b else -1:3d} -> {"GLEICH" if gl else "anders"}')
print('== 3 Beine M1[1..6] gegen G2[12..17]: Punktmengen, 1:1-Zuordnung (Ungarisch), max |d| je Achse')
for i, j in zip(range(1, 7), range(12, 18)):
    A = np.array([v[:3] for v in M1.meshes[i].tv], float); B = np.array([v[:3] for v in G2.meshes[j].tv], float)
    if len(A) != len(B): print('  ungleiche Zahl', i, j, len(A), len(B)); continue
    C = np.abs(A[:, None, :] - B[None, :, :]).max(2)
    r, c = linear_sum_assignment(C)
    d = np.abs(A[r] - B[c]).max(0)
    same_order = int((np.abs(A - B).max(1) == 0).sum())
    fl = (len(M1.meshes[i].tf), len(M1.meshes[i].qf), len(G2.meshes[j].tf), len(G2.meshes[j].qf))
    print(f'  M1[{i}] G2[{j}] n={len(A)} max|d| {d.astype(int).tolist()} exakt gleiche Punkte {int((C[r,c]==0).sum())}, in gleicher Reihenfolge {same_order}; T/Q M1 {fl[0]}/{fl[1]} G2 {fl[2]}/{fl[3]}')
print('== 4 Skelett und Keyframes')
for nm, q in (('D0', D0), ('M1', M1), ('G2', G2)):
    print(f'  {nm}: Knochen {q.skel.bone_count} kf_size {q.skel.kf_size} kf {q.skel.kf_count} Eltern {q.skel.parent}')
print('  EMR-Kopf+Rel D0==M1:', D0.sek[2][:8 + 6*16] == M1.sek[2][:8 + 6*16])
kd = {D0.skel.kf_data[k*84:(k+1)*84] for k in range(D0.skel.kf_count)}
km = [M1.skel.kf_data[k*84:(k+1)*84] for k in range(M1.skel.kf_count)]
print('  M1-Keyframes, die bytegleich in D0 vorkommen:', sum(x in kd for x in km), '/', len(km))
print('  Clips D0 (Start,Bilder):', [(c[0], c[1]) if isinstance(c, tuple) else c for c in D0.clips][:4], '...')
print('  Clips M1 (Start,Bilder):', [(c[0], c[1]) if isinstance(c, tuple) else c for c in M1.clips][:4], '...')
print('== 5 TIM')
def rgb(q):
    w = q.tex.shape[1] // q.pal.shape[0]
    return np.concatenate([q.pal[k][q.tex[:, k*w:(k+1)*w]] for k in range(q.pal.shape[0])], axis=1).astype(int)
for nm, q in (('D0', D0), ('G2', G2)):
    print(f'  M1 gegen {nm}: TIM-Kopf gleich {M1.tim[:20]==q.tim[:20]}, Pixelindex gleich {(M1.tex==q.tex).mean()*100:.1f}%, '
          f'CLUT-Eintraege gleich {sum(int((M1.pal[k]==q.pal[k]).all(1).sum()) for k in range(3))}/768')
R1, RG, RD = rgb(M1), rgb(G2), rgb(D0)
h, w = R1.shape[:2]
for nm, R in (('G2', RG), ('D0', RD)):
    gl = fast = tot = 0
    for y in range(0, h, 16):
        for x in range(0, w, 16):
            a = R1[y:y+16, x:x+16]
            if a.max() < 16: continue
            tot += 1
            dd = np.abs(a - R[y:y+16, x:x+16]).mean()
            gl += dd < 1.0; fast += dd < 10
    print(f'  RGB 16x16-Kacheln an GLEICHER Stelle M1~{nm}: <1 {gl}, <10 {fast} von {tot} nichtschwarzen')
