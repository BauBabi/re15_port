# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: RE2-Zensus des SPEICHER-Items (Ink Ribbon, Id 0x1E).

Beta->Retail-Bezug fuer die Memory Card: RE1.5 platziert sein Speicher-Item nirgends, RE2
platziert seines. Gezaehlt wird jeder Item_aot_set (0x4E, 22 B) mit Item-Id 0x1E:
Menge, Rechteck, Weltmodell-Slot. Walker: analysis/befunde_runde30/tools/r30_re2_scd.py
(Abdeckung beachten: ein Block, der desynchron endet, wird gezaehlt und gemeldet).
"""
import os, sys, glob, struct, collections
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'analysis', 'befunde_runde30', 'tools'))
import r30_re2_scd as S


def main():
    rdts = sorted(glob.glob(os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM*.RDT')))
    alle = []; bl = 0; des = 0
    for p in rdts:
        d = open(p, 'rb').read()
        if len(d) < 0x64:
            continue
        raum = os.path.basename(p)[4:8]
        for which in (16, 17):
            try:
                blocks = S.blocks(d, which)
            except Exception:
                continue
            for i, s, e in blocks:
                recs, st = S.walk(d, s, e)
                bl += 1
                if st != 'ok':
                    des += 1
                for off, op, r in recs:
                    if op == 0x4E:
                        x, z, w, dd = struct.unpack_from('<4h', r, 6)
                        item, menge, flag = struct.unpack_from('<3H', r, 14)
                        alle.append(dict(raum=raum, off=off, item=item, menge=menge, flag=flag,
                                         md1=r[20], act=r[21], rect=(x, z, w, dd), sat=r[3]))
    print('RE2 PL0: %d RDTs, %d SCD-Bloecke, davon %d desynchron; Item_aot_set (0x4E) gesamt: %d'
          % (len(rdts), bl, des, len(alle)))
    fb = [a for a in alle if a['item'] == 0x1E]
    print('Ink Ribbon (Id 0x1E): %d Platzierungen' % len(fb))
    print('   Mengen:', dict(collections.Counter(a['menge'] for a in fb)))
    print('   mit Weltmodell (md1 != 255): %d, ohne: %d' % (sum(1 for a in fb if a['md1'] != 255),
                                                          sum(1 for a in fb if a['md1'] == 255)))
    print('   Rechtecke (w x d):', dict(collections.Counter((a['rect'][2], a['rect'][3]) for a in fb)))
    for a in fb:
        print('   ROOM%s @0x%05X menge=%d flag=%d md1=%d rect=%s sat=0x%02X' % (
            a['raum'], a['off'], a['menge'], a['flag'], a['md1'], a['rect'], a['sat']))
    print('alle Rechteckgroessen in RE2 (haeufigste 6):',
          collections.Counter((a['rect'][2], a['rect'][3]) for a in alle).most_common(6))


if __name__ == '__main__':
    main()
