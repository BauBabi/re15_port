# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: Zensus der RE1.5-Item-WELTMODELLE.

Fuer jeden Item_aot_set (0x50) mit Prop != 0xFF wird der Obj_model_set (0x2D) desselben
Raums mit derselben obj_id gesucht und ausgegeben:
   Item-Id/Name, Prop-Lage (x,y,z), Drehung, MD1-bbox, AOT-Rechteck,
   Lage des Props RELATIV zum Rechteck (liegt das Modell im Kasten? wie weit vom Rand?)
Daraus folgt, wie RE1.5 einen Aufhebe-Kasten um ein liegendes Modell legt.
"""
import os, sys, glob, struct, collections

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as W
import rdt_objekt_ansicht as RO

CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
NAMEN = {}


def namen_laden():
    d = open(os.path.join(CD, 'BIN', 'DEBUG.BIN'), 'rb').read()
    NT = 0x495C; NB = 0x4A28
    for i in range(102):
        o = struct.unpack_from('<H', d, NT + 2 * i)[0]
        p = NB + o; s = ''
        while d[p] != 0x07 and len(s) < 40:
            c = d[p]; s += ' ' if c == 0 else (chr(c + 0x24) if 0x0c <= c <= 0x5f else '?'); p += 1
        NAMEN[i] = s


def md1_bbox(m):
    if len(m) < 12 + 56:
        return None
    n = struct.unpack_from('<I', m, 8)[0] // 2
    lo = [1 << 30] * 3; hi = [-(1 << 30)] * 3; nt = nq = 0
    for i in range(n):
        h = struct.unpack_from('<14I', m, 12 + 56 * i)
        for (vo, vc, fc) in ((h[0], h[1], h[5]), (h[7], h[8], h[12])):
            if fc == 0:
                continue
            for k in range(vc):
                o = 12 + vo + 8 * k
                if o + 6 > len(m):
                    break
                v = struct.unpack_from('<3h', m, o)
                for a in range(3):
                    lo[a] = min(lo[a], v[a]); hi[a] = max(hi[a], v[a])
        nt += h[5]; nq += h[12]
    if lo[0] > hi[0]:
        return None
    return lo, hi, nt, nq


def main():
    namen_laden()
    zeilen = []
    for st in range(1, 7):
        for p in sorted(glob.glob(os.path.join(CD, 'STAGE%d' % st, 'ROOM*.RDT'))):
            d = open(p, 'rb').read()
            if len(d) < 0x60:
                continue
            raum = os.path.basename(p)[4:8]
            reg = W.regionen(d)
            objs = {}
            items = []
            for (tag, idx), liste in sorted(reg.items()):
                for (pc, op, sz) in liste:
                    if op == 0x2D:
                        oid = d[pc + 1]
                        objs.setdefault(oid, []).append(dict(
                            off=pc, typ=d[pc + 2], band=d[pc + 4], eltern=d[pc + 5],
                            pos=struct.unpack_from('<3h', d, pc + 10), rot=struct.unpack_from('<3h', d, pc + 16),
                            box=struct.unpack_from('<6h', d, pc + 22)))
                    elif op == 0x50:
                        lang = (d[pc + 3] & 0x80) != 0
                        o = 8 if lang else 0
                        items.append(dict(off=pc, slot=d[pc + 1], sat=d[pc + 3],
                                          rect=struct.unpack_from('<4h', d, pc + 6),
                                          item=W.u16(d, pc + 14 + o), menge=W.u16(d, pc + 16 + o),
                                          bit=W.u16(d, pc + 18 + o), prop=d[pc + 20 + o]))
            if not items:
                continue
            props = RO.props_lesen(d)
            for it in items:
                if it['prop'] == 0xFF:
                    continue
                oid = it['prop']
                ob = objs.get(oid)
                bb = md1_bbox(props[oid][1]) if oid < len(props) else None
                zeilen.append((raum, it, ob, bb))
    print('Item-Platzierungen MIT Weltmodell: %d' % len(zeilen))
    rel = []
    for raum, it, ob, bb in zeilen:
        x, z, w, dd = it['rect']
        nm = NAMEN.get(it['item'], '?')
        if not ob:
            print('ROOM%s @0x%05X item=0x%02X %-22s prop=%d  KEIN Obj_model_set im SCD gefunden' % (raum, it['off'], it['item'], nm, it['prop']))
            continue
        o = ob[0]
        px, py, pz = o['pos']
        innen = (x <= px <= x + w) and (z <= pz <= z + dd)
        dx = px - (x + w / 2.0); dz = pz - (z + dd / 2.0)
        bs = ''
        if bb:
            lo, hi, nt, nq = bb
            bs = 'bbox=(%d..%d, %d..%d, %d..%d) %dx%dx%d Tri=%d Quad=%d' % (
                lo[0], hi[0], lo[1], hi[1], lo[2], hi[2], hi[0] - lo[0], hi[1] - lo[1], hi[2] - lo[2], nt, nq)
        print('ROOM%s @0x%05X item=0x%02X %-22s bit=%3d prop=%2d sat=0x%02X rect=(%d,%d,%d,%d) | Obj @0x%05X typ=%d pos=(%d,%d,%d) rot=(%d,%d,%d) eltern=0x%02X | im Kasten=%d dMitte=(%d,%d) | %s'
              % (raum, it['off'], it['item'], nm, it['bit'], it['prop'], it['sat'], x, z, w, dd,
                 o['off'], o['typ'], px, py, pz, o['rot'][0], o['rot'][1], o['rot'][2], o['eltern'],
                 1 if innen else 0, dx, dz, bs))
        rel.append((innen, abs(dx), abs(dz), w, dd, py, it['item']))
    n = len(rel)
    print()
    print('Modell liegt IM Aufhebe-Rechteck: %d von %d' % (sum(1 for r in rel if r[0]), n))
    import statistics
    print('Abstand Modell <-> Rechteckmitte: Median |dx|=%.0f |dz|=%.0f ; Maximum |dx|=%.0f |dz|=%.0f' % (
        statistics.median(r[1] for r in rel), statistics.median(r[2] for r in rel),
        max(r[1] for r in rel), max(r[2] for r in rel)))
    ys = collections.Counter(r[5] for r in rel)
    print('Prop-Hoehen y (Haeufigkeit):', ', '.join('%d: %d' % kv for kv in sorted(ys.items())))


if __name__ == '__main__':
    main()
