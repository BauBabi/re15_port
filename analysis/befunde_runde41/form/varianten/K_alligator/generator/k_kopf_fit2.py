#!/usr/bin/env python3
"""Nachbesserung 2 (E22), Kopf-Fit fuer die Kopfart 'kroko' (k_form.kopf_kroko/kiefer_kroko): Pose Hals 21 / Kopf 22 / Kiefer 23 in Bild 2 und
die Kopfmasse (Laenge, Schaedelbreite, Verjuengung, Schnauzenhoehe) in GRENZEN, die die Krokodilform halten (Keil, Spitze schmaler als der Schaedel),
gegen die Nutzer-Teile Oberkiefer/Unterkiefer. Anders als k_kopf_fit:
 - die Gelenkkoerper am Kopf (Muffe Kopf - Hals am Kind 22, Mundwinkel Kiefer - Kopf am Eltern 22, Masse wie k_naht mit den Faktoren von K3)
   zaehlen schon im Fit mit; bei K3 senkten sie die Oberkiefer-IoU nachtraeglich von 0,57 auf 0,51;
 - mehrere Startlagen des Kopfes (Spitze zu verschiedenen Bildpunkten), nicht nur die Lesung "Spitze links bei (220, 290)";
 - Umriss Bild 2 darf nicht unter den K3-Wert dieses Rasters (0,9426) fallen, Unterkiefer nicht unter 0,80.

    python k_kopf_fit2.py --name K4 --aus ../_lauf/K4/kopf.json [--iters 900] [--starts 5]

Kein RE-Wert: Gestaltung eines neuen Modells nach Videostandbild und Nutzer-Teilen (E20/E22)."""
import argparse
import json
import math
import os
import time

import numpy as np
from scipy.optimize import minimize

import k_kern as KK
import k_form as KF
import k_kopf_fit as F
import k_naht as KN

UMRISS_MIN = 0.9426       # K3 in diesem Raster (K3_fit/kopf.json bild2.mass.umriss); form_pruefung gab dafuer 0,948
UK_MIN = 0.80             # Unterkiefer-IoU von K3 mit Gelenkkoerpern (K.md Abschnitt 6: 0,801)
# Gelenkkoerper am Kopf wie K3 (K3_fit/naht.json): Muffe Kopf - Hals fq 1,0 / fa 1,6, Kiefer - Kopf (Mundwinkel) fq 0,8 / fn 1,0
MUFFE = (1.0, 1.6)
MUNDWINKEL = (0.8, 1.0)
# Grenzen der freien Kopfmasse: (Vorgabe, min, max)
# Erster Lauf mit Schaedel 470/-470 und Schnauze 570 hoch: Kopf im Bild 10000-13000 px gegen 5832 px Maske, IoU 0,30 (_lauf/K4.log);
# darum klein und flach wie ein Krokodil (Schnauze 280-390 hoch, Schaedeldach -230).
FREI = {'L': (1800.0, 1500.0, 2400.0), 'Wk': (360.0, 300.0, 460.0), 'taper': (0.38, 0.30, 0.50), 'yt_s': (-120.0, -220.0, -60.0),
        'yt_k': (-230.0, -330.0, -180.0), 'yt_t': (-10.0, -80.0, 30.0),
        # zweiter Lauf (_lauf/K4b.log: IoU 0,43 / Unterkiefer 0,71): Krokodil-Merkmale, die die Maske fuellen helfen, ohne die Spitze breit
        # zu machen: Nasenhoecker/Rosette an der Spitze, Unterkieferbreite (Band 37 px), obere Zahnlaenge (hingen ins Unterkiefer-Band)
        'nase': (60.0, 30.0, 240.0), 'bulbe': (1.12, 1.0, 1.45), 'kiefer_breit': (0.86, 0.80, 1.05), 'zahn_oben': (0.75, 0.5, 1.0)}
# Startlagen: Bildpunkt der Schnauzenspitze (Kamera Bild 2); None = Winkel von K3 (K3_fit/kopf.json)
STARTS = [None, (220.0, 300.0), (240.0, 370.0), (228.0, 335.0), (260.0, 300.0)]


def _radius_ansatz(V, ax):
    t = V @ ax
    r = np.linalg.norm(V - np.outer(t, ax), axis=1)
    m = (t > -100) & (t < KN.ANSATZ)
    return float(np.percentile(r[m], 90)) if m.sum() >= 4 else float(np.percentile(r, 50))


