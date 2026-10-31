#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur O "keyitems": die RE2-Bytes der Kurbel als C-Einbindung engine/src/gen/keyitems_r41.inc.

Dossier: analysis/befunde_runde41/O_keyitems.md. Muster: tools/r36_schluessel/schluessel_export.py.

WARUM EINGEBACKENE BYTES UND KEIN ASSET-PATCH: ITEMALL.PIX/ITPS.ITP bleiben byte-true; Icon und Bild werden im GELADENEN
Puffer eingesetzt (re15_keyitems_r41_icon_einsetzen / _bild_einsetzen, dieselben Ladestellen wie re15_sicherung.h).

DIE QUELLEN (RE2 Retail, info/re2leon):
  CRANK = RE2 Item 0x43 (Namensblob PSX.EXE Datei @0x8F041 "Crank", Tile == Id wie in schluessel_export.py)
    Icon     COMMON/DATA/ITEMALL.PIX Tile 67 = @0x13A10 (67 x 1200), 1200 B
    Bild     COMMON/DATA/ITPS.ITP    Block 67 = @0xC9000 (67 x 0x3000), 0x3000 B
    Name     "Crank"                PSX.EXE Datei @0x8F041 (Klartext-Glyphen wie RE1.5, 0x07 statt 0xF7 als Ende)
    CHECK    "The end is" / "square-shaped"   PSX.EXE Datei @0x9206D `30 44 41 00 41 4a 40 00 45 4f fc 4f 4d 51 3d 4e 41 3b 4f 44
             3d 4c 41 40 01` (0xFC = RE2-Umbruch -> 0x08 in der RE1.5-Beschreibungsbank, Ende `01 00`)

EINZIGE DATEN-AENDERUNG (wie schluessel_export.py): die zwei Rechtecke im TIM-Kopf des Item-Bilds. RE2 laedt an crect (0,480) /
prect (0,0), RE1.5s CHECK-Foto-Lader @0x800c0258 an die EINGEBETTETEN Rechtecke (0,489) / (832,256):
  Block+0x0C u16 cx=0, +0x0E u16 cy=489 und (Bildblock @+0x214, Laenge u32) Block+0x218 u16 px=832, +0x21A u16 py=256.

    python re15_port/tools/r41_keyitems/keyitems_export.py [--pruefen]
"""
import hashlib, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
RE2 = os.path.join(REPO, "info", "re2leon")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "keyitems_r41.inc")


def lies(rel):
    return open(os.path.join(RE2, rel.replace("/", os.sep)), "rb").read()


def rechtecke(b):
    assert b[:8] == bytes.fromhex("1000000009000000"), "kein 8bpp-TIM"
    cx, cy = struct.unpack_from("<HH", b, 0x0C)
    px, py = struct.unpack_from("<HH", b, 0x218)
    assert (cx, cy, px, py) == (0, 480, 0, 0), "unerwartete Rechtecke %r" % ((cx, cy, px, py),)
    b = bytearray(b)
    struct.pack_into("<HH", b, 0x0C, 0, 489)
    struct.pack_into("<HH", b, 0x218, 832, 256)
    return bytes(b)


def carr(name, b):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), 24):
        z.append("    " + ",".join("0x%02x" % x for x in b[i:i + 24]) + ",")
    z.append("};")
    return "\n".join(z)


def main():
    exe = lies("PSX.EXE")
    # Name: Glyphen wie RE1.5, in RE2 durch 0xF7 getrennt
    i = exe.index(bytes([0x1F, 0x4E, 0x3D, 0x4A, 0x47, 0xF7]))     # "Crank" + Ende
    assert i == 0x8F041, hex(i)
    name = bytes([0x1F, 0x4E, 0x3D, 0x4A, 0x47, 0x07])
    # CHECK: @0x9206D "The end is" 0xFC "square-shaped" 0x01
    d = exe[0x9206D:0x9206D + 25]
    assert d == bytes.fromhex("30444100414a4000454f" "fc" "4f4d513d4e413b4f443d4c4140" "01"), d.hex()
    desc = d[:-1].replace(b"\xfc", b"\x08") + b"\x01\x00"

    pix = lies("COMMON/DATA/ITEMALL.PIX")
    itps = lies("COMMON/DATA/ITPS.ITP")
    icon = pix[67 * 1200:68 * 1200]
    bild = rechtecke(itps[67 * 0x3000:68 * 0x3000])

    t = ["/* ERZEUGT von tools/r41_keyitems/keyitems_export.py - NICHT von Hand aendern. RE2-Quellen im Werkzeug-Kopf.",
         " * ITEMALL.PIX md5 %s, ITPS.ITP md5 %s */" % (hashlib.md5(pix).hexdigest(), hashlib.md5(itps).hexdigest()),
         carr("re15_r41o_kurbel_name", name),
         carr("re15_r41o_kurbel_desc", desc),
         carr("re15_r41o_kurbel_icon", icon),
         carr("re15_r41o_kurbel_itps", bild),
         ""]
    neu = "\n".join(t).replace("\n", "\n")
    if "--pruefen" in sys.argv:
        alt = open(ZIEL, encoding="ascii", newline="").read().replace("\r\n", "\n")
        if alt != neu:
            sys.exit("gen/keyitems_r41.inc weicht ab")
        print("keyitems_r41.inc aktuell")
        return
    open(ZIEL, "w", encoding="ascii", newline="\n").write(neu)
    print("geschrieben", ZIEL, "Index 0 im Icon:", icon.count(0))


main()
