# -*- coding: utf-8 -*-
"""Formregistrierung: (ox,oy,sx,sy) so, dass die projizierte SCA-Flaeche den gezeichneten
   Grundriss maximal ueberdeckt (IoU). Beide Eingaben sind zeilenfrei."""
import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
import numpy as np
from scipy.signal import fftconvolve

def bmask(pg,ri,voll=True):
    R=rects(pg)[ri]; U,V=rect_uv(pg,ri); pix=page_pix(pg)
    B=np.zeros((R[3],R[2]),bool)
    for j in range(R[3]):
        for i in range(R[2]):
            if V+j>=256 or U+i>=256: continue
            c=pix[V+j][U+i]
            if c==0: continue
            if not voll and c==4: continue
            B[j,i]=True
    return B,R

def raster0(cells,sx,sy):
    """Projizierte SCA-Flaeche bei ox=oy=0 als (arr, x0, y0)."""
    boxes=[]
    for (x,z,w,d) in cells:
        a=proj(x,z,0,0,sx,sy); b=proj(x+w,z+d,0,0,sx,sy)
        boxes.append((min(a[0],b[0]),max(a[0],b[0]),min(a[1],b[1]),max(a[1],b[1])))
    x0=min(b[0] for b in boxes); x1=max(b[1] for b in boxes)
    y0=min(b[2] for b in boxes); y1=max(b[3] for b in boxes)
    A=np.zeros((y1-y0+1,x1-x0+1),bool)
    for (bx0,bx1,by0,by1) in boxes: A[by0-y0:by1-y0+1, bx0-x0:bx1-x0+1]=True
    return A,x0,y0

def best_shift(A,x0,y0,B,R):
    """bestes (ox,oy) und Ueberlappung: A an Bildschirmposition (x0+ox, y0+oy)."""
    ov=fftconvolve(B.astype(np.float32), A[::-1,::-1].astype(np.float32), mode='full')
    ov=np.rint(ov).astype(np.int32)
    ah,aw=A.shape
    # ov[i,j] = Ueberlappung, wenn A's linke obere Ecke bei B-Index (i-ah+1, j-aw+1) liegt
    i,j=np.unravel_index(np.argmax(ov),ov.shape)
    best=int(ov[i,j])
    by=i-ah+1; bx=j-aw+1
    ox = R[0]+bx-x0; oy = R[1]+by-y0
    return ox,oy,best

def suche(cells,B,R,s_lo=768,s_hi=4352):
    nA=None; bestv=(-1,)
    def ev(sx,sy):
        A,x0,y0=raster0(cells,sx,sy)
        if A.size>400000: return None
        ox,oy,ov=best_shift(A,x0,y0,B,R)
        u=int(A.sum())+int(B.sum())-ov
        return (ov/float(u), ox,oy,sx,sy)
    for step,rad in ((128,None),(32,192),(8,48),(1,12)):
        if rad is None:
            xs=range(s_lo,s_hi+1,step); ys=range(s_lo,s_hi+1,step)
        else:
            xs=range(max(2,bestv[3]-rad),bestv[3]+rad+1,step); ys=range(max(2,bestv[4]-rad),bestv[4]+rad+1,step)
        for sx in xs:
            for sy in ys:
                r=ev(sx,sy)
                if r and r[0]>bestv[0]: bestv=r
    return bestv    # (iou, ox,oy,sx,sy)
