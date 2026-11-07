#!/usr/bin/env python3
"""Variante K, Schritt 3a: Kopf-Pose (Hals 21, Kopf 22, Kiefer 23) und optional Kopfmasse an die Nutzer-Teile Oberkiefer/Unterkiefer in Bild 2 passen,
danach die Pose in Bild 1 so, dass der Umriss nicht schlechter wird als bei J. Rumpf, Glieder, Schwanz: Posen von J unveraendert.

    python k_kopf_fit.py --name K1 --form L=2400,Ws=330,Wt=250 [--frei L,Ws,Wt] --aus ../K1_lang/kopf.json

Messgroessen je Lauf: IoU Kopf (Knochen 22 sichtbar) gegen maske_oberkiefer, IoU Kiefer (23 sichtbar) gegen maske_unterkiefer, Umriss-IoU gegen ziel2 (nicht-grau),
alles mit dem Gleitkomma-Tiefenpuffer aus k_kern (Abnahme danach mit form_pruefung.py). Kein RE-Wert."""
import argparse
import json
import math
import os
import time

import cv2
import numpy as np
from scipy.optimize import minimize

import k_kern as KK
import k_form as KF

FE = KK.FE
KOPF = (21, 22, 23)
UMRISS_J = {1: 0.948, 2: 0.935}      # J_dicker/pruefung/bericht.txt (a)
SPANNE = {'L': 0.4}       # frei: Faktor 1 +- Spanne (Vorgabe 0,25)
WK_GE_WS = True


def mat(a):
    """mat_euler in Gleitkomma (EA.mat_euler, RotMatrix RE1.5): Winkel in 1/4096."""
    ax, ay, az = (x * 2 * math.pi / 4096.0 for x in a)
    sx, cx, sy, cy, sz, cz = math.sin(ax), math.cos(ax), math.sin(ay), math.cos(ay), math.sin(az), math.cos(az)
    return np.array([[cz * cy, -sz * cy, sy],
                     [sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx],
                     [sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx]])


def flaechen_tris(F):
    out = []
    for f in F:
        if len(f) == 3:
            out.append(f)
        else:
            out += [[f[0], f[1], f[2]], [f[0], f[2], f[3]]]
    return np.array(out, int)


class Buehne:
    """Bild n mit festem Koerper (alle Knochen ausser 21-23 in der Pose von J) und beweglichem Kopf."""

    def __init__(self, n, form, mo, fj, clip=None):
        self.n = n
        s = fj['bild%d' % n]
        self.kam = KK.kamera(n)
        self.clip = s['clip'] if clip is None else clip
        kf = KK.kf_von(mo, self.clip, s['bild'])
        self.kf = kf
        R, T = mo.pose(kf)
        self.Rw, self.Tw = KK.welt_pose(R, T, s['lage'], s['gier'])
        tris, ids = KK.tris_welt(form, self.Rw, self.Tw, knochen=[b for b in range(len(form['meshes'])) if b not in KOPF])
        self.tief, self.idb = KK.raster(self.kam, tris, ids)
        self.pool = form['pool'][kf]['winkel']
        self.off = [np.array(k['versatz'], float) for k in form['knochen']]
        self.hals = np.array(form['meshes'][21]['ecken'], float)
        self.hals_f = flaechen_tris([f['e'] if len(f['e']) == 3 else [f['e'][0], f['e'][1], f['e'][3], f['e'][2]]
                                     for f in form['meshes'][21]['dreiecke'] + form['meshes'][21]['vierecke']])
        z = cv2.imread(os.path.join(KK.FORM, 'ziel2_bild%d.png' % n), 0)
        self.weiss, self.grau = z == 255, z == 128
        if n == 2:
            self.ok, self.uk = KK.maske('oberkiefer'), KK.maske('unterkiefer')
            self.rest_face = KK.maske('face_and_upper_body') & ~self.ok & ~self.uk

    def welt(self, w21, w22, w23):
        R10, T10 = self.Rw[10], self.Tw[10]
        R21 = R10 @ mat(w21)
        T21 = R10 @ self.off[21] + T10
        R22 = R21 @ mat(w22)
        T22 = R21 @ self.off[22] + T21
        R23 = R22 @ mat(w23)
        T23 = R22 @ self.off[23] + T22
        return {21: (R21, T21), 22: (R22, T22), 23: (R23, T23)}

    def rendern(self, geo, w21, w22, w23):
        W = self.welt(w21, w22, w23)
        P, I = [], []
        for b, (V, F) in geo.items():
            R, T = W[b]
            v = V @ R.T + T
            P.append(v[F])
            I.append(np.full(len(F), b))
        return KK.raster(self.kam, np.concatenate(P), np.concatenate(I), self.tief.copy(), self.idb.copy())

    def mass(self, idb):
        fig = idb >= 0
        u = float((fig & self.weiss).sum()) / max(float(((fig | self.weiss) & ~self.grau).sum()), 1.0)
        d = dict(umriss=round(u, 4))
        if self.n == 2:
            d['iou_ok'] = round(KK.iou(idb == 22, self.ok), 4)
            g = ~self.rest_face
            d['iou_ok_ohne_rumpf'] = round(KK.iou((idb == 22) & g, self.ok & g), 4)
            d['iou_uk'] = round(KK.iou(idb == 23, self.uk), 4)
            d['kopf_px'] = int((idb == 22).sum())
            d['kiefer_px'] = int((idb == 23).sum())
        d['kopf_sichtbar_px'] = int(np.isin(idb, KOPF).sum())
        return d


