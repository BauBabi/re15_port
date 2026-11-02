#!/usr/bin/env python3
"""Variante G (E14): F, aber jede Zeichnung in genau den Farben, die der Screenshot dort zeigt. Kein RE-Wert.

E14 hebt die pauschale Wundregel aus E10 auf (Fleischrot, dunkelrote Tiefen, Glanz in jeder hellen Stelle): Z5 bekam so rote Flecken, die der Screenshot
nicht hat. Hier:
 - Haut wie F (vm_sauber.grund: Bereiche aus der Projektion, Inseln weg, Kanten glatt, kein Feuerschein vorn, keine Spiegelung, kein Gruen).
 - Zeichnung (Z1-Z9, Q, G, P, H, Schwanzende X): je Texel die Projektion aus DEM Bild, in dem die Stelle liegt (rf_marken: Bild 1 oder 2; Teile von G,
   die Bild 1 als Z5-Umgebung sieht, aus Bild 1), Schirmfarbe / Port-Licht wie D. Unsichtbare Markentexel: Mittel der sichtbaren derselben Stelle.
   Innerhalb der Stelle Gauss 1,0 Texel nur ueber Texel derselben Stelle: Verlauf bleibt (Z5 oben hell), Rauschen der Projektion geht.
 - Helligkeit je Stelle wie D am Screenshot geeicht (vm_bild.wunden_eichen), aber OHNE gemalten Wundaufbau (vm_bild.wunden aus).
 - Z5: nur der Rand in der Spanne helle_baender (Pruefpunkte kappe_*); die ganze Stelle zu klemmen machte sie olivgrau, flach und im Filter zu
   hell (Lauf _probe/T1). Gegenproben N1/N2 wie bisher.
"""
import cv2
import numpy as np

import fb_mal as FM
import vm_bild
import vm_ordnung as VO
import vm_sauber

YK = np.array([.299, .587, .114], np.float32)


def marken_bild(ml):
    """id -> Bild (1/2) aus rf_marken.liste; '_z5'-Teile von G aus Bild 1."""
    nb = {i: n for i, n, _mk, _f in FM.liste()}
    aus = {}
    for i, nm, grund, fak, s_, r in ml.marken:
        aus[i] = 1 if i.endswith('_z5') else nb.get(i, 1)
    return aus


def zeichnung_aus_bild(ml, licht, rgb):
    m = ml.m
    pr = VO.proben(ml, licht)
    T = {}
    for n in (1, 2):
        t, w = pr[n]
        tt = np.zeros(m.shape + (3,), np.float32)
        ww = np.zeros(m.shape, np.float32)
        tt[m] = t
        ww[m] = w
        T[n] = (tt, ww)
    bild = marken_bild(ml)
    out = rgb.copy()
    info = {}
    for i, nm, grund, fak, s_, r in ml.marken:
        if not s_.any():
            continue
        n = bild[i]
        tt, ww = T[n]
        sicht = s_ & (ww > 0.15)
        if not sicht.any():
            # Stelle in ihrem Bild nicht sichtbar (verdeckt): anderes Bild, sonst bleibt die Haut von F
            n2 = 3 - n
            tt, ww = T[n2]
            sicht = s_ & (ww > 0.15)
            if not sicht.any():
                continue
        mittel = tt[sicht].mean(0)
        c = np.where(sicht[..., None], tt, mittel[None, None])
        mm = s_.astype(np.float32)
        gl = cv2.GaussianBlur(c * mm[..., None], (0, 0), 1.0) / np.maximum(cv2.GaussianBlur(mm, (0, 0), 1.0), 1e-3)[..., None]
        out[s_] = gl[s_]
        info[i] = dict(bild=n, texel=int(s_.sum()), sichtbar=int(sicht.sum()), mittel=[round(float(x)) for x in mittel])
    return np.clip(out, 0, 255), info


def backen_eichen(form, ml, aus, form_json, schreib, runden=5):
    vm_sauber.marken_glaetten(ml)
    psx, info = vm_sauber.grund(form, ml, aus, form_json)
    licht = vm_bild.licht_bilder(aus, form_json)
    psx, zinfo = zeichnung_aus_bild(ml, licht, psx)
    info['zeichnung'] = zinfo
    cache = dict(psx=psx, ber=info, roh=psx, bekannt=ml.m)
    return vm_bild.wunden_eichen(ml, aus, form_json, schreib, runden=runden, cache=cache, wunden_an=False, z5_nur_rand=True)
