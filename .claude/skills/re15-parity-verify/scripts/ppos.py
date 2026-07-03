#!/usr/bin/env python3
"""ppos.py — read the PLAYER position (x, z, rot, hp) from a DuckStation RE1.5 savestate.

The player entity block is DAT_800aca54 (HP at +0x9a = 0x800acaee). Fields:
  x = s16 @ +0x34 (0x800aca88)   z = s16 @ +0x3c (0x800aca90)
  rot = u16 @ +0x6a (0x800acabe, the heading; spawn -96 shows as 65440)   hp = s16 @ +0x9a

Reuses the Ram loader from re15-savestate-ghidra so decoding stays in one place.

  python ppos.py room.sav [more.sav ...]

Parity note: DuckStation emulation fps is VARIABLE per run (see re15-parity-verify SKILL §4) —
use these coords for TIMING-INDEPENDENT invariants (spawn, forward-direction ratio, wall/rest
positions), NOT to measure per-frame movement rates.
"""
import sys, os

# locate the sibling re15-savestate-ghidra/scripts (Ram class) relative to this skills tree
HERE = os.path.dirname(os.path.abspath(__file__))
_CANDS = [
    os.path.join(HERE, "..", "..", "re15-savestate-ghidra", "scripts"),   # .claude/skills/<this>/scripts
    os.path.join(HERE, "..", "..", "..", ".kiro", "skills", "re15-savestate-ghidra", "scripts"),
]
for c in _CANDS:
    if os.path.isdir(c):
        sys.path.insert(0, os.path.abspath(c))
        break
try:
    from re15_enemy_state import Ram
except Exception as e:
    sys.exit("cannot import Ram from re15-savestate-ghidra/scripts (%s)" % e)

PLAYER = 0x800aca54

def main():
    if len(sys.argv) < 2:
        sys.exit("usage: ppos.py <sav> [<sav> ...]")
    for sav in sys.argv[1:]:
        r = Ram(sav)
        x   = r.s16(PLAYER + 0x34)
        z   = r.s16(PLAYER + 0x3c)
        rot = r.u16(PLAYER + 0x6a)      # raw; spawn -96 == 65440
        srot = rot - 65536 if rot >= 32768 else rot
        hp  = r.s16(PLAYER + 0x9a)
        print("%-40s PL(x=%d, z=%d, rot=%d [s16 %d], hp=%d)" %
              (os.path.basename(sav), x, z, rot, srot, hp))

if __name__ == "__main__":
    main()
