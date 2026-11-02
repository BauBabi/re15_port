import sys
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\analysis\befunde_runde41\AN_belege\re2_claire\werkzeug")
from re2c_ss import SS
LAUF3 = r"C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\an\capture\lauf3"
BASE = 0x800d07d0; STRIDE = 0x7c
for n in sys.argv[1:]:
    s = SS(LAUF3 + "\\" + n + ".sav")
    bir = s.u32(0x800c79b0 + 4)
    ann = s.u32(0x800c79b0)
    print("==", n, "Birkin @%08x  Annette @%08x" % (bir, ann))
    act = 0
    for i in range(96):
        r = BASE + i * STRIDE
        fl = s.u16(r + 0x18)
        if fl == 0: continue
        act += 1
        idw = s.u32(r + 0x1c)
        print(" slot %2d fl=%04x id=%d sub=%d ang=%d sc=%04x +20=%d +24=%08x +28=%08x +2c=%08x +30=%08x +32=%d +34=%08x +3c=%08x +40=%08x +44=%08x +48=%08x anc=%08x T=(%d,%d,%d) 0..14=%s" % (
            i, fl, idw & 0xffff, idw >> 16, s.s16(r + 0x22), s.u32(r + 0x38) >> 16, s.u16(r + 0x20), s.u32(r+0x24), s.u32(r+0x28), s.u32(r+0x2c), s.u32(r+0x30), s.s16(r+0x32), s.u32(r+0x34), s.u32(r+0x3c), s.u32(r+0x40), s.u32(r+0x44), s.u32(r+0x48),
            s.u32(r + 0x6c), s.s32(r + 0x60), s.s32(r + 0x64), s.s32(r + 0x68), s.bytes(r, 0x18).hex()))
    print(" aktiv", act)
