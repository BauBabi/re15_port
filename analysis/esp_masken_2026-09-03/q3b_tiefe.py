"""Q3b: Tiefenmodell sauber geprueft.

Korrekturen gegenueber q3_tiefe.py:
  - C-Division schneidet gegen NULL ab, Pythons // rundet ab -> eigener trunc-Div.
  - Auswertung PRO CUT (feste Kamera), nicht global ueber alle Raeume gemischt.
  - mehrere Kandidaten fuer den Bezugspunkt der Maske.
  - zusaetzlich: ist die Tiefe innerhalb einer GRUPPE konstant?
"""
import os, sys, struct, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, u16, s16, ROOT

def tdiv(a, b):                      # C-Semantik: gegen null abschneiden
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q

def isqrt(n): return int(np.sqrt(float(n)))

def build_view(fov, px,py,pz, tx,ty,tz):
    dx,dy,dz = tx-px, ty-py, tz-pz
    dist = isqrt(dx*dx+dy*dy+dz*dz)
    if dist == 0: return None
    horiz = isqrt(dx*dx+dz*dz)
    sp = tdiv(-dy*4096, dist); cp = tdiv(horiz*4096, dist)
    if horiz != 0:
        sy = tdiv(dx*4096, horiz); cy = tdiv(dz*4096, horiz)
        R = [cy,0,-sy, (sp*sy)>>12, cp, (sp*cy)>>12, (cp*sy)>>12, -sp, (cp*cy)>>12]
    else:
        R = [4096,0,0, 0,cp,sp, 0,-sp,cp]
    t = [ (R[0]*-px + R[1]*-py + R[2]*-pz) >> 12,
          (R[3]*-px + R[4]*-py + R[5]*-pz) >> 12,
          (R[6]*-px + R[7]*-py + R[8]*-pz) >> 12 ]
    return R, t, (fov >> 7)

def vz_at_floor(R,t,H,sx,sy,y0):
    a=(sx-160.0)/H; b=(sy-120.0)/H
    k = R[1]*a + R[4]*b + R[7]
    c = R[1]*t[0] + R[4]*t[1] + R[7]*t[2]
    if abs(k) < 1e-9: return None
    vz=(y0*4096.0+c)/k
    return vz if vz>1 else None

def groups_masks(d, po):
    """wie masks(), aber mit Gruppenindex."""
    gc=u16(d,po); mc=u16(d,po+2)
    if gc==0xFFFF or gc==0 or mc==0 or gc>256: return []
    gn=[];gdx=[];gdy=[]; p=po+4
    for i in range(gc):
        gn.append(u16(d,p)); gdx.append(s16(d,p+4)); gdy.append(s16(d,p+6)); p+=8
    out=[];gi=0;used=0
    for _ in range(sum(gn)):
        if p+8>len(d): break
        sx,sy,dx,dy=d[p],d[p+1],d[p+2],d[p+3]
        dep=u16(d,p+4); size=u16(d,p+6); p+=8
        if (size&0xf000)==0: w=u16(d,p); h=u16(d,p+2); p+=4
        else: w=h=(size>>12)*8
        while gi<gc and used>=gn[gi]: gi+=1; used=0
        ax=gdx[gi] if gi<gc else 0; ay=gdy[gi] if gi<gc else 0
        used+=1
        X=((dx+ax+0x8000)&0xffff)-0x8000; Y=((dy+ay+0x8000)&0xffff)-0x8000
        out.append((gi,X,Y,w,h,dep))
    return out

REF = {"unterkante":lambda X,Y,w,h:(X+w/2.0, Y+h),
       "mitte":      lambda X,Y,w,h:(X+w/2.0, Y+h/2.0),
       "oberkante":  lambda X,Y,w,h:(X+w/2.0, Y)}
cor = {k:[] for k in REF}
grp_spread=[]; ncuts=0
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
        if len(ms)<6: continue
        fov=u16(d,rec+2); P=struct.unpack_from("<iii",d,rec+4); T=struct.unpack_from("<iii",d,rec+0x10)
        v=build_view(fov,*P,*T)
        if not v: continue
        R,t,H=v
        if H<=0: continue
        ncuts+=1
        # Streuung der Tiefe INNERHALB einer Gruppe
        byg={}
        for (gi,X,Y,w,h,dep) in ms: byg.setdefault(gi,[]).append(dep)
        for gi,ds in byg.items():
            if len(ds)>=3: grp_spread.append((max(ds)-min(ds))/max(1.0,np.mean(ds)))
        for key,f in REF.items():
            xs=[];ys=[]
            for (gi,X,Y,w,h,dep) in ms:
                if dep<=0 or w<=0 or h<=0: continue
                sx,sy=f(X,Y,w,h)
                vz=vz_at_floor(R,t,H,sx,sy,0)
                if vz is None: continue
                xs.append(dep*64.0); ys.append(vz)
            if len(xs)>=6 and np.std(xs)>1e-6 and np.std(ys)>1e-6:
                cor[key].append(np.corrcoef(xs,ys)[0,1])

print("ausgewertete Cuts: %d"%ncuts)
for k in REF:
    a=np.array(cor[k])
    print("  Bezugspunkt %-11s: Korrelation je Cut  Median %+.3f   >0.8 bei %.1f%% der Cuts   <0.3 bei %.1f%%"
          %(k,np.median(a),100*(a>0.8).mean(),100*(a<0.3).mean()))
g=np.array(grp_spread)
print("\nTiefenstreuung innerhalb einer Gruppe (max-min)/Mittel: Median %.3f, <10%% bei %.1f%% der Gruppen"
      %(np.median(g),100*(g<0.1).mean()))
