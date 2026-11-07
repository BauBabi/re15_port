import sys
from PIL import Image, ImageDraw
d, a, b, st, out = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
ims = []
for n in range(a, b + 1, st):
    im = Image.open('%s/bild_%03d.png' % (d, n)).convert('RGB').resize((320, 240))
    ImageDraw.Draw(im).text((4, 4), str(n), fill=(255, 255, 0))
    ims.append(im)
sp = 6
z = (len(ims) + sp - 1) // sp
o = Image.new('RGB', (320 * sp, 240 * z))
for i, im in enumerate(ims):
    o.paste(im, ((i % sp) * 320, (i // sp) * 240))
o.save(out, quality=80)
