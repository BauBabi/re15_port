#!/usr/bin/env python3
"""jal_scan.py <ziel_hex> [...] — sucht 'jal ziel' (und j ziel) als 32-bit-Wort in PSX.EXE und allen
STAGE*.BIN / DEBUG.BIN / TITLE.BIN (RE1.5 Auslieferungsstand info/Re1.5). Gibt RAM-Adresse aus.
EXE: RAM = t_addr + (off - 0x800); Overlays: RAM = 0x80100000 + off (kein Header); DEBUG.BIN @0x800c0000.
"""
import struct, sys, os, glob
ROOT = os.path.join(os.path.dirname(__file__), '..', '..', '..')
RE15 = os.path.join(ROOT, 'info', 'Re1.5')
def words(path):
    d = open(path, 'rb').read()
    return d
def scan(ziel):
    jal = 0x0c000000 | ((ziel >> 2) & 0x03ffffff)
    j   = 0x08000000 | ((ziel >> 2) & 0x03ffffff)
    out = []
    exe = open(os.path.join(RE15, 'PSX.EXE'), 'rb').read()
    t_addr = struct.unpack_from('<I', exe, 0x18)[0]
    for off in range(0x800, len(exe) - 3, 4):
        w = struct.unpack_from('<I', exe, off)[0]
        if w in (jal, j):
            out.append(('PSX.EXE', t_addr + off - 0x800, 'jal' if w == jal else 'j'))
    for p in sorted(glob.glob(os.path.join(RE15, 'PSX', 'BIN', '*.BIN'))):
        name = os.path.basename(p)
        base = 0x800c0000 if name == 'DEBUG.BIN' else 0x80100000
        d = open(p, 'rb').read()
        for off in range(0, len(d) - 3, 4):
            w = struct.unpack_from('<I', d, off)[0]
            if w in (jal, j):
                out.append((name, base + off, 'jal' if w == jal else 'j'))
    return out
if __name__ == '__main__':
    for a in sys.argv[1:]:
        z = int(a, 16)
        r = scan(z)
        print(f'# Ziel 0x{z:08x}: {len(r)} Treffer')
        for n, ad, k in r:
            print(f'  {n:12s} 0x{ad:08x} {k}')
