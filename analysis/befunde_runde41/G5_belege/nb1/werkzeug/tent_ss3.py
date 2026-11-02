import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
for n in [int(a) for a in sys.argv[1:]]:
    s = SS(SN % n)
    g5 = s.u32(0x800cfe1c)
    print("== s%02d cut %d G5 x %d t158 %d word %08x 14d %d" % (n, s.u16(0x800d4820), s.s32(g5+0x38), s.s16(g5+0x158), s.u32(g5+4), s.u8(g5+0x14d)))
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4*i)
        parts = s.u32(e+0x198)
        sc = s.s16(parts+0x8c)
        print("  arm%d word %08x mot %d fr %d scaleX %d rot(%d,%d,%d)" % (i-1, s.u32(e+4), s.u8(e+0x14c), s.u8(e+0x14d), sc, s.s16(e+0x74), s.s16(e+0x76), s.s16(e+0x78)))
        for p in range(4):
            b = parts + p*172
            M = [s.s16(b+72+2*k) for k in range(9)]
            t = (s.s32(b+92), s.s32(b+96), s.s32(b+100))
            tip = ""
            if p == 3:
                tip = " tip(%d,%d,%d)" % (t[0] + 1500*M[0]//4096, t[1] + 1500*M[3]//4096, t[2] + 1500*M[6]//4096)
            print("     p%d ang(%d,%d,%d) t(%d,%d,%d) Mcol0(%d,%d,%d)%s" % (p, s.s16(b+104), s.s16(b+106), s.s16(b+108), t[0], t[1], t[2], M[0], M[3], M[6], tip))
