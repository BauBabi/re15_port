#!/usr/bin/env python3
"""Variante K5, Arbeitsansicht (kein Nutzerblatt): Fit-Ergebnis als Kennungsbild ueber Bild 2 mit Zielen und Modell-Landmarken, dazu der Kopf
(Kopf 22, Kiefer 23, Augen) schattiert aus 6 Winkeln in Pose Bild 2. Schnell, ohne EMD-Bau.

    python k5_sicht.py ../_lauf/K5a/fit.json AUS.png"""
import json
import math
import sys

import cv2
import numpy as np

import k5_kern as C
import k5_fit as F
import k5_kopf as K5

KK, FK = C.KK, C.FK
FARBE = {22: (60, 160, 230), 23: (200, 120, 255), C.AUGE_L: (0, 0, 255), C.AUGE_R: (0, 0, 255), 21: (120, 200, 120), 10: (150, 90, 60),
         0: (110, 110, 70), 7: (90, 120, 90)}


def zustand(fit):
    L = F.Lauf(fit['f4'], fit['f5'])
    L.start21, L.start22, L.start_hl = fit['start']['w21'], fit['start']['w22'], fit['start']['hl']
    L.arm_r = (np.array(fit['arm_r'][0]), np.array(fit['arm_r'][1]))
    x = np.array(fit['x'])
    p, w, hl, Rw, Tw = L.welt(x)
    L.fig.kopf_neu(p)
    L.fig.hals_neu(hl)
    return L, p, w, hl, Rw, Tw


def kennbild(L, p, Rw, Tw):
    tris, kenn = L.fig.dreiecke(Rw, Tw)
    _t, idb = C.raster(L.kam, tris, kenn)
    o = cv2.imread(KK.os.path.join(C.FORM, 'bild2.png'))
    a = (o * 0.55).astype(np.uint8)
    for k, f in FARBE.items():
        m = idb == k
        a[m] = (0.45 * a[m] + 0.55 * np.array(f)).astype(np.uint8)
    rest = (idb >= 0) & ~np.isin(idb, list(FARBE))
    a[rest] = (0.7 * a[rest] + 0.3 * np.array((200, 200, 200))).astype(np.uint8)
    for n, c in (('oberkiefer', (0, 140, 255)), ('unterkiefer', (255, 80, 200))):
        cs, _ = cv2.findContours(KK.maske(n).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(a, cs, -1, c, 1)
    q, _ = L.lm_bild(p, Rw, Tw)
    for nm, z in C.LANDMARKEN_BILD.items():
        cv2.drawMarker(a, (int(z[0]), int(z[1])), (0, 255, 0), cv2.MARKER_CROSS, 7, 1)
    for nm in ('auge_l', 'auge_r', 'ok_spitze', 'uk_spitze', 'maulwinkel_l'):
        cv2.circle(a, (int(round(q[nm][0])), int(round(q[nm][1]))), 3, (0, 255, 255), 1)
    return cv2.resize(a[140:470, 120:500], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)


def kopf_schatten(L, Rw, Tw, az, el, gross=360):
    """Kopf + Kiefer + Hals in Weltlage Pose Bild 2, orthografisch um die Kopfmitte, Lambert-Licht aus Blickrichtung oben links."""
    tris, kenn = L.fig.dreiecke(Rw, Tw, knochen=(21, 22, 23))
    mitte = Tw[22] + Rw[22] @ np.array([700.0, 0, 0])
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    q = (tris.reshape(-1, 3) - mitte) @ A.T
    s = gross / 3200.0
    xy = q[:, :2] * s + gross / 2
    z = q[:, 2].reshape(-1, 3)
    xy = xy.reshape(-1, 3, 2)
    n = np.cross(tris[:, 1] - tris[:, 0], tris[:, 2] - tris[:, 0])
    n /= np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9)
    licht = A.T @ (np.array([-0.4, -0.6, -0.7]) / np.linalg.norm([-0.4, -0.6, -0.7]))
    hell = 0.35 + 0.65 * np.abs(n @ licht)
    img = np.full((gross, gross, 3), 70, np.uint8)
    zb = np.full((gross, gross), np.inf)
    reihen = np.argsort(-z.mean(1))
    for i in reihen:
        P = xy[i].astype(np.int32)
        f = FARBE.get(int(kenn[i]), (180, 180, 180))
        c = tuple(int(v * hell[i]) for v in f)
        cv2.fillConvexPoly(img, P, c, lineType=cv2.LINE_AA)
    return img


def main(fit_json, aus):
    fit = json.load(open(fit_json, encoding='utf-8'))
    L, p, w, hl, Rw, Tw = zustand(fit)
    a = kennbild(L, p, Rw, Tw)
    ks = [kopf_schatten(L, Rw, Tw, az, el) for az, el in ((0, 0), (90, 0), (45, 20), (180, 0), (0, 80), (-45, 20))]
    # Blick der Kamera Bild 2 (zum Vergleich): Richtung aus der Kamera
    reihe = np.concatenate(ks, 1)
    breite = max(a.shape[1], reihe.shape[1])
    out = np.full((a.shape[0] + reihe.shape[0], breite, 3), 30, np.uint8)
    out[:a.shape[0], :a.shape[1]] = a
    out[a.shape[0]:, :reihe.shape[1]] = reihe
    txt = '%s f4=%s f5=%s S=%.0f e=%.2f Hals=%.0f' % (fit['name'], fit['f4'], fit['f5'], p['S'], K5.schnauze_e(p), hl)
    cv2.putText(out, txt, (a.shape[1] + 10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
    for i, (k, v) in enumerate(fit['bild2']['mass'].items()):
        if k.startswith('R'):
            cv2.putText(out, '%s %s %s' % (k, 'ok' if v['ok'] else 'NEIN', v['wert']), (a.shape[1] + 10, 60 + 24 * i), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                        (120, 255, 120) if v['ok'] else (80, 80, 255), 1)
    cv2.imwrite(aus, out)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
