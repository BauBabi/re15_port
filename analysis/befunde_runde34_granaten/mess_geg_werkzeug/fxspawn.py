# fxspawn.py <laufdir> [ab_bild] : ESP-Spawns (wf.log "SPAWN id=.. sub=..") nach dem Sprung, je Bild; SE-Zeilen dazu.
import re, sys, os, collections
d = sys.argv[1]; ab = int(sys.argv[2]) if len(sys.argv) > 2 else 0
L = open(os.path.join(d, 'wf.log'), errors='replace').read().splitlines()
prev = -1; nach = False; f = None
per = collections.OrderedDict()
for l in L:
    m = re.match(r'F(\d+) ', l)
    if m:
        n = int(m.group(1))
        if not nach and prev > n: nach = True
        prev = n; f = n; continue
    if not nach or f is None or f < ab: continue
    m = re.search(r'SPAWN id=(\d+) sub=(\d+) scale=(0x[0-9a-f]+)', l)
    if m: per.setdefault(f, []).append('fx%s/%s' % (m.group(1), m.group(2)))
    m = re.search(r'SE  (\w+) code=(0x[0-9a-f]+)', l)
    if m: per.setdefault(f, []).append('SE %s %s' % (m.group(1), m.group(2)))
tot = collections.Counter()
for k, v in per.items():
    tot.update(x for x in v if x.startswith('fx'))
    print('F%d: %s' % (k, ' '.join(v)))
print('Summe:', dict(tot))
