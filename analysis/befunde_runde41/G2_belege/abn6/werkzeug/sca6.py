"""sca6.py <ROOM.RDT>: SCA-Zellen (Kollision) selbst geparst - Zeiger @RDT+0x20, Kopf 24 B (ceilingX/Z s16, 5 x u32 Anzahl je Quadrant), Zellen je 12 B
(width u16, density u16, x s16, z s16, type u8, u0 u8, u1 u8, floor u8; Band = floor >> 4). Abnahme 6: Belege der Raumhalt-Grenzen (Aussenwaende, Rampe,
Plattform, Leisten)."""
import struct, sys
d = open(sys.argv[1], "rb").read()
s = struct.unpack_from("<I", d, 0x20)[0]
cx, cz = struct.unpack_from("<hh", d, s)
cnt = struct.unpack_from("<5I", d, s + 4)
print("SCA @0x%x ceiling (%d,%d) Zellen je Quadrant %s" % (s, cx, cz, cnt))
off = s + 24
for k in range(sum(cnt)):
    w, dn, x, z, t, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, off)
    print("#%2d @0x%x x %6d..%6d z %6d..%6d type %d u0 %02x u1 %02x floor %02x band %d" % (k, off, x, x + w, z, z + dn, t, u0, u1, fl, fl >> 4))
    off += 12
