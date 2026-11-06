import sys, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
import birkin_formen as BF
blob = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD','rb').read()
mo = BF.modell_aus_blob(blob)
for b in (10, 11, 13, 14, 8):
    m = mo.meshes[b]; ys=[]; us=[]; vs=[]
    for f, uv in zip(m.qf, m.quv):
        for k in range(4):
            v = m.qv[f[2*k+1]]; ys.append(v[1]); us.append(uv[3*k]); vs.append(uv[3*k+1])
    ys, us, vs = map(np.array, (ys, us, vs))
    print(b, 'corr(y,v)=%.2f corr(y,u)=%.2f' % (np.corrcoef(ys, vs)[0,1], np.corrcoef(ys, us)[0,1]), 'y', ys.min(), ys.max())
# Streifen Seite 2 u 43-127 v 0-135 mit Zeile 482 x4
t = mo.tex[0:136, 256+40:256+128]
img = mo.pal[2][t]
cv2.imwrite('streifen.png', cv2.resize(img[:, :, ::-1], None, fx=5, fy=5, interpolation=cv2.INTER_NEAREST))
t = mo.tex[0:150, 0:128]; img = mo.pal[0][t]
cv2.imwrite('seite0.png', cv2.resize(img[:, :, ::-1], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST))
