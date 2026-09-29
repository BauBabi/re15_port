#!/usr/bin/env python3
"""kontaktbogen.py <mess-ordner> <aus.jpg> [spalten] [breite] [muster]
Legt die Framedumps (fNNNNNN.ppm) eines Laufs als beschrifteten Kontaktbogen ab."""
import sys, os, glob
from PIL import Image, ImageDraw
d, aus = sys.argv[1], sys.argv[2]
sp = int(sys.argv[3]) if len(sys.argv) > 3 else 6
bw = int(sys.argv[4]) if len(sys.argv) > 4 else 320
mu = sys.argv[5] if len(sys.argv) > 5 else "f*.ppm"
fs = sorted(glob.glob(os.path.join(d, mu)))
if not fs: sys.exit("keine Bilder")
im0 = Image.open(fs[0]); bh = int(im0.height * bw / im0.width)
zeilen = (len(fs) + sp - 1) // sp
bogen = Image.new("RGB", (sp * bw, zeilen * (bh + 14)), (40, 40, 40))
dr = ImageDraw.Draw(bogen)
for i, f in enumerate(fs):
    im = Image.open(f).convert("RGB").resize((bw, bh))
    x, y = (i % sp) * bw, (i // sp) * (bh + 14)
    bogen.paste(im, (x, y + 14))
    dr.text((x + 3, y + 1), os.path.basename(f), fill=(255, 255, 0))
bogen.save(aus, quality=85)
print(aus, len(fs), "Bilder")
