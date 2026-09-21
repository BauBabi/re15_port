# -*- coding: utf-8 -*-
"""Messung 3: Was steht rechts der Deckung? Hintergrund + Nutzer-Alpha + Deckung."""
import numpy as np
from PIL import Image

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), np.uint8)
COV = ~np.asarray(Image.open("re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM"))
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), np.uint8)

png = np.zeros((240, 320), bool)
a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"))
png[123:123 + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

# Panel: bg aufgehellt (gamma) damit man die dunklen Rohre sieht
hell = (np.clip((BG / 255.0) ** 0.45, 0, 1) * 255).astype(np.uint8)
p_png = hell.copy(); p_png[png] = [255, 0, 0]
p_cov = hell.copy(); p_cov[COV] = [0, 255, 0]

box = (48, 108, 104, 196)
S = 16
def cr(arr):
    im = Image.fromarray(arr).crop(box)
    return im.resize(((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.NEAREST)
renh = (np.clip((REN / 255.0) ** 0.45, 0, 1) * 255).astype(np.uint8)
ims = [cr(hell), cr(p_png), cr(p_cov), cr(renh)]
w, h = ims[0].size
comb = Image.new("RGB", (w * 4 + 18, h), (255, 255, 0))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "34_hell_png_deckung_render.png")
print("box", box, "size", comb.size)
print("Panel 1 Hintergrund (gamma 0.45) | 2 rot=Nutzer-PNG 07_01 | 3 gruen=PBM-Deckung | 4 Render")
