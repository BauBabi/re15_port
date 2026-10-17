#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur F "dining1141": die RE2-Bytes der Spur.

  python re15_port/tools/r38_dining/re2_export.py            (schreibt)
  python re15_port/tools/r38_dining/re2_export.py --pruefen  (vergleicht nur, Rueckgabe 0 = bytegleich)

1. DINING ROOM KEY (Item-Id 0x43, include/re15_dining_r38.h) = RE2 Item 0x5B "Precinct Key" (Herz, mesh066;
   abgestimmter Ablauf Runde 37 Nachtrag 14). Quellen (info/re2leon, md5-geprueft):
     Weltmodell  PL0/RDT/ROOM2020.RDT MD1 @0x003474 236 B (md5 4b543edc8c8cbee84e6e27565503cab1),
                 TIM @0x01A160 34848 B (md5 6b864a6f02ffd6c0720be1efa654c3cf) - Platzierung room2020 sub00
                 @0x01C5C Item_aot_set Id 0x5B (extracted_re2_items/platzierungen.csv Z. 212)
     Icon        COMMON/DATA/ITEMALL.PIX Tile 91 (Tile == Id, 1200 B)
     Item-Bild   COMMON/DATA/ITPS.ITP Block 91 (Block == Id, 0x3000 B) - crect/prect wie Runde 36 Spur Q auf das
                 RE1.5-Fotofenster umgesetzt (Block+0x0C cx 0 / +0x0E cy 489, +0x218 px 832 / +0x21A py 256)
   -> engine/src/gen/dining_r38_schluessel.inc
   Icon-CLUT: RE1.5 und RE2 unterscheiden sich nur in Eintrag 0 (Runde 36 Spur Q 2.4); gemessen wird hier, ob
   Index 0 in Tile 91 vorkommt (muss 0 sein, sonst Abbruch).
2. KLOPF-BANK = RE2 ROOM1060 snd0 (die Huette "Zombies are inside...!!"; sub05 @0x0F26 schlaegt in einer Schleife
   Satz 0x0A/0x0B), bytegleich aus der RDT (Kopfworte +0x08/+0x0C/+0x10 = EDT 0x3F58 / VH 0x4018 / VB 0x4C38)
   -> shared_assets/RE2/KLOPF1060.EDT/.VH/.VB (Muster tools/r35_fenster/glas1090_extrakt.py).
