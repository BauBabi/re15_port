#!/usr/bin/env python3
"""records.py - Runde 34 Gegenpruefung: RE2-Schadens-Records 0x800A6A88[Typ] + (Zeile-1)*20.
Liest ueber load()/word() von re2_disasm.py (Offsets NIE selbst gerechnet).
Ausgabe je Typ und Zeile: Adresse, Rohbytes w0/w1, drei 10-Bit-Klammern, Sperre (w1>>9)&0x7f, Reserve-Bits."""
import sys, importlib.util, os, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
spec = importlib.util.spec_from_file_location("d", os.path.join(REPO, ".claude/skills/re15-psx-disasm/scripts/re2_disasm.py"))
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)
data, fo, path = d.load(0x800a6a88, None)
types = [int(x, 16) for x in sys.argv[1].split(",")] if len(sys.argv) > 1 else [0x10,0x11,0x13,0x16,0x20,0x21,0x25,0x26,0x2d]
rows = [int(x) for x in sys.argv[2].split(",")] if len(sys.argv) > 2 else [9,10,11]
for t in types:
    base = d.word(data, fo, 0x800a6a88 + 4*t)
    for r in rows:
        a = base + (r-1)*20
        w0 = d.word(data, fo, a); w1 = d.word(data, fo, a+4)
        b = bytes(data[fo(a):fo(a)+8])
        k = [(w0 >> (10*i)) & 0x3ff for i in range(3)]
        print("typ %02x ptr@%08x base %08x zeile %2d @%08x  bytes %s  w0=%08x w1=%08x  K0/K1/K2=%d/%d/%d  sperre=%d  w1&7=%d (w1>>3)&7=%d (w1>>6)&7=%d" % (
            t, 0x800a6a88+4*t, base, r, a, b.hex(" "), w0, w1, k[0], k[1], k[2], (w1>>9)&0x7f, w1&7, (w1>>3)&7, (w1>>6)&7))
