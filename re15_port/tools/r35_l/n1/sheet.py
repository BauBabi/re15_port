# Kontaktbogen aus PPM-Dumps: sheet.py <dir> <out.png> <frame,frame,...> [cols]
import sys, os
from PIL import Image, ImageDraw
d, out, fr = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(',')]
cols = int(sys.argv[4]) if len(sys.argv) > 4 else 4
ims = []
for f in fr:
    p = os.path.join(d, 'f_%06d.ppm' % f)
    if os.path.exists(p):
        im = Image.open(p).convert('RGB'); im = im.resize((im.width // 2, im.height // 2)) if im.width > 400 else im; ims.append((f, im))
w, h = ims[0][1].size
rows = (len(ims) + cols - 1) // cols
sheet = Image.new('RGB', (cols * w, rows * (h + 14)), (0, 0, 0))
dr = ImageDraw.Draw(sheet)
for i, (f, im) in enumerate(ims):
    x, y = (i % cols) * w, (i // cols) * (h + 14)
    sheet.paste(im, (x, y + 14)); dr.text((x + 3, y + 1), 'F%d' % f, fill=(255, 255, 0))
sheet.save(out); print(out, len(ims), w, h)
