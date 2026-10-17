import sys, os
from PIL import Image, ImageDraw
d = sys.argv[1]; out = sys.argv[2]; frames = [int(x) for x in sys.argv[3].split(',')]
cols = int(sys.argv[4]) if len(sys.argv) > 4 else 4
ims = []
for f in frames:
    p = os.path.join(d, 'fd_%06d.ppm' % f)
    if os.path.exists(p):
        im = Image.open(p).convert('RGB')
    else:
        im = Image.new('RGB', (320, 240), (255, 0, 255))
    dr = ImageDraw.Draw(im); dr.text((4, 4), 'F%d' % f, fill=(255, 255, 0))
    ims.append(im)
w, h = ims[0].size
rows = (len(ims) + cols - 1) // cols
sheet = Image.new('RGB', (cols * w, rows * h))
for i, im in enumerate(ims):
    sheet.paste(im.resize((w, h)), ((i % cols) * w, (i // cols) * h))
sheet.save(out)
print(out, sheet.size)
