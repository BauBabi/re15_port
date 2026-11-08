#!/usr/bin/env python3
"""Versuch 3 Runde 5: Tafel 10_krallen_r3_r4_r5 und Tupfen-Zaehlung an den Vorderkrallen.

Zeilen (Krallen-Ausschnitt, gleicher Ausschnitt je Zeile, roh, kein Gamma, gleiches Licht je Zeile):
  Z.1 Bild 2: Original (Video roh) | Runde 3 | Runde 4 | Runde 5 unter Disc-Licht 5090 Cut 11 (wie Tafel 02 Z.1)
  Z.2 Bild 2: Original (Video roh) | Runde 3 | Runde 4 | Runde 5 unter Disc-Licht + Videofilter (Saat 15, wie Tafel 02 Z.2)
  Z.3 Spielfenster 5090 Schnitt 11 (Pose Bild 2): Original-Standbild | Runde 3 | Runde 4 | Runde 5 (echtes Fenster, gdigrab, farbe/v3r3,
      farbe/v3r4, farbe/spiel_v3), Ausschnitt um die Krallen laut Kennungs-Spielbild (--kennung-spiel, Krallen gruen)
Tupfen (Sicht-Pruefer Runde 4: "Pixel heller als das Umfeld um > 10 L*, Zusammenhangskomponenten"; eigene Umsetzung, fuer alle Spalten gleich):
  Umfeld = Mittel-L* der Krallenmaske im 9 x 9-Fenster (normierte Box), Tupfen = Maske & (L* - Umfeld > 10), Komponenten 8-Nachbarschaft.
  Maske = Krallenpixel aus dem Kennungs-Render (Bild 2) bzw. dem Kennungs-Spielbild (5090), fuer alle Staende und das Original dieselbe.
    python kf_tafel10_krallen.py <aus-Ordner> [--kennung-spiel <png>]
"""
import argparse
import json
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_tafeln as KT  # noqa: E402
import riegel_farbe_v3 as RV  # noqa: E402

H, PZ = KT.H, KT.PZ
K5 = KT.K5
sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V3R5_skripte'))
import kralle_rv as KR  # noqa: E402

STAENDE = [('Runde 3', os.path.join(K5, 'farbe', 'K5c_farbe_v3r3'), os.path.join(K5, 'farbe', 'v3r3', 'spiel_v3')),
           ('Runde 4', os.path.join(K5, 'farbe', 'K5c_farbe_v3r4'), os.path.join(K5, 'farbe', 'v3r4', 'spiel_v3')),
           ('Runde 5', os.path.join(K5, 'farbe', 'K5c_farbe_v3'), os.path.join(K5, 'farbe', 'spiel_v3'))]
SPIEL = 'spiel_pose2_r5090_s11.png'
UMFELD = 9          # Fenster des Umfeld-Mittels (px); Sicht-Pruefer nennt kein Fenster, 9 px = gut doppelte Krallenbreite im Bild 2 (gemessen 3-5 px)
TUPFEN_DL = 10.0    # Sicht-Pruefer Runde 4: "heller als das Umfeld um > 10 L*"
RAND = 8            # Ausschnitt: Bbox der Krallenmaske + 8 px


def lstern(rgb):
    return cv2.cvtColor(rgb.astype(np.float32) / 255.0, cv2.COLOR_RGB2LAB)[..., 0]


def tupfen(rgb, maske):
    L = lstern(rgb)
    m = maske.astype(np.float32)
    s = cv2.boxFilter(L * m, -1, (UMFELD, UMFELD), normalize=False)
    c = cv2.boxFilter(m, -1, (UMFELD, UMFELD), normalize=False)
    um = s / np.maximum(c, 1e-6)
    t = maske & (L - um > TUPFEN_DL)
    n, _l = cv2.connectedComponents(t.astype(np.uint8), connectivity=8)
    return dict(px=int(t.sum()), komponenten=int(n - 1), maske_px=int(maske.sum()), anteil=round(100.0 * t.sum() / max(maske.sum(), 1), 2))


