import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
from reg import *
import statistics
print('IoU der SCA-Flaeche gegen den gezeichneten Grundriss, mit der ECHTEN Zeile')
print('%-7s %-6s %8s %8s %8s %8s %8s'%('Raum.Z','pg/rc','IoU_voll','IoU_o4','|A|','|B_voll|','|B_o4|'))
vs=[];v4=[]
for r in ECHT:
    ks=[k for k in zonen(r) if ZONES[k]['sx']>1] or [zonen(r)[0]]
    k=ks[0]; z=ZONES[k]
    Bv,R=maske(z['page'],z['rect'],True); B4,_=maske(z['page'],z['rect'],False)
    A=flaeche(ZCELLS[k],*zeile(r)[2])
    a=iou(A,Bv); b=iou(A,B4)
    vs.append(a); v4.append(b)
    print('%04X.%d  %2d/%-2d %8.3f %8.3f %8d %8d %8d'%(r,k[1],z['page'],z['rect'],a,b,len(A),len(Bv),len(B4)))
print()
print('IoU_voll  median %.3f  min %.3f  max %.3f'%(statistics.median(vs),min(vs),max(vs)))
print('IoU_ohne4 median %.3f  min %.3f  max %.3f'%(statistics.median(v4),min(v4),max(v4)))
