# -*- coding: utf-8 -*-
import sys
sys.path.insert(0,'analysis/karte_audit_0901'); sys.path.insert(0,'analysis/karte_0901')
exec(open('analysis/karte_0901/_probe_c2.py',encoding='utf-8').read())
from sonde_lib import *

def tile_mask(pg,r):
    RX,RY,RW,RH=rects(pg)[r]; U,V=rect_uv(pg,r); pix=page_pix(pg)
    M=[[pix[V+j][U+i]!=0 for i in range(RW)] for j in range(RH)]
    xs=[i for i in range(RW) for j in range(RH) if M[j][i]]
    ys=[j for j in range(RH) for i in range(RW) if M[j][i]]
    if not xs: return None,None
    return M,(min(xs),max(xs),min(ys),max(ys))

def tile_grid(pg,r,N):
    """Kachel-Innenraum (ohne Rahmen? nein: alles Gezeichnete) auf NxN normiert (bbox-Streckung)."""
    M,bb=tile_mask(pg,r)
    if bb is None: return None
    x0,x1,y0,y1=bb; w=x1-x0+1; h=y1-y0+1
    G=[[False]*N for _ in range(N)]
    for j in range(N):
        for i in range(N):
            si=x0+int(i*w/N); sj=y0+int(j*h/N)
            G[j][i]=M[sj][si]
    return G

def room_grid(pts,N,flipx=False,flipz=False):
    x0=min(p[0] for p in pts); x1=max(p[0] for p in pts)
    z0=min(p[1] for p in pts); z1=max(p[1] for p in pts)
    G=[[False]*N for _ in range(N)]
    for (x,z) in pts:
        u=(x-x0)/max(1,x1-x0); v=(z1-z)/max(1,z1-z0)
        if flipx: u=1-u
        if flipz: v=1-v
        i=min(N-1,int(u*N)); j=min(N-1,int(v*N)); G[j][i]=True
    return G

def iou(A,B):
    N=len(A); it=un=0
    for j in range(N):
        for i in range(N):
            a=A[j][i]; b=B[j][i]
            if a and b: it+=1
            if a or b: un+=1
    return it/un if un else 0.0
