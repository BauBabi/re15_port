#!/usr/bin/env python3
"""Varianten, Vergleich im sauberen Render: je Bild ein Ausschnitt in Spielgroesse (1-fach, Port-Fenster 640x480) und 3-fach, Kandidaten nebeneinander.
python vm_vergleich.py AUS.png "Name=form.json" ... Kein RE-Wert."""
import sys

import cv2
import numpy as np
from PIL import Image, ImageDraw

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

BOX = {1: (190, 175, 445, 470), 2: (190, 140, 440, 420)}


def main():
    ziel = sys.argv[1]
    ks = [(z.split('=', 1)[0], P.Kandidat(z.split('=', 1)[1])) for z in sys.argv[2:]]
    zeilen = []
    for n in (1, 2):
        x0, y0, x1, y1 = BOX[n]
        o = P.original(n)[y0:y1, x0:x1]
        r1, r3 = [('Screenshot %d' % n, o)], []
        for name, k in ks:
            s, _a, _m, _ = k.rendern(n)
            r1.append((name + ', sauber 1-fach', s[y0:y1, x0:x1]))
            r3.append((name + ', sauber 3-fach', cv2.resize(s[y0:y1, x0:x1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)))
        zeilen.append(r1)
        zeilen.append(r3)
    pad, kopf = 12, 24
    W = max(sum(a.shape[1] + pad for _, a in z) for z in zeilen) + pad
    H = sum(max(a.shape[0] for _, a in z) + kopf + pad for z in zeilen) + 40
    b = Image.new('RGB', (W, H), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 8), 'Sauberes Render (Port-Licht, ohne Videofilter): Spielgroesse 1-fach und 3-fach, naechster Nachbar', font=EA.font(17, True), fill=(240, 200, 90))
    y = 40
    for z in zeilen:
        x = pad
        for t, a in z:
            d.text((x, y), t, font=EA.font(14, True), fill=(214, 138, 74))
            b.paste(Image.fromarray(a), (x, y + kopf))
            x += a.shape[1] + pad
        y += max(a.shape[0] for _, a in z) + kopf + pad
    b.save(ziel)


if __name__ == '__main__':
    main()
