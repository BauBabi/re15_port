import sys, os
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/nutzer_batch_2026-08-27/tools")
from re2_scd_walk import all_subs, walk
from collections import defaultdict
rooms = defaultdict(set)
for fn,d,name,base,i,s,e,last in all_subs():
    ops,st = walk(d,s,e,last)
    for (a,op,ln) in ops:
        if op == 0x44:
            typ = d[a+3]
            rooms[fn].add(typ)
for fn in sorted(rooms):
    t = rooms[fn]
    if t & {0x47,0x49,0x48,0x46,0x4a,0x4b,0x4c,0x4d,0x4e}:
        print(fn, " ".join("%02X"%x for x in sorted(t)))
