# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: Auswertung eines Rig-Bilds gegen den reinen Hintergrund.

Zaehlt im 960x720-Framedump die Pixel, die vom Hintergrund des Cuts abweichen - getrennt
fuer das Feld des Dokuments und das Feld der Karte (Huellen aus dem Kontrollabzug
abzug.txt, um 2 Pixel erweitert). 0 abweichende Pixel = das Prop ist NICHT zu sehen.

    python r30_idw_rig_auswertung.py <lauf-ordner> <cut> [bild]
"""
import os, sys
import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
AUS = os.path.join(REPO, 'build', 'r30_irons-diary-welt')

FELDER = {   # 320er-Koordinaten (x0, x1, y0, y1), aus abzug.txt (Fall A_marke), +-2
    2: {'Dokument': (145, 160, 121, 132), 'Karte': (134, 146, 122, 131)},
    6: {'Dokument': (176, 212, 116, 159), 'Karte': (146, 171, 118, 148)},
}


def main():
    lauf = sys.argv[1]; cut = int(sys.argv[2])
    name = sys.argv[3] if len(sys.argv) > 3 else 'f_000600'
    ordner = os.path.join(AUS, 'rig_lauf', lauf)
    F = np.array(Image.open(os.path.join(ordner, name + '.ppm')).convert('RGB')).astype(int)
    # Bezug ist ein LEERLAUF desselben Rigs (Props aus), NICHT das BG-PNG: der Framedump ist
    # 960x720 aus dem beschleunigten Renderer, ein hochskaliertes PNG weicht ueberall ab
    # (gemessen: 19791 Pixel) und taugt nicht als Nullbild.
    leer = os.path.join(AUS, 'rig_lauf', 'c%d_leer' % cut, name + '.ppm')
    B = np.array(Image.open(leer).convert('RGB')).astype(int)
    D = np.abs(F - B).sum(axis=2)
    print('%s/%s gegen Leerlauf c%d_leer: ganzes Bild %d Pixel abweichend (>0), groesste Abweichung %d'
          % (lauf, name, cut, int((D > 0).sum()), int(D.max())))
    for n, (x0, x1, y0, y1) in FELDER[cut].items():
        d = D[y0 * 3:y1 * 3, x0 * 3:x1 * 3]
        ys, xs = np.nonzero(d > 0)
        if len(xs):
            print('   Feld %-8s x %d..%d y %d..%d: %5d von %5d Pixeln abweichend; Huelle (320er) x %.1f..%.1f y %.1f..%.1f, Mitte (%.2f, %.2f)'
                  % (n, x0, x1, y0, y1, len(xs), d.size,
                     x0 + xs.min() / 3.0, x0 + (xs.max() + 1) / 3.0, y0 + ys.min() / 3.0, y0 + (ys.max() + 1) / 3.0,
                     x0 + (xs.min() + xs.max() + 1) / 6.0, y0 + (ys.min() + ys.max() + 1) / 6.0))
        else:
            print('   Feld %-8s x %d..%d y %d..%d:     0 von %5d Pixeln abweichend  -> NICHT ZU SEHEN'
                  % (n, x0, x1, y0, y1, d.size))
    Image.fromarray(F.astype('uint8')).crop((90 * 3, 100 * 3, 230 * 3, 150 * 3) if cut == 2 else (60 * 3, 60 * 3, 300 * 3, 200 * 3)
                                            ).save(os.path.join(ordner, name + '_tisch.png'))


if __name__ == '__main__':
    main()
