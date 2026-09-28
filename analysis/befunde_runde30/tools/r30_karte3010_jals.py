#!/usr/bin/env python3
"""r30_karte3010_jals.py - alle jal-Ziele in einem Adressbereich (RE2 Leon PSX.EXE).

Aufruf: r30_karte3010_jals.py <von> <bis> [--exe re2|re15]
Zweck: belegen, dass ein Codebereich eine bestimmte Funktion NICHT ruft
(z.B. der Hinweis-Zeichner den Flag-Setzer 0x8007730C).
"""
import struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
args = [a for a in sys.argv[1:] if not a.startswith("--")]
exe = "re2"
if "--exe" in sys.argv:
    exe = sys.argv[sys.argv.index("--exe") + 1]; args.remove(exe)
p = {"re2": os.path.join(REPO, "info", "re2leon", "PSX.EXE"),
     "re15": os.path.join(REPO, "info", "Re1.5", "PSX.EXE")}[exe]
d = open(p, "rb").read()
base = struct.unpack_from("<I", d, 0x18)[0]
a0 = int(args[0], 0); a1 = int(args[1], 0)
seen = {}
for a in range(a0, a1, 4):
    w = struct.unpack_from("<I", d, a - base + 0x800)[0]
    if (w >> 26) == 3:
        t = ((a + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
        seen.setdefault(t, []).append(a)
print("; %s  jal-Ziele in 0x%08X..0x%08X" % (os.path.basename(os.path.dirname(p)), a0, a1))
for t in sorted(seen):
    print("  0x%08X  x%d  von %s" % (t, len(seen[t]), " ".join("0x%08X" % x for x in seen[t][:8])))
