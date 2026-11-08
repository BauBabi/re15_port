import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, birkin_buehne as BB
P = G.posen()
for e in sys.argv[1:]:
    mo = b2.modell(e)
    for h in ('85',):
        pose = P[h]; kf = BB.kf_von_clip(mo, pose['clip'], pose['bild']); R, Tt = mo.pose(kf)
        V = np.array([v[:3] for v in mo.meshes[9].tv], float)
        W = (R[9].astype(float) / 4096.0 @ V.T).T + Tt[9].astype(float)
        xy = b2.projiziere(pose, W)
        o = np.argsort(-xy[:, 0])[:10]
        print(e[-14:], h, [(int(i), round(float(xy[i, 0]), 1), round(float(xy[i, 1]), 1)) for i in o])
