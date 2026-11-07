"""hell.py <ein.jpg> <aus.jpg> [faktor=2.5] [x0,y0,x1,y1]: Belegkopie einer Bildleiste aufgehellt (PIL ImageEnhance.Brightness), optional Ausschnitt."""
import sys
from PIL import Image, ImageEnhance
im = Image.open(sys.argv[1]).convert("RGB")
f = float(sys.argv[3]) if len(sys.argv) > 3 else 2.5
if len(sys.argv) > 4:
    im = im.crop(tuple(int(v) for v in sys.argv[4].split(",")))
ImageEnhance.Brightness(im).enhance(f).save(sys.argv[2], quality=88)
print("geschrieben", sys.argv[2], im.size)
