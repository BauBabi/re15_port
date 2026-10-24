#!/usr/bin/env python3
"""Runde 41 Spur J: Ausschnitt-Bogen - Aufruf: ausschnitt.py <out.png> <x0,y0,x1,y1> <skala> <spalten> <datei...>"""
import sys, os
from PIL import Image, ImageDraw
out = sys.argv[1]; x0, y0, x1, y1 = map(int, sys.argv[2].split(",")); sk = int(sys.argv[3]); cols = int(sys.argv[4])
files = sys.argv[5:]
tw, th = (x1 - x0) * sk, (y1 - y0) * sk
rows = (len(files) + cols - 1) // cols
sheet = Image.new("RGB", (cols * tw, rows * (th + 12)), (0, 0, 0))
d = ImageDraw.Draw(sheet)
for i, f in enumerate(files):
    im = Image.open(f).convert("RGB").crop((x0, y0, x1, y1)).resize((tw, th), Image.NEAREST)
    x, y = (i % cols) * tw, (i // cols) * (th + 12)
    sheet.paste(im, (x, y + 12)); d.text((x + 2, y), os.path.splitext(os.path.basename(f))[0], fill=(255, 255, 0))
sheet.save(out); print(out, sheet.size)
