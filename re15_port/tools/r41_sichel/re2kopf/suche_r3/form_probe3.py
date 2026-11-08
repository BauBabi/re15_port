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
def kugel(P):
    M = np.c_[2*P, np.ones(len(P))]; s, *_ = np.linalg.lstsq(M, (P**2).sum(1), rcond=None)
    c = s[:3]; R = np.sqrt(s[3] + (c**2).sum()); return c, R
for x0 in (0, 50, 80, 100):
  for thr in (1.04, 1.06, 1.08):
    front = V[:, 0] > x0
    keep = front.copy()
    for it in range(20):
        c, R = kugel(V[keep]); rr = np.linalg.norm(V - c, axis=1) / R; nk = front & (rr <= thr)
        if (nk == keep).all(): break
        keep = nk
    mv = np.nonzero(front & ~keep)[0]
    ring = sorted(set().union(*[nb[v] for v in mv]) - set(mv)) if len(mv) else []
    fr = np.nonzero(front)[0]
    print('x>%d thr %.2f it %d n %d moved %s r %s | front-r p10/50/90 %s | ring r %s | c %s R %.0f' % (x0, thr, it, front.sum(), mv.tolist(), rr[mv].round(3).tolist(),
          np.percentile(rr[keep], [10, 50, 90]).round(3), {j: round(rr[j], 3) for j in ring}, c.round(0), R))
