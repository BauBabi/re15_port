#!/usr/bin/env python3
"""Variante I (E16): Unterschenkel und Fuesse neu. Kein RE-Wert: neues Modell nach den Nutzerteilen bein_links, fuss_links, bein_rechts, fuss_rechts
(nutzer_teile/, Bild 1). Knochen bleiben (Fuss hat schon eigene Knochen 25/27, Zehen haengen daran); Oberschenkel 8/9 bleiben in Form und Pose.

Formen im Knochenraum (Y unten):
 - Unterschenkel (Achse +Y vom Knie zum Knoechel, 829 lang = Versatz des Fussknochens): ovaler Schaft, Knie r_k, Wade, Knoechel r_a, Verhaeltnis e.
 - Fuss (Ursprung Knoechel, +X nach vorn, Sohle bei y = H): kantiger Block aus Ringen laengs X (Ferse hl hinter dem Knoechel, Laenge L), Querschnitt
   gerundetes Rechteck (Breite w(x), Oberkante y_oben(x), Sohle flach mit Ferse- und Ballenpolster), vorn drei kurze Zehen (Kegel, Kralle).
Pose 1 (Bild 1): Nutzer-Nachtrag "Fuesse quasi von unten, man schaut frontal auf die Unterseite": Gelenkwinkel von Unterschenkel und Fuss und die Masse
so angepasst, dass die Silhouetten die Masken treffen (Unterschenkel: Maske bein_* ohne die Flaeche des Oberschenkels; Fuss: fuss_*) und die Sohle
zur Kamera zeigt (Kosinus Sohlennormale/Blick zur Kamera >= 0,7). Gemessen mit form_kamera (Gleitkomma) und gefuellten Dreiecken.
Pose 2 und Ruhe: Unterschenkel wie bisher, Fuss flach (Sohlennormale = Welt unten), Gier wie bisher. python vm_beine.py fit -> zw/beine.json."""
import json
import math
import os
import sys

import cv2
import numpy as np
from scipy.optimize import minimize

from rf_kern import FORM, HIER
import form_emd as FE
import form_kamera as FK
import form_pruefung as P
import rf_textur as RT

M8 = 8
PHI = 2 * np.pi * np.arange(M8) / M8
SEITEN = {'l': dict(ober=8, unter=24, fuss=25, bein='bein_links', fusst='fuss_links'),
          'r': dict(ober=9, unter=26, fuss=27, bein='bein_rechts', fusst='fuss_rechts')}
LAENGE_U = 829.0


def euler(a):
    ax, ay, az = [x * 2 * math.pi / 4096 for x in a]
    Rx = np.array([[1, 0, 0], [0, math.cos(ax), -math.sin(ax)], [0, math.sin(ax), math.cos(ax)]])
    Ry = np.array([[math.cos(ay), 0, math.sin(ay)], [0, 1, 0], [-math.sin(ay), 0, math.cos(ay)]])
    Rz = np.array([[math.cos(az), -math.sin(az), 0], [math.sin(az), math.cos(az), 0], [0, 0, 1]])
    return Rx @ Ry @ Rz


# ═══════════════════════════════════════════════════ Formen
def unterschenkel_ringe(s):
    """-> Liste von Ringen [8,3] (Knochenraum), k0, k1."""
    rk, ra, e = s['rk'], s['ra'], s['e']
    stat = [(-40.0, rk * 1.02), (160.0, rk * 1.04), (360.0, 0.5 * (rk + ra) * 1.10), (600.0, ra * 1.05), (LAENGE_U + 30.0, ra * 0.95)]
    ringe = []
    for t, r in stat:
        ringe.append(np.stack([e * r * np.cos(PHI), np.full(M8, t), r * np.sin(PHI)], 1))
    return ringe, np.array([0.0, -40.0 - rk * 0.6, 0.0]), np.array([0.0, LAENGE_U + 30.0 + ra * 0.5, 0.0])


def fuss_querschnitt(w, y0, y1, polster=0.0):
    """Gerundetes Rechteck in (z, y): Ecken in Umlaufrichtung wie PHI (Winkel um die X-Achse, z = cos, y = sin). Sohle bei y1 (unten)."""
    h = y1 - y0
    pts = []
    for ph in PHI:
        c, s_ = math.cos(ph), math.sin(ph)
        # Superellipse-artig: nahe am Rechteck
        z = math.copysign(abs(c) ** 0.35, c) * w / 2
        yy = math.copysign(abs(s_) ** 0.35, s_)
        y = y0 + h * (0.5 + 0.5 * yy)
        if s_ > 0.5:
            y += polster
        pts.append((z, y))
    return pts


