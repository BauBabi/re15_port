# SCA-Zellen eines Raums (RDT+0x20, rdt_common.c: 24-B-Kopf, 12-B-Eintraege
# {width u16, density u16, x s16, z s16, type u8, u0 u8, u1 u8, floor u8});
# Typwort w = u1 | floor<<8 (re15_collision_floor_typeword, @0x8003b8a8-ac):
# Band = w>>12, Bit 0 = hinaufklettern, Bit 1 = hinabspringen (FUN_8002d474 @0x8002d74c-50).
import sys, struct
d = open(sys.argv[1], "rb").read()
s = struct.unpack_from("<I", d, 0x20)[0]
cx, cz = struct.unpack_from("<HH", d, s)
cnt = struct.unpack_from("<5I", d, s + 4)
print("SCA @0x%X ceiling=(%d,%d) Quadranten=%s" % (s, cx, cz, cnt))
i = 0; q = 0; qi = 0
for g, c in enumerate(cnt):
    for k in range(c):
        o = s + 24 + 12 * i
        w_, dn, x, z = struct.unpack_from("<HHhh", d, o)
        ty, u0, u1, fl = d[o+8], d[o+9], d[o+10], d[o+11]
        w = u1 | (fl << 8)
        band = w >> 12
        flt = sys.argv[2] if len(sys.argv) > 2 else "all"
        if flt == "all" or (flt == "drop" and (w & 2)) or (flt == "climb" and (w & 1)) or (flt.startswith("b") and band == int(flt[1:])):
            print("@0x%05X q%d i%-3d x[%6d..%6d] z[%6d..%6d] type=%d u0=0x%02X u1=0x%02X floor=0x%02X w=0x%04X band=%d%s%s"
                  % (o, g, k, x, x + w_, z, z + dn, ty, u0, u1, fl, w, band, " DROP" if w & 2 else "", " CLIMB" if w & 1 else ""))
        i += 1
