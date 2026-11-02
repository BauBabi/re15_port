import sys, json
sys.path.insert(0,'.')
from kn import *
import numpy as np, form_kamera as FK
ks=knochen(); P=lade_posen()
q=json.load(open('korr.json'))
for n in (1,2):
    k=FK.lade_kamera(n)
    _,_,Rw,Tw=fk(ks,P['bild%d'%n])
    for nm in ('oberschenkel_r','unterschenkel_r','fuss_r','oberschenkel_l','unterschenkel_l','fuss_l'):
        i=INDEX[nm]
        L=ks[i]['laenge']
        mitte=Tw[i]+Rw[i]@np.array([0,L*.5,0] if 'schenkel' in nm else [L*.5,0,0])
        blick=mitte-np.array(k['auge']); blick/=np.linalg.norm(blick)
        print(n,nm,'Blick lokal',np.round(Rw[i].T@blick,2),'Winkel',P['bild%d'%n]['winkel'][i])
np.set_printoptions(precision=2,suppress=True,linewidth=150)
print(np.array(q['oberschenkel_r']))
print(np.array(q['oberschenkel_l']))
