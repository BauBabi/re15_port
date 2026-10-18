import sys
from PIL import Image
import numpy as np
SPL={'map1F':((0,954),(1103,2057)),'map2F':((0,959),(1097,2056)),'map3F':((0,958),(1164,2122)),'mapB2':((0,959),(1045,2004))}
def halves(n):
    im=np.array(Image.open(f'analysis/befunde_runde39/nutzer/{n}_correct.png').convert('RGB')).astype(int)
    (a0,a1),(b0,b1)=SPL[n]
    return im[:,a0:a1+1], im[:,b0:b1+1]
if __name__=='__main__':
    for n in SPL:
        L,R=halves(n)
        best=None
        for dy in range(-6,7):
            for dx in range(-12,13):
                # compare L[y,x] with R[y+dy, x+dx] over overlap
                h=min(L.shape[0], R.shape[0]-dy) if dy>=0 else min(L.shape[0]+dy, R.shape[0])
                ys0=max(0,-dy); ys1=min(L.shape[0], R.shape[0]-dy)
                xs0=max(0,-dx); xs1=min(L.shape[1], R.shape[1]-dx)
                a=L[ys0:ys1, xs0:xs1]; b=R[ys0+dy:ys1+dy, xs0+dx:xs1+dx]
                eq=(np.abs(a-b).sum(axis=2)<=3).mean()
                if best is None or eq>best[0]: best=(eq,dx,dy)
        print(n, L.shape, R.shape, 'best', best)
