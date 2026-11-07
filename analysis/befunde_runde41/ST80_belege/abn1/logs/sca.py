# SCA-Zellen eines RDT (Header @0x20 -> sca_start; 24 B Kopf: ..., count[5] u32; dann 12-B-Eintraege
# width u16, density u16, x s16, z s16, type u8, u0 u8, u1 u8, floor u8) - re15_rdt.h / rdt-Lader. Nur lesen.
import sys, struct
d = open(sys.argv[1], "rb").read()
s = struct.unpack_from("<I", d, 0x20)[0]
cnt = struct.unpack_from("<5I", d, s + 4)
tot = sum(cnt)
print("sca_start", hex(s), "counts", cnt, "total", tot, "kopf", d[s:s+4].hex(" "))
xmin, xmax, zmin, zmax = [int(a) for a in sys.argv[2:6]] if len(sys.argv) > 5 else (-99999, 99999, -99999, 99999)
for i in range(tot):
    w, de, x, z, ty, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, s + 24 + 12 * i)
    if x + w < xmin or x > xmax or z + de < zmin or z > zmax: continue
    print("#%d typ=%d band=%d u0=%d u1=%d x[%d..%d] z[%d..%d]" % (i, ty, fl >> 4, u0, u1, x, x + w, z, z + de))
