#!/usr/bin/env python3
"""Bildpruefung ohne Zielgroessen der Kandidaten.

form_ncc   : Textur durch einen Ton ersetzt, nur Geometrie und Normalen unter dem Licht. Wer die Farbe aus dem Bild zurueckgerechnet hat, hat hier keinen Vorteil.
band_ncc   : NCC der Differenz zweier Gauss (1,5 und 6 px), also Bau statt Grossflaeche.
fremd      : helle Malerei im Render, die nicht an einer Sollstelle dieses Bilds sitzt.
offen      : Deckung der offenen Zonen in Bild 2 und Video dort.
Montagen fuer das Auge.
"""
import json, math, os, sys
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
sys.path.insert(0, G.TOOLS)
import form_pruefung as P
import form_video as FV
import form_buehne as FB
import geo3 as G3
ZW = os.path.join(G.FORM, 'bewertung_mass_zw')
YK = P.YK


def luma(a):
    return a.astype(np.float32) @ YK


def ncc(a, b, m):
    x, y = a[m], b[m]
    return float(np.corrcoef(x, y)[0, 1])


def flach(K):
    """Gleiche Figur, Textur einheitlich Grundhaut aus der Palette."""
    t = K.tex
    alt = t.lut.copy()
    t.lut = t.lut.copy()
    t.lut[..., 0] = 44 * 2
    t.lut[..., 1] = 18 * 2
    t.lut[..., 2] = 27 * 2
    t.lut[..., 3] = 255
    return alt


def proben(k):
    K = G3.kand(k)
    aus = {}
    sollen = {1: [], 2: []}
    for s in P.stellen():
        pm = np.zeros((480, 640), bool)
        for q in s['polygone']:
            pm |= P.poly_maske(q)
        sollen[s['bild']].append(pm)
    M = P.formblatt()['zeichnung']['materialien']
    t = float(np.array(M['rotbraun_mittelton']['oberflaeche_von'], np.float32) @ YK)
    for n in (1, 2):
        z = cv2.imread(os.path.join(G.FORM, 'ziel_bild%d.png' % n), 0)
        o = P.original(n)
        Yo = luma(o)
        s, alb, m, lk = K.rendern(n)
        f = FV.video_filter(s, bild=n, entzerren=False, rauschen=False)
        Yf = luma(f)
        w = z == 255
        d = dict(ncc=round(ncc(Yf, Yo, w), 3))
        g1 = cv2.GaussianBlur(Yo, (0, 0), 1.5) - cv2.GaussianBlur(Yo, (0, 0), 6)
        g2 = cv2.GaussianBlur(Yf, (0, 0), 1.5) - cv2.GaussianBlur(Yf, (0, 0), 6)
        d['band_ncc'] = round(ncc(g2, g1, w), 3)
        # Chroma im Weiss
        d['farbe_mittel_video'] = [round(float(x)) for x in o[w].mean(0)]
        d['farbe_mittel_render'] = [round(float(x)) for x in f[w].mean(0)]
        # fremde Malerei
        Ya = luma(alb)
        hell = m & (Ya >= t)
        er = np.zeros((480, 640), bool)
        for q in sollen[n]:
            er |= q
        er = P.weite(er, 6)
        fr = hell & ~er
        d['hell_gesamt'] = int(hell.sum())
        d['hell_fremd'] = int(fr.sum())
        if fr.any():
            d['fremd_Y_render'] = round(float(Yf[fr].mean()))
            d['fremd_Y_video'] = round(float(Yo[fr].mean()))
            # Pixel, an denen der gefilterte Render deutlich heller ist als das Video
            d['fremd_ueberstrahlt_30'] = int((fr & (Yf > Yo + 30)).sum())
        np.save(os.path.join(ZW, 'fremd_%s_%d.npy' % (k, n)), fr)
        aus[n] = d
    # Form-NCC
    alt = flach(K)
    for n in (1, 2):
        z = cv2.imread(os.path.join(G.FORM, 'ziel_bild%d.png' % n), 0)
        o = P.original(n)
        s, alb, m, lk = K.rendern(n)
        f = FV.video_filter(s, bild=n, entzerren=False, rauschen=False)
        w = z == 255
        aus[n]['form_ncc'] = round(ncc(luma(f), luma(o), w), 3)
        g1 = cv2.GaussianBlur(luma(o), (0, 0), 1.5) - cv2.GaussianBlur(luma(o), (0, 0), 6)
        g2 = cv2.GaussianBlur(luma(f), (0, 0), 1.5) - cv2.GaussianBlur(luma(f), (0, 0), 6)
        aus[n]['form_band_ncc'] = round(ncc(g2, g1, w), 3)
    K.tex.lut = alt
    return aus


def offen2(k):
    m = np.load(os.path.join(ZW, 'maske_%s_2.npy' % k))
    o = P.original(2)
    Y = luma(o)
    R = o[..., 0].astype(float)
    zonen = dict(band_210_250_376_384=P.rechteck(210, 376, 250, 384), flaeche_Y=P.scheibe(239, 418, 36), F_v_form=P.poly_maske([(263, 355), (304, 355), (290, 399), (278, 399)]),
                 feuer_rechts=P.rechteck(415, 225, 462, 300), feuer_unten_rechts=P.rechteck(415, 310, 505, 400), feuer_links=P.rechteck(160, 325, 228, 388))
    aus = {}
    for a, b in zonen.items():
        aus[a] = dict(deck=round(float(m[b].mean()), 2), R_video_gedeckt=None if not (m & b).any() else round(float(R[m & b].mean())))
    return aus


def montage(n, box, s=2):
    x0, y0, x1, y1 = box
    o = P.original(n)
    z = []
    def gross(a):
        return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
    zeile1 = [gross(o)]
    zeile2 = [gross(o)]
    for k in 'abc':
        K = G3.kand(k)
        sa, alb, m, lk = K.rendern(n)
        f = FV.video_filter(sa, bild=n, entzerren=False)
        zeile1.append(gross(f))
        u = gross(o).copy()
        q = cv2.resize(m.astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)[y0 * s:y1 * s, x0 * s:x1 * s].astype(bool)
        r = q & ~P.enge(q, 1)
        u[r] = (255, 245, 0)
        zeile2.append(u)
    b = np.vstack([np.hstack(zeile1), np.hstack(zeile2)])
    for i, t in enumerate(('Original', 'A', 'B', 'C')):
        cv2.putText(b, t, (8 + i * (x1 - x0) * s, 22), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2, cv2.LINE_AA)
    cv2.imwrite(os.path.join(ZW, 'montage_bild%d.png' % n), cv2.cvtColor(b, cv2.COLOR_RGB2BGR))


if __name__ == '__main__':
    for k in 'abc':
        p = proben(k)
        print('\n====', k.upper())
        print('Bild 1', p[1])
        print('Bild 2', p[2])
        print('offen Bild 2', offen2(k))
    montage(1, (180, 170, 450, 460))
    montage(2, (190, 140, 490, 480))
