import sys, json
sys.path.insert(0,'.')
from kn import *
import numpy as np, form_kamera as FK, bau, entwurf
import netz as NZ
ks=knochen(); P=lade_posen()
N=NZ.netze(entwurf.alle(), bau.korr_laden())
k=FK.lade_kamera(2)
_,_,Rw,Tw=fk(ks,P['bild2'])
aus=[]
for m in N:
    if m['name'] not in ('brust','wirbel','huefte','hals','kopf'): continue
    X=m['V']@Rw[m['knochen']].T+Tw[m['knochen']]
    b=FK.projiziere(k,X)
    t=m['teil']
    q=np.array(json.load(open('korr.json'))[m['name']])
    for idx in np.argsort(b[:,1])[:4]:
        i,j=idx//t.n, idx%t.n
        aus.append((round(b[idx,1],1),round(b[idx,0],1),m['name'],int(i),int(j),round(float(t.TH[i,j])),round(float(t.R[i,j])),round(float(q[i,j]),2)))
for a in sorted(aus)[:10]: print(a)
