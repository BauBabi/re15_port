#!/usr/bin/env python3
"""Ein 32-Bit-Wort in der Binaerdatei suchen (Zeiger-/Dispatch-Tabellen finden).
   usage: scan_word.py <bin> <rambase-hex> <skip-hex> <wort-hex>
"""
import sys, struct

path, base, skip = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
val = int(sys.argv[4], 16)
d = open(path, 'rb').read()[skip:]
n = 0
for i in range(0, len(d) - 3, 4):
    if struct.unpack_from('<I', d, i)[0] == val:
        print("%08x" % (base + i))
        n += 1
print("-- %d Vorkommen von 0x%08x" % (n, val))
