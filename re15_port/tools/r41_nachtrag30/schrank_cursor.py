#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""schrank_cursor.py - Runde 41 Spur N, Nachbesserung 1 (Mangel 1): Cursor-Stempel des Einsetz-Schritts ROOM2010/2011.

Der Cursor in ROOM2060 Cut 8 ist obj 4 (Obj_model_set @0x00F12); der Port zeigt die Ansicht in 2010 als Vollbild-Ebene, also wird das Objekt als Stempel gezeichnet.
Quelle: Spielbild ROOM2060 Cut 8 (F500, Cursor an der Startlage, Laufmitte (159,119)) gegen den Hintergrund BG08 (probe_bg_dump): genau die Bildpunkte, in denen beide abweichen.
Erzeugt engine/src/gen/schrank2010_cursor_r41.inc und den Beleg analysis/befunde_runde41/N_belege/schrank_cursor_2060_cut8.png.

Aufruf: python re15_port/tools/r41_nachtrag30/schrank_cursor.py <spielbild.ppm> <ROOM20608.ppm>
"""
import os
import sys
import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")
BELEG = os.path.join(REPO, "analysis", "befunde_runde41", "N_belege", "schrank_cursor_2060_cut8.png")
MX, MY = 159, 119       # Cursor-Mitte an der Startlage: Rahmen x 147..171, y 108..130 (Lauf c_fein)


def main():
    a = np.array(Image.open(sys.argv[1]).convert("RGB")).astype(np.int32)
    b = np.array(Image.open(sys.argv[2]).convert("RGB")).astype(np.int32)
    ys, xs = np.nonzero(np.abs(a - b).sum(2))
    assert 140 <= len(xs) <= 160 and xs.min() >= 140 and xs.max() <= 180 and ys.min() >= 100 and ys.max() <= 140, (len(xs), xs.min(), xs.max(), ys.min(), ys.max())
    z = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_nachtrag30/schrank_cursor.py - nicht von Hand aendern.",
         " * Cursor ROOM2060 obj 4 unter Cut 8, gemessen am Spielbild gegen BG08; Mitte (%d,%d). */" % (MX, MY),
         "typedef struct { signed char dx, dy; unsigned char r, g, b; } re15_r41n_stempelpx_t;",
         "static const re15_r41n_stempelpx_t re15_r41n_schrank_cursor[%d] = {" % len(xs)]
    for x, y in zip(xs, ys):
        z.append("    { %d, %d, %d, %d, %d }," % (x - MX, y - MY, a[y, x, 0], a[y, x, 1], a[y, x, 2]))
    z.append("};")
    ziel = os.path.join(GEN, "schrank2010_cursor_r41.inc")
    open(ziel, "w", newline="\n").write("\n".join(z) + "\n")
    print("geschrieben:", ziel, len(xs), "Bildpunkte")
    ausschnitt = np.zeros((31, 33, 3), np.uint8)
    for x, y in zip(xs, ys):
        ausschnitt[y - MY + 15, x - MX + 16] = a[y, x]
    Image.fromarray(ausschnitt).resize((33 * 6, 31 * 6), Image.NEAREST).save(BELEG)


if __name__ == "__main__":
    main()
