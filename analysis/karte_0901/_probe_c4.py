# -*- coding: utf-8 -*-
import sys
sys.path.insert(0,'analysis/karte_audit_0901'); sys.path.insert(0,'analysis/karte_0901')
exec(open('analysis/karte_0901/_probe_c3.py',encoding='utf-8').read())

def drawn_bbox(pg,r):
    M,bb=tile_mask(pg,r); return M,bb

def wallset(pg,r):
    """Rand-/Wandpixel (Palettenindex 4) und Innenpixel (1..3) in SCHIRM-Koordinaten."""
    RX,RY,RW,RH=rects(pg)[r]; U,V=rect_uv(pg,r); pix=page_pix(pg)
    wall=set(); inner=set()
    for j in range(RH):
        for i in range(RW):
            v=pix[V+j][U+i]
            if v==0: continue
            if v==4: wall.add((RX+i,RY+j))
            else:    inner.add((RX+i,RY+j))
    return wall,inner

def proj_var(pg,r,zbb,wx,wz,fx_flip,fz_flip,use_drawn=True):
    RX,RY,RW,RH=rects(pg)[r]
    if use_drawn:
        M,bb=drawn_bbox(pg,r); RX=RX+bb[0]; RY=RY+bb[2]; RW=bb[1]-bb[0]+1; RH=bb[3]-bb[2]+1
    x0,x1,z0,z1=zbb
    fx=min(max(wx-x0,0),x1-x0); fz=min(max(wz-z0,0),z1-z0)
    dx=fx*RW//(x1-x0); dz=fz*RH//(z1-z0)
    mx = RX+RW-1-dx if fx_flip else RX+dx
    my = RY+dz      if fz_flip else RY+RH-1-dz
    return mx,my
