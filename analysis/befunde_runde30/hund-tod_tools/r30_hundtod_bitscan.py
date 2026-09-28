#!/usr/bin/env python3
"""r30_hundtod_bitscan.py - sucht Masken-Instruktionen (andi/ori imm) in der Naehe eines
Zugriffs mit niedrigem Adress-Wort <lo> (ohne das lui zu verlangen).
Aufruf: python r30_hundtod_bitscan.py <lo-hex> <maske-hex> <datei> <basis-hex> [kopf-hex] [fenster]
Nur Lesen."""
import struct, sys, os
def main():
    lo = int(sys.argv[1], 16); mask = int(sys.argv[2], 16)
    path = sys.argv[3]; base = int(sys.argv[4], 16)
    off0 = int(sys.argv[5], 16) if len(sys.argv) > 5 else 0
    win = int(sys.argv[6]) if len(sys.argv) > 6 else 10
    data = open(path, "rb").read()
    n = (len(data) - off0) // 4
    w = struct.unpack_from("<%dI" % n, data, off0)
    hits = 0
    for i, x in enumerate(w):
        op = x >> 26; imm = x & 0xffff
        if op in (0x0c, 0x0d) and imm in (mask, (~mask) & 0xffff):
            for j in range(max(0, i - win), min(n, i + win)):
                y = w[j]
                if (y & 0xffff) == lo and (y >> 26) in (0x23, 0x2b, 0x09, 0x25, 0x29):
                    hits += 1
                    print("%s 0x%04x @0x%08x  (Zugriff @0x%08x)" % (
                        "andi" if op == 0x0c else "ori ", imm, base + i * 4, base + j * 4))
                    break
    print("# %s: %d Treffer fuer lo=0x%04x Maske=0x%x" % (os.path.basename(path), hits, lo, mask))
main()
