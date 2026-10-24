#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur C (Nachbesserung 5, Nachtrag 18 Punkt 3): die RE2-Ventil-Animation und ihr Ton.

Kopiert BYTEGLEICH aus RE2 Leon ROOM1080 (Hubschrauber-Landeplatz, msg 0 @0x267A "A water pressure valve";
info/re2leon/PL0/RDT/ROOM1080.RDT, Extrakt info/re2leon/PL0/RDT/room1080/) nach re15_port/shared_assets/RE2/ und
prueft jede Datei gegen die RDT-Bytes selbst (Muster tools/r35_fenster/glas1090_extrakt.py, r38_dining/re2_export.py):

  VENTIL1080.RBJ = animation.rbj = RDT-Kopfwort [22] (+0x60) @0x39AC, 13128 B: Laenge 0x3340 + 1 Record (Trailer
                   8 B: EMR 0x8 / EDD 0x3084); EMR 15 Knochen x 80 B (155 Keyframes), EDD 19 Clips - 0..14 je 1 Bild,
                   15 = 30, 16 = 50, 17 = 40, 18 = 20 Bilder. sub08 @0x0213C/@0x02156/@0x02186 spielt 15/16/17.
  VENTIL1080.EDT/.VH/.VB = snd0 = RDT-Kopfworte +0x08/+0x0C/+0x10 (EDT @0x6CF4 192 B = 48 Saetze, VH @0x6DB4,
                   VB @0x79D4); Satz 5 = `00 00 54 16` = das Drehgeraeusch (sub08 se_on `36 02 05 01 ...` @0x02168).

Aufruf: python re2_ventil_export.py [--pruefen]   (--pruefen: nur vergleichen, nichts schreiben)
Rueckgabe 0 = alles bytegleich.
"""
import hashlib
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM1080.RDT")
SRC = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room1080")
DST = os.path.join(REPO, "re15_port", "shared_assets", "RE2")

PAARE = [("animation.rbj", "VENTIL1080.RBJ", 0x60), ("snd0.edt", "VENTIL1080.EDT", 0x08),
         ("snd0.vh", "VENTIL1080.VH", 0x0C), ("snd0.vb", "VENTIL1080.VB", 0x10)]


def main():
    nur_pruefen = "--pruefen" in sys.argv
    rdt = open(RDT, "rb").read()
    fehler = 0
    for quelle, ziel, kopf in PAARE:
        b = open(os.path.join(SRC, quelle), "rb").read()
        o = struct.unpack_from("<I", rdt, kopf)[0]
        gleich = rdt[o:o + len(b)] == b
        print("%-14s RDT-Kopf +0x%02X @0x%05X %6d B %s md5 %s" % (quelle, kopf, o, len(b), "gleich" if gleich else "ABWEICHUNG",
                                                              hashlib.md5(b).hexdigest()))
        fehler += 0 if gleich else 1
        if quelle == "animation.rbj":
            laenge, rec = struct.unpack_from("<II", b, 0)
            emr, edd = struct.unpack_from("<II", b, laenge)
            o_arm, o_frm, n, ks = struct.unpack_from("<HHHH", b, emr + 4)
            n_clips = struct.unpack_from("<HH", b, edd)[1] // 4
            clips = [struct.unpack_from("<H", b, edd + 4 * c)[0] for c in range(n_clips)]
            print("   RBJ Laenge 0x%X Records %d EMR @0x%X (%d Knochen x %d B) EDD @0x%X Clips %d: 15/16/17 = %d/%d/%d Bilder"
                  % (laenge, rec, emr, n, ks, edd, n_clips, clips[15], clips[16], clips[17]))
            fehler += 0 if (rec, n, ks, n_clips, clips[15], clips[16], clips[17]) == (1, 15, 80, 19, 30, 50, 40) else 1
        if quelle == "snd0.edt":
            print("   EDT Satz 5 = %s" % b[20:24].hex(" "))
            fehler += 0 if b[20:24] == bytes([0x00, 0x00, 0x54, 0x16]) else 1
        p = os.path.join(DST, ziel)
        if nur_pruefen:
            ok = os.path.exists(p) and open(p, "rb").read() == b
            print("  %-16s %s" % (ziel, "ok" if ok else "FEHLT/ANDERS"))
            fehler += 0 if ok else 1
        else:
            open(p, "wb").write(b)
            print("  -> %-16s" % ziel)
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
