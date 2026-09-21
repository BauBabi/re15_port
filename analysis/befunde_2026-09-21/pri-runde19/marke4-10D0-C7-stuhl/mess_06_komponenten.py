# -*- coding: utf-8 -*-
"""Messung 6: die verlorenen Punkte in Zusammenhangskomponenten, mit Bildbeleg.

Abdeckung der Schiene: sie sieht JEDEN Punkt des 320x240-Bildes; gemeldet werden
alle Komponenten >= 20 Punkte. Die Zahl der gesehenen/gemeldeten Punkte steht dabei.
"""
import numpy as np
from PIL import Image, ImageDraw

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
COV = ~np.asarray(Image.open("re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM"))

d = np.abs(REN - BG).sum(2) > 30
verloren = d & ~COV
print("Abdeckung: 76800 Bildpunkte geprueft, %d uebermalt, %d davon ohne Deckung"
      % (d.sum(), verloren.sum()))

# 4er-Nachbarschaft, eigene Markierung (keine scipy-Abhaengigkeit)
lab = np.zeros((240, 320), int)
nxt = 0
komp = []
for y0 in range(240):
    for x0 in range(320):
        if verloren[y0, x0] and lab[y0, x0] == 0:
            nxt += 1
            stack = [(y0, x0)]
            lab[y0, x0] = nxt
            pts = []
            while stack:
                y, x = stack.pop()
                pts.append((y, x))
                for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    ny, nx = y + dy, x + dx
                    if 0 <= ny < 240 and 0 <= nx < 320 and verloren[ny, nx] and lab[ny, nx] == 0:
                        lab[ny, nx] = nxt
                        stack.append((ny, nx))
            komp.append(pts)

komp.sort(key=len, reverse=True)
print("%d Komponenten; gemeldet werden alle >= 20 Punkte" % len(komp))
print(" #  Punkte  bbox x..  y..     mittl. Hintergrundfarbe  max-Helligkeit")
gemeldet = 0
for i, pts in enumerate(komp):
    if len(pts) < 20:
        continue
    gemeldet += len(pts)
    ys = np.array([p[0] for p in pts]); xs = np.array([p[1] for p in pts])
    c = BG[ys, xs]
    print("%2d  %5d  x%3d..%3d y%3d..%3d   rgb(%3d,%3d,%3d)   max %d"
          % (i, len(pts), xs.min(), xs.max(), ys.min(), ys.max(),
             c[:, 0].mean(), c[:, 1].mean(), c[:, 2].mean(), c.sum(1).max()))
print("gemeldet %d von %d verlorenen Punkten (%.1f %%)"
      % (gemeldet, verloren.sum(), 100.0 * gemeldet / verloren.sum()))

# Bildbeleg: Komponenten nummeriert auf den aufgehellten Hintergrund
hell = (np.clip((BG / 255.0) ** 0.45, 0, 1) * 255).astype(np.uint8)
farben = [(255, 0, 0), (0, 255, 255), (255, 255, 0), (255, 0, 255), (0, 255, 0),
          (255, 128, 0), (128, 128, 255)]
vis = hell.copy()
for i, pts in enumerate(komp):
    if len(pts) < 20:
        continue
    ys = np.array([p[0] for p in pts]); xs = np.array([p[1] for p in pts])
    vis[ys, xs] = farben[i % len(farben)]
im = Image.fromarray(vis).resize((320 * 4, 240 * 4), Image.NEAREST)
dr = ImageDraw.Draw(im)
for i, pts in enumerate(komp):
    if len(pts) < 20:
        continue
    ys = np.array([p[0] for p in pts]); xs = np.array([p[1] for p in pts])
    dr.text((xs.min() * 4, ys.min() * 4 - 10), str(i), fill=(255, 255, 255))
im.save(OUT + "35_komponenten.png")
