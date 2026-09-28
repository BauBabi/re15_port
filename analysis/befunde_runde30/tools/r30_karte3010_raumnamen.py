#!/usr/bin/env python3
"""r30_karte3010_raumnamen.py - RE1.5-Raumnamentabelle aus DEBUG.BIN lesen.

Tabelle: DEBUG.BIN Datei-Offset 0x263A (= 0x800C263A; der Code adressiert die
Felder ueber die Basen 0x800C263C/3E/40/42), 6 Stages x 49 Saetze x 26 Byte.
Satzindex = (637*stage + 13*raum)*2 Byte  (@0x8001d39c-3c8).
Satz: +0 u16 belegt, +2 s16 X, +4 s16 Z, +6 u8 Band, +8 char[..] Name (NUL-terminiert).
Aufruf: r30_karte3010_raumnamen.py [Suchwort]
"""
import struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
P = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "DEBUG.BIN")
d = open(P, "rb").read()
BASE = 0x263A
pat = sys.argv[1].upper() if len(sys.argv) > 1 else None
for st in range(6):
    for r in range(49):
        off = BASE + (637 * st + 13 * r) * 2
        used = struct.unpack_from("<H", d, off)[0]
        if not used: continue
        x, z = struct.unpack_from("<hh", d, off + 2)
        band = d[off + 6]
        name = d[off + 8:off + 26].split(b"\0")[0].decode("ascii", "replace")
        if pat and pat not in name.upper(): continue
        print("Stage %d Raum 0x%02X = ROOM%X%02X0  @Datei 0x%05X  belegt=0x%04X X=%d Z=%d Band=%d  \"%s\"" % (
            st + 1, r, st + 1, r, off, used, x, z, band, name))
