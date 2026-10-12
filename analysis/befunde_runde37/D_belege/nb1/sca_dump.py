# SCA-Zellen eines RDT (Layout re15_rdt.h re15_sca_entry_t, Kopf an RDT+0x20: ceilX u16, ceilZ u16, count[5] u32).
import struct, sys
path = sys.argv[1]; bands = [int(b) for b in sys.argv[2].split(",")]; x0, x1, z0, z1 = (int(v) for v in sys.argv[3:7])
d = open(path, "rb").read()
s = struct.unpack_from("<I", d, 0x20)[0]
cx, cz = struct.unpack_from("<HH", d, s)
cnt = struct.unpack_from("<5I", d, s + 4)
off = s + 24; idx = 0
print("SCA @0x%X ceil=(%d,%d) counts=%s" % (s, cx, cz, cnt))
for g, c in enumerate(cnt):
    for k in range(c):
        w, dn, x, z, t, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, off)
        if (fl >> 4) in bands and x < x1 and x + w > x0 and z < z1 and z + dn > z0:
            print("r%d[%d] @0x%05X band %d x %d..%d z %d..%d type %d u0 0x%02x u1 0x%02x fl 0x%02x" % (g, k, off, fl >> 4, x, x + w, z, z + dn, t, u0, u1, fl))
        off += 12
