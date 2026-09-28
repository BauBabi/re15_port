#!/usr/bin/env python3
"""r30_hundtod_re2_flag0.py - RE2-Skripte: wer setzt/prueft Flag-Bank 0 (= 0x800CFB74,
Zeigertabelle @0x800A78C8[0]) Bit <n>?  Bitzaehlung MSB zuerst: Maske = 0x80000000 >> n.
Aufruf: python r30_hundtod_re2_flag0.py <bit-dezimal> [bank]
Benutzt den RE2-SCD-Laeufer der Runde (analysis/befunde_runde30/tools/r30_re2_scd.py)."""
import sys, os, glob
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import r30_re2_scd as S
bit = int(sys.argv[1]); bank = int(sys.argv[2]) if len(sys.argv) > 2 else 0
rdts = sorted(glob.glob(os.path.join(S.REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM*.RDT')))
setz = ck = 0; raeume = 0; abbruch = 0
for p in rdts:
    d = open(p, 'rb').read()
    raeume += 1
    for which in (16, 17):
        try:
            bl = S.blocks(d, which)
        except Exception:
            continue
        for (i, s, e) in bl:
            ops, st = S.walk(d, s, e)
            if st != 'ok': abbruch += 1
            for (pc, op, raw) in ops:
                if op == 0x22 and len(raw) >= 4 and raw[1] == bank and raw[2] == bit:
                    setz += 1
                    print("%s %s%02d @0x%04X  Set(%d,%d) := %d" % (os.path.basename(p),
                          'main' if which == 16 else 'sub', i, pc, bank, bit, raw[3]))
                if op == 0x21 and len(raw) >= 4 and raw[1] == bank and raw[2] == bit:
                    ck += 1
                    print("%s %s%02d @0x%04X  Ck(%d,%d) == %d" % (os.path.basename(p),
                          'main' if which == 16 else 'sub', i, pc, bank, bit, raw[3]))
print("# Raeume %d | Set %d | Ck %d | Bloecke mit Abbruch %d" % (raeume, setz, ck, abbruch))