def fuss_ringe(s):
    W, H, L, hl = s['W'], s['H'], s['L'], s['hl']
    # x, Breitenfaktor, Oberkante (Anteil der Hoehe ueber der Sohle), Polster
    stat = [(-hl, 0.72, 0.55, 18.0), (-0.45 * hl, 0.88, 0.85, 22.0), (0.15 * L, 1.0, 1.0, 0.0), (0.45 * L, 1.08, 0.62, 6.0),
            (0.72 * L, 1.15, 0.42, 20.0), (L, 1.02, 0.30, 4.0)]
    ringe = []
    for x, fw, fo, po in stat:
        y1 = H
        y0 = H - fo * H
        q = fuss_querschnitt(W * fw, y0, y1, po)
        ringe.append(np.array([[x, y, z] for z, y in q]))
    k0 = np.array([-hl - 70.0, H * 0.72, 0.0])
    k1 = np.array([L + 40.0, H * 0.85, 0.0])
    return ringe, k0, k1


def zehen(s):
    """Drei Zehen: (Ringe, k0, k1, Richtung) je Zeh, Knochenraum des Fusses."""
    W, H, L = s['W'], s['H'], s['L']
    aus = []
    for zf, dz in ((-0.32, -0.25), (0.0, 0.0), (0.32, 0.25)):
        r = 0.15 * W
        o = np.array([L - 40.0, H - r * 0.9, zf * W])
        d = np.array([1.0, 0.12, dz])
        d /= np.linalg.norm(d)
        ea = np.array([0.0, 1.0, 0.0]) - d[1] * d
        ea /= np.linalg.norm(ea)
        eb = np.cross(d, ea)
        tl = 0.28 * L
        ringe = [o + rr * (np.cos(PHI[::2])[:, None] * ea + np.sin(PHI[::2])[:, None] * eb) + t * d for t, rr in ((0.0, r), (tl * 0.55, r * 0.8))]
        aus.append((ringe, o - d * r, o + d * (tl + 0.45 * L * 0.3), d))
    return aus


# ═══════════════════════════════════════════════════ Projektion fuer die Anpassung
def dreiecke_aus_ringen(ringe, k0, k1):
    P_ = np.concatenate(list(ringe) + [k0[None], k1[None]])
    m = len(ringe[0])
    T = len(ringe)
    tri = []
    for a in range(T - 1):
        for j in range(m):
            i0, i1, i2, i3 = a * m + j, a * m + (j + 1) % m, (a + 1) * m + (j + 1) % m, (a + 1) * m + j
            tri += [(i0, i1, i2), (i0, i2, i3)]
    c0, c1 = T * m, T * m + 1
    for j in range(m):
        tri.append((c0, j, (j + 1) % m))
        tri.append((c1, (T - 1) * m + j, (T - 1) * m + (j + 1) % m))
    return P_, np.array(tri)


