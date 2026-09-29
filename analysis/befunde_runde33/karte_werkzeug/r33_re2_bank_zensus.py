#!/usr/bin/env python3
"""Runde 33 / Thema K: Zensus der RE2-SCD-Records Ck (0x21) / Set (0x22) auf den Karten-Baenken.
Handler selbst disassembliert (info/re2leon/PSX.EXE):
  Ck  0x21 @0x80054354: pc[1]=Bank, pc[2..3]=u16 Bit (lhu a1,2(v0)), Wert = Bit>>8; Laenge 4
  Set 0x22 @0x800543B4: pc[1]=Bank, pc[2]=Bit, pc[3]=Op (0 loeschen, 1 setzen, 7 umschalten); Laenge 4
  Bank-Zeiger: lw v1,[0x800A78C8 + Bank*4]  (@0x80054384 / @0x800543E8)
Baenke: 9 = 0x800D490C (Raum besucht), 31 = 0x800D4A34 (Item-Marken erledigt),
        32 = 0x800D4920 (Kartenzeichner Satzbyte +13), 33 = 0x800D4924 (Karte im Besitz),
        35 = 0x800D4908 (Blatt besucht = Etagenwahl-Gatter).
Aufruf: r33_re2_bank_zensus.py [bank ...]   (Default 32 33 35 9)
"""
import sys, os, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'analysis', 'befunde_runde30', 'tools'))
import r30_re2_scd as S
banks = [int(a) for a in sys.argv[1:]] or [32, 33, 35, 9]
rows = []; desync = []
for p in sorted(glob.glob(os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM*.RDT'))):
    room = os.path.basename(p)[4:8]
    d = open(p, 'rb').read()
    for which, tag in ((16, 'main'), (17, 'sub')):
        try:
            bl = S.blocks(d, which)
        except Exception as e:
            continue
        for i, s, e in bl:
            recs, st = S.walk(d, s, e)
            if st != 'ok': desync.append((room, tag, i, st))
            for off, op, r in recs:
                if op in (0x21, 0x22) and r[1] in banks:
                    if op == 0x21:
                        bit = r[2] | (r[3] << 8); val = bit >> 8; bit &= 0xff
                        rows.append((r[1], room, tag, i, off, 'Ck ', bit, val, r.hex(' ')))
                    else:
                        rows.append((r[1], room, tag, i, off, 'Set', r[2], r[3], r.hex(' ')))
for b in banks:
    sel = [x for x in rows if x[0] == b]
    print('=== Bank %d: %d Records (Set %d, Ck %d)' % (b, len(sel), sum(1 for x in sel if x[5]=='Set'),
          sum(1 for x in sel if x[5]=='Ck ')))
    for x in sel:
        print('  ROOM%s %s%02d @0x%05X %s bit %3d %s %d   [%s]' % (x[1], x[2], x[3], x[4], x[5], x[6],
              'op' if x[5]=='Set' else '==', x[7], x[8]))
print('desync-Bloecke:', len(desync))
for x in desync[:40]: print('  ', x)
