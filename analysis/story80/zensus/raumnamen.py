#!/usr/bin/env python3
"""Raumnamen-Tabelle der EXE: @0x8006ff36, 26 Byte je Raum (x s16, z s16, Kamera u16, Name 20 Zeichen),
49 Raeume je Stage (Schrittweite 637 Halbworte, @0x8001d39c-0x8001d3d8; analysis/story_begegnungen/pruefung/daten.md).
Aufruf: python raumnamen.py <PSX.EXE> -> JSON {'1':{'00':name,...},...} auf stdout."""
import sys, json, struct
d = open(sys.argv[1], 'rb').read()
BASE = 0x8006ff36
out = {}
for s in range(1, 7):
    for r in range(49):
        a = BASE + (s-1)*1274 + r*26
        o = a - 0x80010000 + 0x800
        name = d[o+6:o+26].split(b'\0')[0].decode('latin-1').strip()
        name = ''.join(c for c in name if 32 <= ord(c) < 127).strip()
        if name:
            out['%d%02X' % (s, r)] = name
print(json.dumps(out, indent=0, ensure_ascii=False))
