# -*- coding: utf-8 -*-
"""Fuer JEDE echte Zeile: auf WELCHES Rechteck ihrer Seite faellt die Projektion?
   (Die Zeile leitet das Rechteck ab - kein Raten.)"""
import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
def pg_of(r):
    for k in zonen(r):
        if ZONES[k]['sx']>1: return ZONES[k]['page']
    ks=zonen(r); return ZONES[ks[0]]['page'] if ks else None
print('%-7s %-4s %-7s | %s'%('Raum.Z','Seite','Port-Rc','Rechtecke der Seite mit klemm<25%% (rc: klemm%/fuell%/deck%)'))
for r in ECHT:
    for k in zonen(r):
        z=ZONES[k]; pg=z['page']
        w=zeile(r)[2]; P=[proj(a,c,*w) for a,c in pts_of(k,4)]
        if not P: continue
        px0=min(p[0] for p in P);px1=max(p[0] for p in P);py0=min(p[1] for p in P);py1=max(p[1] for p in P)
        hits=[]
        for ri,R in enumerate(rects(pg)):
            if R[2]<=0 or R[3]<=0: continue
            inn=[p for p in P if R[0]<=p[0]<=R[0]+R[2]-1 and R[1]<=p[1]<=R[1]+R[3]-1]
            q=len(inn)/float(len(P))
            fx=min(px1,R[0]+R[2]-1)-max(px0,R[0])+1; fy=min(py1,R[1]+R[3]-1)-max(py0,R[1])+1
            deck=(fx*fy)/float(R[2]*R[3]) if fx>0 and fy>0 else 0.0
            if q>=0.75: hits.append('%d:%.0f/%.0f/%.0f'%(ri,100*(1-q),100*len(set(inn))/(R[2]*R[3]),100*deck))
        print('%04X.%d  %2d   %2d      | %s'%(r,k[1],pg,z['rect'],' '.join(hits) if hits else '-- KEIN Rechteck --'))
