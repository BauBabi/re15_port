#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 35 Spur F, Punkt 5: Zensus der ORIGINAL-Platzierungen von "Shotgun Shells" (Item 0x16) in
allen 240 RE1.5-RDTs und Export des Weltmodells als eingebackene Engine-Bytes
(re15_port/engine/src/gen/r35_schrot_prop.inc). KEIN Asset-Patch.

Item-Id 0x16 = "Shotgun Shells": DEBUG.BIN Offsettabelle @0x495C + 2*0x16 = @0x4988 -> Namensblob
DAT_800c4a28 (Datei 0x4A28) + Offset (siehe F_inhalt.md Punkt 5).
Item_aot_set (RE1.5 Op 0x50, 22 Byte, Kurzform): +14 Item, +16 Menge, +18 Zone-9-Bit, +20 Prop
(irons_tisch.h: `lhu a1,18(a2)` @0x80040680, `lbu s1,20(a2)` @0x80040684, Vorschub 22 @0x80040688).
Prop = Index in die RDT+0x30-Modelltabelle (je Prop TIM- und MD1-Offset; Scheibe bis zur naechsten
Grenze wie rdt_common.c parse_props / rdt_next_boundary).

Aufruf: C:/Python310/python.exe re15_port/tools/r35_inhalt/schrot_export.py [--schreiben]
"""
import collections
import glob
import hashlib
import os
import struct
import subprocess
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
CD = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "r35_schrot_prop.inc")
DUMP = os.path.join(REPO, "re15_port", "tools", "scd_dump_room.py")


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def grenze(d, start, tbl, n):
    best = len(d)
    for h in range(0x08, 0x5C + 1, 4):
        p = u32(d, h)
        if start < p < best:
            best = p
    for i in range(n):
        p = u32(d, tbl + 4 * i)
        if start < p < best:
            best = p
    return best


def prop(d, k):
    nom = d[2]
    tbl = u32(d, 0x30)
    n = nom * 2
    tim = u32(d, tbl + 8 * k)
    md1 = u32(d, tbl + 8 * k + 4)
    return (md1, d[md1:grenze(d, md1, tbl, n)]), (tim, d[tim:grenze(d, tim, tbl, n)])


def zensus():
    treffer = []
    for f in sorted(glob.glob(os.path.join(CD, "STAGE*", "ROOM*.RDT"))):
        out = subprocess.run([sys.executable, DUMP, f], capture_output=True, text=True).stdout
        d = open(f, "rb").read()
        for z in out.splitlines():
            if "Item_aot_set" not in z:
                continue
            teile = z.split()
            off = int(teile[0], 16)
            b = bytes(int(x, 16) for x in teile[3:25])
            if len(b) < 22 or b[0] != 0x50:
                continue
            item, menge, bit, pr = struct.unpack_from("<H", b, 14)[0], struct.unpack_from("<H", b, 16)[0], \
                struct.unpack_from("<H", b, 18)[0], b[20]
            if item != 0x16:
                continue
            md5 = "-"
            if pr < d[2]:
                (mo, mb), (to, tb) = prop(d, pr)
                md5 = hashlib.md5(mb).hexdigest()
            treffer.append((os.path.basename(f), off, menge, bit, pr, md5))
    return treffer


def inc_zeile(name, b):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), 16):
        z.append("    " + ",".join("0x%02x" % x for x in b[i:i + 16]) + ",")
    z.append("};")
    return "\n".join(z)


def main():
    t = zensus()
    print("Shotgun Shells (0x16) Item_aot_set: %d Saetze" % len(t))
    for r in t:
        print("  %s @0x%05X Menge %d Bit %d Prop %d MD1-md5 %s" % r)
    mengen = collections.Counter(r[2] for r in t)
    meshes = collections.Counter(r[5] for r in t if r[5] != "-")
    print("Mengen:", mengen.most_common())
    print("Meshes:", meshes.most_common())
    if "--schreiben" not in sys.argv:
        return
    md5_ziel = meshes.most_common(1)[0][0]
    quelle = next(r for r in t if r[5] == md5_ziel)
    stage = quelle[0][4]
    d = open(os.path.join(CD, "STAGE%s" % stage, quelle[0]), "rb").read()
    (mo, mb), (to, tb) = prop(d, quelle[4])
    kopf = ("/* AUTOMATISCH ERZEUGT von re15_port/tools/r35_inhalt/schrot_export.py - nicht von Hand aendern.\n"
            " * Weltmodell \"Shotgun Shells\" (Item 0x16) = das haeufigste Original-Mesh der %d Saetze:\n"
            " *   MD1 %s @0x%06X %d B md5 %s\n"
            " *   TIM %s @0x%06X %d B md5 %s\n"
            " * (Prop %d der RDT+0x30-Tabelle; Satz %s @0x%05X). UNVERAENDERT. */\n"
            % (len(t), quelle[0], mo, len(mb), hashlib.md5(mb).hexdigest(),
               quelle[0], to, len(tb), hashlib.md5(tb).hexdigest(), quelle[4], quelle[0], quelle[1]))
    open(ZIEL, "w", newline="\n").write(kopf + inc_zeile("re15_r35_schrot_md1", mb) + "\n" +
                                        inc_zeile("re15_r35_schrot_tim", tb) + "\n")
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
