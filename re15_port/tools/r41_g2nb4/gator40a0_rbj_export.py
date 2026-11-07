#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur G2-RE2 NB4 (T): die RE2-Raum-Animationsbank des Alligator-Raums fuer die RE2-Trefferreaktion der Figur.

Kopiert BYTEGLEICH aus RE2 Leon ROOM40A0 (Alligator-Kanal; info/re2leon/PL0/RDT/ROOM40A0.RDT, Extrakt
info/re2leon/PL0/RDT/room40A0/animation.rbj) nach re15_port/shared_assets/RE2/GATOR40A0.RBJ und prueft gegen die RDT-Bytes
(Muster tools/r41_ventilkette/re2_ventil_export.py):

  GATOR40A0.RBJ = animation.rbj = RDT-Kopfwort [24] (+0x60) @0x65E8, 14328 B = Laenge 0x37E8 + 2 Records (Trailer 2 x 8 B).
  Record 0 (Leon, Flags 0x1): EMR 15 Knochen x 80 B, EDD 21 Clips - 17/18/19/20 = 16/10/30/21 Bilder
  (Messblatt NB4 Abschnitt 1.1, mess_rbj40a0.txt). Record 1 (Claire) hat nur 16 Clips.
  Leser: RE2 FUN_80018e08 schreibt PL+0x190/+0x194 aus RDT+0x60 (`sw v0,0x0(t5)` @0x80018ea4 / `sw v0,0x4(t5)` @0x80018eb8,
  ghidra_re2_Leon); der Spieler-Haken des Gators 0x80102D08 spielt daraus die Clips 18/19 (Sub 2) und 20/17 (Sub 3)
  (`lw a1,400(s1) / lw a2,404(s1)` @0x8010312c-30 / @0x80103294-98).

Aufruf: python gator40a0_rbj_export.py [--pruefen]   Rueckgabe 0 = bytegleich.
"""
import hashlib
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
REPO = WURZEL
while REPO and not os.path.exists(os.path.join(REPO, "info", "re2leon")):   # Worktree: info/ liegt im Hauptbaum
    neu = os.path.dirname(REPO)
    if neu == REPO:
        break
    REPO = neu
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM40A0.RDT")
SRC = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room40A0", "animation.rbj")
DST = os.path.join(WURZEL, "re15_port", "shared_assets", "RE2", "GATOR40A0.RBJ")


def main():
    nur_pruefen = "--pruefen" in sys.argv
    rdt = open(RDT, "rb").read()
    b = open(SRC, "rb").read()
    o = struct.unpack_from("<I", rdt, 0x60)[0]
    fehler = 0
    gleich = rdt[o:o + len(b)] == b
    print("animation.rbj RDT-Kopf +0x60 @0x%05X %6d B %s md5 %s" % (o, len(b), "gleich" if gleich else "ABWEICHUNG", hashlib.md5(b).hexdigest()))
    fehler += 0 if (gleich and o == 0x65E8 and len(b) == 14328) else 1
    laenge, rec = struct.unpack_from("<II", b, 0)
    for r in range(rec):
        emr, edd = struct.unpack_from("<II", b, laenge + 8 * r)
        o_arm, o_frm, n, ks = struct.unpack_from("<HHHH", b, emr + 4)
        n_clips = struct.unpack_from("<HH", b, edd)[1] // 4
        clips = [struct.unpack_from("<H", b, edd + 4 * c)[0] for c in range(n_clips)]
        print("   Record %d: EMR @0x%X (%d Knochen x %d B) EDD @0x%X Clips %d%s" % (r, emr, n, ks, edd, n_clips,
              (": 17/18/19/20 = %d/%d/%d/%d Bilder" % tuple(clips[17:21])) if n_clips > 20 else ""))
        if r == 0:
            fehler += 0 if (n, ks, n_clips, tuple(clips[17:21])) == (15, 80, 21, (16, 10, 30, 21)) else 1
    fehler += 0 if (laenge, rec) == (0x37E8, 2) else 1
    if nur_pruefen:
        ok = os.path.exists(DST) and open(DST, "rb").read() == b
        print("  GATOR40A0.RBJ %s" % ("ok" if ok else "FEHLT/ANDERS"))
        fehler += 0 if ok else 1
    else:
        open(DST, "wb").write(b)
        print("  -> GATOR40A0.RBJ")
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
