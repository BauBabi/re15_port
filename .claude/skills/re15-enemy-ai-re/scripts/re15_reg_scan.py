#!/usr/bin/env python3
"""re15_reg_scan.py — enumerate the REGISTERED enemy roster of a RE1.5 stage overlay.

The entity dispatch table lives at 0x80072bac in the EXE; each stage overlay's init writes
its per-type handler pointers there with `sw handler, (0x80072bac + type*4)`. This tool scans
a STAGE{N}.BIN for those stores and decodes type -> handler, giving you the authoritative
"what enemies does this stage register" list in one shot — BEFORE you spend a workflow RE'ing
a type that turns out to be a stub or an unused (zero-HP) slot.

STAGE*.BIN load at 0x80100000 with NO 0x800 header: file_off = addr - 0x80100000.

Usage:
    python re15_reg_scan.py <STAGE1.BIN ...>          # one or more overlay BINs
    python re15_reg_scan.py info/Re1.5/PSX/BIN/STAGE*.BIN

Output per BIN: `type 0xNN -> handler 0x80xxxxxx` grouped by handler (so shared roots — e.g.
the zombie band 0x10..0x1f on one root, or G-Birkin 0x30+0x36 on one root — are obvious).

Follow-ups the scan sets up (do these next, don't guess):
  * Shared handler for >1 type? Scan its region for `lbu rX,8(rY)` (+0x8 type reads) and the
    other type as an immediate: 0 hits => byte-IDENTICAL AI, the type only selects the model.
  * Registered but is it real? Disasm the INIT slot: no +0x9a HP write / all-`jr ra` leaves =
    a stub (like 0x22) — leave it UNROUTED. Check the type-indexed HP table too (a zero row =
    unused slot, like the zombies 0x1c-0x1f @0x8011f034+type*0x20).
"""
import sys, struct, os

DISP = 0x80072bac        # EXE entity dispatch table base
BASE = 0x80100000        # overlay load address (no header)

def scan(path):
    data = open(path, 'rb').read()
    hi = {}                # reg -> lui immediate << 16
    regs = {}              # reg -> full value after addiu
    t2h = {}               # type -> handler
    for off in range(0, len(data) - 3, 4):
        w = struct.unpack_from('<I', data, off)[0]
        op = w >> 26; rs = (w >> 21) & 0x1f; rt = (w >> 16) & 0x1f; imm = w & 0xffff
        if op == 0x0f:                         # lui rt, imm
            hi[rt] = imm << 16; regs[rt] = imm << 16
        elif op == 0x09 and rs in hi:          # addiu rt, rs, simm  (builds a pointer)
            simm = imm - 0x10000 if imm & 0x8000 else imm
            regs[rt] = (hi.get(rs, 0) + simm) & 0xffffffff
        elif op == 0x2b:                       # sw rt, imm(rs)
            abs_off = 0x80070000 + imm         # rs assumed = lui at,0x8007
            if DISP <= abs_off <= DISP + 0x100 and (abs_off - DISP) % 4 == 0:
                t2h[(abs_off - DISP) // 4] = regs.get(rt, 0)
    return t2h

def main(argv):
    if len(argv) < 2:
        print(__doc__); return 1
    for path in argv[1:]:
        t2h = scan(path)
        name = os.path.basename(path)
        types = sorted(t2h)
        print(f"=== {name}: {len(types)} types "
              f"[{' '.join('0x%02x' % t for t in types)}] ===")
        byh = {}
        for t, h in sorted(t2h.items()):
            byh.setdefault(h, []).append(t)
        for h, ts in sorted(byh.items()):
            tag = "  (SHARED root -> byte-identity check!)" if len(ts) > 1 and h else ""
            print(f"  handler 0x{h:08x}: " + " ".join("0x%02x" % t for t in ts) + tag)
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
