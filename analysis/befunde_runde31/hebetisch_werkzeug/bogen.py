# -*- coding: utf-8 -*-
"""Kontaktbogen ganzer Framedumps: bogen.py <ordner> <aus.png> <faktor> <spalten> bild [bild ...]"""
import os, sys
from PIL import Image, ImageDraw
o, aus, fak, sp = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
ims = []
for b in sys.argv[5:]:
    im = Image.open(os.path.join(o, 'f_%06d.ppm' % int(b))).convert('RGB')
    im = im.resize((im.width * fak, im.height * fak), Image.NEAREST)
    ImageDraw.Draw(im).text((4, 4), 'F%s' % b, fill=(255, 255, 0))
    ims.append(im)
w, h = ims[0].size
zl = (len(ims) + sp - 1) // sp
bg = Image.new('RGB', (sp * w, zl * h))
for i, im in enumerate(ims):
    bg.paste(im, ((i % sp) * w, (i // sp) * h))
bg.save(aus); print(aus, bg.size)
