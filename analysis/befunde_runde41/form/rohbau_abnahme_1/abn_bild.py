#!/usr/bin/env python3
"""Abnahme 1: Original und gefiltertes Render nebeneinander, ohne Einsetzen ins Original. Kein RE-Wert.

Weg: Hintergrund der Pruefung (Bild 1 ausgemalte Platte, Bild 2 BSS), Render mit Lichtweg der Pruefung, durch video_filter, unveraendert daneben gestellt.
Nichts wird ins Original zurueckgemischt, also schmeichelt kein Saum.
"""
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_buehne as FB        # noqa: E402
import form_pruefung as P       # noqa: E402
import form_video as FV         # noqa: E402

K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'bild')
os.makedirs(AUS, exist_ok=True)
SCHRIFT = cv2.FONT_HERSHEY_SIMPLEX


def schreib(a, t, xy=(6, 18), g=.52, f=(255, 255, 90)):
    cv2.putText(a, t, xy, SCHRIFT, g, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, t, xy, SCHRIFT, g, f, 1, cv2.LINE_AA)
    return a


def schnitt(a, box, s, ton=None):
    x0, y0, x1, y1 = box
    c = a[y0:y1, x0:x1].astype(np.float32)
    if ton:
        c = 255 * np.power(np.clip(c / 255 * ton[0], 0, 1), ton[1])
    return cv2.resize(np.clip(c, 0, 255).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)


def speichere(tafeln, name):
    h = max(t.shape[0] for t in tafeln)
    z = []
    for t in tafeln:
        t = cv2.copyMakeBorder(t, 0, h - t.shape[0], 2, 2, cv2.BORDER_CONSTANT, value=(18, 18, 22))
        z.append(t)
    cv2.imwrite(os.path.join(AUS, name), np.hstack(z)[:, :, ::-1])


BOXEN = {1: (('figur', (180, 170, 445, 480), 3), ('kopfteil', (235, 175, 445, 335), 4), ('unten', (185, 320, 350, 480), 4), ('arm', (365, 232, 440, 385), 4)),
         2: (('figur', (135, 140, 480, 480), 3), ('kopf', (200, 140, 430, 345), 4), ('hand', (138, 310, 345, 480), 4), ('arm', (355, 250, 480, 425), 4))}


def main():
    k = P.Kandidat(K)
    for n in (1, 2):
        sauber, alb, m, _k = k.rendern(n)
        gef = FV.video_filter(sauber, bild=n, entzerren=False)
        o = P.original(n)
        cv2.imwrite(os.path.join(AUS, 'gefiltert_bild%d.png' % n), gef[:, :, ::-1])
        cv2.imwrite(os.path.join(AUS, 'sauber_bild%d.png' % n), sauber[:, :, ::-1])
        cv2.imwrite(os.path.join(AUS, 'maske_bild%d.png' % n), m.astype(np.uint8) * 255)
        speichere([schreib(o.copy(), 'Original Bild %d' % n), schreib(gef.copy(), 'Render gefiltert')], 'ganz_bild%d.png' % n)
        for t, box, s in BOXEN[n]:
            speichere([schreib(schnitt(o, box, s), 'Original'), schreib(schnitt(gef, box, s), 'Render gefiltert'),
                       schreib(schnitt(sauber, box, s), 'Render sauber')], 'nah_b%d_%s.png' % (n, t))
            speichere([schreib(schnitt(o, box, s, (2.4, .7)), 'Original hell'), schreib(schnitt(gef, box, s, (2.4, .7)), 'Render gefiltert hell')],
                      'nah_b%d_%s_hell.png' % (n, t))
        # Umriss des Renders auf dem Original, aufgehellt. Weiss = Modellumriss, nichts sonst
        hell = 255 * np.power(np.clip(o.astype(np.float32) / 255 * 2.2, 0, 1), .7)
        hell = cv2.resize(hell.astype(np.uint8), None, fx=2, fy=2, interpolation=cv2.INTER_CUBIC)
        mm = cv2.resize(m.astype(np.uint8), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
        ko, _ = cv2.findContours(mm, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(hell, ko, -1, (255, 255, 255), 1)
        cv2.imwrite(os.path.join(AUS, 'umriss_auf_original_bild%d.png' % n), hell[:, :, ::-1])
    print('fertig')


if __name__ == '__main__':
    main()
