import numpy as np, os, json
from PIL import Image
A=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/roh/alle'
V=np.asarray(Image.open(r'C:/workspace/git/reAi_v2/Version_80%/Darstellung_Vorlage.png').convert('RGB'),np.float32)
fs=sorted(os.listdir(A))
im0=Image.open(os.path.join(A,fs[0])); print('size',im0.size)
res=[]
for f in fs:
    I=np.asarray(Image.open(os.path.join(A,f)).convert('RGB'),np.float32)
    if I.shape!=V.shape: I=np.asarray(Image.open(os.path.join(A,f)).convert('RGB').resize((640,480)),np.float32)
    d=np.abs(I-V).mean()
    dfig=np.abs(I[150:450,250:500]-V[150:450,250:500]).mean()
    res.append((d,dfig,f))
res.sort()
for r in res[:12]: print('%.3f %.3f %s'%r)
json.dump(res,open('m1_suche.json','w'))
