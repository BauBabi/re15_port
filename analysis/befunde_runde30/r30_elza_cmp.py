"""Pixelvergleich zweier Messordner (k<laufnr>_f<bild>.ppm oder .png, gleiche Namen).
Aufruf: python r30_elza_cmp.py <ordner_a> <ordner_b>
Je Bild die Zahl abweichender Pixel; am Ende die Summe. Vorlage: elza_cmp.py (Runde 35).
"""
import os, sys, glob
from PIL import Image, ImageChops

def main():
    a, b = sys.argv[1], sys.argv[2]
    fa = sorted(glob.glob(os.path.join(a, "k*_f*.ppm")) + glob.glob(os.path.join(a, "k*_f*.png")))
    tot = 0; n = 0
    for pa in fa:
        nm = os.path.basename(pa)
        pb = os.path.join(b, nm)
        if not os.path.exists(pb):
            print("%s fehlt in B" % nm); continue
        ia = Image.open(pa).convert("RGB"); ib = Image.open(pb).convert("RGB")
        if ia.size != ib.size:
            print("%s GROESSE %s vs %s" % (nm, ia.size, ib.size)); continue
        d = ImageChops.difference(ia, ib).convert("L").point(lambda v: 255 if v else 0)
        cnt = d.histogram()[255]
        tot += cnt; n += 1
        print("%s  abweichende Pixel %d von %d" % (nm, cnt, ia.size[0] * ia.size[1]))
    print("BILDER verglichen: %d   SUMME abweichende Pixel: %d" % (n, tot))

main()
