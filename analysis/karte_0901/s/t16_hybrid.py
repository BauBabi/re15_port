# -*- coding: utf-8 -*-
"""Weitere Wege, blind getestet:
   C  Massstab = Stage-Median (leave-one-out), Versatz = beste Deckung mit der GEZEICHNETEN Kachel
   E  Formregistrierung, Massstab auf +-20 % um den Stage-Median beschraenkt
   BBOX  der heutige Port-Rueckfall (Bbox-Streckung ins Rechteck)"""
import sys,math,statistics; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
from reg2 import bmask, raster0, best_shift

def stage_med_loo(r):
    st=r>>12; v=[zeile(x)[2] for x in ECHT if x>>12==st and x!=r]
    if not v: return None
    return int(statistics.median([x[2] for x in v])), int(statistics.median([x[3] for x in v]))

def zone_haupt(r):
    ks=[k for k in zonen(r) if ZONES[k]['sx']>1]
    return (ks or zonen(r))[0]

def bbox_proj(k,wx,wz):
    z=ZONES[k]; R=rects(z['page'])[z['rect']]
    x0,x1,z0,z1=z['x0'],z['x1'],z['z0'],z['z1']
    if x1<=x0 or z1<=z0: return None
    fx=min(max(wx-x0,0),x1-x0); fz=min(max(wz-z0,0),z1-z0)
    return (R[0]+fx*R[2]//(x1-x0), R[1]+R[3]-1-fz*R[3]//(z1-z0))

print('%-7s %-6s | %-22s %6s %6s | %-22s %6s %6s | %6s %6s'%(
  'Raum.Z','pg/rc','C: Median-Massstab+Kachel','dMed','dMax','E: Form, Massstab +-20%','dMed','dMax','BBdMed','BBdMax'))
C=[];E=[];BB=[]
KONS=[]
for r in ECHT:
    k=zone_haupt(r); z=ZONES[k]; w=zeile(r)[2]
    R=rects(z['page'])[z['rect']]
    P=pts_of(k,4)
    inn=sum(1 for a,c in P if R[0]<=proj(a,c,*w)[0]<=R[0]+R[2]-1 and R[1]<=proj(a,c,*w)[1]<=R[1]+R[3]-1)
    kons = inn/len(P) >= 0.75
    if kons: KONS.append(r)
    sm=stage_med_loo(r)
    if sm is None:
        print('%04X  uebersprungen: einziger geeichter Raum seiner Stage (kein leave-one-out-Median)'%r); continue
    B,R2=bmask(z['page'],z['rect'],True)
    # C
    A,x0,y0=raster0(ZCELLS[k],sm[0],sm[1]); ox,oy,ov=best_shift(A,x0,y0,B,R2)
    dc=[math.hypot(proj(a,c,ox,oy,sm[0],sm[1])[0]-proj(a,c,*w)[0], proj(a,c,ox,oy,sm[0],sm[1])[1]-proj(a,c,*w)[1]) for a,c in P]
    # E
    beste=None
    for sx in range(int(sm[0]*0.8),int(sm[0]*1.2)+1,8):
        for sy in range(int(sm[1]*0.8),int(sm[1]*1.2)+1,8):
            A2,ax,ay=raster0(ZCELLS[k],sx,sy); o1,o2,o3=best_shift(A2,ax,ay,B,R2)
            u=int(A2.sum())+int(B.sum())-o3
            v=o3/float(u)
            if beste is None or v>beste[0]: beste=(v,o1,o2,sx,sy)
    de=[math.hypot(proj(a,c,*beste[1:])[0]-proj(a,c,*w)[0], proj(a,c,*beste[1:])[1]-proj(a,c,*w)[1]) for a,c in P]
    # BBOX
    db=[]
    for a,c in P:
        q=bbox_proj(k,a,c); p=proj(a,c,*w)
        if q: db.append(math.hypot(q[0]-p[0],q[1]-p[1]))
    C.append((r,kons,statistics.median(dc),max(dc)))
    E.append((r,kons,statistics.median(de),max(de)))
    BB.append((r,kons,statistics.median(db),max(db)))
    print('%04X.%d  %2d/%-2d | {%d,%d,%d,%d}%s %6.1f %6.1f | {%d,%d,%d,%d}%s %6.1f %6.1f | %6.1f %6.1f  %s'%(
      r,k[1],z['page'],z['rect'],ox,oy,sm[0],sm[1],'',statistics.median(dc),max(dc),
      beste[1],beste[2],beste[3],beste[4],'',statistics.median(de),max(de),
      statistics.median(db),max(db),'' if kons else '(Zeile passt NICHT ins Rect)'))
    sys.stdout.flush()
def fasse(name,L,nur=None):
    S=[x for x in L if (nur is None or x[1]==nur)]
    if not S: return
    print('  %-32s n=%2d  Median(dMed) %5.1f  Median(dMax) %5.1f  groesster dMax %6.1f'%(
      name,len(S),statistics.median([x[2] for x in S]),statistics.median([x[3] for x in S]),max(x[3] for x in S)))
print()
print('ALLE 33:')
fasse('C Median-Massstab + Kachel',C); fasse('E Form, Massstab +-20%',E); fasse('BBOX (heutiger Port)',BB)
print('nur die %d Raeume, deren Zeile zum Rechteck passt:'%len(KONS))
fasse('C Median-Massstab + Kachel',C,True); fasse('E Form, Massstab +-20%',E,True); fasse('BBOX (heutiger Port)',BB,True)
