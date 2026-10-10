import sys, os
from PIL import Image, ImageDraw
# vergleich.py out.png dir_alt dir_neu x0 y0 x1 y1 scale frames...
out, da, dn = sys.argv[1], sys.argv[2], sys.argv[3]
x0, y0, x1, y1, sc = map(int, sys.argv[4:9]); frames = [int(f) for f in sys.argv[9:]]
tiles = []
for f in frames:
    row = []
    for lab, d in (("alt", da), ("neu", dn)):
        p = os.path.join(d, "f%06d.ppm" % f)
        im = Image.open(p).convert("RGB").crop((x0, y0, x1, y1))
        im = im.resize((im.width * sc, im.height * sc), Image.NEAREST)
        dr = ImageDraw.Draw(im); dr.rectangle([0, 0, 64, 12], fill=(0, 0, 0)); dr.text((2, 1), "F%d %s" % (f, lab), fill=(255, 255, 0))
        row.append(im)
    tiles.append(row)
w, h = tiles[0][0].size
S = Image.new("RGB", (w * len(frames), h * 2))
for i, row in enumerate(tiles):
    S.paste(row[0], (i * w, 0)); S.paste(row[1], (i * w, h))
S.save(out); print(out, S.size)
