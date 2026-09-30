#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): Bildstreifen aus Framedumps eines Laufs (echte exe, 960x720 -> 320x240,
naechster Nachbar), Ausschnitt + Beschriftung "F<n>" je Bild. Aufruf:
streifen.py <laufdir> <out.png> <x0> <y0> <x1> <y1> <skala> F1 F2 ..."""
import sys, os
from PIL import Image, ImageDraw
d, out = sys.argv[1], sys.argv[2]
x0, y0, x1, y1, sk = [int(v) for v in sys.argv[3:8]]
fs = [int(v) for v in sys.argv[8:]]
w, h = (x1 - x0) * sk, (y1 - y0) * sk
bild = Image.new("RGB", (len(fs) * (w + 4), h + 14), (0, 0, 0))
dr = ImageDraw.Draw(bild)
for i, f in enumerate(fs):
    im = Image.open(os.path.join(d, "fd_%06d.ppm" % f)).convert("RGB").resize((320, 240), Image.NEAREST)
    bild.paste(im.crop((x0, y0, x1, y1)).resize((w, h), Image.NEAREST), (i * (w + 4), 14))
    dr.text((i * (w + 4) + 2, 1), "F%d" % f, fill=(255, 255, 0))
bild.save(out)
print(out, bild.size)
