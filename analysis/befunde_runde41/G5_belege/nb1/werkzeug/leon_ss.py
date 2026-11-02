import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
for n in range(1, 27):
    s = SS(SN % n)
    pl = s.u32(0x800cfe14); g5 = s.u32(0x800cfe1c)
    print("s%02d cut %2d Leon x %6d z %6d dir %4d +04 %08x 14c %3d 14d %3d 14e %3d 1cc %04x | G5 x %6d t158 %4d word %08x | 0x800d4864 %08x" % (n, s.u16(0x800d4820), s.s32(pl+0x38), s.s32(pl+0x40), s.s16(pl+0x76), s.u32(pl+4), s.u8(pl+0x14c), s.u8(pl+0x14d), s.u8(pl+0x14e), s.u16(pl+0x1cc), s.s32(g5+0x38), s.s16(g5+0x158), s.u32(g5+4), s.u32(0x800d4864)))
