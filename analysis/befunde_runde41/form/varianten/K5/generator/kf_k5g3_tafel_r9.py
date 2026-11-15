#!/usr/bin/env python3
"""R9-Einbau: Zusatztafeln zur rechten Hand (roh, kein Gamma).
    python kf_k5g3_tafel_r9.py <K5g3-Ordner (neu)> <K5g3-Ordner Runde 3 (vorher)> <Spiel-Ordner> <aus-Ordner>
12_rechte_hand.png   Bild 2 (Szenenkette wie Tafel 02, Ausschnitt Krallen vorn x2): Vorlage | K5g3 Runde 3 (K5c-Haltung) | K5g3 + R9 (Variante 1);
                     Spielkamera (echtes Fenster, gdigrab, kf_k5g3_fenster.sh): 5090 Schnitt 11 Bild-2-Pose und 5080 von vorn, ganzes Bild + Hand x3
13_bewegung.png      Filmstreifen weisses Licht (Tafel-07-Licht), gleiche Blickrichtung je Zeile: die Clips, in denen der Einbau 17/18 aendert
                     (Clip 9 N52/Boden-Fenster, Clip 10 Rampe + Fenster, Clip 12 Ausblendung, Clip 27 Fenster) - oben Runde 3 (K5c-Haltung), unten R9.
"""
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
reihe, stapeln, kachel, speichern = T.reihe, T.stapeln, T.kachel, T.speichern
RD = rl.R          # render.py (blick, kf)


def modell(d):
    emd = open(os.path.join(d, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim = open(os.path.join(d, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    mo = rl.fl.FE.modell(emd, tim)
    mo.tim = tim
    mo.T = rl.FB.Textur(tim)
    return mo


def bild(mo, clip, b, az, el, gross=300):
    x = RD.blick(mo, RD.kf(mo, clip, b), az, el, gross, None)
    return x[0] if isinstance(x, tuple) else x


def main():
    neu, alt, sp, aus = sys.argv[1:5]
    os.makedirs(aus, exist_ok=True)
    lade = lambda p: cv2.cvtColor(cv2.imread(os.path.join(sp, p)), cv2.COLOR_BGR2RGB)  # noqa: E731
    st = {'K5g3 Runde 3 (K5c-Haltung)': R.Stand(alt), 'K5g3 + R9 (Variante 1)': R.Stand(neu)}
    vor = v4f.original(2)
    box = (150, 290, 500, 480)
    z = []
    for art, txt in (('sauber', 'Disc-Licht x k, ohne Filter'), ('video', 'Disc-Licht x k + Videofilter Saat 15')):
        bilder = [kachel(vor, box, 2)] + [kachel(s.d[2]['sauber'] if art == 'sauber' else s.d[2]['video'][15], box, 2) for s in st.values()]
        z.append(reihe(bilder, ['Vorlage Bild 2, Krallen vorn x2'] + ['%s: %s' % (k, txt) for k in st]))
    for titel, datei, hb in (('5090 Schnitt 11, Bild-2-Pose (Clip 12 Bild 0)', 'pose2_r5090_s11.png', (170, 300, 450, 470)),
                             ('5080 Schnitt 1, von vorn (Clip 12 Bild 0)', 'vorn_r5080.png', (200, 280, 480, 460))):
        a, b = lade('r3_' + datei), lade('r9_' + datei)
        z.append(reihe([a, b], ['K5g3 Runde 3 im Spielfenster: ' + titel, 'K5g3 + R9 im Spielfenster: ' + titel]))
        z.append(reihe([kachel(a, hb, 2), kachel(b, hb, 2)], ['Runde 3: Hand x2', 'R9: Hand x2']))
    speichern(os.path.join(aus, '12_rechte_hand.png'),
              stapeln(z, 'Rechte Hand (Variante 1) in Bild 2 und in der Spielkamera: vorher (Runde 3) | nachher (R9-Einbau), roh, kein Gamma'))
    mods = {'Runde 3': modell(alt), 'R9': modell(neu)}
    zeilen = []
    for titel, clip, bilder, az, el in (('Clip 9 (N52-/Boden-Fenster Keys 309-334)', 9, (6, 9, 12, 16, 20, 23, 25, 28, 30), 45, 15),
                                        ('Clip 10 (Rampe 398-425, Fenster 440-448)', 10, (31, 38, 45, 52, 58, 65, 80, 84, 88), 45, 15),
                                        ('Clip 12 (Bild 2 = Bild 0, Ausblendung 1-14)', 12, (0, 2, 4, 6, 8, 10, 12, 14, 16), 45, 15),
                                        ('Clip 27 (Fenster Bild 29-37 und 55-62)', 27, (29, 31, 33, 35, 37, 56, 58, 60, 62), 45, 15)):
        for k, mo in mods.items():
            zeilen.append(reihe([bild(mo, clip, b, az, el, 220) for b in bilder], ['%s %s Bild %d' % (k, 'Clip %d' % clip, b) for b in bilder]))
        zeilen[-2] = reihe([zeilen[-2]], [titel])
    speichern(os.path.join(aus, '13_bewegung.png'),
              stapeln(zeilen, 'Bewegung: je Clip oben Runde 3 (K5c-Haltung), unten R9-Einbau; weisses Licht, 3/4 vorn, roh'))
    print('Tafeln 12/13 fertig:', aus)


if __name__ == '__main__':
    main()
