#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Same prop cut for the RE1.5 disc (info/Re1.5/PSX/STAGE1..6/*.RDT).

RE1.5 RDT header verified at STAGE1/ROOM1000.RDT:
  +0x01 nCut=9, +0x02 nOmodel=2, +0x24 camera=0x60 (= header size),
  0x60 + 9*32 = 0x180 = value at +0x30  -> +0x30 is the object-model pointer
  table, and 0x180 + 2*8 = 0x190 = value at +0x28 (zone) -> 8 bytes per entry,
  exactly as in RE2.
"""
import os, sys, struct, json, hashlib
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1lib

ROOT = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\Re1.5\PSX"
OUT = sys.argv[1]
os.makedirs(os.path.join(OUT, 'md1'), exist_ok=True)


def cut(path):
    d = open(path, 'rb').read()
    if len(d) < 0x60:
        return 0, [], len(d)
    hdr = struct.unpack_from('<8B', d, 0)
    n = hdr[2]
    offs = list(struct.unpack_from('<22I', d, 8))
    tbl = offs[(0x30 - 8) // 4]
    ents = []
    if n and tbl and tbl + 8 * n <= len(d):
        for i in range(n):
            t, m = struct.unpack_from('<II', d, tbl + 8 * i)
            ents.append((t, m))
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
    out = []
    for i, (t, m) in enumerate(ents):
        mb = d[m:nxt(m)] if 0 < m < len(d) else b''
        out.append((i, t, m, len(mb), mb))
    return n, out, len(d)


rows = []
tot = 0
nfiles = 0
for st in sorted(os.listdir(ROOT)):
    sd = os.path.join(ROOT, st)
    if not os.path.isdir(sd) or not st.upper().startswith('STAGE'):
        continue
    for fn in sorted(os.listdir(sd)):
        if not fn.upper().endswith('.RDT'):
            continue
        nfiles += 1
        n, ents, size = cut(os.path.join(sd, fn))
        tot += n
        room = fn[:-4]
        for (i, t, m, ln, mb) in ents:
            md1 = md1lib.parse(mb) if mb else None
            polys = md1lib.faces_world(md1) if md1 else []
            bb = md1lib.bbox(polys)
            ok = md1 is not None and len(polys) > 0
            if ok:
                open(os.path.join(OUT, 'md1', '%s_m%02d.md1' % (room, i)), 'wb').write(mb)
            rows.append({'stage': st, 'room': room, 'idx': i, 'md1_off': m,
                         'tim_off': t, 'md1_len': ln, 'nfaces': len(polys),
                         'dx': (bb[1] - bb[0]) if bb else 0,
                         'dy': (bb[3] - bb[2]) if bb else 0,
                         'dz': (bb[5] - bb[4]) if bb else 0,
                         'md5': hashlib.md5(mb).hexdigest() if mb else '',
                         'ok': ok})
json.dump(rows, open(os.path.join(OUT, 'props.json'), 'w'), indent=0)
print('RE1.5 RDT files      : %d' % nfiles)
print('sum nOmodel (props)  : %d' % tot)
print('parsed with geometry : %d' % sum(1 for r in rows if r['ok']))
print('distinct md5         : %d' % len(set(r['md5'] for r in rows if r['ok'])))
