#!/usr/bin/env python3
"""Variante K5 (FORMBLATT_V2), Geometrie: Krokodilkopf fuer die FRONTALE Lage in Bild 2 (Augen oben sichtbar, Schnauze nach unten/zur Kamera,
Maul offen nach rechts (W) = zur Frau). Baut auf k_form (K4, Kopfart 'kroko') auf; neu gegenueber K4:
 - Augenabstand S frei und von der Schaedelbreite getrennt (K4: Augen bei +-0,52 Wk, Abstand 397; Formblatt 2.4: 560-770),
 - echte Augen: kleine Augaepfel (eigene Atlas-Stelle 'auge', dunkles Rotviolett, AE10) auf erhabenen Augenhoeckern (Haut),
 - Kiefergelenk (Knochen 23) frei waehlbar (gx, gy) statt fest (500, 400),
 - 6 helle Unterkieferzaehne je Seite (AE7: 5-6 Zahnflecke am Band), 11 obere wie K4,
 - Landmarken im Knochenraum (Augen, OK-Spitze, UK-Vorderende, Maulwinkel links (W)) fuer die Messung R1-R7.
Knochenraum wie k_form: +X vorn laengs, +Y unten, +Z = links (W) (Knochen 11 = linker Arm liegt bei +Z, form.json 'left_arm_front').
Kein RE-Wert: Gestaltung eines neuen Modells nach Videostandbild, Nutzerlinien und Nutzer-Teilen (PORT-WAHL)."""
import math
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.normpath(os.path.join(HIER, '..', '..', 'K_alligator', 'generator')))
import k_form as KF  # noqa: E402

ATLAS = KF.ATLAS

# Vorgabe (Einheiten, halbe Breiten). S = Augenabstand Mitte-Mitte.
VORGABE = dict(art='k5', S=640.0, x_auge=560.0, Wk=520.0, L=1500.0, taper=0.42, x_hinten=-250.0, yt_k=-330.0, yt_s=-200.0, yt_t=-80.0,
               y_gaumen=270.0, nase=80.0, bulbe=1.15, zaehne=11, zahn_lang=170.0, zahn_oben=0.55, uk_zaehne=6, uk_zahn_lang=200.0,
               kiefer_breit=0.92, kiefer_boden=380.0, kiefer_boden_t=120.0, kiefer_tief=110.0, kiefer_kuerzer=-30.0,
               gx=430.0, gy=420.0, auge_r=0.085, hoecker=0.17, hoecker_hoch=0.55)


def _ring(w, yt, yb):
    return KF.ring_kroko(w, yt, yb)


def stationen(p):
    """Oberkopf: x, halbe Breite, Oberkante, Unterkante. Schaedel breit bis hinter die Augen (Kiefergelenke liegen dort aussen), Schnauze ab
    x_auge + 0,35 S linear auf taper*Wk, Nasenrosette (bulbe) kurz vor der Spitze. Wie k_form.kroko_stationen, nur mit eigener Augenlage."""
    L, Wk, xa, S = p['L'], p['Wk'], p['x_auge'], p['S']
    yb = p['y_gaumen']
    Wt = p['taper'] * Wk
    xs0 = xa + 0.35 * S
    xh = p['x_hinten']
    sn = lambda t: xs0 + t * (L - 140.0 - xs0)
    xs = [xh, xh + 0.45 * (xa - xh), xa, xs0, sn(0.33), sn(0.66), sn(0.86), L - 140.0, L - 40.0, L]
    ws = [0.82 * Wk, 0.98 * Wk, Wk, 0.93 * Wk]
    for t in (0.33, 0.66, 0.86):
        ws.append(0.93 * Wk + t * (Wt - 0.93 * Wk))
    bu = p['bulbe']
    ws += [Wt * bu, Wt * 0.85 * bu, Wt * 0.5]
    yk, ys_, yt_ = p['yt_k'], p['yt_s'], p['yt_t']
    yts = [yk * 0.80, yk, yk * 0.97, ys_]
    for t, h in ((0.33, 0.0), (0.66, 0.0), (0.86, 0.35)):
        yts.append(ys_ + t * (yt_ - ys_) - h * p['nase'])
    yts += [yt_ - p['nase'], yt_ - 0.75 * p['nase'], yt_ * 0.45 - 0.3 * p['nase']]
    ybs = [yb - 60, yb, yb, yb, yb, yb, yb, yb, yb - 15, yb - 50]
    return xs, ws, yts, ybs


