#!/usr/bin/env python3
"""EMR-Hierarchie + Bind-Offsets dumpen. Layout wie der Port-Parser
   (re15_port/engine/src/emd_common.c:143-183):
     u16 bones_table_off, u16 keyframes_off, u16 bone_count, u16 keyframe_size
     @8            bone_count x 3 x s16   Bind-Offset (relativ zum Elternknochen)
     @bones_table  bone_count x { u16 child_count, u16 child_list_off }
                   child_list_off ist relativ zu bones_table.
   usage: emr_hier.py <datei.EMR>
"""
import sys, struct

d = open(sys.argv[1], 'rb').read()
bt, kf_off, nb, kf_sz = struct.unpack_from('<4H', d, 0)
print("bones_table=0x%x keyframes=0x%x bone_count=%d keyframe_size=%d  (Datei %d B)"
      % (bt, kf_off, nb, kf_sz, len(d)))
parent = [-1] * nb
bind = []
for i in range(nb):
    bind.append(struct.unpack_from('<3h', d, 8 + i * 6))
for i in range(nb):
    cnt, coff = struct.unpack_from('<2H', d, bt + i * 4)
    kids = [d[bt + coff + k] for k in range(cnt)]
    for k in kids:
        if k < nb:
            parent[k] = i
    print("  Bone %2d  Kinder %-18s  bind(x,y,z)=(%6d,%6d,%6d)"
          % (i, str(kids), bind[i][0], bind[i][1], bind[i][2]))
print("\nEltern: %s" % {i: parent[i] for i in range(nb)})
for b in (11, 10, 9):
    if b < nb:
        ch, p = [b], parent[b]
        while p >= 0:
            ch.append(p)
            p = parent[p]
        print("Kette zu Bone %2d: %s" % (b, " <- ".join(str(c) for c in ch)))
