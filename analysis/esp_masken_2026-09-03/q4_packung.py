"""Q4: Wie ist der Vordergrundatlas gepackt?

Aus den 480 Cuts MIT Maskenrecords laesst sich die Packung ablesen: fuer jede Maske
ist (dst - src) die Verschiebung, mit der ihr Atlas-Block auf den Bildschirm kommt.
Fragen:
  a) Wie viele VERSCHIEDENE Verschiebungen hat ein Cut?
  b) Ist die Verschiebung je GRUPPE konstant (dann = Gruppenanker)?
  c) Entsprechen Zusammenhangskomponenten der undurchsichtigen Atlas-Pixel
     genau einer Verschiebung? (dann kann man sie ohne Maskenliste wiederfinden)
"""
import os, sys, glob, struct, collections
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, TBL, u32, ROOT
from q3b_tiefe import groups_masks

per_cut_shifts, per_group_const, comp_vs_shift = [], [], []
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
        po=u32(d,cam+c*32+0x1C)
        ms=groups_masks(d,po)
        if len(ms)<4: continue
        # src wieder mitlesen (groups_masks liefert nur dst) -> eigener Mini-Parser
        raw=[]
        gc=struct.unpack_from("<H",d,po)[0]
        p=po+4; gn=[]; gdx=[]; gdy=[]
        for i in range(gc):
            gn.append(struct.unpack_from("<H",d,p)[0])
            gdx.append(struct.unpack_from("<h",d,p+4)[0])
            gdy.append(struct.unpack_from("<h",d,p+6)[0]); p+=8
        gi=0; used=0
        for _ in range(sum(gn)):
            if p+8>len(d): break
            sx,sy,dx,dy=d[p],d[p+1],d[p+2],d[p+3]
            dep=struct.unpack_from("<H",d,p+4)[0]; size=struct.unpack_from("<H",d,p+6)[0]; p+=8
            if (size&0xf000)==0: w=struct.unpack_from("<H",d,p)[0]; h=struct.unpack_from("<H",d,p+2)[0]; p+=4
            else: w=h=(size>>12)*8
            while gi<gc and used>=gn[gi]: gi+=1; used=0
            ax=gdx[gi] if gi<gc else 0; ay=gdy[gi] if gi<gc else 0
            used+=1
            X=((dx+ax+0x8000)&0xffff)-0x8000; Y=((dy+ay+0x8000)&0xffff)-0x8000
            raw.append((gi,sx,sy,X,Y,w,h))
        if not raw: continue
        shifts=collections.Counter((X-sx, Y-sy) for (gi,sx,sy,X,Y,w,h) in raw)
        per_cut_shifts.append(len(shifts))
        byg=collections.defaultdict(set)
        for (gi,sx,sy,X,Y,w,h) in raw: byg[gi].add((X-sx, Y-sy))
        per_group_const.append(sum(1 for v in byg.values() if len(v)==1)/len(byg))
        # c) Komponenten der Atlas-Abdeckung vs. Verschiebungen
        A=atlas_rgb(stage,rid,c)
        if A is None: continue
        at, idx = A
        cov=np.zeros(idx.shape,bool)
        for (gi,sx,sy,X,Y,w,h) in raw:
            if sy+h<=cov.shape[0] and sx+w<=cov.shape[1]: cov[sy:sy+h, sx:sx+w]=True
        lab,n=ndimage.label(cov, structure=np.ones((3,3)))
        if n==0: continue
        pure=0
        for k in range(1,n+1):
            s=set()
            for (gi,sx,sy,X,Y,w,h) in raw:
                cy=min(idx.shape[0]-1, sy+h//2); cx=min(idx.shape[1]-1, sx+w//2)
                if lab[cy,cx]==k: s.add((X-sx,Y-sy))
            if len(s)==1: pure+=1
        comp_vs_shift.append((pure, n))

a=np.array(per_cut_shifts); g=np.array(per_group_const)
pv=np.array([p for p,n in comp_vs_shift]); nv=np.array([n for p,n in comp_vs_shift])
print("Cuts %d"%len(a))
print("a) verschiedene Verschiebungen je Cut : Median %d, 90%%-Quantil %d, Max %d"
      %(np.median(a), np.percentile(a,90), a.max()))
print("b) Gruppen mit KONSTANTER Verschiebung: %.1f%% (Mittel je Cut)"%(100*g.mean()))
print("c) Atlas-Komponenten mit genau EINER Verschiebung: %d von %d = %.1f%%"
      %(pv.sum(), nv.sum(), 100*pv.sum()/max(1,nv.sum())))
print("   Komponenten je Cut: Median %d"%np.median(nv))
