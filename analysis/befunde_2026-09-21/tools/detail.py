#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""High-res 3-view render of individual MD1 files (front / side / top)."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1lib
from PIL import Image, ImageDraw, ImageFont

WORK = sys.argv[1]
OUT = sys.argv[2]
names = sys.argv[3:]
S = 260
LBL = 26
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 14)
except Exception:
    F = ImageFont.load_default()
sheet = Image.new('RGB', (S * 3 + 8, (S + LBL) * len(names)), (8, 8, 10))
dr = ImageDraw.Draw(sheet)
for i, n in enumerate(names):
    p = n if os.path.isabs(n) else os.path.join(WORK, 'md1', n + '.md1')
    md1 = md1lib.parse(open(p, 'rb').read())
    polys = md1lib.faces_world(md1)
    bb = md1lib.bbox(polys)
    y = i * (S + LBL)
    for j, v in enumerate(('front', 'side', 'top')):
        sheet.paste(md1lib.render_z(polys, S, S, v), (j * (S + 4), y + LBL))
    dr.text((3, y + 4), '%s   dx=%d dy=%d dz=%d  faces=%d  meshes=%d   [front | side | top]'
            % (os.path.basename(p), bb[1] - bb[0], bb[3] - bb[2], bb[5] - bb[4],
               len(polys), md1['nmesh']), fill=(235, 235, 140), font=F)
sheet.save(OUT)
print('wrote', OUT)
