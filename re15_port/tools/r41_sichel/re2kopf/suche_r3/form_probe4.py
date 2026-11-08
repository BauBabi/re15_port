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
keep = np.ones(len(V), bool)
for it in range(10):
    c, ax = fit(V[keep]); U = (V - c) / ax; rr = np.linalg.norm(U, axis=1); nk = rr <= 1.15
    if (nk == keep).all(): break
    keep = nk
ux = U[:, 0] / rr
for cosmin in (0.3, 0.5, 0.7):
    mv = np.nonzero((rr > 1.0) & (ux > cosmin))[0]
    print(cosmin, 'moved', mv.tolist(), {int(i): (round(rr[i], 3), round(ux[i], 2)) for i in mv})
print('x>100 vertices', {int(i): (round(rr[i], 3), round(ux[i], 2)) for i in np.nonzero(V[:, 0] > 100)[0]})
