import sys
from PIL import Image
# zoom.py <in.bmp> <out.png> <scale> [x0 y0 x1 y1]
im = Image.open(sys.argv[1]).convert('RGB')
s = int(sys.argv[3])
if len(sys.argv) > 4:
    x0, y0, x1, y1 = map(int, sys.argv[4:8])
    im = im.crop((x0, y0, x1 + 1, y1 + 1))
im = im.resize((im.width * s, im.height * s), Image.NEAREST)
im.save(sys.argv[2])
print(sys.argv[2], im.size)
