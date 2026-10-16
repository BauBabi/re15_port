#!/usr/bin/env python3
"""Runde 37 Spur O: RE2-Zensus "wie stellt RE2 zwei Gespraechspartner auf und haelt sie zugewandt".

Opcode-exakt ueber alle RE2-Leon-RDTs (Walker der Runde 30, analysis/befunde_runde30/tools/r30_re2_scd.py).
(A) AUFSTELLUNG: Folgen Work_set(k,i) -> Pos_set -> Dir_set fuer ZWEI verschiedene Figuren (Spieler 1,0 / Partner 2,x /
    NPC 3,x) im selben Sub vor dem naechsten Message_on; je Fund: beide Orte, beide Dir_set-Gieren und die Peilung von
    jedem auf den anderen (FUN_8001a6d4-Form: -atan2(dz,dx) in 4096-Einheiten) -> zeigt RE2 die Partner zueinander?
(B) JE ZEILE: wie oft steht zwischen einem Message_on und dem naechsten Message_on/Evt_end ein Plc_neck (Kopf auf ...).
Aufruf: RE15_INFO_DIR=<hauptbaum>/info/Resident_Evil_und_Playstation_Information python re2_aufstellung_zensus.py
"""
import os, sys, struct, glob, math
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'analysis', 'befunde_runde30', 'tools'))
_cwd = os.getcwd(); os.chdir(REPO)
import r30_re2_scd as R
os.chdir(_cwd)


def s16(r, o):
    return struct.unpack_from('<h', r, o)[0]


def peil(vx, vz, zx, zz):
    return int(round(-math.degrees(math.atan2(zz - vz, zx - vx)) / 360.0 * 4096)) & 0xfff


def wdiff(a, b):
    d = (b - a) & 0xfff
    return d - 0x1000 if d > 0x800 else d


def main():
    rooms = sorted(os.path.basename(p)[4:8] for p in glob.glob(R.rdt_path('*')))
    funde = 0
    zeilen = 0
    zeilen_mit_neck = 0
    for room in rooms:
        d = open(R.rdt_path(room), 'rb').read()
        for which, nm in ((16, 'main'), (17, 'sub')):
            for i, s, e in R.blocks(d, which):
                recs, st = R.walk(d, s, e)
                # (A)
                work = None
                fig = {}
                for off, op, r in recs:
                    if op == 0x2e:
                        work = (r[1], r[2])
                    elif op == 0x32 and work:
                        fig.setdefault(work, {})['pos'] = (s16(r, 2), s16(r, 6), off)
                    elif op == 0x33 and work:
                        fig.setdefault(work, {})['dir'] = (s16(r, 4) & 0xfff, off)
                    elif op == 0x2b:
                        voll = [(k, v) for k, v in fig.items() if 'pos' in v and 'dir' in v and k[0] in (1, 2, 3)]
                        if len(voll) >= 2:
                            (ka, a), (kb, b) = voll[0], voll[1]
                            pa = peil(a['pos'][0], a['pos'][1], b['pos'][0], b['pos'][1])
                            pb = peil(b['pos'][0], b['pos'][1], a['pos'][0], a['pos'][1])
                            funde += 1
                            print('ROOM%s %s%02d Message_on @0x%05X: Work%s Pos(%d,%d) Dir %d @0x%05X -> Peilung %d (%+d) | '
                                  'Work%s Pos(%d,%d) Dir %d @0x%05X -> Peilung %d (%+d)' % (
                                      room, nm, i, off, ka, a['pos'][0], a['pos'][1], a['dir'][0], a['dir'][1], pa,
                                      wdiff(pa, a['dir'][0]), kb, b['pos'][0], b['pos'][1], b['dir'][0], b['dir'][1], pb,
                                      wdiff(pb, b['dir'][0])))
                        fig = {}
                # (B)
                for k, (off, op, r) in enumerate(recs):
                    if op != 0x2b:
                        continue
                    zeilen += 1
                    for off2, op2, r2 in recs[k + 1:]:
                        if op2 in (0x2b, 0x01):
                            break
                        if op2 == 0x41:
                            zeilen_mit_neck += 1
                            break
    print('(A) Aufstellungen (zwei Figuren mit Pos_set + Dir_set vor einem Message_on): %d' % funde)
    print('(B) Message_on gesamt %d, davon mit Plc_neck bis zur naechsten Zeile: %d' % (zeilen, zeilen_mit_neck))


if __name__ == '__main__':
    main()
