# Zeilen + Zellen fuer die NUTZERWUNSCH-Schemata 10B0 (Blatt 2) und 1260 (Blatt 0). Nur Messhilfe/Generator.
import sys
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/tools')
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r4')
import kartlib as K, geh
def f(v, s): return ((((v + 32000) * 10 * s) >> 20) + 5) // 10
def proj(x, z, r): ox, oy, sx, sy = r; return f(x, sx) + ox, -f(z, sy) + oy
def zeile(x0, z0, x1, z1, bx, by, s):
    # Westkante x0 -> bx, Nordkante z1 -> by
    return (bx - f(x0, s), by + f(z1, s), s, s)
def maske(pts, r):
    m = set()
    for x, z in pts: m.add(proj(x, z, r))
    return m
def zellen(m):
    # Zeilenlaeufe, dann senkrecht gleiche Laeufe zusammenfassen
    rows = {}
    for x, y in m: rows.setdefault(y, set()).add(x)
    laeufe = []
    for y in sorted(rows):
        xs = sorted(rows[y]); a = xs[0]; p = a
        for x in xs[1:] + [None]:
            if x is not None and x == p + 1: p = x; continue
            laeufe.append((y, a, p)); 
            if x is not None: a = p = x
    offen = {}; fertig = []
    for y, a, b in laeufe:
        k = (a, b)
        if k in offen and offen[k][1] == y - 1: offen[k][1] = y
        else:
            if k in offen: fertig.append((a, offen[k][0], b, offen[k][1]))
            offen[k] = [y, y]
    for (a, b), (y0, y1) in offen.items(): fertig.append((a, y0, b, y1))
    return fertig
if sys.argv[1] == '10B0':
    p0, _ = geh.flaeche(0x10B0, [(-20250, -6700)], step=200, band=0)
    p1, _ = geh.flaeche(0x10B0, [(0, -6000)], step=200, band=1)
    p5, _ = geh.flaeche(0x10B0, [(4014, -7436)], step=200, band=5)
    pts = sorted(set(p0) | set(p1) | set(p5))
    for n, p in (('b0', p0), ('b1', p1), ('b5', p5)):
        xs = [a for a, b in p]; zs = [b for a, b in p]; print(n, len(p), min(xs), max(xs), min(zs), max(zs))
    s = int(sys.argv[2]); bx, by = int(sys.argv[3]), int(sys.argv[4])
else:
    s_, d, _ = K.read_rdt(0x1260)
    x0 = min(c[0] for c in s_); x1 = max(c[0] + c[2] for c in s_); z0 = min(c[1] for c in s_); z1 = max(c[1] + c[3] for c in s_)
    pts = [(x, z) for x in range(x0, x1 + 1, 200) for z in range(z0, z1 + 1, 200)]
    s = int(sys.argv[2]); bx, by = int(sys.argv[3]), int(sys.argv[4])
xs = [a for a, b in pts]; zs = [b for a, b in pts]
X0, X1, Z0, Z1 = min(xs), max(xs), min(zs), max(zs)
r = zeile(X0, Z0, X1, Z1, bx, by, s)
print('welt x %d..%d z %d..%d -> zeile' % (X0, X1, Z0, Z1), r, 'ecken', proj(X0, Z1, r), proj(X1, Z0, r))
m = maske(pts, r)
z = zellen(m)
print(len(z), 'zellen (x0,y0,x1,y1 inkl):')
for c in sorted(z, key=lambda c: (c[1], c[0])): print('   ', c, 'w', c[2] - c[0] + 1, 'h', c[3] - c[1] + 1)
