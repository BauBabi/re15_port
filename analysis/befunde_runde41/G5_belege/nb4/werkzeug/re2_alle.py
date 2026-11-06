# Nachbesserung 4 G5A: RE2-RAM s01..s26 kompakt: G5 (Ph, +0x158, Clip, Bild), je Arm (Sub, Ph, Streckung, +0x158, Drehung x/y/z)
import sys
from re2_ram import SS, SN
for n in range(1, 27):
    s = SS(SN % n); g5 = s.u32(0x800cfe1c)
    w = s.u32(g5 + 4)
    z = "s%02d G5 sub %d ph %2d t %4d clip %d bild %3d |" % (n, (w >> 8) & 0xff, (w >> 16) & 0xff, s.s16(g5 + 0x158), s.u8(g5 + 0x14c), s.u8(g5 + 0x14d))
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4 * i)
        if e < 0x80000000: z += " leer |"; continue
        p = s.u32(e + 0x198); a = s.u32(e + 4)
        z += " A%d s%d p%d sc%d t%d r(%d,%d,%d) |" % (s.u8(e + 0x218), (a >> 8) & 0xff, (a >> 16) & 0xff, s.s16(p + 0x8c), s.s16(e + 0x158), s.s16(e + 0x74), s.s16(e + 0x76), s.s16(e + 0x78))
    print(z)
