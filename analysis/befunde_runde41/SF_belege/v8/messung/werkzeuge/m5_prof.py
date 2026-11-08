import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import v4_gemein as G
V=vorlage(); Lv=G.lab(V); dE=np.load(S8+'/m4_dET.npy')
for y in range(200,341,10):
    xs=range(256,372,4)
    print(y,'L ',' '.join('%2d'%Lv[y,x,0] for x in xs))
    print(y,'a ',' '.join('%2d'%Lv[y,x,1] for x in xs))
    print(y,'dE',' '.join('%2d'%min(99,dE[y,x]) for x in xs))
print('x  ',' '.join('%3d'%x for x in range(256,372,4)))