def geometrie(p, hals_V, hals_F):
    o22, o23 = KF.kopf(p), KF.kiefer(p)
    P22, F22, _, _ = o22.arrays()
    P23, F23, _, _ = o23.arrays()
    return {21: (hals_V, hals_F), 22: (P22, flaechen_tris(F22)), 23: (P23, flaechen_tris(F23))}


def _auf_strahl(kam, px, mitte, abstand):
    """Punkt auf dem Sehstrahl durch px mit Abstand `abstand` von `mitte` (naeherer Schnitt)."""
    auge, d = KK.FK.strahl(kam, np.array(px, float))
    b = 2 * d @ (auge - mitte)
    c = (auge - mitte) @ (auge - mitte) - abstand ** 2
    disk = b * b - 4 * c
    if disk < 0:
        s = -b / 2
    else:
        s = (-b - math.sqrt(disk)) / 2
    return auge + s * d


def _rahmen(u):
    v0 = np.array([0, 1.0, 0])
    v = KF._einheit(v0 - (v0 @ u) * u)
    return np.stack([u, v, np.cross(u, v)], 1)


def ziel_kopf(bue, p, w0, kiefer_auf, spitze=(220.0, 290.0), basis=(338.0, 300.0)):
    """Anfangslage nach der Lesung "Kopf zur Frau gedreht, Maul offen": Hals zeigt zum Bildpunkt `basis`, Kopfachse zum Bildpunkt `spitze`
    (Abstand L - 300 vom Kopfursprung), unten = Welt unten. Rueckgabe Engine-Winkel Hals 21 und Kopf 22."""
    R10, T10 = bue.Rw[10], bue.Tw[10]
    T21 = R10 @ bue.off[21] + T10
    B = _auf_strahl(bue.kam, basis, T21, 503.0)
    R21 = _rahmen(KF._einheit(B - T21))
    w21 = list(FE.winkel_aus_matrix(R10.T @ R21))
    R21 = R10 @ mat(w21)
    T22 = R21 @ bue.off[22] + T21
    S = _auf_strahl(bue.kam, spitze, T22, p['L'] - 300.0)
    R22 = _rahmen(KF._einheit(S - T22))
    return w21, list(FE.winkel_aus_matrix(R21.T @ R22))


