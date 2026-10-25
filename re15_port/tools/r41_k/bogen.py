#!/usr/bin/env python3
"""Runde 41 Spur K: Bildbogen aus RE15_FRAMEDUMP-PPMs (f<bild>.ppm) mit Bildnummer; optional Ausschnitt + Zoom.
    python bogen.py <ordner> <ziel.png> <bild,bild,...> [spalten=4] [zoom=1.0] [x0,y0,x1,y1]
"""
import sys, os
from PIL import Image, ImageDraw
ordner, ziel, liste = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(',')]
sp = int(sys.argv[4]) if len(sys.argv) > 4 else 4
zoom = float(sys.argv[5]) if len(sys.argv) > 5 else 1.0
box = tuple(int(v) for v in sys.argv[6].split(',')) if len(sys.argv) > 6 else None
bilder = []
for f in liste:
    p = os.path.join(ordner, 'f%06d.ppm' % f)
    if not os.path.exists(p): continue
    im = Image.open(p).convert('RGB')
    if box: im = im.crop(box)
    if zoom != 1.0: im = im.resize((int(im.width * zoom), int(im.height * zoom)), Image.NEAREST)
    d = ImageDraw.Draw(im); d.rectangle((0, 0, 44, 11), fill=(0, 0, 0)); d.text((2, 0), 'F%d' % f, fill=(255, 255, 0))
    bilder.append(im)
if not bilder: sys.exit('keine Bilder')
w, h = bilder[0].size; zl = (len(bilder) + sp - 1) // sp
bogen = Image.new('RGB', (w * sp, h * zl), (40, 40, 40))
for i, im in enumerate(bilder): bogen.paste(im, ((i % sp) * w, (i // sp) * h))
bogen.save(ziel); print('geschrieben', ziel, bogen.size, len(bilder), 'Bilder')
