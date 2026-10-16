#!/usr/bin/env python3
"""Runde 37 Spur O: RE2-Zensus "wie reagieren Figuren in RE2-Szenen auf ein Geraeusch / im Dialog".

Laeuft opcode-exakt ueber ALLE RE2-Leon-RDTs (info/re2leon/PL0/RDT/ROOM*.RDT) mit dem Walker der
Runde 30 (analysis/befunde_runde30/tools/r30_re2_scd.py: Laengentabelle aus den 143 RE2-Handlern
ab 0x800A74C8 + 10 Overrides). Ausgabe:
  (A) je Se_on (0x36): die naechsten Figuren-Opcodes bis zum naechsten Message_on/Cut_chg/Evt_end —
      Plc_neck (0x41), Plc_dest (0x40, Modus 9 = Drehen am Ort), Plc_rot (0x58), Dir_set (0x33):
      reagiert die Figur mit dem KOPF oder dem KOERPER?
  (B) Zaehlung Plc_neck-Modi und Plc_dest-Modi in Szenen-Subs, die Message_on enthalten.
Aufruf: RE15_INFO_DIR=<hauptbaum>/info/Resident_Evil_und_Playstation_Information \
        python re2_reaktion_zensus.py [ROOMxxxx ...]
"""
import os, sys, struct, glob, collections
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'analysis', 'befunde_runde30', 'tools'))
_cwd = os.getcwd(); os.chdir(REPO)
import r30_re2_scd as R
os.chdir(_cwd)

FIG = {0x41: 'Plc_neck', 0x40: 'Plc_dest', 0x58: 'Plc_rot', 0x33: 'Dir_set', 0x3f: 'Plc_motion',
       0x2e: 'Work_set', 0x0a: 'Sleeping', 0x18: 'Gosub'}
STOP = {0x2b, 0x29, 0x01}

def s16(r, o): return struct.unpack_from('<h', r, o)[0]

def beschreibe(op, r):
    if op == 0x41:
        return 'Plc_neck m%d (%d,%d,%d) speed %02x%02x' % (r[1], s16(r, 2), s16(r, 4), s16(r, 6), r[9], r[8])
    if op == 0x40:
        return 'Plc_dest m%d f%02x (%d,%d)' % (r[2], r[3], s16(r, 4), s16(r, 6))
    if op == 0x58:
        return 'Plc_rot %s' % r[1:].hex(' ')
    if op == 0x33:
        return 'Dir_set (%d,%d,%d)' % (s16(r, 2), s16(r, 4), s16(r, 6))
    if op == 0x3f:
        return 'Plc_motion %s' % r[1:].hex(' ')
    if op == 0x2e:
        return 'Work_set(%d,%d)' % (r[1], r[2])
    if op == 0x0a:
        return 'Sleep %d' % struct.unpack_from('<H', r, 1)[0]
    if op == 0x18:
        return 'Gosub sub%02d' % r[1]
    return R.NAMES.get(op, 'op%02X' % op)

def main():
    rooms = sys.argv[1:] or sorted(os.path.basename(p)[4:8] for p in glob.glob(R.rdt_path('*')))
    neck_modi = collections.Counter(); dest_modi = collections.Counter()
    zaehl = collections.Counter()   # erste Figuren-Reaktion (nur Figuren: Work-Art 1 Spieler, 2 Partner, 3 NPC)
    for room in rooms:
        d = open(R.rdt_path(room), 'rb').read()
        for which, nm in ((16, 'main'), (17, 'sub')):
            for i, s, e in R.blocks(d, which):
                recs, st = R.walk(d, s, e)
                has_msg = any(op == 0x2b for _, op, _ in recs)
                if has_msg:
                    for _, op, r in recs:
                        if op == 0x41: neck_modi[r[1]] += 1
                        if op == 0x40: dest_modi[r[2]] += 1
                work = None
                works = []
                for off, op, r in recs:
                    if op == 0x2e: work = r[1]
                    works.append(work)
                for k, (off, op, r) in enumerate(recs):
                    if op != 0x36: continue
                    folge = []
                    erste = None
                    for j in range(k + 1, min(k + 25, len(recs))):
                        off2, op2, r2 = recs[j]
                        if op2 in STOP: break
                        if op2 in FIG: folge.append('@0x%05X %s' % (off2, beschreibe(op2, r2)))
                        figur = works[j] in (1, 2, 3)
                        if erste is None and figur and op2 == 0x41: erste = 'Kopf'
                        if erste is None and figur and ((op2 == 0x40 and r2[2] == 9) or op2 in (0x58, 0x33)): erste = 'Koerper'
                    if erste: zaehl[erste] += 1
                    if any(('Plc_neck' in f) or ('Plc_dest m9' in f) or ('Plc_rot' in f) or ('Dir_set' in f)
                           for f in folge):
                        print('ROOM%s %s%02d Se_on @0x%05X %s' % (room, nm, i, off, r.hex(' ')))
                        for f in folge: print('      ' + f)
    print('Erste Figuren-Reaktion nach Se_on (Work-Art 1/2/3): Kopf (Plc_neck) %d, Koerper (Plc_dest m9/Plc_rot/Dir_set) %d'
          % (zaehl['Kopf'], zaehl['Koerper']))
    print('Plc_neck-Modi in Szenen-Subs mit Message_on:', dict(sorted(neck_modi.items())))
    print('Plc_dest-Modi in Szenen-Subs mit Message_on:', dict(sorted(dest_modi.items())))

if __name__ == '__main__':
    main()
