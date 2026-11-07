#!/usr/bin/env python3
"""Variante F (E13): D ohne Pixel-Mischmasch. Kein RE-Wert.

Basis D (vm_bild): Farbe je Texel aus den Screenshots, Bereiche in Kraft und Lage wie D. Geaendert:
 1. Fehlerbilder aus D wie in E behoben: Bild 2 nur mit seiner Helligkeit, Farbton aus dem groben Feld von Bild 1 (Feuerschein vorn weg);
    unsichtbare Texel aus dem 3D-Farbfeld der Ruhepose statt gespiegelt (keine Spiegelnaehte); Gruenueberschuss gekappt.
 2. Bereiche je Teil: k-Mittel im Lab-Raum (Helligkeit schwach gewichtet, damit Verlaeufe im Bereich bleiben), dann zusammenhaengende Stuecke;
    Stuecke unter MIN_TEXEL Texeln gehen an den haeufigsten Nachbarbereich (bei Bild 1 ist ein Texel rund 3,3 px gross: 6 Texel sind rund 8 x 8 px,
    kleiner liest im Spiel als Rauschen). Kanten mit Mehrheitsfilter (3 x 3, zweimal) geglaettet statt ausgefranst.
 3. Im Bereich: Farbton (a, b) = Mittel des Bereichs, Helligkeit = Helligkeit von D, innerhalb des Bereichs weich (Gauss 1,2 Texel, nur ueber
    Texel desselben Bereichs). Also nur sanfte Helligkeitsverlaeufe, keine bunten Einzeltexel.
 4. Danach wie D: Wunden je Stelle am Screenshot geeicht, Wundaufbau, Glanzpunkte, Z5, Gegenproben (vm_bild.backen mit dieser Grundtextur);
    die Wundmasken vorher geglaettet (Gauss 0,8 Texel, Kleinteile unter 3 Texel weg), damit ihre Raender sauber sind.
"""
import cv2
import numpy as np
from skimage.color import lab2rgb, rgb2lab

import vm_bild
import vm_mal
import vm_ordnung as VO

YK = np.array([.299, .587, .114], np.float32)
MIN_TEXEL = 6
K_JE_TEIL = 7
L_GEWICHT = 0.35


def projektion(form, ml, licht):
    """Wie vm_bild.aus_bild, aber Bild 2 nur mit Helligkeit (Farbton aus Bild 1 grob), unsichtbar aus dem 3D-Feld, ohne Gruen. -> rgb [H,W,3]."""
    m = ml.m
    pos = VO.ruhelage(form, ml)
    pr = VO.proben(ml, licht)
    f = VO.Feld(pos)
    t1, w1 = pr[1]
    t2, w2 = pr[2]
    q1, q2 = np.sqrt(t1 / 255.0), np.sqrt(t2 / 255.0)
    Sg, Wg = f.glaetten(pos, q1, w1, VO.FELD_SIGMA_GROSS)
    cg, wg = f.lesen(Sg, Wg, pos)
    ton1 = cg / np.maximum(wg, 1e-6)[:, None]
    y2 = q2 @ YK
    q2t = ton1 * (y2 / np.maximum(ton1 @ YK, 1e-4))[:, None]
    # je Texel wie D: gewichtetes Mittel, Bild 1 Gewicht 1, Bild 2 0,5
    w2b = 0.5 * w2
    sw = w1 + w2b
    je = (q1 * w1[:, None] + q2t * w2b[:, None]) / np.maximum(sw, 1e-6)[:, None]
    bekannt = sw > 0.15
    # unsichtbar: 3D-Feld (fein 110, Rueckfall grob), keine Spiegelung
    farbe = np.concatenate([q1, q2t])
    gew = np.concatenate([w1, w2b])
    S, W = f.glaetten(np.concatenate([pos, pos]), farbe, gew, VO.FELD_SIGMA)
    c, w = f.lesen(S, W, pos)
    fein = c / np.maximum(w, 1e-6)[:, None]
    alpha = np.clip(w / 0.05, 0, 1)[:, None]
    med = np.average(q1[w1 > 0.3], axis=0) if (w1 > 0.3).any() else np.array([.35, .3, .32])
    grob = np.where((wg > 0.02)[:, None], ton1, med[None])
    feld = fein * alpha + grob * (1 - alpha)
    q = np.where(bekannt[:, None], je, feld)
    rgb = (np.clip(q, 0, 1) ** 2) * 255.0
    y0 = rgb @ YK
    rgb[:, 1] = np.minimum(rgb[:, 1], np.maximum(rgb[:, 0], rgb[:, 2]))
    rgb *= (y0 / np.maximum(rgb @ YK, 1e-3))[:, None]
    out = np.zeros(m.shape + (3,), np.float32)
    out[m] = rgb
    bk = np.zeros(m.shape, bool)
    bk[m] = bekannt
    return out, bk


