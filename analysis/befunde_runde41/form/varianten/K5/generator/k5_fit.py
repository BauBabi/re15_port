#!/usr/bin/env python3
"""Variante K5: Fit von Kopfform und Pose Bild 2 an die Landmarken L1-L8 (FORMBLATT_V2), danach rechter Arm aus dem Kieferbereich (AE4),
Feinlauf mit Tiefenpuffer (Augen sichtbar, Kiefer-IoU, Umriss) und Pose Bild 1 (Kopf versteckt, Umriss).

    python k5_fit.py --name K5a --f4 kurz --f5 hals --aus ../_lauf/K5a/fit.json

F4 (Schnauzenlaenge im Raum, Augenmitte -> OK-Spitze in Augenabstaenden S): kurz 1,4-1,6 (zeigt nach unten), lang 2,5-3,0 (stark zur Kamera).
F5 (Weg zum Kopf vor der Brust): hals = Hals laenger (Knochenversatz 22, Halsnetz gestreckt), Rumpf J unveraendert;
     rumpf = Hals 503 wie J, Wirbel 7 und Brust 10 in Pose Bild 2 frei; beide Arme per IK (linke Hand bleibt in der Welt, wo sie bei J ist).
Kein RE-Wert: Gestaltung eines neuen Modells nach Videostandbild und Nutzer-Vorgaben (PORT-WAHL)."""
import argparse
import json
import math
import os
import time

import cv2
import numpy as np
from scipy.optimize import minimize

import k5_kern as C
import k5_kopf as K5

KK, FE, FK = C.KK, C.FE, C.FK
F4 = {'kurz': (1.4, 1.6), 'lang': (2.5, 3.0)}
L6_ZIEL = (270.0, 404.0)      # Lappenmitte zwischen Nutzerpunkt (279,399) und tiefstem Punkt (264,408) (Festlegung K5, Pruefung R5 deckt den Lappen)
# Formgrenzen (Vorgabe, min, max); S nach Formblatt 2.4
# Lauf 1 (_lauf/K5a, K5b) lief mit Wk_s <= 1,15, x_auge <= 760, gx <= 620 an die Grenzen; Krokodilschaedel sind hinten breit, die Augen liegen
# eng oben (Augenabstand Mitte-Mitte rund 1/3-1/2 der Schaedelbreite), darum Wk bis 1,5 S. Kiefertiefe/-breite frei (UK-Band 37 px breit).
FORMGRENZEN = {'S': (640.0, 560.0, 770.0), 'Wk_s': (1.0, 0.55, 1.5), 'taper': (0.40, 0.25, 0.55), 'x_auge': (650.0, 420.0, 950.0),
               'yt_s_s': (-0.30, -0.45, -0.15), 'yt_t_s': (-0.12, -0.25, 0.02), 'gx': (500.0, 300.0, 850.0),
               'kiefer_boden': (320.0, 180.0, 420.0), 'kiefer_breit': (0.92, 0.80, 1.0),
               # Lauf v3-K5b/K5d: S und e an den Untergrenzen, UK-Spitze 9 px zu tief, OK-Spitze 5 px zu hoch. Frei dazu: Gelenk-y (Schaedeltiefe) und
               # Kieferueberstand (Krokodil: Oberkiefer ragt vorn ueber den Unterkiefer, positiv = Unterkiefer kuerzer)
               'gy': (420.0, 300.0, 650.0), 'kiefer_kuerzer': (-30.0, -60.0, 400.0)}
NFORM = 12
# Mindestoeffnung des Kiefers (az, 4096 = 360 Grad): kurz 180 = 16 Grad; lang 70 = 6 Grad - der lange Kiefer (rund 1700) klafft an der Spitze
# schon bei 6 Grad rund 180 Einheiten (Lauf v2-K5b mit 16 Grad: UK-Spitze 14-19 px zu tief, Phase-1-Kosten 35-61 statt 7)
KIEFER_MIN = {'kurz': 180.0, 'lang': 70.0}


