# -*- coding: utf-8 -*-
# UNABHAENGIGE GEGENMESSUNG: nur ROHE Kachel-Pixel (DATA/MAP0x.PIX), KEIN symbolkatalog.csv.
import os, sys, re, math, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import rects, rect_uv, page_pix, ROOT

SYM = [(0,-2),(1,-2),(2,-2),(3,-2),(4,-2),(5,-2),(5,-1),(5,0),(4,1),(0,2),(1,2),(2,2),(3,2)]
def stamp(kind, mx, my):
    out=[]
    for dx,dy in SYM:
        if kind==3: px,py= dx, dy
        elif kind==1: px,py=-dx, dy
        elif kind==0: px,py= dy, dx
        else:         px,py= dy,-dx
        out.append((mx+px, my+py, dx))
    return out

# ---- Tinte je Seite, direkt aus den PIX ----
INK={}
def ink(pg):
    if pg in INK: return INK[pg]
    pix=page_pix(pg); s=set()
    if pix is not None:
        for r,(RX,RY,RW,RH) in enumerate(rects(pg)):
            U,V=rect_uv(pg,r)
            for j in range(RH):
                for i in range(RW):
                    if V+j<256 and U+i<256 and pix[V+j][U+i]!=0: s.add((RX+i,RY+j))
    INK[pg]=s; return s

def marks(path):
    s=open(path,encoding='utf-8').read()
    m=re.search(r's_map_marks\[\]\s*=\s*\{(.*?)\n\};', s, re.S)
    return [tuple(int(x) for x in t) for t in re.findall(
        r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(-?\d+)\s*\}', m.group(1))]

def messe(path, lab):
    M=[m for m in marks(path) if m[4]<=3]
    print(f"\n=== {lab}: {len(M)} Tuermarken ===")
    hist=collections.Counter(); det=[]
    for (pg,r,mx,my,kind,zid,zid2) in M:
        I=ink(pg)
        off=[(x,y) for (x,y,dx) in stamp(kind,mx,my) if dx>=1]   # 11 Pixel NEBEN der Wand
        n=sum(1 for p in off if p in I)
        hist[n]+=1; det.append((n,pg,r,mx,my,kind))
    print("  Tinte unter den 11 Nicht-Wand-Pixeln des Port-Stempels:")
    for k in sorted(hist): print(f"    {k:2d}/11 : {hist[k]:3d}")
    hoch=sum(v for k,v in hist.items() if k>=6)
    print(f"  >=6/11 (Kachel malt dort schon eine Nische): {hoch}/{len(M)} = {100.0*hoch/max(1,len(M)):.0f}%")
    return det,M

# ---- NULLVERTEILUNG: derselbe Stempel an BELIEBIGEN Wandstellen ----
def null(pg_list):
    import random
    random.seed(7)
    hist=collections.Counter(); n_tot=0
    for pg in pg_list:
        I=ink(pg)
        for r,(RX,RY,RW,RH) in enumerate(rects(pg)):
            for kind in (0,1,2,3):
                for y in range(RY+3, RY+RH-3):
                    for x in range(RX+3, RX+RW-3):
                        if (x,y) not in I: continue
                        off=[(a,b) for (a,b,dx) in stamp(kind,x,y) if dx>=1]
                        hist[sum(1 for p in off if p in I)]+=1; n_tot+=1
    print(f"\n=== NULLVERTEILUNG: Stempel auf JEDEM Tintenpixel, alle 4 Richtungen (n={n_tot}) ===")
    for k in sorted(hist): print(f"    {k:2d}/11 : {hist[k]:6d}  {100.0*hist[k]/n_tot:5.1f}%")
    print(f"  >=6/11 zufaellig: {100.0*sum(v for k,v in hist.items() if k>=6)/n_tot:.1f}%")

dA,MA=messe('analysis/karte_0901/snapshot/re15_map_zones.h','SNAPSHOT (Stand des Nutzer-Reports)')
dB,MB=messe('re15_port/engine/src/re15_map_zones.h','ARBEITSBAUM (Fix bereits eingebaut)')
null(sorted(set(m[0] for m in MA)))
