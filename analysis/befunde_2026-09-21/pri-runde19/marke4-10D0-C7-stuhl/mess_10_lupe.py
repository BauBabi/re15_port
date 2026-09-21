# -*- coding: utf-8 -*-
"""Messung 10: Lupe mit Koordinatengitter. Links Hintergrund (gamma 0.45),
Mitte derselbe Hintergrund mit der Kante der Nutzer-Freistellung (rot) und der
PBM-Deckung (gruen umrandet), rechts der Render."""
import numpy as np
from PIL import Image, ImageDraw

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), np.uint8)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), np.uint8)
COV = ~np.asarray(Image.open("re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM"))


def g(a):
    return (np.clip((a / 255.0) ** 0.45, 0, 1) * 255).astype(np.uint8)


BOX = (50, 118, 86, 172)
S = 22
X0, Y0, X1, Y1 = BOX


def panel(arr, marken=None):
    im = Image.fromarray(arr).crop(BOX).resize(((X1 - X0) * S, (Y1 - Y0) * S), Image.NEAREST)
    dr = ImageDraw.Draw(im)
    if marken is not None:
        for (y, x, col) in marken:
            if X0 <= x < X1 and Y0 <= y < Y1:
                dr.rectangle([(x - X0) * S, (y - Y0) * S,
                              (x - X0) * S + S - 1, (y - Y0) * S + S - 1], outline=col)
    for x in range(X0, X1, 5):
        dr.line([(x - X0) * S, 0, (x - X0) * S, im.height], fill=(90, 90, 90))
        dr.text(((x - X0) * S + 2, 2), str(x), fill=(255, 255, 0))
    for y in range(Y0, Y1, 5):
        dr.line([0, (y - Y0) * S, im.width, (y - Y0) * S], fill=(90, 90, 90))
        dr.text((2, (y - Y0) * S + 2), str(y), fill=(255, 255, 0))
    return im


a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
png = np.zeros((240, 320), bool)
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

mk = []
for y in range(Y0, Y1):
    for x in range(X0, X1):
        if png[y, x]:
            mk.append((y, x, (255, 60, 60)))
        elif COV[y, x]:
            mk.append((y, x, (60, 255, 60)))

ims = [panel(g(BG)), panel(g(BG), mk), panel(g(REN))]
w, h = ims[0].size
comb = Image.new("RGB", (w * 3 + 12, h), (255, 255, 0))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "37_lupe_gitter.png")
print("BOX", BOX, comb.size)
print("rot = Nutzer-PNG 07_01, gruen = Deckung nur aus Quader/07_02")
