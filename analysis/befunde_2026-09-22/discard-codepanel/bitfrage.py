"""Vollzensus fuer beliebige (bank,bit): jedes Set/Ck ueber alle ausgelieferten RDTs."""
import os, sys, glob, collections
HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.normpath(os.path.join(HERE, "..", "..", "..", "re15_port", "tools"))
sys.path.insert(0, TOOLS)
from scd_walk_lib import (u16, u32, op_size, fwd_target, rdt_section_end,
                          section_regions, walk_ops, regionen, messages, NAMES)


ROOT = os.path.normpath(os.path.join(HERE, "..", "..", "..", "re15_port", "shared_assets", "PSX"))
FRAGE = [(3, 127), (3, 187), (3, 110), (3, 50), (3, 137), (3, 139), (3, 60), (3, 32),
         (3, 179), (4, 36), (4, 232)]

setzt = collections.defaultdict(list)
prueft = collections.defaultdict(list)
n = 0
for p in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x48:
        continue
    n += 1
    room = os.path.basename(p).split(".")[0]
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec in (ms, ss):
        if sec == 0 or sec >= len(d):
            continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            for (pc, op, sz) in walk_ops(d, sec + o, sec + e):
                if op == 0x22:
                    setzt[(d[pc + 1], d[pc + 2])].append((room, pc, d[pc + 3]))
                elif op == 0x21:
                    prueft[(d[pc + 1], d[pc + 2])].append((room, pc, d[pc + 3]))

print("Vollzensus ueber %d RDTs" % n)
for k in FRAGE:
    s, c = setzt[k], prueft[k]
    print("\nflag(%d,%d): %d Set / %d Ck" % (k[0], k[1], len(s), len(c)))
    for (r, o, v) in s:
        print("   Set %s @0x%05X val=%d" % (r, o, v))
    for (r, o, v) in c:
        print("   Ck  %s @0x%05X val=%d" % (r, o, v))
