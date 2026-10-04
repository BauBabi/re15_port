#!/usr/bin/env python3
"""glas1090_extrakt.py - Runde 35 Spur M: die RE2-Daten des Fenster-Ereignisses ROOM1120.

Kopiert BYTEGLEICH aus RE2 Leon room1090 (info/re2leon/PL0/RDT/room1090, Extrakt der RDT) nach
re15_port/shared_assets/RE2/ und prueft jede Datei gegen die RDT-Bytes selbst:

  GLAS1090.ESP      = effect.esp  (Raum-ESP, Ids 29 10 11 12 13 14 0C 19; Banken 0x10..0x13 Splitter,
                                    0x14 Aufprall-Glitzern; Dossier M_cut11c0_fenster.md 2.3)
  GLAS1090_10.TIM .. GLAS1090_14.TIM = esp10.tim .. esp14.tim (die Texturen dieser fuenf Banken)
  GLAS1090.EDT/.VH/.VB = snd0.edt/.vh/.vb = RDT-Kopfworte +0x08/+0x0C/+0x10 (Raum-SE-Bank 2;
                                    Satz 0x21 = Glasbruch, Dossier 2.4)

Aufruf: python glas1090_extrakt.py [--pruefen]   (--pruefen: nur vergleichen, nichts schreiben)
Rueckgabe 0 = alles bytegleich.
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
SRC = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room1090")
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM1090.RDT")
DST = os.path.join(REPO, "re15_port", "shared_assets", "RE2")

PAARE = [("effect.esp", "GLAS1090.ESP")] + \
        [("esp%02X.tim" % b, "GLAS1090_%02X.TIM" % b) for b in (0x10, 0x11, 0x12, 0x13, 0x14)] + \
        [("snd0.edt", "GLAS1090.EDT"), ("snd0.vh", "GLAS1090.VH"), ("snd0.vb", "GLAS1090.VB")]


def main():
    nur_pruefen = "--pruefen" in sys.argv
    rdt = open(RDT, "rb").read()
    fehler = 0
    # Ton-Bank: Kopfworte +0x08/+0x0C/+0x10 der RDT (FUN_80059e54: DAT_800dbb80 = [RDT+8]).
    kopf = {"snd0.edt": struct.unpack_from("<I", rdt, 8)[0],
            "snd0.vh": struct.unpack_from("<I", rdt, 12)[0],
            "snd0.vb": struct.unpack_from("<I", rdt, 16)[0]}
    for quelle, ziel in PAARE:
        b = open(os.path.join(SRC, quelle), "rb").read()
        if quelle in kopf:
            o = kopf[quelle]
            gleich = rdt[o:o + len(b)] == b
            print("%-12s RDT@0x%05X %s" % (quelle, o, "gleich" if gleich else "ABWEICHUNG"))
            fehler += 0 if gleich else 1
        else:
            # effect.esp + esp1x.tim stehen bytegleich in der RDT (gemessen: ESP-Block,
            # TIMs @0x2D188/0x2E1C8/0x2F208/0x31248/0x32688).
            o = rdt.find(b)
            gleich = o >= 0
            print("%-12s in RDT @0x%05X: %s" % (quelle, max(o, 0), "ja" if gleich else "NEIN"))
            fehler += 0 if gleich else 1
        p = os.path.join(DST, ziel)
        if nur_pruefen:
            ok = os.path.exists(p) and open(p, "rb").read() == b
            print("  %-18s %s" % (ziel, "ok" if ok else "FEHLT/ANDERS"))
            fehler += 0 if ok else 1
        else:
            open(p, "wb").write(b)
            print("  -> %-18s %6d B md5 %s" % (ziel, len(b), hashlib.md5(b).hexdigest()))
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
