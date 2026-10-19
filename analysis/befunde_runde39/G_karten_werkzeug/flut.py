"""Flutfuellung der begehbaren Flaeche ab einem Saatpunkt (Raum, Baender, Saat):
flut.py <hexraum> <band[,band]> <x,z> [bild.png]  -> Bbox, Zeilen-Laeufe, 'LECKT' wenn offen.
Waende = SCA typ&0x0f==1 und u0&1 und floor>>4 im Bandsatz (wie frei.py)."""
import sys, os, collections
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'tools'))
import gen_map_zones as GZ
S = 100
def flut(rid, bands, seed, typen=(1,)):
    sca = GZ.read_rdt(rid)[0]
    walls = [c for c in sca if (c[4] >> 4) in bands and (c[6] & 1) and (c[5] & 0x0f) in typen]
    xs = [c[0] for c in sca] + [c[0] + c[2] for c in sca]; zs = [c[1] for c in sca] + [c[1] + c[3] for c in sca]
    x0, x1, z0, z1 = min(xs) - 2000, max(xs) + 2000, min(zs) - 2000, max(zs) + 2000
    nx, nz = (x1 - x0) // S + 1, (z1 - z0) // S + 1
    solid = np.zeros((nz, nx), bool)
    for (x, z, w, d, flr, typ, u0) in walls:
        a = max(0, (x - x0) // S); b = min(nx - 1, (x + w - x0 - 1) // S)
        c = max(0, (z - z0) // S); e = min(nz - 1, (z + d - z0 - 1) // S)
        solid[c:e + 1, a:b + 1] = True
    sx, sz = (seed[0] - x0) // S, (seed[1] - z0) // S
    if solid[sz, sx]: return None
    seen = np.zeros_like(solid); q = collections.deque([(sz, sx)]); seen[sz, sx] = True
    while q:
        z, x = q.popleft()
        for dz, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            a, b = z + dz, x + dx
            if 0 <= a < nz and 0 <= b < nx and not seen[a, b] and not solid[a, b]:
                seen[a, b] = True; q.append((a, b))
    zz, xx = np.nonzero(seen)
    leck = xx.min() == 0 or zz.min() == 0 or xx.max() == nx - 1 or zz.max() == nz - 1
    return dict(x0=x0, z0=z0, nx=nx, nz=nz, solid=solid, seen=seen, leck=leck,
                bx=(x0 + xx.min() * S, x0 + xx.max() * S + S), bz=(z0 + zz.min() * S, z0 + zz.max() * S + S))
if __name__ == '__main__':
    rid = int(sys.argv[1], 16); bands = [int(b) for b in sys.argv[2].split(',')]
    seed = tuple(map(int, sys.argv[3].split(',')))
    f = flut(rid, bands, seed)
    if f is None: print('Saat liegt in einer Wand'); sys.exit(1)
    print(f"ROOM{rid:04X} Baender {bands} Saat {seed}: Bbox x {f['bx'][0]}..{f['bx'][1]} z {f['bz'][0]}..{f['bz'][1]} {'LECKT' if f['leck'] else 'geschlossen'}")
    prev = None
    for zi in range(f['nz'] - 1, -1, -1):
        xi = np.nonzero(f['seen'][zi])[0]
        sp = (f['x0'] + xi.min() * S, f['x0'] + xi.max() * S + S) if len(xi) else None
        if sp != prev:
            if sp: print('   ab z %6d abwaerts: x %6d..%6d' % (f['z0'] + zi * S + S, sp[0], sp[1]))
            prev = sp
    if len(sys.argv) > 4:
        from PIL import Image
        img = np.zeros((f['nz'], f['nx'], 3), np.uint8); img[f['solid']] = (120, 120, 120); img[f['seen']] = (255, 200, 0)
        Image.fromarray(img[::-1]).resize((f['nx'] * 2, f['nz'] * 2), Image.NEAREST).save(sys.argv[4])
