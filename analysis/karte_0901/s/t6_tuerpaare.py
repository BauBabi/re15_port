# -*- coding: utf-8 -*-
"""M2: Wie gut haelt die Annahme 'ein Durchgang = EIN Kartenpunkt' bei ECHTEN Zeilen?
   Das ist die Rausch-Untergrenze jeder Tuer-basierten Bestimmung."""
import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
import math, statistics

def zeil(r): return zeile(r)[2]
def hat_echt(r): return r in ECHT

# alle Tueren je Raum
D={r:doors_of(r) for r in ROOMS}
paare=[]      # (A, dA, B, dB)  dB = Gegen-Tuer in B nach A
for A in ROOMS:
    for da in D[A]:
        B=da['dest']&0xFFF0
        if B not in ROOMS: continue
        # Gegen-Tuer: Tuer in B mit dest==A, deren Mitte dem Landepunkt (nx,nz) am naechsten ist
        cands=[db for db in D.get(B,()) if (db['dest']&0xFFF0)==A]
        db=min(cands,key=lambda d: (d['lx']-da['nx'])**2+(d['lz']-da['nz'])**2) if cands else None
        paare.append((A,da,B,db))
print('Tuer-Datensaetze gesamt:',sum(len(v) for v in D.values()))
print('davon mit Ziel im Kartensatz:',len(paare))
print('davon mit Gegen-Tuer:',sum(1 for p in paare if p[3]))

sp=[]   # gleiche Seite, beide echt
for (A,da,B,db) in paare:
    if not (hat_echt(A) and hat_echt(B)): continue
    zA=[k for k in zonen(A)]; zB=[k for k in zonen(B)]
    # Seite der Zone, die die Zeile traegt bzw. Zone 0
    def pg_of(r):
        for k in zonen(r):
            if ZONES[k]['sx']>1: return ZONES[k]['page']
        return ZONES[zonen(r)[0]]['page'] if zonen(r) else None
    if pg_of(A) is None or pg_of(A)!=pg_of(B): continue
    oA=zeil(A); oB=zeil(B)
    pA=proj(da['lx'],da['lz'],*oA)               # Tuer in A
    pL=proj(da['nx'],da['nz'],*oB)               # Landepunkt in B
    d1=math.hypot(pA[0]-pL[0],pA[1]-pL[1])
    d2=None
    if db:
        pB=proj(db['lx'],db['lz'],*oB)           # Gegen-Tuer in B
        d2=math.hypot(pA[0]-pB[0],pA[1]-pB[1])
    sp.append((A,B,d1,d2,da,db,oA,oB))
print()
print('=== Tuerpaare, BEIDE Raeume mit echter Zeile, GLEICHE Seite: %d ==='%len(sp))
print('%-5s %-5s %9s %9s   %s'%('A','B','|Tuer_A-Land_B|','|Tuer_A-Tuer_B|','Welt'))
for (A,B,d1,d2,da,db,oA,oB) in sorted(sp,key=lambda t:t[2]):
    print('%04X %04X %9.1f %9s   A(%6d,%6d) land(%6d,%6d)%s'%(
      A,B,d1,('%.1f'%d2) if d2 is not None else '-',da['lx'],da['lz'],da['nx'],da['nz'],
      '' if db is None else ' B(%6d,%6d)'%(db['lx'],db['lz'])))
v1=[t[2] for t in sp]; v2=[t[3] for t in sp if t[3] is not None]
if v1: print('\n|Tuer_A - Land_B| : n=%d median %.2f px  mittel %.2f  max %.2f'%(len(v1),statistics.median(v1),sum(v1)/len(v1),max(v1)))
if v2: print('|Tuer_A - Tuer_B| : n=%d median %.2f px  mittel %.2f  max %.2f'%(len(v2),statistics.median(v2),sum(v2)/len(v2),max(v2)))