def _b(v, lo, hi, x):
    """beschraenkt: x = 0 -> v."""
    return lo + (hi - lo) * 0.5 * (1 + math.tanh(x + math.atanh(min(max(2 * (v - lo) / (hi - lo) - 1, -0.999), 0.999))))


def form_aus(fx, f4):
    """Formvektor -> Kopfparameter; L so, dass die Schnauze (Augenmitte -> Spitze) e * S lang ist (e frei in F4)."""
    g = FORMGRENZEN
    S = _b(*g['S'], fx[0])
    p = dict(K5.VORGABE)
    p['S'] = S
    p['Wk'] = _b(*g['Wk_s'], fx[1]) * S
    p['taper'] = _b(*g['taper'], fx[2])
    p['x_auge'] = _b(*g['x_auge'], fx[3])
    p['yt_s'] = _b(*g['yt_s_s'], fx[4]) * S
    p['yt_t'] = _b(*g['yt_t_s'], fx[5]) * S
    p['yt_k'] = min(p['yt_s'] - 0.12 * S, -0.40 * S)
    p['gx'] = _b(*g['gx'], fx[6])
    p['kiefer_boden'] = _b(*g['kiefer_boden'], fx[8])
    p['kiefer_breit'] = _b(*g['kiefer_breit'], fx[9])
    fx = list(fx) + [0.0] * (12 - len(fx))
    p['gy'] = _b(*g['gy'], fx[10])
    p['kiefer_kuerzer'] = _b(*g['kiefer_kuerzer'], fx[11])
    lo, hi = F4[f4]
    e = _b((lo + hi) / 2, lo, hi, fx[7])
    L = p['x_auge'] + e * S
    for _ in range(4):
        p['L'] = L
        L += (e - K5.schnauze_e(p)) * S
    p['L'] = L
    p['e_soll'] = e
    return p


def dreh_auf(a, b):
    a = np.asarray(a, float) / np.linalg.norm(a)
    b = np.asarray(b, float) / np.linalg.norm(b)
    v = np.cross(a, b)
    c = float(a @ b)
    if np.linalg.norm(v) < 1e-9:
        return np.eye(3)
    K = np.array([[0, -v[2], v[1]], [v[2], 0, -v[0]], [-v[1], v[0], 0]])
    return np.eye(3) + K + K @ K / (1 + c)


def winkel(R):
    return [float(v) for v in FE.winkel_aus_matrix(R)]


