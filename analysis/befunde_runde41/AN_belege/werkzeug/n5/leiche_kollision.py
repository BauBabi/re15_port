"""NB5 Punkt 3: hat die tote Annette (RE2 Claire ROOM6141, Savestates lauf3) eine Kollision gegen den Spieler?
RE2-Schub FUN_80034D0C (Leon-EXE): Tore (a.w0|p.w0)&2 @0x80034d48-50, (a.w0&p.w0)&0x1000 @0x80034d58-60, p.w0&4 @0x80034d68-6c,
Segmentzahl lw +0x1E8 @0x80034d74 -> -1 -> bltz @0x80034da4 (0 Segmente = kein Schub). Segmente ab +0x84 (addiu s5,s6,132 @0x80034d78)."""
import sys, struct
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_belege/re2_claire/werkzeug")
from re2c_ss import SS
D = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/an/capture/lauf3"
for n in sys.argv[1:]:
    s = SS(D + "/" + n + ".sav")
    pl = s.u32(0x800c79a8); a = s.u32(0x800c79b0); b = s.u32(0x800c79b4)
    def ent(nm, p):
        if not p or p == 0x800cbde0: print("  %s -" % nm); return
        w0 = s.u32(p); nseg = s.s32(p + 0x1e8)
        segs = []
        for i in range(max(0, min(nseg, 4))):
            q = p + 0x84 + i * 0x20   # Schrittweite unten geprueft
            segs.append((s.s32(q), s.s32(q+4), s.s32(q+8), s.s16(q+0x14), s.u16(q+0x16), s.u16(q+0x1a)))
        print("  %s @%08x id%02x w0=%08x (b1=%d b2=%d b4=%d b1000=%d b0x10_0000=%d) +0x1E8=%d pos(%d,%d,%d) hp=%d seg=%s" % (
            nm, p, s.u8(p+8), w0, w0&1, (w0>>1)&1, (w0>>2)&1, (w0>>12)&1, (w0>>20)&1, nseg,
            s.s32(p+0x38), s.s32(p+0x3c), s.s32(p+0x40), s.s16(p+0x156), segs))
        print("     +0x84.. raw", s.bytes(p+0x84, 0x40).hex())
    print(n)
    ent("Claire ", pl); ent("Annette", a); ent("Birkin ", b)
    # Liste der Entities, die FUN_800355C4-Gegenstueck durchlaeuft (Leon: 0x800cfe14..[0x800ce334])