class Szene:
    def __init__(self, form_json):
        self.k = P.Kandidat(form_json)
        self.form = FE.lese_emd(self.k.emd)
        st = json.load(open(os.path.join(HIER, 'stand_final.json')))
        p = st['posen']['1']
        self.Rw, self.Tw = RT.pose_matrizen(self.form, 0, round(p['gier']), [round(p['lage'][0]), 0, round(p['lage'][2])])
        self.kam = FK.lade_kamera(1)
        self.auge = np.asarray(self.kam['auge'], float)
        self.versatz = {b: np.array(self.form['knochen'][b]['versatz'], float) for b in range(len(self.form['knochen']))}
        self.ziel = (P.ziel_laden(1)[1] == 255)
        self.schwarz = (P.ziel_laden(1)[1] == 0)
        self.masken = {nm: P.teil_roh(nm) for nm in ('bein_links', 'fuss_links', 'bein_rechts', 'fuss_rechts')}
        self.ober = {}
        for sd, d in SEITEN.items():
            self.ober[sd] = self.silhouette_knochen(d['ober'])

    def silhouette_knochen(self, b):
        import form_buehne as FB
        import vm_matrix as VM
        lage, gier, clip, bild = self.k.stand(1)
        kam = P.kamera(1)
        mo = VM.nur_knochen(self.k.mo, {b})
        a = np.zeros((480, 640, 3), np.uint8)
        return FB.rastern(FB.dreiecke(mo, self.k.kf(clip, bild), lage, gier, kam, None), self.k.tex, a, kam.abbildung)

    def welt(self, sd, a_u, a_f):
        d = SEITEN[sd]
        Ro, To = self.Rw[d['ober']].astype(float), self.Tw[d['ober']].astype(float)
        Ru = Ro @ euler(a_u)
        Tu = Ro @ self.versatz[d['unter']] + To
        Rf = Ru @ euler(a_f)
        Tf = Ru @ self.versatz[d['fuss']] + Tu
        return (Ru, Tu), (Rf, Tf)

    def raster(self, Pw, tri):
        q = FK.projiziere(self.kam, Pw)
        m = np.zeros((480, 640), np.uint8)
        pts = np.round(q * 4).astype(np.int32)
        for t in tri:
            cv2.fillConvexPoly(m, pts[list(t)], 1, shift=2)
        return m > 0


def parameter_zu(sd, x):
    a_u, a_f = x[0:3], x[3:6]
    s = dict(rk=x[6], ra=x[7], e=1.15, W=x[8], H=x[9], L=x[10], hl=x[11])
    return a_u, a_f, s


def bewerten(sz, sd, x, ausgabe=False):
    a_u, a_f, s = parameter_zu(sd, x)
    (Ru, Tu), (Rf, Tf) = sz.welt(sd, a_u, a_f)
    r, k0, k1 = unterschenkel_ringe(s)
    Pu, tu = dreiecke_aus_ringen(r, k0, k1)
    mu = sz.raster(Pu @ Ru.T + Tu, tu)
    rf, f0, f1 = fuss_ringe(s)
    Pf, tf = dreiecke_aus_ringen(rf, f0, f1)
    mf = sz.raster(Pf @ Rf.T + Tf, tf)
    for zr, z0, z1, _d in zehen(s):
        Pz, tz = dreiecke_aus_ringen(zr, z0, z1)
        mf |= sz.raster(Pz @ Rf.T + Tf, tz)
    d = SEITEN[sd]
    ober = sz.ober[sd]
    zb = sz.masken[d['bein']] & ~ober
    zf = sz.masken[d['fusst']]
    iou = lambda a, b: float((a & b).sum()) / max(float((a | b).sum()), 1.0)   # noqa: E731
    # Bein wie die Teilepruefung: Ober- und Unterschenkel zusammen (ohne Fuss) gegen die ganze Beinmaske ohne Fuss
    zb = sz.masken[d['bein']]
    i_u = iou((mu | ober) & ~mf, zb & ~zf)
    i_f = iou(mf, zf)
    schwarz = float(((mu | mf) & sz.schwarz).sum()) / max(float((mu | mf).sum()), 1.0)
    n = Rf @ np.array([0.0, 1.0, 0.0])
    c = Tf + Rf @ np.array([0.4 * s['L'], s['H'], 0.0])
    zur = sz.auge - c
    cosw = float(n @ zur / np.linalg.norm(zur))
    strafe = 2.0 * max(0.0, 0.7 - cosw) + 0.002 * max(0.0, abs(s['L'] / s['W']) - 3.0)
    L_ = (1 - i_u) + 1.3 * (1 - i_f) + 1.5 * schwarz + strafe
    if ausgabe:
        return dict(iou_unter=round(i_u, 3), iou_fuss=round(i_f, 3), schwarz=round(schwarz, 3), sohle_cos=round(cosw, 3), verlust=round(L_, 4)), mu, mf
    return L_


BODEN_STRAFE = 0.0     # Pose 1: Fuss darf unter die Bodenebene (Hintergrund ist ein Bild, nichts wird verdeckt); mit 0,004 fiel fuss_links 0,86 -> 0,65
GRENZEN_MASS = [(120, 260), (70, 180), (180, 420), (200, 450), (260, 700), (80, 200)]   # L ab 260: rechter Fuss (naeher an der Kamera) war mit 380 15 px zu lang     # rk, ra, W, H, L, hl: anatomische Spanne, frei gesetzt


