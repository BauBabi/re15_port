import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import kopf_re2_bau as KB, re2_leser as RL
m = RL.modell(KB.G2).meshes[3]
V = np.array([v[:3] for v in m.tv], float)
maulT = KB.MAUL_T
cen = [V[[m.tf[i][2*k+1] for k in range(3)]].mean(0) for i in maulT]
print('Maulflaechen-Mittelpunkte y', sorted(round(c[1]) for c in cen))
def fit_rund(P):
    P = np.asarray(P, float)
    M = np.c_[P[:,0]**2 + P[:,1]**2, P[:,2]**2, P]
    s, *_ = np.linalg.lstsq(M, np.ones(len(P)), rcond=None)
    A, C, D, E, F = s
    c = -np.array([D/(2*A), E/(2*A), F/(2*C)])
    g = 1 + A*(c[0]**2 + c[1]**2) + C*c[2]**2
    return c, np.sqrt(g/np.array([A, A, C]))
def kappe_rund(V, trimm=1.15, sel=None):
    idx = np.arange(len(V)) if sel is None else np.asarray(sel)
    keep = np.ones(len(idx), bool)
    for it in range(30):
        c, ax = fit_rund(V[idx[keep]])
        r = np.linalg.norm((V - c)/ax, axis=1)
        nk = r[idx] <= trimm
        if (nk == keep).all(): break
        keep = nk
    return c, ax, r, [int(i) for i in idx[~keep]]
ym = min(c[1] for c in cen)
for s in (1.0, 0.85, 0.75, 0.65):
    W = V.copy(); xp = V[:,0].min(); W[:,0] = xp + s*(V[:,0]-xp)
    Hk = W[:,1].ptp()
    c, ax, r, raus = kappe_rund(W)
    ob = np.nonzero(W[:,1] < ym)[0]
    vor = [(int(i), round(float((np.linalg.norm(W[i]-c))*(1-1/r[i])/Hk*100),1)) for i in ob if r[i] > 1 and W[i,0] > c[0]]
    vor.sort(key=lambda t: -t[1])
    print('s', s, 'x-ext/y-ext %.3f' % (W[:,0].ptp()/Hk), 'c', c.round(0), 'ax', ax.round(0), 'raus', raus, 'vor>0:', vor[:8])
    c2, ax2, r2, raus2 = kappe_rund(W, sel=ob)
    vor = [(int(i), round(float((np.linalg.norm(W[i]-c2))*(1-1/r2[i])/Hk*100),1)) for i in ob if r2[i] > 1 and W[i,0] > c2[0]]
    vor.sort(key=lambda t: -t[1])
    print('   nur oben: c', c2.round(0), 'ax', ax2.round(0), 'raus', raus2, 'vor>0:', vor[:8])