class Lauf:
    def __init__(self, f4, f5):
        self.f4, self.f5 = f4, f5
        self.fig = C.Figur()
        self.kam = KK.kamera(2)
        self.kam1 = KK.kamera(1)
        self.Rj, self.Tj = self.fig.welt(1)        # Pose Bild 2 von J
        # statischer Tiefenpuffer: alles ausser Kopf/Hals/rechter Arm (hals) bzw. ausser Rumpf oben und Armen (rumpf)
        bew = set(C.KOPF) | set(C.RECHTER_ARM)
        if f5 == 'rumpf':
            bew |= {7, 10} | set(C.LINKER_ARM)
        self.bewegt = sorted(bew)
        tris, kenn = self.fig.dreiecke(self.Rj, self.Tj, ohne=self.bewegt)
        self.tief0, self.idb0 = C.raster(self.kam, tris, kenn)
        self.hand_l = (self.Rj[13].copy(), self.Tj[13].copy())
        self.hand_r = (self.Rj[18].copy(), self.Tj[18].copy())
        self.ellbogen = {'l': self.Tj[12].copy(), 'r': self.Tj[17].copy()}
        self.okm, self.ukm = KK.maske('oberkiefer'), KK.maske('unterkiefer')
        self.raf = KK.maske('right_arm_front') & ~self.ukm
        self.schwarz2 = cv2.imread(os.path.join(C.FORM, 'ziel2_bild2.png'), 0) == 0
        self.arm_r = None        # (Zielpunkt Handgelenk Welt, Handdrehung Welt) nach Stufe A
        self.lm_j = {b: FK.projiziere(self.kam, self.Tj[b]) for b in (10, 11, 16)}
        self.umriss_min = 0.0

    # ───────── Gerippe aus dem Parametervektor
    def arm_ik(self, Rw, Tw, seite, ziel_t, ziel_R):
        s0, s1, s2 = (11, 12, 13) if seite == 'l' else (16, 17, 18)
        a = float(np.linalg.norm(self.fig.versatz[s1]))
        b = float(np.linalg.norm(self.fig.versatz[s2]))
        S = Tw[s0]
        d = np.asarray(ziel_t, float) - S
        dl = float(np.clip(np.linalg.norm(d), abs(a - b) + 1, a + b - 1))
        u = d / max(np.linalg.norm(d), 1e-9)
        ca = (a * a + dl * dl - b * b) / (2 * a * dl)
        pol = self.ellbogen[seite] - self.Tj[s0]
        v = pol - (pol @ u) * u
        v = v / max(np.linalg.norm(v), 1e-9)
        E = S + a * (ca * u + math.sqrt(max(1 - ca * ca, 0)) * v)
        Wr = S + dl * u
        R11 = dreh_auf(self.Rj[s0][:, 1], E - S) @ self.Rj[s0]
        R12 = dreh_auf(self.Rj[s1][:, 1], Wr - E) @ self.Rj[s1]
        e = 10
        return {s0: winkel(Rw[e].T @ R11), s1: winkel(R11.T @ R12), s2: winkel(R12.T @ ziel_R)}

    def gerippe(self, x, mit_arm=True):
        """x = [Form 8 | Kopfpose ...]. Rueckgabe (Kopfparameter, Winkel-Ueberschreibung Clip 1, Halslaenge)."""
        p = form_aus(x[:NFORM], self.f4)
        y = x[NFORM:]
        Wj = self.fig.winkel[1]
        w = {}
        w[21] = [self.start21[i] + 200 * y[i] for i in range(3)]
        w[22] = [self.start22[i] + 200 * y[3 + i] for i in range(3)]
        w[23] = [0.0, 60 * math.tanh(y[7]), float(np.clip(380 + 200 * y[6], KIEFER_MIN[self.f4], 800))]      # Maul offen (E20, Formblatt 2.2)
        hl = 503.0
        k = 8
        if self.f5 == 'hals':
            hl = _b(self.start_hl, 503.0, 2200.0, y[8])
            k = 9
        else:
            w[7] = [Wj[7][i] + 200 * y[k + i] for i in range(3)]
            w[10] = [Wj[10][i] + 200 * y[k + 3 + i] for i in range(3)]
            k += 6
        return p, w, hl, k

    def welt(self, x, mit_arm=True):
        p, w, hl, k = self.gerippe(x)
        self.fig.versatz[22] = np.array([hl, 0.0, 0.0])
        self.fig.versatz[23] = np.array([p['gx'], p['gy'], 0.0])
        Rw, Tw = self.fig.welt(1, w)
        if self.f5 == 'rumpf':
            w.update(self.arm_ik(Rw, Tw, 'l', self.hand_l[1], self.hand_l[0]))
        if mit_arm and self.arm_r is not None:
            w.update(self.arm_ik(Rw, Tw, 'r', self.arm_r[0], self.arm_r[1]))
        if self.f5 == 'rumpf' or (mit_arm and self.arm_r is not None):
            Rw, Tw = self.fig.welt(1, w)
        return p, w, hl, Rw, Tw

    # ───────── Phase 1: Landmarken (ohne Raster)
    def lm_bild(self, p, Rw, Tw):
        lm = K5.landmarken(p)
        out = {}
        for b, d in lm.items():
            for nm, v in d.items():
                out[nm] = FK.projiziere(self.kam, Rw[b] @ np.asarray(v, float) + Tw[b])
        return out, lm

    def kosten_lm(self, p, Rw, Tw, w, hl):
        q, lm = self.lm_bild(p, Rw, Tw)
        if not all(np.isfinite(v).all() for v in q.values()):
            return 1e6, {}
        z = [(q['auge_l'], C.L1, 1.5), (q['auge_r'], C.L2, 1.5), (q['ok_spitze'], C.L4, 1.0), (q['maulwinkel_l'], C.L7, 0.8), (q['uk_spitze'], L6_ZIEL, 0.8)]
        e = sum(g * ((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2) / 16.0 for a, b, g in z)
        # Augenlinie waagerecht (R2, <= 3 Grad = 3 px Hoehenunterschied auf 60 px)
        e += 1.0 * ((q['auge_l'][1] - q['auge_r'][1]) / 2.0) ** 2
        # rechter Kieferast (W) nicht sichtbar (Formblatt 2.3): rechter Maulwinkel (W) nicht links (B) neben dem Oberkiefer-Keil (x 212-233),
        # dort liegen die weissen Flecken (F7, nicht Wesen). Lauf v2-K5b: rechter Maulwinkel bei x 175, sichtbarer UK quer (R7 Achse 2,6 Grad).
        e += 0.5 * (max(0.0, 214.0 - q['maulwinkel_r'][0]) / 4.0) ** 2
        # Augen vor dem statischen Koerper (Tiefe am Zielpunkt) und vor dem Hals/Rumpf; Kopf nicht hinter der Brust
        for nm in ('auge_l', 'auge_r'):
            b = 22
            wpt = Rw[b] @ np.asarray(lm[22][nm], float) + Tw[b]
            dz = FK.tiefe(self.kam, wpt)
            xx, yy = int(round(q[nm][0])), int(round(q[nm][1]))
            if 0 <= xx < C.W and 0 <= yy < C.H and np.isfinite(self.tief0[yy, xx]):
                e += 4.0 * max(0.0, dz - (self.tief0[yy, xx] - 120)) ** 2 / 100.0 ** 2
            if self.f5 == 'hals':
                # Brust (10) steht im statischen Puffer; Halsnetz/Hals vor den Augen ist Phase 2
                pass
        # Augentiefe gegen J-Brust an den Augenpunkten (Leser A: 4759-4822), nur Hals-Weg (Rumpf J)
        if self.f5 == 'rumpf':
            # moeglichst wenig Rumpfbewegung: Winkel und vor allem Bildlage von Brust und Schultern (Umriss-Ersatz ohne Raster)
            Wj = self.fig.winkel[1]
            e += 0.002 * sum(((w[b][i] - Wj[b][i]) / 50.0) ** 2 for b in (7, 10) for i in range(3))
            for b in (10, 11, 16):
                a = FK.projiziere(self.kam, Tw[b])
                j = self.lm_j[b]
                e += 0.5 * ((a[0] - j[0]) ** 2 + (a[1] - j[1]) ** 2) / 64.0
        else:
            e += 0.5 * ((hl - 503.0) / 1000.0) ** 2
        return e, q

    def kosten1(self, x):
        p, w, hl, Rw, Tw = self.welt(x, mit_arm=False)
        e, _q = self.kosten_lm(p, Rw, Tw, w, hl)
        return e + 0.01 * float(np.sum(np.asarray(x[:NFORM]) ** 2))

    # ───────── Raster
    def raster(self, Rw, Tw, ohne=()):
        knochen = [b for b in self.bewegt if b not in ohne]
        tris, kenn = self.fig.dreiecke(Rw, Tw, knochen=knochen)
        return C.raster(self.kam, tris, kenn, self.tief0.copy(), self.idb0.copy())

    def kosten2(self, x, protokoll=False):
        p, w, hl, Rw, Tw = self.welt(x)
        self.fig.kopf_neu(p)
        self.fig.hals_neu(hl)
        e, q = self.kosten_lm(p, Rw, Tw, w, hl)
        tief, idb = self.raster(Rw, Tw)
        for k in (C.AUGE_L, C.AUGE_R):
            n = int((idb == k).sum())
            e += 3.0 * max(0, 8 - n) ** 2 / 16.0
        uk = idb == 23
        kopf = np.isin(idb, (22, C.AUGE_L, C.AUGE_R))
        unten = np.zeros_like(kopf)
        unten[287:] = True
        iou_uk = KK.iou(uk, self.ukm)
        iou_ok = KK.iou(kopf & unten, self.okm & unten)
        u = C.umriss(idb, 2)
        lap = C.lappen_maske()
        dk = float((uk & lap).sum()) / float(lap.sum())
        # rechter Arm nicht im Kiefer (AE4)
        arm_im_kiefer = float((np.isin(idb, C.RECHTER_ARM) & (self.okm | self.ukm)).sum())
        # R7: sichtbarer Unterkiefer rechts (B) der Kopfmittellinie (Augenmitte -> OK-Spitze); Pixel links davon = rechter Ast (W)
        ys, xs = np.nonzero(uk)
        m0 = (q['auge_l'] + q['auge_r']) / 2
        dvec = q['ok_spitze'] - m0
        links = int(((xs - m0[0]) * dvec[1] - (ys - m0[1]) * dvec[0] < 0).sum()) if len(xs) else 0
        e += links / 150.0
        # Kopf/Kiefer nicht auf Ziel-Schwarz (Bau v2-K5a: 301 px Kopf bei x 189-204, y 305-324 links (B) neben dem Kopfrand rechts (W) x 204-206)
        e += float((np.isin(idb, (22, 23, C.AUGE_L, C.AUGE_R)) & self.schwarz2).sum()) / 40.0
        # Umriss als Gewinn (0,01 Umriss = 0,4 = rund 2,5 px Fehler an einer Landmarke) plus harte Strafe unter umriss_min
        s = e - 6.0 * iou_uk - 4.0 * iou_ok - 3.0 * dk - 40.0 * u + 400.0 * max(0.0, self.umriss_min - u) + arm_im_kiefer / 300.0
        if protokoll:
            return s, dict(iou_uk=round(iou_uk, 3), iou_ok_unten=round(iou_ok, 3), lappen=round(dk, 3), umriss=u, arm_im_kiefer=int(arm_im_kiefer),
                           augen_px=[int((idb == C.AUGE_L).sum()), int((idb == C.AUGE_R).sum())], lm_kosten=round(e, 2))
        return s

    # ───────── Stufe A: rechter Arm (Hand an right_arm_front ohne Unterkiefer)
    def arm_kosten(self, ya, x):
        Rh0, Th0 = self.hand_r
        ziel_t = Th0 + 400.0 * np.asarray(ya[:3])
        ziel_R = KFIT_mat_welt(ya[3:6]) @ Rh0
        self.arm_r = (ziel_t, ziel_R)
        p, w, hl, Rw, Tw = self.welt(x)
        tief, idb = self.raster(Rw, Tw)
        arm = np.isin(idb, C.RECHTER_ARM)
        i = KK.iou(arm, self.raf)
        im_kiefer = float((arm & (self.okm | self.ukm)).sum())
        u = C.umriss(idb, 2)
        # Umriss staerker (Bau v2-K5a: rechte Klaue verliess (252-283, 395-443), Umriss Bild 2 0,936 gegen K4 0,949)
        return -(2.0 * i + 5.0 * u) + im_kiefer / 400.0 + 0.02 * float(np.sum(np.asarray(ya) ** 2))

    def stufe_a(self, x, iters=300):
        best = None
        for st in range(4):
            # Starts: Hand wie J (0), nach links unten im Bild (1, Kamera rechts/unten in der Welt), zwei zufaellige um (1)
            y0 = np.zeros(6)
            if st >= 1:
                y0[:3] = 0.6 * (-1.3 * self.kam['R'][0] + 0.9 * self.kam['R'][1])
            if st >= 2:
                y0 += np.random.RandomState(st).normal(0, 0.3, 6)
            r = minimize(lambda ya: self.arm_kosten(ya, x), y0, method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-4))
            if best is None or r.fun < best.fun:
                best = r
        self.arm_kosten(best.x, x)
        return best

    # ───────── Bild 1: Kopf verstecken (Umriss), Winkel 21/22/23 in Clip 0
    def bild1(self, p, hl, iters=500):
        f = self.fig
        f.kopf_neu(p)
        f.hals_neu(hl)
        f.versatz[22] = np.array([hl, 0.0, 0.0])
        Rj1, Tj1 = f.welt(0)
        tris, kenn = f.dreiecke(Rj1, Tj1, ohne=C.KOPF)
        t0, i0 = C.raster(self.kam1, tris, kenn)
        W0 = {b: list(f.winkel[0][b]) for b in C.KOPF}

        def zerl(x):
            return {21: [W0[21][i] + 200 * x[i] for i in range(3)], 22: [W0[22][i] + 200 * x[3 + i] for i in range(3)],
                    23: [W0[23][0], W0[23][1], float(np.clip(W0[23][2] + 200 * x[6], 0, 300))]}

        def fk(x):
            w = zerl(x)
            Rw, Tw = f.welt(0, w)
            tr, kn = f.dreiecke(Rw, Tw, knochen=C.KOPF)
            _t, idb = C.raster(self.kam1, tr, kn, t0.copy(), i0.copy())
            sicht = float(np.isin(idb, (21, 22, 23, C.AUGE_L, C.AUGE_R)).sum())
            return -C.umriss(idb, 1) + sicht / 40000.0 + 0.002 * float(np.sum(np.asarray(x[:6]) ** 2))
        best = None
        for st in range(3):
            x0 = np.zeros(7) if st == 0 else np.random.RandomState(10 + st).normal(0, 0.6, 7)
            r = minimize(fk, x0, method='Powell', options=dict(maxfev=iters, xtol=1e-2, ftol=1e-5))
            if best is None or r.fun < best.fun:
                best = r
        w = zerl(best.x)
        Rw, Tw = f.welt(0, w)
        tr, kn = f.dreiecke(Rw, Tw)
        _t, idb = C.raster(self.kam1, tr, kn)
        return {b: [int(round(v)) for v in w[b]] for b in C.KOPF}, idb


