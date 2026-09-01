# -*- coding: utf-8 -*-
import sys, os, math, collections, itertools
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from d2f import *

P3 = [0x10C0,0x10D0,0x10E0,0x10F0,0x1100,0x1110]
print("=== ALLE ZONEN dieser Raeume (aus dem Header), egal welche Seite ===")
for r in P3:
    zs = [ZONES[k] for k in sorted(ZONES) if k[0]==r]
    gen = zonen(r)   # was der Generator aus dem RDT rechnet
    print("ROOM%04X: RDT-Zonen=%d, im Header=%d" % (r, len(gen), len(zs)))
    for i,bb in enumerate(gen):
        z = ZONES.get((r,i))
        w = bb[1]-bb[0]; d = bb[3]-bb[2]
        if z:
            print("   z%d bbox x[%7d..%7d] z[%7d..%7d]  %6dx%6d  -> Seite %d Rect %d  eich=%s" % (
                i, bb[0],bb[1],bb[2],bb[3], w,d, z['page'], z['rect'],
                (z['ox'],z['oy'],z['sx'],z['sy']) if z['sx'] else 'BBOX'))
        else:
            print("   z%d bbox x[%7d..%7d] z[%7d..%7d]  %6dx%6d  -> NICHT ZUGEORDNET" % (
                i, bb[0],bb[1],bb[2],bb[3], w,d))
