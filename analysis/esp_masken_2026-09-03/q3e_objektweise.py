"""Q3e: Bodenkontakt je ZUSAMMENHAENGENDEM OBJEKT (Zusammenhangskomponente der Abdeckung).

Vorher wurden alle Masken einer Bildspalte zusammengeworfen; stehen dort zwei Objekte
in verschiedener Entfernung, ist die Unterkante die des vorderen und der Wert fuer das
hintere falsch. Jetzt: Abdeckungskarte -> Zusammenhangskomponenten -> je Komponente der
Bodenkontakt in der Spalte der jeweiligen Kachel.

Zielgroesse ist NICHT "die Zahl der Kuenstler nachbauen", sondern die FUNKTIONALE
Uebereinstimmung: liefert das Modell dieselbe Verdeckungsentscheidung
(Figur-vz > depth*64) wie das Original — ausgewertet ueber die Kamera-Z-Spanne,
die im jeweiligen Cut ueberhaupt vorkommt?
"""
import os, sys, struct, glob
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, u16, ROOT
from q3b_tiefe import groups_masks, build_view, vz_at_floor

cors, ratios, agree, ncut = [], [], [], 0
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid & 1: continue
    if ((rid>>12)&0xF) not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    for c in range(nCut):
        rec=cam+c*32
        ms=[m for m in groups_masks(d,u32(d,rec+0x1C)) if m[5]>0 and m[3]>0 and m[4]>0]
        if len(ms)<6: continue
        fov=u16(d,rec+2); P=struct.unpack_from("<iii",d,rec+4); T=struct.unpack_from("<iii",d,rec+0x10)
        v=build_view(fov,*P,*T)
        if not v: continue
        R,t,H=v
        if H<=0: continue
        cov=np.zeros((240,320),bool)
        for (gi,X,Y,w,h,dep) in ms:
            x0,x1=max(0,X),min(320,X+w); y0_,y1=max(0,Y),min(240,Y+h)
            if x1>x0 and y1>y0_: cov[y0_:y1,x0:x1]=True
        lab,n=ndimage.label(cov, structure=np.ones((3,3)))
        if n==0: continue
        # unterste bedeckte Zeile je (Komponente, Spalte)
        bot={}
        for k in range(1,n+1):
            m=(lab==k)
            col=np.where(m.any(0))[0]
            b=np.full(320,-1,int)
            for x in col: b[x]=np.where(m[:,x])[0].max()
            bot[k]=b
        xs=[];ys=[]
        for (gi,X,Y,w,h,dep) in ms:
            cx=int(np.clip(X+w/2.0,0,319)); cy=int(np.clip(Y+h/2.0,0,239))
            k=lab[cy,cx]
            if k==0: continue
            yb=bot[k][cx]
            if yb<0 or yb>=239: continue
            vz=vz_at_floor(R,t,H,cx+0.5,float(yb),0)
            if vz is None: continue
            xs.append(dep*64.0); ys.append(vz)
        if len(xs)<6: continue
        xs=np.array(xs); ys=np.array(ys)
        if np.std(xs)<1e-6 or np.std(ys)<1e-6: continue
        cors.append(np.corrcoef(xs,ys)[0,1]); ratios.append(np.median(ys/xs)); ncut+=1
        # funktionale Uebereinstimmung: gleiche Verdeckungsentscheidung ueber die
        # Z-Spanne des Cuts (100 Stufen zwischen min/max der Original-Schwellen)
        lo,hi=xs.min()*0.5, xs.max()*1.5
        probe=np.linspace(lo,hi,100)
        a=(probe[None,:]>xs[:,None]); b=(probe[None,:]>ys[:,None])
        agree.append((a==b).mean())

cors=np.array(cors); ratios=np.array(ratios); agree=np.array(agree)
print("Cuts %d"%ncut)
print("Korrelation je Cut : Median %+.3f | >0.8 bei %.1f%% | >0.5 bei %.1f%%"
      %(np.median(cors),100*(cors>0.8).mean(),100*(cors>0.5).mean()))
print("Verhaeltnis Ist/Soll: Median %.2f  (25%%=%.2f 75%%=%.2f)"
      %(np.median(ratios),np.percentile(ratios,25),np.percentile(ratios,75)))
print("FUNKTIONALE Uebereinstimmung der Verdeckungsentscheidung: Median %.1f%% | >90%% bei %.1f%% der Cuts | <70%% bei %.1f%%"
      %(100*np.median(agree),100*(agree>0.9).mean(),100*(agree<0.7).mean()))
