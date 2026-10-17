# sheet.py <out.png> <cols> <label-prefix> <ppm...> : Kontaktbogen mit Bildnummer
import sys, os
from PIL import Image, ImageDraw
out, cols, pre = sys.argv[1], int(sys.argv[2]), sys.argv[3]
files = sys.argv[4:]
ims = [Image.open(f).convert('RGB') for f in files]
w, h = ims[0].size
rows = (len(ims) + cols - 1) // cols
sheet = Image.new('RGB', (cols * w, rows * (h + 14)), (40, 40, 40))
d = ImageDraw.Draw(sheet)
for i, (f, im) in enumerate(zip(files, ims)):
    x, y = (i % cols) * w, (i // cols) * (h + 14)
    sheet.paste(im, (x, y + 14))
    lab = pre + ' F' + str(int(os.path.basename(f)[1:7]))
    d.text((x + 3, y + 1), lab, fill=(255, 255, 0))
sheet.save(out)
print(out, sheet.size)
