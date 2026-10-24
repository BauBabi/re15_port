#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur F "gatorszene": die Bytes des Schluessels, den der Alligator mit der Leiche frisst.

  python re15_port/tools/r41_gatorszene/schluessel_export.py            (schreibt engine/src/gen/gatorszene_r41_schluessel.inc)
  python re15_port/tools/r41_gatorszene/schluessel_export.py --pruefen  (vergleicht nur, Rueckgabe 0 = bytegleich)

GEGENSTAND (Dossier §3.2): der nie platzierte RE1.5-"Water Key" - Name DEBUG.BIN Namenstabelle @0x800c495c Eintrag 0x45
"Water Key", CHECK-Text Beschreibungsbank @0x800c50de Eintrag 0x45 'The label says: "Sewer Mainteinance Room."', Icon
ITEMALL.PIX Tile 0x45 (Schluesselbund), Item-Bild ITPS.ITP Block 0x45. Die Id 0x45 traegt im Port seit Runde 36 Spur Q den
Parking Lot Key (re15_schluessel_r36.h); der Water Key bekommt deshalb die nie platzierte Id 0x34 ("Minidisc Player",
Zensus Item_aot_set/Aot_reset Dossier §3.2) und ALLE vier Teile des Originals 0x45 bytegleich (aus den AUSGELIEFERTEN Dateien,
nicht aus dem vom Port veraenderten Puffer).
WELTMODELL: RE1.5 hat fuer den Water Key keins (nie platziert) -> RE2-Weltmodell, das dem RE1.5-Icon (Schluesselbund) am
naechsten kommt: RE2 Item 0x63 "Platform Key" (mesh074, Ring + Bartschluessel), Platzierung room7040 sub00 @0x01370
(extracted_re2_items/platzierungen.csv), MD1 ROOM7040.RDT @0x00354C 140 B, TIM @0x033844 34336 B.
"""
import hashlib, os, re, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
ZIEL = os.path.join(PORT, "engine", "src", "gen", "gatorszene_r41_schluessel.inc")
WATER = 0x45

QUELLEN = (
    ("re15_gsz_key_md1",  os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM7040.RDT"), 0x00354C, 140,
     "070227d1adee81ee9d3fc2242d550092"),
    ("re15_gsz_key_tim",  os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM7040.RDT"), 0x033844, 34336,
     "e2c09bb9b5e97fcaefe6474a4f1b0ff6"),
    ("re15_gsz_key_icon", os.path.join(PORT, "shared_assets", "PSX", "DATA", "ITEMALL.PIX"), WATER * 1200, 1200, None),
    ("re15_gsz_key_itps", os.path.join(PORT, "shared_assets", "PSX", "ITEM", "ITPS.ITP"), WATER * 0x3000, 0x3000, None),
)


def bank_eintrag(inc, name_off, name_blob, idx, ende):
    src = open(os.path.join(PORT, "engine", "src", "gen", inc), encoding="utf-8").read()
    def arr(n):
        m = re.search(n + r"\[\d+\] = \{([^}]*)\}", src)
        return [int(x, 0) for x in m.group(1).split(",")]
    off, blob = arr(name_off), arr(name_blob)
    o = off[idx]; out = []
    while o < len(blob):
        out.append(blob[o]); o += 1
        if out[-len(ende):] == list(ende): break
    return bytes(out)


def main():
    pruefen = "--pruefen" in sys.argv
    zeilen = ["/* ERZEUGT von re15_port/tools/r41_gatorszene/schluessel_export.py - NICHT von Hand aendern.",
              " * Runde 41 Spur F: RE1.5-Water-Key (Id 0x45 des Originals) als Port-Id 0x34 + RE2-Weltmodell Platform Key. */"]
    for name, pfad, off, n, soll in QUELLEN:
        d = open(pfad, "rb").read()
        b = d[off:off + n]
        ist = hashlib.md5(b).hexdigest()
        if len(b) != n or (soll and ist != soll):
            sys.exit("ABBRUCH: %s @0x%X %d B md5 %s statt %s" % (pfad, off, len(b), ist, soll))
        rel = os.path.relpath(pfad, REPO).replace("\\", "/")
        zeilen.append("/* %s @0x%06X %d B md5 %s (Datei md5 %s) */" % (rel, off, n, ist, hashlib.md5(d).hexdigest()))
        zeilen.append("static const uint8_t %s[%d] = {" % (name, n))
        for i in range(0, n, 24):
            zeilen.append("    " + ",".join("0x%02x" % x for x in b[i:i + 24]) + ",")
        zeilen.append("};")
    name = bank_eintrag("inv_name_bank.inc", "re15_inv_name_off", "re15_inv_name_blob", WATER, (0x07,))
    desc = bank_eintrag("inv_desc_bank.inc", "re15_inv_desc_off", "re15_inv_desc_blob", WATER, (0x01, 0x00))
    zeilen.append("/* Name 0x45 der DEBUG.BIN-Namenstabelle (gen/inv_name_bank.inc) \"Water Key\" bis 0x07 */")
    zeilen.append("static const uint8_t re15_gsz_key_name[%d] = { %s };" % (len(name), ",".join("0x%02x" % x for x in name)))
    zeilen.append("/* CHECK-Text 0x45 der Beschreibungsbank (gen/inv_desc_bank.inc) bis 01 00 */")
    zeilen.append("static const uint8_t re15_gsz_key_desc[%d] = { %s };" % (len(desc), ",".join("0x%02x" % x for x in desc)))
    text = "\n".join(zeilen) + "\n"
    if pruefen:
        alt = open(ZIEL, encoding="utf-8").read() if os.path.exists(ZIEL) else ""
        print("bytegleich" if alt == text else "ABWEICHUNG")
        return 0 if alt == text else 1
    open(ZIEL, "w", encoding="utf-8", newline="\n").write(text)
    print("geschrieben", ZIEL, len(text), "Zeichen; Name", name.hex(), "Desc", len(desc), "B")
    return 0


if __name__ == "__main__":
    sys.exit(main())
