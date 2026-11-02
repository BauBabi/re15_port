import sys
sys.path.insert(0,'.')
from kn import *
import numpy as np, form_kamera as FK
ks=knochen(); P=lade_posen()
def lokal(n,b,q,tiefe):
    k=FK.lade_kamera(n)
    _,_,Rw,Tw=fk(ks,P['bild%d'%n])
    au,d=FK.strahl(k,np.array(q,float))
    vor=k['R'][2]
    s=(tiefe)/(d@vor)
    X=au+d*s
    i=INDEX[b]
    return np.round(Rw[i].T@(X-Tw[i])).tolist(), np.round(X).tolist()
for q in ((429,272),(420,262),(411,240),(402,258),(405,290)):
    for t in (7300,7700):
        print(q,t,lokal(1,'oberarm_r',q,t))
k=FK.lade_kamera(1); _,_,Rw,Tw=fk(ks,P['bild1'])
for nm in ('oberarm_r','unterarm_r','brust'):
    i=INDEX[nm]; print(nm,np.round(FK.projiziere(k,Tw[i])),round(FK.tiefe(k,Tw[i])))
# wo landet derselbe Punkt links in Bild 2
k2=FK.lade_kamera(2); _,_,Rw2,Tw2=fk(ks,P['bild2'])
i=INDEX['oberarm_l']
for p in ((-260,-120,-650),(-120,-60,-500)):
    v=np.array(p,float)*np.array([1,1,-1])
    print('links b2',p,np.round(FK.projiziere(k2,Rw2[i]@v+Tw2[i])))