def _modus(lab_, maske, n_lab):
    """Mehrheitsfilter 3 x 3 auf Bereichsnummern, nur innerhalb der Maske."""
    zaehl = np.zeros(lab_.shape + (n_lab,), np.float32)
    for k in range(n_lab):
        zaehl[..., k] = cv2.boxFilter(((lab_ == k) & maske).astype(np.float32), -1, (3, 3), normalize=False)
    neu = zaehl.argmax(-1)
    return np.where(maske, neu, lab_)


def bereiche(ml, rgb):
    """Je Teil: Bereiche (k-Mittel in Lab), Inseln unter MIN_TEXEL an den Nachbarn, Kanten glaetten, im Bereich Farbton fest, Helligkeit weich."""
    out = rgb.copy()
    info = dict(teile=0, inseln=0)
    lab_alle = rgb2lab(np.clip(rgb, 0, 255) / 255.0)
    for i, p in enumerate(ml.teile):
        x, y, w, h = ml.lage[i]
        sub = ml.tid[y:y + h, x:x + w] == i
        n = int(sub.sum())
        if n < 12:
            continue
        L = lab_alle[y:y + h, x:x + w]
        merk = np.stack([L[..., 0] * L_GEWICHT, L[..., 1], L[..., 2]], -1)[sub].astype(np.float32)
        k = int(min(K_JE_TEIL, max(2, n // 40)))
        crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 40, 0.1)
        cv2.setRNGSeed(11)
        _, lab1, _ = cv2.kmeans(merk, k, None, crit, 3, cv2.KMEANS_PP_CENTERS)
        lab_ = np.full(sub.shape, -1, np.int32)
        lab_[sub] = lab1[:, 0]
        # Inseln zuschlagen (mehrmals, bis keine kleine mehr uebrig ist)
        for _ in range(6):
            geaendert = 0
            for c in range(k):
                nn, cc, st, _ = cv2.connectedComponentsWithStats(((lab_ == c) & sub).astype(np.uint8), connectivity=4)
                for j in range(1, nn):
                    if st[j, cv2.CC_STAT_AREA] >= MIN_TEXEL:
                        continue
                    ins = cc == j
                    rand = (cv2.dilate(ins.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0) & ~ins & sub
                    nb = lab_[rand]
                    nb = nb[(nb >= 0) & (nb != c)]
                    if nb.size:
                        lab_[ins] = np.bincount(nb, minlength=k).argmax()
                        geaendert += 1
                        info['inseln'] += 1
            if not geaendert:
                break
        for _ in range(2):
            lab_ = _modus(lab_, sub, k)
        # im Bereich: Farbton = Mittel, Helligkeit weich innerhalb des Bereichs
        Lr = L.copy()
        for c in range(k):
            s = (lab_ == c) & sub
            if not s.any():
                continue
            ab = L[s][:, 1:].mean(0)
            mm = s.astype(np.float32)
            lw = cv2.GaussianBlur(L[..., 0] * mm, (0, 0), 1.2) / np.maximum(cv2.GaussianBlur(mm, (0, 0), 1.2), 1e-3)
            Lr[s, 0] = lw[s]
            Lr[s, 1] = ab[0]
            Lr[s, 2] = ab[1]
        rr = np.clip(lab2rgb(Lr) * 255.0, 0, 255).astype(np.float32)
        out[y:y + h, x:x + w][sub] = rr[sub]
        info['teile'] += 1
    return out, info


def marken_glaetten(ml):
    """Wundmasken wie vm_ordnung.wunden_formen (Gauss 0,8, Schwelle, Kleinteile weg), Randring neu."""
    formen, alle, tief = VO.wunden_formen(ml)
    neu = []
    for (i, nm, grund, fak, s_, r), (_i2, s2) in zip(ml.marken, formen):
        r2 = ((cv2.dilate(s2.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0) & ~s2 & ml.m) if (r is not None and s2.any()) else None
        neu.append((i, nm, grund, fak, s2, r2))
    ml.marken = neu


def grund(form, ml, aus, form_json):
    licht = vm_bild.licht_bilder(aus, form_json)
    rgb, bekannt = projektion(form, ml, licht)
    sauber, info = bereiche(ml, rgb)
    # leise gemalte Form wie D (Fasern, Woelbung, Furchen schwach), ohne Korn: keine Einzeltexel
    m = ml.m
    mod = np.ones(m.shape, np.float32)
    fur = ml.furche * np.where(ml.ist_schwanz[m], 0.0, 1.0)
    mod[m] = (1 + 0.08 * (0.9 * ml.faser + 0.8 * (ml.woelb - 0.5)) - 0.15 * fur).astype(np.float32)
    sauber = np.clip(sauber * np.clip(mod, 0.5, 1.4)[..., None], 0, 255)
    info.update(texel=int(m.sum()), aus_bild=int((bekannt & m).sum()))
    return sauber, info


def backen_eichen(form, ml, aus, form_json, schreib, runden=5):
    marken_glaetten(ml)
    psx, info = grund(form, ml, aus, form_json)
    cache = dict(psx=psx, ber=info, roh=psx, bekannt=ml.m)
    return vm_bild.wunden_eichen(ml, aus, form_json, schreib, runden=runden, cache=cache, gruppen=True)
