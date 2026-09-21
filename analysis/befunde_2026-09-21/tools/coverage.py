#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Did the brute MD1 scan find geometry in every known model container?"""
import os, json, sys, collections
ROOT = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\re2leon"
scan = json.load(open(sys.argv[1]))
hit = collections.Counter()
for r in scan:
    hit[r['file'].upper()] += 1
EXT = ('.EMD', '.PLD', '.PLW', '.EMS', '.DO2', '.MD1')
tot = collections.Counter()
miss = collections.Counter()
missing = []
for dirpath, dirnames, filenames in os.walk(ROOT):
    for fn in filenames:
        e = os.path.splitext(fn)[1].upper()
        if e not in EXT:
            continue
        rel = os.path.relpath(os.path.join(dirpath, fn), ROOT).replace('\\', '/').upper()
        tot[e] += 1
        if hit[rel] == 0:
            miss[e] += 1
            missing.append(rel)
print('container coverage (files that yielded >=1 MD1 hit):')
for e in EXT:
    print('  %-6s total %4d   without a hit %4d' % (e, tot[e], miss[e]))
print()
for m in missing[:60]:
    print('  MISS', m)
