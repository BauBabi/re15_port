#!/usr/bin/env python3
# Spur M r36: Tueren/Treppen/SCA-Komponenten je Raum (nutzt read_rdt/zones_of aus gen_map_zones.py)
import sys, os
TOOLS = sys.argv[1]
sys.path.insert(0, TOOLS)
import gen_map_zones as G

for arg in sys.argv[2:]:
    rid = int(arg, 16)
    r = G.read_rdt(rid)
    if not r:
        print(f"ROOM{rid:04X}: fehlt"); continue
    sca, doors, stairs = r
    print(f"=== ROOM{rid:04X}  page_of={G.page_of(rid)}  sca={len(sca)}")
    bands = sorted(set(c[4] for c in sca))
    print("  SCA-Baender:", bands)
    for b in bands:
        cells = [c for c in sca if c[4] == b]
        xs=[c[0] for c in cells]; zs=[c[1] for c in cells]; xe=[c[0]+c[2] for c in cells]; ze=[c[1]+c[3] for c in cells]
        print(f"   band {b}: {len(cells)} Zellen  x {min(xs)}..{max(xe)}  z {min(zs)}..{max(ze)}")
    for zb in G.zones_of(sca):
        print("  Zone bbox x %d..%d z %d..%d" % zb)
    for d in doors:
        print("  Tuer slot %d sce %d band %d -> %04X  trig (%d,%d %dx%d) mitte (%d,%d)  spawn (%d,%d,%d) yaw %d" % (
            d['slot'], d['sce'], d['band'], d['dest'], d['rx'], d['rz'], d['rw'], d['rd'], d['lx'], d['lz'], d['nx'], d['ny'], d['nz'], d['yaw']))
    for s in stairs:
        print("  Treppe axis %d band %d count %d mitte (%d,%d)" % (s['axis'], s['band'], s['count'], s['x'], s['z']))
