# -*- coding: utf-8 -*-
"""Messung 15: Wo STEHT der blaugraue Kasten, und liegt er vor dem Spieler?

Silhouette aus dem Hintergrund nach der in Messung 14 belegten Regel
(b-r >= 0 UND Helligkeit >= 120; Boden-Fehlalarm 0 von 609 Punkten).
Dann: Bodenkontakt je Spalte -> Weltpunkt -> Kamera-z, gegen den Spieler an
Marke 4 (pos 418/0/26497, Fuss-vz 6073, Kopf-vz 5692).
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import geom                                     # noqa: E402

CD = os.path.join("re15_port", "shared_assets", "PSX")
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)
br = BG[..., 2] - BG[..., 0]

rdt, _ = geom.load_rdt(CD, "ROOM10D0")
import struct                                   # noqa: E402
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
print("Kamera: H=%d t=(%d,%d,%d)" % (H, t[0], t[1], t[2]))

roh = np.zeros((240, 320), bool)
roh[124:159, 50:74] = (br[124:159, 50:74] >= 0) & (lum[124:159, 50:74] >= 120)
print("Rohregel im Fenster x50..73 y124..158: %d Punkte" % roh.sum())

# groesste 8er-Komponente
lab = np.zeros((240, 320), int)
n = 0
komp = []
for y0 in range(240):
    for x0 in range(320):
        if roh[y0, x0] and lab[y0, x0] == 0:
            n += 1
            st = [(y0, x0)]
            lab[y0, x0] = n
            pts = []
            while st:
                y, x = st.pop()
                pts.append((y, x))
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        ny, nx = y + dy, x + dx
                        if 0 <= ny < 240 and 0 <= nx < 320 and roh[ny, nx] and lab[ny, nx] == 0:
                            lab[ny, nx] = n
                            st.append((ny, nx))
            komp.append(pts)
komp.sort(key=len, reverse=True)
print("Komponenten:", [len(k) for k in komp[:6]])
K = np.zeros((240, 320), bool)
for (y, x) in komp[0]:
    K[y, x] = True
ys, xs = np.nonzero(K)
print("groesste Komponente %d Punkte, x%d..%d y%d..%d" % (K.sum(), xs.min(), xs.max(), ys.min(), ys.max()))

print()
print("Spalte | unterste Silhouettenzeile | Weltpunkt am Boden y=0 | Kamera-z | Tiefenstufe")
for x in range(xs.min(), xs.max() + 1):
    r = np.nonzero(K[:, x])[0]
    if len(r) == 0:
        continue
    sy = r.max()
    w = geom.welt_am_boden(R, t, H, x, sy, 0)
    vz = geom.vz_at_floor(R, t, H, x, sy, 0)
    print("  %3d  |  %3d  |  (%7d, %7d) | %7.0f | %3d"
          % (x, sy, w[0], w[1], vz, int((1023 * vz) / 65536.0)))

print()
FUSS, KOPF = 6073.0, 5692.0
print("Spieler an Marke 4: Fuss-vz %.0f -> wirksame Grenze Tiefe < %d; Kopf-vz %.0f -> < %d"
      % (FUSS, int(1023 * FUSS / 65536.0), KOPF, int(1023 * KOPF / 65536.0)))
