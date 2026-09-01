# -*- coding: utf-8 -*-
import sys, math, itertools
sys.path.insert(0,'analysis/karte_audit_0901')
from sonde_lib import *

def tile_mask(pg, r):
    """drawn-mask (True=gezeichnet) des Rechtecks, plus dessen gezeichnete bbox"""
    RX,RY,RW,RH = rects(pg)[r]
    U,V = rect_uv(pg,r)
    pix = page_pix(pg)
    M = [[pix[V+j][U+i] != 0 for i in range(RW)] for j in range(RH)]
    xs = [i for i in range(RW) for j in range(RH) if M[j][i]]
    ys = [j for j in range(RH) for i in range(RW) if M[j][i]]
    if not xs: return M, None
    return M, (min(xs), max(xs), min(ys), max(ys))

def sca_mask(room, N=64):
    """Welt-SCA -> NxN Belegungsraster ueber der Zonen-Bbox (z nach unten)"""
    sca = RDT[room][0]
    xs0=min(c[0] for c in sca); xs1=max(c[0]+c[2] for c in sca)
    zs0=min(c[1] for c in sca); zs1=max(c[1]+c[3] for c in sca)
    M=[[False]*N for _ in range(N)]
    for j in range(N):
        wz = zs0 + (zs1-zs0)*(N-1-j+0.5)/N     # j=0 -> grosses z (Norden oben)
        for i in range(N):
            wx = xs0 + (xs1-xs0)*(i+0.5)/N
            for (x,z,w,d) in sca:
                if x<=wx<x+w and z<=wz<z+d: M[j][i]=True; break
    return M,(xs0,xs1,zs0,zs1)

def resize(M, N):
    h=len(M); w=len(M[0])
    return [[M[min(h-1,int(j*h/N))][min(w-1,int(i*w/N))] for i in range(N)] for j in range(N)]

def crop(M, bb):
    x0,x1,y0,y1 = bb
    return [[M[j][i] for i in range(x0,x1+1)] for j in range(y0,y1+1)]

def iou(A,B):
    inter=sum(1 for j in range(len(A)) for i in range(len(A[0])) if A[j][i] and B[j][i])
    uni  =sum(1 for j in range(len(A)) for i in range(len(A[0])) if A[j][i] or  B[j][i])
    return inter/uni if uni else 0.0
