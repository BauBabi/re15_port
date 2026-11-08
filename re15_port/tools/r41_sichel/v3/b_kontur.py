# Leser B v3: nur Maskenkontur (wie b_schwanz.py) mit Koordinatengitter zeigen, um das Kerbfenster je Bild zu waehlen.
import sys, os, cv2, numpy as np
S,OUT=sys.argv[1],sys.argv[2]
def lab(i): return cv2.cvtColor(i.astype(np.float32)/255,cv2.COLOR_BGR2Lab)
bg=lab(cv2.imread(f"{S}/tmp/bg_modus_ki.png"))
t=[]
for n in map(int,sys.argv[3:]):
    raw=cv2.imread(f"{S}/ki/n{n}.png"); f=lab(raw)
    d=cv2.GaussianBlur(np.sqrt(((f-bg)**2).sum(2)),(5,5),0)
    m=((d>6)|((f[...,1]>12)&(f[...,0]>8))).astype(np.uint8)*255
    cs,_=cv2.findContours(m,cv2.RETR_LIST,cv2.CHAIN_APPROX_NONE)
    o=raw.copy(); cv2.drawContours(o,[c for c in cs if len(c)>40],-1,(0,255,0),1)
    for x in range(480,800,20): cv2.line(o,(x,340),(x,345),(0,255,255),1); cv2.putText(o,str(x),(x,338),0,0.3,(0,255,255),1) if x%40==0 else None
    for y in range(360,760,20): cv2.line(o,(470,y),(476,y),(0,255,255),1); cv2.putText(o,str(y),(440,y+3),0,0.3,(0,255,255),1) if y%40==0 else None
    c=o[320:760,430:800]; cv2.putText(c,f"n{n}",(5,15),0,0.5,(0,255,255),1); t.append(cv2.resize(c,None,fx=1.5,fy=1.5))
cv2.imwrite(f"{OUT}/kontur.jpg",np.hstack(t))
