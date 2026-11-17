import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import v4_gemein as G
bgv=G.videoraum(G.hintergrund().astype(float))
ns=[n for n in range(1352,1476) if n not in range(1418,1436)]  # ohne Vordergrund-Verdecker 1418-1435
F=np.stack([frame(n) for n in ns])          # N,480,640,3
Fl=G.lab(F.reshape(-1,640,3)).reshape(F.shape)
Bl=G.lab(bgv)
d=np.sqrt(((Fl-Bl[None])**2).sum(-1))       # N,480,640
# je Pixel: Frames, die nahe am BSS sind (d< P30 je Pixel), Median davon
k=max(3,len(ns)//4)
idx=np.argsort(d,0)[:k]
sel=np.take_along_axis(F,idx[...,None],0)
bgT=np.median(sel,0)
np.save(S8+'/m4_bgT.npy',bgT)
V=vorlage()
dE=np.sqrt(((G.lab(V)-G.lab(bgT))**2).sum(2))
np.save(S8+'/m4_dET.npy',dE)
Image.fromarray(bgT.astype(np.uint8)).save(S8+'/m4_bgT.png')
Image.fromarray(np.clip(dE*8,0,255).astype(np.uint8)).crop((200,120,520,460)).resize((640,680),Image.NEAREST).save(S8+'/m4_dET.png')
