import sys
G=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G39k'
sys.path.insert(0, G)
from halves import halves, SPL
import numpy as np
from PIL import Image
PH={'map1F':(1,2),'map2F':(1,0),'map3F':(0,2),'mapB2':(1,2)}
PAGE={'map1F':2,'map2F':3,'map3F':4,'mapB2':1}
def down(h,px,py):
    H=(h.shape[0]-py)//3; W=(h.shape[1]-px)//3
    return h[py:py+3*H:3, px:px+3*W:3]
def lade(p): return np.array(Image.open(p).convert('RGB')).astype(int)
def register(n, sweepdir):
    L,R=halves(n); px,py=PH[n]
    l=down(L,px,py); r=down(R,px,py)
    ours=lade(f'{sweepdir}/s{PAGE[n]:02d}.bmp')
    best=None
    for oy in range(-3,4):
        for ox in range(-3,4):
            # l[y,x] corresponds to screen (x+ox, y+oy)
            ys0=max(0,-oy); ys1=min(l.shape[0], 240-oy)
            xs0=max(0,-ox); xs1=min(l.shape[1], 320-ox)
            a=l[ys0:ys1, xs0:xs1]; b=ours[ys0+oy:ys1+oy, xs0+ox:xs1+ox]
            eq=(np.abs(a-b).sum(axis=2)==0).mean()
            if best is None or eq>best[0]: best=(eq,ox,oy)
    return best,l,r,ours
if __name__=='__main__':
    sd=sys.argv[1]
    for n in SPL:
        best,l,r,ours=register(n, sd)
        print(n,'down',l.shape,'best eq/ox/oy',best)
