#!/usr/bin/env python3
"""Zensus aller RE2-Tuer-AOTs (Door_aot_se 0x3B / 0x68) mit Schloss (key_id & 0x80) ueber
alle RDTs in info/re2leon/PL0/RDT, dazu der Zustand der Raumbank-Saetze 0x16/0x25/0x26.
Tuer-Handler @0x80051514: key_id&0x80 = verschlossen (@0x800515b0), Flag = key_id&0x3f
(@0x800515c4), key_type 0xFE -> entriegeln von dieser Seite (@0x800515d4), 0xFF -> von der
anderen Seite verriegelt (@0x800515dc), sonst Item-Id (@0x80051628 jal 0x800696cc)."""
import sys, os, glob, struct, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import r30_re2_scd as S
from r30_re2_roombank import load_bank, resolve
files = sorted(glob.glob('info/re2leon/PL0/RDT/ROOM*.RDT'))
tot = collections.Counter(); kt = collections.Counter()
rows = []
desync = []
for f in files:
    room = os.path.basename(f)[4:8]
    d = open(f, 'rb').read()
    b = load_bank(f)
    st = {}
    for sid in (0x16, 0x25, 0x26):
        if b is None: st[sid] = 'keine Bank'
        else:
            r = resolve(b, sid)
            st[sid] = 'leer' if (r is None or r['empty']) else (r['layers'][0].get('sha1', '?')[:8])
    for which, nm in ((16, 'main'), (17, 'sub')):
        for i, s, e in S.blocks(d, which):
            recs, status = S.walk(d, s, e)
            if status != 'ok': desync.append((room, '%s%02d' % (nm, i), status))
            for off, op, r in recs:
                if op in (0x3B, 0x68):
                    fl = S.door_fields(r, op)
                    tot['tueren'] += 1
                    if fl['key_id'] & 0x80:
                        tot['verschlossen'] += 1
                        kt[fl['key_type']] += 1
                        rows.append((room, '%s%02d' % (nm, i), off, fl, st))
print('# %d RDTs, %d Tuer-AOTs, davon %d mit key_id&0x80; %d desynchrone Bloecke' % (len(files), tot['tueren'], tot['verschlossen'], len(desync)))
print('# key_type-Verteilung: ' + ', '.join('0x%02x x%d' % kv for kv in sorted(kt.items())))
for room, blk, off, fl, st in rows:
    print('ROOM%s %-6s @0x%05X aot %2d -> %d%02X0 key_id 0x%02x (flag %2d) key_type 0x%02x | EDT 0x16=%s 0x25=%s 0x26=%s' % (
        room, blk, off, fl['aot'], fl['stage'] + 1, fl['room'], fl['key_id'], fl['key_id'] & 0x3f, fl['key_type'], st[0x16], st[0x25], st[0x26]))
print('# desync:', desync[:60])
