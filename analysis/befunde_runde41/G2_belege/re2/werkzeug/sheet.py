# Abnahme 4: Bildleiste aus Einzelbildern. sheet.py out.jpg cols scale crop(x0,y0,x1,y1|voll) hell(1.0) label_fmt bild1 bild2 ...
import sys
from PIL import Image, ImageDraw, ImageEnhance
out, cols, scale, crop, hell = sys.argv[1], int(sys.argv[2]), float(sys.argv[3]), sys.argv[4], float(sys.argv[5])
items = sys.argv[6:]
ims = []
for it in items:
    path, _, lab = it.partition("=")
    im = Image.open(path).convert("RGB")
    if crop != "voll":
        im = im.crop(tuple(int(v) for v in crop.split(",")))
    if scale != 1.0:
        im = im.resize((int(im.width * scale), int(im.height * scale)), Image.LANCZOS)
    if hell != 1.0:
        im = ImageEnhance.Brightness(im).enhance(hell)
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 8 * len(lab) + 6, 14), fill=(0, 0, 0))
    d.text((3, 1), lab, fill=(255, 255, 0))
    ims.append(im)
w, h = ims[0].size
rows = (len(ims) + cols - 1) // cols
S = Image.new("RGB", (cols * w + (cols - 1) * 2, rows * h + (rows - 1) * 2), (60, 60, 60))
for i, im in enumerate(ims):
    S.paste(im, ((i % cols) * (w + 2), (i // cols) * (h + 2)))
S.save(out, quality=90)
print(out, S.size)
