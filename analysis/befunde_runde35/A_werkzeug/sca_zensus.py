# -*- coding: utf-8 -*-
"""Runde 35 Spur A (Nachbesserung 1) — Zensus der SCA-Zellfelder aller Raeume.

    python sca_zensus.py <baum>                 Verteilung typ / u0 / u1 / floor-Nibbles
    python sca_zensus.py <baum> <rid hex>       alle Zellen eines Raums

Zelle (12 B @ RDT[+0x20]+24): u16 w, u16 d, s16 x, s16 z, u8 typ, u8 u0, u8 u1, u8 floor.
"""
import collections, glob, os, struct, sys

def read_sca(p):
    d = open(p, 'rb').read()
    if len(d) < 0x24: return []
    s = struct.unpack_from('<I', d, 0x20)[0]
    if s == 0 or s + 24 > len(d): return []
    counts = struct.unpack_from('<5I', d, s + 4); n = sum(counts)
    if n > 2000: return []
    sca = []
    q = 0; acc = counts[0]
    for i in range(n):
        while i >= acc and q < 4:
            q += 1; acc += counts[q]
        if s + 24 + 12 * i + 12 > len(d): break
        w, dep, x, z = struct.unpack_from('<HHhh', d, s + 24 + 12 * i)
        typ, u0, u1, flr = struct.unpack_from('<BBBB', d, s + 24 + 12 * i + 8)
        sca.append(dict(i=i, q=q, x=x, z=z, w=w, d=dep, typ=typ, u0=u0, u1=u1, floor=flr))
    return sca

def main():
    root = sys.argv[1]
    base = os.path.join(root, 're15_port', 'shared_assets', 'PSX')
    if len(sys.argv) > 2:
        rid = int(sys.argv[2], 16)
        p = os.path.join(base, 'STAGE%d' % (rid >> 12), 'ROOM%04X.RDT' % rid)
        for c in read_sca(p):
            print('#%-3d q%d typ%-2d u0=%02x u1=%02x floor=%02x x[%d..%d] z[%d..%d] w%d d%d' % (
                c['i'], c['q'], c['typ'], c['u0'], c['u1'], c['floor'], c['x'], c['x'] + c['w'],
                c['z'], c['z'] + c['d'], c['w'], c['d']))
        return
    seen = set(); cells = []
    for p in sorted(glob.glob(os.path.join(base, 'STAGE*', 'ROOM*.RDT'))):
        room = os.path.basename(p)[4:8]
        for c in read_sca(p):
            key = (room, c['x'], c['z'], c['w'], c['d'], c['typ'], c['u0'], c['u1'], c['floor'])
            if key in seen: continue
            seen.add(key); c['room'] = room; cells.append(c)
    print('eindeutige Zellen: %d' % len(cells))
    for name, fn in (('typ', lambda c: c['typ']), ('u0', lambda c: c['u0']), ('u1', lambda c: c['u1']),
                     ('floor>>4 (Band)', lambda c: c['floor'] >> 4), ('floor&0xf', lambda c: c['floor'] & 15)):
        cnt = collections.Counter(fn(c) for c in cells)
        print('%s: %s' % (name, ' '.join('%x:%d' % (k, v) for k, v in sorted(cnt.items()))))
    sol = [c for c in cells if c['u0'] & 1]
    print('u0&1: %d' % len(sol))
    cnt = collections.Counter((c['floor'] & 15) for c in sol)
    print('u0&1 floor&0xf: %s' % ' '.join('%x:%d' % (k, v) for k, v in sorted(cnt.items())))
    cnt = collections.Counter((c['u0']) for c in sol)
    print('u0&1 u0: %s' % ' '.join('%02x:%d' % (k, v) for k, v in sorted(cnt.items())))
    cnt = collections.Counter((c['floor'] & 15, c['u0']) for c in sol)
    print('u0&1 (floor&0xf,u0): %s' % ' '.join('%x/%02x:%d' % (k[0], k[1], v) for k, v in sorted(cnt.items())))

if __name__ == '__main__':
    main()
