#!/usr/bin/env python
"""Rechteck-Tabellen der Kartenseiten aus PSX.EXE lesen (Paar-Tabelle @0x80076840:
{count, list ptr} je Seite; Eintrag 12 B {x,y,w,h,u,v} - FUN_80046fd8 @0x80047048-70,
@0x8004731c-60). Aufruf: python r30_rects.py <seite> [...]"""
import struct, sys
EXE = open('info/Re1.5/PSX.EXE', 'rb').read()
def fo(a): return a - 0x80010000 + 0x800
for pg in map(int, sys.argv[1:]):
    cnt, ptr = struct.unpack_from('<II', EXE, fo(0x80076840 + 8 * pg))
    print("Seite %d: count %d, Liste @0x%08X" % (pg, cnt, ptr))
    for i in range(cnt):
        x, y, w, h, u, v = struct.unpack_from('<hhhhhh', EXE, fo(ptr + 12 * i))
        print("   rect %2d @0x%08X: (%3d,%3d) %3dx%-3d  x %d..%d  y %d..%d   uv(%d,%d)"
              % (i, ptr + 12 * i, x, y, w, h, x, x + w - 1, y, y + h - 1, u, v))
