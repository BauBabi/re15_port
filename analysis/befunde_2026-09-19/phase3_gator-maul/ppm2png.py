"""PPM-Framedumps (RE15_FRAMEDUMP) nach PNG wandeln - fuer die Sichtpruefung mit dem Read-Werkzeug.
Aufruf: python ppm2png.py <verzeichnis> [praefix]   (loescht die PPMs nach der Wandlung)"""
import os, sys
from PIL import Image

d = sys.argv[1]
pre = sys.argv[2] if len(sys.argv) > 2 else "fd_"
n = 0
for f in sorted(os.listdir(d)):
    if f.startswith(pre) and f.endswith(".ppm"):
        p = os.path.join(d, f)
        im = Image.open(p)
        out = p[:-4] + ".png"
        im.save(out)
        os.remove(p)
        n += 1
        print(out, im.size)
print("gewandelt:", n)