def geometrie(p, hals_V, hals_F, koerper=True):
    o22, o23 = KF.kopf(p), KF.kiefer(p)
    P22, F22, _, _ = o22.arrays()
    P23, F23, _, _ = o23.arrays()
    T22 = F.flaechen_tris(F22)
    T23 = F.flaechen_tris(F23)
    if koerper:
        ax = np.array([1.0, 0, 0])
        r22 = _radius_ansatz(P22, ax)
        tri = [KN.ellipsoid_dreiecke((0.0, 0.0, 0.0), ax, r22 * MUFFE[0], r22 * MUFFE[1])]
        r23 = _radius_ansatz(P23, ax)
        piv = np.array([500.0, 400.0, 0.0])
        d = piv - P22.mean(0)
        n = d / np.linalg.norm(d) if np.linalg.norm(d) >= 100 else ax
        tri.append(KN.ellipsoid_dreiecke(piv, n, r23 * MUNDWINKEL[0], r23 * MUNDWINKEL[1]))
        k = len(P22)
        extra = np.concatenate(tri).reshape(-1, 3)
        P22 = np.concatenate([P22, extra])
        T22 = np.concatenate([T22, k + np.arange(len(extra)).reshape(-1, 3)])
    return {21: (hals_V, hals_F), 22: (P22, T22), 23: (P23, T23)}


def masse_aus(x, frei):
    q = dict(KF.VORGABE_KROKO)
    for i, k in enumerate(frei):
        v0, lo, hi = FREI[k]
        q[k] = lo + (hi - lo) * 0.5 * (1 + math.tanh(x[i] + math.atanh(2 * (v0 - lo) / (hi - lo) - 1)))
    return q


