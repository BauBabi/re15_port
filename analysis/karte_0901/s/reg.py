# -*- coding: utf-8 -*-
"""Registrierung SCA-Flaeche <-> gezeichneter Grundriss. Beide Eingaben sind von der
   Skalenzeile UNABHAENGIG: die SCA-Zellen kommen aus dem RDT, die Maske aus MAP0x.PIX."""
import sys; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *

def maske(pg,ri,voll=True):
    """Menge der gezeichneten Pixel (Bildschirm-Koordinaten) des Rechtecks.
       voll=True: alles != 0.  voll=False: ohne Palettenindex 4 (die Wandlinien)."""
    R=rects(pg)[ri]; U,V=rect_uv(pg,ri); pix=page_pix(pg)
    out=set()
    for j in range(R[3]):
        for i in range(R[2]):
            if V+j>=256 or U+i>=256: continue
            c=pix[V+j][U+i]
            if c==0: continue
            if not voll and c==4: continue
            out.add((R[0]+i,R[1]+j))
    return out,R

def flaeche(cells, ox,oy,sx,sy):
    """Pixelmenge der projizierten SCA-Zellen (Rechteck-Fuellung)."""
    out=set()
    for (x,z,w,d) in cells:
        a=proj(x,z,ox,oy,sx,sy); b=proj(x+w,z+d,ox,oy,sx,sy)
        x0,x1=sorted((a[0],b[0])); y0,y1=sorted((a[1],b[1]))
        for yy in range(y0,y1+1):
            for xx in range(x0,x1+1): out.add((xx,yy))
    return out

def iou(A,B):
    if not A or not B: return 0.0
    return len(A&B)/float(len(A|B))
