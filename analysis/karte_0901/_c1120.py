# -*- coding: utf-8 -*-
import sys, os, math, itertools
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *

def P(*a): print(*a)

P("=== ZONEN im generierten Header ===")
for k in sorted(ZONES):
    if k[0] in (0x1120, 0x1130, 0x1150, 0x1160, 0x1140, 0x1170, 0x1060):
        z = ZONES[k]
        P(f"  room {k[0]:04X} z{k[1]}  bbox x[{z['x0']:7d}..{z['x1']:7d}] z[{z['z0']:7d}..{z['z1']:7d}]"
          f"  -> Seite {z['page']} Rect {z['rect']}  zid={z['zid']}  Zeile ox={z['ox']} oy={z['oy']} sx={z['sx']} sy={z['sy']}")

P()
P("=== RECHTECKE Seite 4 ===")
for i, R in enumerate(rects(4)):
    P(f"  Rect {i}: x={R[0]:4d} y={R[1]:4d} w={R[2]:3d} h={R[3]:3d}  -> x[{R[0]}..{R[0]+R[2]-1}] y[{R[1]}..{R[1]+R[3]-1}]  uv={rect_uv(4,i)}")

P()
P("=== TUEREN ROOM1130 ===")
for i, d in enumerate(doors_of(0x1130)):
    P(f"  [{i}] lx={d['lx']:7d} lz={d['lz']:7d}  rect=({d['rx']},{d['rz']}) {d['rw']}x{d['rd']}  band={d['band']:2d}"
      f"  n=({d['nx']:7d},{d['nz']:7d})  dest={d['dest']:04X}")
P()
P("=== TUEREN ROOM1120 ===")
for i, d in enumerate(doors_of(0x1120)):
    P(f"  [{i}] lx={d['lx']:7d} lz={d['lz']:7d}  rect=({d['rx']},{d['rz']}) {d['rw']}x{d['rd']}  band={d['band']:2d}"
      f"  n=({d['nx']:7d},{d['nz']:7d})  dest={d['dest']:04X}")
P()
P("=== TREPPEN 1120 / 1130 ===")
for r in (0x1120, 0x1130):
    for s in RDT[r][2]:
        P(f"  {r:04X}: x={s['x']} z={s['z']} axis={s['axis']} band={s['band']} count={s['count']}")
