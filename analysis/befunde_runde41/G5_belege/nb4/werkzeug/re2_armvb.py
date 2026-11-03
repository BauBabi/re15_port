# Nachbesserung 4 G5A: RE2-RAM Mesh-0-Vertexpuffer der Arme (Teil0+8 -> [0]), Lage relativ t0 im Teil-0-Rahmen (W0^T (v - t0))
import sys
from re2_ram import SS, SN
n = int(sys.argv[1]); nv = int(sys.argv[2]) if len(sys.argv) > 2 else 400
s = SS(SN % n)
for i in (1, 2, 3, 4):
    e = s.u32(0x800cfe1c + 4 * i); p = s.u32(e + 0x198)
    W = [s.s16(p + 0x48 + 2 * j) for j in range(9)]; t0 = (s.s32(p + 0x5c), s.s32(p + 0x60), s.s32(p + 0x64))
    obj = s.u32(p + 8); vb = s.u32(obj); sc = s.s16(p + 0x8c)
    loc = []
    for k in range(nv):
        v = (s.s16(vb + 8 * k), s.s16(vb + 8 * k + 2), s.s16(vb + 8 * k + 4))
        d = [v[j] - t0[j] for j in range(3)]
        l = [(W[0 + c] * d[0] + W[3 + c] * d[1] + W[6 + c] * d[2]) >> 12 for c in range(3)]   # W^T d
        loc.append(l)
    xs = [l[0] for l in loc]; ys = [l[1] for l in loc]; zs = [l[2] for l in loc]
    print("s%02d slot %d scale %d vb %08x: lokal x %d..%d  y %d..%d  z %d..%d  | gezeichnet x %d..%d" % (n, i, sc, vb, min(xs), max(xs), min(ys), max(ys), min(zs), max(zs),
          min(xs) * sc >> 12, max(xs) * sc >> 12))
