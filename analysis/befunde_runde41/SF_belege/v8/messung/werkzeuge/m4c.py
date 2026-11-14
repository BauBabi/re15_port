import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
A=np.median(np.stack([frame(n) for n in range(1384,1399)]),0)
B=np.median(np.stack([frame(n) for n in range(1450,1473)]),0)
np.save(S8+'/m4_bgA.npy',A); np.save(S8+'/m4_bgB.npy',B)
V=vorlage()
W=Image.new('RGB',(3*320,340))
for i,X in enumerate((V,A,B)):
    W.paste(Image.fromarray(X.astype(np.uint8)).crop((220,130,540,470)),(i*320,0))
W.save(S8+'/m4_bgAB.png')
