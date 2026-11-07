# Zustandslog in Segmente (je Ruecksprung des Bildzaehlers) teilen; je Segment das letzte Bild mit allen Slots.
import re, sys
path = sys.argv[1]
segs = []; prev = -1
for l in open(path, errors='replace'):
    m = re.match(r'F(\d+) ', l)
    if not m: continue
    f = int(m.group(1))
    if f < prev or not segs: segs.append([])
    prev = f
    segs[-1].append((f, l))
for i, s in enumerate(segs):
    f, l = s[-1]
    pl = re.search(r'PL\(([^)]*)\)', l)
    acts = re.findall(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ([^\]]*)\] hp=(-?\d+)', l)
    print('Segment %d: F%d..F%d PL(%s)' % (i, s[0][0], f, pl.group(1) if pl else '?'))
    for sl, t, st, rest, hp in acts:
        pos = re.search(r'@\((-?\d+),(-?\d+)', rest)
        print('   slot %2s t=%s st=%s hp=%s pos=%s' % (sl, t, st, hp, pos.groups() if pos else '?'))
