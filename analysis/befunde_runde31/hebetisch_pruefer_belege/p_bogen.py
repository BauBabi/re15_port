# -*- coding: utf-8 -*-
"""Pruefer r31/hebetisch: Kontaktbogen aus Framedump-PPMs. p_bogen.py <ordner> <aus.png> <skala> <spalten> F1 F2 ...
Beschriftet jedes Bild mit seiner Bildnummer (gelb, oben links)."""
import os, sys
from PIL import Image, ImageDraw
d, aus, sk, sp = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
fs = [int(x) for x in sys.argv[5:]]
ims = []
for f in fs:
    p = os.path.join(d, 'f_%06d.ppm' % f)
    im = Image.open(p).convert('RGB')
    im = im.resize((im.width * sk, im.height * sk), Image.NEAREST)
    ImageDraw.Draw(im).text((4, 2), 'F%d' % f, fill=(255, 255, 0))
    ims.append(im)
w, h = ims[0].size
zeilen = (len(ims) + sp - 1) // sp
b = Image.new('RGB', (w * sp, h * zeilen))
for i, im in enumerate(ims):
    b.paste(im, ((i % sp) * w, (i // sp) * h))
b.save(aus)
print(aus, b.size)
