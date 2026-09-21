# -*- coding: utf-8 -*-
"""Messung 2: WAS ist verloren gegangen?

verloren = (Render weicht vom Hintergrund ab) AND (keine Deckung in der .PBM)
Ausgabe: der Hintergrund NUR an den verlorenen Punkten (Rest auf 15 % gedimmt)
-> man sieht unmittelbar, welcher Bildinhalt uebermalt wurde.
"""
import numpy as np
from PIL import Image

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
COV = ~np.asarray(Image.open("re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM"))

d = np.abs(REN - BG).sum(2) > 30
verloren = d & ~COV
print("uebermalt(>30) %d | mit Deckung uebermalt %d (Renderer-Leck) | VERLOREN %d"
      % (d.sum(), (d & COV).sum(), verloren.sum()))

# nur der verlorene Hintergrund, Rest gedimmt
nur = (BG * 0.15).astype(np.uint8)
nur[verloren] = BG[verloren]
box = (40, 105, 105, 200)
S = 13
def cr(a):
    im = Image.fromarray(a.astype(np.uint8)).crop(box)
    return im.resize(((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.NEAREST)
ims = [cr(BG), cr(nur)]
w, h = ims[0].size
comb = Image.new("RGB", (w * 2 + 6, h), (255, 255, 0))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "33_was_ist_verloren.png")
print("box", box, comb.size)

# Zeilenweise: wie weit reicht der verlorene Bereich nach rechts
print("y : verlorene x-Spanne (Anzahl)")
for y in range(105, 200, 2):
    xs = np.nonzero(verloren[y])[0]
    if len(xs):
        print("%3d : %3d..%3d  (%d)" % (y, xs.min(), xs.max(), len(xs)))
