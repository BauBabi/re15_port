#!/usr/bin/env python3
"""Variante K5, gemeinsamer Schritt fuer alle Varianten (AE9, R9): Pose Bild 1 der Unterschenkel und Fuesse (Knochen 24-27, Clip 0) so, dass die
Bodenpixel in fuss_links/fuss_rechts frei bleiben (Pruefer: 189 + 160 px, kNN in Lab) und der dunkle Rest der Bein-/Fussschnitte gedeckt ist.
Huefte und Oberschenkel (8, 9) bleiben wie J (Pruefpunkte der Beinkanten). Keine "Sohle zur Kamera" mehr (E16-Nachtrag entfaellt, E26).
Dazu die Fusstextur ohne die tuerkisen Bodenfarben (fuss_r/fuss_l/zeh: Farbton des Unterschenkels, Helligkeitsstruktur bleibt), siehe k5_bau.

    python k5_fuss.py ../_lauf/fuss.json

Kein RE-Wert: Gestaltung (PORT-WAHL) nach Nutzersatz 2026-11-05 ("helle Dinge = Boden") und Pruefer-Messung."""
import json
import os
import sys
import time

import cv2
import numpy as np
from scipy.optimize import minimize

import k5_kern as C

KK = C.KK
TOOLS = KK.TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P  # noqa: E402

BEWEGT = (24, 25, 26, 27)


def lauf(aus, iters=600):
    t0 = time.time()
    f = C.Figur()
    kam = KK.kamera(1)
    Rj, Tj = f.welt(0)
    tris, kenn = f.dreiecke(Rj, Tj, ohne=BEWEGT)
    tief0, idb0 = C.raster(kam, tris, kenn)
    bl, br = C.boden_masken()
    boden = bl | br
    logo = P.logo_maske()
    teile = KK.maske('fuss_links') | KK.maske('fuss_rechts') | KK.maske('bein_links') | KK.maske('bein_rechts')
    dunkel = teile & ~boden & ~logo
    bereich = cv2.dilate(teile.astype(np.uint8), np.ones((15, 15), np.uint8)) > 0
    W0 = {b: list(f.winkel[0][b]) for b in BEWEGT}
    # Ziel Bild 1 mit Boden (E26): ziel2_bild1 fuehrt die Bodenbaender seit E16 als Figur (weiss); hier zaehlen sie als Hintergrund.
    z = cv2.imread(os.path.join(C.FORM, 'ziel2_bild1.png'), 0)
    weiss, grau = (z == 255) & ~boden, z == 128

    def umriss_e26(idb):
        fig = idb >= 0
        return float((fig & weiss).sum()) / max(float(((fig | weiss) & ~grau).sum()), 1.0)

    # Start: Fuss flach wie beim Stehen (Sohle = Welt unten, Zehen = Vorderrichtung der Figur, also von der Kamera weg): von hinten verdeckt der
    # Fuss sich hinter Ferse und Unterschenkel, keine Sohle zur Kamera (E26). Vorderrichtung = x-Achse der Huefte (0), waagerecht.
    import k5_fit as KF5
    vor = Rj[0][:, 0].copy()
    vor[1] = 0.0
    vor /= np.linalg.norm(vor)
    unten = np.array([0.0, 1.0, 0.0])
    Rf = np.stack([vor, unten, np.cross(vor, unten)], 1)
    for sch, fu in ((24, 25), (26, 27)):
        W0[fu] = KF5.winkel(Rj[sch].T @ Rf)

    def zerl(x):
        return {b: [W0[b][i] + 60 * x[3 * k + i] for i in range(3)] for k, b in enumerate(BEWEGT)}

    def mess(x):
        w = zerl(x)
        Rw, Tw = f.welt(0, w)
        tr, kn = f.dreiecke(Rw, Tw, knochen=BEWEGT)
        _t, idb = C.raster(kam, tr, kn, tief0.copy(), idb0.copy())
        fig = idb >= 0
        gedeckt = int((fig & boden).sum())
        iou = KK.iou(fig & bereich & ~logo, dunkel)
        return gedeckt, iou, umriss_e26(idb), idb

    def kosten(x):
        g, i, u, _ = mess(x)
        return 0.3 * g - 1.0 * i - 30.0 * u + 0.02 * float(np.sum(np.asarray(x) ** 2))
    gj, ij, uj, _ = mess(np.array([(f.winkel[0][b][i] - W0[b][i]) / 60.0 for b in BEWEGT for i in range(3)]))
    print('J (Sohle zur Kamera): Boden %d, IoU %.3f, Umriss %.4f' % (gj, ij, uj), flush=True)
    g0, i0, u0, _ = mess(np.zeros(12))
    print('Start Fuss flach: ', end='')
    print('J: Boden gedeckt %d, IoU dunkler Rest %.3f, Umriss (Ziel mit Boden E26) %.4f' % (g0, i0, u0), flush=True)
    best = None
    for st in range(4):
        x0 = np.zeros(12) if st == 0 else np.random.RandomState(30 + st).normal(0, 0.3, 12)
        r = minimize(kosten, x0, method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-5))
        g, i, u, _ = mess(r.x)
        print('  Start %d: Boden %d, IoU %.3f, Umriss %.4f (%.0f s)' % (st, g, i, u, time.time() - t0), flush=True)
        if best is None or r.fun < best.fun:
            best = r
    g, i, u, idb = mess(best.x)
    w = {b: [int(round(v)) for v in ww] for b, ww in zerl(best.x).items()}
    erg = dict(winkel_bild1=w, j=dict(boden=gj, iou=round(ij, 3), umriss_e26=round(uj, 4)), start_flach=dict(boden=g0, iou=round(i0, 3), umriss_e26=round(u0, 4)), neu=dict(boden=g, iou=round(i, 3), umriss_e26=round(u, 4)),
               anmerkung='Umriss gegen ziel2_bild1 mit den Bodenpixeln (Pruefer) als Hintergrund (E26)', sekunden=round(time.time() - t0))
    json.dump(erg, open(aus, 'w', encoding='utf-8'), indent=1)
    np.save(os.path.splitext(aus)[0] + '_id1.npy', idb)
    print('fertig: %s' % erg)


if __name__ == '__main__':
    lauf(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 600)
