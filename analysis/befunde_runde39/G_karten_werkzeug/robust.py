import sys, copy; sys.path.insert(0,'.')
from edits import *
from collections import Counter
def als_aktuell(ops, ri):
    vorne=[]; rest=[]
    for o in ops:
        if ist_kachel(o) and o.get('rect')==ri:
            o2=dict(o); o2['clut']=18; vorne.append(o2)
        else: rest.append(o)
    # Kacheln des aktuellen Raums zuerst (Marken bleiben vorne: Marken stehen vor den Kacheln)
    marken=[o for o in rest if not ist_kachel(o) and rest.index(o) < next((k for k,x in enumerate(rest) if ist_kachel(x)), len(rest))]
    andere=[o for o in rest if o not in marken]
    return marken+vorne+andere
for seite in map(int, sys.argv[1:]):
    S=Seite(seite); bas=S.basis()
    soll_w=(S.soll==KL['W']); m=np.zeros(soll_w.shape,bool); m[BOX]=True; soll_w&=m
    ops0=verschiebe(S.ops,seite)
    opsE=wende_an(S, ops0, seite)
    opsI=wende_an(S, ops0, seite, edits=[])
    R=rect_liste(seite)
    print('==',NAMEN[seite])
    for ri in range(len(R)):
        for name,opsx in (('IST',opsI),('NEU',opsE)):
            oa=als_aktuell(opsx, ri)
            mod=render_e(S,oa,bas)
            bad=soll_w & ~np.isin(mod,[KL['W'],KL['D'],KL['S']])
            if bad.sum():
                ys,xs=np.nonzero(bad)
                print(f'  rect{ri} aktuell {name}: {bad.sum()} Wandpixel nicht W', Counter(NAME[mod[y,x]] for y,x in zip(ys,xs)), 'x',xs.min(),xs.max(),'y',ys.min(),ys.max())
