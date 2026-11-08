# Leser A V4: Vorlagebilder im Original-Video (ohne _slp) per Bildsuche finden (sequentielles Dekodieren, n ab 0)
import sys, numpy as np, cv2
from PIL import Image
V='C:/workspace/git/reAi_v2/Version_80%/'
OUT=sys.argv[1]
vid=V+'Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p).mp4'
T=[np.array(Image.open(V+f).convert('RGB')).astype(np.float32) for f in ['Beschreibung1_Vorlage.png','Beschreibung2_Vorlage.png']]
c=cv2.VideoCapture(vid); n=0; res=[[],[]]; frames={}
while True:
    ok,fr=c.read()
    if not ok: break
    if n>=1200:
        rgb=fr[:,:,::-1].astype(np.float32)
        for i,t in enumerate(T): res[i].append((float(np.abs(rgb-t).mean()),n))
        frames[n]=fr[:,:,::-1].copy()
    n+=1
print('Bilder gesamt',n)
for i in range(2):
    r=sorted(res[i])[:6]; print('Vorlage',i+1,'beste (MAE,n):',[(round(a,2),b) for a,b in r])
    best=r[0][1]
    for k in range(best-3,best+4):
        if k in frames: Image.fromarray(frames[k]).save(OUT+'/v%d_n%04d.png'%(i+1,k))
np.save(OUT+'/bildsuche.npy',np.array([[a for a,b in sorted(res[0],key=lambda z:z[1])],[a for a,b in sorted(res[1],key=lambda z:z[1])]]))
