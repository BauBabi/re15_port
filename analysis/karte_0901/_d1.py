# -*- coding: utf-8 -*-
import sys, os, math, collections, itertools
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from d2f import *

print("== Seite-3-Raeume nach dem Seiten-Setzer ==")
for r in range(0x1000, 0x1300, 0x10):
    pg = page_of(r)
    if pg == 3:
        room = (r>>4)&0xFF
        print(f"  ROOM{r:04X}  room-index {room:2d}  page {pg}")
print()
print("== rects(3) ==")
for i,R in enumerate(rects(3)):
    print(f"  rect {i}: x={R[0]:3d} y={R[1]:3d} w={R[2]:3d} h={R[3]:3d}  uv={rect_uv(3,i)}")
print()
print("== ZONES-Header, Eintraege mit page==3 ==")
for k,z in sorted(ZONES.items()):
    if z['page']==3:
        print(f"  ROOM{z['room']:04X} z{z['zi']} -> rect {z['rect']:2d} zid={z['zid']:3d} "
              f"bb=({z['x0']},{z['x1']},{z['z0']},{z['z1']}) eich=({z['ox']},{z['oy']},{z['sx']},{z['sy']})")
print()
print("== ALLE Zonen der Seite-3-Raeume (auch wenn auf anderer Seite/unzugeordnet) ==")
for r in range(0x1000, 0x1300, 0x10):
    if page_of(r)!=3: continue
    g = rdt(r)
    if not g:
        print(f"  ROOM{r:04X}: KEIN RDT"); continue
    zs = zones_of(g[0])
    print(f"  ROOM{r:04X}: {len(g[0])} SCA-Zellen, {len(zs)} Zonen, {len(g[1])} Tueren, {len(g[2])} Treppen")
    for i,bb in enumerate(zs):
        got = ZONES.get((r,i))
        tgt = f"S{got['page']}/R{got['rect']}" if got else "-- KEINE ZUORDNUNG --"
        w = bb[1]-bb[0]; d = bb[3]-bb[2]
        print(f"      z{i}: bb x[{bb[0]:7d},{bb[1]:7d}] z[{bb[2]:7d},{bb[3]:7d}]  {w}x{d}  asp={w/float(d):.3f}  -> {tgt}")
