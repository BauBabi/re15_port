# -*- coding: utf-8 -*-
"""Welche der ZWEI Funktionen, die @0x800768b0 lesen, passt zu den Daten?
   F1 = FUN_800473f8 @0x8004741c : mx = ((((wx+32000)*10*sx)>>20)+5)/10 + ox
   F2 = FUN_80046fd8 @0x80047014 : mx = (wx+25000)/sx + ox      (div, +0x61a8)"""
import sys,statistics; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
def f2(wx,wz,ox,oy,sx,sy):
    return ((wx+25000)//sx if wx+25000>=0 else -((-(wx+25000))//sx))+ox, \
           (-(wz+25000)//sy if -(wz+25000)>=0 else -(((wz+25000))//sy))+oy
def f2b(wx,wz,ox,oy,sx,sy):
    import math
    q=lambda a,b: int(a/b)          # C-Division schneidet zur Null
    return q(wx+25000,sx)+ox, q(-(wz+25000),sy)+oy
print('%-7s %-6s | %8s %8s | %8s %8s'%('Raum.Z','pg/rc','klemm_F1','fuell_F1','klemm_F2','fuell_F2'))
a1=[];a2=[];b1=[];b2=[]
for r in ECHT:
    ks=[k for k in zonen(r) if ZONES[k]['sx']>1] or [zonen(r)[0]]
    k=ks[0]; z=ZONES[k]; R=rects(z['page'])[z['rect']]
    w=zeile(r)[2]; P=pts_of(k,4)
    def stat(fn):
        Q=[fn(x,zz,*w) for x,zz in P]
        inn=[p for p in Q if R[0]<=p[0]<=R[0]+R[2]-1 and R[1]<=p[1]<=R[1]+R[3]-1]
        return 100*(1-len(inn)/len(Q)), 100*len(set(inn))/float(R[2]*R[3])
    k1=stat(proj); k2=stat(f2b)
    a1.append(k1[0]);a2.append(k2[0]);b1.append(k1[1]);b2.append(k2[1])
    print('%04X.%d  %2d/%-2d | %8.1f %8.1f | %8.1f %8.1f'%(r,k[1],z['page'],z['rect'],k1[0],k1[1],k2[0],k2[1]))
print()
print('Median Klemmrate  F1 %.1f %%   F2 %.1f %%'%(statistics.median(a1),statistics.median(a2)))
print('Median Fuellgrad  F1 %.1f %%   F2 %.1f %%'%(statistics.median(b1),statistics.median(b2)))
