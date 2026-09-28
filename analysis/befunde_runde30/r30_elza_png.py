"""PPM -> PNG (und PPM loeschen) fuer einen Messordner; optional Verkleinerung.
Aufruf: python r30_elza_png.py <ordner> [--halb]
Gibt je Bild den Mittelwert (R,G,B) und den Anteil nicht-schwarzer Pixel aus - damit ist
"ist hier ein Bild zu sehen?" eine Zahl und keine Ansichtssache.
"""
import os, sys, glob
from PIL import Image

def main():
    d = sys.argv[1]
    halb = "--halb" in sys.argv
    for p in sorted(glob.glob(os.path.join(d, "*.ppm"))):
        im = Image.open(p).convert("RGB")
        px = im.resize((160, 120), Image.BILINEAR).getdata()
        n = len(px)
        r = sum(v[0] for v in px) / n; g = sum(v[1] for v in px) / n; b = sum(v[2] for v in px) / n
        hell = sum(1 for v in px if max(v) > 16) / n
        if halb:
            im = im.resize((im.width // 2, im.height // 2), Image.BILINEAR)
        q = p[:-4] + ".png"
        im.save(q)
        os.remove(p)
        print("%s  mittel=(%.1f,%.1f,%.1f)  nichtschwarz=%.3f" % (os.path.basename(q), r, g, b, hell))

main()
