#!/usr/bin/env python3
"""Einbau K5e v0.8.30 (E32): Tafeln 08 und 09, roh (kein Gamma, Ausschnitte Nearest x3).

    python kf_k5e_f1_tafeln.py <spiel-Ordner> <aus-Ordner> --griff K5C.png F1.png --rueckstoss K5C.png F1.png

08_einbau_spiel.png: je Zeile Original-Standbild | K5c vorher | K5e_f1 Asset im echten Spielfenster (gdigrab):
  5080 Bild-1-Lage, 5080 Ruhe, 5090 Bild-2-Pose, 5090 Ruhe, 5090 Seitenansicht mit F1 (Gier 2850), Griff-Szene, Rueckstoss-Szene.
  K5c vorher = ruhe/spiel/heute_* (Asset K5c, kf_k5e_spiel.sh) bzw. RE15_RE2_ASSET_ROOT=einbau_k5c (Seite, Griff, Rueckstoss).
09_f1_rand.png: F1 nah, K5c | K5e | K5e_f1: weisses Licht 'Seite' (rl.weiss_id wie Tafel 06/07, Clip 2), Bild-1-Kette ohne Filter
  (v4f.szene_id), Spielfenster 5080 Bild-1-Lage und 5090 Seitenansicht. Ausschnitt: Offline aus den F1-Flaechen (Kennungspuffer),
  im Spielfenster das Rechteck der Pixel, in denen K5e und K5e_f1 sich unterscheiden (nur die F1-Zeile ist verschieden), +-24 px
  (5090: aus den Framedumps seite_dump_{k5e,f1}.ppm, weil das Feuer in den Fensterbildern flackert).
"""
import argparse
import json
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import riegel_k5d as R  # noqa: E402
import kf_k5d_tafeln as T  # noqa: E402

v4f = R.v4f
reihe, stapeln, speichern = T.reihe, T.stapeln, T.speichern
STAENDE = (('K5c (Spiel-Asset bis E32)', R.BASIS),   # stufe3/basis_k5c = einbau_k5c/FORM.EMD/.TIM, mit form.json fuer die Kette
           ('K5e (Vorlage 19)', os.path.join(K5, 'ruhe', 'K5e')),
           ('K5e_f1 (neues Asset, F1-Hautrand)', os.path.join(K5, 'ruhe', 'K5e_f1')))
F1 = [tuple(int(x) for x in s.split('|')) for s in
      json.load(open(os.path.join(K5, 'ruhe', 'messung', 'material.json'), encoding='utf-8'))['material']['F1']['liste']]


def lade(p):
    b = cv2.imread(p)
    if b is None:
        raise SystemExit('fehlt: %s' % p)
    return cv2.cvtColor(b, cv2.COLOR_BGR2RGB)


def box_um(m, rand, h, w):
    ys, xs = np.nonzero(m)
    return max(int(xs.min()) - rand, 0), max(int(ys.min()) - rand, 0), min(int(xs.max()) + rand + 1, w), min(int(ys.max()) + rand + 1, h)


def f1_maske(K):
    m = np.zeros(K['maske'].shape, bool)
    for me, vi, fi in F1:
        m |= (K['mesh'] == me) & (K['flaeche'] == vi * 100000 + fi)
    return m & K['maske']


