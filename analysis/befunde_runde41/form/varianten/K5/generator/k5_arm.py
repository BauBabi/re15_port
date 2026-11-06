#!/usr/bin/env python3
"""Variante K5, Nachlauf rechter Arm (AE4, R8): Kopf, Hals, Rumpf aus dem Fit bleiben; nur Handgelenk-Ziel und Handdrehung des rechten Arms
(IK wie k5_fit) neu, mit Umriss Bild 2 als Hauptmass und der rechten Klaue (W) unter dem Kiefer: Nutzer-Linie "rechte Klaue" endet bei (281,434),
Krallenspitze im Nutzerschnitt (274,443) (FORMBLATT_V2 3.1). Bau v3: form_pruefung meldete bei K5a "Weiss fehlt (253-300, 374-443) 800 px" und
Klauenspitzen links ausserhalb (134-159, 327-359) - die Hand lag zu weit links.

    python k5_arm.py ../_lauf/K5a/fit.json [Schritte]      schreibt fit.json neu (alte Fassung als fit_vor_arm.json)

Kein RE-Wert: Gestaltung (PORT-WAHL)."""
import json
import os
import shutil
import sys
import time

import numpy as np
from scipy.optimize import minimize

import k5_kern as C
import k5_fit as F

KK = C.KK
KLAUE_BOX = (245, 370, 305, 450)      # x0, y0, x1, y1: Ziel-Weiss unter dem Kiefer, dort liegt die rechte Klaue (W)


def main(fit_json, iters=350):
    t0 = time.time()
    fit = json.load(open(fit_json, encoding='utf-8'))
    L = F.Lauf(fit['f4'], fit['f5'])
    L.start21, L.start22, L.start_hl = fit['start']['w21'], fit['start']['w22'], fit['start']['hl']
    nf = fit.get('nform', 10)
    x = list(fit['x'])
    x = np.array(x[:nf] + [0.0] * (F.NFORM - nf) + x[nf:])
    p, w, hl, Rw, Tw = L.welt(x, mit_arm=False)
    L.fig.kopf_neu(p)
    L.fig.hals_neu(hl)
    import cv2
    z = cv2.imread(os.path.join(C.FORM, 'ziel2_bild2.png'), 0)
    weiss = z == 255
    box = np.zeros_like(weiss)
    box[KLAUE_BOX[1]:KLAUE_BOX[3], KLAUE_BOX[0]:KLAUE_BOX[2]] = True
    box &= weiss & ~L.ukm

    def kosten(ya):
        Rh0, Th0 = L.hand_r
        L.arm_r = (Th0 + 400.0 * np.asarray(ya[:3]), F.KFIT_mat_welt(ya[3:6]) @ Rh0)
        _p, _w, _hl, Rw2, Tw2 = L.welt(x)
        _t, idb = L.raster(Rw2, Tw2)
        arm = np.isin(idb, C.RECHTER_ARM)
        u = C.umriss(idb, 2)
        deck = float((arm & box).sum()) / max(float(box.sum()), 1.0)
        im_kiefer = float((arm & (L.okm | L.ukm)).sum())
        return -(10.0 * u + 1.0 * deck + 1.0 * KK.iou(arm, L.raf)) + im_kiefer / 400.0 + 0.01 * float(np.sum(np.asarray(ya) ** 2)), u, deck

    alt = fit['arm_r']
    Rh0, Th0 = L.hand_r
    y_alt = np.zeros(6)
    y_alt[:3] = (np.array(alt[0]) - Th0) / 400.0
    starts = [y_alt, np.zeros(6)]
    for s in range(2):
        starts.append(y_alt + np.random.RandomState(50 + s).normal(0, 0.3, 6))
    k_alt = kosten(y_alt)
    best = None
    for y0 in starts:
        r = minimize(lambda ya: kosten(ya)[0], y0, method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-5))
        if best is None or r.fun < best.fun:
            best = r
    k_neu = kosten(best.x)
    _p, w2, _hl, Rw2, Tw2 = L.welt(x)
    print('%s: Umriss %.4f -> %.4f, Klaue-Box %.3f -> %.3f (%.0f s)' % (fit['name'], k_alt[1], k_neu[1], k_alt[2], k_neu[2], time.time() - t0))
    if k_neu[0] < k_alt[0]:
        shutil.copy(fit_json, os.path.splitext(fit_json)[0] + '_vor_arm.json')
        for b in C.RECHTER_ARM:
            if b in w2:
                fit['bild2']['winkel'][str(b)] = [int(round(v)) for v in w2[b]]
        fit['arm_r'] = [L.arm_r[0].tolist(), L.arm_r[1].tolist()]
        fit['arm_nachlauf'] = dict(umriss_vor=k_alt[1], umriss_nach=k_neu[1], klaue_box_vor=round(k_alt[2], 3), klaue_box_nach=round(k_neu[2], 3))
        json.dump(fit, open(fit_json, 'w', encoding='utf-8'), indent=1)


if __name__ == '__main__':
    main(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 350)
