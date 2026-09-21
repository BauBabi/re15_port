#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Does the brute MD1 scan re-find every prop the RDT pointer table declares?"""
import json, sys, collections
props = json.load(open(sys.argv[1]))
scan = json.load(open(sys.argv[2]))
have = set((r['file'].upper(), r['off']) for r in scan)
miss = []
found = 0
for p in props:
    if not p['ok']:
        continue
    key = ('PL0/RDT/%s.RDT' % p['room'], p['md1_off'])
    if key in have:
        found += 1
    else:
        miss.append(key)
print('declared props with geometry : %d' % sum(1 for p in props if p['ok']))
print('re-found by the brute scan   : %d' % found)
print('missed                       : %d' % len(miss))
for m in miss[:20]:
    print('   ', m)
print()
byfile = collections.Counter()
for r in scan:
    f = r['file']
    if f.upper().startswith('PL0/RDT/') and f.upper().endswith('.RDT'):
        byfile['(RDT room files)'] += 1
    else:
        byfile[f] += 1
print('--- MD1 hits outside the RDTs, by file')
for f, n in byfile.most_common(80):
    print('  %-52s %d' % (f, n))
