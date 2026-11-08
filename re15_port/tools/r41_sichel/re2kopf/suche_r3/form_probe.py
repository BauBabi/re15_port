import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import kopf_re2_bau as KB, re2_leser as RL
m = RL.modell(KB.G2).meshes[3]
V = np.array([v[:3] for v in m.tv], float)
def fit(P):
    M = np.c_[P**2, P]; s, *_ = np.linalg.lstsq(M, np.ones(len(P)), rcond=None)
    A, B, C, D, E, F = s
    c = -np.array([D/(2*A), E/(2*B), F/(2*C)]); g = 1 + A*c[0]**2 + B*c[1]**2 + C*c[2]**2
    return c, np.sqrt(g/np.array([A, B, C]))
def r(P, c, ax): return np.sqrt((((P - c)/ax)**2).sum(1))
for thr in (1.10, 1.15, 1.20):
    keep = np.ones(len(V), bool)
    for it in range(10):
        c, ax = fit(V[keep]); rr = r(V, c, ax)
        nk = rr <= thr
        if (nk == keep).all(): break
        keep = nk
    print('thr', thr, 'it', it, 'raus', np.nonzero(~keep)[0].tolist(), 'c', c.round(1), 'ax', ax.round(1), 'r raus', rr[~keep].round(3))
# Nachbarn
from collections import defaultdict
nb = defaultdict(set)
for f in m.tf:
    i = [f[2*k+1] for k in range(3)]
    for a in i: nb[a] |= set(i) - {a}
for f in m.qf:
    i = [f[2*k+1] for k in range(4)]
    for a in i: nb[a] |= set(i) - {a}
for v in (7, 10, 18, 8, 30, 3, 42, 46, 4, 20):
    print(v, 'nb', sorted(nb[v]))
print('---')
keep = np.ones(len(V), bool)
for it in range(10):
    c, ax = fit(V[keep]); rr = r(V, c, ax); nk = rr <= 1.15
    if (nk == keep).all(): break
    keep = nk
mv = np.nonzero(~keep)[0]
for v in mv:
    ring = [j for j in nb[v] if keep[j]]
    print(v, 'r %.3f' % rr[v], 'ring', {j: round(rr[j], 3) for j in ring}, 'median %.3f' % np.median(rr[ring]), '|p-c| %.0f' % np.linalg.norm(V[v] - c))
H = V[:, 1].max() - V[:, 1].min()
print('Kopfhoehe', H)