def _oberkante(p, x, z):
    """Hoehe der Oberflaeche (y, negativ = oben) des Oberkopfs an (x, z), aus den Stationen und ring_kroko (flache Oberseite bis 0,62 w)."""
    xs, ws, yts, ybs = stationen(p)
    w = float(np.interp(x, xs, ws))
    yt = float(np.interp(x, xs, yts))
    yb = float(np.interp(x, xs, ybs))
    a = abs(z) / max(w, 1.0)
    if a <= 0.62:
        return yt
    h = yb - yt
    # Flanke von (0,62 w, yt) nach (0,93 w, yb - 0,42 h)
    t = min((a - 0.62) / 0.31, 1.0)
    return yt + t * (h - 0.42 * h)


def ellipsoid(mitte, rx, ry, rz, rect, U=8, V=5):
    """Ellipsoid als Rohr (Pole auf der x-Achse), Texel im Rechteck rect."""
    mitte = np.asarray(mitte, float)
    Z, EA, EB, R = [], [], [], []
    for i in range(1, V):
        th = math.pi * i / V
        Z.append(mitte + np.array([-rx * math.cos(th), 0.0, 0.0]))
        EA.append([0, 0, 1.0])
        EB.append([0, 1.0, 0])
        R.append([(rz * math.sin(th) * math.cos(2 * math.pi * j / U), ry * math.sin(th) * math.sin(2 * math.pi * j / U)) for j in range(U)])
    return KF.rohr(Z, EA, EB, R, rect, spitze0=mitte + np.array([-rx, 0, 0]), spitze1=mitte + np.array([rx, 0, 0]))


def augen_lage(p):
    """Mittelpunkte der Augaepfel (Knochenraum 22): auf dem Augenhoecker oben, Abstand S. +Z = links (W)."""
    S = p['S']
    r = p['auge_r'] * S
    out = {}
    for nm, vz in (('auge_l', 1.0), ('auge_r', -1.0)):
        z = vz * S / 2
        y0 = _oberkante(p, p['x_auge'], z)
        hh = p['hoecker_hoch'] * p['hoecker'] * S
        out[nm] = np.array([p['x_auge'] + 0.25 * r, y0 - hh - 0.15 * r, z])
    return out


def kopf(p):
    """Knochen 22: Schaedel + Oberkiefer (ring_kroko), Augenhoecker (Haut), Augaepfel (Atlas 'auge'), obere Zaehne. Rueckgabe Netz."""
    xs, ws, yts, ybs = stationen(p)
    L = p['L']
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([_ring(w, a, b) for w, a, b in zip(ws, yts, ybs)])
    o = KF.rohr(Z, ea, eb, R, ATLAS['kopf'], spitze0=(xs[0] - 120.0, (yts[0] + ybs[0]) / 2, 0.0),
                spitze1=(L + 40.0, (yts[-1] + ybs[-1]) / 2 + 30, 0.0), bogen=True)
    S = p['S']
    hr = p['hoecker'] * S
    hh = p['hoecker_hoch'] * hr
    hoecker_rect = (ATLAS['kopf'][0], ATLAS['kopf'][1] + 4, 40, 30)      # Haut der Kopfseite (gleiche Seite, gleiche Zeile)
    for nm, c in augen_lage(p).items():
        z = c[2]
        y0 = _oberkante(p, p['x_auge'], z)
        # Augenhoecker: flaches Ellipsoid, steckt zur Haelfte im Schaedel (Haut), laengs x gestreckt (Krokodil: Augen in erhabenen Hoeckern)
        o.dazu(ellipsoid((p['x_auge'] - 0.2 * hr, y0 - 0.15 * hh, z), 1.5 * hr, hh + 0.3 * hr, 1.05 * hr, hoecker_rect))
        r = p['auge_r'] * S
        o.dazu(ellipsoid(c, 1.15 * r, r, r, ATLAS['auge'], U=8, V=4))
    n = int(p['zaehne'])
    x0, x1 = p['x_auge'] + 120.0, L - 120.0
    for vz in (-1, 1):
        for i in range(n):
            t = (i + 0.5) / n
            x = x0 + t * (x1 - x0)
            w = np.interp(x, xs, ws)
            gross = i in (2, 3, n - 3) or (i % 3 == 0)
            lang = p['zahn_lang'] * p['zahn_oben'] * (1.3 if gross else 0.8)
            o.dazu(KF.zahn((x, p['y_gaumen'] - 30, vz * 0.93 * w), (0.12, 1, vz * 0.22), lang, 40.0 if gross else 32.0, ATLAS['zahn']))
    return o