def KFIT_mat_welt(y):
    return C.mat([200 * v for v in y])


def start_kopf(lauf, p, yaw_grad=20.0):
    """Startlage: Augenmitte auf dem Sehstrahl durch (248,5/275) in Tiefe 451*S/63; Schnauze zur Spitze (245,371) mit Laenge e*S; links (W)
    = Kamera rechts, um yaw_grad zur rechten Seite (W) gedreht. Rueckgabe Kopf-Weltdrehung und -ursprung."""
    kam = lauf.kam
    lm = K5.landmarken(p)[22]
    e_lok = np.array(lm['augenmitte'])
    t_lok = np.array(lm['ok_spitze'])
    L = float(np.linalg.norm(t_lok - e_lok))
    d = 451.0 * p['S'] / 63.0
    auge, r = FK.strahl(kam, np.array([248.5, 275.0]))
    Ew = auge + r * d / float(r @ kam['R'][2])
    auge2, r2 = FK.strahl(kam, np.array([245.0, 371.0]))
    # Punkt auf Strahl 2 mit Abstand L von Ew (naeher zur Kamera)
    b = 2 * r2 @ (auge2 - Ew)
    c = (auge2 - Ew) @ (auge2 - Ew) - L * L
    disk = b * b - 4 * c
    s = (-b - math.sqrt(max(disk, 0))) / 2 if disk > 0 else -b / 2
    Tw = auge2 + s * r2
    xh = (Tw - Ew) / np.linalg.norm(Tw - Ew)
    zr = kam['R'][0] - (kam['R'][0] @ xh) * xh
    zh = zr / np.linalg.norm(zr)
    yh = np.cross(zh, xh)
    Rh = np.stack([xh, yh, zh], 1)
    # lokale Achse Augenmitte->Spitze ist nicht genau +x: auf die lokale Richtung drehen
    dl = (t_lok - e_lok) / L
    Rh = Rh @ dreh_auf(dl, [1.0, 0, 0]).T
    Ry = C.mat([0, int(yaw_grad / 360 * 4096), 0])
    Rh = Rh @ Ry
    Oh = Ew - Rh @ e_lok
    return Rh, Oh


