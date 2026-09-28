#!/usr/bin/env python3
"""SCA-Kollisionszellen von ROOM5080 roh auslesen (Layout = rdt_common.c:250-275 +
re15_rdt.h re15_sca_entry_t: RDT+0x20 -> Kopf 24 B (ceilX u16, ceilZ u16, count[5] u32),
dann sum(count) Eintraege zu 12 B: width u16, density u16, x s16, z s16, type u8, u0, u1, floor).
Nur Datei-Bytes, keine Deutung ausser der Ausgabe je Zelle; dazu der Querschnitt bei gegebenem x:
welche Zellen schneiden die Linie x = const (fuer den Gang zwischen Generator und Tuer)."""
import os, sys, struct
HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
raum = sys.argv[1] if len(sys.argv) > 1 else "ROOM5080"
p = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "STAGE" + raum[4], raum + ".RDT")
d = open(p, "rb").read()
sca = struct.unpack_from("<I", d, 0x20)[0]
cx, cz = struct.unpack_from("<HH", d, sca)
cnt = struct.unpack_from("<5I", d, sca + 4)
print(f"{raum} SCA @0x{sca:05X} ceil=({cx},{cz}) counts={cnt}")
zellen = []
o = sca + 24
for g, c in enumerate(cnt):
    for i in range(c):
        w, dd, x, z, ty, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, o)
        zellen.append((g, x, z, w, dd, ty, u0, u1, fl, o))
        o += 12
for (g, x, z, w, dd, ty, u0, u1, fl, off) in zellen:
    print(f"  g{g} @0x{off:05X} x {x:6d}..{x+w:6d}  z {z:6d}..{z+dd:6d}  type={ty} u0=0x{u0:02x} u1=0x{u1:02x} floor={fl}")
for xs in [int(a) for a in sys.argv[2:]]:
    print(f"# Querschnitt x={xs}: Zellen, die die Linie schneiden")
    for (g, x, z, w, dd, ty, u0, u1, fl, off) in zellen:
        if x <= xs <= x + w:
            print(f"    g{g} z {z:6d}..{z+dd:6d} type={ty} (x {x}..{x+w})")
