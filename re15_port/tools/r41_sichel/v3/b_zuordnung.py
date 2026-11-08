# Gegenprobe der Bildzuordnung birkinNN -> n (Leser B, v3). Freisteller sind Ausschnitte: Lage per Template-Match.
import sys, numpy as np, cv2
from PIL import Image
S=sys.argv[1]; V=sys.argv[2]
for nn,n in [(12,1435),(19,1468),(20,1475)]:
    b=np.asarray(Image.open(f"{V}/birkin{nn:02d}.png").convert("RGBA"))
    m=(b[...,3]>250).astype(np.uint8)
    t=np.ascontiguousarray(b[...,:3][...,::-1])
    f=cv2.imread(f"{S}/ki/n{n}.png")
    r=cv2.matchTemplate(f,t,cv2.TM_SQDIFF,mask=m*255)
    _,_,loc,_=cv2.minMaxLoc(r); x,y=loc
    res=[]
    for k in range(n-3,n+4):
        g=cv2.imread(f"{S}/ki/n{k}.png").astype(float)[y:y+t.shape[0],x:x+t.shape[1]]
        res.append((k,round(float(np.sqrt(((g-t.astype(float))[m>0]**2).mean())),1)))
    print(f"birkin{nn:02d} Ausschnitt x{x} y{y} {t.shape[1]}x{t.shape[0]}",res)
