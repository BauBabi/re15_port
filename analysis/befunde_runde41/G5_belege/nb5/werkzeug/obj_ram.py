import sys, struct
sys.path.insert(0, ".")
from re2ram4 import Ram, SN
for n in range(1, 27):
    r = Ram(SN % n)
    g5 = r.u32(0x800cfe1c)
    pl = r.u32(0x800cfe14)
    row = "s%02d G5 ph %d t %4d pl_x %6d |" % (n, r.u8(g5+6), r.s16(g5+0x158), r.s32(pl+0x38))
    for k in (2, 3, 4):
        o = 0x800d0324 + k*0x1f8
        row += " O%d f %08x x %6d y %6d z %6d d %04x %04x %04x 14c %d |" % (k, r.u32(o), r.s32(o+0x38), r.s32(o+0x3c), r.s32(o+0x40),
              r.s16(o+0x74)&0xffff, r.s16(o+0x76)&0xffff, r.s16(o+0x78)&0xffff, r.u32(o+0x14c))
    print(row)
