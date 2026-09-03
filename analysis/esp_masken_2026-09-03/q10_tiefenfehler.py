"""Q10: Ist der Tiefenfehler ein VERSATZ (korrigierbar) oder STREUUNG (Modell zu schwach)?

q9 meldet nur 67.9 % richtige Verdeckungsentscheidungen. Bevor daran weiter geschraubt
wird, muss die Fehlerart feststehen: ein systematischer Faktor liesse sich messen und
herausrechnen, breite Streuung nicht.
"""
import os, sys, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, ROOT
from q3b_tiefe import groups_masks
from gen_masken import build_cut

def field(rects):
    f=np.zeros((240,320),np.int32)
    for (X,Y,w,h,dep) in rects:
        x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
        if x1<=x0 or y1<=y0: continue
        sub=f[y0:y1,x0:x1]
        f[y0:y1,x0:x1]=np.where((sub==0)|(dep<sub),dep,sub)
    return f

ratios=[]; percut=[]
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid&1: continue
    stage=(rid>>12)&0xF
    if stage not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    for c in range(nCut):
        ms=[m for m in groups_masks(d,u32(d,cam+c*32+0x1C)) if m[3]>0 and m[4]>0 and m[5]>0]
        if len(ms)<6: continue
        r=build_cut(d,cam,stage,rid,c)
        if not r: continue
        g,mk=r
        ours=[];gi=0;used=0
        for (x,y,w,h,dep) in mk:
            while gi<len(g) and used>=g[gi][0]: gi+=1; used=0
            ours.append((x+g[gi][1],y+g[gi][2],w,h,dep)); used+=1
        FA=field([(X,Y,w,h,dep) for (gi_,X,Y,w,h,dep) in ms]); FB=field(ours)
        sel=(FA>0)&(FB>0)
        if sel.sum()<64: continue
        rr=FB[sel].astype(float)/FA[sel]
        ratios.append(rr); percut.append(float(np.median(rr)))
r=np.concatenate(ratios); pc=np.array(percut)
print("gemeinsame Pixel: %d in %d Cuts"%(len(r),len(pc)))
print("Verhaeltnis unsere/Kuenstler-Tiefe:")
print("   Median %.3f | 25%% %.3f | 75%% %.3f | 10%% %.3f | 90%% %.3f"
      %(np.median(r),np.percentile(r,25),np.percentile(r,75),np.percentile(r,10),np.percentile(r,90)))
print("   innerhalb +-20%%: %.1f%%   innerhalb +-50%%: %.1f%%"
      %(100*((r>0.8)&(r<1.2)).mean(),100*((r>0.5)&(r<1.5)).mean()))
print("je Cut (Median):  Median %.3f | 25%% %.3f | 75%% %.3f"
      %(np.median(pc),np.percentile(pc,25),np.percentile(pc,75)))
print("   Cuts mit Cut-Median innerhalb +-20%%: %.1f%%"%(100*((pc>0.8)&(pc<1.2)).mean()))
