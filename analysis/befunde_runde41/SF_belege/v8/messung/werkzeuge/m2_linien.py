import numpy as np, json
from PIL import Image
from scipy import ndimage as nd
D=np.asarray(Image.open(r'C:/workspace/git/reAi_v2/Version_80%/Darstellung.png').convert('RGBA'),np.int32)
V=np.asarray(Image.open(r'C:/workspace/git/reAi_v2/Version_80%/Darstellung_Vorlage.png').convert('RGB'),np.int32)
print('alpha uniq',np.unique(D[...,3])[:5])
d=np.abs(D[...,:3]-V).max(2)
m=d>40
print('diff px',m.sum())
lab,n=nd.label(m,np.ones((3,3)))
print('komponenten',n)
cols={}
for i in range(1,n+1):
    ys,xs=np.nonzero(lab==i)
    if len(ys)<15: continue
    c=D[ys,xs,:3].mean(0).astype(int)
    print(i,len(ys),'x',xs.min(),xs.max(),'y',ys.min(),ys.max(),'rgb',c)
Image.fromarray((m*255).astype(np.uint8)).save('m2_diffmaske.png')
