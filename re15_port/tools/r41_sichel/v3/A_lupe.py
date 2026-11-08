# Leser A V3: Lupe auf einen Freisteller-Ausschnitt mit Pixelraster-Achsen (nur Ansicht)
# Aufruf: A_lupe.py NN x0 y0 x1 y1 faktor out.png   (Koordinaten im Freisteller)
import sys
from PIL import Image, ImageDraw
SRC = r'C:/workspace/git/reAi_v2/Version_80%/'
nn, x0, y0, x1, y1, f = sys.argv[1], *map(int, sys.argv[2:7])
out = sys.argv[7]
im = Image.open(SRC + 'birkin%s.png' % nn).convert('RGBA')
bg = Image.new('RGBA', im.size, (60, 120, 60, 255))
bg.alpha_composite(im)
c = bg.crop((x0, y0, x1, y1)).convert('RGB').resize(((x1 - x0) * f, (y1 - y0) * f), Image.NEAREST)
M = 30
can = Image.new('RGB', (c.width + M, c.height + M), (255, 255, 255))
can.paste(c, (M, M))
d = ImageDraw.Draw(can)
step = 10
for x in range((x0 // step + 1) * step, x1, step):
    X = M + (x - x0) * f
    d.line([(X, M - 6), (X, M)], fill=(0, 0, 0))
    if x % 20 == 0:
        d.text((X - 8, 2), str(x), fill=(0, 0, 0))
for y in range((y0 // step + 1) * step, y1, step):
    Y = M + (y - y0) * f
    d.line([(M - 6, Y), (M, Y)], fill=(0, 0, 0))
    if y % 20 == 0:
        d.text((0, Y - 5), str(y), fill=(0, 0, 0))
can.save(out)
