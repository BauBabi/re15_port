# -*- coding: utf-8 -*-
"""Maske der Hand auf die Bodenebene zurueckrechnen und im Handraum zeichnen (Draufsicht). Hilfe zum Bau der Hand."""
import numpy as np, cv2
import figur, schau
from schau import FK, MASKEN, LOGO
f=figur.bauen(); fig=figur.fertig(f)
def plan(n,knochen,maske,hoehe,pfad):
    kam=FK.lade_kamera(n); p=fig['posen']['bild%d'%n]
    R,T=schau.fk(fig['eltern'],fig['versatz'],p['wurzel'],p['winkel'])
    Y=schau.ry(p['gier']*schau.E); la=np.array(p['lage'],float)
    b=f.ix[knochen]
    ys,xs=np.nonzero(maske)
    pts=np.stack([xs,ys],1).astype(float)
    w=FK.strahl_auf_boden(kam,pts,y=-hoehe)
    ok=np.isfinite(w).all(1)
    w=w[ok]; pts=pts[ok]
    # Welt -> Knochenraum
    a=(w-la)@Y   # Y^T(w-la)
    lok=(a-T[b])@R[b]
    S=0.1; W,H=420,420
    im=np.full((H,W,3),30,np.uint8)
    ux=(lok[:,2]*S+W/2).astype(int); uy=(H-40-lok[:,0]*S).astype(int)
    k=(ux>=0)&(ux<W)&(uy>=0)&(uy<H)
    lg=LOGO[ys[ok],xs[ok]]
    im[uy[k],ux[k]]=np.where(lg[k,None],[90,90,160],[220,220,220])
    for v in range(-2000,2001,500):
        cv2.line(im,(int(v*S+W/2),0),(int(v*S+W/2),H),(70,70,70),1)
        cv2.line(im,(0,int(H-40-v*S)),(W,int(H-40-v*S)),(70,70,70),1)
        cv2.putText(im,str(v),(int(v*S+W/2)+2,12),0,.35,(0,255,255),1)
        cv2.putText(im,str(v),(2,int(H-40-v*S)-2),0,.35,(0,255,255),1)
    cv2.imwrite(pfad,im)
    print(knochen,'lokal x',lok[:,0].min(),lok[:,0].max(),'z',lok[:,2].min(),lok[:,2].max(),'y',lok[:,1].mean())
m=MASKEN['right_arm_front'].copy()
m[:,285:]=False
plan(2,'hand_r',m,150,'lauf/plan_hand_r.png')
m=MASKEN['left_arm_front'].copy()
m[:365,:]=False
plan(2,'hand_l',m,150,'lauf/plan_hand_l.png')
