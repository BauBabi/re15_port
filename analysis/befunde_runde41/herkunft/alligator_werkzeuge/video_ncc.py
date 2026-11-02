# Jedes Videobild (2 B/s) gegen alle Disc-Hintergruende (531) + Mod-209: normierte Korrelation 64x48 Graustufen, Mittelausschnitt.
import glob,os,sys,json,collections
import numpy as np
from PIL import Image
SP=r"/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad".replace('/c/','C:/')
def vec(im,box=None):
    im=im.convert('L')
    if box: im=im.crop(box)
    a=np.asarray(im.resize((64,48)),float).ravel()
    a=a-a.mean(); n=np.linalg.norm(a)
    return a/n if n else a
names=[];B=[]
for f in sorted(glob.glob(SP+'/s40/bg40/ROOM*.ppm')):
    im=Image.open(f); w,h=im.size
    names.append(os.path.basename(f)[4:-4]); B.append(vec(im,(int(w*.08),int(h*.08),int(w*.92),int(h*.92))))
for f in sorted(glob.glob(SP+'/bgall/ROOM209*.ppm')+glob.glob(SP+'/bgall/ROOM208*.ppm')+glob.glob(SP+'/bgall/ROOM20A*.ppm')):
    im=Image.open(f); w,h=im.size
    names.append('M'+os.path.basename(f)[4:-4]); B.append(vec(im,(int(w*.08),int(h*.08),int(w*.92),int(h*.92))))
B=np.array(B)
print(B.shape)
out={}
for v in '123456':
    fs=sorted(glob.glob(SP+f'/gator40/v/{v}/f_*.jpg'))
    best=[]
    for i,f in enumerate(fs):
        im=Image.open(f); 
        w,h=im.size
        x=vec(im,(int(w*.12),int(h*.12),int(w*.88),int(h*.88)))
        s=B@x
        o=np.argsort(-s)[:3]
        best.append((i/2.0,[(names[k],round(float(s[k]),3)) for k in o]))
    out[v]=best
    c=collections.Counter(b[1][0][0][:3] for b in best if b[1][0][1]>=0.75)
    print('Video',v,'Bilder',len(fs),'Top-Raeume (>=0.75):',c.most_common(25))
    for t,b in best:
        if any(n.startswith(('209','208','20A','M209','M208','M20A')) and sc>=0.6 for n,sc in b[:1]):
            print('  ',t,b)
json.dump(out,open(SP+'/gator40/ncc.json','w'))