def lauf(name, form_p, frei, aus, iters=400, starts=3):
    t0 = time.time()
    form, mo, fj = KK.lade()
    b2 = Buehne(2, form, mo, fj)
    b1 = Buehne(1, form, mo, fj)
    p = dict(KF.VORGABE)
    p.update(form_p)
    w0 = {b: list(b2.pool[b]) for b in KOPF}
    print('%s: J-Winkel Bild 2 %s' % (name, w0))
    frei = [f for f in frei if f]

    def zerlegen(x):
        q = dict(p)
        for i, k in enumerate(frei):
            q[k] = p[k] * (1 + SPANNE.get(k, 0.25) * math.tanh(x[8 + i]))
        if WK_GE_WS:
            q['Wk'] = max(q['Wk'], q['Ws'])      # Schaedel mindestens so breit wie der Schnauzenansatz (Krokodil)
        w21 = [k21[0] + 200 * x[0], k21[1] + 200 * x[1], k21[2] + 200 * x[2]]
        w22 = [x[3] * 200 + k22[0], x[4] * 200 + k22[1], x[5] * 200 + k22[2]]
        w23 = [0.0, 40 * math.tanh(x[7]), 550 + 200 * x[6]]      # Kiefer: nur Oeffnen (az, +X dreht nach +Y), Start 550 = 48 Grad; Seitendrehung hoechstens 40 = 3,5 Grad
        return q, w21, w22, w23

    k21, k22 = ziel_kopf(b2, p, w0, 0)
    best = None
    hist = []

    def f(x):
        q, w21, w22, w23 = zerlegen(x)
        geo = geometrie(q, b2.hals, b2.hals_f)
        _t, idb = b2.rendern(geo, w21, w22, w23)
        m = b2.mass(idb)
        sc = m['iou_ok'] + m['iou_uk'] + 10 * min(0.0, m['umriss'] - UMRISS_J[2])
        hist.append(sc)
        return -sc

    rng = np.random.RandomState(11)
    for st in range(starts):
        x0 = np.zeros(8 + len(frei))
        if st:
            x0[:6] = rng.normal(0, 0.6, 6)
            x0[6] = rng.normal(0, 0.5)
        r = minimize(f, x0, method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-4))
        print('  Start %d: Wert %.4f nach %d Aufrufen, %.0f s' % (st, -r.fun, r.nfev, time.time() - t0), flush=True)
        if best is None or r.fun < best.fun:
            best = r
    q, w21, w22, w23 = zerlegen(best.x)
    W2 = [int(round(v)) for v in w21], [int(round(v)) for v in w22], [int(round(v)) for v in w23]
    geo = geometrie(q, b2.hals, b2.hals_f)
    _t, idb2 = b2.rendern(geo, *W2)
    m2 = b2.mass(idb2)
    # Bild 1: Kopf so legen, dass der Umriss am besten passt (Hals, Kopf, Kiefer), Start = Winkel von J
    w1 = {b: list(b1.pool[b]) for b in KOPF}

    def f1(x):
        a21 = [w1[21][i] + 200 * x[i] for i in range(3)]
        a22 = [w1[22][i] + 200 * x[3 + i] for i in range(3)]
        a23 = [w1[23][0], w1[23][1], min(max(w1[23][2] + 200 * x[6], 0.0), 600.0)]      # Kiefer zu bis 53 Grad offen, nie in den Schaedel
        _t, idb = b1.rendern(geo, a21, a22, a23)
        mm = b1.mass(idb)
        return -mm['umriss'] + 0.002 * float(np.sum(np.asarray(x[:6]) ** 2))      # leichte Bindung an die Kopflage von J in Bild 1
    r1 = minimize(f1, np.zeros(7), method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-5))
    x = r1.x
    W1 = ([int(round(w1[21][i] + 200 * x[i])) for i in range(3)], [int(round(w1[22][i] + 200 * x[3 + i])) for i in range(3)],
          [int(w1[23][0]), int(w1[23][1]), int(round(min(max(w1[23][2] + 200 * x[6], 0.0), 600.0)))])
    _t, idb1 = b1.rendern(geo, *W1)
    m1 = b1.mass(idb1)
    # Kiefer-Oeffnung in 3D: Winkel zwischen Kopfachse und Kiefer-Achse
    Wk = b2.welt(*W2)
    auf = math.degrees(math.acos(np.clip(Wk[22][0][:, 0] @ Wk[23][0][:, 0], -1, 1)))
    erg = dict(name=name, form=q, bild2=dict(winkel={21: W2[0], 22: W2[1], 23: W2[2]}, mass=m2), bild1=dict(winkel={21: W1[0], 22: W1[1], 23: W1[2]}, mass=m1),
               kiefer_offen_grad=round(auf, 1), j_winkel=dict(bild2={b: list(b2.pool[b]) for b in KOPF}, bild1={b: list(b1.pool[b]) for b in KOPF}),
               aufrufe=len(hist), sekunden=round(time.time() - t0))
    os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
    json.dump(erg, open(aus, 'w', encoding='utf-8'), indent=1)
    np.save(os.path.splitext(aus)[0] + '_id2.npy', idb2)
    np.save(os.path.splitext(aus)[0] + '_id1.npy', idb1)
    print('%s fertig: Bild 2 %s | Bild 1 %s | Kiefer offen %.1f Grad | %.0f s' % (name, m2, m1, auf, time.time() - t0))
    return erg


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--name', required=True)
    a.add_argument('--form', default='')
    a.add_argument('--frei', default='')
    a.add_argument('--aus', required=True)
    a.add_argument('--iters', type=int, default=400)
    a.add_argument('--starts', type=int, default=3)
    x = a.parse_args()
    fp = {}
    for kv in x.form.split(','):
        if kv:
            k, v = kv.split('=')
            fp[k] = float(v)
    lauf(x.name, fp, x.frei.split(','), x.aus, x.iters, x.starts)
