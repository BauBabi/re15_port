# -*- coding: utf-8 -*-
"""Messung 33: Nachpruefung des Koerperkastens (eigener Nachvollzug).

Die gezeichnete Figur = Punkte, an denen der Render vom Hintergrund abweicht, ohne
die Komponente der Leiche im Flur (x136..173, y102..129, aus mess_06 belegt).
Dann: welche Welthoehe ueber dem Fuss entspricht der obersten Figurzeile?
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import geom                                     # noqa: E402

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)
rdt, _ = geom.load_rdt(os.path.join("re15_port", "shared_assets", "PSX"), "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
PX, PZ = 418, 26497


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


for thr in (12, 20, 30):
    d = np.abs(REN - BG).sum(2) > thr
    d[:, 120:200] = False          # Flur mit der Leiche ausblenden
    ys, xs = np.nonzero(d)
    print("Schwelle >%d: %d Punkte, x%d..%d y%d..%d"
          % (thr, d.sum(), xs.min(), xs.max(), ys.min(), ys.max()))

d = np.abs(REN - BG).sum(2) > 30
d[:, 120:200] = False
ys, xs = np.nonzero(d)
oben, unten = int(ys.min()), int(ys.max())
print()
print("gezeichnete Figur (Schwelle >30, Flur ausgeblendet): Zeilen %d..%d, Spalten %d..%d"
      % (oben, unten, xs.min(), xs.max()))
print("Kasten aus befund.log: x59..95, y118..182 -> %d von %d Zeilen = %.0f %%"
      % (182 - 118 + 1, unten - oben + 1, 100.0 * (182 - 118 + 1) / (unten - oben + 1)))
figur = np.zeros((240, 320), bool)
figur[d] = True
K = np.zeros((240, 320), bool)
K[118:183, 59:96] = True
print("Figurpunkte im Log-Kasten: %d von %d = %.1f %%"
      % ((figur & K).sum(), figur.sum(), 100.0 * (figur & K).sum() / figur.sum()))

print()
print("Welthoehe ueber dem Fuss -> Bildzeile (Kamera von Cut 7):")
for hoehe in (1500, 1700, 2000, 2500, 3000, 3200):
    sx, sy, vz = proj(PX, -hoehe, PZ)
    print("   %5d -> Zeile %6.1f %s" % (hoehe, sy, "<-- oberste Figurzeile %d" % oben
                                        if abs(sy - oben) <= 3 else ""))
sx, sy, vz = proj(PX, 0, PZ)
print("       0 -> Zeile %6.1f (Fuss; befund.log scr y=179)" % sy)
