"""wa_esp.py - Abnahme W1: ESP-Bank eines RDT (RDT+0x4C/0x50, Parser wie engine/src/re15_esp.c esp_parse_core),
Row-Block eines Effekts/Subs (Layout re15_esp_row_stream: rowblk=eff_end, 8xu16 Sub-Offsets *4, u16 streams, je Stream
u16 nrows + nrows x 40 B). Aufruf: python wa_esp.py <RDT> <effekt_id> <sub...>"""
import struct, sys
fn = sys.argv[1]; eid = int(sys.argv[2], 0); subs = [int(s) for s in sys.argv[3:]]
d = open(fn, "rb").read()
idh, pe = struct.unpack_from("<II", d, 0x4C)
ids = []
for i in range(32):
    if d[idh + i] == 0xFF: break
    ids.append(d[idh + i])
print(fn, "ESP idh=0x%X pe=0x%X ids=%s" % (idh, pe, " ".join("%02x" % x for x in ids)))
i = ids.index(eid)
entry = struct.unpack_from("<i", d, pe - 4 * i)[0]
es = idh + entry
ca, cb = struct.unpack_from("<HH", d, es)
ee = es + (ca * 2 + cb + 2) * 4
print("effekt 0x%02X body @0x%X count_a=%d count_b=%d rowblk @0x%X subtab=%s" % (eid, es, ca, cb, ee, list(struct.unpack_from("<8H", d, ee))))
for sub in subs:
    so = struct.unpack_from("<H", d, ee + (sub & 7) * 2)[0] * 4
    base = ee + so
    streams = struct.unpack_from("<H", d, base)[0]
    print(" sub %d base @0x%X streams=%d" % (sub, base, streams))
    p = base + 4
    for s in range(min(streams, 8)):
        nr = struct.unpack_from("<H", d, p)[0]
        print("  stream %d nrows=%d @0x%X" % (s, nr, p))
        for r in range(min(nr, 12)):
            row = d[p + 4 + r * 40: p + 4 + (r + 1) * 40]
            A, B, w, h, ax, ay, az, prm0e, vx, vy, vz, p16, avx, avy, avz, p1e, ex, ey, ez, gate = struct.unpack("<HHhhhhhHhhhHhhhHhhhH", row)
            print("   row%d A=%d B=%d wh=(%d,%d) accel=(%d,%d,%d) +0e=0x%04X vel=(%d,%d,%d) +16=0x%04X angvel=(%d,%d,%d) +1e=0x%04X euler=(%d,%d,%d) gate=%d" % (r, A, B, w, h, ax, ay, az, prm0e, vx, vy, vz, p16, avx, avy, avz, p1e, ex, ey, ez, gate))
        p += 4 + nr * 40