def anpassen(sz, sd, start, runden=3):
    """Powell mit Grenzen fuer die Masse (Winkel frei), mehrere Starts mit gedrehtem Fuss (Sohle zur Kamera ist nicht nahe am Start)."""
    grenzen = [(None, None)] * 6 + GRENZEN_MASS
    starts = []
    for dux in (0, 400, -400):
        for dfx in (0, 512, -512, 1024):
            for dfy in (0, 1024, -1024):
                x = np.array(start, float)
                x[0] += dux
                x[3] += dfx
                x[4] += dfy
                starts.append(x)
    best, bl = None, 1e9
    for x0 in starts:
        res = minimize(lambda x: bewerten(sz, sd, x), x0, method='Powell', bounds=grenzen, options=dict(maxiter=3000, xtol=1.0, ftol=1e-4))
        if res.fun < bl:
            best, bl = res.x, res.fun
    for r in range(runden):
        res = minimize(lambda x: bewerten(sz, sd, x), best, method='Powell', bounds=grenzen, options=dict(maxiter=4000, xtol=0.5, ftol=1e-5))
        if res.fun < bl:
            best, bl = res.x, res.fun
    return best, bl


def fit():
    st = json.load(open(os.path.join(HIER, 'stand_final.json')))
    w1 = st['posen']['1']['winkel']
    sz = Szene(os.path.join(HIER, '..', 'H_stoff', 'h2', 'form.json'))
    aus = {}
    for sd, d in SEITEN.items():
        start = list(w1[d['unter']]) + list(w1[d['fuss']]) + [190, 150, 330, 260, 620, 150]
        x, L_ = anpassen(sz, sd, start)
        info, mu, mf = bewerten(sz, sd, x, ausgabe=True)
        a_u, a_f, s = parameter_zu(sd, x)
        aus[sd] = dict(winkel_unter=[round(float(v), 1) for v in a_u], winkel_fuss=[round(float(v), 1) for v in a_f],
                       mass={k: round(float(v), 1) for k, v in s.items()}, **info)
        print(sd, json.dumps(aus[sd]))
    json.dump(aus, open(os.path.join(HIER, 'zw', 'beine.json'), 'w'), indent=1)
    return aus


if __name__ == '__main__':
    if sys.argv[1:] == ['fit']:
        fit()


# ═══════════════════════════════════════════════════ Zwei Stufen (E16, Lauf 3: gemeinsam blieb die Anpassung in Nebenminima, Fuesse 0,57-0,72)
def _iou(a, b):
    return float((a & b).sum()) / max(float((a | b).sum()), 1.0)


def stufe_bein(sz, sd, a_u0, s0):
    d = SEITEN[sd]
    zb, zf, ober = sz.masken[d['bein']], sz.masken[d['fusst']], sz.ober[sd]
    ys, xs = np.nonzero(zf)
    ziel_kn = np.array([xs.mean(), ys.min() + 4.0])          # Knoechel/Ferse: Oberkante der Fussmaske

    def f(x):
        s = dict(s0, rk=x[3], ra=x[4])
        (Ru, Tu), (Rf, Tf) = sz.welt(sd, x[:3], [0, 0, 0])
        r, k0, k1 = unterschenkel_ringe(s)
        Pu, tu = dreiecke_aus_ringen(r, k0, k1)
        mu = sz.raster(Pu @ Ru.T + Tu, tu)
        kn = FK.projiziere(sz.kam, Tf)
        dist = float(np.hypot(*(kn - ziel_kn))) if np.all(np.isfinite(kn)) else 99.0
        schwarz = float((mu & sz.schwarz).sum()) / max(float(mu.sum()), 1.0)
        return (1 - _iou((mu | ober) & ~zf, zb & ~zf)) + 0.02 * dist + 1.5 * schwarz
    best, bl = None, 1e9
    for dx in (0, 300, -300, 600, -600):
        for dz in (0, 300, -300):
            x0 = np.array([a_u0[0] + dx, a_u0[1], a_u0[2] + dz, s0['rk'], s0['ra']], float)
            res = minimize(f, x0, method='Powell', bounds=[(None, None)] * 3 + GRENZEN_MASS[:2], options=dict(maxiter=2000, xtol=0.5, ftol=1e-5))
            if res.fun < bl:
                best, bl = res.x, res.fun
    return best, bl


