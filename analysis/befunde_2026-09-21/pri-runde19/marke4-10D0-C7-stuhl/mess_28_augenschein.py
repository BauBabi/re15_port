# -*- coding: utf-8 -*-
"""Messung 28: Augenschein. Der Render im Original-Kontrast, 20-fach, und daneben
derselbe Ausschnitt des Hintergrunds. Kein Gamma, kein Filter — genau das, was der
Nutzer sieht (nur vergroessert)."""
import numpy as np
from PIL import Image

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
BG = Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB")
REN = Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST)

box = (46, 122, 82, 166)
S = 20
ims = [BG.crop(box).resize(((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.NEAREST),
       REN.crop(box).resize(((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.NEAREST)]
w, h = ims[0].size
comb = Image.new("RGB", (w * 2 + 8, h), (255, 255, 0))
comb.paste(ims[0], (0, 0))
comb.paste(ims[1], (w + 8, 0))
comb.save(OUT + "42_augenschein.png")
print("42_augenschein.png box", box, comb.size, "links Hintergrund, rechts Render, ohne Gamma")
