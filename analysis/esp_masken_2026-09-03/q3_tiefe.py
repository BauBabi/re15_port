"""Q3: Ist die von den Kuenstlern gesetzte `depth` aus der Geometrie herleitbar?

Hypothese: ein Vordergrund-Objekt steht auf dem Boden, und seine Maskentiefe ist die
Kamera-Z am FUSS der Silhouette. Also: Strahl durch die untere Kante der Maske, Schnitt
mit der Bodenebene y = y0, daraus Kamera-Z — und der Vergleich mit depth*64
(das Schwellenmodell aus re15_pri.h: Maske verdeckt, solange Figur-vz > depth*64).

Kameramathematik = Python-Fassung von re15_camera_build_view (FUN_80053ca4):
LookAt mit ganzzahliger Normalisierung, H = fov>>7, sx = 160 + H*vx/vz.
"""
import os, sys, struct, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import masks, TBL, u32, ROOT

def isqrt(n): return int(np.sqrt(float(n)))

def build_view(fov, px,py,pz, tx,ty,tz):
    dx,dy,dz = tx-px, ty-py, tz-pz
    dist = isqrt(dx*dx+dy*dy+dz*dz)
    if dist == 0: return None
    horiz = isqrt(dx*dx+dz*dz)
    sp = int((-dy*4096)//dist) if dy <= 0 else -int((dy*4096)//dist)
    cp = int((horiz*4096)//dist)
    if horiz != 0:
        sy = int(dx*4096/horiz); cy = int(dz*4096/horiz)
        R = [cy,0,-sy, (sp*sy)>>12, cp, (sp*cy)>>12, (cp*sy)>>12, -sp, (cp*cy)>>12]
    else:
        R = [4096,0,0, 0,cp,sp, 0,-sp,cp]
    t = [ (R[0]*-px + R[1]*-py + R[2]*-pz) >> 12,
          (R[3]*-px + R[4]*-py + R[5]*-pz) >> 12,
          (R[6]*-px + R[7]*-py + R[8]*-pz) >> 12 ]
    return R, t, (fov >> 7)

def vz_at_floor(R, t, H, sx, sy, y0):
    """Kamera-Z des Punktes, an dem der Sehstrahl durch (sx,sy) die Ebene y=y0 trifft."""
    a = (sx-160.0)/H; b = (sy-120.0)/H
    # W_y = (sum_j R[j*3+1]*v_j - sum_j R[j*3+1]*t_j)/4096, v = (a*vz, b*vz, vz)
    k = R[1]*a + R[4]*b + R[7]
    c = R[1]*t[0] + R[4]*t[1] + R[7]*t[2]
    if abs(k) < 1e-9: return None
    vz = (y0*4096.0 + c)/k
    return vz if vz > 1 else None

rows=[]
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid & 1: continue
    if ((rid>>12)&0xF) not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    for c in range(nCut):
        rec = cam+c*32
        ms = masks(d, u32(d, rec+0x1C))
        if not ms: continue
        fov = struct.unpack_from("<H", d, rec+2)[0]
        P = struct.unpack_from("<iii", d, rec+4)
        T = struct.unpack_from("<iii", d, rec+0x10)
        v = build_view(fov, *P, *T)
        if not v: continue
        R,t,H = v
        if H <= 0: continue
        for (sx_,sy_,X,Y,w,h,dep) in ms:
            if w<=0 or h<=0 or dep<=0: continue
            cx = X + w/2.0; by = Y + h          # untere Kante = Fuss des Objekts
            vz = vz_at_floor(R,t,H, cx, by, 0)
            if vz is None: continue
            rows.append((name,c,dep,dep*64.0,vz,w,h,X,Y))

import numpy as np
a=np.array([[r[3],r[4]] for r in rows])
print("Masken mit auswertbarer Geometrie: %d"%len(rows))
soll=a[:,0]; ist=a[:,1]
ratio = ist/soll
print("depth*64 (Soll)  Median %8.0f"%np.median(soll))
print("Boden-vz  (Ist)  Median %8.0f"%np.median(ist))
print("Verhaeltnis Ist/Soll: Median %.3f  25%%=%.3f  75%%=%.3f"%(np.median(ratio),np.percentile(ratio,25),np.percentile(ratio,75)))
print("Korrelation (Pearson, log): %.3f"%np.corrcoef(np.log(np.clip(soll,1,None)),np.log(np.clip(ist,1,None)))[0,1])
for lo,hi,lbl in [(0.8,1.25,"±25%"),(0.66,1.5,"±50%"),(0.5,2.0,"Faktor 2")]:
    print("  innerhalb %-9s: %.1f%%"%(lbl,100*((ratio>lo)&(ratio<hi)).mean()))
