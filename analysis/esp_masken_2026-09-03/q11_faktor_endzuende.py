"""Q11: Den Tiefenfaktor END-TO-ENDE kalibrieren.

q8 hat den Faktor gegen die KUENSTLER-Regionen kalibriert. Der Generator benutzt aber
seine EIGENEN (unvollstaendigen) Regionen — und q10 zeigt dadurch einen systematischen
Versatz von ~1.32 (unsere Tiefe zu weit, weil der untere Objektrand oft fehlt und der
Bodenkontakt dadurch zu hoch geschaetzt wird).

Hier laeuft der ganze Generator je Faktor und wird an der Zielgroesse gemessen:
stimmt die Verdeckungsentscheidung mit der der Kuenstler ueberein? Getrennt
ausgewiesen wird der schaedliche Fehler (wir verdecken, das Original nicht).
"""
import os, sys, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, ROOT
from q3b_tiefe import groups_masks
import gen_masken
from gen_masken import build_cut

def field(rects):
    f=np.zeros((240,320),np.int32)
    for (X,Y,w,h,dep) in rects:
        x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
        if x1<=x0 or y1<=y0: continue
        sub=f[y0:y1,x0:x1]
        f[y0:y1,x0:x1]=np.where((sub==0)|(dep<sub),dep,sub)
    return f

TARGETS=[]
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
        if len(ms)>=6: TARGETS.append((d,cam,stage,rid,c,ms))
print("Pruefmenge: %d Cuts"%len(TARGETS))

print("Faktor  Uebereinstimmung  falsch_verdeckt  falsch_gezeigt")
for f in (0.60, 0.70, 0.75, 0.80, 0.90, 1.00):
    gen_masken.DEPTH_SAFETY = f
    ag=tot=wo=ws=0
    for (d,cam,stage,rid,c,ms) in TARGETS:
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
        da=FA[sel].astype(float)*64; db=FB[sel].astype(float)*64
        for m in np.linspace(0.4,1.8,25):
            z=da*m
            o=(z>da); u=(z>db)
            ag+=int((o==u).sum()); tot+=int(sel.sum())
            wo+=int((u&~o).sum()); ws+=int((~u&o).sum())
    print("  %.2f      %6.1f %%          %5.1f %%          %5.1f %%"
          %(f,100*ag/max(1,tot),100*wo/max(1,tot),100*ws/max(1,tot)))