def fuss_start(sz, sd, a_u, s):
    """Sohle (lokal +Y nach aussen) zur Kamera, Zehen (lokal +X) im Bild nach unten, Masse aus der Maske."""
    d = SEITEN[sd]
    (Ru, Tu), (Rf0, Tf) = sz.welt(sd, a_u, [0, 0, 0])
    n = sz.auge - Tf
    n /= np.linalg.norm(n)
    unten = sz.kam['R'][1]
    t = unten - (unten @ n) * n
    t /= np.linalg.norm(t)
    z = np.cross(t, n)
    Rf = np.stack([t, n, z], 1)
    a_f = np.array(FE.winkel_aus_matrix(Ru.T @ Rf), float)
    zf = sz.masken[d['fusst']]
    ys, xs = np.nonzero(zf)
    tiefe = FK.tiefe(sz.kam, Tf)
    # Pixel je Einheit an dieser Tiefe: zwei Punkte 100 Einheiten quer
    q0, q1 = FK.projiziere(sz.kam, Tf), FK.projiziere(sz.kam, Tf + 100 * sz.kam['R'][0])
    ppe = float(np.hypot(*(q1 - q0))) / 100.0
    W = float(np.clip((xs.max() - xs.min()) / ppe * 0.9, *GRENZEN_MASS[2]))
    Lg = float((ys.max() - ys.min()) / ppe)
    L = float(np.clip(Lg * 0.78, *GRENZEN_MASS[4]))
    hl = float(np.clip(Lg * 0.18, *GRENZEN_MASS[5]))
    return a_f, dict(s, W=W, L=L, hl=hl, H=float(np.clip(W * 1.1, *GRENZEN_MASS[3]))), dict(tiefe=round(tiefe), px_je_einheit=round(ppe, 4))


def stufe_fuss(sz, sd, a_u, a_f0, s0, roi_gewicht=0.8):
    """Fuss gegen seinen Schnitt, dazu Abdeckung der Zielmaske (ziel2, weiss) im Bereich um Unterschenkel und Fuss: dort zeigt der Screenshot Figur, die
    die neuen Schnitte nicht enthalten (rechts neben dem rechten Fuss 711 px, Lauf I_fuesse/_g v2: Gesamt-IoU 0,946)."""
    d = SEITEN[sd]
    zf = sz.masken[d['fusst']]
    (Ru, Tu), _ = sz.welt(sd, a_u, [0, 0, 0])
    r_, k0_, k1_ = unterschenkel_ringe(s0)
    Pu, tu = dreiecke_aus_ringen(r_, k0_, k1_)
    mu = sz.raster(Pu @ Ru.T + Tu, tu)
    ys, xs = np.nonzero(zf | sz.masken[d['bein']])
    roi = np.zeros_like(zf)
    roi[max(ys.max() - 90, 0):min(ys.max() + 25, 480), max(xs.min() - 25, 0):min(xs.max() + 25, 640)] = True
    weiss = roi & sz.ziel & ~P.logo_maske()
    basis = mu | sz.ober[sd]

    def f(x):
        s = dict(s0, W=x[3], H=x[4], L=x[5], hl=x[6])
        _, (Rf, Tf) = sz.welt(sd, a_u, x[:3])
        rf, f0, f1 = fuss_ringe(s)
        Pf, tf = dreiecke_aus_ringen(rf, f0, f1)
        mf = sz.raster(Pf @ Rf.T + Tf, tf)
        for zr, z0, z1, _d in zehen(s):
            Pz, tz = dreiecke_aus_ringen(zr, z0, z1)
            mf |= sz.raster(Pz @ Rf.T + Tf, tz)
        n = Rf @ np.array([0.0, 1.0, 0.0])
        c = Tf + Rf @ np.array([0.4 * s['L'], s['H'], 0.0])
        zur = sz.auge - c
        cosw = float(n @ zur / np.linalg.norm(zur))
        schwarz = float((mf & sz.schwarz).sum()) / max(float(mf.sum()), 1.0)
        fehlt = float((weiss & ~(mf | basis)).sum()) / max(float(weiss.sum()), 1.0)
        # nicht in den Boden (Welt y = 0, + = unten): Lauf I_fuesse/_g v4 hatte den linken Fuss 337 Einheiten tief
        ymax = float((Pf @ Rf.T + Tf)[:, 1].max())
        return (1 - _iou(mf, zf)) + 2.0 * max(0.0, 0.75 - cosw) + 1.5 * schwarz + roi_gewicht * fehlt + BODEN_STRAFE * max(0.0, ymax)
    best, bl = None, 1e9
    for dz in (0, 150, -150, 300, -300):
        x0 = np.array(list(a_f0[:2]) + [a_f0[2] + dz, s0['W'], s0['H'], s0['L'], s0['hl']], float)
        res = minimize(f, x0, method='Powell', bounds=[(None, None)] * 3 + GRENZEN_MASS[2:], options=dict(maxiter=3000, xtol=0.5, ftol=1e-5))
        if res.fun < bl:
            best, bl = res.x, res.fun
    return best, bl


