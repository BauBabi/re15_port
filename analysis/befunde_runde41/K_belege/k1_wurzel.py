#!/usr/bin/env python3
"""Spur K (Runde 41): Wurzel-Lage (Keyframe-Bytes 0..5 = x,y,z s16, emd_common.c re15_emd_get_keyframe_pos) je Bank:
ROOM3091-RBJ (RDT +0x5C) rec0..2, 0x42-CDEMD1 dir2/dir4, 0x4B dir2/dir4, 0x49 dir2. Zeigt, ob die Wurzelhoehe in den
Keyframes steckt (dann schwebt/sinkt ein anderes Skelett) oder im Skelett.
"""
import os, sys, struct, statistics, importlib.util
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, HIER)
spec = importlib.util.spec_from_file_location('kb', os.path.join(HIER, 'k1_baenke.py'))
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
s16 = lambda b, o: struct.unpack_from('<h', b, o)[0]
exe = open(os.path.join(REPO, 'info', 'Re1.5', 'PSX.EXE'), 'rb').read()
def blob(arch, typ):
    a = (0x80072f38 if arch == 0 else 0x80073178) + (typ - 0x10) * 8
    s, l = struct.unpack_from('<II', exe, a - 0x80010000 + 0x800)
    d = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'EMD', 'CDEMD%d.EMS' % arch), 'rb').read()
    return d[s*2048:s*2048+l]
def emd_dir(b): return [u32(b, u32(b, 0) + i*4) for i in range(u32(b, 4))]
def kf_stats(emr, name):
    o_arm, o_frm, cnt, ksz = u16(emr, 0), u16(emr, 2), u16(emr, 4), u16(emr, 6)
    n = (len(emr) - o_frm) // ksz if ksz else 0
    ys = [s16(emr, o_frm + k*ksz + 2) for k in range(n)]
    xs = [s16(emr, o_frm + k*ksz + 0) for k in range(n)]
    print('%-28s Knochen %2d ksize %3d kf %4d  Wurzel-y min %6d med %6d max %6d | x med %5d' %
          (name, cnt, ksz, n, min(ys), int(statistics.median(ys)), max(ys), int(statistics.median(xs))))
    return ys
rdt = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE3', 'ROOM3091.RDT'), 'rb').read()
s = u32(rdt, 0x5C); total = u32(rdt, s); nrec = rdt[s+4]
print('ROOM3091 RBJ @0x%X total %d nrec %d' % (s, total, nrec))
for r in range(nrec):
    pre = u32(rdt, s + total + r*8); edd = u32(rdt, s + total + r*8 + 4)
    marker = u32(rdt, s + pre)
    emr = rdt[s + pre + 4: s + edd]
    nclip = u16(rdt, s + edd + 2) // 4
    kf_stats(emr, 'RBJ rec%d Marker 0x%x Clips %d' % (r, marker, nclip))
for arch, typ in ((1, 0x42), (0, 0x4B), (1, 0x49)):
    b = blob(arch, typ); d = emd_dir(b)
    kf_stats(b[d[2]:d[3]], 'CDEMD%d 0x%02X dir2 (Bank0)' % (arch, typ))
    kf_stats(b[d[4]:d[5]], 'CDEMD%d 0x%02X dir4 (eigene)' % (arch, typ))
