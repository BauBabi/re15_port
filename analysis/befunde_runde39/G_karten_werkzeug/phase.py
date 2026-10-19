import sys
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G39k')
from halves import halves, SPL
import numpy as np
for n in SPL:
    L,R=halves(n)
    res=[]
    for py in range(3):
        for px in range(3):
            H=(L.shape[0]-py)//3; W=(L.shape[1]-px)//3
            blk=L[py:py+3*H, px:px+3*W].reshape(H,3,W,3,3)
            # uniform if all 9 equal to top-left
            tl=blk[:,0:1,:,0:1,:]
            uni=(np.abs(blk-tl).sum(axis=4)==0).all(axis=(1,3)).mean()
            res.append((uni,px,py))
    res.sort(reverse=True)
    print(n, res[:3])
