import sys, glob, os, struct
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/.claude/skills/re15-savestate-ghidra/scripts')
import re15_ss
RD = r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/re15_port/shared_assets/PSX/'
rdts = {os.path.basename(p): open(p,'rb').read() for p in glob.glob(RD+'STAGE*/ROOM*.RDT')}
def ident(r):
    base = r.u32(0x800ac778)
    head = r.bytes(base, 0x200)
    hits = [n for n, d in rdts.items() if d[:0x200] == head]
    return base, hits
for f in sys.argv[1:]:
    r = re15_ss.Ram(f)
    base, hits = ident(r)
    print(os.path.basename(f), 'rdt@%08x' % base, hits[:6], 'cut=%d' % r.u16(0x800b532e))
    for i in range(4):
        e = 0x800acc2c + i*0x1f4
        t = r.u8(e+8)
        if not t: continue
        print('   e%d typ=%02x st=%d sub=%d clip(+94)=%d frm(+95)=%d 1c4=%04x pos=(%d,%d,%d) mesh+10=%08x edd16c=%08x emr84=%08x own174=%08x rbj180=%08x rbj184=%08x' % (
            i, t, r.u8(e+4), r.u8(e+5), r.u8(e+0x94), r.u8(e+0x95), r.u16(e+0x1c4),
            r.s32(e+0x34) if False else r.s16(e+0x38), r.s16(e+0x3a), r.s16(e+0x3c),
            r.u32(e+0x10), r.u32(e+0x16c), r.u32(e+0x84), r.u32(e+0x174), r.u32(e+0x180), r.u32(e+0x184)))
