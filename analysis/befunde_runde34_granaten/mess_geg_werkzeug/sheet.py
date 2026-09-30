# Kontaktbogen aus Framedump-PPMs: sheet.py <dir> <out.png> <frame,...> [scale]
import sys, os
from PIL import Image, ImageDraw
d, out, frames = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(',')]
sc = float(sys.argv[4]) if len(sys.argv) > 4 else 0.5
ims = []
for f in frames:
    p = os.path.join(d, 'f_%06d.ppm' % f)
    if not os.path.exists(p): continue
    im = Image.open(p).convert('RGB')
    im = im.resize((int(im.width*sc), int(im.height*sc)))
    ImageDraw.Draw(im).text((4,4), 'F%d' % f, fill=(255,255,0))
    ims.append(im)
cols = min(4, len(ims)); rows = (len(ims)+cols-1)//cols
W, H = ims[0].size
sheet = Image.new('RGB', (cols*W, rows*H))
for i, im in enumerate(ims): sheet.paste(im, ((i%cols)*W, (i//cols)*H))
sheet.save(out); print(out, sheet.size)
