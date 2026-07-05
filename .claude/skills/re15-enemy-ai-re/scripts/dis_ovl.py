#!/usr/bin/env python3
"""dis_ovl.py - capstone-Disasm eines RE1.5-STAGE-Overlays (MIPS32 LE).

Overlays laden @0x80100000 und haben KEINEN 0x800-Header:
    file_off = addr - 0x80100000
(Der ad-hoc "+0x800"-Fehler liest Garbage und sieht wie "runtime-patched" aus -
 siehe Memory reai-v2-re-pitfalls.)

Usage:
    python dis_ovl.py <lo_hex> <hi_hex> [stage]      # stage 1..6, default 1
    python dis_ovl.py 0x80102548 0x80102c00 1
Braucht: pip install capstone (plain Windows-python, NICHT mingw64).
"""
import os
import sys
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN

BASE = 0x80100000
BIN_DIR = os.path.join("C:" + os.sep, "workspace", "git", "reAi_v2",
                       "info", "Re1.5", "PSX", "BIN")

def dis(lo, hi, stage=1):
    binpath = os.path.join(BIN_DIR, "STAGE%d.BIN" % stage)
    with open(binpath, "rb") as f:
        f.seek(lo - BASE)
        data = f.read(hi - lo)
    md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 | CS_MODE_LITTLE_ENDIAN)
    md.skipdata = True
    for ins in md.disasm(data, lo):
        b = ins.bytes.hex()
        print(f"{ins.address:08x}: {b}  {ins.mnemonic} {ins.op_str}")

if __name__ == "__main__":
    lo = int(sys.argv[1], 16)
    hi = int(sys.argv[2], 16)
    stage = int(sys.argv[3]) if len(sys.argv) > 3 else 1
    dis(lo, hi, stage)
