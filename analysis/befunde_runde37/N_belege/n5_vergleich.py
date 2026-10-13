#!/usr/bin/env python3
"""Runde 37 Spur N — Vergleichsbild ORIGINAL (VRAM-Anzeigepuffer eines DuckStation-Endstands) gegen Port VORHER / NACHHER
(RE15_FRAMEDUMP, WINDOW_SCALE=1). Je Spalte ein Bild, optional ein Ausschnitt (x0,y0,x1,y1) vergroessert.

Aufruf: n5_vergleich.py <aus.png> <titel>=<bilddatei>[@x0,y0,x1,y1] ...
"""
import sys
from PIL import Image, ImageDraw

aus = sys.argv[1]
teile = []
for arg in sys.argv[2:]:
    titel, rest = arg.split('=', 1)
    box = None
    if '@' in rest:
        rest, b = rest.rsplit('@', 1)
        box = tuple(int(v) for v in b.split(','))
    im = Image.open(rest).convert('RGB')
    if im.size[0] > 320:
        im = im.crop((0, 0, 320, 240))          # Anzeigepuffer 0 des VRAM-Doppelbilds
    if box:
        im = im.crop(box)
    im = im.resize((int(im.size[0] * 300 / im.size[1]), 300), Image.NEAREST)
    teile.append((titel, im))
W = sum(t[1].size[0] for t in teile) + 10 * (len(teile) - 1)
blatt = Image.new('RGB', (W, 318), (20, 20, 20))
d = ImageDraw.Draw(blatt)
x = 0
for titel, im in teile:
    blatt.paste(im, (x, 18))
    d.text((x + 3, 3), titel, fill=(255, 255, 0))
    x += im.size[0] + 10
blatt.save(aus)
print('ok', aus, blatt.size)
