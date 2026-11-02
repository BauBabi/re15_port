import sys, os
from PIL import Image, ImageDraw
# Video 640x480 -> PSX 320x240: psx = video/2 + (-1,-3) (kalib.py). Ausgabe mit Lineal (PSX-Koordinaten) am Rand, Zoom 2.
src, out, cols = sys.argv[1], sys.argv[2], int(sys.argv[3])
idx = [int(a) for a in sys.argv[4].split(',')]
Z = 2; M = 18
tiles = []
for i in idx:
    f = os.path.join(src, 'f_%05d.png' % i)
    v = Image.open(f).convert('RGB').resize((320, 240), Image.BILINEAR)
    p = Image.new('RGB', (320, 240)); p.paste(v, (-1, -3))
    p = p.resize((320 * Z, 240 * Z), Image.BILINEAR)
    t = Image.new('RGB', (320 * Z + M, 240 * Z + M), (40, 40, 40)); t.paste(p, (M, M))
    d = ImageDraw.Draw(t)
    for x in range(0, 321, 10):
        L = 8 if x % 50 == 0 else 4
        d.line([(M + x * Z, M - L), (M + x * Z, M)], fill=(255, 255, 0))
        d.line([(M + x * Z, M + 240 * Z - L), (M + x * Z, M + 240 * Z)], fill=(255, 255, 0))
        if x % 50 == 0: d.text((M + x * Z + 1, 0), str(x), fill=(255, 255, 0))
    for y in range(0, 241, 10):
        L = 8 if y % 50 == 0 else 4
        d.line([(M - L, M + y * Z), (M, M + y * Z)], fill=(255, 255, 0))
        d.line([(M + 320 * Z - L, M + y * Z), (M + 320 * Z, M + y * Z)], fill=(255, 255, 0))
        if y % 50 == 0: d.text((0, M + y * Z + 1), str(y), fill=(255, 255, 0))
    d.text((M + 4, M + 4), 'h%d' % (i - 30), fill=(0, 255, 255))
    tiles.append(t)
w, h = tiles[0].size; rows = (len(tiles) + cols - 1) // cols
S = Image.new('RGB', (w * cols, h * rows))
for k, t in enumerate(tiles): S.paste(t, ((k % cols) * w, (k // cols) * h))
S.save(out, quality=92); print(len(tiles), S.size)
