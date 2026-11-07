#!/usr/bin/env python3
"""Form bemalt, Stellschleife der Marken (aus rf_stell.py, mit Glaetter und Maler fb_*): Farbe je Marke so, dass die Schirmfarbe im Render (Licht `mess`) der zurueckgerechneten Schirmfarbe des Videos entspricht.
Grund: L0 steht in Bild 1 fast senkrecht, Oberseiten bekommen bis Faktor 1, Flaechen zur Kamera rund 0,4. Palette = Textur bei mittlerem Faktor. python rf_stell.py STAND.json [Runden]"""
import json
import os
import sys

import cv2
import numpy as np

import fb_bau as rf_bau
import fb_mal
import rf_plan
import rf_textur
from rf_kern import HIER
import form_pruefung as P


def messen(k, liste):
    aus = {}
    bild = {n: k.rendern(n) for n in (1, 2)}
    ec = {n: rf_textur.echt(n) for n in (1, 2)}
    for i, n, m, f in liste:
        s = m > 0
        s &= bild[n][2]
        if s.sum() < 6:
            aus[i] = None
            continue
        e_soll = ec[n][s].mean(0)
        e_ist = bild[n][0][s].astype(np.float32).mean(0)
        aus[i] = (float(e_soll @ P.YK), float(e_ist @ P.YK), int(s.sum()))
    return aus


def haut_stellen(k, li):
    """Haut je Knochen: Median der Schirmhelligkeit im Render (Licht mess) gegen farbe_bild*_echt.png auf Figurpixeln ohne Zeichnung. Faktor ^0.8 je Runde,
    Grenze 0,7 bis 1,8 (m5: 2,2 trieb die Haut aus der Palette). Knochen in beiden Bildern: Mittel. Knochen ohne Bild: Mittel aller."""
    import rf_kern
    hp = os.path.join(HIER, 'zw', 'haut.json')
    hg = json.load(open(hp)) if os.path.exists(hp) else {}
    ms, logo = rf_kern.masken()
    q = {}
    for n in (1, 2):
        sauber, alb, m, _ = k.rendern(n)
        e = rf_textur.echt(n)
        zeich = np.zeros(m.shape, bool)
        for i, nn, mk, f in li:
            if nn == n:
                zeich |= cv2.dilate(mk.astype(np.uint8), np.ones((7, 7), np.uint8)) > 0
        kb = kennbild_knochen(k, n)
        fig = np.zeros(m.shape, bool)
        for t in rf_kern.TEILE[n]:
            fig |= ms[t]
        s = m & fig & ~zeich & ~logo
        s = cv2.erode(s.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
        Ye, Yr = e @ P.YK, sauber.astype(np.float32) @ P.YK
        for b in np.unique(kb[s]):
            if b < 0:
                continue
            ss = s & (kb == b)
            if ss.sum() < 25:
                continue
            q.setdefault(int(b), []).append(float(np.median(Ye[ss]) + 2) / float(np.median(Yr[ss]) + 2))
    alle = [np.mean(v) for v in q.values()]
    mit = float(np.median(alle)) if alle else 1.0
    for b in range(len(k.mo.meshes)):
        r = float(np.mean(q[b])) if b in q else mit
        hg[str(b)] = float(np.clip(hg.get(str(b), 1.0) * r ** 0.8, 0.7, 1.8))
    json.dump(hg, open(hp, 'w'), indent=0)
    print('Haut', {b: round(v, 2) for b, v in hg.items()})


def ton_stellen(k, li):
    """Farbton der Haut je Kanal: gefiltertes Render (video_filter, ohne Rauschen) gegen das Original ueber der Figur ohne Zeichnung, beide Bilder,
    geometrisches Mittel, ^0.6 je Runde, Grenze 0,6 bis 1,6."""
    import rf_kern
    import form_video as FV
    tp = os.path.join(HIER, 'zw', 'hautton.json')
    t = np.array(json.load(open(tp)) if os.path.exists(tp) else [1.0, 1.0, 1.0])
    ms, logo = rf_kern.masken()
    r = []
    for n in (1, 2):
        sauber, alb, m, _ = k.rendern(n)
        gef = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False).astype(np.float32)
        o = P.original(n).astype(np.float32)
        zeich = np.zeros(m.shape, bool)
        for i, nn, mk, f in li:
            if nn == n:
                zeich |= cv2.dilate(mk.astype(np.uint8), np.ones((7, 7), np.uint8)) > 0
        fig = np.zeros(m.shape, bool)
        for tt in rf_kern.TEILE[n]:
            fig |= ms[tt]
        s = fig & m & ~logo & ~zeich
        r.append((o[s].mean(0) + 5) / (gef[s].mean(0) + 5))
        print('Ton Bild', n, 'Original', np.round(o[s].mean(0)).astype(int), 'Filter', np.round(gef[s].mean(0)).astype(int))
    # Nur Bild 1 bestimmt den Ton: Bild 2 hat Rot-Einstreuung bis +21 bei dunklen Stoffen und Gruen mit 0,665 gedaempft (farbe.md 3, 4); m5 mit beiden Bildern
    # trieb Gruen auf 1,59 (Haut grau). Helligkeit bleibt bei haut_stellen: Faktor auf Y = 1 normiert.
    q = r[0] / float(r[0] @ P.YK)
    t = np.clip(t * q ** 0.6, 0.6, 1.6)
    t = t / float(t @ P.YK)
    json.dump(t.tolist(), open(tp, 'w'))
    print('Hautton', np.round(t, 3))


