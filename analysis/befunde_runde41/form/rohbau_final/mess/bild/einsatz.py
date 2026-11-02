#!/usr/bin/env python3
"""Linse BILD: gefiltertes Render ins Original einsetzen und vergroessert vergleichen. Kein RE-Wert.

    python einsatz.py            Einsatz je Rohbau, Blaetter in diesem Ordner

Weg: Platte = Video-Umkehr ohne Figur (Bild 1 die Platte aus form_pruefung, Bild 2 Umkehr mit Raum aus der BSS unter der Figur, Feuer und Frau bleiben).
Render auf die Platte, durch video_filter mit Rauschen, dann nur die Figurzone ins Original zurueck. Ausserhalb bleibt jedes Pixel Original, das Logo auch.
"""
import json
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_buehne as FB        # noqa: E402
import form_pruefung as P       # noqa: E402
import form_video as FV         # noqa: E402

BAUTEN = (('F', 'rohbau_final'),)
SCHRIFT = cv2.FONT_HERSHEY_SIMPLEX


def rgb(p):
    return cv2.cvtColor(cv2.imread(p, cv2.IMREAD_COLOR), cv2.COLOR_BGR2RGB)


def platte(n):
    if n == 1:
        return rgb(P.daten('hintergrund_bild1.png'))
    e = np.clip(FV.video_umkehr(P.original(2), 2) + .5, 0, 255).astype(np.float32)
    raum = rgb(P.daten('hintergrund_bild2.png')).astype(np.float32)
    fig = P.nutzer_union(2).copy()
    z = cv2.imread(os.path.join(FORM, 'ziel2_bild2.png'), 0)
    fig |= z == 255
    fig[300:385, 330:420] = True                # Luecke zwischen den Armen: dunkel im Video, in keinem Teil, darf nicht als Platte durchscheinen
    a = cv2.GaussianBlur(P.weite(fig, 7).astype(np.float32), (0, 0), 2)[..., None]
    return np.clip(e * (1 - a) + raum * a + .5, 0, 255).astype(np.uint8)


def einsetzen(k, n, pl):
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    licht, _ = P.licht_fuer(n, k.licht)
    s = pl.copy()
    roh = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, licht), k.tex, s, kam.abbildung)
    m = P.geschlossen(roh)
    gef = FV.video_filter(s, bild=n, entzerren=False).astype(np.float32)
    orig = P.original(n).astype(np.float32)
    zone = m | P.nutzer_union(n)
    a = cv2.GaussianBlur(P.weite(zone, 12).astype(np.float32), (0, 0), 3)[..., None]
    logo = P.logo_maske().astype(np.float32)
    logo = np.clip(cv2.GaussianBlur(logo, (0, 0), 1)[..., None] * 1.5, 0, 1)
    voll = orig * (1 - a * (1 - logo)) + gef * a * (1 - logo)
    b = cv2.GaussianBlur(P.weite(m, 1).astype(np.float32), (0, 0), 1.5)[..., None]       # eng: nur wo das Modell steht, sonst Original
    eng = orig * (1 - b * (1 - logo)) + gef * b * (1 - logo)
    return np.clip(voll + .5, 0, 255).astype(np.uint8), np.clip(eng + .5, 0, 255).astype(np.uint8), m, s


def beschriften(a, t, f=(255, 255, 80)):
    cv2.putText(a, t, (7, 21), SCHRIFT, .62, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, t, (7, 21), SCHRIFT, .62, f, 1, cv2.LINE_AA)
    return a


def schnitt(a, box, s, ton=None):
    x0, y0, x1, y1 = box
    c = a[y0:y1, x0:x1].astype(np.float32)
    if ton:
        c = 255 * np.power(np.clip(c / 255 * ton[0], 0, 1), ton[1])
    return cv2.resize(np.clip(c, 0, 255).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)


def reihe(bilder, box, s, ziel, ton=None, spalten=2):
    t = [beschriften(schnitt(a, box, s, ton), name) for name, a in bilder]
    z = []
    for i in range(0, len(t), spalten):
        r = t[i:i + spalten]
        r = [cv2.copyMakeBorder(x, 2, 2, 2, 2, cv2.BORDER_CONSTANT, value=(16, 17, 20)) for x in r]
        while len(r) < spalten:
            r.append(np.full_like(r[0], 16))
        z.append(np.hstack(r))
    cv2.imwrite(ziel, np.vstack(z)[:, :, ::-1])


def schachbrett(o, e, kante=24):
    y, x = np.mgrid[0:o.shape[0], 0:o.shape[1]]
    w = ((x // kante + y // kante) % 2 == 0)[..., None]
    return np.where(w, o, e)


AUSSCHNITTE = {1: (('oben', (235, 175, 445, 335), 4), ('unten', (185, 320, 350, 480), 4), ('arm', (365, 232, 440, 385), 4)),
               2: (('kopf', (200, 140, 430, 345), 4), ('hand', (138, 310, 345, 480), 4), ('arm', (355, 250, 480, 425), 4))}
TON = {1: None, 2: None}


def main():
    for n in (1, 2):
        pl = platte(n)
        cv2.imwrite(os.path.join(HIER, 'platte_bild%d.png' % n), pl[:, :, ::-1])
        orig = P.original(n)
        rv, re = [('Original', orig)], [('Original', orig)]
        for name, ordner in BAUTEN:
            k = P.Kandidat(os.path.join(FORM, ordner, 'form.json'))
            v, e, m, s = einsetzen(k, n, pl)
            cv2.imwrite(os.path.join(HIER, 'einsatz_%s_bild%d.png' % (name, n)), v[:, :, ::-1])
            cv2.imwrite(os.path.join(HIER, 'eng_%s_bild%d.png' % (name, n)), e[:, :, ::-1])
            cv2.imwrite(os.path.join(HIER, 'maske_%s_bild%d.png' % (name, n)), m.astype(np.uint8) * 255)
            rv.append((name + ' Einsatz voll', v))
            re.append((name + ' Einsatz eng', e))
        reihe(rv, (0, 0, 640, 480), 1, os.path.join(HIER, 'gesamt_bild%d.png' % n))
        reihe(re, (0, 0, 640, 480), 1, os.path.join(HIER, 'gesamt_eng_bild%d.png' % n))
        for t, box, s in AUSSCHNITTE[n]:
            reihe(re, box, s, os.path.join(HIER, 'nah_b%d_%s_eng.png' % (n, t)))
            reihe(rv, box, s, os.path.join(HIER, 'nah_b%d_%s_voll.png' % (n, t)))
            reihe(re, box, s, os.path.join(HIER, 'nah_b%d_%s_hell.png' % (n, t)), ton=(2.2, .7))
    print('fertig')


if __name__ == '__main__':
    main()
