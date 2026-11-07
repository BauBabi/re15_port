#!/usr/bin/env python3
"""Nachbesserung 2 (E22): grobe globale Suche der Kopflage in Bild 2 fuer die Kopfart 'kroko' (feste Masse), als Startlagen fuer k_kopf_fit2.

Halsrichtung (Hals 21 zeigt vom Halsansatz auf einen Bildpunkt), Schnauzenrichtung in der Welt (Gier/Nick um die Blickachse der Kamera),
Rollwinkel um die Schnauzenachse, Kiefer fest offen. Bewertet wie k_kopf_fit2 (IoU Oberkiefer x 1,5 + IoU Unterkiefer, Strafe unter dem
K3-Umriss), Gelenkkoerper am Kopf mitgerechnet. Ausgabe: die besten Lagen als Winkel (Engine-Einheiten) in JSON.

    python k_kopf_suche.py --aus ../_lauf/K4_suche.json [--form L=1800,Wk=400,...]

Kein RE-Wert."""
import argparse
import json
import math
import time

import numpy as np

import k_kern as KK
import k_form as KF
import k_kopf_fit as F
import k_kopf_fit2 as F2

FE = KK.FE
KIEFER = (300.0,)          # Kieferwinkel az (1/4096); --kiefer 300,450,600 fuer weit offene Maeuler


def rahmen_roll(u, roll):
    v0 = np.array([0, 1.0, 0])
    v = KF._einheit(v0 - (v0 @ u) * u)
    w = np.cross(u, v)
    c, s = math.cos(roll), math.sin(roll)
    v2 = c * v + s * w
    return np.stack([u, v2, np.cross(u, v2)], 1)


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--aus', required=True)
    a.add_argument('--form', default='')
    a.add_argument('--top', type=int, default=8)
    a.add_argument('--kiefer', default='300')
    x = a.parse_args()
    global KIEFER
    KIEFER = tuple(float(v) for v in x.kiefer.split(','))
    p = dict(KF.VORGABE_KROKO)
    for kv in x.form.split(','):
        if kv:
            k, v = kv.split('=')
            p[k] = float(v)
    form, mo, fj = KK.lade()
    b2 = F.Buehne(2, form, mo, fj)
    geo = F2.geometrie(p, b2.hals, b2.hals_f)
    R10, T10 = b2.Rw[10], b2.Tw[10]
    T21 = R10 @ b2.off[21] + T10
    t0 = time.time()
    erg = []
    hals_ziele = [(330.0, 285.0), (300.0, 260.0), (360.0, 260.0), (310.0, 310.0), (360.0, 310.0)]
    # Schnauzenrichtungen: Kugel, nur die Halbkugel, die nicht von der Kamera weg zeigt (Blickrichtung (-0,915, 0,219, 0,339))
    blick = np.array([-0.915, 0.219, 0.339])
    richtungen = []
    for th in np.radians(np.arange(10, 180, 20)):
        for ph in np.radians(np.arange(0, 360, 20)):
            d = np.array([math.sin(th) * math.cos(ph), math.cos(th), math.sin(th) * math.sin(ph)])
            if d @ blick < 0.35:
                richtungen.append(d)
    for hz in hals_ziele:
        B = F._auf_strahl(b2.kam, hz, T21, 503.0)
        R21 = F._rahmen(KF._einheit(B - T21))
        w21 = list(FE.winkel_aus_matrix(R10.T @ R21))
        R21 = R10 @ F.mat(w21)
        for d in richtungen:
            for roll in (-0.6, 0.0, 0.6):
                R22 = rahmen_roll(d, roll)
                w22 = list(FE.winkel_aus_matrix(R21.T @ R22))
                for kz in KIEFER:
                    _t, idb = b2.rendern(geo, w21, w22, [0.0, 0.0, kz])
                    m = b2.mass(idb)
                    sc = 1.5 * m['iou_ok'] + m['iou_uk'] + 10 * min(0.0, m['umriss'] - F2.UMRISS_MIN)
                    erg.append((sc, [round(v) for v in w21], [round(v) for v in w22], [0, 0, kz], m, list(hz), d.round(3).tolist(), roll))
        print('Hals %s: %d Lagen, bestes %.3f, %.0f s' % (hz, len(erg), max(e[0] for e in erg), time.time() - t0), flush=True)
    erg.sort(key=lambda e: -e[0])
    out = [dict(wert=round(e[0], 4), winkel={21: e[1], 22: e[2], 23: e[3]}, mass=e[4], hals_ziel=e[5], richtung=e[6], roll=e[7]) for e in erg[:x.top]]
    json.dump(dict(form=p, lagen=out, anzahl=len(erg)), open(x.aus, 'w', encoding='utf-8'), indent=1)
    for o in out:
        print(o['wert'], o['mass']['iou_ok'], o['mass']['iou_uk'], o['mass']['umriss'], o['richtung'], o['roll'], o['hals_ziel'])


if __name__ == '__main__':
    main()
