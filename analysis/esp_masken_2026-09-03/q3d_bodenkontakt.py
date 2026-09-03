"""Q3d: Die physikalisch richtige Fassung — Tiefe = Bodenkontakt der REGION, nicht der Kachel.

Der Uebergang "Figur verschwindet" kann nur dort stattfinden, wo das Objekt den BODEN
beruehrt. Fuer eine Kachel MITTEN im Objekt ist ihre eigene Unterkante bedeutungslos;
massgeblich ist die Unterkante der zusammenhaengenden Maskenregion in derselben Spalte.

Gemessen je Cut: Korrelation und Verhaeltnis zwischen depth*64 und der Kamera-Z des
Bodenpunktes unter der Regionsunterkante. Zusaetzlich: Empfindlichkeit gegenueber der
angenommenen Bodenhoehe y0 (die Raeume haben verschiedene Ebenen).
"""
import os, sys, struct, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, u16, ROOT
from q3b_tiefe import groups_masks, build_view, vz_at_floor

def eval_y0(y0):
    cors, ratios, ncut = [], [], 0
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
            ms=groups_masks(d,u32(d,rec+0x1C))
            ms=[m for m in ms if m[5]>0 and m[3]>0 and m[4]>0]
            if len(ms)<6: continue
            fov=u16(d,rec+2); P=struct.unpack_from("<iii",d,rec+4); T=struct.unpack_from("<iii",d,rec+0x10)
            v=build_view(fov,*P,*T)
            if not v: continue
            R,t,H=v
            if H<=0: continue
            # Abdeckungskarte -> unterste bedeckte Zeile je Spalte
            bottom=np.full(320,-1,int)
            for (gi,X,Y,w,h,dep) in ms:
                x0=max(0,X); x1=min(320,X+w); y1=min(240,Y+h)
                if x1>x0 and y1>0: bottom[x0:x1]=np.maximum(bottom[x0:x1],y1)
            xs=[];ys=[]
            for (gi,X,Y,w,h,dep) in ms:
                cx=int(np.clip(X+w/2.0,0,319))
                yb=bottom[cx]
                if yb<0: continue
                if yb>=239: continue                     # Objekt laeuft aus dem Bild -> kein Bodenkontakt
                vz=vz_at_floor(R,t,H,cx+0.5,float(yb),y0)
                if vz is None: continue
                xs.append(dep*64.0); ys.append(vz)
            if len(xs)>=6 and np.std(xs)>1e-6 and np.std(ys)>1e-6:
                cors.append(np.corrcoef(xs,ys)[0,1])
                ratios.append(np.median(np.array(ys)/np.array(xs)))
                ncut+=1
    return np.array(cors), np.array(ratios), ncut

for y0 in (0, -500, -1000, 500, 1000):
    c,r,n = eval_y0(y0)
    print("y0=%+6d  Cuts %3d | Korrelation Median %+.3f  >0.8 bei %.1f%% | Verhaeltnis Ist/Soll Median %.2f"
          %(y0,n,np.median(c),100*(c>0.8).mean(),np.median(r)))
