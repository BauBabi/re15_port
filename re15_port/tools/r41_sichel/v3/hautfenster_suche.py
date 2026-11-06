import sys, numpy as np
from scipy import ndimage as nd
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
import birkin_formen as BF
from P_farbe import lab
mo = BF.modell_aus_blob(open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD','rb').read())
res=[]
for row in (0,1,2):
    A = lab(mo.pal[row][mo.tex].astype(float))
    for page in (0,1,2):
        for v0 in range(0, 220, 4):
            for u0 in range(0, 100, 4):
                bw, bh = 32, 40
                u1, v1 = u0+bw, v0+bh
                if u1>128 or v1>256: continue
                t = mo.tex[v0:v1, page*128+u0:page*128+u1]
                W = A[v0:v1, page*128+u0:page*128+u1]
                if (W[...,0] < 4).mean() > 0.01: continue        # schwarze Raender / leere Flaechen
                if (W[...,2] < -6).mean() > 0.2: continue        # Jeans
                m = W.reshape(-1,3).mean(0)
                band = W[:, bw//3:2*bw//3, 1].mean(1)
                hp = band - nd.gaussian_filter1d(band, 6, mode='nearest')
                C = np.hypot(m[1],m[2]); h = np.degrees(np.arctan2(m[2],m[1]))
                res.append((round(hp.std(),2), 480+row, page, u0, v0, u1-1, v1-1, round(m[0],1), round(C,1), round(h,1), round((W[...,1]>15).mean(),2)))
sel = [r for r in res if 15.5 <= r[7] <= 22 and 7 <= r[8] <= 12.5 and 17 <= r[9] <= 37 and r[10] <= 0.28]
sel.sort()
for r in sel[:25]: print(r)
