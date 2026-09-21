#!/usr/bin/env python3
"""clut_probe.py - alle 16 CLUT-Baenke einer ITEMALL-Kachel nebeneinander zeigen.

Der CLUT-Block der ITEMALL-TIMs ist 256 Farben breit bei VRAM-y=496, also
16 Baenke a 16 Farben. Welche Bank der Zeichner waehlt, steht im Code; hier
werden erst alle 16 sichtbar gemacht, damit die Wahl belegt und nicht geraten ist.
"""
import sys, os
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import itemall_extract as IX

idx = int(sys.argv[1], 0)
out = sys.argv[2]
scale = 3
cols = 4
cw, ch = IX.W * scale + 8, IX.H * scale + 16
img = Image.new("RGB", (cw * cols, ch * (16 // cols)), (24, 24, 30))
dr = ImageDraw.Draw(img)
for b in range(16):
    r, c = divmod(b, cols)
    pal = IX.clut(bank=b)
    t = IX.tile_image(idx, pal).resize((IX.W * scale, IX.H * scale), Image.NEAREST)
    bg = Image.new("RGB", t.size, (90, 20, 90))
    bg.paste(t, (0, 0), t)
    img.paste(bg, (c * cw + 4, r * ch + 12))
    dr.text((c * cw + 4, r * ch + 1), "CLUT-Bank %d" % b, fill=(240, 240, 240))
img.save(out)
print("geschrieben:", out, img.size)
