# -*- coding: utf-8 -*-
"""Messung 26: die Freistellung 07_01 als Bild — welche Teile sind es, und wo steht
jeder Teil? Zusammenhangskomponenten der Alpha-Maske, je Komponente der Sockel
(unterste Zeile), sein Weltpunkt und seine Kamera-z."""
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, "re15_port/tools/maske")
import geom                                     # noqa: E402

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
CD = os.path.join("re15_port", "shared_assets", "PSX")
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
rdt, _ = geom.load_rdt(CD, "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)

a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
png = np.zeros((240, 320), bool)
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

lab = np.zeros((240, 320), int)
n = 0
komp = []
for y0 in range(240):
    for x0 in range(320):
        if png[y0, x0] and lab[y0, x0] == 0:
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
                        if 0 <= ny < 240 and 0 <= nx < 320 and png[ny, nx] and lab[ny, nx] == 0:
                            lab[ny, nx] = n
                            st.append((ny, nx))
            komp.append(pts)
komp.sort(key=len, reverse=True)
print("Alpha-Komponenten der Freistellung 07_01: %d" % len(komp))
print("  #  Punkte  bbox            Sockel (Spalte,Zeile) -> Welt        Kamera-z  Tiefe")
for i, pts in enumerate(komp):
    if len(pts) < 10:
        continue
    ys = np.array([p[0] for p in pts]); xs = np.array([p[1] for p in pts])
    j = ys.argmax()
    cxm = float(xs.mean()) + 0.5
    sy = float(ys.max()) + 0.5
    w = geom.welt_am_boden(R, t, H, cxm, min(sy, 239.5), 0)
    vz = geom.vz_at_floor(R, t, H, cxm, min(sy, 239.5), 0)
    print("%3d  %5d  x%3d..%3d y%3d..%3d  (%.1f,%.1f) -> (%6d,%6d)  %7.0f  %3d"
          % (i, len(pts), xs.min(), xs.max(), ys.min(), ys.max(), cxm, sy,
             w[0] if w else 0, w[1] if w else 0, vz or 0,
             int(1023 * vz / 65536.0) if vz else 0))

hell = (np.clip((BG / 255.0) ** 0.45, 0, 1) * 255).astype(np.uint8)
farben = [(255, 40, 40), (40, 255, 255), (255, 255, 40), (255, 40, 255), (40, 255, 40),
          (255, 150, 0), (150, 150, 255), (255, 255, 255)]
vis = hell.copy()
for i, pts in enumerate(komp):
    if len(pts) < 10:
        continue
    for (y, x) in pts:
        vis[y, x] = farben[i % len(farben)]
im = Image.fromarray(vis).crop((0, 110, 90, 240))
S = 10
im = im.resize((90 * S, 130 * S), Image.NEAREST)
dr = ImageDraw.Draw(im)
for i, pts in enumerate(komp):
    if len(pts) < 10:
        continue
    ys = np.array([p[0] for p in pts]); xs = np.array([p[1] for p in pts])
    dr.text((xs.min() * S, (ys.min() - 110) * S), str(i), fill=(0, 0, 0))
im.save(OUT + "41_alpha_komponenten.png")
print("41_alpha_komponenten.png (Ausschnitt x0..89 y110..239, %dx)" % S)
