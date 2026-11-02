import sys, os
from PIL import Image, ImageDraw
# Bildfolge Video | Port, gleiche Bildnummer h (Video: Hangar-Schnitt bei Datei-Bild 30; Port: erstes Cut-12-Bild = p0).
vdir, pdir, p0, out = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4]
hs = [int(x) for x in sys.argv[5].split(',')]
cols = int(sys.argv[6]) if len(sys.argv) > 6 else 3
tiles = []
for h in hs:
    v = Image.open(os.path.join(vdir, 'f_%05d.png' % (30 + h))).convert('RGB').resize((320, 240), Image.BILINEAR)
    vp = Image.new('RGB', (320, 240)); vp.paste(v, (-1, -3))
    pf = os.path.join(pdir, 'gd_%04d.png' % (p0 + h))
    pp = Image.open(pf).convert('RGB').resize((320, 240)) if os.path.exists(pf) else Image.new('RGB', (320, 240))
    t = Image.new('RGB', (644, 252), (20, 20, 20)); t.paste(vp, (0, 12)); t.paste(pp, (324, 12))
    d = ImageDraw.Draw(t)
    d.text((2, 0), 'Video h%d' % h, fill=(255, 255, 0)); d.text((326, 0), 'Port h%d (gd_%04d)' % (h, p0 + h), fill=(0, 255, 255))
    tiles.append(t)
w, hh = tiles[0].size; rows = (len(tiles) + cols - 1) // cols
S = Image.new('RGB', (w * cols, hh * rows), (0, 0, 0))
for k, t in enumerate(tiles): S.paste(t, ((k % cols) * w, (k // cols) * hh))
S.save(out, quality=90); print(len(tiles), S.size)
