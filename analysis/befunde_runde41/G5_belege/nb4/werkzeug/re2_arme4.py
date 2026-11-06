# Nachbesserung 4 G5A: RE2-RAM Arme (Slots 1..4) und G5 (Slot 0) je Savestate, voll: Index +0x218, Lage, Drehung +0x74/76/78,
# Zaehler +0x158, Clip-Wort +0x14c, Zeichenbild +0x178, Streckung parts[0]+0x8c, Gelenk-Weltlagen parts[i]+92..100 (Teilgroesse 172)
import sys
from re2_ram import SS, SN
for n in [int(a) for a in sys.argv[1:]]:
    s = SS(SN % n); g5 = s.u32(0x800cfe1c)
    print("s%02d Schnitt %d G5 Wort %08x t158 %d clip %d bild %d x %d | Leon x %d" % (n, s.u16(0x800d4820), s.u32(g5 + 4), s.s16(g5 + 0x158),
          s.u8(g5 + 0x14c), s.u8(g5 + 0x14d), s.s32(g5 + 0x38), s.s32(s.u32(0x800cfe14) + 0x38)))
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4 * i)
        if e < 0x80000000: print("   slot %d leer" % i); continue
        p = s.u32(e + 0x198)
        gel = [(s.s32(p + 172 * k + 92), s.s32(p + 172 * k + 96), s.s32(p + 172 * k + 100)) for k in range(4)]
        print("   slot %d idx %d Wort %08x pos (%d,%d,%d) rot (%d,%d,%d) t158 %d w14c %08x z178 %08x scale %d anker %d | p0 %s p3 %s" % (
            i, s.u8(e + 0x218), s.u32(e + 4), s.s32(e + 0x38), s.s32(e + 0x3c), s.s32(e + 0x40), s.s16(e + 0x74), s.s16(e + 0x76), s.s16(e + 0x78),
            s.s16(e + 0x158), s.u32(e + 0x14c), s.u32(e + 0x178), s.s16(p + 0x8c), s.u8(e + 0x219), gel[0], gel[3]))
