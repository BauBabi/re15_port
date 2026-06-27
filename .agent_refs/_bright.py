import sys
from PIL import Image, ImageEnhance
src, dst = sys.argv[1], sys.argv[2]
crop = None
if len(sys.argv) > 3:
    crop = tuple(int(x) for x in sys.argv[3].split(','))
im = Image.open(src).convert("RGB")
if crop:
    im = im.crop(crop)
im = ImageEnhance.Brightness(im).enhance(2.2)
im = ImageEnhance.Contrast(im).enhance(1.15)
im.save(dst)
print("saved", dst, im.size)
