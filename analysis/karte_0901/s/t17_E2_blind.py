# -*- coding: utf-8 -*-
"""Verfahren E2: Formregistrierung gegen die Kachel OHNE Palettenindex 4.
   Index 4 sind Wandlinien UND die gemalten Tuerblaetter (analysis/kartensymbole:
   92 Innenlinien-Gruppen, alle Index 4). Damit ist die Lage der Tuersymbole KEINE
   Eingabe der Registrierung mehr -> der Symbolabstand ist eine STRENG unabhaengige
   Gegengroesse."""
import sys,math,statistics,itertools,json; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
from reg2 import bmask, raster0, best_shift

def stage_med_loo(r=None,st=None):
    st = st if st is not None else (r>>12)
    v=[zeile(x)[2] for x in ECHT if x>>12==st and x!=r]
    if not v: return None
    return int(statistics.median([x[2] for x in v])), int(statistics.median([x[3] for x in v]))

def zone_haupt(r):
    ks=[k for k in zonen(r) if ZONES[k]['sx']>1]
    return (ks or zonen(r))[0]

def schaetze(k, sm, spanne=0.20, schritt=8, voll=False):
    z=ZONES[k]; B,R=bmask(z['page'],z['rect'],voll)
    beste=None
    for sx in range(int(sm[0]*(1-spanne)),int(sm[0]*(1+spanne))+1,schritt):
        for sy in range(int(sm[1]*(1-spanne)),int(sm[1]*(1+spanne))+1,schritt):
            A,ax,ay=raster0(ZCELLS[k],sx,sy)
            o1,o2,ov=best_shift(A,ax,ay,B,R)
            u=int(A.sum())+int(B.sum())-ov
            v=ov/float(u)
            if beste is None or v>beste[0]: beste=(v,o1,o2,sx,sy)
    return beste

def symbolabstand(r,k,zl):
    """mittlerer Abstand der projizierten RDT-Tueren zu den GEMALTEN Symbolen des Rechtecks.
       Streng unabhaengig von E2 (Index-4-Pixel sind nicht in der Maske)."""
    z=ZONES[k]; gl=[(g[0],g[1]) for g in GLYPHEN.get((z['page'],z['rect']),())]
    dp=[proj(d['lx'],d['lz'],*zl) for d in doors_of(r)
        if zone_at(r,d['lx'],d['lz'])==k[1]]
    if not gl or not dp: return None,len(dp),len(gl)
    n=len(dp);m=len(gl);kk=min(n,m); best=None
    for wahl in itertools.permutations(range(m),kk):
        for sub in itertools.combinations(range(n),kk):
            f=sum(math.hypot(dp[sub[t]][0]-gl[wahl[t]][0],dp[sub[t]][1]-gl[wahl[t]][1]) for t in range(kk))/kk
            if best is None or f<best: best=f
    return best,n,m

print('BLINDTEST E2 (Maske ohne Index 4) an den Raeumen mit echter Zeile')
print('%-6s %-6s | %-24s %6s %6s | %8s %8s'%('Raum','pg/rc','Schaetzung','dMed','dMax','Sym_wahr','Sym_E2'))
R1=[];KONS=[]
for r in ECHT:
    k=zone_haupt(r); z=ZONES[k]; w=zeile(r)[2]
    RC=rects(z['page'])[z['rect']]; P=pts_of(k,4)
    inn=sum(1 for a,c in P if RC[0]<=proj(a,c,*w)[0]<=RC[0]+RC[2]-1 and RC[1]<=proj(a,c,*w)[1]<=RC[1]+RC[3]-1)
    kons=inn/len(P)>=0.75
    sm=stage_med_loo(r)
    if sm is None: print('%04X  uebersprungen (kein LOO-Median)'%r); continue
    b=schaetze(k,sm)
    est=(b[1],b[2],b[3],b[4])
    dd=[math.hypot(proj(a,c,*est)[0]-proj(a,c,*w)[0],proj(a,c,*est)[1]-proj(a,c,*w)[1]) for a,c in P]
    sw,_,_=symbolabstand(r,k,w); se,nd,ng=symbolabstand(r,k,est)
    if kons: KONS.append(r)
    R1.append((r,kons,statistics.median(dd),max(dd),sw,se))
    print('%04X  %2d/%-2d | {%d,%d,%d,%d} %6.1f %6.1f | %8s %8s %s'%(
      r,z['page'],z['rect'],est[0],est[1],est[2],est[3],statistics.median(dd),max(dd),
      ('%.1f'%sw) if sw is not None else '-', ('%.1f'%se) if se is not None else '-',
      '' if kons else '(Zeile passt NICHT ins Rect)'))
    sys.stdout.flush()
print()
for nur in (None,True):
    S=[x for x in R1 if nur is None or x[1]]
    print('%s n=%d  Median(dMed) %.1f  Median(dMax) %.1f  groesster dMax %.1f'%(
      'ALLE:  ' if nur is None else 'KONSISTENTE:',len(S),statistics.median([x[2] for x in S]),
      statistics.median([x[3] for x in S]),max(x[3] for x in S)))
sw=[x[4] for x in R1 if x[4] is not None]; se=[x[5] for x in R1 if x[5] is not None]
print('unabhaengige Gegengroesse Symbolabstand: WAHRE Zeile Median %.1f px (n=%d), E2-Schaetzung Median %.1f px (n=%d)'%(
  statistics.median(sw),len(sw),statistics.median(se),len(se)))
json.dump([(x[0],x[1],x[2],x[3],x[4],x[5]) for x in R1],open(r'analysis\karte_0901\s\e2_blind.json','w'))
