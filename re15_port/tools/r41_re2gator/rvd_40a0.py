# rvd_40a0.py <ROOM40A0.RDT> : RVD-Kamerazonen (Offset-Tabelle [8]) + Punkt-Test wie re15_aot_point_in_quad (zyklisch)
import struct, sys
d = open(sys.argv[1], "rb").read()
offs = [struct.unpack_from('<I', d, 8 + 4 * i)[0] for i in range(23)]
p = offs[8]; zones = []
while struct.unpack_from('<I', d, p)[0] != 0xffffffff:
    be, nf, fc, tc = d[p], d[p+1], d[p+2], d[p+3]
    pts = struct.unpack_from('<8h', d, p + 4)
    zones.append((p, be, nf, fc, tc, pts)); p += 20
def inside(pts, x, z):
    xs, zs = pts[0::2], pts[1::2]; sign = 0
    for i in range(4):
        j = (i + 1) & 3
        cr = (xs[j]-xs[i])*(z-zs[i]) - (zs[j]-zs[i])*(x-xs[i])
        if cr == 0: continue
        s = 1 if cr > 0 else -1
        if sign == 0: sign = s
        elif sign != s: return False
    return True
print("RVD @0x%X, %d Saetze (20 B)" % (offs[8], len(zones)))
for name, (x, z) in {"Leon FRESSEN P0 (x_g+10643, -915) @0x8010106c-84": (-23078 + 10643, -915),
                     "Gator FRESSEN P0 (x=-23078 @0x80101048, z=-915)": (-23078, -915)}.items():
    hits = [(hex(zp), fc, tc) for (zp, be, nf, fc, tc, pts) in zones if inside(pts, x, z)]
    print("%-52s (%d,%d): Zonen (Datei-Offset, von Cut, nach Cut) = %s" % (name, x, z, hits))
