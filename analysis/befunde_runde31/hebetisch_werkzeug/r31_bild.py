# -*- coding: utf-8 -*-
"""ID-Puffer-Bilder (r31_raster) fuer Kandidaten-Paare, Ausschnitt um das Fach, 6-fach.
r31_bild.py <aus.png> "Sx,Sy,Sz,Sry;Gx,Gy,Gz,Gry" [...]"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_raster as RS  # noqa
FARBE = {0: (20, 20, 30), 1: (90, 90, 100), 2: (150, 120, 170), 3: (230, 230, 230), 4: (60, 160, 60)}
sz = RS.Szene()
aus = sys.argv[1]
zeilen = []
for arg in sys.argv[2:]:
    s, g = arg.split(';')
    s = tuple(int(v) for v in s.split(',')); g = tuple(int(v) for v in g.split(','))
    kacheln = []
    for py, box in ((-305, (170, 135, 260, 185)), (-1205, (175, 0, 265, 50))):
        idb = sz.render(py, 150, s, g)
        rgb = np.zeros(idb.shape + (3,), np.uint8)
        for k, c in FARBE.items():
            rgb[idb == k] = c
        im = Image.fromarray(rgb).crop(box)
        im = im.resize((im.width * 6, im.height * 6), Image.NEAREST)
        ImageDraw.Draw(im).text((4, 4), 'y=%d %s' % (py, arg), fill=(255, 255, 0))
        kacheln.append(im)
    zeilen.append(kacheln)
w, h = zeilen[0][0].size
bg = Image.new('RGB', (2 * w, len(zeilen) * h))
for i, z in enumerate(zeilen):
    for j, im in enumerate(z):
        bg.paste(im, (j * w, i * h))
bg.save(aus); print(aus, bg.size)
