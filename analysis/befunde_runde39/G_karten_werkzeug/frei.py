"""Begehbare Flaeche je Raum und Band: Komplement der WAND-Zellen (SCA typ&0x0f==1, u0&1 solide,
floor>>4 == Band; re15_collision.c Kopfkommentar, Memory reai-v2-kollisionszellen-sind-waende),
4er-Flutfuellung im 100er-Raster, nur geschlossene Bereiche (nicht zum Rand leckend).
frei.py <hexraum> <band> [min_n]  -> Komponenten mit Bbox und Zeilen-Laeufen"""
import sys, os, collections
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'tools'))
import gen_map_zones as GZ
S = 100
def frei(rid, band, R=0):
    sca = GZ.read_rdt(rid)[0]
    walls = [c for c in sca if (c[4] >> 4) == band and (c[6] & 1) and (c[5] & 0x0f) == 1]
    xs = [c[0] for c in sca] + [c[0] + c[2] for c in sca]; zs = [c[1] for c in sca] + [c[1] + c[3] for c in sca]
    x0, x1, z0, z1 = min(xs) - 2000, max(xs) + 2000, min(zs) - 2000, max(zs) + 2000
    nx, nz = (x1 - x0) // S + 1, (z1 - z0) // S + 1
    solid = np.zeros((nz, nx), bool)
    for (x, z, w, d, flr, typ, u0) in walls:
        a = max(0, (x - R - x0) // S); b = min(nx - 1, (x + w + R - x0 - 1) // S)
        c = max(0, (z - R - z0) // S); e = min(nz - 1, (z + d + R - z0 - 1) // S)
        solid[c:e + 1, a:b + 1] = True
    lab = np.zeros((nz, nx), np.int32); k = 0; comps = []
    for sz in range(nz):
        for sx in range(nx):
            if solid[sz, sx] or lab[sz, sx]: continue
            k += 1; q = collections.deque([(sz, sx)]); lab[sz, sx] = k; pts = [(sz, sx)]
            while q:
                z, x = q.popleft()
                for dz, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    a, b = z + dz, x + dx
                    if 0 <= a < nz and 0 <= b < nx and not lab[a, b] and not solid[a, b]:
                        lab[a, b] = k; q.append((a, b)); pts.append((a, b))
            P = np.array(pts)
            leck = (P[:, 1].min() == 0 or P[:, 0].min() == 0 or P[:, 1].max() == nx - 1 or P[:, 0].max() == nz - 1)
            comps.append(dict(k=k, n=len(pts), leck=leck, pts=P,
                              bx=(x0 + P[:, 1].min() * S, x0 + P[:, 1].max() * S + S),
                              bz=(z0 + P[:, 0].min() * S, z0 + P[:, 0].max() * S + S)))
    return dict(x0=x0, z0=z0, nx=nx, nz=nz, lab=lab, solid=solid, walls=walls,
                comps=[c for c in comps if not c['leck']])
def laeufe(f, c, schritt=500):
    """Zeilenweise x-Spannen (je 'schritt' Einheiten in z), von +z nach -z."""
    m = np.zeros((f['nz'], f['nx']), bool); m[c['pts'][:, 0], c['pts'][:, 1]] = True
    k = schritt // S; out = []; prev = None; start = None
    for zi in range(f['nz'] - 1, -1, -1):
        xi = np.nonzero(m[zi])[0]
        sp = (f['x0'] + xi.min() * S, f['x0'] + xi.max() * S + S) if len(xi) else None
        if sp != prev:
            if prev is not None: out.append((f['z0'] + zi * S + S, start, prev))
            prev = sp; start = f['z0'] + zi * S + S
    if prev is not None: out.append((f['z0'], start, prev))
    return out
if __name__ == '__main__':
    rid = int(sys.argv[1], 16); band = int(sys.argv[2]); mn = int(sys.argv[3]) if len(sys.argv) > 3 else 100
    f = frei(rid, band)
    print(f'ROOM{rid:04X} Band {band}: {len(f["walls"])} Wandzellen')
    for c in sorted(f['comps'], key=lambda c: -c['n']):
        if c['n'] < mn: continue
        print(f"  Komp n={c['n']} x {c['bx'][0]}..{c['bx'][1]} z {c['bz'][0]}..{c['bz'][1]}")
        for (zu, zo, sp) in laeufe(f, c):
            print(f"     z {zu:6d}..{zo:6d}: x {sp[0]}..{sp[1]}")
