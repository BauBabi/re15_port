#!/usr/bin/env python3
"""Ruhe-Masse (Messblatt RUHE, K5e). Definitionen (alle Zahlen hier):
- Lab: CIE-Lab D65, OpenCV float (RGB/255) wie Messblatt V4F. C* = hypot(a*, b*), Farbton h = atan2(b*, a*) in Grad 0..360.
- Figur: getroffene Pixel des Renders, erodiert 5x5 (2 px; Videokette 7x7 = 3 px, Filter verschmiert den Rand), damit kein Grund mitzaehlt.
- (a) Farbtonklassen: 24 Sektoren zu 15 Grad, nur Pixel mit C* >= 6; Anteil je Sektor an diesen Pixeln; S5 = Zahl der Sektoren mit >= 5 %.
- (b) Flecken: Klasse je Pixel = (Sektor oder 'unbunt' bei C* < 6, L*-Klasse floor(L*/8)); 4-Komponenten gleicher Klasse mit 6 px .. 5 % der
  Figur; Ring = Dilatation Kreis Radius 4 px minus Komponente, nur Figur, >= 6 px. Fleck, wenn |L*(Komp) - L*(Ring)| >= 8 (eine L*-Klasse)
  oder der Farbton abweicht: beide C* >= 6 und Winkelabstand >= 15 Grad (ein Sektor), oder genau einer C* >= 6 und |dC*| >= 6.
  Kennzahl F = Flecken je 10 000 Figurpixel; dazu Fleckflaeche in % der Figur.
  (b') geschaerft (Gegenprobe RUHE 2): farbfremde Flecken F_fremd = Flecken aus (b), deren eigener Farbton (C* >= 6) mehr als FREMD = 45 Grad vom
  Hauptfarbton der Figur (C*-gewichtetes Kreismittel aller bunten Figurpixel) abliegt; F_grau_hell = unbunte, hellere Insel in bunter Haut.
- (c) Buntheit-Streuung: Standardabweichung C* der Figur; Farbtonspanne = kleinster Kreisbogen, der 90 % der bunten Pixel (C* >= 6) enthaelt.
- (d) Flickenmass (Zusatz): Bloecke 16 x 16 px mit >= 50 % Figur, Block-Mittel a*/b*; Kreis-Streuung des Block-Farbtons (Grad, nur Bloecke mit
  Mittel-C* >= 4, gewichtet mit C*) und Spanne p10..p90 des Block-L*.
"""
import json, math, sys, os
import numpy as np, cv2
import rl

SEK = 15.0
C_MIN = 6.0
L_KL = 8.0
RING = 4
FREMD = 45.0   # Grad: Hauptfarbton +- (halbe Farbtonspanne der Originale 62/2 = 31) + 1 Sektor 15 -> 46, gerundet 45


def lab(img):
    return rl.v4f.lab(img).astype(np.float64)


def kreisspanne(h, anteil=0.9):
    """kleinster Kreisbogen (Grad), der `anteil` der Werte h (0..360) enthaelt."""
    if len(h) < 10:
        return float('nan')
    s = np.sort(np.mod(h, 360.0))
    n = len(s)
    k = int(math.ceil(anteil * n))
    s2 = np.concatenate([s, s + 360.0])
    breite = s2[np.arange(n) + k - 1] - s2[np.arange(n)]
    return float(breite.min())


