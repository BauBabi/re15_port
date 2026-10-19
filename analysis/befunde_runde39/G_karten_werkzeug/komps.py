import sys; sys.path.insert(0,'.')
from modell import *
from collections import Counter
from scipy import ndimage
def komponenten(seite):
    S=Seite(seite)
    d=vergleich(S.ist,S.soll)
    lab,n=ndimage.label(d, structure=np.ones((3,3)))
    top=S.oberste(S.ops)
    print('==',NAMEN[seite],'diff px',d.sum(),'komponenten',n)
    for k in range(1,n+1):
        ys,xs=np.nonzero(lab==k)
        tr=Counter((NAME[S.ist[y,x]],NAME[S.soll[y,x]]) for y,x in zip(ys,xs))
        opsx=Counter(top[(y,x)][0] for y,x in zip(ys,xs) if (y,x) in top)
        print(f' K{k}: x{xs.min()}..{xs.max()} y{ys.min()}..{ys.max()} n={len(xs)} {dict(tr)} ops={dict(opsx)}')
    return S
if __name__=='__main__':
    for s in map(int,sys.argv[1:]): komponenten(s)
