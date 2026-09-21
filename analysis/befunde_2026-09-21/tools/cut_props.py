#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Cut every RE2-Leon RDT object-model (prop) MD1 out of the 250 RDTs,
parse it, and write an index with bbox dimensions.

Also cross-checks the cut against the room1110 models that a previous
extraction already produced (byte-equality) so the cutting rule is proven,
not assumed.
"""
import os, sys, struct, json, hashlib
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1lib

ROOT = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\re2leon\PL0\RDT"
OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)
os.makedirs(os.path.join(OUT, 'md1'), exist_ok=True)


def cut(path):
    d = open(path, 'rb').read()
    hdr = struct.unpack_from('<8B', d, 0)
    nOmodel = hdr[2]
    offs = list(struct.unpack_from('<23I', d, 8))
    tbl = offs[(0x30 - 8) // 4]
    ents = []
    if nOmodel and tbl and tbl + 8 * nOmodel <= len(d):
        for i in range(nOmodel):
            t, m = struct.unpack_from('<II', d, tbl + 8 * i)
            ents.append([t, m])
    bset = set(o for o in offs if 0 < o <= len(d))
    for t, m in ents:
        if 0 < t <= len(d):
            bset.add(t)
        if 0 < m <= len(d):
            bset.add(m)
    bset.add(len(d))
    bl = sorted(bset)

    def nxt(o):
        for b in bl:
            if b > o:
                return b
        return len(d)
    res = []
    for i, (t, m) in enumerate(ents):
        if not (0 < m < len(d)):
            res.append((i, t, m, b'', b''))
            continue
        mb = d[m:nxt(m)]
        tb = d[t:nxt(t)] if 0 < t < len(d) else b''
        res.append((i, t, m, mb, tb))
    return nOmodel, tbl, res, len(d)


def main():
    rows = []
    nprops = 0
    nparsed = 0
    files = sorted(f for f in os.listdir(ROOT) if f.upper().endswith('.RDT'))
    for f in files:
        room = f[:-4]
        nOmodel, tbl, res, size = cut(os.path.join(ROOT, f))
        nprops += nOmodel
        for (i, t, m, mb, tb) in res:
            md1 = md1lib.parse(mb) if mb else None
            polys = md1lib.faces_world(md1) if md1 else []
            bb = md1lib.bbox(polys)
            ok = md1 is not None and len(polys) > 0
            if ok:
                nparsed += 1
                fn = '%s_m%02d.md1' % (room, i)
                open(os.path.join(OUT, 'md1', fn), 'wb').write(mb)
            rows.append({
                'room': room, 'idx': i, 'md1_off': m, 'tim_off': t,
                'md1_len': len(mb), 'tim_len': len(tb),
                'nmesh': md1['nmesh'] if md1 else 0,
                'nfaces': len(polys),
                'bbox': bb,
                'dx': (bb[1] - bb[0]) if bb else 0,
                'dy': (bb[3] - bb[2]) if bb else 0,
                'dz': (bb[5] - bb[4]) if bb else 0,
                'md5': hashlib.md5(mb).hexdigest() if mb else '',
                'ok': ok,
            })
    json.dump(rows, open(os.path.join(OUT, 'props.json'), 'w'), indent=0)
    print('RDTs                : %d' % len(files))
    print('sum nOmodel         : %d' % nprops)
    print('parsed with geometry: %d' % nparsed)
    print('failed / empty      : %d' % (nprops - nparsed))
    # distinct geometry
    print('distinct md5        : %d' % len(set(r['md5'] for r in rows if r['ok'])))

    # --- cross-check against the pre-existing extraction of room1110 ---
    ref = os.path.join(ROOT, 'room1110', 'obj')
    same = diff = 0
    for i in range(12):
        p = os.path.join(ref, 'model%02d.md1' % i)
        q = os.path.join(OUT, 'md1', 'ROOM1110_m%02d.md1' % i)
        if os.path.exists(p) and os.path.exists(q):
            if open(p, 'rb').read() == open(q, 'rb').read():
                same += 1
            else:
                diff += 1
    print('room1110 crosscheck : %d byte-identical, %d different' % (same, diff))


main()
