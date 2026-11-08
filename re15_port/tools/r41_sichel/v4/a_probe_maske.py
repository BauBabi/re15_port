# Leser A V4: Rote Linienmaske + Komponenten (Probe)
import sys, numpy as np, cv2
from PIL import Image
V='C:/workspace/git/reAi_v2/Version_80%/'
RED=np.array([237,28,36],float)
def maske(f):
    B=np.array(Image.open(V+f+'.png').convert('RGB')).astype(float)
    W=np.array([255,255,255.])
    # Abstand zur Strecke rot->weiss (Antialias gegen weiss)
    d=W-RED; t=((B-RED)@d)/(d@d); t=np.clip(t,0,1)
    proj=RED+t[...,None]*d; dist=np.linalg.norm(B-proj,axis=2)
    core=np.linalg.norm(B-RED,axis=2)<=40
    return B,core
for f in ['Beschreibung1','Beschreibung2']:
    B,core=maske(f)
    n,lab,st,cen=cv2.connectedComponentsWithStats(core.astype(np.uint8),connectivity=8)
    print(f,'core px',core.sum(),'comps',n-1)
    for i in range(1,n):
        x,y,w,h,a=st[i]
        if a>=15: print('  comp',i,'bbox x%d-%d y%d-%d area %d'%(x,x+w-1,y,y+h-1,a))
