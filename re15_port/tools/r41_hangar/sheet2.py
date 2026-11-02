# Runde 41 Spur H: Bildbogen. Aufruf: sheet2.py "<muster>" <ziel.jpg> <spalten> <schritt> <kachelbreite>
import sys, glob, os
from PIL import Image, ImageDraw
pat, out, cols, step, w = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
fs = sorted(glob.glob(pat))[::step]
im = []
for f in fs:
    m = Image.open(f).convert('RGB'); h = int(m.size[1] * w / m.size[0]); m = m.resize((w, h))
    ImageDraw.Draw(m).text((4, 4), os.path.basename(f), fill=(255, 255, 0)); im.append(m)
h = im[0].size[1]; rows = (len(im) + cols - 1) // cols
S = Image.new('RGB', (w * cols, h * rows))
for i, m in enumerate(im): S.paste(m, ((i % cols) * w, (i // cols) * h))
S.save(out, quality=85); print(len(im), S.size)
