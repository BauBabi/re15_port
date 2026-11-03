# Abnahme 3: RE2-RAM Arme (Gegner-Slots 1..4 = Typ 0x37) und G5 (Slot 0) je Savestate
import sys
sys.path.insert(0, "wz")
from re2_ram import SS, SN
for n in [int(a) for a in sys.argv[1:]]:
    s = SS(SN % n); g5 = s.u32(0x800cfe1c)
    print("s%02d Schnitt %d G5 x %d Wort %08x t158 %d clip %d bild %d | Leon x %d" % (n, s.u16(0x800d4820), s.s32(g5 + 0x38), s.u32(g5 + 4), s.s16(g5 + 0x158), s.u8(g5 + 0x14c), s.u8(g5 + 0x14d), s.s32(s.u32(0x800cfe14) + 0x38)))
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4 * i)
        if e < 0x80000000: print("   slot %d leer" % i); continue
        print("   slot %d typ %02x w0 %08x Wort %08x (%d,%d,%d) clip %d bild %d +06 %d scale? %d %d %d" % (i, s.u8(e + 0x8c + 0x10) if False else s.u8(e + 0x8 + 0x3), s.u32(e), s.u32(e + 4), s.s32(e + 0x38), s.s32(e + 0x3c), s.s32(e + 0x40),
              s.u8(e + 0x14c), s.u8(e + 0x14d), s.u8(e + 6), s.s16(e + 0x80), s.s16(e + 0x82), s.s16(e + 0x84)))
