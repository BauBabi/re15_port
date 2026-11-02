#!/usr/bin/env python3
"""Varianten, Nahblatt: Screenshot | gefiltert | sauber (aufgehellt), Bild 1 und 2, 3-fach; dazu Nahausschnitte einer Wunde und eines Hautstuecks
im sauberen Render (6-fach, naechster Nachbar). python vm_nah.py FORM.json AUS.png [Beschriftung]. Kein RE-Wert."""
import sys

import cv2
import numpy as np
from PIL import Image, ImageDraw

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import form_video as FV        # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

BOX = {1: (190, 175, 445, 470), 2: (190, 140, 440, 420)}
# Nahausschnitte (Bild, x0, y0, x1, y1, Titel): Wunde Z1 (rosa Stelle) und Z5/Kerbe, Hautstueck am linken Bein (Knollen) und Ruecken
NAH = [(1, 296, 192, 346, 242, 'Wunde Z1 (rosa Stelle), Bild 1'), (1, 372, 262, 422, 312, 'Wunde Z5 und Arm, Bild 1'),
       (1, 196, 330, 246, 380, 'Haut linkes Bein, Bild 1'), (1, 250, 200, 300, 250, 'Haut Schwanz/Ruecken, Bild 1'),
       (2, 240, 290, 300, 350, 'Wunde Q und G, Bild 2')]


def main():
    k = P.Kandidat(sys.argv[1])
    titel = sys.argv[3] if len(sys.argv) > 3 else k.name
    reihen, nah = [], []
    r = {}
    for n in (1, 2):
        s, alb, m, _ = k.rendern(n)
        g = FV.video_filter(s, bild=n, entzerren=False)
        o = P.original(n)
        r[n] = (s, g, o)
        x0, y0, x1, y1 = BOX[n]
        hell = np.clip(np.power(s / 255.0, 0.55) * 255, 0, 255).astype(np.uint8)
        gr = lambda a: cv2.resize(a[y0:y1, x0:x1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)   # noqa: E731
        reihen.append(('Bild %d: Screenshot | durch video_filter | sauber, aufgehellt (Gamma 0,55)' % n, np.hstack([gr(o), gr(g), gr(hell)])))
    for n, x0, y0, x1, y1, t in NAH:
        s, g, o = r[n]
        hell = np.clip(np.power(s / 255.0, 0.55) * 255, 0, 255).astype(np.uint8)
        gr = lambda a: cv2.resize(a[y0:y1, x0:x1], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)   # noqa: E731
        nah.append((t, np.hstack([gr(o), gr(s), gr(hell)])))
    W = max(a.shape[1] for _, a in reihen)
    hn = sum(a.shape[0] + 30 for _, a in nah[:3])
    H = 40 + sum(a.shape[0] + 30 for _, a in reihen) + max(hn, 1)
    b = Image.new('RGB', (W + 20, H + 300), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((10, 8), titel, font=EA.font(18, True), fill=(240, 200, 90))
    y = 40
    for t, a in reihen:
        d.text((10, y), t, font=EA.font(15, True), fill=(214, 138, 74))
        b.paste(Image.fromarray(a), (10, y + 24))
        y += a.shape[0] + 30
    x, y0_ = 10, y
    for i, (t, a) in enumerate(nah):
        if x + a.shape[1] > W + 10:
            x, y = 10, y + a.shape[0] + 30
        d.text((x, y), t + ': Screenshot | sauber | sauber aufgehellt, 6-fach', font=EA.font(14, True), fill=(214, 138, 74))
        b.paste(Image.fromarray(a), (x, y + 22))
        x += a.shape[1] + 12
    y += nah[-1][1].shape[0] + 40
    b.crop((0, 0, W + 20, y)).save(sys.argv[2])


if __name__ == '__main__':
    main()
