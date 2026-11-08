import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
fr={}
for n in range(1384,1480): fr[n]=frame(n)
ref=vorlage()
for n in range(1384,1480,2):
    d=np.abs(fr[n]-ref)
    print(n,'tail-box %.1f'%d[225:340,262:300].mean(),'wand %.2f'%d[60:140,0:150].mean())
