#!/usr/bin/env python3
"""Runde 41 Spur J: Bildbogen aus Framedumps (PPM/PNG) - Aufruf: bogen.py <out.png> <spalten> <skala> <datei...>
Jede Kachel traegt den Dateinamen-Stamm als Beschriftung."""
import sys, os
from PIL import Image, ImageDraw
out, cols, sk = sys.argv[1], int(sys.argv[2]), float(sys.argv[3])
files = sys.argv[4:]
ims = [Image.open(f).convert("RGB") for f in files]
w, h = ims[0].size
tw, th = int(w * sk), int(h * sk)
rows = (len(ims) + cols - 1) // cols
sheet = Image.new("RGB", (cols * tw, rows * (th + 12)), (0, 0, 0))
d = ImageDraw.Draw(sheet)
for i, (f, im) in enumerate(zip(files, ims)):
    x, y = (i % cols) * tw, (i // cols) * (th + 12)
    sheet.paste(im.resize((tw, th)), (x, y + 12))
    d.text((x + 2, y), os.path.splitext(os.path.basename(f))[0], fill=(255, 255, 0))
sheet.save(out)
print(out, sheet.size)
