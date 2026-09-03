"""Q5: Prueft gen_region gegen die 480 Cuts, in denen die Kuenstler-Rechtecke bekannt sind.

Zielgroesse: IoU zwischen der zurueckgewonnenen Bildschirmregion und der Vereinigung
der Original-Maskenrechtecke. Das ist die Menge, die der Generator spaeter fuer die
238 Cuts OHNE Rechtecke selbst herleiten muss.
"""
import os, sys, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb, TBL, u32, ROOT
from q3b_tiefe import groups_masks
from gen_region import region_from_atlas

ious, covs, rows = [], [], []
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid & 1: continue
    stage=(rid>>12)&0xF
    if stage not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    for c in range(nCut):
        ms=[m for m in groups_masks(d,u32(d,cam+c*32+0x1C)) if m[3]>0 and m[4]>0]
        if len(ms)<4: continue
        A=atlas_rgb(stage,rid,c); B=bg_rgb(stage,rid,c)
        if A is None or B is None: continue
        at, idx = A
        truth=np.zeros((240,320),bool)
        for (gi,X,Y,w,h,dep) in ms:
            x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
            if x1>x0 and y1>y0: truth[y0:y1,x0:x1]=True
        got, cov = region_from_atlas(idx, at, B)
        inter=(got&truth).sum(); union=(got|truth).sum()
        if union==0: continue
        iou=inter/union
        ious.append(iou); covs.append(cov)
        rows.append((name,c,iou,cov,int(truth.sum()),int(got.sum())))

ious=np.array(ious); covs=np.array(covs)
print("Cuts %d"%len(ious))
print("IoU gegen die Kuenstler-Rechtecke: Median %.3f | >0.7 bei %.1f%% | >0.5 bei %.1f%% | <0.3 bei %.1f%%"
      %(np.median(ious),100*(ious>0.7).mean(),100*(ious>0.5).mean(),100*(ious<0.3).mean()))
print("zugeordnete Atlas-Pixel: Median %.1f%%"%(100*np.median(covs)))
rows.sort(key=lambda r:r[2])
print("\nschlechteste: "+", ".join("%s#%d IoU %.2f zug %.0f%%"%(r[0],r[1],r[2],100*r[3]) for r in rows[:5]))
print("beste       : "+", ".join("%s#%d IoU %.2f"%(r[0],r[1],r[2]) for r in rows[-5:]))