def bbox(m):
    ys, xs = np.nonzero(m)
    return max(0, ys.min() - RAND), min(m.shape[0], ys.max() + RAND + 1), max(0, xs.min() - RAND), min(m.shape[1], xs.max() + RAND + 1)


def zeile(bilder, maske, titel, unter, f):
    y0, y1, x0, x1 = bbox(maske)
    ks, werte = [], []
    for (nm, img), u in zip(bilder, unter):
        w = tupfen(img, maske)
        werte.append((nm, w))
        a = cv2.resize(img[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)
        ks.append(KT.kachel(a, '%s, %s' % (nm, titel), 'Tupfen %d px / %d Komponenten (%s %% der Krallenpixel)' % (
            w['px'], w['komponenten'], ('%.2f' % w['anteil']).replace('.', ',')), u))
    return H.reihe(ks), werte


def main():
    a = argparse.ArgumentParser()
    a.add_argument('aus')
    a.add_argument('--kennung-spiel', default=None)
    x = a.parse_args()
    os.makedirs(x.aus, exist_ok=True)
    kr = KR.kennung(STAENDE[2][1], os.path.join(x.aus, '_kennung_tmp'))[2]
    o = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild2.png'), cv2.COLOR_BGR2RGB)
    roh, vid = [('Original Bild 2', o)], [('Original Bild 2', o)]
    for nm, d, _sp in STAENDE:
        k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
        s, alb, m, _k = k.rendern(2)
        roh.append((nm, s))
        vid.append((nm, np.asarray(PZ.FV.video_filter(s, bild=2, entzerren=False, rauschen=True, saat=RV.SAAT), np.uint8)))
    z1, w1 = zeile(roh, kr, 'Bild 2 Krallen', ['Video roh'] + ['Disc-Licht 5090 Cut 11, roh'] * 3, 4)
    z2, w2 = zeile(vid, kr, 'Bild 2 Krallen', ['Video roh'] + ['Disc-Licht + Videofilter Saat 15, roh'] * 3, 4)
    zeilen, erg = [z1, z2], {'Bild 2 Disc-Licht roh': w1, 'Bild 2 + Videofilter': w2}
    if x.kennung_spiel and os.path.exists(x.kennung_spiel):
        ks = cv2.cvtColor(cv2.imread(x.kennung_spiel), cv2.COLOR_BGR2RGB)
        ks = cv2.resize(ks, (640, 480), interpolation=cv2.INTER_NEAREST) if ks.shape[:2] != (480, 640) else ks
        mk = (ks[..., 1].astype(int) - np.maximum(ks[..., 0], ks[..., 2]).astype(int) > 60)     # Markerfarbe (0,255,0) unter Spiellicht
        sp = [('Original Bild 2', o)]
        for nm, _d, spd in STAENDE:
            b = cv2.cvtColor(cv2.imread(os.path.join(spd, SPIEL)), cv2.COLOR_BGR2RGB)
            sp.append((nm, cv2.resize(b, (640, 480), interpolation=cv2.INTER_NEAREST) if b.shape[:2] != (480, 640) else b))
        z3, w3 = zeile(sp, mk, 'Spielfenster 5090', ['Video roh (Standbild)'] + ['echtes Fenster gdigrab, roh'] * 3, 4)
        zeilen.append(z3)
        erg['Spielfenster 5090'] = w3
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('10 Vorderkrallen: Original | Runde 3 | Runde 4 | Runde 5, je Zeile gleicher Ausschnitt und gleiches Licht, roh, kein Gamma. '
                       'Z.1 Disc-Licht, Z.2 + Videofilter, Z.3 echtes Spielfenster 5090. Tupfen: L* > Umfeld (9 px) + 10 in der Krallenmaske.', t.shape[1])
    cv2.imwrite(os.path.join(x.aus, '10_krallen_r3_r4_r5.png'), H.stapel([kopf, t]))
    json.dump(erg, open(os.path.join(x.aus, 'krallen_tupfen.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    for k_, v in erg.items():
        print(k_, [(n, w['px'], w['komponenten'], w['anteil']) for n, w in v])
    import shutil
    shutil.rmtree(os.path.join(x.aus, '_kennung_tmp'), ignore_errors=True)


if __name__ == '__main__':
    main()
