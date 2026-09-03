"""Q9: Der GESAMTE Generator gegen die Kuenstlerdaten — die Abnahmepruefung.

Bisher wurden Einzelteile geprueft (Platzierung, Praezision, Tiefenfaktor). Hier laeuft
build_cut auf den 480 Cuts, die MASKEN HABEN, und das Ergebnis wird mit dem der
Kuenstler verglichen. Das ist die Zielmenge, die der Nutzer von Anfang an gefordert
hat: erst reproduzieren, was bekannt ist, dann dem Generator auf dem Unbekannten glauben.

Verglichen wird die VERDECKUNGSENTSCHEIDUNG je Bildpixel ueber die Kamera-Z-Spanne:
  beide_kennen   : Pixel, das beide als Vordergrund fuehren -> stimmen die Schwellen?
  nur_kuenstler  : wir verpassen es          -> Verhalten wie heute (harmlos)
  nur_wir        : wir erfinden Vordergrund  -> POTENZIELL SICHTBAR SCHLIMMER
"""
import os, sys, glob, struct
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, ROOT
from q3b_tiefe import groups_masks
from gen_masken import build_cut

def depth_field(rects):
    """Bildpixel -> kleinste (naechste) Tiefe der ueberdeckenden Rechtecke, 0 = keine."""
    f = np.zeros((240,320), np.int32)
    for (X,Y,w,h,dep) in rects:
        x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
        if x1<=x0 or y1<=y0: continue
        sub=f[y0:y1,x0:x1]
        f[y0:y1,x0:x1]=np.where((sub==0)|(dep<sub), dep, sub)
    return f

agree=tot=0; only_a=only_b=both=0; cuts=0
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
        ms=[m for m in groups_masks(d,u32(d,cam+c*32+0x1C)) if m[3]>0 and m[4]>0 and m[5]>0]
        if len(ms)<6: continue
        r=build_cut(d,cam,stage,rid,c)
        if not r: continue
        g,mk=r
        # unsere Rechtecke in Bildkoordinaten
        ours=[]
        gi=0; used=0
        for (x,y,w,h,dep) in mk:
            while gi<len(g) and used>=g[gi][0]: gi+=1; used=0
            dx,dy=g[gi][1],g[gi][2]; used+=1
            ours.append((x+dx,y+dy,w,h,dep))
        FA=depth_field([(X,Y,w,h,dep) for (gi_,X,Y,w,h,dep) in ms])
        FB=depth_field(ours)
        a=FA>0; b=FB>0
        only_a+=int((a&~b).sum()); only_b+=int((b&~a).sum()); both+=int((a&b).sum())
        sel=a&b
        if sel.sum()==0: continue
        da=FA[sel].astype(float)*64; db=FB[sel].astype(float)*64
        probe=np.linspace(0.4,1.8,25)
        for f in probe:
            z=da*f
            agree+=int(((z>da)==(z>db)).sum()); tot+=int(sel.sum())
        cuts+=1

print("Cuts %d"%cuts)
print("Bildpixel:  beide %d | nur Kuenstler (verpasst) %d | nur wir (erfunden) %d"%(both,only_a,only_b))
print("            -> wir decken %.1f %% der Kuenstlerflaeche ab, und %.1f %% unserer Flaeche"
      " liegt ausserhalb ihrer"%(100*both/max(1,both+only_a), 100*only_b/max(1,both+only_b)))
print("VERDECKUNGSENTSCHEIDUNG auf gemeinsamen Pixeln: %.1f %% Uebereinstimmung"%(100*agree/max(1,tot)))
