#!/usr/bin/env python3
"""Variante I (E16): Blaetter fuer Beine und Fuesse. python vm_beinblatt.py FORM.json AUSORDNER [Vergleich=form.json]
 - beine_bild1.png: wie nutzer_teile/beinkarte.png (Ausschnitt 170..450 x 230..470, 4-fach): Screenshot mit den Umrissen der vier Teile |
   Modell sauber | Modell durch video_filter; mit Vergleich eine zweite Reihe.
 - beine_rundum.png: nur Becken und Beine (Knochen 0, 8, 9, 24-27), Ruhepose und Pose 1, Ansichten Seite, hinten, vorn, von unten schraeg,
   Blattlicht, aufgehellt (Gamma 0,6). Kein RE-Wert."""
import os
import sys

import cv2
import numpy as np
from PIL import Image, ImageDraw

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import form_video as FV        # noqa: E402
import form_buehne as FB       # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402
import vm_matrix as VM         # noqa: E402

BOX = (170, 230, 450, 470)
S = 4
TEILE = (('bein_links', (90, 255, 90)), ('fuss_links', (255, 255, 60)), ('bein_rechts', (255, 60, 255)), ('fuss_rechts', (40, 140, 255)))
BEINE = {0, 8, 9, 24, 25, 26, 27}
ANSICHTEN = (('Seite (+Z)', 180, 5), ('hinten', -90, 8), ('vorn', 90, 8), ('von unten schraeg', 35, -38))


def reihe(k):
    s, _a, m, _ = k.rendern(1)
    g = FV.video_filter(s, bild=1, entzerren=False)
    o = P.original(1).copy()
    for t, f in TEILE:
        cs, _ = cv2.findContours(P.teil_roh(t).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(o, cs, -1, f, 1)
    x0, y0, x1, y1 = BOX
    gr = lambda a: cv2.resize(a[y0:y1, x0:x1], None, fx=S, fy=S, interpolation=cv2.INTER_NEAREST)   # noqa: E731
    return [gr(o), gr(s), gr(g)]


def blatt(zeilen, ziel, kopf):
    pad, top = 10, 34
    W = pad + 3 * (zeilen[0][1][0].shape[1] + pad)
    H = top + len(zeilen) * (zeilen[0][1][0].shape[0] + 30)
    b = Image.new('RGB', (W, H), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 8), kopf, font=EA.font(17, True), fill=(240, 200, 90))
    y = top
    for t, bl in zeilen:
        d.text((pad, y), t, font=EA.font(15, True), fill=(214, 138, 74))
        for j, a in enumerate(bl):
            b.paste(Image.fromarray(a), (pad + j * (a.shape[1] + pad), y + 22))
        y += bl[0].shape[0] + 30
    b.save(ziel)


def rundum(k, ziel):
    zeilen = []
    for clip, txt in ((2, 'Ruhepose'), (0, 'Pose 1 (Bild 1)')):
        kf = k.kf(clip, 0)
        mo = VM.nur_knochen(k.mo, BEINE)
        bl = []
        for t, az, el in ANSICHTEN:
            a = np.full((420, 420, 3), 64, np.uint8)
            m = FB.rastern(P.rundum_dreiecke(mo, kf, az, el, 420), k.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
            a[m] = np.clip(np.power(a[m] / 255.0, 0.6) * 255, 0, 255).astype(np.uint8)
            cv2.putText(a, t, (8, 412), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (240, 200, 90), 1, cv2.LINE_AA)
            bl.append(a)
        zeilen.append(('%s: Becken und Beine allein, Blattlicht, aufgehellt' % txt, bl))
    pad, top = 10, 34
    W = pad + 4 * (420 + pad)
    H = top + 2 * (420 + 30)
    b = Image.new('RGB', (W, H), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 8), 'Beine und Fuesse rundum (%s)' % k.name, font=EA.font(17, True), fill=(240, 200, 90))
    y = top
    for t, bl in zeilen:
        d.text((pad, y), t, font=EA.font(15, True), fill=(214, 138, 74))
        for j, a in enumerate(bl):
            b.paste(Image.fromarray(a), (pad + j * (420 + pad), y + 22))
        y += 420 + 30
    b.save(ziel)


def main():
    k = P.Kandidat(sys.argv[1])
    aus = sys.argv[2]
    zeilen = [('%s: Screenshot mit Umrissen bein/fuss | sauber | durch video_filter' % k.name, reihe(k))]
    for z in sys.argv[3:]:
        nm, pf = z.split('=', 1)
        zeilen.append(('%s: Screenshot | sauber | durch video_filter' % nm, reihe(P.Kandidat(pf))))
    blatt(zeilen, os.path.join(aus, 'beine_bild1.png'), 'Beine und Fuesse in Bild 1, Ausschnitt 170..450 x 230..470, 4-fach (wie nutzer_teile/beinkarte.png)')
    rundum(k, os.path.join(aus, 'beine_rundum.png'))


if __name__ == '__main__':
    main()
