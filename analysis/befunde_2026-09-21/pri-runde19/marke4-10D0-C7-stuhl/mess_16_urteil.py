# -*- coding: utf-8 -*-
"""Messung 16: Punkt fuer Punkt — Deckung, Maskentiefe, Urteil, und was im Bild steht.

Quelle der Maskentiefe: die GESCHRIEBENEN Dateien (MASKS/ROOM10D0_PRI07.TIM + .MSK),
gelesen mit demselben Leser, den die Sonde nutzt (maskenbild + abnahme).
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import maskenbild                               # noqa: E402

MASKS = "re15_port/shared_assets/PSX/MASKS"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)

blob = open(os.path.join(MASKS, "ROOM10D0.MSK"), "rb").read()
ms = maskenbild.masken(blob, 7) or []
idx = maskenbild.lies_tim(os.path.join(MASKS, "ROOM10D0_PRI07.TIM"))[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)
print("%d Rechtecke, Deckung %d Punkte, Tiefen %d..%d"
      % (len(ms), deck.sum(), tief[deck].min(), tief[deck].max()))

FUSS = 6073.0
grenze = int(1023 * FUSS / 65536.0)
print("Spieler-Fuss vz %.0f -> Maske gewinnt, wenn Tiefe < %d" % (FUSS, grenze))

print()
print("Zeile: Deckung(Tiefe) je Spalte, x=50..80. '.' = keine Deckung")
print("      " + "".join("%4d" % x for x in range(50, 81, 2)))
for y in range(120, 190, 2):
    z = []
    for x in range(50, 81, 2):
        z.append("%4s" % (str(tief[y, x]) if deck[y, x] else "."))
    print("%3d   " % y + "".join(z))

print()
d = np.abs(REN - BG).sum(2) > 30
gew = deck & (tief < grenze)            # Maske gewinnt gegen den Fuss
print("Deckung mit Tiefe < %d (Maske gewinnt): %d Punkte" % (grenze, gew.sum()))
print("davon im Spielerkasten x59..95 y118..182: %d" % gew[118:183, 59:96].sum())
print("Deckung mit Tiefe >= %d (Spieler gewinnt): %d Punkte" % (grenze, (deck & ~gew).sum()))
print("uebermalt(>30) und ungedeckt: %d" % (d & ~deck).sum())
