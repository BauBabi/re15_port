# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: AUSWERTUNG der Abnahme-Laeufe am GEBAUTEN SPIEL.

Zaehlt im 960x720-Framedump die Pixel, die von einem NULLBILD abweichen. Das Nullbild ist
derselbe Lauf mit gesetzten Genommen-Bits (9,54)/(9,55) (r30_idw_bau_lauf.sh ... null) -
kein BG-PNG, kein Rig. Ausgegeben: Anzahl, Huelle und Mitte der abweichenden Pixel in
320er-Koordinaten (stetig, Pixel p deckt [p, p+1): x320 = (x960 + 0.5)/3 - dieselbe
Konvention wie die Marken aus marken.txt und die Projektion 160 + H*vx/vz), Abstand der Mitte
zur Nutzermarke und das mittlere RGB der abweichenden Pixel im Prop-Bild.
(Korrigiert in der Nachbesserung: vorher x320 = (x960 + 0.5)/3 - 0.5, ein halbes Pixel Versatz
in x UND y gegen die Marken.)

    python r30_idw_bau_auswertung.py <lauf> <nullauf> [bild ...] [--marke x,y] [--ausschnitt]

Laeufe liegen unter build/r30_irons-diary-welt/bau/<lauf>/f_<bild>.ppm.
"""
import os
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
BAU = os.path.join(REPO, 'build', 'r30_irons-diary-welt', 'bau')


def lade(lauf, bild):
    return np.array(Image.open(os.path.join(BAU, lauf, 'f_%06d.ppm' % bild)).convert('RGB')).astype(int)


def main():
    a = sys.argv[1:]
    marke = None
    if '--marke' in a:
        i = a.index('--marke')
        marke = tuple(float(v) for v in a[i + 1].split(','))
        del a[i:i + 2]
    ausschnitt = '--ausschnitt' in a
    a = [x for x in a if x != '--ausschnitt']
    lauf, null = a[0], a[1]
    bilder = [int(x) for x in a[2:]] or [400, 500, 600]
    for b in bilder:
        F = lade(lauf, b)
        N = lade(null, b)
        D = np.abs(F - N).sum(axis=2)
        ys, xs = np.nonzero(D > 0)
        if not len(xs):
            print('%s F%d gegen %s: 0 abweichende Pixel' % (lauf, b, null))
            continue
        x0, x1 = xs.min(), xs.max()
        y0, y1 = ys.min(), ys.max()
        mx = ((x0 + x1) / 2.0 + 0.5) / 3.0
        my = ((y0 + y1) / 2.0 + 0.5) / 3.0
        rgb = F[ys, xs].mean(axis=0)
        txt = ('%s F%d gegen %s: %d abweichende Pixel (960x720), Huelle x %d..%d y %d..%d, '
               'Mitte (320er) (%.2f ; %.2f), Mittel-RGB im Bild (%.0f,%.0f,%.0f)'
               % (lauf, b, null, len(xs), x0, x1, y0, y1, mx, my, rgb[0], rgb[1], rgb[2]))
        if marke:
            txt += ', Abstand zur Marke (%.1f ; %.1f) = %.2f px' % (
                marke[0], marke[1], float(np.hypot(mx - marke[0], my - marke[1])))
        print(txt)
        if ausschnitt:
            cx, cy = int((x0 + x1) / 2), int((y0 + y1) / 2)
            box = (max(0, cx - 150), max(0, cy - 100), min(960, cx + 150), min(720, cy + 100))
            Image.fromarray(F.astype('uint8')).crop(box).resize(
                ((box[2] - box[0]) * 2, (box[3] - box[1]) * 2), Image.NEAREST).save(
                os.path.join(BAU, lauf, 'f_%06d_ausschnitt.png' % b))


if __name__ == '__main__':
    main()
