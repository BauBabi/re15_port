# -*- coding: utf-8 -*-
"""Runde 30 Nachschliff tischlicht: In welchen Cuts von ROOM1150 sind Buch (obj 5) und Karte
(obj 6) ueberhaupt zu sehen? Je Cut 0..8 zwei Laeufe der AUSGANGS-exe (cac33993):
basis_c<N> (Props da) und basis_c<N>_null (Bits (9,54)/(9,55) gesetzt, Props nie angelegt),
RE15_FORCE_CUT=N, Spieler weit weg (-20500,-24500). Gezaehlt: abweichende Pixel (960x720)
in Bild 400/500/600.

    python r30_tl_zensus.py   (Laeufe: build/r30_tischlicht/laeufe/, r30_tl_lauf.sh)
"""
import os
import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
LAEUFE = os.path.join(REPO, 'build', 'r30_tischlicht', 'laeufe')


def lade(lauf, b):
    return np.array(Image.open(os.path.join(LAEUFE, lauf, 'f_%06d.ppm' % b)).convert('RGB')).astype(int)


def main():
    for c in range(9):
        z = []
        for b in (400, 500, 600):
            try:
                d = np.abs(lade('basis_c%d' % c, b) - lade('basis_c%d_null' % c, b)).sum(axis=2) > 0
                z.append(int(d.sum()))
            except FileNotFoundError:
                z.append(None)
        print('Cut %d: abweichende Pixel Props gegen Nullbild F400/F500/F600 = %s' % (c, z))


if __name__ == '__main__':
    main()
