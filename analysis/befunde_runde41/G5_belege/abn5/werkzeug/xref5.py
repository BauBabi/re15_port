# Abnahme 5: jal-/Wort-Xrefs auf eine Adresse in der RE2-EXE (oder einem Overlay mit Basis)
import sys, struct
p = sys.argv[1]; b = open(p, 'rb').read()
if b[:8] == b'PS-X EXE':
    base = struct.unpack_from('<I', b, 0x18)[0]; hdr = 0x800
else:
    base = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0x80100000; hdr = 0
ziel = int(sys.argv[2], 16)
jal = 0x0C000000 | ((ziel >> 2) & 0x03FFFFFF)
j = 0x08000000 | ((ziel >> 2) & 0x03FFFFFF)
for o in range(hdr, len(b) - 3, 4):
    w = struct.unpack_from('<I', b, o)[0]
    a = o - hdr + base
    if w == jal: print('jal  @%08x' % a)
    elif w == j: print('j    @%08x' % a)
    elif w == ziel: print('wort @%08x' % a)
# lui/addiu-Paare
hi = (ziel + 0x8000) >> 16; lo = ziel & 0xFFFF
for o in range(hdr, len(b) - 3, 4):
    w = struct.unpack_from('<I', b, o)[0]
    if (w >> 26) == 0x0F and (w & 0xFFFF) == hi:
        rt = (w >> 16) & 31
        for k in range(1, 6):
            if o + 4*k + 4 > len(b): break
            w2 = struct.unpack_from('<I', b, o + 4*k)[0]
            if (w2 >> 26) in (0x09, 0x0D) and (w2 & 0xFFFF) == lo and ((w2 >> 21) & 31) == rt:
                print('lui/addiu @%08x' % (o - hdr + base)); break
