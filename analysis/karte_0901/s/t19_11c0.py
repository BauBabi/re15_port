import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
for r in (0x11C0,):
    for k in zonen(r):
        z=ZONES[k]; R=rects(z['page'])[z['rect']]
        print('%04X.%d Seite %d Rect %d = (%d,%d) %dx%d ; Zeile %s'%(r,k[1],z['page'],z['rect'],R[0],R[1],R[2],R[3],zeile(r)[2]))
        ox,oy=zeile(r)[2][0],zeile(r)[2][1]
        print('   Stub-Marker landet fest bei (%d,%d) -> im Rechteck? %s'%(ox,oy,
              'JA' if R[0]<=ox<=R[0]+R[2]-1 and R[1]<=oy<=R[1]+R[3]-1 else 'NEIN'))
print()
print('Alle Rechtecke Seite 0:',rects(0))
print()
# Was zeigt der Stub {0,0,1,1} rechnerisch?
for wx in (-32000,-8000,0,20000,32000):
    print('  Welt x=%6d -> mx = %d'%(wx, ((((wx+32000)*10*1)>>20)+5)//10))
