#!/usr/bin/env python3
"""Linse BILD, zweiter Weg ohne Saum: Modell auf die Umkehr des Originals gemalt, mit Figur darunter, dann derselbe Filter.
Keine Platte, also kein heller Fleck um die Figur. Wo das Modell schmaler ist als die Figur, bleibt deren Rand stehen und ist sichtbar. Kein RE-Wert."""
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from einsatz import BAUTEN, FORM, FB, FV, P, reihe


def auflegen(k, n):
    e = np.clip(FV.video_umkehr(P.original(n), n) + .5, 0, 255).astype(np.uint8)
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    licht, _ = P.licht_fuer(n, k.licht)
    s = e.copy()
    roh = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, licht), k.tex, s, kam.abbildung)
    m = P.geschlossen(roh)
    gef = FV.video_filter(s, bild=n, entzerren=False).astype(np.float32)
    o = P.original(n).astype(np.float32)
    a = cv2.GaussianBlur(P.weite(m, 8).astype(np.float32), (0, 0), 3)[..., None]
    logo = P.logo_maske().astype(np.float32)
    logo = np.clip(cv2.GaussianBlur(logo, (0, 0), 1)[..., None] * 1.5, 0, 1)
    a = a * (1 - logo)
    return np.clip(o * (1 - a) + gef * a + .5, 0, 255).astype(np.uint8)


BOXEN = {1: (('figur', (185, 175, 445, 480), 3), ('oben', (235, 175, 445, 335), 4), ('unten', (185, 320, 350, 480), 4), ('arm', (365, 232, 440, 385), 4)),
         2: (('figur', (135, 140, 480, 480), 3), ('kopf', (200, 140, 430, 345), 4), ('hand', (138, 310, 345, 480), 4), ('arm', (355, 250, 480, 425), 4))}


def main():
    for n in (1, 2):
        o = P.original(n)
        r = [('Original', o)]
        for name, ordner in BAUTEN:
            k = P.Kandidat(os.path.join(FORM, ordner, 'form.json'))
            a = auflegen(k, n)
            cv2.imwrite(os.path.join(HIER, 'auflage_%s_bild%d.png' % (name, n)), a[:, :, ::-1])
            r.append((name, a))
        for t, box, s in BOXEN[n]:
            reihe(r, box, s, os.path.join(HIER, 'auf_b%d_%s.png' % (n, t)))
            reihe(r, box, s, os.path.join(HIER, 'auf_b%d_%s_hell.png' % (n, t)), ton=(2.2, .7))
    print('fertig')


if __name__ == '__main__':
    main()
