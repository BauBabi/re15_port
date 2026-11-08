"""S2 geometrisch: Rumpf (Knochen 0-15) und Schwanz (16-22) getrennt gerastert (b2.umriss_schnell, PV). Je Spalte x 330-410:
Oberkante Rumpf und Schwanz. Huckel an J = groesste Hoehe, um die der Rumpf in den Spalten, in denen der Schwanz noch liegt
(und 10 px rechts davon), ueber die Schwanz-Oberkante bzw. ihre Fortsetzung (Gerade durch die letzten 15 Schwanzspalten) ragt."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, b2
def j_huckel(mo,PV,log=False):
    mr=b2.umriss_schnell(mo,PV,knochen=list(range(16))); ms=b2.umriss_schnell(mo,PV,knochen=list(range(16,23)))
    top=lambda m,x: int(np.nonzero(m[:,x])[0].min()) if m[:,x].any() else None
    tr={x:top(mr,x) for x in range(280,460)}; ts={x:top(ms,x) for x in range(280,460)}
    xs_s=[x for x in ts if ts[x] is not None]; xe=max(xs_s)   # rechtes Ende des Schwanzes im Bild
    fit=[x for x in range(xe-25,xe-9) if ts.get(x) is not None]
    c=np.polyfit(fit,[ts[x] for x in fit],1)
    H=[]
    for x in range(xe-25,min(xe+25,459)):
        if tr.get(x) is None: continue
        ref=ts[x] if ts.get(x) is not None else np.polyval(c,x)
        H.append((ref-tr[x],x))
    h=max(H)
    if log: print('Schwanz rechtes Ende x',xe,'Huckel an J %.1f px bei x %d'%h)
    return dict(schwanz_ende_x=xe,huckel_px=round(float(h[0]),1),bei_x=int(h[1]),oben_rumpf={x:tr[x] for x in range(320,420,5)},oben_schwanz={x:ts[x] for x in range(320,420,5)})
if __name__=='__main__':
    mo=b2.modell(EMD_V7); PV=dict(clip=17,bild=47,gier=587,x=-8246,z=-21266)
    r=j_huckel(mo,PV,True); print(r)
    json.dump(r,open(S8+'/m26_huckel_j.json','w'))
