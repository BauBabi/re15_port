# Nachbesserung 4 G5A: RE2-RAM Teile-Strukturen (172 B) der Arme: Flags, lokale R/t, Weltlage, Eltern-Zeiger +0x74, Streckung +0x8c, +0x94
import sys
from re2_ram import SS, SN
n = int(sys.argv[1]); slots = [int(a) for a in sys.argv[2:]] or [1, 2, 3, 4]
s = SS(SN % n)
for i in slots:
    e = s.u32(0x800cfe1c + 4 * i); p = s.u32(e + 0x198)
    print("s%02d slot %d idx %d Wort %08x e %08x parts %08x" % (n, i, s.u8(e + 0x218), s.u32(e + 4), e, p))
    for k in range(4):
        q = p + 172 * k
        R = [s.s16(q + 0x18 + 2 * j) for j in range(9)]
        tl = (s.s32(q + 0x2c), s.s32(q + 0x30), s.s32(q + 0x34))
        W = [s.s16(q + 0x48 + 2 * j) for j in range(9)]
        tw = (s.s32(q + 0x5c), s.s32(q + 0x60), s.s32(q + 0x64))
        print("  teil %d @%08x w0 %08x Rl %s tl %s | W %s tw %s | +74 %08x +94 %08x scale %d %d %d ang %d %d %d" % (
            k, q, s.u32(q), R, tl, W, tw, s.u32(q + 0x74), s.u32(q + 0x94), s.s16(q + 0x8c), s.s16(q + 0x8e), s.s16(q + 0x90),
            s.s16(q + 0x68), s.s16(q + 0x6a), s.s16(q + 0x6c)))
