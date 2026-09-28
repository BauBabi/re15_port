#!/usr/bin/env python3
"""Zensus: (1) alle SCD-Se_on mit Bank 2 und Id 0x16/0x25/0x26 in allen Leon-A-Raeumen,
(2) alle Skriptbloecke, die eine 'verschlossen'-Nachricht zeigen (Message_on auf einen Text
mit lock/won't open/card/...), mit den Se_on desselben Blocks,
(3) alle Message-AOTs (Aot_set sce 4) auf solche Texte (dort laeuft KEIN Skript)."""
import sys, os, glob, re, struct, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import r30_re2_scd as S
import r30_re2_msg as M
from r30_re2_roombank import load_bank, resolve
PAT = re.compile(r"lock|won<3a>t open|card reader|nailed|can<3a>t open|sealed|doesn<3a>t open", re.I)
files = sorted(glob.glob('info/re2leon/PL0/RDT/ROOM' + (sys.argv[1] if len(sys.argv) > 1 else '[1-7]') + '*.RDT'))
seon16 = []; lockblocks = []; msgaots = []
allse = collections.Counter()
for f in files:
    room = os.path.basename(f)[4:8]
    d = open(f, 'rb').read()
    try: ms = {i: s.replace('\n', ' ') for i, o, s in M.msgs(f)}
    except Exception: ms = {}
    b = load_bank(f)
    for which, nm in ((16, 'main'), (17, 'sub')):
        for i, s, e in S.blocks(d, which):
            recs, st = S.walk(d, s, e)
            tag = '%s%02d' % (nm, i)
            ses = [(off, r) for off, op, r in recs if op == 0x36]
            mss = [(off, r[2]) for off, op, r in recs if op == 0x2B]
            for off, r in ses:
                allse[(r[1], r[2])] += 1
                if r[1] == 2 and r[2] in (0x16, 0x25, 0x26):
                    seon16.append((room, tag, off, r, [ms.get(m, '?') for _, m in mss]))
            lk = [(off, m) for off, m in mss if PAT.search(ms.get(m, ''))]
            if lk:
                lockblocks.append((room, tag, lk, ses, ms, b))
            for off, op, r in recs:
                if op in (0x2C, 0x67) and r[2] == 4:
                    pay = r[14:] if op == 0x2C else r[22:]
                    mi = struct.unpack_from('<H', pay, 0)[0]
                    if PAT.search(ms.get(mi, '')):
                        msgaots.append((room, tag, off, r[1], mi, ms.get(mi)))
                if op == 0x46 and r[2] == 4:
                    mi = struct.unpack_from('<H', r, 4)[0]
                    if PAT.search(ms.get(mi, '')):
                        msgaots.append((room, tag, off, r[1], mi, ms.get(mi) + '  [Aot_reset]'))
print('### (1) SCD-Se_on Bank 2 Id 0x16/0x25/0x26: %d' % len(seon16))
for room, tag, off, r, m in seon16:
    print('ROOM%s %-6s @0x%05X  %s  | Texte im Block: %s' % (room, tag, off, r.hex(' '), ' / '.join(x[:70] for x in m)))
print()
print('### (2) Skriptbloecke mit verschlossen-Nachricht: %d' % len(lockblocks))
for room, tag, lk, ses, ms, b in lockblocks:
    print('ROOM%s %-6s' % (room, tag))
    for off, m in lk:
        print('      Message_on @0x%05X msg %d: %s' % (off, m, ms.get(m)[:110]))
    if not ses: print('      (kein Se_on im Block)')
    for off, r in ses:
        w = ''
        if b and r[1] == 2:
            rr = resolve(b, r[2])
            if rr and not rr['empty']:
                L = rr['layers'][0]; w = ' -> Welle sha1 %s %d B vol%d' % (L.get('sha1', '?')[:12], L.get('vag_size', -1), L['vol'])
            else: w = ' -> EDT-Satz LEER'
        print('      Se_on      @0x%05X bank %d id 0x%02x low %02x%s' % (off, r[1], r[2], r[3], w))
print()
print('### (3) Message-AOTs (sce 4) auf verschlossen-Texte: %d' % len(msgaots))
for room, tag, off, aot, mi, t in msgaots:
    print('ROOM%s %-6s @0x%05X aot %2d msg %2d: %s' % (room, tag, off, aot, mi, t[:120]))