def fit2():
    st = json.load(open(os.path.join(HIER, 'stand_final.json')))
    w1 = st['posen']['1']['winkel']
    sz = Szene(os.path.join(HIER, '..', 'H_stoff', 'h2', 'form.json'))
    aus = {}
    for sd, d in SEITEN.items():
        s0 = dict(rk=190.0, ra=110.0, e=1.15, W=260.0, H=260.0, L=450.0, hl=120.0)
        xb, lb = stufe_bein(sz, sd, w1[d['unter']], s0)
        a_u = xb[:3]
        s1 = dict(s0, rk=xb[3], ra=xb[4])
        a_f0, s2, kinfo = fuss_start(sz, sd, a_u, s1)
        xf, lf = stufe_fuss(sz, sd, a_u, a_f0, s2)
        a_f = xf[:3]
        s3 = dict(s2, W=xf[3], H=xf[4], L=xf[5], hl=xf[6])
        x = np.array(list(a_u) + list(a_f) + [s3['rk'], s3['ra'], s3['W'], s3['H'], s3['L'], s3['hl']])
        info, mu, mf = bewerten(sz, sd, x, ausgabe=True)
        aus[sd] = dict(winkel_unter=[round(float(v), 1) for v in a_u], winkel_fuss=[round(float(v), 1) for v in a_f],
                       mass={k: round(float(v), 1) for k, v in s3.items()}, kamera=kinfo, **info)
        print(sd, json.dumps(aus[sd]))
    json.dump(aus, open(os.path.join(HIER, 'zw', 'beine.json'), 'w'), indent=1)
    return aus


if __name__ == '__main__' and sys.argv[1:] == ['fit2']:
    fit2()


# ═══════════════════════════════════════════════════ Einbau in vm_form (Ringe, Winkel)
BEIN_TEILE = ('unterschenkel_l', 'fuss_l', 'zeh0_l', 'zeh1_l', 'zeh2_l', 'unterschenkel_r', 'fuss_r', 'zeh0_r', 'zeh1_r', 'zeh2_r')


def ringe_ersetzen(teile, rk):
    """rk[i] = [ring[T,n,3], k0, k1] fuer Unterschenkel, Fuss, Zehen aus zw/beine.json. -> Menge der ersetzten Teilindizes."""
    B = json.load(open(os.path.join(HIER, 'zw', 'beine.json')))
    ers = set()
    for i, p in enumerate(teile):
        if p.name not in BEIN_TEILE:
            continue
        sd = p.name[-1]
        s = dict(B[sd]['mass'])
        if p.name.startswith('unterschenkel'):
            r, k0, k1 = unterschenkel_ringe(s)
        elif p.name.startswith('fuss'):
            r, k0, k1 = fuss_ringe(s)
        else:
            z = zehen(s)
            oz = float(p.o[2])
            wahl = 1 if abs(oz) < 50 else (0 if oz < 0 else 2)
            r, k0, k1, _d = z[wahl]
        rk[i] = [np.stack(r), np.asarray(k0, float), np.asarray(k1, float)]
        ers.add(i)
    return ers


def _fuss_flach(Ru, Rf):
    x = Rf[:, 0]
    psi = math.atan2(-x[2], x[0])
    return np.array([[math.cos(psi), 0, math.sin(psi)], [0, 1, 0], [-math.sin(psi), 0, math.cos(psi)]])


