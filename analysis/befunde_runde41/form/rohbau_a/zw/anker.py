import sys, re
sys.path.insert(0,'.')
import numpy as np
from kn import *
import form_kamera as FK
import fit as FI
ks=knochen(); P=lade_posen(); R=posen_roh()
for n in (1,2):
    k=FK.lade_kamera(n)
    _,_,Rw,Tw=fk(ks,P['bild%d'%n])
    for nm,lok,z,g in FI.ANKER[n]:
        i=INDEX[nm]; X=Rw[i]@np.array(lok,float)+Tw[i]
        q=FK.projiziere(k,X)
        print(n,'%-12s'%nm,lok[:2],z,g,np.round(q,0),'d=%.1f'%np.linalg.norm(q-np.array(z)),'H=%d'%(-X[1]))
for n in (1,2):
    a=np.array(P['bild%d'%n]['winkel'],float); b=np.array(R['bild%d'%n]['winkel'],float)
    d=(a-b+2048)%4096-2048
    print(n,[ (ks[i]['name'],[int(x*360/4096) for x in d[i]]) for i in range(len(ks)) if abs(d[i]).max()*360/4096>12])
