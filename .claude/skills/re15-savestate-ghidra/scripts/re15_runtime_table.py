#!/usr/bin/env python3
"""re15_runtime_table.py - read a pointer/dispatch table from a savestate's live RAM,
to CROSS-CHECK it against the static BIN (is it really runtime-built, or static?).

WHY THIS EXISTS: most overlay dispatch tables are STATIC overlay data — read them with
`re15_disasm.py table <addr>` (overlay off = addr-0x80100000, NO 0x800 header). Use THIS
tool to confirm a static table matches live RAM, or to read a table that is genuinely
built in RAM at runtime. (Cautionary tale, 2026-06-30: the @0x8011f840/890 dispatch
tables were briefly believed "runtime-patched / null on disc" — that was an ad-hoc
OFFSET BUG, adding a 0x800 header that overlays don't have; read correctly they are
fully present in STAGE1.BIN and 40/40 identical to RAM. See reai-v2-re-pitfalls.)

STAGE1 live-zombie m0 brain (these are STATIC in the BIN; values here == BIN == RAM):
  @0x8011f840  DECIDE  table  (the +0x5 decision handlers)
  @0x8011f890  ANIMATE table  (the +0x5 animation handlers)
Values (BIN == savestate, from stage_saves/mzd_stage1_engage_live.sav):
  +0x5  decide       animate      meaning
  0x00  0x80101b64   0x80101d08   search/idle
  0x02  0x80102058   0x801021f8   ENGAGE
  0x03  0x80102540   0x80102548   GRAB (face)
  0x04  0x80102540   0x80102548   GRAB (behind)
  0x05  0x80102bd0   0x80102bd8   forward-walk
  0x06  0x80102bd0   0x80102bd8   forward-walk
  0x07  0x80102d20   0x80102dc8   TURN-to-face
  0x0c  0x801048a8   0x801048e8   feeding
  0x13  0x8010561c   0x801057bc   engage + a +0x90&3-gated forward-walk pre-check

Usage:
  python re15_runtime_table.py <sav> <addr> [count]          # one table (default 16 entries)
  python re15_runtime_table.py <sav> <addr> [count] --pair <addr2>   # two side-by-side
Each entry is tagged overlay(0x8010xxxx)/EXE(0x800xxxxx)/data/NULL. After reading,
`re15_disasm.py dis <handler>` to disassemble the actual (patched-in) handler.
"""
import sys, os, argparse
from re15_ss import Ram

def tag(v):
    if v == 0:                       return "NULL"
    if 0x80100000 <= v < 0x80130000: return "overlay"
    if 0x80010000 <= v < 0x80100000: return "EXE"
    if 0x80000000 <= v < 0x80200000: return "data/RAM"
    return "?"

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sav")
    ap.add_argument("addr", type=lambda s: int(s, 0))
    ap.add_argument("count", nargs="?", type=lambda s: int(s, 0), default=16)
    ap.add_argument("--pair", type=lambda s: int(s, 0), default=0,
                    help="second table address to print side-by-side (e.g. the ANIMATE table)")
    a = ap.parse_args()
    r = Ram(a.sav)
    print("file        :", os.path.basename(a.sav))
    print("RAM base off : 0x%x" % r.base)
    if a.pair:
        print("\n idx    @0x%08x        @0x%08x" % (a.addr, a.pair))
        for i in range(a.count):
            v1 = r.u32(a.addr + i*4); v2 = r.u32(a.pair + i*4)
            print("  0x%02x  0x%08x %-8s  0x%08x %-8s" % (i, v1, tag(v1), v2, tag(v2)))
    else:
        print("\n table @0x%08x (%d entries):" % (a.addr, a.count))
        nulls = 0
        for i in range(a.count):
            v = r.u32(a.addr + i*4)
            if v == 0: nulls += 1
            print("  [0x%02x] 0x%08x -> 0x%08x  %s" % (i, a.addr + i*4, v, tag(v)))
        if nulls > a.count // 2:
            print("\n  NOTE: %d/%d entries NULL — likely the wrong address (RAM reads are direct," % (nulls, a.count))
            print("        no header). Compare with re15_disasm.py table <addr> on the static BIN.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
