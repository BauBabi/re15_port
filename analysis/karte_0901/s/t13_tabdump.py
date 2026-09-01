import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
print('Rohtabelle @0x800768b0, 8 B je Zeile:')
for i in range(0,120):
    a=0x800768b0+i*8
    ox,oy,sx,sy=s16(a),s16(a+2),u16(a+4),u16(a+6)
    tag=''
    for st in range(6):
        if 0<= i-BASES[st] <=0x30:
            tag+=' %d%02X0?'%(st+1,i-BASES[st])
    print('%3d @0x%08X  {%5d,%5d,%5d,%5d}%s'%(i,a,ox,oy,sx,sy,tag))
