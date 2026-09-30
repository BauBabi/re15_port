#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Spur B (Runde 34 Nacht, BAU): backt den Cursor der RE1.5-Cursor-Raetsel fuer den Hebetisch
in Irons' Buero ein -> re15_port/engine/src/gen/hebetisch_cursor.inc.

QUELLE (unveraendert, Byte fuer Byte): re15_port/shared_assets/PSX/STAGE1/ROOM11F0.RDT
  Prop-Tabelle @0x0240 (RDT-Kopf +0x30 zeigt dorthin), Eintrag 0 = (TIM, MD1):
    TIM @0x018DAC, 33312 B (bis zum TIM von Prop 1 @0x020FCC), 8bpp 128x256, CLUT @VRAM(0,480)
    MD1 @0x001928,  5556 B (bis zum MD1 von Prop 1 @0x002EDC)
  Derselbe Cursor steht bytegleich in acht Raeumen (Gegenpruefung (b)).
WARUM EINGEBACKEN: die ausgelieferten RDTs bleiben byte-true, ROOM1150/1151 bekommen kein
neues Prop (Linie von gen/sicherung_prop.inc / gen/irons_tisch_props.inc). Eingebunden nur auf
dem PC (der Cursor ist auf dem PSX-Ziel aus, include/re15_hebetisch_cursor.h).

Das Werkzeug prueft die Tabelleneintraege und die md5 der beiden Bloecke; weicht etwas ab,
bricht es ab. Riegel unit_r34n_b_cursor_bytes vergleicht das .inc zur Laufzeit gegen die RDT.

    C:/Python310/python.exe re15_port/tools/r34n_b/cursor_export.py
"""
import hashlib
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
QUELLE = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE1", "ROOM11F0.RDT")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "hebetisch_cursor.inc")

TABELLE = 0x0240
MD1_OFF, MD1_LEN = 0x001928, 5556
TIM_OFF, TIM_LEN = 0x018DAC, 33312


def block(name, data):
    zeilen = []
    for i in range(0, len(data), 16):
        zeilen.append("    " + ",".join("0x%02x" % b for b in data[i:i + 16]) + ",")
    return "static const unsigned char %s[%d] = {\n%s\n};\n" % (name, len(data), "\n".join(zeilen))


def main():
    d = open(QUELLE, "rb").read()
    tab = struct.unpack_from("<I", d, 0x30)[0]
    if tab != TABELLE:
        sys.exit("RDT+0x30 = 0x%X, erwartet 0x%X" % (tab, TABELLE))
    tim0, md10 = struct.unpack_from("<II", d, TABELLE)
    tim1, md11 = struct.unpack_from("<II", d, TABELLE + 8)
    if (tim0, md10) != (TIM_OFF, MD1_OFF):
        sys.exit("Prop 0 = (TIM 0x%X, MD1 0x%X), erwartet (0x%X, 0x%X)" % (tim0, md10, TIM_OFF, MD1_OFF))
    if md11 - md10 != MD1_LEN or tim1 - tim0 != TIM_LEN:
        sys.exit("Laengen MD1 %d / TIM %d, erwartet %d / %d" % (md11 - md10, tim1 - tim0, MD1_LEN, TIM_LEN))
    md1 = d[MD1_OFF:MD1_OFF + MD1_LEN]
    tim = d[TIM_OFF:TIM_OFF + TIM_LEN]
    m_md1 = hashlib.md5(md1).hexdigest()
    m_tim = hashlib.md5(tim).hexdigest()
    kopf = (
        "/* GENERIERT von re15_port/tools/r34n_b/cursor_export.py - NICHT HAND-EDITIEREN.\n"
        " *\n"
        " * Cursor der RE1.5-Cursor-Raetsel (Spur B, Runde 34 Nacht; include/re15_hebetisch_cursor.h).\n"
        " * Quelle unveraendert: STAGE1/ROOM11F0.RDT, Prop-Tabelle @0x%04X Eintrag 0\n"
        " *   MD1 @0x%06X, %d B, md5 %s\n"
        " *   TIM @0x%06X, %d B, md5 %s\n"
        " */\n\n" % (TABELLE, MD1_OFF, MD1_LEN, m_md1, TIM_OFF, TIM_LEN, m_tim))
    text = kopf + block("re15_hc_cursor_md1", md1) + "\n" + block("re15_hc_cursor_tim", tim)
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    with open(ZIEL, "w", newline="\n") as f:
        f.write(text)
    print("geschrieben: %s (MD1 %d B md5 %s, TIM %d B md5 %s)" % (ZIEL, len(md1), m_md1, len(tim), m_tim))


if __name__ == "__main__":
    main()
