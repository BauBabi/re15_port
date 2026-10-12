import sys, os
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv/analysis/befunde_runde35/H_raeume/re2_mess")
import re2_ss
PL, ETAB = 0x800CFBF8, 0x800CFE18
for p in sys.argv[1:]:
    r = re2_ss.load(p)
    print("==", os.path.basename(p), "stage", r.s16(0x800D481C), "room", r.s16(0x800D481E), "PL", (r.s32(PL+0x38), r.s32(PL+0x40)))
    for i in range(32):
        a = r.u32(ETAB + 4*i)
        if a < 0x80000000 or a >= 0x80200000: continue
        t = r.u8(a + 8)
        if t == 0: continue
        print("  [%2d] @%08x typ %02x w4 %08x desc(+10E) %04x pos(%d,%d) d(+1F0)=%d los(+154)=%04x t158=%d t15a=%d hp=%d clip=%08x" % (
            i, a, t, r.u32(a+4), r.u16(a+0x10E), r.s32(a+0x38), r.s32(a+0x40), r.u32(a+0x1F0), r.u16(a+0x154),
            r.s16(a+0x158), r.s16(a+0x15A), r.s16(a+0x156), r.u32(a+0x14C)))
