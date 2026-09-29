#!/usr/bin/env python3
"""volldis.py - Runde 34, Gegenpruefung re_gegner_re2_familie.
Vollstaendiges Disasm-Listing einer RE2- (Default) oder RE1.5-Binaerdatei zum Greppen.
Nutzt load()/dis_one() der Skill-Skripte (Offsets NIE selbst gerechnet).
Aufruf: python volldis.py re2|re15 <start_hex> <end_hex> [--bin NAME] > listing.txt
"""
import sys, importlib.util, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
which = sys.argv[1]
script = {"re2": "re2_disasm.py", "re15": "re15_disasm.py"}[which]
spec = importlib.util.spec_from_file_location("d", os.path.join(REPO, ".claude/skills/re15-psx-disasm/scripts", script))
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)
start = int(sys.argv[2], 16); end = int(sys.argv[3], 16)
binname = None
if "--bin" in sys.argv:
    binname = sys.argv[sys.argv.index("--bin") + 1]
data, fo, path = d.load(start, binname)
print("; %s  %08x..%08x" % (path, start, end))
a = start
while a < end:
    off = fo(a)
    if off + 4 > len(data): break
    w = d.word(data, fo, a)
    try:
        t, _ = d.dis_one(w, a)
    except Exception:
        t = ".word 0x%08x" % w
    print("%08x: %08x  %s" % (a, w, t))
    a += 4
