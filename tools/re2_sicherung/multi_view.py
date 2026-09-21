#!/usr/bin/env python3
"""multi_view.py - ein MD1 aus mehreren Blickwinkeln rendern (Kontaktbogen).

Aufruf: multi_view.py <a.md1> <a.tim> <out.png> [kachelgroesse]
"""
import sys, os
from PIL import Image, ImageDraw
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import md1_view

md1 = open(sys.argv[1], "rb").read()
tim = sys.argv[2]
out = sys.argv[3]
S = int(sys.argv[4]) if len(sys.argv) > 4 else 300
VIEWS = [(0.0, 0.0), (0.8, 0.0), (1.6, 0.0), (2.4, 0.0),
         (0.0, 0.7), (0.8, 0.7), (1.6, 0.7), (0.0, -0.7)]
tmp = out + ".tmp.png"
cols = 4
img = Image.new("RGB", (S * cols, (S + 16) * ((len(VIEWS) + cols - 1) // cols)), (10, 10, 14))
dr = ImageDraw.Draw(img)
for i, (yaw, pitch) in enumerate(VIEWS):
    md1_view.render(md1, tim, tmp, S, yaw, pitch)
    r, c = divmod(i, cols)
    img.paste(Image.open(tmp), (c * S, r * (S + 16) + 16))
    dr.text((c * S + 4, r * (S + 16) + 2), "yaw %.1f pitch %.1f" % (yaw, pitch), fill=(255, 255, 255))
os.remove(tmp)
img.save(out)
print("geschrieben:", out, img.size)
