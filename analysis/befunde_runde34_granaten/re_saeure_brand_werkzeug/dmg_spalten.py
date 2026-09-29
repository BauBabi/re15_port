#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: RE1.5-Waffen-Schadenstabelle @0x8006E0D0 fuer die
Granaten-Spalten lesen (u16 @ 0x8006E0D0 + typ*0x58 + waffe*4; Beleg der Adressrechnung:
FUN_80011f50 @0x800124b0-0x800124dc, `lbu v1,8(s1)` * 0x58 + `addiu a0,a0,-7984`).

Nutzt den load()-Helfer von re15_disasm.py (NIE Datei-Offsets selbst rechnen).
Ausgabe: je Gegnertyp 0x00..0x3f die Spalten 9/10/11 (Hand/Acid/Incendiary Grenade),
14 (Flammenwerfer), 15/16/17 (GL-Klasse), plus Gleichheits-Checks.
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R

BASE = 0x8006E0D0
data, fo, path = R.load(BASE, None)

def u16(a):
    return struct.unpack_from("<H", data, fo(a))[0]

cols = [9, 10, 11, 14, 15, 16, 17]
print("; %s  dmg = u16 @ 0x8006E0D0 + typ*0x58 + w*4" % path)
print("typ  addr_row    " + " ".join("w%-4d" % c for c in cols) + "  9==15 10==16 11==17  10!=11")
for t in range(0x40):
    row = BASE + t * 0x58
    v = [u16(row + c * 4) for c in cols]
    d = dict(zip(cols, v))
    if not any(u16(row + w * 4) for w in range(22)):
        tag = "  (Zeile komplett 0)"
    else:
        tag = ""
    print("0x%02x 0x%08x  " % (t, row) + " ".join("%-5d" % x for x in v) +
          "  %-5s %-5s %-5s  %s%s" % (d[9] == d[15], d[10] == d[16], d[11] == d[17],
                                     "JA" if d[10] != d[11] else "-", tag))
