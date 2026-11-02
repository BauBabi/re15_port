import os,sys
G=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/stufe3/gen'
sys.path.insert(0,G); os.chdir(G)
import s3_pruef as P, s3_rig as S
import numpy as np
f=P.lade(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD')
r=S.Rig()
for ci,c in enumerate(f['clips']):
    n=len(c['bilder'])
    if n<=1: continue
    zmin=zmax=0; xmin=xmax=0; ymin=0; ymax=-1e9; ztief=[]
    for bi,b in enumerate(c['bilder']):
        k=f['pool'][b['kf']]; p=r.aus_keyframe(k); R,T=r.fk(p); Pk=r.punkte(p,R,T)
        A=np.vstack([x for x in Pk if len(x)])
        zmin=min(zmin,A[:,2].min()); zmax=max(zmax,A[:,2].max())
        xmin=min(xmin,A[:,0].min()); xmax=max(xmax,A[:,0].max())
        ymin=min(ymin,A[:,1].min()); ymax=max(ymax,A[:,1].max())
        ztief.append(max(abs(A[:,2].min()),A[:,2].max()))
    print(f"clip {ci:2d} n={n:3d}  quer z [{zmin:7.0f},{zmax:7.0f}]  laengs x [{xmin:7.0f},{xmax:7.0f}]  hoehe y [{ymin:7.0f},{ymax:5.0f}]  Bilder mit |z|>1500: {sum(1 for v in ztief if v>1500)}")
