import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
ziel=[0x11A0,0x3060,0x30E0,0x6030,0x6040,0x1190,0x3070,0x4030,0x5030,0x5110,0x5120,0x1020]
for r in ziel:
    for k in zonen(r):
        z=ZONES[k]; w=zeile(r)[2]
        P=[proj(a,c,*w) for a,c in pts_of(k,4)]
        if not P: continue
        best=[]
        for pg in range(14):
            try: RR=rects(pg)
            except Exception: continue
            for ri,R in enumerate(RR):
                if R[2]<=0 or R[3]<=0: continue
                inn=[p for p in P if R[0]<=p[0]<=R[0]+R[2]-1 and R[1]<=p[1]<=R[1]+R[3]-1]
                q=len(inn)/float(len(P))
                if q>=0.75:
                    best.append('%d/%d:%.0f%%,f%.0f%%'%(pg,ri,100*q,100*len(set(inn))/(R[2]*R[3])))
        print('%04X.%d  Port %d/%-2d  Zeile{%d,%d,%d,%d}  Projektion x[%d..%d] y[%d..%d]  -> %s'%(
          r,k[1],z['page'],z['rect'],*w,min(p[0] for p in P),max(p[0] for p in P),
          min(p[1] for p in P),max(p[1] for p in P), ' '.join(best) if best else 'KEIN Rechteck auf KEINER Seite'))
