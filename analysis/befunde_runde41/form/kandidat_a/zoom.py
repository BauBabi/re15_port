#!/usr/bin/env python3
"""zoom.py lauf bild x0 y0 x1 y1 s aus : Ausschnitt Original + Zielmaske + Render-Umriss."""
import sys,os,json
import numpy as np, cv2
H=os.path.dirname(os.path.abspath(__file__))
T=os.path.normpath(os.path.join(H,'..','..','..','..','re15_port','tools'))
sys.path.insert(0,T)
import form_pruefung as P
lauf,n,x0,y0,x1,y1,s,aus=sys.argv[1],int(sys.argv[2]),*[int(v) for v in sys.argv[3:8]],sys.argv[8]
k=P.Kandidat(os.path.join(H,lauf,'A.json'))
sauber,alb,m,_=k.rendern(n)
o=P.original(n)
z=cv2.imread(os.path.join(P.FORM,'ziel_bild%d.png'%n),0)
f=cv2.imread(os.path.join(P.FORM,'pruefung','figur_bild%d.png'%n),0)>0
def gr(a): return cv2.resize(a[y0:y1,x0:x1],None,fx=s,fy=s,interpolation=cv2.INTER_NEAREST)
a=gr(o).copy()
for mm,c in ((z==255,(70,230,90)),(z>0,(70,200,255)),(f,(255,120,255)),(m,(255,245,0))):
    q=gr(mm.astype(np.uint8)).astype(np.uint8)
    cs,_=cv2.findContours(q,cv2.RETR_LIST,cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a,cs,-1,c,1)
b=gr(sauber).copy()
c=np.hstack([a,b])
cv2.imwrite(aus,c[:,:,::-1])
