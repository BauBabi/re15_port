import numpy as np, cv2, json
from skimage.morphology import skeletonize
from scipy import ndimage as ndi
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
PAL={'rot':(237,28,36),'gruen':(34,177,76),'grau':(127,127,127),'blau':(63,72,204),'lila':(163,73,164),'orange':(255,127,39),
     'dunkelrot':(136,0,21),'hellblau':(0,162,232),'braun':(185,122,87)}
vs=json.load(open('versatz.json'))
info={}
for lf,v in vs.items():
    L=lies(D+lf).astype(int); B=lies(D+v['grund']).astype(int); H,W=L.shape[:2]; h,w=B.shape[:2]
    Bc=np.zeros_like(L); Bc[v['dy']:v['dy']+h, v['dx']:v['dx']+w]=B
    # Grundlage-Farbe auf weissem Grund (wie angezeigt) und beschriftetes Bild ebenso
    def flach(X): a=X[...,3:4]/255.0; return X[...,:3][..., ::-1]*a+255*(1-a)
    Lf=flach(L); Bf=flach(Bc)
    diff=np.abs(L-Bc).max(2)>0
    dB=np.sqrt(((Lf-Bf)**2).sum(2))
    best=None; lab=np.full((H,W),-1)
    names=list(PAL)
    dist=np.stack([np.sqrt(((Lf-np.array(PAL[k]))**2).sum(2)) for k in names])
    k=dist.argmin(0); dmin=dist.min(0)
    # Anteil der Annotationsfarbe (Mischung base*(1-t)+c*t): t = Projektion
    t=np.zeros((H,W))
    for i,n in enumerate(names):
        c=np.array(PAL[n]); d=c-Bf; den=(d*d).sum(2)+1e-9; ti=((Lf-Bf)*d).sum(2)/den
        res=np.sqrt((((Bf+ti[...,None]*d)-Lf)**2).sum(2))
        sel=(k==i)
        t[sel]=np.where(res[sel]<25,ti[sel],0)
    anno=diff&(t>0.45)&(dmin<120)
    masks={}
    for i,n in enumerate(names):
        m=anno&(k==i)
        if m.sum()>20: masks[n]=m
    info[lf]={n:int(m.sum()) for n,m in masks.items()}
    np.savez_compressed('anno_%s.npz'%lf.replace('.png','').encode('ascii','replace').decode().replace('?','_'),**{n:m for n,m in masks.items()})
    print(lf,info[lf])
json.dump(info,open('anno_info.json','w'),ensure_ascii=False,indent=1)
