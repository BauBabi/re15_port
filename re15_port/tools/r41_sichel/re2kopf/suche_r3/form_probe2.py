import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import kopf_re2_bau as KB, re2_leser as RL
from collections import defaultdict
m = RL.modell(KB.G2).meshes[3]
V = np.array([v[:3] for v in m.tv], float)
nb = defaultdict(set)
for f in m.tf:
    i = [f[2*k+1] for k in range(3)]
    for a in i: nb[a] |= set(i) - {a}
for f in m.qf:
    i = [f[2*k+1] for k in range(4)]
    for a in i: nb[a] |= set(i) - {a}
def fit(P):
    M = np.c_[P**2, P]; s, *_ = np.linalg.lstsq(M, np.ones(len(P)), rcond=None)
    A, B, C, D, E, F = s
    c = -np.array([D/(2*A), E/(2*B), F/(2*C)]); g = 1 + A*c[0]**2 + B*c[1]**2 + C*c[2]**2
    return c, np.sqrt(g/np.array([A, B, C]))
def r(P, c, ax): return np.sqrt((((P - c)/ax)**2).sum(1))
for x0 in (-300, 0, 50, 80):
  for thr in (1.08, 1.10, 1.15):
    front = V[:, 0] > x0
    keep = front.copy()
    for it in range(20):
        c, ax = fit(V[keep]); rr = r(V, c, ax); nk = front & (rr <= thr)
        if (nk == keep).all(): break
        keep = nk
    mv = np.nonzero(front & ~keep)[0]
    ring = sorted(set().union(*[nb[v] for v in mv]) - set(mv)) if len(mv) else []
    print('x>%d thr %.2f it %d n %d moved %s r %s | ring r %s | c %s ax %s' % (x0, thr, it, front.sum(), mv.tolist(), rr[mv].round(2).tolist(),
          {j: round(rr[j], 2) for j in ring}, c.round(0), ax.round(0)))
