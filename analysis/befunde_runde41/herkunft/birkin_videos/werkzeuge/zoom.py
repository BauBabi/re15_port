# zoom.py dir outfile x y w h scale frame1 frame2 ...  (nearest neighbour, label)
import sys
from PIL import Image, ImageDraw
d, out, x, y, w, h, s = sys.argv[1], sys.argv[2], *map(int, sys.argv[3:8])
fr = [int(v) for v in sys.argv[8:]]
tiles = []
for n in fr:
    im = Image.open('%s/%05d.jpg' % (d, n)).convert('RGB').crop((x, y, x+w, y+h)).resize((w*s, h*s), Image.LANCZOS)
    ImageDraw.Draw(im).text((3, 3), '%d %.2fs' % (n, n/30), fill=(255, 255, 0))
    tiles.append(im)
cols = min(len(tiles), int(sys.argv[-0]) if False else 4)
rows = (len(tiles)+cols-1)//cols
S = Image.new('RGB', (cols*w*s, rows*h*s))
for i, t in enumerate(tiles):
    S.paste(t, ((i % cols)*w*s, (i//cols)*h*s))
S.save(out, quality=92)
print(out, S.size)
