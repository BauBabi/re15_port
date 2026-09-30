#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: Beschreibungstexte (CHECK) der Granaten-Ids aus der
Desc-Bank @0x800C50DE (DEBUG.BIN, generiert nach re15_port/engine/src/gen/inv_desc_bank.inc;
Zeichen = Code + 0x24, 0x57 = '.', 0x01 = Ende, 0x02 = Seitenwechsel, 0x08 = Zeilenumbruch).
"""
import os, re
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
s = open(os.path.join(REPO, "re15_port", "engine", "src", "gen", "inv_desc_bank.inc")).read()
offs = [int(x, 16) for x in re.search(r're15_inv_desc_off\[72\] = \{([^}]*)\}', s).group(1).split(',')]
blob = bytes(int(x, 16) for x in re.search(r're15_inv_desc_blob\[\d+\] = \{([^}]*)\}', s).group(1).split(','))
def dec(o):
    out = ''; i = o
    while i < len(blob):
        c = blob[i]
        if c == 0x01: break
        if c == 0x02: out += ' / '; i += 2; continue
        if c == 0x08: out += ' '; i += 1; continue
        if c == 0x00: out += ' '; i += 1; continue
        out += '.' if c == 0x57 else chr(c + 0x24); i += 1
    return out
for iid in (0x09, 0x0A, 0x0B, 0x0E, 0x0F, 0x10, 0x11, 0x19, 0x1A, 0x1B):
    print('Id 0x%02X  Desc @Bank+0x%03X: %s' % (iid, offs[iid], dec(offs[iid])))