YAWS = (25.0, 10.0, 40.0)


def lauf(name, f4, f5, aus, iters=(3000, 700, 300, 400)):
    t0 = time.time()
    L = Lauf(f4, f5)
    L.start21 = L.start22 = None
    L.start_hl = 503.0
    fx0 = np.zeros(NFORM)
    p0 = form_aus(fx0, f4)
    prot = []
    best = None
    for yaw in YAWS:
        Rh, Oh = start_kopf(L, p0, yaw)
        R10, T10 = L.Rj[10], L.Tj[10]
        T21 = L.Tj[21]
        u = Oh - T21
        hl = float(np.linalg.norm(u))
        R21 = KFIT_rahmen(u / hl)
        w21 = winkel(R10.T @ R21)
        R21 = R10 @ C.mat(w21)
        w22 = winkel(R21.T @ Rh)
        L.start21, L.start22 = w21, w22
        L.start_hl = float(np.clip(hl, 520.0, 2150.0))
        n = NFORM + 8 + (1 if f5 == 'hals' else 6)
        x0 = np.zeros(n)
        if f5 == 'rumpf':
            # Hals bleibt 503: Wirbel 7 / Brust 10 so drehen, dass das Halsende den Kopfursprung erreicht, dann Hals/Kopf aus der neuen Brust
            yt = rumpf_start(L, Oh)
            x0[NFORM + 8:NFORM + 14] = yt
            Wj = L.fig.winkel[1]
            wt = {7: [Wj[7][i] + 200 * yt[i] for i in range(3)], 10: [Wj[10][i] + 200 * yt[3 + i] for i in range(3)]}
            Rw_, Tw_ = L.fig.welt(1, wt)
            u = Oh - Tw_[21]
            R21 = KFIT_rahmen(u / np.linalg.norm(u))
            w21 = winkel(Rw_[10].T @ R21)
            R21 = Rw_[10] @ C.mat(w21)
            L.start21, L.start22 = w21, winkel(R21.T @ Rh)
            print('  Rumpf-Start yaw %.0f: Abstand Halsbasis - Kopfursprung %.0f (Soll 503), Rumpf %s' % (yaw, np.linalg.norm(u), {b: [round(v) for v in ww] for b, ww in wt.items()}))
        r = minimize(L.kosten1, x0, method='Powell', options=dict(maxfev=iters[0], xtol=1e-3, ftol=1e-6))
        p, w, hl2, Rw, Tw = L.welt(r.x, mit_arm=False)
        e, q = L.kosten_lm(p, Rw, Tw, w, hl2)
        prot.append(dict(phase=1, yaw_start=yaw, kosten=round(float(r.fun), 3), lm={k: [round(float(v[0]), 1), round(float(v[1]), 1)] for k, v in q.items()},
                         halslaenge=round(hl2), S=round(p['S']), e=round(p['e_soll'], 2)))
        print('  Phase 1 yaw %.0f: Kosten %.3f, Hals %.0f, S %.0f, e %.2f, %s  (%.0f s)' % (yaw, r.fun, hl2, p['S'], p['e_soll'],
              {k: [round(float(v[0])), round(float(v[1]))] for k, v in q.items() if k != 'gaumen_hinten'}, time.time() - t0), flush=True)
        if best is None or r.fun < best[0]:
            best = (r.fun, r.x.copy(), (list(L.start21), list(L.start22), L.start_hl))
    _f, x, (L.start21, L.start22, L.start_hl) = best
    # Umriss-Untergrenze: J-Koerper mit dem Kopf aus Phase 1, ohne rechten Arm-Umbau
    p, w, hl, Rw, Tw = L.welt(x)
    L.fig.kopf_neu(p)
    L.fig.hals_neu(hl)
    L.umriss_min = 0.0
    # Stufe A: rechter Arm
    ra = L.stufe_a(x, iters[2])
    s, info = L.kosten2(x, protokoll=True)
    print('  Stufe A (rechter Arm): %s (%.0f s)' % (info, time.time() - t0), flush=True)
    L.umriss_min = min(0.940, info['umriss'] - 0.002)
    # Phase 2: Feinlauf mit Raster
    r2 = minimize(L.kosten2, x, method='Powell', options=dict(maxfev=iters[1], xtol=1e-2, ftol=1e-4))
    x = r2.x
    ra = L.stufe_a(x, iters[2] // 2)
    s, info = L.kosten2(x, protokoll=True)
    print('  Phase 2: %.3f %s (%.0f s)' % (s, info, time.time() - t0), flush=True)
    p, w, hl, Rw, Tw = L.welt(x)
    L.fig.kopf_neu(p)
    L.fig.hals_neu(hl)
    tief, idb = L.raster(Rw, Tw)
    q, _lm = L.lm_bild(p, Rw, Tw)
    m2 = C.messen_bild2(idb, q, umriss=C.umriss(idb, 2))
    # Bild 1
    w1, idb1 = L.bild1(p, hl, iters[3])
    m1 = C.messen_bild1(idb1)
    m1['umriss1_raster'] = C.umriss(idb1, 1)
    erg = dict(name=name, f4=f4, f5=f5, nform=NFORM, kopf=p, halslaenge=round(hl, 1), schnauze_e=round(K5.schnauze_e(p), 3),
               bild2=dict(winkel={int(b): [int(round(v)) for v in ww] for b, ww in w.items()}, mass=m2, info=info),
               bild1=dict(winkel={int(b): v for b, v in w1.items()}, mass=m1), x=[float(v) for v in x],
               start=dict(w21=L.start21, w22=L.start22, hl=L.start_hl), arm_r=[L.arm_r[0].tolist(), L.arm_r[1].tolist()], phase1=prot,
               sekunden=round(time.time() - t0))
    os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
    json.dump(erg, open(aus, 'w', encoding='utf-8'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    np.save(os.path.splitext(aus)[0] + '_id2.npy', idb)
    np.save(os.path.splitext(aus)[0] + '_id1.npy', idb1)
    print('%s fertig (%.0f s): %s' % (name, time.time() - t0, {k: v['wert'] for k, v in m2.items() if k.startswith('R')}))
    print('   Bild 1: %s' % {k: (v['wert'] if isinstance(v, dict) else v) for k, v in m1.items()})
    return erg


def rumpf_start(L, Oh):
    Wj = L.fig.winkel[1]

    def f(y):
        wt = {7: [Wj[7][i] + 200 * y[i] for i in range(3)], 10: [Wj[10][i] + 200 * y[3 + i] for i in range(3)]}
        Rw, Tw = L.fig.welt(1, wt)
        d = float(np.linalg.norm(Oh - Tw[21]))
        return ((d - 503.0) / 50.0) ** 2 + 0.05 * float(np.sum(np.asarray(y) ** 2))
    best = None
    for st in range(4):
        y0 = np.zeros(6) if st == 0 else np.random.RandomState(20 + st).normal(0, 1.0, 6)
        r = minimize(f, y0, method='Powell', options=dict(maxfev=2000, xtol=1e-3, ftol=1e-6))
        if best is None or r.fun < best.fun:
            best = r
    return best.x


def KFIT_rahmen(u):
    v0 = np.array([0, 1.0, 0])
    v = v0 - (v0 @ u) * u
    v /= np.linalg.norm(v)
    return np.stack([u, v, np.cross(u, v)], 1)


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--name', required=True)
    a.add_argument('--f4', choices=('kurz', 'lang'), required=True)
    a.add_argument('--f5', choices=('hals', 'rumpf'), required=True)
    a.add_argument('--aus', required=True)
    a.add_argument('--iters', default='3000,700,300,400')
    a.add_argument('--yaws', default='25,10,40')
    x = a.parse_args()
    YAWS = tuple(float(v) for v in x.yaws.split(','))
    lauf(x.name, x.f4, x.f5, x.aus, tuple(int(v) for v in x.iters.split(',')))
