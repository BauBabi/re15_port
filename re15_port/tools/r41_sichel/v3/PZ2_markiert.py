# Zahlen-Pruefer V3 r2: Marker-EMD fuer die Bildmessung im Spiellauf. Neue CLUT-Zeile (480 + Zeilenzahl, bei V3 487) = reines Gruen (0x03E0), die Schwanz-
# Meshes 16-22 zeigen darauf. Geometrie, Skelett, Keyframes bleiben byte-gleich -> gleiche Posen im deterministischen Lauf.
# python PZ2_markiert.py EIN.EMD AUS.EMD [MESHES, Standard 16-22]
import sys, struct
sys.path.insert(0, __import__('os').path.dirname(__file__))
from PZ_emd import sektionen, u32
A = open(sys.argv[1], 'rb').read()
s, do = sektionen(A)
B = bytearray(A)
m0 = s[7][0]; d = bytes(B[m0:s[7][1]])
ZEILE = 480 + struct.unpack_from('<4H', A, s[8][0] + 12)[3]   # neue Zeile direkt unter den vorhandenen
obj = u32(d, 8); n = 0
MESHES = range(16, obj // 2) if len(sys.argv) < 4 else [int(x) for x in sys.argv[3].split(',')]
for i in MESHES:
    t = struct.unpack_from('<14I', d, 12 + i * 56)
    tfc, tuo, qfc, quo = t[5], t[6], t[12], t[13]
    for k in range(tfc):
        o = m0 + 12 + tuo + k * 12 + 2
        c = struct.unpack_from('<H', B, o)[0]; struct.pack_into('<H', B, o, (ZEILE << 6) | (c & 0x3F)); n += 1
    for k in range(qfc):
        o = m0 + 12 + quo + k * 16 + 2
        c = struct.unpack_from('<H', B, o)[0]; struct.pack_into('<H', B, o, (ZEILE << 6) | (c & 0x3F)); n += 1
t0 = s[8][0]; o = t0 + 8
cs = u32(B, o); cx, cy, cw, ch = struct.unpack_from('<4H', B, o + 4)
assert cy == 480, cy
ende = o + cs
gruen = struct.pack('<H', 0x03E0) * cw
B = B[:ende] + bytearray(gruen) + B[ende:]
struct.pack_into('<I', B, o, cs + len(gruen)); struct.pack_into('<4H', B, o + 4, cx, cy, cw, ch + 1)
struct.pack_into('<I', B, 0, do + len(gruen))
open(sys.argv[2], 'wb').write(B)
print('Flaechen umgehaengt', n, 'Verzeichnis', do, '->', do + len(gruen))
