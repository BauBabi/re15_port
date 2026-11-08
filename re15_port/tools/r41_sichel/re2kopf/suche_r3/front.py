import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np, v4_gemein as G, b2
v = G.vorlage(2); bg = G.videoraum(G.hintergrund())
d = np.abs(G.lab(v)[..., 0] - G.lab(bg)[..., 0])
X0, X1, Y0, Y1 = 440, 492, 186, 217
def front_foto():
    out = []
    for y in range(Y0, Y1 + 1):
        xs = [x for x in range(450, 483) if d[y, x] > 9]
        out.append(max(xs) if xs else X0)
    return np.array(out)
ff = front_foto(); print('Foto Vorderkante je Zeile', ff.tolist())
P = G.posen()
for e in sys.argv[1:]:
    img, ids = G.zeichne(b2.modell(e), P['85'])
    fr = []
    for y in range(Y0, Y1 + 1):
        xs = np.nonzero(ids[y, X0:X1 + 1] >= 0)[0]
        fr.append(X0 + xs.max() if len(xs) else X0)
    fr = np.array(fr)
    inter = np.minimum(fr, ff) - X0; uni = np.maximum(fr, ff) - X0
    print(e[-20:], 'Render', fr.tolist(), 'IoU %.3f' % (inter.sum() / uni.sum()), 'mittl. Ueberstand %.1f px' % (fr - ff).mean(), 'max %d' % (fr - ff).max())
