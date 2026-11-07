#!/usr/bin/env python3
"""Tafel 11 Einbau v0.8.30 (E30): je Zeile Original-Standbild | K4 vorher im Spiel (Bilder der Tafel 07, farbe/spiel_v3/heute_*) |
Spiel-Asset neu im Spiel (form_spiel.sh OHNE --emd/--tim: das Spiel laedt shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM). Alle Spielbilder roh,
echtes Fenster, kein Gamma, kein AUTOSHOT, kein SOFTWARE_RENDER.

    python kf_tafel11_einbau.py <spiel-Ordner neu> <aus.png> [--heute <Ordner mit heute_*.png>] [--verfahren TEXT]

Zeilen (Posen im 28-Clip-Asset: Bild 1 = Clip 0 Bild 11, Bild 2 = Clip 12 Bild 0, Ruhe = Clip 2 Bild 0; STUFE3.md 3):
  ROOM5080 Schnitt 1, Nachstellung Bild 1 (Lage -20711,-15984, Gier 756; fb_spiel.sh) | ROOM5080 Schnitt 1 Ruhe |
  ROOM5090 Schnitt 11, Pose Bild 2 (Lage 941,-23776, Gier 3874) | ROOM5090 Schnitt 11 Ruhe.
"""
import argparse
import os
import sys

import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_tafeln as KT  # noqa: E402

H, PZ = KT.H, KT.PZ
ZEILEN = (('einbau_pose1_nachstellung_r5080.png', 'heute_spiel_pose1_nachstellung_r5080.png', 1,
           'ROOM5080 Schnitt 1, Nachstellung Bild 1 (Clip 0 Bild 11)', 'ROOM5080 Schnitt 1, Nachstellung Bild 1 (Clip 0)'),
          ('einbau_ruhe_r5080_s1.png', 'heute_spiel_ruhe_r5080_s1.png', 1, 'ROOM5080 Schnitt 1, Ruhe (Clip 2 Bild 0)', 'ROOM5080 Schnitt 1, Ruhe (Clip 2)'),
          ('einbau_pose2_r5090_s11.png', 'heute_spiel_pose2_r5090_s11.png', 2, 'ROOM5090 Schnitt 11, Pose Bild 2 (Clip 12 Bild 0)',
           'ROOM5090 Schnitt 11, Pose Bild 2 (Clip 1 = Ruhe-Platzhalter)'),
          ('einbau_ruhe_r5090_s11.png', 'heute_spiel_ruhe_r5090_s11.png', 2, 'ROOM5090 Schnitt 11, Ruhe (Clip 2 Bild 0)', 'ROOM5090 Schnitt 11, Ruhe (Clip 2)'))


def main():
    a = argparse.ArgumentParser()
    a.add_argument('spiel')
    a.add_argument('aus')
    a.add_argument('--heute', default=os.path.join(KT.K5, 'farbe', 'spiel_v3'))
    a.add_argument('--verfahren', default='gdigrab')
    g = a.parse_args()
    zeilen = []
    for neu, alt, n, t_neu, t_alt in ZEILEN:
        pn, pa = os.path.join(g.spiel, neu), os.path.join(g.heute, alt)
        o = cv2.imread(PZ.FORM + '/bild%d.png' % n)
        bn, ba = cv2.imread(pn), cv2.imread(pa)
        if bn is None:
            print('fehlt:', pn)
            continue
        ks = [o, ba, bn]
        ks = [cv2.resize(k, (640, 480), interpolation=cv2.INTER_NEAREST) if k is not None and k.shape[:2] != (480, 640)
              else (k.copy() if k is not None else cv2.imread(pn) * 0) for k in ks]
        for k, t in zip(ks, ('Original Bild %d (Video-Standbild)' % n, 'K4 vorher im Spiel (Tafel 07): ' + t_alt,
                             'Spiel-Asset neu (K5c V3 R5, 16 Zeilen): ' + t_neu)):
            H.schrift(k, t, 6, 18, 0.45)
            H.schrift(k, 'echtes Fenster (%s), roh, kein Gamma' % g.verfahren, 6, 36, 0.4, (200, 230, 255))
        zeilen.append(H.reihe(ks))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('11 Einbau v0.8.30 (E30): Original | K4 vorher (Tafel 07, RE15_RE2_ASSET_ROOT-Lauf K4) | Spiel-Asset neu aus '
                       'shared_assets/RE2 (form_spiel.sh ohne Umleitung, eigene exe-Kopie, echtes Fenster, kein AUTOSHOT, kein SOFTWARE_RENDER).',
                       t.shape[1])
    cv2.imwrite(g.aus, H.stapel([kopf, t]))
    print(g.aus)


if __name__ == '__main__':
    main()
