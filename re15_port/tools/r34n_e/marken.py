# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: die roten Nutzer-Marken in elliot.bmp / marvin.bmp / interrogation.bmp.

Kein Schwellwert nach Gefuehl (Vorbild: analysis/befunde_runde30/werkzeuge/r30_idw_marken.py):
  1. CUT: das Nutzerbild gegen JEDEN Hintergrund des Raums stellen (Engine-Dekoder,
     probe_bg_dump -> build/r34n_e/bg/ROOM<rrr><cc>.ppm) - mittlere Abweichung je Pixel
     AUSSERHALB der Marke. Kleinste = Cut des Bilds; dazu der Abstand zum Zweitbesten.
  2. MARKE: die Pixel, die vom Hintergrund des Cuts abweichen UND exakt die Markenfarbe tragen
     (Farbzensus der abweichenden Pixel). Huelle, Mitte in Pixelmitten-Konvention
     (Pixel p deckt [p, p+1), Mitte = (min + max + 1) / 2 - dieselbe wie Runde 30 9.6.4).

Aufruf: python re15_port/tools/r34n_e/marken.py <bild> <raum-hex> [--bg build/r34n_e/bg]
"""
import argparse
import collections
import glob
import os

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("bild")
    ap.add_argument("raum")
    ap.add_argument("--bg", default=os.path.join(REPO, "build", "r34n_e", "bg"))
    a = ap.parse_args()
    N = np.array(Image.open(a.bild).convert("RGB")).astype(int)
    grp = a.raum[:3]
    rot = (N[:, :, 0] == 237) & (N[:, :, 1] == 28) & (N[:, :, 2] == 36)
    print("Nutzerbild %s: %dx%d, Pixel mit exakter Markenfarbe (237,28,36): %d"
          % (os.path.basename(a.bild), N.shape[1], N.shape[0], int(rot.sum())))
    erg = []
    for p in sorted(glob.glob(os.path.join(a.bg, "ROOM%s??.ppm" % grp))):
        B = np.array(Image.open(p).convert("RGB")).astype(int)
        if B.shape != N.shape:
            continue
        D = np.abs(N - B).sum(axis=2)
        cut = int(os.path.basename(p)[7:9])
        erg.append((D[~rot].mean(), cut, float(np.median(D[~rot])), p))
    erg.sort()
    for m, cut, med, p in erg:
        print("  Cut %2d: mittlere Abweichung ausserhalb der Marke %7.2f  Median %5.1f" % (cut, m, med))
    m, cut, med, p = erg[0]
    print("CUT = %d (Abstand zum Zweitbesten: %.2f gegen %.2f)" % (cut, m, erg[1][0]))
    B = np.array(Image.open(p).convert("RGB")).astype(int)
    D = np.abs(N - B).sum(axis=2)
    ys, xs = np.nonzero(D > 0)
    c = collections.Counter(tuple(N[y, x]) for y, x in zip(ys, xs))
    print("abweichende Pixel gegen Cut %d: %d; haeufigste Farben %s" % (cut, len(xs), c.most_common(4)))
    ys, xs = np.nonzero(rot & (D > 0))
    print("MARKE: %d Pixel, x %d..%d, y %d..%d, Huelle %dx%d (voll: %s)"
          % (len(xs), xs.min(), xs.max(), ys.min(), ys.max(), xs.max() - xs.min() + 1,
             ys.max() - ys.min() + 1,
             len(xs) == (xs.max() - xs.min() + 1) * (ys.max() - ys.min() + 1)))
    print("MITTE (Pixelmitten) = (%.1f ; %.1f)" % ((xs.min() + xs.max() + 1) / 2.0, (ys.min() + ys.max() + 1) / 2.0))


if __name__ == "__main__":
    main()
