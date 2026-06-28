#!/usr/bin/env python3
"""re15_savestate_inspect.py - extract labeled RE1.5 game state from a DuckStation
savestate, using addresses recovered with Ghidra + an overlay fingerprint for the
stage. Run:  python re15_savestate_inspect.py <savestate.sav> [out.png]

The .sav -> RAM decode lives in re15_ss.py (same dir). Every printed field cites
the RE1.5 PSX.EXE global / Ghidra evidence it comes from.
"""
import sys, os, zlib
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re15_ss import Ram, vram_png

# --- Stage identity via the resident stage overlay ---------------------------
# Each stage loads its overlay at 0x80100000 (CLAUDE.md / Ghidra: the stage code
# is mapped there and stays resident across all rooms of the stage). CRC32 of the
# first 32 KB is therefore a stable per-stage fingerprint. Seed table = the two
# stages captured live on 2026-06-28; extend by capturing more stages and running
# this script with --fp to print a new fingerprint.
STAGE_FP = {
    0x96290818: "STAGE 1 (boot / briefing-area)",
    0x1c0394d0: "STAGE 2",
}

# --- Ghidra-cited game-state globals (RE1.5 PSX.EXE) -------------------------
#   0x800acaee s16 player HP        FUN_80012d60 (damage): HP = DAT_800acaee = player+0x9a
#   0x800aca58 u8  player FSM state FUN_80031c44: 1=idle, 2=hit, 3=death
#   0x800aca3c u32 display flags    FUN_80014230 gate: bit0x100 = BG-renderer skip,
#                                   bit0x10 = fade direction
#   0x800b5568 u8  fade level       FUN_80021a0c: 0x00..0xF0 (0xF0 = full black)
#   0x800b532e u16 active cam cut   fresh Ghidra (decomp_globals.py): DAT_800b5330 =
#                                   DAT_800b532e<<3 -> 532e indexes the camera-cut tables
def inspect(path, png=None):
    r = Ram(path)
    fp = zlib.crc32(r.bytes(0x80100000, 0x8000)) & 0xffffffff
    stage = STAGE_FP.get(fp, "UNKNOWN (new fp - add to STAGE_FP)")
    print("file            : %s" % os.path.basename(path))
    print("blob / RAM base : %d bytes / 0x%x" % (len(r.blob), r.base))
    print("stage           : %s" % stage)
    print("overlay fp      : 0x%08x  (CRC32 of 0x80100000..+0x8000)" % fp)
    print("camera cut      : %d        (DAT_800b532e)" % r.u16(0x800b532e))
    print("player HP       : %d        (DAT_800acaee, FUN_80012d60)" % r.s16(0x800acaee))
    print("player state    : %d        (DAT_800aca58: 1=idle 2=hit 3=death)" % r.u8(0x800aca58))
    print("display flags   : 0x%x      (DAT_800aca3c: 0x100=BG-skip 0x10=fade-dir)" % r.u32(0x800aca3c))
    print("fade level      : 0x%x      (DAT_800b5568: 0xF0=black)" % r.u8(0x800b5568))
    if png:
        vram_png(r, png)
        print("framebuffer PNG : %s  (full 1024x512 VRAM)" % png)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(1)
    inspect(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)
