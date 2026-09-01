import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
print('Raeume %d  echt %d  stub %d  Zonen %d'%(len(ROOMS),len(ECHT),len(STUB),len(ZONES)))
print()
print('%-5s %-9s %5s %5s %6s %6s %8s %8s %6s %8s %8s %8s %8s  %s'%(
  'Raum','Zone/Rect','ox','oy','sx','sy','E/px_x','E/px_y','npts','klemm%','fuell%','projW','projH','hdr'))
for r in ECHT:
    idx,a,(ox,oy,sx,sy)=zeile(r)
    for k in zonen(r):
        z=ZONES[k]; R=rects(z['page'])[z['rect']]
        m=klemm_fuell(k,ox,oy,sx,sy)
        if m is None: 
            print('%04X.%d  -> %2d/%-2d  keine Punkte'%(k[0],k[1],z['page'],z['rect'])); continue
        print('%04X.%d %2d/%-2d %4dx%-3d %5d %5d %6d %6d %8.1f %8.1f %6d %8.1f %8.1f %8d %8d  %s'%(
          k[0],k[1],z['page'],z['rect'],R[2],R[3],ox,oy,sx,sy,(1<<20)/sx,(1<<20)/sy,
          m['n'],100*m['klemm'],100*m['fuell'],m['bb'][1]-m['bb'][0]+1,m['bb'][3]-m['bb'][2]+1,
          'JA' if z['sx']>1 else '-'))
