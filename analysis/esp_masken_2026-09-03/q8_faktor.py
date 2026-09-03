"""Q8: Den Tiefen-Faktor MESSEN statt raten.

Der erste Ansatz benutzte 1.10 "zur Sicherheit". Die Sonde zeigte, was das kostet:
in ROOM3040 Cut 0 liegen alle erreichbaren Punkte hinter der Maske bei vz 8882..9457,
die Schwelle aber bei 9984 — die Maske verdeckt NIE. Ein geratener Faktor hat also
die ganze Wirkung aufgefressen.

Hier wird der Faktor an den 480 Kuenstler-Cuts kalibriert. Entscheidend ist nicht die
Trefferquote allein, sondern die ASYMMETRIE der beiden Fehler:
  falsch_verdeckt  = wir verdecken, das Original nicht  -> SICHTBAR SCHLIMMER
  falsch_gezeigt   = wir verdecken nicht, das Original schon -> wie der Ist-Zustand
Gesucht ist der groesste Nutzen bei praktisch null falsch_verdeckt.
"""
import os, sys, glob, struct
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import TBL, u32, u16, ROOT
from q3b_tiefe import groups_masks, build_view, vz_at_floor

FACTORS = [0.70, 0.80, 0.85, 0.90, 0.95, 1.00, 1.10, 1.25]
agree = {f: [0,0] for f in FACTORS}
wrong_occ = {f: 0 for f in FACTORS}
wrong_show= {f: 0 for f in FACTORS}

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
            x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
            if x1>x0 and y1>y0: cov[y0:y1,x0:x1]=True
        lab,n=ndimage.label(cov, structure=np.ones((3,3)))
        if n==0: continue
        bot={}
        for k in range(1,n+1):
            m=(lab==k); b=np.full(320,-1,int)
            for x in np.where(m.any(0))[0]: b[x]=int(np.where(m[:,x])[0].max())
            bot[k]=b
        for (gi,X,Y,w,h,dep) in ms:
            cx=int(np.clip(X+w/2.0,0,319)); cy=int(np.clip(Y+h/2.0,0,239))
            k=lab[cy,cx]
            if k==0: continue
            yb=bot[k][cx]
            if yb<0 or yb>=239: continue
            vz=vz_at_floor(R,t,H,cx+0.5,float(yb),0)
            if vz is None: continue
            soll=dep*64.0
            probe=np.linspace(soll*0.4, soll*1.8, 60)
            orig=(probe>soll)
            for f in FACTORS:
                ours=(probe > vz*f)
                a=agree[f]; a[0]+=int((ours==orig).sum()); a[1]+=len(probe)
                wrong_occ[f]  += int((ours & ~orig).sum())
                wrong_show[f] += int((~ours & orig).sum())

print("Faktor  Uebereinstimmung  falsch_verdeckt  falsch_gezeigt")
for f in FACTORS:
    a=agree[f]
    print("  %.2f      %6.1f %%          %5.1f %%          %5.1f %%"
          %(f, 100*a[0]/max(1,a[1]), 100*wrong_occ[f]/max(1,a[1]), 100*wrong_show[f]/max(1,a[1])))
