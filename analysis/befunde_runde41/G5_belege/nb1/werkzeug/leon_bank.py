import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
s = SS(SN % int(sys.argv[1]))
pl = s.u32(0x800cfe14)
for nm, (oe, od) in (("+108/+17C base", (0x108, 0x17c)), ("+180/+184", (0x180, 0x184)), ("+190/+194", (0x190, 0x194)), ("+188/+18C", (0x188, 0x18c))):
    a, b = s.u32(pl+oe), s.u32(pl+od)
    print(nm, hex(a), hex(b))
    # find which is EDD: EDD first word = (n*4)<<16 | cnt
    for EDD, EMR in ((b, a), (a, b)):
        if not (0x80000000 <= EDD < 0x80200000 and 0x80000000 <= EMR < 0x80200000): continue
        w0 = s.u32(EDD); n = (w0 >> 16) // 4
        if not (1 <= n <= 64): continue
        foff = s.u16(EMR+2); esz = (s.u32(EMR+4) >> 18) * 4
        if esz == 0 or esz > 400: continue
        print("  EDD %08x EMR %08x clips %d esz %d" % (EDD, EMR, n, esz))
        for c in range(min(n, 20)):
            w = s.u32(EDD+4*c); cnt = w & 0xffff; off = w >> 16
            sp = []
            for f in range(cnt):
                kf = s.u32(EDD+off+4*f) & 0xfff
                ka = EMR + foff + esz*kf
                sp.append((s.s16(ka+6), s.s16(ka+8), s.s16(ka+10)))
            print("   clip %2d frames %3d speed first %s last %s" % (c, cnt, sp[0] if sp else None, sp[-1] if sp else None))
        break