def lauf(name, aus, iters=900, starts=5, frei=('L', 'Wk', 'taper', 'yt_s'), start_json=None):
    t0 = time.time()
    form, mo, fj = KK.lade()
    b2 = F.Buehne(2, form, mo, fj)
    b1 = F.Buehne(1, form, mo, fj)
    k3 = json.load(open(os.path.join(KK.K_DIR, 'K3_fit', 'kopf.json'), encoding='utf-8'))
    nf = len(frei)
    best = None
    protokoll = []

    liste = list(STARTS[:starts])
    if start_json:
        liste = [dict(l['winkel']) for l in json.load(open(start_json, encoding='utf-8'))['lagen'][:starts]]
    for si, ziel in enumerate(liste):
        p0 = masse_aus(np.zeros(nf), frei)
        if isinstance(ziel, dict):
            k21, k22, k23 = list(ziel['21']), list(ziel['22']), list(ziel['23'])
            ziel = 'Suche %d' % si
        elif ziel is None:
            k21, k22 = list(k3['bild2']['winkel']['21']), list(k3['bild2']['winkel']['22'])
            k23 = list(k3['bild2']['winkel']['23'])
        else:
            k21, k22 = F.ziel_kopf(b2, p0, None, 0, spitze=ziel)
            k23 = [0.0, 0.0, 300.0]

        def zerlegen(x):
            q = masse_aus(x[:nf], frei)
            y = x[nf:]
            w21 = [k21[i] + 200 * y[i] for i in range(3)]
            w22 = [k22[i] + 200 * y[3 + i] for i in range(3)]
            w23 = [0.0, 40 * math.tanh(y[7]), min(max(k23[2] + 200 * y[6], 0.0), 700.0)]
            return q, w21, w22, w23

        def f(x):
            q, w21, w22, w23 = zerlegen(x)
            _t, idb = b2.rendern(geometrie(q, b2.hals, b2.hals_f), w21, w22, w23)
            m = b2.mass(idb)
            sc = 1.5 * m['iou_ok'] + m['iou_uk'] + 10 * min(0.0, m['umriss'] - UMRISS_MIN) + 3 * min(0.0, m['iou_uk'] - UK_MIN)
            return -sc

        x0 = np.zeros(nf + 8)
        r = minimize(f, x0, method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-4))
        q, w21, w22, w23 = zerlegen(r.x)
        _t, idb = b2.rendern(geometrie(q, b2.hals, b2.hals_f), w21, w22, w23)
        m = b2.mass(idb)
        eintrag = dict(start=si, ziel=ziel, wert=round(-r.fun, 4), aufrufe=r.nfev, mass=m, form={k: round(q[k], 1) for k in frei},
                       winkel={21: [round(v) for v in w21], 22: [round(v) for v in w22], 23: [round(v) for v in w23]})
        protokoll.append(eintrag)
        print('  Start %d (Spitze %s): Wert %.4f, %s, %s, %.0f s' % (si, ziel, -r.fun, m, eintrag['form'], time.time() - t0), flush=True)
        if best is None or r.fun < best[0]:
            best = (r.fun, q, w21, w22, w23)
    _f, q, w21, w22, w23 = best
    W2 = [int(round(v)) for v in w21], [int(round(v)) for v in w22], [int(round(v)) for v in w23]
    geo = geometrie(q, b2.hals, b2.hals_f)
    _t, idb2 = b2.rendern(geo, *W2)
    m2 = b2.mass(idb2)
    _t, idb2o = b2.rendern(geometrie(q, b2.hals, b2.hals_f, koerper=False), *W2)
    m2o = b2.mass(idb2o)
    # Bild 1: wie k_kopf_fit (Umriss, leichte Bindung an die Kopflage von J), Gelenkkoerper mitgerechnet
    w1 = {b: list(b1.pool[b]) for b in F.KOPF}
    geo1 = geometrie(q, b1.hals, b1.hals_f)

    def zerl1(x):
        return ([w1[21][i] + 200 * x[i] for i in range(3)], [w1[22][i] + 200 * x[3 + i] for i in range(3)],
                [w1[23][0], w1[23][1], min(max(w1[23][2] + 200 * x[6], 0.0), 600.0)])

    def f1(x):
        _t, idb = b1.rendern(geo1, *zerl1(x))
        return -b1.mass(idb)['umriss'] + 0.002 * float(np.sum(np.asarray(x[:6]) ** 2))
    b1best = None
    for st in range(3):
        x0 = np.zeros(7) if st == 0 else np.random.RandomState(st).normal(0, 0.5, 7)
        r1 = minimize(f1, x0, method='Powell', options=dict(maxfev=500, xtol=1e-2, ftol=1e-5))
        if b1best is None or r1.fun < b1best.fun:
            b1best = r1
    W1 = [[int(round(v)) for v in w] for w in zerl1(b1best.x)]
    _t, idb1 = b1.rendern(geo1, *W1)
    m1 = b1.mass(idb1)
    Wk = b2.welt(*W2)
    auf = math.degrees(math.acos(np.clip(Wk[22][0][:, 0] @ Wk[23][0][:, 0], -1, 1)))
    erg = dict(name=name, form=q, bild2=dict(winkel={21: W2[0], 22: W2[1], 23: W2[2]}, mass=m2, mass_ohne_koerper=m2o),
               bild1=dict(winkel={21: W1[0], 22: W1[1], 23: W1[2]}, mass=m1), kiefer_offen_grad=round(auf, 1),
               starts=protokoll, grenzen=FREI, sekunden=round(time.time() - t0))
    os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
    json.dump(erg, open(aus, 'w', encoding='utf-8'), indent=1)
    np.save(os.path.splitext(aus)[0] + '_id2.npy', idb2)
    np.save(os.path.splitext(aus)[0] + '_id1.npy', idb1)
    print('%s fertig: Bild 2 %s (ohne Koerper %s) | Bild 1 %s | Kiefer offen %.1f Grad | %.0f s' % (name, m2, m2o, m1, auf, time.time() - t0))
    return erg


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--name', required=True)
    a.add_argument('--aus', required=True)
    a.add_argument('--iters', type=int, default=900)
    a.add_argument('--starts', type=int, default=5)
    a.add_argument('--frei', default='L,Wk,taper,yt_s,yt_k,yt_t')
    a.add_argument('--start-json', default=None, help='Startlagen aus k_kopf_suche.py')
    a.add_argument('--grenzen', default=None, help='JSON {name: [vorgabe, min, max]} ersetzt Eintraege in FREI (Referenzlauf)')
    x = a.parse_args()
    if x.grenzen:
        FREI.update({k: tuple(v) for k, v in json.loads(x.grenzen).items()})
    lauf(x.name, x.aus, x.iters, x.starts, tuple(k for k in x.frei.split(',') if k), x.start_json)
