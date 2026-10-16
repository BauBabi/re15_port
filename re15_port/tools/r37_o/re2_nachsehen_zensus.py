#!/usr/bin/env python3
"""Runde 37 Spur O (Nachtrag 12): RE2-Zensus "einer Figur nachsehen".

Opcode-exakt ueber alle RE2-Leon-RDTs (Walker r30). Je Lauf/Gang einer Figur (Plc_dest Modus 4/5 = gehen/rennen bzw.
6/7/8, Work-Art 1/2/3) werden die naechsten Opcodes (bis zum Ende des Blocks, hoechstens 30) nach Kopf-/Koerper-Befehlen
einer ANDEREN Figur durchsucht: Plc_neck (0x41, Modus + Ziel), Plc_dest Modus 9 (Drehen am Ort) bzw. 20/21, Plc_rot (0x58),
Dir_set (0x33). Ausgabe je Fund mit Datei-Offsets; am Ende die Zaehlung je Reaktionsform.
Aufruf: RE15_INFO_DIR=<hauptbaum>/info/Resident_Evil_und_Playstation_Information python re2_nachsehen_zensus.py
"""
import os, sys, struct, glob, collections
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'analysis', 'befunde_runde30', 'tools'))
_cwd = os.getcwd(); os.chdir(REPO)
import r30_re2_scd as R
os.chdir(_cwd)


def s16(r, o):
    return struct.unpack_from('<h', r, o)[0]


def main():
    zaehl = collections.Counter()
    for room in sorted(os.path.basename(p)[4:8] for p in glob.glob(R.rdt_path('*'))):
        d = open(R.rdt_path(room), 'rb').read()
        for which, nm in ((16, 'main'), (17, 'sub')):
            for i, s, e in R.blocks(d, which):
                recs, st = R.walk(d, s, e)
                works = []
                w = None
                for off, op, r in recs:
                    if op == 0x2e:
                        w = (r[1], r[2])
                    works.append(w)
                for k, (off, op, r) in enumerate(recs):
                    if op != 0x40 or r[2] not in (4, 5) or not works[k] or works[k][0] not in (1, 2, 3):
                        continue
                    laeufer = works[k]
                    funde = []
                    for j in range(k + 1, min(k + 31, len(recs))):
                        off2, op2, r2 = recs[j]
                        if op2 == 0x01:
                            break
                        wj = works[j]
                        if not wj or wj == laeufer or wj[0] not in (1, 2, 3):
                            continue
                        if op2 == 0x41:
                            funde.append('@0x%05X Work%s Plc_neck m%d (%d,%d,%d) Rate %02x/%02x' % (
                                off2, wj, r2[1], s16(r2, 2), s16(r2, 4), s16(r2, 6), r2[8], r2[9]))
                            zaehl['Plc_neck m%d' % r2[1]] += 1
                        elif op2 == 0x40 and r2[2] in (9, 20, 21):
                            funde.append('@0x%05X Work%s Plc_dest m%d (%d,%d)' % (off2, wj, r2[2], s16(r2, 4), s16(r2, 6)))
                            zaehl['Plc_dest m%d' % r2[2]] += 1
                        elif op2 == 0x58:
                            funde.append('@0x%05X Work%s Plc_rot %s' % (off2, wj, r2[1:].hex(' ')))
                            zaehl['Plc_rot'] += 1
                        elif op2 == 0x33:
                            funde.append('@0x%05X Work%s Dir_set %d' % (off2, wj, s16(r2, 4)))
                            zaehl['Dir_set'] += 1
                    if funde:
                        print('ROOM%s %s%02d Work%s Plc_dest m%d (%d,%d) @0x%05X -> andere Figur:' % (
                            room, nm, i, laeufer, r[2], s16(r, 4), s16(r, 6), off))
                        for f in funde:
                            print('      ' + f)
    print('Zaehlung (Reaktion einer anderen Figur nach einem Gang/Lauf):', dict(zaehl))


if __name__ == '__main__':
    main()
