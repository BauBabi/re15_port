# -*- coding: utf-8 -*-
"""Kontaktbogen aus Framedumps: bogen.py <ordner> <ausgabe.png> <bild> [<bild> ...] [--x2]"""
import os, sys
from PIL import Image, ImageDraw
a = [x for x in sys.argv[1:] if not x.startswith('--')]
zoom = 2 if '--x2' in sys.argv else 1
ordner, aus, bilder = a[0], a[1], [int(x) for x in a[2:]]
ims = []
for b in bilder:
    p = os.path.join(ordner, 'f_%06d.ppm' % b)
    im = Image.open(p).convert('RGB') if os.path.exists(p) else Image.new('RGB', (320, 240), (255, 0, 255))
    if zoom != 1: im = im.resize((im.width * zoom, im.height * zoom), Image.NEAREST)
    ImageDraw.Draw(im).text((4, 4), 'F%d' % b, fill=(255, 255, 0))
    ims.append(im)
sp = min(4, len(ims)); zl = (len(ims) + sp - 1) // sp
w, h = ims[0].size
bg = Image.new('RGB', (sp * w, zl * h), (0, 0, 0))
for i, im in enumerate(ims):
    bg.paste(im, ((i % sp) * w, (i // sp) * h))
bg.save(aus)
print(aus, bg.size)
