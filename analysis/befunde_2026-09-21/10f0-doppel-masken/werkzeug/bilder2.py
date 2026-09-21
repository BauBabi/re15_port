# -*- coding: utf-8 -*-
"""Zweiter Bildbeleg: die WIRKLICH undurchsichtigen Punkte (Atlas-Deckung),
nicht die Kachelrahmen."""
import os
import sys
import numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bilder import lade, MARKE, OUT

masks, idx, clut, bl, regs = lade("ROOM10F0", 4)
im = Image.open(MARKE).convert("RGB")
a = np.asarray(im, np.uint8).copy()

# Deckung von Objekt 0 (Lasso) und Objekt 7 (Quader-Doppel), 3-fach vergroessert
g0 = np.kron(regs[0], np.ones((3, 3), bool))
g7 = np.kron(regs[7], np.ones((3, 3), bool))
nur0 = g0 & ~g7
nur7 = g7 & ~g0
beide = g0 & g7
a[nur0] = (a[nur0] * 0.3 + np.array([90, 200, 255]) * 0.7).astype(np.uint8)
a[nur7] = (a[nur7] * 0.3 + np.array([255, 60, 60]) * 0.7).astype(np.uint8)
a[beide] = (a[beide] * 0.35 + np.array([255, 200, 60]) * 0.65).astype(np.uint8)
im = Image.fromarray(a)
d = ImageDraw.Draw(im)
d.rectangle([99 * 3, 81 * 3, 121 * 3, 118 * 3], outline=(255, 255, 255), width=2)
im = im.crop((40 * 3, 45 * 3, 165 * 3, 145 * 3)).resize((125 * 6, 100 * 6), Image.NEAREST)
d = ImageDraw.Draw(im)
d.rectangle([0, 0, im.width - 1, 18], fill=(0, 0, 0))
d.text((4, 4), "Undurchsichtige Punkte: gelb = BEIDE Objekte (%d px, 100 %%), "
               "blau = nur Lasso (%d), rot = nur Quader (%d); weiss = Leons Kasten"
       % (int((regs[0] & regs[7]).sum()), int((regs[0] & ~regs[7]).sum()),
          int((regs[7] & ~regs[0]).sum())), fill=(255, 255, 255))
p = os.path.join(OUT, "c4_deckung_f335.png")
im.save(p)
print("->", p, im.size)
