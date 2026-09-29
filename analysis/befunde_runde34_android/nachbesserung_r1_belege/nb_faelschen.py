# -*- coding: utf-8 -*-
"""Nachbesserung R1: weitere Faelschungen an Kopien der Referenz-APK - auf ROHEN Bytes ueber das
Werkzeug der Gegenpruefung (pruefer_umgehung_r1_belege/umgehung_werkzeug.py: eigener Parser,
unabhaengig vom Gate). Ergaenzt dessen Arten (backslash, nul, lfh_crc, lfh_usize, crc_kollision,
case, abschneiden, cd_sig, neu_identisch) um:
   lfh_name <e>      letzter Buchstabe des Namens NUR im Local Header anders
   utf8 <e>          ein Namensbyte (LFH + CD) -> 0xFF
   verschl <e>       Bit 0 (verschluesselt) in LFH + CD
   meth99 <e>        Methode 99 in LFH + CD
   weniger           EOCD nennt einen Eintrag weniger (n_hier/n_ges - 1)
Aufruf: nb_faelschen.py <quelle.apk> <ziel.apk> <art> [<eintrag>]
"""
import os
import shutil
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), "pruefer_umgehung_r1_belege"))
import umgehung_werkzeug as uw  # noqa: E402


def main():
    src, dst, art = sys.argv[1], sys.argv[2], sys.argv[3]
    e_name = sys.argv[4] if len(sys.argv) > 4 else None
    if art in ("lfh_name", "utf8", "verschl", "meth99"):
        z = uw.cd_lesen(src)
        e = uw.finde(z, e_name)
        shutil.copyfile(src, dst)
        if art == "lfh_name":
            neu = e["name"][:-1] + (b"3" if e["name"][-1:] != b"3" else b"4")
            uw.patch(dst, e["lho"] + 30, neu)
        elif art == "utf8":
            k = len(e["name"]) - 5
            uw.patch(dst, e["lho"] + 30 + k, b"\xff")
            uw.patch(dst, e["cd_pos"] + 46 + k, b"\xff")
        elif art == "verschl":
            uw.patch(dst, e["lho"] + 6, struct.pack("<H", e["lfh_flags"] | 1))
            uw.patch(dst, e["cd_pos"] + 8, struct.pack("<H", e["flags"] | 1))
        elif art == "meth99":
            uw.patch(dst, e["lho"] + 8, struct.pack("<H", 99))
            uw.patch(dst, e["cd_pos"] + 10, struct.pack("<H", 99))
        print("%s: %s" % (art, e_name))
    elif art == "weniger":
        z = uw.cd_lesen(src)
        shutil.copyfile(src, dst)
        n = len(z["eintraege"]) - 1
        uw.patch(dst, z["eocd_pos"] + 8, struct.pack("<HH", n, n))
        print("weniger: EOCD nennt %d statt %d Eintraege" % (n, n + 1))
    else:
        uw.forge(src, dst, art, *( [e_name] if e_name else []))


if __name__ == "__main__":
    main()
