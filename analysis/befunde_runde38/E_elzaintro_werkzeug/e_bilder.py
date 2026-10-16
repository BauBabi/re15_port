"""Runde 38 Spur E: PPM -> PNG fuer einen Messordner + Pixelzaehlung je Bild.

Aufruf:  python e_bilder.py <ordner> [--behalten]
Je Bild: Mittelwert (R,G,B), Anteil nicht-schwarzer Pixel (max(R,G,B) > 16) im GANZEN Bild und
Anteil nicht-schwarzer Pixel, die NICHT weiss-grau-neutral sind (|R-G|,|G-B|,|R-B| > 12) = farbige Pixel.
Die Erzaehlertexte sind neutral weiss/grau; ein durchscheinender Raumhintergrund ist farbig.
"""
import os, sys, glob
from PIL import Image

def main():
    d = sys.argv[1]
    behalten = "--behalten" in sys.argv
    for p in sorted(glob.glob(os.path.join(d, "*.ppm"))):
        im = Image.open(p).convert("RGB")
        px = list(im.getdata())
        n = len(px)
        r = sum(v[0] for v in px) / n; g = sum(v[1] for v in px) / n; b = sum(v[2] for v in px) / n
        hell = sum(1 for v in px if max(v) > 16)
        farbig = sum(1 for v in px if max(v) > 16 and (abs(v[0]-v[1]) > 12 or abs(v[1]-v[2]) > 12 or abs(v[0]-v[2]) > 12))
        q = p[:-4] + ".png"
        im.save(q)
        if not behalten:
            os.remove(p)
        print("%s  %dx%d  mittel=(%.2f,%.2f,%.2f)  nichtschwarz=%d  farbig=%d" % (
            os.path.basename(q), im.width, im.height, r, g, b, hell, farbig))

main()
