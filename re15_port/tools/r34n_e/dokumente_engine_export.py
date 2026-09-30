#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Exportiert die vier Weltmodelle der neuen Dokumente (Runde 34 Nacht, Spur E) als
C-Einbindung engine/src/gen/dokumente_props.inc.

Dossier: analysis/befunde_runde34_nacht/E_dokumente.md (3.1, 5.2 "Modelle", 9.0).

WARUM EINGEBACKENE BYTES UND KEIN ASSET-PATCH: die ausgelieferten RDTs bleiben byte-true
(dieselbe Linie wie gen/irons_tisch_props.inc, tools/irons_tisch_engine_export.py). Die
Dokumente kommen als ZUSAETZLICHE Props portseitig dazu (obj_id je Raum: re15_dokumente.h).

DIE ACHT QUELLEN — alle UNVERAENDERT aus RE2 (info/re2leon/PL0/RDT), jede gegen ihre md5
geprueft; stimmt eine nicht, bricht das Werkzeug ab. Zuordnung Dokument -> Modell ist die
NUTZER-VORGABE (AUFTRAG.md Z. 21/28/40/52, VERTRAG 1.4), geprueft in Dossier 3.1:

  Dok 1  mesh00_0541704e  ROOM1150.RDT MD1 @0x029F34 372 B / TIM @0x04C3F0 34848 B
         (RE2 room1150 sub00 Item 0x68 -> Dok 0 "CHRIS's diary" -> FILE00 = RE2-Originalpaar)
  Dok 2  mesh03_cf9f316d  ROOM10E0.RDT MD1 @0x002320 372 B / TIM @0x015224 17440 B
         (dasselbe Modell wie das Irons Diary; RE2 Item 0x70 "Secretary's diary B" -> FILE08)
  Dok 3  mesh01_ae2d0a30  ROOM2020.RDT MD1 @0x003254 404 B / TIM @0x01A160 34848 B
         (RE2 Item 0x6A "Memo to LEON" -> FILE02 = RE2-Originalpaar)
  Dok 4  mesh04_cab7b32d  ROOM60A0.RDT MD1 @0x002F08 404 B / TIM @0x03B924 34848 B
         (Geometrie = mesh01, getipptes gefaltetes Blatt; Papier FILE06 == FILE02 byte-gleich)

    C:/Python310/python.exe re15_port/tools/r34n_e/dokumente_engine_export.py
"""
import hashlib
import io
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
RE2 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "dokumente_props.inc")

# (C-Name, RE2-Datei, Offset, Laenge, md5)
QUELLEN = (
    ("re15_dokument1_md1", "ROOM1150.RDT", 0x029F34, 372, "0541704ee41d6445b718ee0b1b8abb3b"),
    ("re15_dokument1_tim", "ROOM1150.RDT", 0x04C3F0, 34848, "6e6f32cb59ddb4b83b27c921a2dbfd90"),
    ("re15_dokument2_md1", "ROOM10E0.RDT", 0x002320, 372, "cf9f316dd6ba0ecf599bea18a83c36d1"),
    ("re15_dokument2_tim", "ROOM10E0.RDT", 0x015224, 17440, "63bd93f2be1cc97066aa09afd3c67e4d"),
    ("re15_dokument3_md1", "ROOM2020.RDT", 0x003254, 404, "ae2d0a30a18ed9cb9f089c21839ca61e"),
    ("re15_dokument3_tim", "ROOM2020.RDT", 0x01A160, 34848, "6b864a6f02ffd6c0720be1efa654c3cf"),
    ("re15_dokument4_md1", "ROOM60A0.RDT", 0x002F08, 404, "cab7b32d230a19728221ffb6b1d75104"),
    ("re15_dokument4_tim", "ROOM60A0.RDT", 0x03B924, 34848, "73fa3595c568578eace3379747390ff6"),
)


def quelle(datei, off, n, soll):
    d = open(os.path.join(RE2, datei), "rb").read()
    b = d[off:off + n]
    ist = hashlib.md5(b).hexdigest()
    if len(b) != n or ist != soll:
        sys.exit("ABBRUCH: %s @0x%X (%d B) md5 %s statt %s" % (datei, off, len(b), ist, soll))
    return b


def carr(name, b, f):
    f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n\n")


def main():
    daten = [(name, quelle(datei, off, n, soll), datei, off, soll)
             for name, datei, off, n, soll in QUELLEN]
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    with io.open(ZIEL, "w", encoding="utf-8", newline="\n") as f:
        f.write("/* GENERIERT von re15_port/tools/r34n_e/dokumente_engine_export.py — NICHT HAND-EDITIEREN.\n"
                " *\n"
                " * Weltmodelle der vier Dokumente (Runde 34 Nacht, Spur E), UNVERAENDERT aus RE2\n"
                " * (info/re2leon/PL0/RDT). Dossier: analysis/befunde_runde34_nacht/E_dokumente.md.\n *\n")
        for name, b, datei, off, soll in daten:
            f.write(" *   %-20s %s @0x%06X (%d B, md5 %s)\n" % (name, datei, off, len(b), soll))
        f.write(" */\n\n")
        for name, b, _d, _o, _s in daten:
            carr(name, b, f)
    for name, b, datei, off, soll in daten:
        print("%-20s %s @0x%06X %6d B md5 %s ok" % (name, datei, off, len(b), soll))
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
