"""Runde 38 Spur E: helle Pixel (max(R,G,B) > 16) im Lageplan-Feld x 91..98 / y 107..112 je Bild eines Messordners.

Aufruf:  python e_feld.py <ordner> [muster]      (muster Standard: k*_f*.png)
"""
import os, sys, glob
from PIL import Image

def main():
    d = sys.argv[1]
    muster = sys.argv[2] if len(sys.argv) > 2 else "k*_f*.png"
    for p in sorted(glob.glob(os.path.join(d, muster))):
        im = Image.open(p).convert("RGB")
        feld = sum(1 for y in range(107, 113) for x in range(91, 99) if max(im.getpixel((x, y))) > 16)
        print("%s  feld_hell=%d" % (os.path.basename(p), feld))

main()
