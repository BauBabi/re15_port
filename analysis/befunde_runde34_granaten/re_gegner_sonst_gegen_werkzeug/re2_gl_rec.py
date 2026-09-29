#!/usr/bin/env python3
"""Gegenpruefung R34: RE2-Schadensrecords je Typ ueber die Zeigertabelle 0x800a6a88 (Leser @0x8004722c:
`lw a1,27272(at)` mit at = 0x800a0000 + Typ*4) und Zeile*20-20 (@0x80047230..40). Wort0 -> K0/K1/K2 je 10 Bit
(@0x80047244..5c), Wort1 -> Sperre (w1>>9)&0x7f (@0x80047588..9c). Datei-Offsets ueber load() von re2_disasm.py."""
import sys, os, struct, importlib.util
spec = importlib.util.spec_from_file_location("d", r"C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts\re2_disasm.py")
d = importlib.util.module_from_spec(spec); sys.argv = ["x"]; spec.loader.exec_module(d)
data, fo, path = d.load(0x800a6a88, None)
def w(a): return struct.unpack_from("<I", data, fo(a))[0]
names = {0x23:"Alligator em23", 0x29:"Kakerlake em29", 0x2d:"Zellenarm em2d", 0x2e:"Ivy em2e", 0x30:"G1 em30", 0x36:"G5 em36", 0x37:"Tentakel em37", 0x10:"Zombie em10", 0x2a:"em2a", 0x2b:"em2b"}
print("; " + path)
for t in sorted(names):
    ptr = w(0x800a6a88 + t*4)
    print("Typ 0x%02x %-15s Tabelle *(0x%08x) = 0x%08x" % (t, names[t], 0x800a6a88 + t*4, ptr))
    if not (0x80010000 <= ptr < 0x80200000): continue
    for row in (9, 10, 11):
        ra = ptr + row*20 - 20
        w0 = w(ra); w1 = w(ra+4)
        k = [(w0 >> (i*10)) & 0x3ff for i in range(3)]
        raw = data[fo(ra):fo(ra)+8].hex(' ')
        print("   Zeile %2d @0x%08x  w0=%08x w1=%08x  K0/K1/K2 = %d/%d/%d  Sperre=(w1>>9)&0x7f=%d   bytes %s" % (row, ra, w0, w1, k[0], k[1], k[2], (w1 >> 9) & 0x7f, raw))
