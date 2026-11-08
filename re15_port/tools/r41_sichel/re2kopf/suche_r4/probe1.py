import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, birkin_buehne as BB
import kopf_re2_bau as KB, re2_leser as RL
P = G.posen()
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
for nm, e in (('ref2', G.REF2), ('r3', SP + 'EM30_V5r3.EMD'), ('r2', SP + 'EM30_V4re2.EMD')):
    mo = b2.modell(e)
    for h in ('85', '135'):
        img, ids, iv = G.zeichne(mo, P[h], voll=True)
        ok, so, H = G.h_port(iv)
        kb = G.kopfbox(ids)
        x0, y0, x1, y1 = kb
        print(nm, h, 'H %.0f kopfbox %s  w %d h %d  w/h %.3f  w/H %.3f h/H %.3f' % (H, kb, x1-x0+1, y1-y0+1, (x1-x0+1)/(y1-y0+1), (x1-x0+1)/H, (y1-y0+1)/H))
# raw G2 extents
m = RL.modell(KB.G2).meshes[3]
V = np.array([v[:3] for v in m.tv], float)
print('G2 raw x', V[:,0].min(), V[:,0].max(), 'y', V[:,1].min(), V[:,1].max(), 'z', V[:,2].min(), V[:,2].max())
print('depth/height x/y %.3f' % ((V[:,0].ptp())/(V[:,1].ptp())))
