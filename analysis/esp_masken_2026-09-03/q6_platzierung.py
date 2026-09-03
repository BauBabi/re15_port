"""Q6: Unverzerrtes Mass fuer die Platzierung — PIXELWEISE Wahrheit statt Rechteckvereinigung.

Q5 mass gegen die Vereinigung der Kuenstler-RECHTECKE. Die ist gruober als die echte
Silhouette (achsenparallele Kaesten ueber unregelmaessigen Formen), also kann selbst
eine perfekte Rueckgewinnung dort nie IoU 1.0 erreichen — ich haette gegen ein
unerreichbares Ziel optimiert.

Hier die exakte Wahrheit: jedes Rechteck bildet Atlas(src..src+w) auf Bild(dst..dst+w)
ab, also ist fuer jedes von einem Rechteck bedeckte Atlas-Pixel die Zielposition
BEKANNT. Gemessen wird:
   Obergrenze  = IoU(Atlas-Pixel an ihrer WAHREN Stelle, Rechteckvereinigung)
   Genauigkeit = Anteil der zugeordneten Pixel, die exakt richtig landen
   Abdeckung   = Anteil der wahrheitsbekannten Pixel, die ueberhaupt zugeordnet wurden
"""
import os, sys, glob, struct
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb, TBL, u32, ROOT
from gen_region import find_shifts

def rects_with_src(d, po):
    gc=struct.unpack_from("<H",d,po)[0]
    if gc==0xFFFF or gc==0 or gc>256: return []
    p=po+4; gn=[];gdx=[];gdy=[]
    for i in range(gc):
        gn.append(struct.unpack_from("<H",d,p)[0])
        gdx.append(struct.unpack_from("<h",d,p+4)[0])
        gdy.append(struct.unpack_from("<h",d,p+6)[0]); p+=8
    out=[];gi=0;used=0
    for _ in range(sum(gn)):
        if p+8>len(d): break
        sx,sy,dx,dy=d[p],d[p+1],d[p+2],d[p+3]
        dep=struct.unpack_from("<H",d,p+4)[0]; size=struct.unpack_from("<H",d,p+6)[0]; p+=8
        if (size&0xf000)==0: w=struct.unpack_from("<H",d,p)[0]; h=struct.unpack_from("<H",d,p+2)[0]; p+=4
        else: w=h=(size>>12)*8
        while gi<gc and used>=gn[gi]: gi+=1; used=0
        ax=gdx[gi] if gi<gc else 0; ay=gdy[gi] if gi<gc else 0
        used+=1
        out.append((sx,sy,((dx+ax+0x8000)&0xffff)-0x8000,((dy+ay+0x8000)&0xffff)-0x8000,w,h,dep))
    return out

lum=lambda a:a[...,0]*0.299+a[...,1]*0.587+a[...,2]*0.114
ceil_, acc_, cov_, rows = [], [], [], []
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
        rs=rects_with_src(d,u32(d,cam+c*32+0x1C))
        if len(rs)<4: continue
        A=atlas_rgb(stage,rid,c); B=bg_rgb(stage,rid,c)
        if A is None or B is None: continue
        at, idx = A
        ah,aw = idx.shape
        opaque = idx!=0
        # wahre Zuordnung Atlas-Pixel -> Bildpixel
        tx=np.full((ah,aw),-10000,np.int32); ty=np.full((ah,aw),-10000,np.int32)
        union=np.zeros((240,320),bool)
        for (sx,sy,X,Y,w,h,dep) in rs:
            if w<=0 or h<=0 or sy+h>ah or sx+w>aw: continue
            yy,xx=np.mgrid[sy:sy+h, sx:sx+w]
            tx[sy:sy+h, sx:sx+w]=xx-sx+X; ty[sy:sy+h, sx:sx+w]=yy-sy+Y
            x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
            if x1>x0 and y1>y0: union[y0:y1,x0:x1]=True
        known = opaque & (tx>-9999)
        if known.sum()<64 or union.sum()==0: continue
        # Obergrenze
        true_scr=np.zeros((240,320),bool)
        ys,xs=np.nonzero(known); by,bx=ty[ys,xs],tx[ys,xs]
        ok=(by>=0)&(by<240)&(bx>=0)&(bx<320)
        true_scr[by[ok],bx[ok]]=True
        ceil_.append((true_scr&union).sum()/max(1,(true_scr|union).sum()))
        # eigene Rueckgewinnung
        got_x=np.full((ah,aw),-10000,np.int32); got_y=np.full((ah,aw),-10000,np.int32)
        for dx,dy,m,sc in find_shifts(lum(at), opaque.copy(), lum(B)):
            yy,xx=np.nonzero(m); got_x[yy,xx]=xx+dx; got_y[yy,xx]=yy+dy
        assigned = known & (got_x>-9999)
        if assigned.sum()==0:
            acc_.append(0.0); cov_.append(0.0); rows.append((name,c,0.0,0.0)); continue
        ys,xs=np.nonzero(assigned)
        right=((got_x[ys,xs]==tx[ys,xs])&(got_y[ys,xs]==ty[ys,xs])).mean()
        acc_.append(right); cov_.append(assigned.sum()/known.sum())
        rows.append((name,c,right,assigned.sum()/known.sum()))

import numpy as np
c=np.array(ceil_); a=np.array(acc_); v=np.array(cov_)
print("Cuts %d"%len(a))
print("OBERGRENZE  IoU(Wahrheit, Rechteckvereinigung): Median %.3f  — mehr ist mit Silhouetten nicht drin"%np.median(c))
print("GENAUIGKEIT zugeordneter Pixel exakt richtig  : Median %.3f | >0.95 bei %.1f%% der Cuts | <0.5 bei %.1f%%"
      %(np.median(a),100*(a>0.95).mean(),100*(a<0.5).mean()))
print("ABDECKUNG   wahrheitsbekannte Pixel zugeordnet: Median %.3f"%np.median(v))
rows.sort(key=lambda r:r[2])
print("\nschlechteste: "+", ".join("%s#%d %.2f/%.2f"%(r[0],r[1],r[2],r[3]) for r in rows[:6]))
