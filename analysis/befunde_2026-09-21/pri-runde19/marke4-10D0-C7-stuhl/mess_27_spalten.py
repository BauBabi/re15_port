# -*- coding: utf-8 -*-
"""Messung 27: Spaltenweiser Bodenkontakt der Freistellung 07_01.

Wenn die Freistellung ZWEI Gegenstaende in verschiedener Entfernung enthaelt,
zeigt sich das am spaltenweisen Bodenpunkt: innerhalb eines Gegenstands aendert
er sich glatt, an der Grenze springt er. (Gleiches Verfahren wie ROOM10D0 C1,
Eintrag "bodenkante" in auswahl.json.)
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import geom                                     # noqa: E402

CD = os.path.join("re15_port", "shared_assets", "PSX")
rdt, _ = geom.load_rdt(CD, "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
png = np.zeros((240, 320), bool)
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

print("Spalte | unterste Zeile | Weltpunkt (x,z) | Kamera-z | Tiefe | Sprung z zur Vorspalte")
vor = None
for x in range(0, 80):
    r = np.nonzero(png[:, x])[0]
    if len(r) == 0:
        continue
    sy = min(int(r.max()), 239)
    w = geom.welt_am_boden(R, t, H, x + 0.5, sy + 0.5, 0)
    vz = geom.vz_at_floor(R, t, H, x + 0.5, sy + 0.5, 0)
    sp = "" if vor is None else "%+8d" % (w[1] - vor)
    print("  %3d  |  %3d  | (%7d,%7d) | %7.0f | %3d | %s"
          % (x, sy, w[0], w[1], vz, int(1023 * vz / 65536.0), sp))
    vor = w[1]