def winkel_setzen(g, stand, ruhe, form_probe_bauen, wurzel_y=None):
    """stand/ruhe veraendern: Pose 1 (Clip 0) Winkel aus der Anpassung. Pose 2 (Clip 1) und Ruhe: Fuss flach (lokal +Y = Modell +Y), Gier des Fusses wie
    bisher, und das Knie so weit gebeugt (Winkel x des Unterschenkels, Bisektion), dass die Sohle auf dem Boden steht: Pose 2 mit der Wurzelhoehe aus C
    (wurzel_y[1]), Ruhe beide Sohlen auf gleicher Hoehe (Boden setzt danach boden_nachfuehren). form_probe_bauen(winkel) -> (R, T) im Modellraum."""
    B = json.load(open(os.path.join(HIER, 'zw', 'beine.json')))
    w1 = stand['posen']['1']['winkel']
    for sd, d in SEITEN.items():
        w1[d['unter']] = list(B[sd]['winkel_unter'])
        w1[d['fuss']] = list(B[sd]['winkel_fuss'])

    fuss_pkt = {}
    for sd in SEITEN:
        rf, f0, f1 = fuss_ringe(B[sd]['mass'])
        pts = [np.concatenate(list(rf) + [f0[None], f1[None]])]
        for zr, z0, z1, _d in zehen(B[sd]['mass']):
            pts.append(np.concatenate(list(zr) + [z0[None], z1[None]]))
        fuss_pkt[sd] = np.concatenate(pts)

    def sohle_y(w, sd):
        """tiefster Punkt des Fusses (Ecken, Kappen, Zehen) im Modellraum, Wurzel bei 0."""
        d = SEITEN[sd]
        R, T = form_probe_bauen(w)
        Ru = R[d['unter']].astype(float) / 4096.0
        Rf = R[d['fuss']].astype(float) / 4096.0
        Tf = T[d['fuss']].astype(float)
        return float((fuss_pkt[sd] @ Rf.T + Tf)[:, 1].max()), Ru, Rf

    for w, ziel in ((stand['posen']['2']['winkel'], None if wurzel_y is None else -float(wurzel_y[1])), (ruhe['winkel'], 'gleich')):
        # erst flach stellen, dann Knie beugen
        for sd, d in SEITEN.items():
            _y, Ru, Rf = sohle_y(w, sd)
            w[d['fuss']] = list(FE.winkel_aus_matrix(Ru.T @ _fuss_flach(Ru, Rf)))
        if ziel == 'gleich':
            ys = {sd: sohle_y(w, sd)[0] for sd in SEITEN}
            zy = {sd: max(ys.values()) for sd in SEITEN}
        elif ziel is None:
            continue
        else:
            zy = {sd: ziel for sd in SEITEN}
        for sd, d in SEITEN.items():
            w_alt = list(w[d['unter']])
            erledigt = False
            for achse in (0, 2, 1):
                # Knie beugen: erst um x, dann z, dann y, die erste Achse mit Vorzeichenwechsel gilt (x allein reichte links in Pose 2 nicht)
                w[d['unter']] = list(w_alt)
                a0 = float(w_alt[achse])
                lo, hi = -1000.0, 1000.0

                def f(dax):
                    w[d['unter']][achse] = a0 + dax
                    y, Ru, Rf = sohle_y(w, sd)
                    w[d['fuss']] = list(FE.winkel_aus_matrix(Ru.T @ _fuss_flach(Ru, Rf)))
                    y, _a, _b = sohle_y(w, sd)
                    return y - zy[sd]
                f_lo, f_hi = f(lo), f(hi)
                if f_lo * f_hi > 0:
                    continue
                # kleinster Ausschlag: von 0 aus in beide Richtungen auf den naechsten Wechsel
                f0 = f(0.0)
                if f0 * f_hi <= 0:
                    lo, f_lo = 0.0, f0
                else:
                    hi = 0.0
                for _ in range(40):
                    mid = 0.5 * (lo + hi)
                    fm = f(mid)
                    if fm * f_lo > 0:
                        lo, f_lo = mid, fm
                    else:
                        hi = mid
                f(0.5 * (lo + hi))
                erledigt = True
                break
            if not erledigt:
                w[d['unter']] = list(w_alt)
                y, Ru, Rf = sohle_y(w, sd)
                w[d['fuss']] = list(FE.winkel_aus_matrix(Ru.T @ _fuss_flach(Ru, Rf)))
    return stand, ruhe
