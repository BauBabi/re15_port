#!/usr/bin/env python
"""Runde 30 karten-marken-abzug: Pixelvergleich eigener Abzug (320x240, s_fb5) gegen
den Abzug des Nutzers (960x720, F9-Marke).
Der Nutzer-Abzug wird auf 320x240 zurueckgefuehrt (jedes 3x3-Feld muss EINE Farbe
tragen - geprueft und gemeldet), dann Punkt fuer Punkt verglichen.
Aufruf: python r30_abzug_vergleich.py <eigen.bmp> <nutzer.png> [<diff.png>]"""
import sys
import numpy as np
from PIL import Image

def main():
    a = np.array(Image.open(sys.argv[1]).convert('RGB'))
    b = np.array(Image.open(sys.argv[2]).convert('RGB'))
    print("eigen  %s %s" % (sys.argv[1], a.shape))
    print("nutzer %s %s" % (sys.argv[2], b.shape))
    if b.shape[0] == 720:
        bb = b.reshape(240, 3, 320, 3, 3)
        ecke = bb[:, 0, :, 0, :]
        uneinheitlich = int(np.any(bb != ecke[:, None, :, None, :], axis=(1, 3, 4)).sum())
        print("Nutzer-Abzug: %d von 76800 3x3-Feldern sind NICHT einfarbig" % uneinheitlich)
        b = ecke
    d = np.any(a != b, axis=2)
    n = int(d.sum())
    print("abweichende Punkte (320x240): %d von %d" % (n, d.size))
    if n:
        ys, xs = np.nonzero(d)
        print("   Bbox x %d..%d  y %d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
        from collections import Counter
        c = Counter((tuple(a[y, x]), tuple(b[y, x])) for y, x in zip(ys, xs))
        for (ca, cb), k in c.most_common(12):
            print("   eigen %-16s nutzer %-16s %5d" % (ca, cb, k))
        # Kartenfeld getrennt (ohne den pulsierenden Spielermarker und Rahmen)
        feld = d[25:215, 20:300]
        print("   davon im Kartenfeld x 20..299 / y 25..214: %d" % int(feld.sum()))
    if len(sys.argv) > 3:
        out = np.zeros_like(a)
        out[...] = (a // 3)
        out[d] = (255, 0, 255)
        Image.fromarray(out).resize((960, 720), Image.NEAREST).save(sys.argv[3])

if __name__ == '__main__':
    main()