def kachel(im, box, f=3):
    x0, y0, x1, y1 = box
    return cv2.resize(im[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)


def tafel09(sp, aus):
    z = []
    # weisses Licht 'Seite' (0, 8) und Bild-1-Kette; Kasten aus der K5e_f1-Kennung (gleiche Geometrie wie K5e; K5c eigene Kennung)
    for titel, fn in (('weisses Licht Seite, Clip 2', lambda o: rl.weiss_id(rl.fl.lade_var(o), 2, 0, 8)),
                      ('Bild-1-Kette ohne Filter', lambda o: v4f.szene_id(o, 1))):
        bl, ti = [], []
        for nm, o in STAENDE:
            img, K = fn(o)
            m = f1_maske(K)
            box = box_um(m, 14, *img.shape[:2]) if m.any() else (0, 0, img.shape[1], img.shape[0])
            bl.append(kachel(img, box))
            ti.append('%s: %s' % (nm, titel))
        z.append(reihe(bl, ti))
    for titel, dateien in (('Spielfenster 5080 Schnitt 1, Bild-1-Lage', (os.path.join(K5, 'ruhe', 'spiel', 'heute_pose1_nachstellung_r5080.png'),
                                                                        os.path.join(K5, 'ruhe', 'spiel', 'k5e_pose1_nachstellung_r5080.png'),
                                                                        os.path.join(sp, 'f1_pose1_nachstellung_r5080.png'))),
                           ('Spielfenster 5090 Schnitt 11, Seite (Gier 2850, Clip 2)', (os.path.join(sp, 'k5c_seite_r5090_s11.png'),
                                                                                        os.path.join(sp, 'k5e_seite_r5090_s11.png'),
                                                                                        os.path.join(sp, 'f1_seite_r5090_s11.png')))):
        b = [lade(p) for p in dateien]
        # Kasten: 5080 ohne Feuer -> Differenz der Fensterbilder K5e/K5e_f1; 5090 (Feuer flackert) -> Differenz der Framedumps desselben
        # Spielbilds 440 (RE15_FRAMEDUMP, deterministisch: kf_k5e_f1_spiel.sh Teil a 10/10 byte-gleich), seite_dump_{k5e,f1}.ppm
        q = [os.path.join(sp, 'seite_dump_k5e.ppm'), os.path.join(sp, 'seite_dump_f1.ppm')] if '5090' in titel else dateien[1:]
        qa, qb = (cv2.imread(x) for x in q)
        d = np.abs(qa.astype(int) - qb.astype(int)).max(2) > 0
        box = box_um(d, 24, *b[2].shape[:2]) if d.any() else (220, 160, 420, 320)
        z.append(reihe([kachel(x, box) for x in b], ['%s: %s (Kasten %s, %d px K5e != K5e_f1)' % (nm, titel, list(box), int(d.sum()))
                                                    for (nm, _o) in STAENDE]))
    speichern(os.path.join(aus, '09_f1_rand.png'), stapeln(z, '09 F1 nah (E32): K5c | K5e | K5e_f1 - weisses Licht, Bild-1-Kette, echtes '
                                                            'Spielfenster (gdigrab); roh, Nearest x3'))


def tafel08(sp, aus, griff, rs):
    leer = np.full((480, 640, 3), 24, np.uint8)
    S = os.path.join(K5, 'ruhe', 'spiel')
    z = []
    for titel, n, alt, neu in (('5080 Schnitt 1, Bild-1-Lage (Clip 0 Bild 11)', 1, os.path.join(S, 'heute_pose1_nachstellung_r5080.png'),
                                'f1_pose1_nachstellung_r5080.png'),
                               ('5080 Schnitt 1, Ruhe (Clip 2)', 0, os.path.join(S, 'heute_ruhe_r5080_s1.png'), 'f1_ruhe_r5080_s1.png'),
                               ('5090 Schnitt 11, Bild-2-Pose (Clip 12 Bild 0)', 2, os.path.join(S, 'heute_pose2_r5090_s11.png'),
                                'f1_pose2_r5090_s11.png'),
                               ('5090 Schnitt 11, Ruhe (Clip 2)', 0, os.path.join(S, 'heute_ruhe_r5090_s11.png'), 'f1_ruhe_r5090_s11.png'),
                               ('5090 Schnitt 11, Seite mit F1 (Gier 2850, Clip 2)', 0, os.path.join(sp, 'k5c_seite_r5090_s11.png'),
                                'f1_seite_r5090_s11.png'),
                               ('5090 Griff (Spieler in der Klaue)', 0, griff[0], griff[1]),
                               ('5090 Rueckstoss-Szene', 0, rs[0], rs[1])):
        z.append(reihe([v4f.original(n) if n else leer, lade(alt), lade(neu if os.path.exists(neu) else os.path.join(sp, neu))],
                       ['Original-Standbild Bild %d' % n if n else '(kein Original-Standbild)', 'K5c vorher: ' + titel,
                        'K5e_f1 Asset (shared_assets, ohne Umleitung): ' + titel]))
    speichern(os.path.join(aus, '08_einbau_spiel.png'), stapeln(z, '08 Einbau K5e v0.8.30 (E32): Original | K5c vorher | K5e_f1 als Spiel-Asset; '
                                                                   'echtes Fenster (gdigrab), eigene exe-Kopie, kein AUTOSHOT/SOFTWARE_RENDER, roh'))


def main():
    a = argparse.ArgumentParser()
    a.add_argument('spiel')
    a.add_argument('aus')
    a.add_argument('--griff', nargs=2, required=True)
    a.add_argument('--rueckstoss', nargs=2, required=True)
    g = a.parse_args()
    tafel09(g.spiel, g.aus)
    tafel08(g.spiel, g.aus, g.griff, g.rueckstoss)
    print(g.aus)


if __name__ == '__main__':
    main()
