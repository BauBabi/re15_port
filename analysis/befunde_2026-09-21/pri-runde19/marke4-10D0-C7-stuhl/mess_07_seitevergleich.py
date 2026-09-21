# -*- coding: utf-8 -*-
"""Messung 7: Hintergrund und Render am ganzen Stuhl, gleich aufgehellt (gamma 0.45)."""
import numpy as np
from PIL import Image

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), np.uint8)
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), np.uint8)


def g(a):
    return (np.clip((a / 255.0) ** 0.45, 0, 1) * 255).astype(np.uint8)


box = (0, 112, 104, 240)
S = 9
def cr(arr):
    im = Image.fromarray(arr).crop(box)
    return im.resize(((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.NEAREST)


ims = [cr(g(BG)), cr(g(REN))]
w, h = ims[0].size
comb = Image.new("RGB", (w * 2 + 6, h), (255, 255, 0))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "36_stuhl_bg_vs_render_hell.png")
print("box", box, comb.size, "links Hintergrund, rechts Render, beide gamma 0.45")
