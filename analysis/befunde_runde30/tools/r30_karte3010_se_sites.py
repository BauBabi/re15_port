#!/usr/bin/env python3
"""r30_karte3010_se_sites.py - alle Aufrufe Se_on(<wort>) mit 'lui a0,<hi>' (RE2/RE1.5 EXE).

Sucht 'lui a0,HI' (0x3C04hhhh) und meldet die Stelle, wenn in den 3 Instruktionen
danach (oder 1 davor: Delay-Slot-Form) ein 'jal <se>' steht.
Aufruf: r30_karte3010_se_sites.py <HI hex> [--exe re2|re15] [--se <adresse>]
RE2:   Se_on = 0x8005BA28 (Default).  RE1.5: FUN_80045024 (--exe re15 --se 0x80045024).
"""
import struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
a = sys.argv[1:]
exe = "re2"; se = 0x8005BA28
if "--exe" in a: i = a.index("--exe"); exe = a[i + 1]; del a[i:i + 2]
if "--se" in a: i = a.index("--se"); se = int(a[i + 1], 0); del a[i:i + 2]
hi = int(a[0], 16)
p = {"re2": os.path.join(REPO, "info", "re2leon", "PSX.EXE"),
     "re15": os.path.join(REPO, "info", "Re1.5", "PSX.EXE")}[exe]
d = open(p, "rb").read()
base = struct.unpack_from("<I", d, 0x18)[0]
n = (len(d) - 0x800) // 4
W = struct.unpack_from("<%dI" % n, d, 0x800)
jal = (3 << 26) | ((se & 0x0FFFFFFF) >> 2)
want = 0x3C040000 | hi
cnt = 0
for i, w in enumerate(W):
    if w != want: continue
    near = [k for k in range(max(0, i - 1), min(n, i + 4)) if W[k] == jal]
    if near:
        cnt += 1
        print("  0x%08X  lui a0,0x%X   jal @0x%08X" % (base + i * 4, hi, base + near[0] * 4))
print("; %s: %d Aufrufe Se(0x%04X0000)" % (exe, cnt, hi))
