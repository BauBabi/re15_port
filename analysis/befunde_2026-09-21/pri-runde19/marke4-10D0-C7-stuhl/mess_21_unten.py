# -*- coding: utf-8 -*-
"""Messung 21: der UNTERE Bereich (y160..200) — dort liegt der Stuhl VOR dem Spieler.

Kriterium fuer "vor dem Spieler" ist der Bodenkontakt auf dem BILDSCHIRM: der Fuss
des Spielers liegt an Marke 4 auf scr y=179 (befund.log). Ein auf dem Boden stehender
Gegenstand, dessen Sockel UNTER y=179 liegt, ist naeher als der Spieler.
Zusaetzlich die Kamera-z-Rechnung je Spalte.
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402
import maskenbild                               # noqa: E402

CD = os.path.join("re15_port", "shared_assets", "PSX")
MASKS = "re15_port/shared_assets/PSX/MASKS"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)
rdt, _ = geom.load_rdt(CD, "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
blob = open(os.path.join(MASKS, "ROOM10D0.MSK"), "rb").read()
ms = maskenbild.masken(blob, 7) or []
idx = maskenbild.lies_tim(os.path.join(MASKS, "ROOM10D0_PRI07.TIM"))[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)

a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
png = np.zeros((240, 320), bool)
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

d = np.abs(REN - BG).sum(2) > 30
verloren = d & ~deck

print("Fuss des Spielers: scr y=179, vz 6073 -> Grenze Tiefe < 94")
print("y  | PNG-xmax | Deckung-xmax | verlorene x | Kamera-z am Boden dieser Zeile | Tiefe")
for y in range(160, 200):
    px = np.nonzero(png[y])[0]
    cx = np.nonzero(deck[y])[0]
    vx = np.nonzero(verloren[y])[0]
    vz = geom.vz_at_floor(R, t, H, 64, y, 0)
    print("%3d| %4s | %4s | %-18s | %7.0f | %3d"
          % (y, px.max() if len(px) else "-", cx.max() if len(cx) else "-",
             ("%d..%d (%d)" % (vx.min(), vx.max(), len(vx))) if len(vx) else "-",
             vz if vz else 0, int(1023 * vz / 65536.0) if vz else 0))

print()
print("Zeile y183..189, Spalten 56..72: HG / Render / Maskentiefe")
for y in range(180, 192):
    z = []
    for x in range(56, 73):
        z.append("%3d/%3d/%-3s" % (BG[y, x].sum(), REN[y, x].sum(),
                                   str(tief[y, x]) if deck[y, x] else "-"))
    print("y%3d " % y + " ".join(z))
