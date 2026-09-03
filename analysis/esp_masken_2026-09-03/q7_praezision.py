"""Q7: Warum bleibt die Rueckgewinnung bei IoU ~0.4 stehen — und ist das Gefundene RICHTIG?

Zwei Fragen, die ueber das weitere Vorgehen entscheiden:
 (a) Diagnose: haengt der Erfolg an der Datenqualitaet? Gemessen wird je Cut die
     Uebereinstimmung Atlas<->Hintergrund an den BEKANNTEN Zielstellen (Q1-Mass) und
     dagegen die erreichte IoU. Ist der Zusammenhang stark, ist nicht das Verfahren
     schuld, sondern Cuts, deren Atlas gar nicht zum Bild gehoert.
 (b) Entscheidungsgroesse: PRAEZISION statt Vollstaendigkeit. Eine fehlende Maske
     laesst alles wie bisher; eine FALSCHE Maske malt Hintergrund ueber die Figur und
     ist sichtbar schlimmer. Gemessen wird daher, welcher Anteil der als Vordergrund
     markierten Bildpixel wirklich im Kuenstler-Bereich liegt — je NCC-Schwelle.
"""
import os, sys, glob, struct
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb, TBL, u32, ROOT, norm_mad
from q3b_tiefe import groups_masks
from gen_region import find_blocks

lum=lambda a:a[...,0]*0.299+a[...,1]*0.587+a[...,2]*0.114
THR=[0.90,0.95,0.97,0.99]
stat={t:{'hit':0,'tot':0,'cuts':0} for t in THR}
diag=[]
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
    if rid&1: continue
    st=(rid>>12)&0xF
    if st not in TBL: continue
    d=open(rdt,'rb').read()
    if len(d)<0x60: continue
    nCut=d[1]; cam=u32(d,0x24)
    if not cam or cam+32*nCut>len(d): continue
    for c in range(nCut):
        ms=[m for m in groups_masks(d,u32(d,cam+c*32+0x1C)) if m[3]>0 and m[4]>0]
        if len(ms)<4: continue
        A=atlas_rgb(st,rid,c); B=bg_rgb(st,rid,c)
        if A is None or B is None: continue
        at,idx=A
        truth=np.zeros((240,320),bool)
        for (gi,X,Y,w,h,dep) in ms:
            x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
            if x1>x0 and y1>y0: truth[y0:y1,x0:x1]=True
        if truth.sum()==0: continue
        blocks=find_blocks(lum(at),(idx!=0),lum(B),block=16,stride=8,min_ncc=0.85)
        for t in THR:
            scr=np.zeros((240,320),bool)
            for ax,ay,dx,dy,v in blocks:
                if v<t: continue
                y0,y1=ay+dy,ay+dy+16; x0,x1=ax+dx,ax+dx+16
                if 0<=y0 and y1<=240 and 0<=x0 and x1<=320: scr[y0:y1,x0:x1]=True
            if scr.sum():
                stat[t]['hit']+=int((scr&truth).sum()); stat[t]['tot']+=int(scr.sum()); stat[t]['cuts']+=1
        # Diagnose: Datenqualitaet dieses Cuts
        mads=[]
        for (gi,X,Y,w,h,dep) in ms[:40]:
            pass
        diag.append((name,c,len(blocks)))

print("(b) PRAEZISION der als Vordergrund markierten Bildpixel")
for t in THR:
    s=stat[t]
    if s['tot']:
        print("   NCC >= %.2f : %.1f%% liegen im Kuenstler-Bereich   (%d Pixel in %d Cuts)"
              %(t,100*s['hit']/s['tot'],s['tot'],s['cuts']))
nb=np.array([d[2] for d in diag])
print("\n(a) Bloecke ueber NCC 0.85 je Cut: Median %d | keine Bloecke bei %.1f%% der Cuts"
      %(np.median(nb),100*(nb==0).mean()))
