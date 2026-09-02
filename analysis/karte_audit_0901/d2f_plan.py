# -*- coding: utf-8 -*-
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from d2f import *

def raeume(pg, minn=40):
    pix=page_pix(pg); seen=[[0]*256 for _ in range(256)]; out=[]
    for y in range(256):
        for x in range(256):
            if pix[y][x]==1 and not seen[y][x]:
                st=[(x,y)]; seen[y][x]=1; pts=[]
                while st:
                    a,b=st.pop(); pts.append((a,b))
                    for da,db in ((1,0),(-1,0),(0,1),(0,-1)):
                        c,e=a+da,b+db
                        if 0<=c<256 and 0<=e<256 and pix[e][c]==1 and not seen[e][c]:
                            seen[e][c]=1; st.append((c,e))
                if len(pts)>=minn: out.append(pts)
    out.sort(key=len,reverse=True)
    return out

def voll_bbox(pg,p):
    pix=page_pix(pg); S=set(p); W=set()
    for (a,b) in p:
        for da in (-1,0,1):
            for db in (-1,0,1):
                c,e=a+da,b+db
                if 0<=c<256 and 0<=e<256 and pix[e][c]==4: W.add((c,e))
    A=S|W; xs=[q[0] for q in A]; ys=[q[1] for q in A]
    return min(xs),max(xs),min(ys),max(ys)

def norm(p):
    x0=min(q[0] for q in p); y0=min(q[1] for q in p)
    return frozenset((a-x0,b-y0) for a,b in p)

def paare(pg):
    """(Kachel-Raum, Plan-Raum) - formgleiche Zwillinge; Plan = der mit groesserem v."""
    B=raeume(pg); byform={}
    for i,p in enumerate(B): byform.setdefault(norm(p), []).append(i)
    out=[]
    for f,idx in byform.items():
        if len(idx)==2:
            a,b=idx
            va=min(q[1] for q in B[a]); vb=min(q[1] for q in B[b])
            kachel, plan = (a,b) if va<vb else (b,a)
            out.append((kachel, plan))
        elif len(idx)>2:
            out.append(tuple(idx))
    return B, out
