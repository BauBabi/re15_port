# -*- coding: utf-8 -*-
import sys, math
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
P=print

def tile(pg, r, marks=()):
    RX,RY,RW,RH = rects(pg)[r]
    U,V = rect_uv(pg,r)
    pix = page_pix(pg)
    P(f"\n=== Seite {pg} Rect {r}: Schirm x[{RX}..{RX+RW-1}] y[{RY}..{RY+RH-1}]  uv=({U},{V})  {RW}x{RH}")
    hdr = "      " + "".join(str(((RX+i)//10)%10) for i in range(RW))
    hdr2= "      " + "".join(str((RX+i)%10) for i in range(RW))
    P(hdr); P(hdr2)
    for j in range(RH):
        row = ""
        for i in range(RW):
            yy,xx = V+j, U+i
            v = pix[yy][xx] if (0<=yy<256 and 0<=xx<256) else 0
            ch = '.' if v==0 else ('%X'%v)
            for (mx,my,c) in marks:
                if mx==RX+i and my==RY+j: ch=c
            row += ch
        P(f"  {RY+j:3d} {row}")
    # gezeichnete Bbox
    xs=[];ys=[]
    for j in range(RH):
        for i in range(RW):
            yy,xx=V+j,U+i
            if 0<=yy<256 and 0<=xx<256 and pix[yy][xx]!=0: xs.append(RX+i); ys.append(RY+j)
    if xs:
        P(f"  GEZEICHNET: x[{min(xs)}..{max(xs)}] y[{min(ys)}..{max(ys)}]  = {max(xs)-min(xs)+1}x{max(ys)-min(ys)+1} px, {len(xs)} Pixel")
    return set(zip(xs,ys))

# 1130-Tuer ->1120 gesnapt (152,125); 1120s Eintritt (126,149); 1120s eigene Tuer (120,149)
s5 = tile(4,5, marks=[(152,125,'A'),(126,149,'E'),(120,149,'D'),(150,126,'G'),(153,125,'g'),(144,137,'B')])
s4 = tile(4,4, marks=[(152,125,'A'),(147,84,'G'),(160,150,'x'),(168,89,'y')])
s2 = tile(4,2)
s1 = tile(4,1)
s6 = tile(4,6)
s0 = tile(4,0)
s3 = tile(4,3)

P("\n=== GEZEICHNETE UEBERSCHNEIDUNG der Seite-4-Kacheln (Pixel, nicht Bbox) ===")
S = {0:s0,1:s1,2:s2,3:s3,4:s4,5:s5,6:s6}
for i in sorted(S):
    for j in sorted(S):
        if j<=i: continue
        n = len(S[i]&S[j])
        if n: P(f"  Rect {i} n Rect {j}: {n} gemeinsame Pixel")
