import sys
G=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G39k'
sys.path.insert(0, G)
from reg import register, SPL, PAGE
import numpy as np
from PIL import Image
OFF={}
def soll_bild(n, sd):
    best,l,r,ours=register(n, sd)
    eq,ox,oy=best
    # palette from l
    pal=np.unique(l.reshape(-1,3),axis=0)
    # snap r to palette
    rr=r.reshape(-1,3)
    d=((rr[:,None,:]-pal[None,:,:])**2).sum(axis=2)
    snap=pal[d.argmin(axis=1)].reshape(r.shape)
    maxd=np.sqrt(d.min(axis=1)).max()
    # place into 320x240 canvas (unknown border = from ours)
    S=ours.copy(); Lc=ours.copy()
    ys0=max(0,-oy); ys1=min(l.shape[0], 240-oy); xs0=max(0,-ox); xs1=min(l.shape[1], 320-ox)
    S[ys0+oy:ys1+oy, xs0+ox:xs1+ox]=snap[ys0:ys1, xs0:xs1]
    Lc[ys0+oy:ys1+oy, xs0+ox:xs1+ox]=l[ys0:ys1, xs0:xs1]
    return S, Lc, ours, maxd
if __name__=='__main__':
    sd=sys.argv[1]
    for n in SPL:
        S,Lc,ours,maxd=soll_bild(n, sd)
        Image.fromarray(S.astype(np.uint8)).save(f'{G}/soll_{n}.png')
        Image.fromarray(Lc.astype(np.uint8)).save(f'{G}/ist_{n}.png')
        diff=(np.abs(S-Lc).sum(axis=2)>0)
        ys,xs=np.nonzero(diff)
        print(n,'snap maxdist %.1f'%maxd,'diff px',diff.sum(),'bbox', (xs.min(),ys.min(),xs.max(),ys.max()) if len(xs) else None)
