#!/usr/bin/env python3
"""r30_hundtod_jalscan.py - listet alle jal-Aufrufe eines Ziels in einer Binaerdatei.
Aufruf: python r30_hundtod_jalscan.py <ziel-hex> <datei> <ladeadresse-hex> [kopf-bytes]
Nur Lesen."""
import struct, sys, os
def main():
    ziel = int(sys.argv[1], 16)
    path = sys.argv[2]
    base = int(sys.argv[3], 16)
    off0 = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0
    data = open(path, "rb").read()
    n = (len(data) - off0) // 4
    words = struct.unpack_from("<%dI" % n, data, off0)
    enc = (3 << 26) | ((ziel >> 2) & 0x3ffffff)
    hits = [base + i * 4 for i, w in enumerate(words) if w == enc]
    print("jal 0x%08x in %s: %d Aufrufe" % (ziel, os.path.basename(path), len(hits)))
    for h in hits:
        print("  0x%08x" % h)
main()
