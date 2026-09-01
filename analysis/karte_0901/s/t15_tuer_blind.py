# -*- coding: utf-8 -*-
"""Schritt 2+4: Tuer-Partner-Bestimmung, BLIND getestet an den Raeumen, deren Zeile bekannt ist.
   Variante A: Massstab = Stage-Median der UEBRIGEN geeichten Raeume (leave-one-out), Versatz aus Tueren.
   Variante B: Massstab UND Versatz aus Tueren (>=2 unabhaengige Stuetzen je Achse)."""
import sys,math,statistics,collections; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *

def pg_of(r):
    for k in zonen(r):
        if ZONES[k]['sx']>1: return ZONES[k]['page']
    ks=zonen(r); return ZONES[ks[0]]['page'] if ks else None
PG={r:pg_of(r) for r in ROOMS}
D={r:doors_of(r) for r in ROOMS}

def stuetzen(r,kalib,modus='beide'):
    out=[]
    for da in D[r]:
        B=da['dest']&0xFFF0
        if B==r or B not in kalib: continue
        if PG.get(B) is None or PG[B]!=PG[r]: continue
        oB=zeile(B)[2]
        if modus in ('land','beide'):
            p=proj(da['nx'],da['nz'],*oB); out.append((da['lx'],da['lz'],p[0],p[1],B))
        if modus in ('tuer','beide'):
            c=[db for db in D.get(B,()) if (db['dest']&0xFFF0)==r]
            if c:
                db=min(c,key=lambda d:(d['lx']-da['nx'])**2+(d['lz']-da['nz'])**2)
                q=proj(db['lx'],db['lz'],*oB); out.append((da['lx'],da['lz'],q[0],q[1],B))
    return out

def kx(wx,sx): return ((((wx+32000)*10*sx)>>20)+5)//10          # ohne ox
def kz(wz,sy): return -((((wz+32000)*10*sy)>>20)+5)//10          # ohne oy

def loesungA(S,sx,sy):
    if not S: return None
    ox=int(round(statistics.mean([m-kx(w,sx) for (w,_,m,_,_) in S])))
    oy=int(round(statistics.mean([m-kz(w,sy) for (_,w,_,m,_) in S])))
    return ox,oy

def loesungB(S):
    """kleinste Quadrate je Achse: karte = k(welt,s) + o. s ganzzahlig gesucht."""
    if len(set(s[0] for s in S))<2 or len(set(s[1] for s in S))<2: return None
    best=None
    for sx in range(600,4200,2):
        o=statistics.mean([m-kx(w,sx) for (w,_,m,_,_) in S])
        e=sum((m-kx(w,sx)-o)**2 for (w,_,m,_,_) in S)
        if best is None or e<best[0]: best=(e,sx,int(round(o)))
    bx=best
    best=None
    for sy in range(600,4200,2):
        o=statistics.mean([m-kz(w,sy) for (_,w,_,m,_) in S])
        e=sum((m-kz(w,sy)-o)**2 for (_,w,_,m,_) in S)
        if best is None or e<best[0]: best=(e,sy,int(round(o)))
    by=best
    return bx[2],by[2],bx[1],by[1]

def abweichung(r,est):
    w=zeile(r)[2]; dd=[]
    for k in zonen(r):
        for a,c in pts_of(k,4):
            p=proj(a,c,*est); q=proj(a,c,*w)
            dd.append(math.hypot(p[0]-q[0],p[1]-q[1]))
    return statistics.median(dd),max(dd)

STAGE_MED={}
for st in range(1,7):
    v=[zeile(r)[2] for r in ECHT if r>>12==st]
    if v: STAGE_MED[st]=(statistics.median([x[2] for x in v]),statistics.median([x[3] for x in v]))
print('Stage-Median Massstab (alle geeichten Raeume der Stage):',
      {k:(int(a),int(b)) for k,(a,b) in STAGE_MED.items()})
print()
for modus in ('beide','tuer','land'):
    print('===== Stuetzen-Modus: %s ====='%modus)
    print('%-5s %4s | %-28s %8s %8s | %-28s %8s %8s'%('Raum','nSt','Variante A (Massstab=Stage-Median)','dMed','dMax','Variante B (alles aus Tueren)','dMed','dMax'))
    A=[];Bl=[]
    for r in ECHT:
        kal=set(ECHT)-{r}
        S=stuetzen(r,kal,modus)
        if not S: continue
        st=r>>12
        # leave-one-out Stage-Median
        v=[zeile(x)[2] for x in ECHT if x>>12==st and x!=r]
        if not v: continue
        smx=int(statistics.median([x[2] for x in v])); smy=int(statistics.median([x[3] for x in v]))
        a=loesungA(S,smx,smy); b=loesungB(S)
        sa=('{%d,%d,%d,%d}'%(a[0],a[1],smx,smy)) if a else '-'
        da=abweichung(r,(a[0],a[1],smx,smy)) if a else (None,None)
        sb=('{%d,%d,%d,%d}'%b) if b else '-'
        db=abweichung(r,b) if b else (None,None)
        if da[0] is not None: A.append(da)
        if db[0] is not None: Bl.append(db)
        print('%04X %4d | %-28s %8s %8s | %-28s %8s %8s'%(r,len(S),sa,
          ('%.1f'%da[0]) if da[0] is not None else '-',('%.1f'%da[1]) if da[1] is not None else '-',
          sb,('%.1f'%db[0]) if db[0] is not None else '-',('%.1f'%db[1]) if db[1] is not None else '-'))
    if A: print('  A: n=%d  Median(dMed) %.1f px  Median(dMax) %.1f px  groesster dMax %.1f px'%(
        len(A),statistics.median([x[0] for x in A]),statistics.median([x[1] for x in A]),max(x[1] for x in A)))
    if Bl: print('  B: n=%d  Median(dMed) %.1f px  Median(dMax) %.1f px  groesster dMax %.1f px'%(
        len(Bl),statistics.median([x[0] for x in Bl]),statistics.median([x[1] for x in Bl]),max(x[1] for x in Bl)))
    print()
