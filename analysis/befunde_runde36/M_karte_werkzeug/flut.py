# Flutfuellung im begehbaren Komplement (Port-Modell re15_collision.c: Zellen mit (floor>>4)==band
# und u0&1 sind WAENDE, Spielerradius 450 = DAT_80073e94[6]). Aufruf: flut.py <tools> <raum> <band> x,z x,z ...
import sys, collections
sys.path.insert(0, sys.argv[1])
import gen_map_zones as G
import numpy as np
rid = int(sys.argv[2], 16); band = int(sys.argv[3])
pts = [tuple(map(int, p.split(','))) for p in sys.argv[4:]]
sca = G.read_rdt(rid)[0]
cells = [c for c in sca if (c[4] >> 4) == band]
walls = [c for c in cells if (c[6] & 1)]
xs = [c[0] for c in cells] + [c[0] + c[2] for c in cells]
zs = [c[1] for c in cells] + [c[1] + c[3] for c in cells]
x0, x1, z0, z1 = min(xs) - 2000, max(xs) + 2000, min(zs) - 2000, max(zs) + 2000
S = 100; R = 450
nx, nz = (x1 - x0) // S + 1, (z1 - z0) // S + 1
solid = np.zeros((nz, nx), bool)
for (x, z, w, d, flr, typ, u0) in walls:
    a = max(0, (x - R - x0) // S); b = min(nx - 1, (x + w + R - x0) // S)
    c = max(0, (z - R - z0) // S); e = min(nz - 1, (z + d + R - z0) // S)
    solid[c:e + 1, a:b + 1] = True
print("ROOM%04X band %d: %d Zellen, %d Waende; Raster %dx%d" % (rid, band, len(cells), len(walls), nx, nz))
def flood(p):
    sx, sz = (p[0] - x0) // S, (p[1] - z0) // S
    if solid[sz, sx]: return None
    seen = np.zeros_like(solid); q = collections.deque([(sz, sx)]); seen[sz, sx] = True
    while q:
        z, x = q.popleft()
        for dz, dx in ((1,0),(-1,0),(0,1),(0,-1)):
            a, b = z + dz, x + dx
            if 0 <= a < nz and 0 <= b < nx and not seen[a, b] and not solid[a, b]:
                seen[a, b] = True; q.append((a, b))
    return seen
regs = []
for p in pts:
    s = flood(p)
    if s is None: print("  Punkt", p, "liegt IN einer Wand"); regs.append(None); continue
    zz, xx = np.nonzero(s)
    out_edge = (xx.min() == 0 or zz.min() == 0 or xx.max() == nx - 1 or zz.max() == nz - 1)
    print("  Punkt", p, "-> Bereich x %d..%d z %d..%d, %d Felder%s" % (x0 + xx.min() * S, x0 + xx.max() * S, z0 + zz.min() * S, z0 + zz.max() * S, len(xx), " (LECKT an den Rand)" if out_edge else ""))
    regs.append(s)
for i in range(len(pts)):
    for j in range(i + 1, len(pts)):
        if regs[i] is None or regs[j] is None: continue
        sx, sz = (pts[j][0] - x0) // S, (pts[j][1] - z0) // S
        print("  %s <-> %s verbunden: %s" % (pts[i], pts[j], bool(regs[i][sz, sx])))
