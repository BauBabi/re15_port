"""Tafel 21 (Runde 8c, Schwanzfarbe): Nutzerbild | vorher Probe b (66db2244) | neu (Runde 8c) im Spiellicht.

    python tafel21_r8c.py <aus.png> <fd_vorher> <fd_neu> <emd_vorher> <emd_neu>

Zeilen h22 / h24 / h64 = echte Framedumps der Szenenlaeufe (F = h + 243 -> Videoraum 640x480), PV = Vorlage-Pose offline (f_rast, RDT-Licht).
Nutzerbilder Version_80%/Areas*.png nur gelesen (mit den Umrissen des Nutzers). Keine Eichung, nur Ansicht.
"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tafel20 as T20

BOX = {22: (150, 200, 450, 470), 24: (120, 200, 420, 470), 64: (340, 230, 640, 480), 90: (200, 150, 480, 400)}
S = 1.6
NAMEN = ('Nutzerbild (Version_80%, Areas)', 'vorher: Probe b 66db2244', 'neu: Runde 8c (Standard b)')


def main():
    aus, fd_alt, fd_neu, emd_alt, emd_neu = sys.argv[1:6]
    try:
        font = ImageFont.truetype('arial.ttf', 15); fk = ImageFont.truetype('arial.ttf', 20)
    except OSError:
        font = fk = ImageFont.load_default()
    zeilen = []
    for h in (22, 24, 64, 90):
        b = BOX[h]
        w, hh = int((b[2] - b[0]) * S), int((b[3] - b[1]) * S)
        T = Image.new('RGB', (3 * (w + 6), hh + 24), (0, 0, 0))
        d = ImageDraw.Draw(T)
        bilder = [T20.nutzer(h), T20.pv(emd_alt) if h == 90 else T20.port640(fd_alt, h), T20.pv(emd_neu) if h == 90 else T20.port640(fd_neu, h)]
        for k, im in enumerate(bilder):
            T.paste(im.crop(b).resize((w, hh), Image.LANCZOS), (k * (w + 6), 24))
            d.text((k * (w + 6) + 4, 3), '%s - %s' % (NAMEN[k], 'h%d' % h if k == 0 else ('Pose PV offline' if h == 90 else 'Spiel F%d (h%d)' % (h + 243, h))),
                   fill=(255, 255, 0), font=font)
        zeilen.append(T)
    Wd = max(t.width for t in zeilen)
    kopf = 64
    S_ = Image.new('RGB', (Wd, kopf + sum(t.height + 8 for t in zeilen)), (0, 0, 0))
    d = ImageDraw.Draw(S_)
    d.text((6, 6), 'TAFEL 21 - Schwanzfarbe Runde 8c: Bauch heller und weniger rot, helle Ringe an den Gliedfugen', fill=(255, 255, 255), font=fk)
    d.text((6, 34), 'Nur andere ORIGINAL-Texel gewaehlt (RE1.5-Birkin, Zeile 480), keine Farbverschiebung. Zeilen 1-3 echte Framedumps der Hangar-Szene, '
           'Zeile 4 Vorlage-Pose PV offline.', fill=(200, 200, 200), font=font)
    y = kopf
    for t in zeilen:
        S_.paste(t, (0, y)); y += t.height + 8
    S_.save(aus)
    print(aus, S_.size)


if __name__ == '__main__':
    main()
