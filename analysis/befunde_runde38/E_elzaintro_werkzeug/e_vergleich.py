"""Runde 38 Spur E: Bild-fuer-Bild-Vergleich zweier Messordner (k<nr>_f<bild>.ppm, Waechter e_waechter.py).

Aufruf:  python e_vergleich.py <ordner_a> <ordner_b> [--abschnitt]
Abschnitte: die Bildnummer faellt bei jedem Raumwechsel auf 0 zurueck -> Abschnitt n = n-ter Neubeginn.
Je Abschnitt und Bild: Anzahl verschiedener Pixel (a gegen b), Anzahl Pixel mit max(R,G,B) > 16 in a und b.
Gibt nur Bilder mit Unterschied aus, plus eine Zusammenfassung je Abschnitt.
"""
import os, sys, glob, re
from PIL import Image

def abschnitte(d):
    dateien = sorted(glob.glob(os.path.join(d, "k*_f*.ppm")) + glob.glob(os.path.join(d, "k*_f*.png")))
    out, ab, vorher = {}, 0, None
    for p in dateien:
        m = re.search(r"k(\d+)_f(\d+)\.", os.path.basename(p))
        f = int(m.group(2))
        if vorher is not None and f < vorher:
            ab += 1
        vorher = f
        out[(ab, f)] = p
    return out

def hell(px):
    return sum(1 for v in px if max(v) > 16)

def main():
    a, b = abschnitte(sys.argv[1]), abschnitte(sys.argv[2])
    gemeinsam = sorted(set(a) & set(b))
    zus = {}
    for key in gemeinsam:
        pa = list(Image.open(a[key]).convert("RGB").getdata())
        pb = list(Image.open(b[key]).convert("RGB").getdata())
        diff = sum(1 for x, y in zip(pa, pb) if x != y)
        z = zus.setdefault(key[0], [0, 0, None, None])
        z[0] += 1
        if diff:
            z[1] += 1
            if z[2] is None: z[2] = key[1]
            z[3] = key[1]
            print("abschnitt %d F%04d  verschieden=%6d  hell_a=%6d hell_b=%6d" % (key[0], key[1], diff, hell(pa), hell(pb)))
    for ab, z in sorted(zus.items()):
        print("== abschnitt %d: %d Bilder verglichen, %d verschieden (erstes F%s, letztes F%s)" % (ab, z[0], z[1], z[2], z[3]))

main()
