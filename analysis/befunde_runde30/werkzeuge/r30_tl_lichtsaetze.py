# -*- coding: utf-8 -*-
"""Runde 30 Nachschliff tischlicht: alle Lichtsaetze von ROOM1150/ROOM1151 aus der RDT.

RDT-Kopf @0x2C = lightStart (RE15_KNOWLEDGE.md 1.1), 40 Byte je Cut, Anzahl = nCut (Byte 1),
Feldlage wie re15_light_parse (engine/src/light_common.c): +0 scale, +1..3 Typ je Licht,
+4/+7/+10 Farbe Licht 0/1/2, +13 ambient, +16/+22/+28 Position (s16 x3), +34/+36/+38 Helligkeit.

    python r30_tl_lichtsaetze.py
"""
import os
import struct

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))

for r in ("ROOM1150", "ROOM1151"):
    d = open(os.path.join(REPO, f"re15_port/shared_assets/PSX/STAGE1/{r}.RDT"), "rb").read()
    ncut = d[1]
    ls = struct.unpack_from("<I", d, 0x2C)[0]
    print(f"{r}: nCut={ncut} lightStart=0x{ls:X} (Header @0x2C: {d[0x2C:0x30].hex(' ')})")
    for c in range(ncut):
        o = ls + c*40
        p = d[o:o+40]
        sc, t0, t1, t2 = p[0], p[1], p[2], p[3]
        cols = [tuple(p[4+i*3:7+i*3]) for i in range(3)]
        amb = tuple(p[0x0D:0x10])
        pos = [struct.unpack_from("<hhh", p, 0x10+i*6) for i in range(3)]
        br = struct.unpack_from("<HHH", p, 0x22)
        print(f"  cut {c:2d} @0x{o:05X}: scale={sc} types=({t0},{t1},{t2}) amb={amb} cols={cols} pos={pos} bright={br}")
        print(f"        bytes: {p.hex(' ')}")

# Beide Varianten (John 1150 / Elza 1151): ist der Lichtblock byte-gleich?
_d = [open(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE1/%s.RDT" % r), "rb").read()
      for r in ("ROOM1150", "ROOM1151")]
_ls = [struct.unpack_from("<I", d, 0x2C)[0] for d in _d]
_n = [d[1] for d in _d]
print("Lichtblock ROOM1150 @0x%X..0x%X gegen ROOM1151 @0x%X..0x%X: %s"
      % (_ls[0], _ls[0] + 40 * _n[0], _ls[1], _ls[1] + 40 * _n[1],
         "BYTE-GLEICH" if _d[0][_ls[0]:_ls[0] + 40 * _n[0]] == _d[1][_ls[1]:_ls[1] + 40 * _n[1]] else "VERSCHIEDEN"))
