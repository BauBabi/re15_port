# -*- coding: utf-8 -*-
"""M3: Bestimmtheitsgrad. Wie viele GEEICHTE Nachbarn auf DERSELBEN Seite hat jeder Raum?
   Unbekannte je Achse: 2 (Versatz + Massstab). Gleichungen = Tueren zu geeichten Nachbarn.
   Ueberbestimmt heisst: >2 unabhaengige Stuetzpunkte je Achse (verschiedene Weltkoordinaten)."""
import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
import collections

def pg_of(r):
    for k in zonen(r):
        if ZONES[k]['sx']>1: return ZONES[k]['page']
    ks=zonen(r)
    return ZONES[ks[0]]['page'] if ks else None
PG={r:pg_of(r) for r in ROOMS}
D={r:doors_of(r) for r in ROOMS}

def stuetzen(r, kalib):
    """Stuetzpunkte fuer Raum r: (welt_x, welt_z, karte_x, karte_y) aus Tueren zu Raeumen in 'kalib'."""
    out=[]
    for da in D[r]:
        B=da['dest']&0xFFF0
        if B==r: continue                     # Selbst-Tuer: NICHT derselbe Ort (1170: 95 px)
        if B not in kalib: continue
        if PG.get(B) is None or PG[B]!=PG[r]: continue
        oB=zeile(B)[2]
        p=proj(da['nx'],da['nz'],*oB)
        out.append((da['lx'],da['lz'],p[0],p[1],B,'land'))
        cands=[db for db in D.get(B,()) if (db['dest']&0xFFF0)==r]
        if cands:
            db=min(cands,key=lambda d:(d['lx']-da['nx'])**2+(d['lz']-da['nz'])**2)
            q=proj(db['lx'],db['lz'],*oB)
            out.append((da['lx'],da['lz'],q[0],q[1],B,'tuer'))
    return out

print('=== Raeume MIT echter Zeile (Blindtest-Kandidaten) ===')
print('%-5s %3s %-9s %-9s %s'%('Raum','pg','n_x_distinct','n_z_distinct','Nachbarn (geeicht, gleiche Seite)'))
statm=collections.Counter()
for r in ECHT:
    kal=set(ECHT)-{r}
    S=stuetzen(r,kal)
    nx=len(set(s[0] for s in S)); nz=len(set(s[1] for s in S))
    nb=sorted(set('%04X'%s[4] for s in S))
    statm[(min(nx,2)>=2)+(min(nz,2)>=2)]+=1
    print('%04X %3s %9d %9d  %s'%(r,PG[r],nx,nz,' '.join(nb) if nb else '-'))
print()
print('=== STUB-Raeume ===')
print('%-5s %3s %-9s %-9s %s'%('Raum','pg','n_x_distinct','n_z_distinct','Nachbarn (geeicht, gleiche Seite)'))
kal=set(ECHT)
n0=n1=n2=0
for r in STUB:
    S=stuetzen(r,kal)
    nx=len(set(s[0] for s in S)); nz=len(set(s[1] for s in S))
    nb=sorted(set('%04X'%s[4] for s in S))
    if nx>=2 and nz>=2: n2+=1
    elif nx>=1 or nz>=1: n1+=1
    else: n0+=1
    print('%04X %3s %9d %9d  %s'%(r,PG[r],nx,nz,' '.join(nb) if nb else '-'))
print()
print('STUB: 0 Stuetzen: %d,  1 Achse teilbestimmt: %d,  beide Achsen >=2 Stuetzen: %d'%(n0,n1,n2))
