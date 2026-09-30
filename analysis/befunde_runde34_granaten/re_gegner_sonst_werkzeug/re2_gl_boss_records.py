#!/usr/bin/env python3
"""re2_gl_boss_records.py — RE2-GL-Schadensrecords (Zeile 9 Explosiv / 10 Brand / 11 Saeure) fuer die
Boss-/Sonder-Pendants. Basis = *(0x800A6A88 + Typ*4) (info/re2leon/PSX.EXE), Record = Basis + (Zeile-1)*20,
w0 = drei 10-Bit-Schaeden (Klammer 0/1/2), Sperre = (w1 >> 9) & 0x7F. Unabhaengige Gegenprobe zu
re_gegner_re2_werkzeug/re2_gl_records.py der Schwester-Spur."""
import struct, os
root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
exe = open(os.path.join(root, 'info', 're2leon', 'PSX.EXE'), 'rb').read()
t = struct.unpack_from('<I', exe, 0x18)[0]
def w(a): return struct.unpack_from('<I', exe, 0x800 + a - t)[0]
TYPEN = [(0x23, 'Alligator'), (0x29, 'Kakerlake'), (0x2A, 'Mr.X'), (0x2B, 'Tyrant'), (0x2D, 'Zellenarm'),
         (0x2E, 'Ivy'), (0x30, 'G1'), (0x31, 'G2'), (0x33, 'G3'), (0x34, 'G4'), (0x36, 'G5'), (0x27, 'G-Junges'), (0x22, 'Licker rot'), (0x24, 'Licker grau')]
for typ, name in TYPEN:
    zp = 0x800A6A88 + typ * 4
    base = w(zp)
    for row in (9, 10, 11):
        a = base + (row - 1) * 20
        w0, w1 = w(a), w(a + 4)
        k = [(w0 >> (10 * i)) & 0x3ff for i in range(3)]
        print(f'0x{typ:02x} {name:11s} (Zeiger @0x{zp:08x} -> 0x{base:08x}) Zeile {row:2d} @0x{a:08x} '
              f'K0/K1/K2 = {k[0]}/{k[1]}/{k[2]}  Sperre {(w1 >> 9) & 0x7f}  Bytes {exe[0x800+a-t:0x800+a-t+8].hex(" ")}')
