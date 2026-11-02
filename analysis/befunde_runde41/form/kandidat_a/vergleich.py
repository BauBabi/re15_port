#!/usr/bin/env python3
"""vergleich.py lauf bild aus [s]: Original | gefiltert | sauber nebeneinander, Ausschnitt um die Figur."""
import sys,os
import numpy as np, cv2
H=os.path.dirname(os.path.abspath(__file__))
T=os.path.normpath(os.path.join(H,'..','..','..','..','re15_port','tools'))
sys.path.insert(0,T)
import form_pruefung as P, form_video as FV
lauf,n,aus=sys.argv[1],int(sys.argv[2]),sys.argv[3]
s=int(sys.argv[4]) if len(sys.argv)>4 else 3
k=P.Kandidat(os.path.join(H,lauf,'A.json'))
sauber,alb,m,_=k.rendern(n)
gef=FV.video_filter(sauber,bild=n,entzerren=False)
o=P.original(n)
box=(185,170,445,460) if n==1 else (195,145,450,425)
def gr(a):
    x0,y0,x1,y1=box
    return cv2.resize(a[y0:y1,x0:x1],None,fx=s,fy=s,interpolation=cv2.INTER_CUBIC)
def nn(a):
    x0,y0,x1,y1=box
    return cv2.resize(a[y0:y1,x0:x1],None,fx=s,fy=s,interpolation=cv2.INTER_NEAREST)
c=np.hstack([gr(o),gr(gef),nn(sauber)])
cv2.imwrite(aus,c[:,:,::-1])
