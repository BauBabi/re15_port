# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: liegt die Figur VOR dem Tisch UEBER den Props?

Vier Laeufe desselben Bilds (r30_idw_bau_lauf.sh, Cut 2):
  P  = Props ja,  Spieler weit weg     (c2)
  P0 = Props nein, Spieler weit weg    (c2_null)
  S  = Props ja,  Spieler vor dem Tisch (c2_spieler)
  S0 = Props nein, Spieler vor dem Tisch (c2_spieler_null)
Prop-Pixel     = P != P0
Spieler-Pixel  = S0 != P0
Ueberschneidung O = beides. In O muss S == S0 gelten (die Figur liegt obenauf); jede
Abweichung S != S0 in O waere ein Prop, das die Figur UEBERMALT.

    python r30_idw_bau_ueberdeckung.py [bild]
"""
import os
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
BAU = os.path.join(REPO, 'build', 'r30_irons-diary-welt', 'bau')


def lade(lauf, b):
    return np.array(Image.open(os.path.join(BAU, lauf, 'f_%06d.ppm' % b)).convert('RGB')).astype(int)


def main():
    b = int(sys.argv[1]) if len(sys.argv) > 1 else 600
    P, P0, S, S0 = (lade(n, b) for n in ('c2', 'c2_null', 'c2_spieler', 'c2_spieler_null'))
    prop = np.abs(P - P0).sum(axis=2) > 0
    spieler = np.abs(S0 - P0).sum(axis=2) > 0
    o = prop & spieler
    oben = o & (np.abs(S - S0).sum(axis=2) == 0)
    unten = o & (np.abs(S - S0).sum(axis=2) > 0)
    rest = prop & ~spieler
    rest_sichtbar = rest & (np.abs(S - S0).sum(axis=2) > 0)
    print('Bild F%d: Prop-Pixel %d, Spieler-Pixel %d, Ueberschneidung %d' % (
        b, int(prop.sum()), int(spieler.sum()), int(o.sum())))
    print('   in der Ueberschneidung: Figur obenauf %d, Prop obenauf %d' % (int(oben.sum()), int(unten.sum())))
    print('   Prop-Pixel ausserhalb der Figur: %d, davon mit Spieler weiter sichtbar %d' % (
        int(rest.sum()), int(rest_sichtbar.sum())))


if __name__ == '__main__':
    main()
