import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import kopf_re2_bau as KB, re2_leser as RL
m = RL.modell(KB.G2).meshes[3]
V = np.array([v[:3] for v in m.tv], float); N = np.array([n[:3] for n in m.tn], float)
from collections import defaultdict
nv = defaultdict(set)
for f, n in ((f, 3) for f in m.tf):
    for k in range(n): nv[f[2*k]].add(f[2*k+1])
for f in m.qf:
    for k in range(4): nv[f[2*k]].add(f[2*k+1])
print('Normale -> Ecken', {k: sorted(v) for k, v in sorted(nv.items()) if k in range(48)})
def fn(V, wind):
    acc = np.zeros((len(N), 3))
    for f in m.tf:
        i = [f[2*k+1] for k in range(3)]; ni = [f[2*k] for k in range(3)]
        n = np.cross(V[i[1]] - V[i[0]], V[i[2]] - V[i[0]]) * wind
        for j in ni: acc[j] += n
    for f in m.qf:
        i = [f[2*k+1] for k in range(4)]; ni = [f[2*k] for k in range(4)]
        for t in ((0, 1, 3), (0, 3, 2)):
            n = np.cross(V[i[t[1]]] - V[i[t[0]]], V[i[t[2]]] - V[i[t[0]]]) * wind
            for j in t: acc[ni[j]] += n
    return acc / np.linalg.norm(acc, axis=1, keepdims=True)
Nn = N / np.linalg.norm(N, axis=1, keepdims=True)
for w in (1, -1):
    F = fn(V, w)
    ang = np.degrees(np.arccos(np.clip((F * Nn).sum(1), -1, 1)))
    print('wind', w, 'Winkel p50/p90/max', np.percentile(ang, [50, 90, 100]).round(1))
