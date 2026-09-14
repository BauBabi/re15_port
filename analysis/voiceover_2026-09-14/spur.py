#!/usr/bin/env python3
"""Spur: WELCHE Opcodes machen die Zeit zwischen zwei Dialogzeilen aus?
Aufruf: spur.py <RDT> <pc-hex des Message_on>"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scd_zeit as SZ
import stellen as ST
import wav_inventar as WI


def spur(r, voiced, start_pc, stack=()):
    life = {mid: SZ.dialog_lifetime(raw) for mid, raw in r.msgs.items()}
    w = SZ.Walker(r, voiced, life)
    out, total = w.trace(start_pc, stack)
    return out, total


if __name__ == '__main__':
    rdt = sys.argv[1]
    pc = int(sys.argv[2], 16)
    r = SZ.Rdt(rdt)
    # vertonte ids aus dem synchro-Bestand
    import re
    m = re.search(r'ROOM([0-9A-F]{4})\.RDT', rdt.upper())
    rid = m.group(1)
    stage = 'STAGE%d' % (int(rid, 16) >> 12)
    import glob
    voiced = set()
    for p in glob.glob('synchro/%s/room%s/main*.wav' % (stage, rid)):
        voiced.add(int(os.path.basename(p)[4:6]))
    out, total = spur(r, voiced, pc)
    print('Start 0x%04X  vertonte Ids: %s' % (pc, sorted(voiced)))
    for note, c in out:
        print('   %5s  %s' % (('+%d' % c) if c else '', note))
    print('   Summe: %d Bilder' % total)
