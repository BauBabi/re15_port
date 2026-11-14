import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import v4_gemein as G, cv2
bg=G.hintergrund().astype(float)
bgv=G.videoraum(bg)
V=vorlage()
L1=G.lab(V); L2=G.lab(bgv)
dE=np.sqrt(((L1-L2)**2).sum(2))
np.save(S8+'/m4_dE.npy',dE)
im=np.clip(dE*8,0,255).astype(np.uint8)
Image.fromarray(im).crop((200,120,520,460)).resize((640,680),Image.NEAREST).save(S8+'/m4_dE.png')
Image.fromarray(bgv.astype(np.uint8)).save(S8+'/m4_bgv.png')
for t in (4,6,8,10,15): print(t,(dE[140:450,240:500]>t).sum())
