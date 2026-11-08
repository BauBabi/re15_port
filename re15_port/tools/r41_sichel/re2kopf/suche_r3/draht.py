import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import matplotlib; matplotlib.use('Agg'); import matplotlib.pyplot as plt
import kopf_re2_bau as KB, re2_leser as RL
p = json.loads(json.dumps(KB.PARAM_R3))
if len(sys.argv) > 2: p['form'].update(json.loads(sys.argv[2]))
m = RL.modell(KB.G2).meshes[3]
V, V2, N, info = KB.g2_geometrie(p)
kanten = set()
for f in m.tf:
    i = [f[2*k+1] for k in range(3)]; kanten |= {tuple(sorted((i[a], i[(a+1) % 3]))) for a in range(3)}
for f in m.qf:
    i = [f[2*k+1] for k in range(4)]; kanten |= {tuple(sorted(e)) for e in ((i[0], i[1]), (i[1], i[3]), (i[3], i[2]), (i[2], i[0]))}
fig, axs = plt.subplots(2, 3, figsize=(18, 11))
for row, W in enumerate((V, V2)):
    for ax, (a, b, nm) in zip(axs[row], ((0, 1, 'seitlich x / -y'), (0, 2, 'oben x / z'), (2, 1, 'vorn z / -y'))):
        for e in kanten:
            ax.plot(W[list(e), a], -W[list(e), b] if b == 1 else W[list(e), b], 'k-', lw=0.6)
        for i, q in enumerate(W):
            col = 'r' if info and i in info['bewegt'] else 'b'
            ax.text(q[a], -q[b] if b == 1 else q[b], str(i), color=col, fontsize=7)
        c, axl = np.array(info['kappe_c']), np.array(info['kappe_ax'])
        t = np.linspace(0, 2*np.pi, 200)
        ax.plot(c[a] + axl[a]*np.cos(t), (-(c[b] + axl[b]*np.sin(t))) if b == 1 else c[b] + axl[b]*np.sin(t), 'g--', lw=0.8)
        ax.set_aspect('equal'); ax.set_title(('Quelle ' if row == 0 else 'gerundet ') + nm)
plt.tight_layout(); plt.savefig(sys.argv[1], dpi=70)
