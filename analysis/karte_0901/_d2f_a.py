# -*- coding: utf-8 -*-
import sys, os, math, collections, itertools
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from d2f import *

print("=== SEITEN-SETZER: welche Raeume auf Seite 3? ===")
for r in range(0x1000, 0x1300, 0x10):
    pg = page_of(r)
    if pg == 3:
        print("  ROOM%04X  room-idx %2d  page %d" % (r, (r>>4)&0xFF, pg))

print()
print("=== RECHTECKE SEITE 3 (@0x80076840+3*8) ===")
R3 = rects(3)
for i, R in enumerate(R3):
    u,v = rect_uv(3, i)
    print("  rect %d: x=%3d y=%3d w=%2d h=%2d  uv=(%3d,%3d)  flaeche=%4d" % (i, R[0],R[1],R[2],R[3], u, v, R[2]*R[3]))

print()
print("=== ZONEN-HEADER: alles was auf Seite 3 zeigt ===")
for k in sorted(ZONES):
    z = ZONES[k]
    if z['page'] == 3:
        print("  ROOM%04X z%d -> rect %d  bbox x[%6d..%6d] z[%6d..%6d]  eich(ox=%d,oy=%d,sx=%d,sy=%d)" % (
            z['room'], z['zi'], z['rect'], z['x0'], z['x1'], z['z0'], z['z1'], z['ox'], z['oy'], z['sx'], z['sy']))