"""
import hashlib, io, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
RE2 = os.path.join(REPO, "info", "re2leon")
ZIEL_INC = os.path.join(PORT, "engine", "src", "gen", "dining_r38_schluessel.inc")
ZIEL_RE2 = os.path.join(PORT, "shared_assets", "RE2")

RE2_ID = 0x5B
QUELLEN = (
    ("re15_r38f_key_md1",  "PL0/RDT/ROOM2020.RDT", 0x003474, 236,   "4b543edc8c8cbee84e6e27565503cab1"),
    ("re15_r38f_key_tim",  "PL0/RDT/ROOM2020.RDT", 0x01A160, 34848, "6b864a6f02ffd6c0720be1efa654c3cf"),
    ("re15_r38f_key_icon", "COMMON/DATA/ITEMALL.PIX", RE2_ID * 1200, 1200, None),
    ("re15_r38f_key_itps", "COMMON/DATA/ITPS.ITP", RE2_ID * 0x3000, 0x3000, None),
)
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


def inc_text():
    daten = []
    for name, datei, off, n, soll in QUELLEN:
        b, md5 = quelle(datei, off, n, soll)
        if name.endswith("_icon"):
            null = sum(1 for x in b if x == 0)
            print("Icon Tile %d: Index 0 kommt %d-mal vor" % (RE2_ID, null))
            if null:
                sys.exit("ABBRUCH: Icon nutzt CLUT-Index 0 (RE1.5/RE2-CLUT unterscheiden sich dort)")
        if name.endswith("_itps"):
            b = itps_rechtecke(b, name)
        daten.append((name, bytes(b), datei, off, md5))
    f = io.StringIO()
    f.write("/* GENERIERT von re15_port/tools/r38_dining/re2_export.py - NICHT HAND-EDITIEREN.\n"
            " *\n"
            " * Runde 38 Spur F: Weltmodell, Icon und Item-Bild des Dining Room Key (= RE2 Item 0x5B, Herz-Schluessel)\n"
            " * aus RE2 Retail (info/re2leon). Dossier: analysis/befunde_runde38/F_dining1141.md.\n"
            " * Quelle je Feld (Datei-Offset, Laenge, md5 der GELESENEN Bytes; _itps mit umgesetzten Rechtecken):\n *\n")
    for name, b, datei, off, md5 in daten:
        f.write(" *   %-20s %s @0x%06X (%d B, md5 %s)\n" % (name, datei, off, len(b), md5))
    f.write(" *\n * md5 der Quelldateien:\n")
    for datei in sorted(DATEI_MD5):
        f.write(" *   %-24s %s\n" % (datei, DATEI_MD5[datei]))
    f.write(" */\n\n")
    for name, b, _d, _o, _m in daten:
        carr(name, b, f)
    return f.getvalue(), daten


def klopf_bank():
    rdt = open(os.path.join(RE2, "PL0", "RDT", "ROOM1060.RDT"), "rb").read()
    u32 = lambda o: struct.unpack_from("<I", rdt, o)[0]
    e, h, v = u32(8), u32(12), u32(16)
    if (e, h, v) != (0x3F58, 0x4018, 0x4C38):
        sys.exit("ABBRUCH: ROOM1060 Kopfworte %x %x %x" % (e, h, v))
    vh = rdt[h:v]
    if vh[:4] != b"pBAV":
        sys.exit("ABBRUCH: kein pBAV")
    ps = struct.unpack_from("<H", vh, 0x12)[0]
    vs = struct.unpack_from("<H", vh, 0x16)[0]
    groessen = [struct.unpack_from("<H", vh, 0x820 + ps * 0x200 + 2 * i)[0] * 8 for i in range(256)]
    vb_len = sum(groessen[1:vs + 1])
    teile = {"KLOPF1060.EDT": rdt[e:h], "KLOPF1060.VH": vh, "KLOPF1060.VB": rdt[v:v + vb_len]}
    # Gegenprobe: der Extrakt info/re2leon/PL0/RDT/room1060/snd0.* ist bytegleich
    for ext, name in (("edt", "KLOPF1060.EDT"), ("vh", "KLOPF1060.VH"), ("vb", "KLOPF1060.VB")):
        p = os.path.join(RE2, "PL0", "RDT", "room1060", "snd0." + ext)
        if os.path.exists(p):
            ex = open(p, "rb").read()
            print("Gegenprobe snd0.%s: %s" % (ext, "gleich" if ex == teile[name] else
                                              "ABWEICHUNG (%d vs %d B)" % (len(ex), len(teile[name]))))
    # Satz 0x0A / 0x0B (sub05 +0x22 / +0x32) muessen belegt sein
    for satz in (0x0A, 0x0B):
        rec = teile["KLOPF1060.EDT"][satz * 4:satz * 4 + 4]
        print("KLOPF1060 Satz 0x%02X = %s" % (satz, rec.hex(" ")))
        if rec[2] == 0 and rec[3] == 0:
            sys.exit("ABBRUCH: Satz leer")
    return teile


def main():
    pruefen = "--pruefen" in sys.argv
    fehler = 0
    text, daten = inc_text()
    for name, b, datei, off, md5 in daten:
        print("%-20s %-24s @0x%06X %6d B md5 %s" % (name, datei, off, len(b), md5))
    if pruefen:
        ok = os.path.exists(ZIEL_INC) and open(ZIEL_INC, encoding="utf-8").read() == text
        print("%s %s" % (ZIEL_INC, "ok" if ok else "ANDERS"))
        fehler += 0 if ok else 1
    else:
        with io.open(ZIEL_INC, "w", encoding="utf-8", newline="\n") as f:
            f.write(text)
        print("->", ZIEL_INC)
    for name, b in klopf_bank().items():
        p = os.path.join(ZIEL_RE2, name)
        if pruefen:
            ok = os.path.exists(p) and open(p, "rb").read() == b
            print("%-16s %s" % (name, "ok" if ok else "FEHLT/ANDERS"))
            fehler += 0 if ok else 1
        else:
            open(p, "wb").write(b)
            print("-> %-16s %6d B md5 %s" % (name, len(b), hashlib.md5(b).hexdigest()))
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
