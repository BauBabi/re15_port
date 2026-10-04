# -*- coding: utf-8 -*-
"""Runde 35 Spur A — Flugbahn der Granate gegen die SCA-Zellen (Waende) eines Raums pruefen.

Liest ROOM<id>.RDT (SCA @ +0x20, 12 B je Zelle: u16 w, u16 d, s16 x, s16 z, u8 typ, u8 u0,
u8 u1, u8 floor — Layout wie re15_port/tools/gen_map_zones.py read_rdt) und ein RE15_GRANATE_LOG
(Zeilen "T=.. F=.. slot=.. ... wpos=(x,y,z) ..."). Gibt je Granatenbild aus, ob wpos in einer
SOLIDEN Zelle liegt (typ&0xf == 1 Rechteck und u0&1), und die Bbox aller Zellen des Raums.

    python sca_pfad.py <baum> <rid hex> <gr.log> [slot]
"""
import os, re, struct, sys

def read_sca(root, rid):
    stage = rid >> 12
    p = os.path.join(root, 're15_port', 'shared_assets', 'PSX', 'STAGE%d' % stage, 'ROOM%04X.RDT' % rid)
    d = open(p, 'rb').read()
    s = struct.unpack_from('<I', d, 0x20)[0]
    counts = struct.unpack_from('<5I', d, s + 4); n = sum(counts)
    sca = []
    for i in range(n):
        w, dep, x, z = struct.unpack_from('<HHhh', d, s + 24 + 12 * i)
        typ, u0, u1, flr = struct.unpack_from('<BBBB', d, s + 24 + 12 * i + 8)
        sca.append(dict(x=x, z=z, w=w, d=dep, typ=typ, u0=u0, u1=u1, floor=flr))
    return sca

def in_cell(c, x, z):
    t = c['typ'] & 0x0f
    if t == 1:   # Rechteck
        return c['x'] <= x < c['x'] + c['w'] and c['z'] <= z < c['z'] + c['d']
    if t == 2:   # Kreis/Ellipse (Mitte x+w/2, z+d/2)
        cx, cz = c['x'] + c['w'] / 2.0, c['z'] + c['d'] / 2.0
        rx, rz = c['w'] / 2.0, c['d'] / 2.0
        if rx <= 0 or rz <= 0: return False
        return ((x - cx) / rx) ** 2 + ((z - cz) / rz) ** 2 <= 1.0
    return c['x'] <= x < c['x'] + c['w'] and c['z'] <= z < c['z'] + c['d']

def main():
    root, rid, log = sys.argv[1], int(sys.argv[2], 16), sys.argv[3]
    slot = int(sys.argv[4]) if len(sys.argv) > 4 else None
    sca = read_sca(root, rid)
    xs = [c['x'] for c in sca] + [c['x'] + c['w'] for c in sca]
    zs = [c['z'] for c in sca] + [c['z'] + c['d'] for c in sca]
    print('ROOM%04X: %d SCA-Zellen, Bbox x[%d..%d] z[%d..%d]' % (rid, len(sca), min(xs), max(xs), min(zs), max(zs)))
    solid = [c for c in sca if (c['typ'] & 0x0f) == 1 and (c['u0'] & 1)]
    print('  solide Rechtecke (typ&0xf==1, u0&1): %d' % len(solid))
    rx = re.compile(r'^T=(\d+) F=(\d+) slot=(\d+) .* wpos=\((-?\d+),(-?\d+),(-?\d+)\)')
    first_wall = None
    n = 0
    for line in open(log, encoding='utf-8', errors='replace'):
        m = rx.match(line)
        if not m: continue
        T, F, s, x, y, z = [int(v) for v in m.groups()]
        if slot is not None and s != slot: continue
        n += 1
        hits = [c for c in sca if in_cell(c, x, z)]
        sol = [c for c in hits if (c['typ'] & 0x0f) == 1 and (c['u0'] & 1)]
        tag = 'IN-WAND' if sol else ('zelle' if hits else 'frei')
        outside = not (min(xs) <= x <= max(xs) and min(zs) <= z <= max(zs))
        if outside: tag += ' AUSSERHALB-BBOX'
        if (sol or outside) and first_wall is None:
            first_wall = (T, F, x, y, z, tag)
        print('  T=%d F=%d wpos=(%d,%d,%d) %s%s' % (T, F, x, y, z, tag,
              ' ' + ','.join('[x%d z%d w%d d%d typ%02x u0%02x]' % (c['x'], c['z'], c['w'], c['d'], c['typ'], c['u0']) for c in sol) if sol else ''))
    print('Granatenbilder: %d' % n)
    if first_wall:
        print('ERSTE Wand/Aussen-Beruehrung: T=%d F=%d wpos=(%d,%d,%d) %s' % first_wall)
    else:
        print('keine Wand-/Aussen-Beruehrung')

if __name__ == '__main__':
    main()
