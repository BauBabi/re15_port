# -*- coding: utf-8 -*-
"""Runde 31 / H — Zensus der Ruhe-Signatur des Hebetischs ueber ALLE RDTs.
Signatur = ROOM1150.RDT @0x1010..@0x1047 (For-Setzen bis einschliesslich For-Abfahrt, 56 Byte).
Ruhe-Fenster = [Signatur+0x0A (Sleep 30 @0x101A), Signatur+0x32 (For @0x1042))."""
import glob, os
PSX = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'shared_assets', 'PSX')
d = open(os.path.join(PSX, 'STAGE1', 'ROOM1150.RDT'), 'rb').read()
sig = d[0x1010:0x1048]
print('Signatur (%d Byte) @0x1010: %s' % (len(sig), sig.hex(' ')))
assert sig[0x0A:0x0E] == bytes.fromhex('090a1e00') and sig[0x32:0x38] == bytes.fromhex('0d0004005a00')
n = 0
for p in sorted(glob.glob(os.path.join(PSX, 'STAGE*', 'ROOM*.RDT'))):
    b = open(p, 'rb').read(); n += 1
    i = b.find(sig); treffer = []
    while i >= 0:
        treffer.append(i); i = b.find(sig, i + 1)
    if treffer:
        print('%s: %s  Ruhe-Fenster %s' % (os.path.basename(p), ['@0x%04X' % t for t in treffer],
              ['[0x%04X,0x%04X)' % (t + 0x0A, t + 0x32) for t in treffer]))
print('%d RDTs durchsucht' % n)
