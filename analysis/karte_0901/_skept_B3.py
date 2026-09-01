# -*- coding: utf-8 -*-
# GEGENMESSUNG 1 (katalogfrei): deckt der Port-Stempel eine SCHON GEMALTE Linie?
import os, sys, re, collections, random
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import rects, rect_uv, page_pix

SYM=[(0,-2),(1,-2),(2,-2),(3,-2),(4,-2),(5,-2),(5,-1),(5,0),(4,1),(0,2),(1,2),(2,2),(3,2)]
def stamp(kind,mx,my):
    o=[]
    for dx,dy in SYM:
        if kind==3: p=( dx, dy)
        elif kind==1: p=(-dx, dy)
        elif kind==0: p=( dy, dx)
        else:        p=( dy,-dx)
        o.append((mx+p[0],my+p[1],dx))
    return o

LINE={}
def line(pg):
    if pg in LINE: return LINE[pg]
    pix=page_pix(pg); s=set()
    if pix is not None:
        for r,(RX,RY,RW,RH) in enumerate(rects(pg)):
            U,V=rect_uv(pg,r)
            for j in range(RH):
                for i in range(RW):
                    if V+j<256 and U+i<256 and pix[V+j][U+i]==4: s.add((RX+i,RY+j))
    LINE[pg]=s; return s

def marks(p):
    s=open(p,encoding='utf-8').read()
    m=re.search(r's_map_marks\[\]\s*=\s*\{(.*?)\n\};',s,re.S)
    return [tuple(int(x) for x in t) for t in re.findall(
        r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(-?\d+)\s*\}',m.group(1))]

def messe(p,lab):
    M=[m for m in marks(p) if m[4]<=3]
    h=collections.Counter(); det=[]
    for (pg,r,mx,my,kind,zid,zid2) in M:
        L=line(pg)
        n=sum(1 for (x,y,dx) in stamp(kind,mx,my) if dx>=1 and (x,y) in L)
        h[n]+=1; det.append((n,pg,r,mx,my,kind,zid,zid2))
    tot=len(M); hi=sum(v for k,v in h.items() if k>=8)
    print(f"\n=== {lab}: {tot} Tuermarken ===")
    print("  von 11 Nicht-Wand-Stempelpixeln liegen auf einer SCHON GEMALTEN Linie (idx4):")
    for k in sorted(h): print(f"    {k:2d}/11 : {h[k]:3d}")
    print(f"  >=8/11 : {hi}/{tot} = {100.0*hi/max(1,tot):.0f}%")
    return det,M

dA,MA=messe('analysis/karte_0901/snapshot/re15_map_zones.h','SNAPSHOT (Stand des Nutzer-Reports, 12:19)')
dB,MB=messe('re15_port/engine/src/re15_map_zones.h','ARBEITSBAUM (Fix schon drin, 12:54)')

pgs=sorted(set(m[0] for m in MA))
h=collections.Counter(); n=0
for pg in pgs:
    L=line(pg)
    for r,(RX,RY,RW,RH) in enumerate(rects(pg)):
        for kind in (0,1,2,3):
            for y in range(RY,RY+RH):
                for x in range(RX,RX+RW):
                    if (x,y) not in L: continue
                    h[sum(1 for (a,b,dx) in stamp(kind,x,y) if dx>=1 and (a,b) in L)]+=1; n+=1
print(f"\n=== NULLVERTEILUNG (derselbe Stempel auf JEDEM Linienpixel, 4 Richtungen, n={n}) ===")
for k in sorted(h): print(f"    {k:2d}/11 : {h[k]:7d}  {100.0*h[k]/n:5.2f}%")
print(f"  >=8/11 zufaellig: {100.0*sum(v for k,v in h.items() if k>=8)/n:.2f}%")

print("\n=== die Marken mit HOHER Ueberdeckung (>=10/11), Snapshot ===")
for t in sorted(dA, reverse=True)[:20]: print("   ",t)
print("\n=== die Marken mit NIEDRIGER Ueberdeckung (<=4/11), Snapshot ===")
for t in sorted(dA)[:15]: print("   ",t)
