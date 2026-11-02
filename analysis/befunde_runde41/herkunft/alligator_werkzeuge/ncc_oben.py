# Mittel der Videobilder 86.57..87.17 s; Vergleich NUR Wand-Bereich (oberes Bilddrittel) gegen alle Hintergruende, mit Skalen-/Versatzsuche.
import glob,os,sys
import numpy as np
from PIL import Image
SP=r"/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad".replace('/c/','C:/')
fs=sorted(glob.glob(SP+'/gator40/z2/f_*.png'))[11:21]
A=np.mean([np.asarray(Image.open(f).convert('L').resize((320,240)),float) for f in fs],0)
Image.fromarray(A.astype('uint8')).save(SP+'/gator40/z/avg.png')
tpl=A[4:92,30:300]      # Oberer Bereich Wand+Leiter, ohne Wasserzeichen
tpl=np.log1p(tpl)
t0=tpl-tpl.mean(); t0/=np.linalg.norm(t0)
th,tw=tpl.shape
res=[]
fl=sorted(glob.glob(SP+'/s40/bg40/ROOM*.ppm'))+[f for f in sorted(glob.glob(SP+'/bgall/ROOM*.ppm')) if os.path.basename(f)[4:7] in ('209','208','20A','200','201','202','203','204','205','206','207','20B')]
for f in fl:
    im=Image.open(f).convert('L')
    tag=('D' if 'bg40' in f else 'M')+os.path.basename(f)[4:-4]
    best=-1;bp=None
    for sc in (0.9,1.0,1.1,1.2,1.3):
        W=int(320*sc);H=int(240*sc)
        B=np.log1p(np.asarray(im.resize((W,H)),float))
        for dy in range(0,max(1,H-th-40),6):
            for dx in range(0,max(1,W-tw),6):
                c=B[dy:dy+th,dx:dx+tw]
                c=c-c.mean(); n=np.linalg.norm(c)
                if n==0: continue
                s=float((c/n*t0).sum())
                if s>best: best=s;bp=(sc,dx,dy)
    res.append((best,tag,bp))
res.sort(reverse=True)
for r in res[:16]: print(r)
print('Rang D20900:',[i for i,r in enumerate(res) if r[1]=='D20900'],'von',len(res))
