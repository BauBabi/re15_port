# -*- coding: utf-8 -*-
"""Wie sieht die Kachel im Rechteck aus, und wo liegen die projizierten SCA-Punkte?"""
import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
import collections
def show(r):
    k=[kk for kk in zonen(r) if ZONES[kk]['sx']>1]
    k=k[0] if k else zonen(r)[0]
    z=ZONES[k]; pg,ri=z['page'],z['rect']; R=rects(pg)[ri]; U,V=rect_uv(pg,ri)
    pix=page_pix(pg)
    ox,oy,sx,sy=zeile(r)[2]
    P=set(proj(a,c,ox,oy,sx,sy) for a,c in pts_of(k,6))
    print('ROOM%04X  Zone %d  Seite %d Rect %d  screen(%d,%d) %dx%d  uv(%d,%d)  Zeile{%d,%d,%d,%d}'%(
        r,k[1],pg,ri,R[0],R[1],R[2],R[3],U,V,ox,oy,sx,sy))
    hist=collections.Counter()
    for j in range(R[3]):
        line=''
        for i in range(R[2]):
            c=pix[V+j][U+i] if (V+j<256 and U+i<256) else 0
            hist[c]+=1
            here=(R[0]+i,R[1]+j) in P
            line += ('#' if here else ('.' if c==0 else '%X'%c)) if not (here and c) else '@'
        print('  '+line)
    print('  Palettenhistogramm:',dict(sorted(hist.items())))
    # Treffer-Statistik
    tot=0; onbg=0
    for p in P:
        i=p[0]-R[0]; j=p[1]-R[1]
        if not (0<=i<R[2] and 0<=j<R[3]): continue
        tot+=1
        if pix[V+j][U+i]==0: onbg+=1
    print('  proj. Punkte im Rect: %d, davon auf Palette 0 (nicht gezeichnet): %d (%.1f%%)'%(tot,onbg,100*onbg/max(tot,1)))
    print()
for r in (0x3040,0x1150,0x5010,0x3000):
    show(r)