def masse(img, msk, ero=2, mit_karte=False, liste=None):
    k = 2 * ero + 1
    v = cv2.erode(msk.astype(np.uint8), np.ones((k, k), np.uint8)) > 0
    L = lab(img)
    C = np.hypot(L[..., 1], L[..., 2])
    h = np.mod(np.degrees(np.arctan2(L[..., 2], L[..., 1])), 360.0)
    n = int(v.sum())
    bunt = v & (C >= C_MIN)
    sek = np.floor(h / SEK).astype(int) % 24
    hist = np.bincount(sek[bunt], minlength=24) / max(int(bunt.sum()), 1)
    # (b) Flecken
    kl = np.where(C >= C_MIN, sek, 24) * 16 + np.clip(np.floor(L[..., 0] / L_KL), 0, 15).astype(int)
    kl[~v] = -1
    ker = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * RING + 1, 2 * RING + 1))
    flecken, fl_px, gepr = 0, 0, 0
    typ = dict(hell=0, dunkel=0, ton=0, chroma=0, hell_px=0, fremd=0, fremd_px=0, grau_hell=0)
    zb = (C[bunt] * np.exp(1j * np.radians(h[bunt]))).sum()
    h0 = math.degrees(np.angle(zb)) % 360.0   # Hauptfarbton der Figur (C*-gewichtetes Kreismittel)
    karte = np.zeros(v.shape, np.uint8) if mit_karte else None
    maxa = 0.05 * n
    for wert in np.unique(kl[v]):
        m = (kl == wert).astype(np.uint8)
        nc, lb, st, _ = cv2.connectedComponentsWithStats(m, connectivity=4)
        for i in range(1, nc):
            a = st[i, cv2.CC_STAT_AREA]
            if a < 6 or a > maxa:
                continue
            x, y, w, hh = st[i, 0], st[i, 1], st[i, 2], st[i, 3]
            x0, y0 = max(x - RING, 0), max(y - RING, 0)
            x1, y1 = min(x + w + RING, v.shape[1]), min(y + hh + RING, v.shape[0])
            c = (lb[y0:y1, x0:x1] == i)
            r = (cv2.dilate(c.astype(np.uint8), ker) > 0) & ~c & v[y0:y1, x0:x1]
            if r.sum() < 6:
                continue
            gepr += 1
            Lc, Lr = L[y0:y1, x0:x1][c].mean(0), L[y0:y1, x0:x1][r].mean(0)
            Cc, Cr = math.hypot(Lc[1], Lc[2]), math.hypot(Lr[1], Lr[2])
            dl = abs(Lc[0] - Lr[0]) >= L_KL
            if Cc >= C_MIN and Cr >= C_MIN:
                d = abs(math.degrees(math.atan2(Lc[2], Lc[1])) - math.degrees(math.atan2(Lr[2], Lr[1]))) % 360.0
                dh = min(d, 360.0 - d) >= SEK
            else:
                dh = (Cc >= C_MIN) != (Cr >= C_MIN) and abs(Cc - Cr) >= C_MIN
            if liste is not None and (dl or dh):
                ys_, xs_ = np.nonzero(c)
                cov = np.cov(np.stack([xs_, ys_])) if len(xs_) > 2 else np.eye(2)
                ev = np.sort(np.linalg.eigvalsh(cov))
                el_ = float(np.sqrt(max(ev[1], 1e-9) / max(ev[0], 1e-3)))
                beide = Cc >= C_MIN and Cr >= C_MIN
                liste.append(dict(a=int(a), dL=float(Lc[0] - Lr[0]), ton=bool(beide and dh), chroma=bool((not beide) and dh), dC=float(Cc - Cr),
                                  el=el_, Lc=float(Lc[0]), Cc=float(Cc), hc=float(math.degrees(math.atan2(Lc[2], Lc[1])) % 360),
                                  hr=float(math.degrees(math.atan2(Lr[2], Lr[1])) % 360), Cr=float(Cr)))
            if dl or dh:
                flecken += 1
                fl_px += a
                beide_ = Cc >= C_MIN and Cr >= C_MIN
                if Lc[0] - Lr[0] >= L_KL:
                    typ['hell'] += 1
                    typ['hell_px'] += a
                if Lc[0] - Lr[0] <= -L_KL:
                    typ['dunkel'] += 1
                if dh and beide_:
                    typ['ton'] += 1
                if dh and not beide_:
                    typ['chroma'] += 1
                    if Cc < C_MIN and Lc[0] > Lr[0]:
                        typ['grau_hell'] += 1
                hc_ = math.degrees(math.atan2(Lc[2], Lc[1])) % 360.0
                if Cc >= C_MIN and min(abs(hc_ - h0) % 360.0, 360.0 - abs(hc_ - h0) % 360.0) > FREMD:
                    typ['fremd'] += 1
                    typ['fremd_px'] += a
                if karte is not None:
                    karte[y0:y1, x0:x1][c] = 1 if dl and not dh else (2 if dh and not dl else 3)
                    if typ['fremd_px'] and Cc >= C_MIN and min(abs(hc_ - h0) % 360.0, 360.0 - abs(hc_ - h0) % 360.0) > FREMD:
                        karte[y0:y1, x0:x1][c] = 4
    # (d) Bloecke
    bh, bl = [], []
    for y in range(0, v.shape[0] - 15, 16):
        for x in range(0, v.shape[1] - 15, 16):
            vv = v[y:y + 16, x:x + 16]
            if vv.mean() < 0.5:
                continue
            mm = L[y:y + 16, x:x + 16][vv].mean(0)
            bl.append(mm[0])
            cc = math.hypot(mm[1], mm[2])
            if cc >= 4:
                bh.append((math.atan2(mm[2], mm[1]), cc))
    if bh:
        an, wt = np.array([b[0] for b in bh]), np.array([b[1] for b in bh])
        Rr = abs((wt * np.exp(1j * an)).sum()) / wt.sum()
        kstreu = math.degrees(math.sqrt(max(-2 * math.log(max(Rr, 1e-9)), 0)))
    else:
        kstreu = float('nan')
    aus = dict(px=n, bunt_pct=round(100 * bunt.sum() / max(n, 1), 1),
               S5=int((hist >= 0.05).sum()), sektoren={str(int(i * SEK)): round(100 * float(hist[i]), 1) for i in range(24) if hist[i] >= 0.005},
               F=round(1e4 * flecken / max(n, 1), 1), F_hell=round(1e4 * typ['hell'] / max(n, 1), 1), F_dunkel=round(1e4 * typ['dunkel'] / max(n, 1), 1),
               F_ton=round(1e4 * typ['ton'] / max(n, 1), 1), F_chroma=round(1e4 * typ['chroma'] / max(n, 1), 1),
               hell_pct=round(100 * typ['hell_px'] / max(n, 1), 2), F_fremd=round(1e4 * typ['fremd'] / max(n, 1), 1),
               fremd_pct=round(100 * typ['fremd_px'] / max(n, 1), 2), F_grau_hell=round(1e4 * typ['grau_hell'] / max(n, 1), 1), h0=round(h0, 1), flecken=flecken, fleck_pct=round(100 * fl_px / max(n, 1), 1), komp_geprueft=gepr,
               C_std=round(float(C[v].std()), 2), C_mittel=round(float(C[v].mean()), 2), L_mittel=round(float(L[..., 0][v].mean()), 2),
               spanne90=round(kreisspanne(h[bunt]), 1),
               block_ton_streu=round(kstreu, 1), block_L_p10_p90=round(float(np.percentile(bl, 90) - np.percentile(bl, 10)), 1) if bl else None,
               bloecke=len(bl), bloecke_bunt=len(bh))
    if mit_karte:
        return aus, karte, v
    return aus


def mittel(liste):
    o = {}
    for k in liste[0]:
        x = [d[k] for d in liste]
        if isinstance(x[0], (int, float)) and x[0] is not None:
            o[k] = round(float(np.mean(x)), 2)
    o['je_saat'] = [{k: d[k] for k in ('S5', 'F', 'C_std', 'spanne90', 'block_ton_streu')} for d in liste]
    return o
