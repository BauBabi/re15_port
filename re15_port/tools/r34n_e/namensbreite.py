# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Listennamen der vier Dokumente - Kodierung und Breite.

1. Kodierung: RE1.5-Glyphencode = ASCII - 0x24, Leerzeichen 0x00, Apostroph 0x3A, Ende 0x07 -
   belegt an "Chris' Diary" @0x800c4e04 (DEBUG.BIN Datei 0x04e04)
   `1f 44 4e 45 4f 3a 00 20 45 3d 4e 55 07` (C h r i s ' _ D i a r y).
2. Breite: Vorschubtabelle @0x800c4416 (DEBUG.BIN Datei 0x4416 + Code; FUN_80028ec4 `x += width[c]`
   @0x8002910c-24, dieselbe Tabelle re15_port/include/font_width.h).
3. Vergleich:
   - FILE-Liste: Zeile ab x 0x2c (emit_file_list, re15_inv_screen.c), Hervorhebungs-Kachel
     x 0x2b, w 0x9a (@0x800c749c-c0) -> Ende 196.
   - RE1.5s EIGENE Listennamen: Namensbank Ids 0x48..0x65 (Offsettabelle @0x800c495c, Blob
     @0x800c4a28, gen/item_prompt_data.inc) - die 30 vorinstallierten FILE-Zeilen.
   - Meldung "The <Name>" (Skript [5] @0x800c506f: `30 44 41 00 05 01 06 00 05 00 08 ...` = Zeile 1
     "The " + Name, Zeile 2 "has been filed."), gezeichnet ab x 34 (Box 0x22 @0x80027eec).
   - Blaue Tafel: aus einem Framedump der echten exe (FILE-Liste) gemessen, wenn angegeben.

Aufruf: python re15_port/tools/r34n_e/namensbreite.py [framedump_der_liste.ppm]
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
DBG = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "BIN", "DEBUG.BIN"), "rb").read()
W = DBG[0x4416:0x4416 + 256]

NAMEN = [("Dok 0", "Irons Diary"), ("Dok 1", "Police Officer's Final Diary Entry"),
         ("Dok 2", "Elliot's Diary"), ("Dok 3", "Marvin's Notes"), ("Dok 4", "Armory Notice")]


def kodiere(s):
    out = []
    for ch in s:
        if ch == " ":
            out.append(0x00)
        elif ch == "'":
            out.append(0x3A)
        else:
            c = ord(ch) - 0x24
            assert 0x01 <= c < 0x60, ch
            out.append(c)
    return bytes(out + [0x07])


def breite(b):
    return sum(W[c] for c in b if c != 0x07)


def dekodiere(b):
    s = ""
    for c in b:
        if c == 0x07:
            break
        s += " " if c == 0 else ("'" if c == 0x3A else chr(c + 0x24) if c < 0x60 else "<%02x>" % c)
    return s


def main():
    # Beleg der Kodierung an "Chris' Diary"
    chris = DBG[0x4e04:0x4e04 + 13]
    assert chris == kodiere("Chris' Diary"), chris.hex()
    print("Kodierung belegt: DEBUG.BIN 0x04e04 = %s = kodiere(\"Chris' Diary\")" % chris.hex(" "))
    the = kodiere("The ")[:-1]
    print("\nListe: Zeile ab x 44, Hervorhebung x 43..196 (w 0x9a @0x800c749c-c0)")
    for tag, n in NAMEN:
        b = kodiere(n)
        w = breite(b)
        wm = breite(the) + w
        print("  %s %-36s %2d Zeichen  %3d px -> Ende x %3d %s | Meldung Zeile 1 \"The %s\" %3d px -> Ende x %3d"
              % (tag, n, len(n), w, 44 + w - 1, "(ueber die Kachel hinaus um %d)" % (44 + w - 1 - 196) if 44 + w - 1 > 196 else "",
                 n, wm, 34 + wm - 1))
        print("        Bytes %s" % b.hex(" "))
    # RE1.5s eigene FILE-Zeilen (Ids 0x48..0x65)
    off = struct.unpack_from("<102H", DBG, 0x495c)
    blob = 0x4a28
    rows = []
    for i in range(0x48, 0x66):
        a = blob + off[i]
        e = DBG.index(b"\x07", a)
        rows.append((i, dekodiere(DBG[a:e + 1]), breite(DBG[a:e + 1])))
    rows.sort(key=lambda r: -r[2])
    print("\nRE1.5s eigene FILE-Zeilen (Namensbank Ids 0x48..0x65), breiteste zuerst:")
    for i, s, w in rows[:8]:
        print("  0x%02X %-30s %3d px -> Ende x %3d" % (i, s, w, 44 + w - 1))
    print("  ... schmalste %d px; %d von 30 breiter als die Kachel (> %d px)" % (rows[-1][2], sum(1 for r in rows if 44 + r[2] - 1 > 196), 196 - 44 + 1))
    # alle 102 Namen: breiteste Meldungszeile
    alle = []
    for i in range(102):
        a = blob + off[i]
        e = DBG.index(b"\x07", a)
        alle.append((i, dekodiere(DBG[a:e + 1]), breite(DBG[a:e + 1])))
    alle.sort(key=lambda r: -r[2])
    print("\nbreiteste Namen der ganzen Bank (fuer die Meldung):")
    for i, s, w in alle[:4]:
        print("  0x%02X %-30s %3d px" % (i, s, w))
    if len(sys.argv) > 1:
        from PIL import Image
        import numpy as np
        im = np.array(Image.open(sys.argv[1]).convert("RGB").resize((320, 240), Image.BOX)).astype(int)
        for y in (0x35 + 8, 0x35 + 16 * 5 + 8, 0x35 + 16 * 9 + 8):
            zeile = im[y]
            blau = [x for x in range(320) if zeile[x][2] > zeile[x][0] + 40 and zeile[x][2] > 60]
            print("Framedump Zeile y %d: blaue Tafel x %d..%d" % (y, min(blau), max(blau)))


if __name__ == "__main__":
    main()
