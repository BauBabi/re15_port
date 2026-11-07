#!/usr/bin/env python3
"""Tafeln aus bild/ton: je Clip und Art 4x2 (el 10 oben, el 40 unten) mit Beschriftung."""
import os, sys
from PIL import Image, ImageDraw
H = os.path.dirname(os.path.abspath(__file__))
T = os.path.join(H, 'bild', 'ton')
NAME = {0: 'vorn', 45: 'vorn-links', 90: 'links', 135: 'hinten-links', 180: 'hinten', 225: 'hinten-rechts', 270: 'rechts', 315: 'vorn-rechts'}
CLIP = {0: 'Pose1 (Bild1)', 1: 'Pose2 (Bild2)', 2: 'Ruhe'}
for art in ('ton', 'far'):
    for c in (0, 1, 2):
        W, Hh = 480, 360
        bl = Image.new('RGB', (W * 4, Hh * 4), (20, 20, 20))
        d = ImageDraw.Draw(bl)
        for i, az in enumerate(range(0, 360, 45)):
            for j, el in enumerate((10, 40)):
                im = Image.open(os.path.join(T, '%s_c%d_az%03d_el%02d.png' % (art, c, az, el))).convert('RGB').resize((W, Hh))
                x = (i % 4) * W; y = ((i // 4) + 2 * j) * Hh
                bl.paste(im, (x, y))
                d.text((x + 6, y + 4), '%s %s az%d el%d' % (CLIP[c], NAME[az], az, el), fill=(255, 255, 0))
        bl.save(os.path.join(H, 'bild', '%s_clip%d.png' % (art, c)))
print('ok')
