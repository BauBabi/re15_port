#!/usr/bin/env python3
"""re15_flag_sweep.py - sweep a RAM global (or one bit of it) across ALL RE1.5 savestates.

THE DORMANCY CHECK. Born from Phase 8.7 (2026-06-29): the question "does the live game ever set
DAT_800aca3c & 1 (the zombie lunge-arm gate)?" was answered by reading that bit across every
savestate in stage_saves/ -- it was NEVER set (even in a live ROOM1140 combat save with 7 active
zombies), which proved the lunge-arm is DORMANT in this prototype and the zombies attack via the
GRAB instead. That single sweep CORRECTED the ported attack model. This script generalizes it:
before porting a code path gated on a flag/bit, sweep the bit across the saves -- if it is never
set in any relevant-stage save, the gated path is dormant (don't assume the static gate fires).

Reuses re15_ss.Ram (decompress + PSX.EXE-signature RAM-base) + a per-save overlay fingerprint
(CRC32 of the overlay @0x80100000, 32 KB) so you can group saves by STAGE (a gate may be dormant
in one stage's saves but live in another's). Stdlib + zstandard only.

Usage:
  python re15_flag_sweep.py <addr> [bitmask]   # e.g. 0x800aca3c 0x1   (sweep DAT_800aca3c bit 0)
  python re15_flag_sweep.py 0x800acaee          # sweep a value (no mask) -- prints the s16/u32
  python re15_flag_sweep.py <addr> <mask> --width 1|2|4   # value width (default 4)
  python re15_flag_sweep.py <addr> <mask> --glob "stage_saves/*.sav"   # custom save set

Run from the repo root (so stage_saves/ resolves). Exit code 0 always; read the summary.
"""
import sys, os, glob, zlib
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re15_ss import Ram

# Known per-stage overlay fingerprints (extend as new stage captures appear) — see
# re15_savestate_inspect.py STAGE_FP. STAGE 1 = the briefing-area (room1140 zombies).
STAGE_FP = {
    0x96290818: "STAGE1",
    0x1c0394d0: "STAGE2",
}

def overlay_fp(r):
    return zlib.crc32(r.bytes(0x80100000, 32 * 1024)) & 0xffffffff

def read_val(r, addr, width):
    if width == 1: return r.u8(addr)
    if width == 2: return r.u16(addr)
    return r.u32(addr)

def main():
    args = [a for a in sys.argv[1:]]
    if not args:
        print("usage: re15_flag_sweep.py <addr> [bitmask] [--width 1|2|4] [--glob PAT]"); return 1
    width = 4
    pattern = None
    if "--width" in args:
        i = args.index("--width"); width = int(args[i + 1]); del args[i:i + 2]
    if "--glob" in args:
        i = args.index("--glob"); pattern = args[i + 1]; del args[i:i + 2]
    addr = int(args[0], 0)
    mask = int(args[1], 0) if len(args) > 1 else None

    saves = []
    if pattern:
        saves = sorted(glob.glob(pattern))
    else:
        saves = sorted(glob.glob("stage_saves/*.sav")) + sorted(glob.glob("stage_saves/mzd_demo/*.sav"))
    if not saves:
        print("no savestates found (run from the repo root; stage_saves/ must exist)"); return 1

    title = "0x%08x" % addr + ("" if mask is None else " & 0x%x" % mask)
    print("=== sweep %s across %d savestates (width %d) ===" % (title, len(saves), width))
    print("%-42s %-8s %-12s %s" % ("save", "stage", "value", "" if mask is None else "bit-set?"))
    print("-" * 80)
    set_count = 0
    set_in_stages = set()
    for s in saves:
        try:
            r = Ram(s)
            fp = overlay_fp(r)
            stage = STAGE_FP.get(fp, "fp=0x%08x" % fp)
            val = read_val(r, addr, width)
            if mask is None:
                print("%-42s %-8s 0x%-10x" % (os.path.basename(s), stage, val))
            else:
                hit = (val & mask) != 0
                if hit:
                    set_count += 1; set_in_stages.add(stage)
                print("%-42s %-8s 0x%-10x %s" % (os.path.basename(s), stage, val, "SET" if hit else "-"))
        except Exception as e:
            print("%-42s ERROR %s" % (os.path.basename(s), e))
    if mask is not None:
        print()
        if set_count == 0:
            print("VERDICT: 0x%x is NEVER set in any of the %d saves -> the gated path is DORMANT "
                  "in these captures (do NOT assume the static gate fires; verify the actual "
                  "in-game mechanism)." % (mask, len(saves)))
        else:
            print("VERDICT: 0x%x is set in %d/%d saves (stages: %s) -> the gate IS live; the gated "
                  "path runs in-game." % (mask, set_count, len(saves), ", ".join(sorted(set_in_stages))))
    return 0

if __name__ == "__main__":
    sys.exit(main())
