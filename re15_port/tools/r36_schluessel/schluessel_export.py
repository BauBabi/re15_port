#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 36 Spur Q "schluessel": die RE2-Bytes der zwei neuen Schluessel als C-Einbindung
engine/src/gen/schluessel_r36.inc.

Dossier: analysis/befunde_runde36/Q_schluessel.md (Abschnitt 2 "RE-Belege", 3 "Umsetzung").

WARUM EINGEBACKENE BYTES UND KEIN ASSET-PATCH: die ausgelieferten RDTs/ITPS.ITP/ITEMALL.PIX bleiben
byte-true (dieselbe Linie wie gen/dokumente_props.inc, gen/sicherung_itembild.inc). Weltmodell, Icon
und Item-Bild kommen portseitig dazu; Icon und Bild werden im GELADENEN Puffer eingesetzt
(re15_schluessel_r36_icon_einsetzen / _bild_einsetzen).

DIE QUELLEN — alle aus RE2 Retail (info/re2leon), jede gegen ihre md5 geprueft; stimmt eine nicht,
bricht das Werkzeug ab. Die WAHL der Modelle ist PORT-WAHL (Nutzer: "Waehle dir einen Schluessel aus
den Resident Evil 2 Items aus ..."), Begruendung im Dossier 2.4:

  PARKING LOT KEY  = RE2 Item 0x5F "P. Room Key" (Name RE2 PSX.EXE-Namensblob, katalog.csv Z. 95)
    Weltmodell  ROOM6150.RDT Prop-Slot 8 (RDT+0x30-Tabelle): MD1 @0x004FBC 236 B
                (md5 3c9176ee9bbb7e91b7c9433d6fe5c371), TIM @0x030B74 34848 B
                (md5 0d43d30f076929dbc396075df3e051f6) — Platzierung room6150 sub00 @0x02CBE
    Icon        COMMON/DATA/ITEMALL.PIX Tile 95 (= Id 0x5F, Tile == Id wie RE2-Tile 77 = 0x4D
                "Fuse Case" == RE1.5-Tile 0x40, re15_sicherung.h) @0x1BD50, 1200 B
    Item-Bild   COMMON/DATA/ITPS.ITP Block 95 @0x11D000, 0x3000 B
  PRISON KEY       = RE2 Item 0x1F "Small Key" (seit Runde 38 Spur K, Nachbesserung 1 — vorher Item 0x58
                     "Cabin Key", ein duenner dunkler Ring, an der Pinnwand nicht gut sichtbar; Wahl gemessen,
                     analysis/befunde_runde38/K_itemorte.md 9.3)
    Weltmodell  ROOM10F0.RDT Prop-Slot 4: MD1 @0x001650 236 B (md5 c5d49ef60b6b111ecac53a19652f3278),
                TIM @0x0215BC 34848 B (md5 326b822bb81b07ecb6d3544ce6e094a6) — Platzierung
                room10F0 sub00 @0x006F0 (Item_aot_set Item 0x1F, md1 4), Obj_model_set @0x00706 Attribut 0x0010
    Icon        ITEMALL.PIX Tile 31 @0x9150 (= 31 x 1200), 1200 B (== ITEMALL/ITEMALL_032.TIM, Index 0 kommt nicht vor)
    Item-Bild   ITPS.ITP Block 31 @0x5D000, 0x3000 B

EINZIGE DATEN-AENDERUNG: die zwei Rechtecke im TIM-Kopf des Item-Bilds. RE2 laedt das Bild an
crect (0,480) / prect (0,0) (gemessen: beide Bloecke), RE1.5s CHECK-Foto-Lader @0x800c0258 laedt an
die EINGEBETTETEN Rechtecke (`jal 0x8006bbbc` OpenTIM @0x800c0260 ... LoadImage @0x800c0280/@0x800c02a0)
und das Fotofenster liegt bei (832,256) 56x72 mit CLUT-Zeile (0,489) — wie in jedem RE1.5-Block
(gemessen: Block 0x01/0x38/0x42/0x45 tragen crect (0,489) / prect (832,256)). Ohne die Umsetzung bliebe
das Foto leer (dieselbe Falle wie Block 0x40, re15_sicherung.h). Gesetzt werden genau
  Block+0x0C u16 cx=0, +0x0E u16 cy=489  und  Block+0x214 u16 px=832, +0x216 u16 py=256.
Das Icon braucht KEINE Aenderung: die RE1.5-Icon-CLUT (ST_00.TIM CLUT-Zeile 0) und die RE2-Icon-CLUT
(ITEMALL_089.TIM) unterscheiden sich nur in Eintrag 0 (0x0000 / 0x8000), und Index 0 kommt in den
Tiles 31/95 nicht vor (gemessen, Dossier Runde 36 2.4 / Runde 38 K 9.3; ITEMALL_032/089/096 tragen dieselbe CLUT).

    python re15_port/tools/r36_schluessel/schluessel_export.py
"""
import hashlib
import io
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
RE2 = os.path.join(REPO, "info", "re2leon")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "schluessel_r36.inc")

# (C-Name, Datei relativ zu info/re2leon, Offset, Laenge, md5 oder None)
QUELLEN = (
    ("re15_r36q_park_md1",   "PL0/RDT/ROOM6150.RDT", 0x004FBC, 236,   "3c9176ee9bbb7e91b7c9433d6fe5c371"),
    ("re15_r36q_park_tim",   "PL0/RDT/ROOM6150.RDT", 0x030B74, 34848, "0d43d30f076929dbc396075df3e051f6"),
    ("re15_r36q_prison_md1", "PL0/RDT/ROOM10F0.RDT", 0x001650, 236,   "c5d49ef60b6b111ecac53a19652f3278"),
    ("re15_r36q_prison_tim", "PL0/RDT/ROOM10F0.RDT", 0x0215BC, 34848, "326b822bb81b07ecb6d3544ce6e094a6"),
    ("re15_r36q_park_icon",   "COMMON/DATA/ITEMALL.PIX", 95 * 1200, 1200, None),
    ("re15_r36q_prison_icon", "COMMON/DATA/ITEMALL.PIX", 31 * 1200, 1200, None),
    ("re15_r36q_park_itps",   "COMMON/DATA/ITPS.ITP", 95 * 0x3000, 0x3000, None),
    ("re15_r36q_prison_itps", "COMMON/DATA/ITPS.ITP", 31 * 0x3000, 0x3000, None),
)

# md5 der GANZEN Quelldateien (Icons/Bilder): Gegenprobe, dass der richtige RE2-Stand gelesen wird.
DATEI_MD5 = {}


def quelle(datei, off, n, soll):
    d = open(os.path.join(RE2, datei), "rb").read()
    b = bytearray(d[off:off + n])
    ist = hashlib.md5(b).hexdigest()
    if len(b) != n or (soll and ist != soll):
        sys.exit("ABBRUCH: %s @0x%X (%d B) md5 %s statt %s" % (datei, off, len(b), ist, soll))
    DATEI_MD5[datei] = hashlib.md5(d).hexdigest()
    return b, ist


def itps_rechtecke(b, name):
    """Kopf pruefen und crect/prect auf RE1.5 umsetzen (s. Kopf)."""
    magic, flags = struct.unpack_from("<II", b, 0)
    csz, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, 8)
    isz, px, py, pw, ph = struct.unpack_from("<IHHHH", b, 8 + csz)
    if (magic, flags, csz, cx, cy, cw, ch) != (0x10, 9, 524, 0, 480, 256, 1):
        sys.exit("ABBRUCH: %s CLUT-Kopf %r" % (name, (magic, flags, csz, cx, cy, cw, ch)))
    if (isz, px, py, pw, ph) != (8076, 0, 0, 56, 72) or 8 + csz != 0x214:
        sys.exit("ABBRUCH: %s Bild-Kopf %r" % (name, (isz, px, py, pw, ph)))
    struct.pack_into("<HH", b, 0x0C, 0, 489)
    struct.pack_into("<HH", b, 0x214 + 4, 832, 256)
    return b


def carr(name, b, f):
    f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n\n")


def main():
    daten = []
    for name, datei, off, n, soll in QUELLEN:
        b, md5 = quelle(datei, off, n, soll)
        if name.endswith("_itps"):
            b = itps_rechtecke(b, name)
        daten.append((name, bytes(b), datei, off, md5))
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    with io.open(ZIEL, "w", encoding="utf-8", newline="\n") as f:
        f.write("/* GENERIERT von re15_port/tools/r36_schluessel/schluessel_export.py - NICHT HAND-EDITIEREN.\n"
                " *\n"
                " * Runde 36 Spur Q: Weltmodell, Icon und Item-Bild der zwei neuen Schluessel, aus RE2 Retail\n"
                " * (info/re2leon). Dossier: analysis/befunde_runde36/Q_schluessel.md. Quelle je Feld\n"
                " * (Datei-Offset, Laenge, md5 der GELESENEN Bytes; _itps mit umgesetzten Rechtecken, s. Werkzeug):\n *\n")
        for name, b, datei, off, md5 in daten:
            f.write(" *   %-22s %s @0x%06X (%d B, md5 %s)\n" % (name, datei, off, len(b), md5))
        f.write(" *\n * md5 der Quelldateien:\n")
        for datei in sorted(DATEI_MD5):
            f.write(" *   %-24s %s\n" % (datei, DATEI_MD5[datei]))
        f.write(" */\n\n")
        for name, b, _d, _o, _m in daten:
            carr(name, b, f)
    for name, b, datei, off, md5 in daten:
        print("%-22s %-24s @0x%06X %6d B md5 %s" % (name, datei, off, len(b), md5))
    print("->", ZIEL)


if __name__ == "__main__":
    main()
