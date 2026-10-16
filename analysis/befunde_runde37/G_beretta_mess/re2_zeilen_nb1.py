# Nachbesserung 1 Spur G: RE2-Retail-Zeilen 3 (Handgun) und 4 (Custom Handgun) je Gegnertyp aus info/re2leon/PSX.EXE.
# Zeile r = *(0x800A6A88 + Typ*4) + (r-1)*0x14; w0 = Schaden je Klammer (3 x 10 Bit), (w1 >> 9) & 0x7F = Trefferpause
# (Stempel @0x80047338-4C). Aufruf: python re2_zeilen_nb1.py <PSX.EXE>
import struct, sys
exe = open(sys.argv[1], 'rb').read()
t_addr = struct.unpack_from('<I', exe, 0x18)[0]
def rd(a):
    return struct.unpack_from('<I', exe, a - t_addr + 0x800)[0]
for t in (0x10, 0x20, 0x21, 0x23, 0x25, 0x27, 0x29, 0x2b, 0x30, 0x36):
    base = rd(0x800A6A88 + t * 4)
    out = []
    for r in (3, 4):
        a = base + (r - 1) * 0x14
        w0 = rd(a); w1 = rd(a + 4)
        out.append("Z%d @0x%08X w0=%08X w1=%08X dmg=%d/%d/%d pause=%d" % (
            r, a, w0, w1, w0 & 0x3ff, (w0 >> 10) & 0x3ff, (w0 >> 20) & 0x3ff, (w1 >> 9) & 0x7f))
    print("typ 0x%02x tab@0x%08X->0x%08X | %s" % (t, 0x800A6A88 + t * 4, base, " | ".join(out)))
