#!/usr/bin/env python3
"""SCA-Zellen eines RDT (Runde 36 Spur R). Layout wie rdt_common.c: Block @RDT+0x20, Kopf 24 B
(ceilingX u16, ceilingZ u16, count[5] u32), dann 12-B-Zellen: width u16, density u16, x s16, z s16,
type u8, u0 u8, u1 u8, floor u8. Typwort (FUN_8003b7f0 `lhu 0(a1)` = Zelle+10) = u1 | floor<<8;
Band = Nibble floor>>4 (`sll 16 / sra 28` @0x8003b8a8-ac). Absprung (Objekt-Pass @0x8002c130) = Typwort & 2
= u1 & 2. Schubsperre Klasse 2 (FUN_8003b558 @0x8003b64c-660) = u0 & 2.
Aufruf: sca_dump.py <RDT> [band]"""
import struct, sys
d = open(sys.argv[1], "rb").read()
want = int(sys.argv[2]) if len(sys.argv) > 2 else None
sca = struct.unpack_from("<I", d, 0x20)[0]
cx, cz = struct.unpack_from("<HH", d, sca)
cnt = struct.unpack_from("<5I", d, sca + 4)
print("ceiling", (struct.unpack_from("<h", d, sca)[0], struct.unpack_from("<h", d, sca + 2)[0]), "counts", cnt)
off = sca + 24
k = 0
for q in range(5):
    for i in range(cnt[q]):
        w, dd, x, z, t, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, off)
        band = fl >> 4
        word = u1 | (fl << 8)
        tag = ("JUMP " if u1 & 2 else "     ") + ("BLK2 " if u0 & 2 else "     ") + ("BLK1 " if u0 & 1 else "     ")
        if want is None or band == want:
            print("r%d[%2d] @0x%05X x %6d..%6d z %6d..%6d typ %d u0 0x%02x u1 0x%02x fl 0x%02x band %d wort 0x%04x %s" % (
                q, i, off, x, x + w, z, z + dd, t, u0, u1, fl, band, word, tag))
        off += 12
        k += 1
