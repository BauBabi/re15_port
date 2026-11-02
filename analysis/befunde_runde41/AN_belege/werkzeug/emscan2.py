import sys, os
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/nutzer_batch_2026-08-27/tools")
from re2_scd_walk import all_subs, walk
from collections import defaultdict
rooms = defaultdict(set)
for fn,d,name,base,i,s,e,last in all_subs():
    ops,st = walk(d,s,e,last)
    for (a,op,ln) in ops:
        if op == 0x44:
            rooms[fn].add(d[a+3])
for fn in sorted(rooms):
    if fn[4] in "567" or fn.startswith("ROOM6") or fn.startswith("ROOM7"):
        print(fn, " ".join("%02X"%x for x in sorted(rooms[fn])))
