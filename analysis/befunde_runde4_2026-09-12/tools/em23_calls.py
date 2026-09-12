#!/usr/bin/env python3
"""Re-Verify: alle jal 0x8005bd6c im EM23-Overlay + a0-Setup davor."""
import struct
OVL = open("c:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN", "rb").read()
BASE = 0x80100000
target = 0x0C000000 | ((0x8005bd6c >> 2) & 0x3FFFFFF)
for i in range(0, len(OVL)-3, 4):
    w = struct.unpack_from("<I", OVL, i)[0]
    if w == target:
        addr = BASE + i
        # a0-Setup in den 8 Instruktionen davor + Delay-Slot suchen
        ctx = []
        for j in range(max(0, i-32), i+8, 4):
            ww = struct.unpack_from("<I", OVL, j)[0]
            op = ww >> 26; rs = (ww>>21)&31; rt = (ww>>16)&31
            imm = ww & 0xFFFF
            if op == 9 and rs == 0 and rt == 4:      # addiu a0, zero, imm
                ctx.append("li a0,%d @0x%08X" % (imm, BASE+j))
            if op == 0 and (ww & 63) == 0x21 and rs == 0 and ((ww>>16)&31) == 0 and ((ww>>11)&31) == 4:
                ctx.append("move a0,zero(addu) @0x%08X" % (BASE+j))
        print("jal 0x8005bd6c @0x%08X   %s" % (addr, "; ".join(ctx) or "a0 nicht als li erkennbar"))
