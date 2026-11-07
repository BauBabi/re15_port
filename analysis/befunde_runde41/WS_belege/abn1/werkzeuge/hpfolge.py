# HP-Folge eines Slots im Zustandslog (Segment = Raum nach dem Sprung, optional Segmentnummer), nur Aenderungen von Typ/Zustand/HP.
import re, sys
path = sys.argv[1]; slot = sys.argv[2]; segwahl = int(sys.argv[3]) if len(sys.argv) > 3 else None
segs = []; prev = -1
for l in open(path, errors='replace'):
    m = re.match(r'F(\d+) ', l)
    if not m: continue
    f = int(m.group(1))
    if f < prev or not segs: segs.append([])
    prev = f
    segs[-1].append((f, l))
for i, s in enumerate(segs):
    if segwahl is not None and i != segwahl: continue
    if i == 0 and segwahl is None: continue
    alt = None
    for f, l in s:
        m = re.search(r'\[' + slot + r' t=([0-9a-f]+) st=(\d+) ss1=(\d+) [^\]]*?mo=(\d+) [^\]]*\] hp=(-?\d+)', l)
        pl = re.search(r'PL\(([^)]*)\)', l)
        val = (m.group(1), m.group(2), m.group(5)) if m else None
        if val != alt:
            if m: print('seg%d F%d slot %s t=%s st=%s ss1=%s mo=%s hp=%s  PL(%s)' % (i, f, slot, m.group(1), m.group(2), m.group(3), m.group(4), m.group(5), pl.group(1) if pl else '?'))
            else: print('seg%d F%d slot %s fehlt' % (i, f, slot))
            alt = val
