#!/usr/bin/env python3
"""Spur B: Zensus der HALTE-STELLE fuer den Cursor-Modus in sub04 (ROOM1150/1151).

Gesucht werden die 20 Byte  Cut_chg 4 / Pos_set / Sleep 5 / For 15 der Deckelfahrt:
  29 04 | 32 00 24 af cf fe cc bb | 09 0a 05 00 | 0d 00 18 00 0f 00
  (ROOM1150.RDT @0x0FB2 Cut_chg 4, @0x0FB4 Pos_set (-20700,-305,-17460), @0x0FBC Sleep 5,
   @0x0FC0 For 15 = "die Kuppel geht auf")
Die Halte-Stelle ist der For (Signatur + 14). Ausgabe: je Treffer Raum, Datei-Offset der Signatur,
Halte-Offset, und die 56-Byte-Signatur des Ruhe-Fensters (hebetisch_1150.c) zum Gegencheck."""
import glob, os
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
SIG = bytes.fromhex("2904" "320024afcffeccbb" "090a0500" "0d0018000f00")
RUHE = bytes.fromhex("0d0004000a00" "30020e00" "090a1e00" "2e030000" "36020a000300000000000000"
                     "090a0a00" "36020c000300000000000000" "2f010a00" "0d0004005a00")
n = 0
for p in sorted(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    d = open(p, "rb").read()
    i = d.find(SIG)
    while i >= 0:
        n += 1
        r = d.find(RUHE)
        print("%s: Signatur @0x%05X -> HALT (For 15) @0x%05X | Ruhe-Signatur @0x%05X -> Fenster [0x%05X,0x%05X)"
              % (os.path.basename(p), i, i + 14, r, r + 0x0A, r + 0x32))
        i = d.find(SIG, i + 1)
print("# %d Treffer in %d RDTs" % (n, len(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT")))))
