import glob,os,sys
import numpy as np
from PIL import Image
SP=r"/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad".replace('/c/','C:/')
def vec(im,box):
    im=im.convert('L').crop(box)
    a=np.asarray(im.resize((64,48)),float)
    a=np.log1p(a)      # Ueberstrahlung des Fernsehbilds abmildern
    a=a.ravel(); a=a-a.mean(); n=np.linalg.norm(a)
    return a/n if n else a
names=[];B=[]
fl=sorted(glob.glob(SP+'/s40/bg40/ROOM*.ppm'))+sorted(glob.glob(SP+'/bgall/ROOM*.ppm'))
for f in fl:
    im=Image.open(f); w,h=im.size
    tag=('D' if 'bg40' in f else 'M')+os.path.basename(f)[4:-4]
    names.append(tag); B.append(vec(im,(int(w*.03),int(h*.04),int(w*.97),int(h*.80))))
B=np.array(B)
im=Image.open(SP+'/gator40/z/v1_87.0.png'); w,h=im.size
for sx in (0.0,0.04):
    x=vec(im,(int(w*sx),int(h*.02),int(w*(1-sx)),int(h*.82)))
    s=B@x; o=np.argsort(-s)[:14]
    print(sx,[(names[k],round(float(s[k]),3)) for k in o])
