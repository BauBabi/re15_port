"""wb1_fxlog.py - Spur W, Nachbesserung 1: wertet ein RE15_FX_LOG (platform/pc/main.c pc_draw_effects, Zeile nach dem
Regions-Test) fuer Effekt 6 Sub 0 aus: je Tropfpunkt (x,z) erstes/letztes Spielbild, Spanne der Weltlage y, Routinen A, Flags.
Aufruf: python wb1_fxlog.py <fx.log>"""
import collections, re, sys
pat = re.compile(r"id=(\d+) sub=(\d+) .* xlat=\((-?\d+),(-?\d+),(-?\d+)\) drift=\((-?\d+),(-?\d+),(-?\d+)\) .* wpos=\((-?\d+),(-?\d+),(-?\d+)\) A=(\d+) B=(\d+) .* fl=(\w+) .* F=(\d+)")
pts = collections.defaultdict(list)
for l in open(sys.argv[1]):
    m = pat.search(l)
    if not m or m.group(1) != "6" or m.group(2) != "0": continue
    g = m.groups()
    x, y, z = int(g[8]), int(g[9]), int(g[10])
    if (x, z) == (0, 0): continue                     # Bild 0: Weltlage noch nicht gerechnet
    pts[(x, z)].append((int(g[14]), y, int(g[11]), g[13], int(g[3]), int(g[6])))
for k, v in sorted(pts.items(), key=lambda kv: kv[1][0][0]):
    ys = [e[1] for e in v]
    print("Punkt (x,z)=%s F%d..F%d Zeilen %d  Weltlage y %d..%d  A %s  Flags %s  xlat.y %d..%d  drift.y %d..%d" % (
        k, v[0][0], v[-1][0], len(v), min(ys), max(ys), sorted(set(e[2] for e in v)), sorted(set(e[3] for e in v)),
        min(e[4] for e in v), max(e[4] for e in v), min(e[5] for e in v), max(e[5] for e in v)))
