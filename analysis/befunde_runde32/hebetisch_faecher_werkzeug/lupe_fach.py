# -*- coding: utf-8 -*-
"""Ausschnitt der beiden Faecher aus Framedumps (320x240), vergroessert, nebeneinander.
lupe_fach.py <ordner> <aus.png|.jpg> <faktor> <x0> <y0> <x1> <y1> bild [bild ...]"""
import os, sys
from PIL import Image, ImageDraw
o, aus, fak = sys.argv[1], sys.argv[2], int(sys.argv[3])
x0, y0, x1, y1 = (int(v) for v in sys.argv[4:8])
ims = []
for b in sys.argv[8:]:
    im = Image.open(os.path.join(o, 'f_%06d.ppm' % int(b))).convert('RGB').crop((x0, y0, x1, y1))
    im = im.resize((im.width * fak, im.height * fak), Image.NEAREST)
    ImageDraw.Draw(im).text((4, 4), 'F%s' % b, fill=(255, 255, 0))
    ims.append(im)
w, h = ims[0].size
bg = Image.new('RGB', (w * len(ims), h))
for i, im in enumerate(ims):
    bg.paste(im, (i * w, 0))
bg.save(aus, quality=90) if aus.endswith('.jpg') else bg.save(aus)
print(aus, bg.size)
