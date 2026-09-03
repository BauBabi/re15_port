"""Q3c: Ist die Kuenstler-Tiefe ein zusammenhaengendes TIEFENFELD ueber dem Bild?

Physik: jedes sichtbare Pixel zeigt genau EINE Oberflaeche, also MUSS depth eine
Funktion der Bildposition sein. Fuer eine Ebene im Raum ist 1/vz linear in (sx,sy).
Gemessen wird je Cut das Bestimmtheitsmass R^2 von
    1/depth  ~  a*sx + b*sy + c            (eine Ebene)
    1/depth  ~  quadratisch in (sx,sy)     (gekruemmt/mehrere Ebenen)
Dazu die Gegenprobe: wie gut erklaert der blosse MITTELWERT (R^2=0)?

Hoher R^2 => die Tiefe laesst sich aus der Bildgeometrie rekonstruieren.
Niedriger R^2 => die Kuenstler setzten pro Objekt eigene Werte, die kein
glattes Feld bilden — dann ist automatische Herleitung aus dem Bild allein aussichtslos.
"""
import os, sys, struct, glob
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, u16, s16, ROOT
from q3b_tiefe import groups_masks

def r2(X, y):
    X = np.column_stack([X, np.ones(len(y))])
    beta, *_ = np.linalg.lstsq(X, y, rcond=None)
    res = y - X @ beta
    ss = ((y - y.mean())**2).sum()
    return 1.0 - (res**2).sum()/ss if ss > 1e-12 else np.nan

lin, quad, ncut, nmask = [], [], 0, 0
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid & 1: continue
    if ((rid>>12)&0xF) not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    for c in range(nCut):
        ms = groups_masks(d, u32(d, cam+c*32+0x1C))
        pts=[(X+w/2.0, Y+h/2.0, dep) for (gi,X,Y,w,h,dep) in ms if dep>0 and w>0 and h>0]
        if len(pts) < 10: continue
        P=np.array(pts,float)
        sx,sy,dep = P[:,0],P[:,1],P[:,2]
        inv = 1.0/dep
        a = r2(np.column_stack([sx,sy]), inv)
        b = r2(np.column_stack([sx,sy,sx*sx,sy*sy,sx*sy]), inv)
        if np.isfinite(a) and np.isfinite(b):
            lin.append(a); quad.append(b); ncut += 1; nmask += len(pts)

lin=np.array(lin); quad=np.array(quad)
print("Cuts %d, Masken %d"%(ncut,nmask))
print("R^2  Ebene      : Median %.3f | >0.7 bei %.1f%% | >0.9 bei %.1f%% | <0.3 bei %.1f%%"
      %(np.median(lin),100*(lin>0.7).mean(),100*(lin>0.9).mean(),100*(lin<0.3).mean()))
print("R^2  quadratisch: Median %.3f | >0.7 bei %.1f%% | >0.9 bei %.1f%% | <0.3 bei %.1f%%"
      %(np.median(quad),100*(quad>0.7).mean(),100*(quad>0.9).mean(),100*(quad<0.3).mean()))
