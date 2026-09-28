# -*- coding: utf-8 -*-
"""Lupe: lupe.py <ordner> <aus.png> x0 y0 x1 y1 faktor bild [bild ...]"""
import os, sys
from PIL import Image, ImageDraw
o, aus = sys.argv[1], sys.argv[2]
x0, y0, x1, y1, f = (int(v) for v in sys.argv[3:8])
bilder = [int(v) for v in sys.argv[8:]]
ims = []
for b in bilder:
    p = os.path.join(o, 'f_%06d.ppm' % b)
    im = Image.open(p).convert('RGB').crop((x0, y0, x1, y1))
    im = im.resize((im.width * f, im.height * f), Image.NEAREST)
    ImageDraw.Draw(im).text((3, 3), 'F%d' % b, fill=(255, 255, 0))
    ims.append(im)
sp = min(6, len(ims)); zl = (len(ims) + sp - 1) // sp
w, h = ims[0].size
bg = Image.new('RGB', (sp * w, zl * h))
for i, im in enumerate(ims):
    bg.paste(im, ((i % sp) * w, (i // sp) * h))
bg.save(aus); print(aus, bg.size)
