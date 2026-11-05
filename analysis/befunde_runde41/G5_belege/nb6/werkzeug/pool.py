# RE2-FX-Pool 0x800D8CF0 (96 x 0x7C) + Registry 0x800EAE48 + Bankkoepfe 0x800D4CD8 aus den RE2-Savestates sNN dumpen
import sys, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_g5auftritt\analysis\befunde_runde41\G5_belege\abnahme4\werkzeug")
from re2ram4 import Ram, SN
POOL, STEP = 0x800D8CF0, 0x7C
def u16(r, a): return r.s16(a) & 0xffff
def dump(n, nur_bank=None):
    r = Ram(SN % n)
    reg = [r.u8(0x800EAE48 + k) for k in range(16)]
    print("=== s%02d  Registry 0x800EAE48: %s" % (n, " ".join("%02X" % x for x in reg)))
    rdt = r.u32(0x800ce324)
    print("    DAT_800ce324 (RDT) 0x%08X  +0x50 0x%08X +0x54 0x%08X +0x58 0x%08X +0x5c 0x%08X" % (rdt, r.u32(rdt+0x50), r.u32(rdt+0x54), r.u32(rdt+0x58), r.u32(rdt+0x5c)))
    for bid in [x for x in reg if x != 0xff]:
        bk = r.u32(0x800D4CD8 + 4 * bid); tb = r.u32(0x800D4E18 + 4 * bid)
        if bk < 0x80000000: print("    Bank %02X: kein Kopf (0x%08X)" % (bid, bk)); continue
        n1, n2 = u16(r, bk), u16(r, bk + 2)
        cells = []
        uvb = bk + (n1 * 4 + 4) * 2
        if bid == 0x19:
            cells = [(r.u8(uvb + 4*c), r.u8(uvb + 4*c + 1)) for c in range(n2)]
        print("    Bank %02X: Kopf 0x%08X Tab 0x%08X n1 %d n2 %d CLUT +4 0x%04X TPage +6 0x%04X %s" % (bid, bk, tb, n1, n2, u16(r, bk+4), u16(r, bk+6),
              ("Zellen(u,v) " + str(cells)) if cells else ""))
    lebend = 0
    for i in range(96):
        a = POOL + i * STEP
        st = u16(r, a + 0x18)
        if st == 0: continue
        bank = r.u8(a + 0x1C)
        if nur_bank is not None and bank not in nur_bank: continue
        lebend += 1
        print("  [%2d] @%08X st %04X bank %02X sub %02X OpA %2d OpB %2d s2 %3d s3 %3d asp %d/%d acc %d/%d/%d lb %d vel %d/%d/%d ph %d "
              "par %d art %d gier %d lok %d/%d/%d tp %04X ofs %d/%d/%d clut %04X welt %d/%d/%d skala %d anz %d/%d m.t %d/%d/%d"
              % (i, a, st, bank, r.u8(a+0x1E), r.u8(a), r.u8(a+1), r.u8(a+2), r.u8(a+3), u16(r,a+4), u16(r,a+6),
                 *struct.unpack("bbb", bytes([r.u8(a+8), r.u8(a+9), r.u8(a+10)])), r.u8(a+11),
                 r.s16(a+0xC), r.s16(a+0xE), r.s16(a+0x10), u16(r, a+0x12),
                 r.u8(a+0x1A), r.u8(a+0x1B), r.s16(a+0x22), r.s16(a+0x24), r.s16(a+0x26), r.s16(a+0x28), u16(r,a+0x2A),
                 r.s16(a+0x2C), r.s16(a+0x2E), r.s16(a+0x30), u16(r,a+0x32), r.s16(a+0x34), r.s16(a+0x36), r.s16(a+0x38),
                 u16(r,a+0x3A), r.u8(a+0x21), r.u8(a+0x20), r.s32(a+0x60), r.s32(a+0x64), r.s32(a+0x68)))
    print("  lebend %d" % lebend)
if __name__ == "__main__":
    nn = [int(x) for x in sys.argv[1:]] or [1, 2, 3]
    for n in nn: dump(n)
