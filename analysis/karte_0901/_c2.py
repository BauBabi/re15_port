# -*- coding: utf-8 -*-
import sys, os, math, itertools
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
P=print

# --- snap_wall 1:1 aus gen_map_zones.py nachgebaut ---
def snap_wall(pg, r, mx, my, senk):
    R = rects(pg)
    if r >= len(R): return mx, my, (3 if senk else 0)
    RX, RY, RW, RH = R[r]
    pix = page_pix(pg)
    if pix is None: return mx, my, (3 if senk else 0)
    U, V = rect_uv(pg, r)
    def drawn(i, j):
        if not (0 <= i < RW and 0 <= j < RH): return False
        yy, xx = V + j, U + i
        if not (0 <= yy < 256 and 0 <= xx < 256): return False
        return pix[yy][xx] != 0
    QUER = 3
    i0, j0 = mx - RX, my - RY
    best = None
    if senk:
        for dj in range(-QUER, QUER+1):
            j = j0+dj
            if not (0 <= j < RH): continue
            for i in range(RW):
                if not drawn(i,j): continue
                if not (i==0 or not drawn(i-1,j) or i==RW-1 or not drawn(i+1,j)): continue
                k = abs(i-i0)+2*abs(dj); s = 3 if drawn(i+1,j) else 1
                if best is None or k < best[0]: best=(k,i,j,s)
        if best is None: return mx,my,3
        return RX+best[1], RY+best[2], best[3]
    else:
        for di in range(-QUER, QUER+1):
            i = i0+di
            if not (0 <= i < RW): continue
            for j in range(RH):
                if not drawn(i,j): continue
                if not (j==0 or not drawn(i,j-1) or j==RH-1 or not drawn(i,j+1)): continue
                k = abs(j-j0)+2*abs(di); s = 0 if drawn(i,j+1) else 2
                if best is None or k < best[0]: best=(k,i,j,s)
        if best is None: return mx,my,0
        return RX+best[1], RY+best[2], best[3]

SEITE = {0:'Nord',1:'Ost',2:'Sued',3:'West'}

P("############ 1. TUER-PROJEKTIONEN (Bbox-Streckung, aktuelle Regel) ############")
for room in (0x1130, 0x1120):
    z = ZONES[(room,0)]
    R = rects(z['page'])[z['rect']]
    P(f"\n--- ROOM{room:04X}  Zone0 bbox x[{z['x0']}..{z['x1']}] z[{z['z0']}..{z['z1']}]"
      f"  -> Seite {z['page']} Rect {z['rect']} = {R}")
    for i,d in enumerate(doors_of(room)):
        p = proj_bbox(z, d['lx'], d['lz'])
        senk = d['rd'] > d['rw']
        sx_, sy_, sd = snap_wall(z['page'], z['rect'], p[0], p[1], senk)
        cov = 'KACHEL-SYMBOL' if any(abs(sx_-g[0])<=4+g[2]//2 and abs(sy_-g[1])<=4+g[3]//2
                for g in GLYPHEN.get((z['page'],z['rect']),())) else '-'
        P(f"   ->{d['dest']:04X}  welt({d['lx']:7d},{d['lz']:7d})  roh=({p[0]:3d},{p[1]:3d})"
          f"  gesnapt=({sx_:3d},{sy_:3d}) Wand={SEITE[sd]:5s} senk={senk}  {cov}")

P("\n############ 2. WO BETRITT DER SPIELER 1120 ? ############")
z20 = ZONES[(0x1120,0)]; z30 = ZONES[(0x1130,0)]
d3020 = [d for d in doors_of(0x1130) if d['dest']==0x1120][0]
d2030 = [d for d in doors_of(0x1120) if d['dest']==0x1130][0]
P(f"  1130-Tuer ->1120: Ort welt({d3020['lx']},{d3020['lz']})  ZIEL welt({d3020['nx']},{d3020['nz']})")
P(f"  1120-Tuer ->1130: Ort welt({d2030['lx']},{d2030['lz']})  ZIEL welt({d2030['nx']},{d2030['nz']})")
P(f"  Paarprobe: |ziel(1130)-ort(1120)| = {abs(d3020['nx']-d2030['lx'])+abs(d3020['nz']-d2030['lz'])}")
P(f"             |ziel(1120)-ort(1130)| = {abs(d2030['nx']-d3020['lx'])+abs(d2030['nz']-d3020['lz'])}")
pe = proj_bbox(z20, d3020['nx'], d3020['nz'])
pt = proj_bbox(z20, d2030['lx'], d2030['lz'])
P(f"  EINTRITTSPUNKT in 1120 projiziert (1120-Regel): {pe}")
P(f"  1120s eigene Tuer ->1130 projiziert          : {pt}")
p30 = proj_bbox(z30, d3020['lx'], d3020['lz'])
P(f"  1130s Tuer ->1120 projiziert (1130-Regel)    : {p30}")
P(f"  ABSTAND der beiden Tuerpunkte auf dem Blatt   : "
  f"{math.hypot(p30[0]-pt[0], p30[1]-pt[1]):.1f} px  (dx={p30[0]-pt[0]}, dy={p30[1]-pt[1]})")

P("\n############ 3. GLYPHEN auf Seite 4 ############")
for k in sorted([k for k in GLYPHEN if isinstance(k,tuple) and k[0]==4]):
    P(f"  Rect {k[1]}: " + "; ".join(f"({g[0]},{g[1]}) {g[2]}x{g[3]} wand={g[4]}" for g in GLYPHEN[k]))

P("\n############ 4. RECHTECK-UEBERLAPP Seite 4 ############")
Rs = rects(4)
for i in range(len(Rs)):
    for j in range(i+1,len(Rs)):
        A,B = Rs[i],Rs[j]
        ox0=max(A[0],B[0]); ox1=min(A[0]+A[2],B[0]+B[2])
        oy0=max(A[1],B[1]); oy1=min(A[1]+A[3],B[1]+B[3])
        if ox1>ox0 and oy1>oy0:
            P(f"  Rect {i} x Rect {j}: {ox1-ox0}x{oy1-oy0} px bei x[{ox0}..{ox1-1}] y[{oy0}..{oy1-1}]")
