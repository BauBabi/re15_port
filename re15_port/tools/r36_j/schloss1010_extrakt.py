#!/usr/bin/env python3
"""schloss1010_extrakt.py - Runde 36 Spur J: der Zuschliess-Ton von Robert Kendo (RE2 Leon, Gun Shop).

RE2 ROOM1010 sub09 @0x01C20 `36 02 0a 01 00 00 d8 c3 f8 f8 c6 c8` = Se_on Raumbank 2 (`srl t1,a0,24`
@0x8005ba30, Bank 2 = [RDT+0x08], FUN_80059e54) Satz 0x0A, gespielt 20 Bilder nach Kendos Clip 0 an der Tuer
(Dossier analysis/befunde_runde36/J_szenenleon.md §2.1). Kopiert BYTEGLEICH aus info/re2leon/PL0/RDT/room1010
(Extrakt der RDT) nach re15_port/shared_assets/RE2/ und prueft jede Datei gegen die RDT-Bytes selbst:

  SCHLOSS1010.EDT/.VH/.VB = snd0.edt/.vh/.vb = RDT-Kopfworte +0x08/+0x0C/+0x10 (EDT @0xA044 / VH @0xA104 /
                            VB @0xAF24); EDT Satz 0x0A @0x28 = `00 00 83 01` (Prog 0 Ton 8 -> VAG 4)

Aufruf: python schloss1010_extrakt.py [--pruefen]   (--pruefen: nur vergleichen, nichts schreiben)
Rueckgabe 0 = alles bytegleich.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
SRC = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room1010")
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM1010.RDT")
DST = os.path.join(REPO, "re15_port", "shared_assets", "RE2")
PAARE = [("snd0.edt", "SCHLOSS1010.EDT", 8), ("snd0.vh", "SCHLOSS1010.VH", 12), ("snd0.vb", "SCHLOSS1010.VB", 16)]


def main():
    nur_pruefen = "--pruefen" in sys.argv
    rdt = open(RDT, "rb").read()
    fehler = 0
    for quelle, ziel, kopf in PAARE:
        b = open(os.path.join(SRC, quelle), "rb").read()
        o = struct.unpack_from("<I", rdt, kopf)[0]
        gleich = rdt[o:o + len(b)] == b
        print("%-9s RDT+0x%02X -> @0x%05X %6d B %s" % (quelle, kopf, o, len(b), "gleich" if gleich else "ABWEICHUNG"))
        fehler += 0 if gleich else 1
        if quelle == "snd0.edt":
            satz = b[0x0A * 4:0x0A * 4 + 4]
            print("  EDT Satz 0x0A @0x28 = %s (Soll 00 00 83 01)" % satz.hex(" "))
            fehler += 0 if satz == bytes([0x00, 0x00, 0x83, 0x01]) else 1
        p = os.path.join(DST, ziel)
        if nur_pruefen:
            ok = os.path.exists(p) and open(p, "rb").read() == b
            print("  %-16s %s" % (ziel, "ok" if ok else "FEHLT/ANDERS"))
            fehler += 0 if ok else 1
        else:
            open(p, "wb").write(b)
            print("  -> %s" % p)
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
