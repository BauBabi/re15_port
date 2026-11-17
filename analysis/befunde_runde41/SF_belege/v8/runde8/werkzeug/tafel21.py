"""Tafel 21 (Runde 8, Schwanzfarbe): die drei Farbproben a / b / c nebeneinander im Spiellicht, links zum Vergleich der Stand vorher.

    python tafel21.py <aus.png> <fd_vorher> <fd_a> <fd_b> <fd_c> <emd_vorher> <emd_a> <emd_b> <emd_c>

Zeilen: h22 und h64 = echte Framedumps der Szenenlaeufe (Spiellicht des Port-Renderers, F = h + 243 -> Videoraum 640x480), PV = Vorlage-Pose
offline mit f_rast und RDT-Licht (Raum 50A0, wie die Riegel). Keine Eichung, nur Ansicht.
"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tafel20 as T20

BOX = {22: (190, 250, 430, 450), 64: (380, 300, 620, 470), 90: (230, 170, 420, 380)}
S = 2.0
NAMEN = ('vorher 07b6703f', 'Probe a: gedeckt, ohne Baender', 'Probe b: gedeckt, helle Baender (Standard)', 'Probe c: heutiger Ton nur Bauch, Baender')


def main():
    aus = sys.argv[1]
    fds = sys.argv[2:6]
    emds = sys.argv[6:10]
    try:
        font = T20.ImageFont.truetype('arial.ttf', 15); fk = T20.ImageFont.truetype('arial.ttf', 20)
    except OSError:
        font = fk = ImageFont.load_default()
    zeilen = []
    for h in (22, 64, 90):
        b = BOX[h]
        w, hh = int((b[2] - b[0]) * S), int((b[3] - b[1]) * S)
        T = Image.new('RGB', (4 * (w + 6), hh + 24), (0, 0, 0))
        d = ImageDraw.Draw(T)
        for k in range(4):
            im = T20.pv(emds[k]) if h == 90 else T20.port640(fds[k], h)
            T.paste(im.crop(b).resize((w, hh), Image.LANCZOS), (k * (w + 6), 24))
            d.text((k * (w + 6) + 4, 3), '%s - %s' % (NAMEN[k], 'Pose PV offline' if h == 90 else 'Spiel F%d (h%d)' % (h + 243, h)),
                   fill=(255, 255, 0), font=font)
        zeilen.append(T)
    Wd = max(t.width for t in zeilen)
    kopf = 64
    S_ = Image.new('RGB', (Wd, kopf + sum(t.height + 8 for t in zeilen)), (0, 0, 0))
    d = ImageDraw.Draw(S_)
    d.text((6, 6), 'TAFEL 21 - Schwanzfarbe Runde 8: drei Farbproben im Spiellicht (links zum Vergleich vorher)', fill=(255, 255, 255), font=fk)
    d.text((6, 34), 'Zeilen 1-2 echte Framedumps der Hangar-Szene, Zeile 3 Vorlage-Pose PV offline (f_rast, RDT-Licht). Alle Texel und '
           'Paletten = Original-RE1.5-Birkin bzw. Oberseite vorher.', fill=(200, 200, 200), font=font)
    y = kopf
    for t in zeilen:
        S_.paste(t, (0, y)); y += t.height + 8
    S_.save(aus)
    print(aus, S_.size)


if __name__ == '__main__':
    main()
