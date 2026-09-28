#!/usr/bin/env python3
"""MESSUNG IM ECHTEN SPIEL: wie viele Bildpunkte traegt die Sicherung je Bild wirklich bei?

Vergleicht zwei Framedump-Laeufe desselben Ablaufs (gleiche Bildnummern), einen MIT dem
Prop (Bestand) und einen OHNE (flag(9,53) vor dem Raumeintritt gesetzt = Prop wird nicht
angelegt). Jeder Bildpunkt, der sich unterscheidet, gehoert zur Sicherung — das ist ihre
WAHRE sichtbare Flaeche, nach Deckeln, Plattform, PRI-Masken und allem anderen.

    python analysis/befunde_runde30/sicherung_werkzeug/sichtbar_im_spiel.py lauf_aot lauf_aot_ohne
"""
import os
import sys

import numpy as np
from PIL import Image

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
Z = os.path.join(REPO, "build", "r30_sicherung")


def main():
    a, b = sys.argv[1], sys.argv[2]
    bis = int(sys.argv[3]) if len(sys.argv) > 3 else 225
    da, db = os.path.join(Z, a), os.path.join(Z, b)
    namen = sorted(n for n in os.listdir(da) if n.endswith(".ppm") and os.path.exists(os.path.join(db, n)))
    print("Lauf MIT = %s, Lauf OHNE = %s, %d gemeinsame Bilder" % (a, b, len(namen)))
    print("Bild    Format     abweichende Px   = in 320x240   Anteil am Bild   bbox (320x240)")
    for n in namen:
        f = int(n[2:8])
        A = np.asarray(Image.open(os.path.join(da, n)).convert("RGB")).astype(int)
        B = np.asarray(Image.open(os.path.join(db, n)).convert("RGB")).astype(int)
        if A.shape != B.shape:
            print("%s  Format verschieden" % n)
            continue
        d = np.abs(A - B).max(2) > 4
        k = int(d.sum())
        h, w = d.shape
        s = w / 320.0
        if k:
            ys, xs = np.nonzero(d)
            bb = "x%.1f..%.1f y%.1f..%.1f" % (xs.min() / s, xs.max() / s, ys.min() / s, ys.max() / s)
        else:
            bb = "-"
        marke = "" if f <= bis else "   (Modal offen - Bildinhalt = Item-Bild, nicht die Szene)"
        print("F%-5d  %dx%d  %7d          %7.1f        %6.3f %%       %s%s"
              % (f, w, h, k, k / (s * s), 100.0 * k / (w * h), bb, marke))


if __name__ == "__main__":
    main()