def kennbild_knochen(k, n):
    """Knochen je Pixel: jeder Knochen allein gerendert, der vorderste nach Malerfolge (nach Tiefe) ist naeherungsweise der zuletzt gezeichnete."""
    import copy
    import form_buehne as FB
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    tr = []
    orig = k.mo.meshes
    for b in range(len(orig)):
        ms_ = []
        for i, mm in enumerate(orig):
            if i != b:
                mm = copy.copy(mm)
                mm.tf, mm.qf = [], []
            ms_.append(mm)
        k.mo.meshes = ms_
        for x in FB.dreiecke(k.mo, kf, lage, gier, kam, None):
            tr.append((x[0], x[1], b))
    k.mo.meshes = orig
    bild_ = np.full((P.HOEHE, P.BREITE), -1, np.int32)
    for z, p, b in sorted(tr, key=lambda t: -t[0]):
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), kam.abbildung)
        pts = np.round(np.stack([x, y], 1) * 8).astype(np.int32)
        cv2.fillConvexPoly(bild_, pts, int(b), lineType=cv2.LINE_8, shift=3)
    return bild_


def main():
    stand = json.load(open(os.path.join(HIER, sys.argv[1])))
    runden = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    g, teile = rf_plan.plan()
    pf = os.path.join(HIER, 'zw', 'marken.json')
    ks = json.load(open(pf)) if os.path.exists(pf) else {}
    li = fb_mal.liste()
    rf_bau.delta_laden()
    for r in range(runden):
        form, lage, dichte, L, C = rf_bau.baue_form(g, teile, stand)
        rgb, bk = fb_mal.backen(form, g, teile, lage, stand, L=L, C=C)
        rf_bau.schreibe(form, rgb, HIER, g, stand)
        k = P.Kandidat(os.path.join(HIER, 'form.json'))
        z = messen(k, li)
        print('Runde', r, ' '.join('%s %.0f/%.0f' % (i, v[0], v[1]) for i, v in z.items() if v))
        for i, v in z.items():
            if v is None or i in ('Gg',):
                continue
            q = (v[0] + 2.0) / max(v[1], 2.0)
            # Grenzen je Marke: Spanne der Palette (Pruefung Material) nur dort verlassen, wo das Licht von Bild 1 es erzwingt
            hi = dict(Z2=1.4, Q4=1.4, Q1=1.5, Q2=1.25, Q3=1.5, H8=1.5, H9=1.5, Z6a=1.25).get(i, 1.6)      # Z6a: ueber 1,25 faellt sie aus dem Material helle_baender (Abgabe 17:18: 1,54, Material 0,00; vor Abnahme 2: 1,22, Material 1,00)
            lo = dict(Z3=.95, Z4=1.06, Z6a=1.06, Z5=1.03, Z8=1.15).get(i, .3)      # m29: Z8 bei 1,09 Material 0,60 (Kante zur Haut, Quantisierung)      # Abnahme 2: Z4 liegt auf dem Arm jetzt heller, darunter faellt sie aus dem Material. Z5: bei 0,91 Material 0,00 (Lauf s7_final), bei 1,03 0,40
            ks[i] = float(np.clip(ks.get(i, 1.0) * q ** .8, lo, hi))
        json.dump(ks, open(pf, 'w'), indent=0)
        haut_stellen(k, li)
        ton_stellen(k, li)
    print({k_: round(v, 2) for k_, v in ks.items()})


if __name__ == '__main__':
    main()
