"""Q13: Taugt die KOLLISIONSGEOMETRIE als Quelle fuer die fehlenden Vordergrundregionen?

Fuer die 1468 Cuts ohne jede Vordergrunddaten gibt es keinen Atlas, aus dem sich die
Region zurueckgewinnen liesse. Einziges verbliebenes 3D-Wissen des Raums ist die
Kollision (SCA: Grundflaechen von Hindernissen) plus die Kameralage.

Gemessen wird an den 480 Cuts MIT Kuenstlermasken: wie gut deckt sich die projizierte
Kollision mit dem, was die Kuenstler als Vordergrund gezeichnet haben? SCA kennt keine
HOEHE, also wird sie als Parameter mitgefuehrt und die beste gesucht.

Ausgewiesen wird beides getrennt:
  Praezision = Anteil der projizierten Flaeche, der wirklich Vordergrund ist
               (zu wenig davon -> wir wuerden Hintergrund ueber die Figur malen)
  Ausbeute   = Anteil der Kuenstlerflaeche, den wir treffen
"""
import os, sys, glob, struct
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, u16, ROOT
from q3b_tiefe import groups_masks, build_view

def sca_entries(d):
    off = u32(d, 0x20)
    if off < 0x40 or off + 24 > len(d): return []
    cx, cz = struct.unpack_from("<HH", d, off)
    cnt = struct.unpack_from("<5I", d, off+4)
    n = sum(c & 0xFFFF for c in cnt)
    if n <= 0 or n > 4000: return []
    base = off + 24
    out = []
    for i in range(n):
        p = base + i*12
        if p + 12 > len(d): break
        w, dens, x, z = struct.unpack_from("<HHhh", d, p)
        typ, u0, u1, flr = d[p+8], d[p+9], d[p+10], d[p+11]
        out.append((x, z, w, dens, typ, flr))
    return out

def project(R, t, H, wx, wy, wz):
    vx = (wx*R[0] + wy*R[1] + wz*R[2])/4096.0 + t[0]
    vy = (wx*R[3] + wy*R[4] + wz*R[5])/4096.0 + t[1]
    vz = (wx*R[6] + wy*R[7] + wz*R[8])/4096.0 + t[2]
    if vz <= 64: return None
    return (160 + vx*H/vz, 120 + vy*H/vz, vz)

HEIGHTS = [-800, -1500, -2500, -4000]
res = {h: [0,0,0] for h in HEIGHTS}      # [treffer, unsere_flaeche, kuenstlerflaeche]
ncut = 0
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid & 1: continue
    if ((rid>>12)&0xF) not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    sca = sca_entries(d)
    if not sca: continue
    for c in range(nCut):
        rec=cam+c*32
        ms=[m for m in groups_masks(d,u32(d,rec+0x1C)) if m[3]>0 and m[4]>0]
        if len(ms)<6: continue
        fov=u16(d,rec+2); P=struct.unpack_from("<iii",d,rec+4); T=struct.unpack_from("<iii",d,rec+0x10)
        v=build_view(fov,*P,*T)
        if not v: continue
        R,t,Hs=v
        if Hs<=0: continue
        truth=np.zeros((240,320),bool)
        for (gi,X,Y,w,h,dep) in ms:
            x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
            if x1>x0 and y1>y0: truth[y0:y1,x0:x1]=True
        if truth.sum()==0: continue
        ncut += 1
        for hh in HEIGHTS:
            got=np.zeros((240,320),bool)
            for (x,z,w,dens,typ,flr) in sca:
                corners=[(x,z),(x+w,z),(x,z+dens),(x+w,z+dens)]
                pts=[]
                for (cxw,czw) in corners:
                    for yy in (0, hh):
                        pr=project(R,t,Hs,cxw,yy,czw)
                        if pr: pts.append((pr[0],pr[1]))
                if len(pts)<4: continue
                xs=[p[0] for p in pts]; ys=[p[1] for p in pts]
                x0=int(max(0,min(xs))); x1=int(min(320,max(xs)+1))
                y0=int(max(0,min(ys))); y1=int(min(240,max(ys)+1))
                if x1>x0 and y1>y0: got[y0:y1,x0:x1]=True
            r=res[hh]
            r[0]+=int((got&truth).sum()); r[1]+=int(got.sum()); r[2]+=int(truth.sum())

print("Cuts %d"%ncut)
print("Hoehe   Praezision   Ausbeute")
for hh in HEIGHTS:
    hit,mine,theirs=res[hh]
    print("  %5d    %5.1f %%     %5.1f %%"%(hh,100*hit/max(1,mine),100*hit/max(1,theirs)))
