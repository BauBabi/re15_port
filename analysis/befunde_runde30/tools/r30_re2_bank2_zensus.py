#!/usr/bin/env python3
"""Zensus: welche RE2-Raeume (info/re2leon/PL0/RDT/ROOM*.RDT, 250 Stueck) belegen in ihrer
Raumbank (Se_on-Bank 2) die Saetze, die der EXE-Tuer-Handler @0x80051514 ruft:
0x16 (verschlossen, @0x80051610/@0x800516a4), 0x25 (Schluessel benutzt, @0x80051658),
0x26 (entriegelt, @0x800515f8). Ausgabe je Satz: Anzahl Raeume, verschiedene Wellen (sha1)."""
import sys, os, glob, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_re2_roombank import load_bank, resolve
ids = [int(x, 16) for x in sys.argv[1:]] or [0x16, 0x25, 0x26]
files = sorted(glob.glob('info/re2leon/PL0/RDT/ROOM*.RDT'))
print('# %d RDTs' % len(files))
nobank = []
edtn = collections.Counter()
for sid in ids:
    waves = collections.defaultdict(list)
    empty = []; short = []
    for f in files:
        b = load_bank(f)
        name = os.path.basename(f)[4:8]
        if b is None:
            if sid == ids[0]: nobank.append(name)
            continue
        if sid == ids[0]: edtn[b['n']] += 1
        r = resolve(b, sid)
        if r is None: short.append(name); continue
        if r['empty']: empty.append(name); continue
        key = tuple((L.get('sha1', '?'), L.get('vag_size', -1), L['vol'], L['center'], L['shift'], L['mn']) for L in r['layers'])
        waves[key].append((name, r['raw'].hex(' ')))
    print('== Satz 0x%02x: belegt in %d Raeumen, leer in %d, EDT zu kurz in %d' % (sid, sum(len(v) for v in waves.values()), len(empty), len(short)))
    for key, rooms in sorted(waves.items(), key=lambda kv: -len(kv[1])):
        desc = ' + '.join('sha1 %s %dB vol%d center%d shift%d note%d' % (k[0][:12], k[1], k[2], k[3], k[4], k[5]) for k in key)
        raws = collections.Counter(r[1] for r in rooms)
        print('   %3d Raeume  %s' % (len(rooms), desc))
        print('        EDT-Rohsaetze: %s' % ', '.join('%s x%d' % kv for kv in raws.items()))
        print('        Raeume: %s' % ' '.join(r[0] for r in rooms))
    if empty: print('   LEER: %s' % ' '.join(empty))
print('# ohne Raumbank: %s' % ' '.join(nobank))
print('# EDT-Satzzahlen: %s' % dict(edtn))