def kiefer_stationen(p):
    """Unterkiefer im Raum von Knochen 23 (Ursprung = Gelenk (gx, gy) im Kopfraum). Rand geschlossen 15 unter dem Gaumen, Breite kiefer_breit x
    Breite des Oberkiefers an derselben Stelle (Oberkiefer greift seitlich ueber), Boden tief hinten, flach vorn (wie k_form.kiefer_kroko)."""
    L, gx, gy = p['L'], p['gx'], p['gy']
    yr = p['y_gaumen'] - gy + 15.0
    Lj = L - gx - p['kiefer_kuerzer']
    xo, wo, _yt, _yb = stationen(p)
    breite = lambda xk: p['kiefer_breit'] * float(np.interp(xk + gx, xo, wo))
    xs = [-330.0, -120.0, 150.0, 150 + 0.33 * (Lj - 150), 150 + 0.66 * (Lj - 150), Lj - 220, Lj - 80, Lj]
    ws = [0.70 * breite(-120.0), breite(-120.0), breite(150.0)] + [breite(x) for x in xs[3:7]] + [0.55 * breite(Lj - 80)]
    bt = lambda t: yr + p['kiefer_boden'] + t * (p['kiefer_boden_t'] - p['kiefer_boden'])
    ybs = [yr + p['kiefer_boden'] * 0.9, bt(0.0), bt(0.1), bt(0.4), bt(0.7), bt(0.9), bt(1.0), yr + 0.5 * p['kiefer_boden_t']]
    tf = [40.0, p['kiefer_tief'], p['kiefer_tief'], p['kiefer_tief'], 0.8 * p['kiefer_tief'], 0.6 * p['kiefer_tief'], 0.4 * p['kiefer_tief'], 20.0]
    return xs, ws, ybs, tf, yr, Lj


def kiefer(p):
    xs, ws, ybs, tf, yr, Lj = kiefer_stationen(p)
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([KF.ring_kiefer(w, yr, b, t) for w, b, t in zip(ws, ybs, tf)])
    o = KF.rohr(Z, ea, eb, R, ATLAS['kiefer'], spitze0=(-400.0, yr + 120, 0.0), spitze1=(Lj + 40.0, yr + 30, 0.0), bogen=True)
    n = int(p['uk_zaehne'])
    for vz in (-1, 1):
        for i in range(n):
            t = (i + 0.5) / n
            x = 260 + t * (Lj - 140 - 260)
            w = np.interp(x, xs, ws)
            lang = p['uk_zahn_lang'] * (1.0 if i % 2 == 0 else 0.8)
            o.dazu(KF.zahn((x, yr + 15, vz * 0.88 * w), (0.1, -1, vz * 0.18), lang, 46.0, ATLAS['zahn']))
    return o


def landmarken(p):
    """Landmarken im Knochenraum: Kopf (22) und Kiefer (23). Maulwinkel links (W) = oberer Kieferrand an Station 2 (x 150) aussen bei +Z
    (dort treffen Ober- und Unterlippe zusammen, hinten am Maulspalt)."""
    xs, ws, yts, ybs = stationen(p)
    au = augen_lage(p)
    kx, kw, kyb, _tf, yr, Lj = kiefer_stationen(p)
    return {22: dict(auge_l=au['auge_l'].tolist(), auge_r=au['auge_r'].tolist(), ok_spitze=[p['L'] + 40.0, (yts[-1] + ybs[-1]) / 2 + 30, 0.0],
                     augenmitte=((au['auge_l'] + au['auge_r']) / 2).tolist(), gaumen_hinten=[p['gx'], p['y_gaumen'], 0.0]),
            23: dict(uk_spitze=[Lj + 40.0, yr + 30, 0.0], maulwinkel_l=[kx[2], yr, 0.96 * kw[2]], maulwinkel_r=[kx[2], yr, -0.96 * kw[2]])}


def schnauze_e(p):
    """3D-Schnauzenlaenge Augenmitte -> OK-Spitze in Augenabstaenden (F4)."""
    lm = landmarken(p)[22]
    return float(np.linalg.norm(np.array(lm['ok_spitze']) - np.array(lm['augenmitte'])) / p['S'])
