# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt, NACHBESSERUNG: Abzuege der erneuten Abnahme (Laeufe nb_* aus
r30_idw_nb_abnahme.sh) nach analysis/befunde_runde30/irons-diary-welt/:

  nb_cut2_marken.png         Nutzerbild (Marken) | Bau Cut 2 F600 | Nullbild, Tischausschnitt
  nb_cut6.png                Cut 6 mit Props | Nullbild
  nb_spieler_vor_tisch.png   Spieler vor dem Tisch + Props | ohne Props | Props, Spieler weit weg
  nb_druck_reihe.png         Bild 420 nach dem Druck in F400 je Standort z (Karte = Modal,
                             Buch = Leser)
  nb_karte_aufheben.png      Karte: vor dem Druck, Modal, danach (Cut 6, Stand z -18900)
  nb_diary_aufheben.png      Buch: vor dem Druck, Leser, Meldung, danach, FILE-Liste (Cut 6)
"""
import os

from PIL import Image, ImageDraw

import r30_idw_bau_abzuege as ab

AUS = ab.AUS


def main():
    os.makedirs(AUS, exist_ok=True)
    box320 = (110, 100, 190, 150)
    kand = [os.path.join(ab.REPO, 'irons items.png'),
            os.path.join(os.environ.get('R30_HAUPTBAUM', 'C:/workspace/git/reAi_v2'), 'irons items.png')]
    pfad = next(p for p in kand if os.path.exists(p))
    nutzer = Image.open(pfad).convert('RGB').crop(box320)
    nutzer = nutzer.resize((nutzer.size[0] * 6, nutzer.size[1] * 6), Image.NEAREST)
    box960 = tuple(v * 3 for v in box320)
    mit = ab.bild('nb_c2', 600).crop(box960)
    ohne = ab.bild('nb_c2_null', 600).crop(box960)
    mit = mit.resize((mit.size[0] * 2, mit.size[1] * 2), Image.NEAREST)
    ohne = ohne.resize((ohne.size[0] * 2, ohne.size[1] * 2), Image.NEAREST)
    ab.reihe([nutzer, mit, ohne], ['Nutzerbild (Marken)', 'Nachbesserung Cut 2 F600',
                                   'Nullbild (9,54)+(9,55)']).save(os.path.join(AUS, 'nb_cut2_marken.png'))
    box = (380, 280, 700, 480)
    ab.reihe([ab.bild('nb_c6', 600).crop(box).resize((640, 400), Image.NEAREST),
              ab.bild('nb_c6_null', 600).crop(box).resize((640, 400), Image.NEAREST)],
             ['Cut 6 F600', 'Nullbild']).save(os.path.join(AUS, 'nb_cut6.png'))
    box = (330, 250, 570, 450)
    ab.reihe([ab.bild('nb_c2_spieler', 600).crop(box).resize((480, 400), Image.NEAREST),
              ab.bild('nb_c2_spieler_null', 600).crop(box).resize((480, 400), Image.NEAREST),
              ab.bild('nb_c2', 600).crop(box).resize((480, 400), Image.NEAREST)],
             ['Spieler vor dem Tisch + Props', 'Spieler, ohne Props', 'Props, Spieler weit weg']
             ).save(os.path.join(AUS, 'nb_spieler_vor_tisch.png'))
    zs = (-18900, -18800, -18720, -18649, -18463, -18462, -18275, -17950)
    bl = [ab.bild('nb_druck_%d' % z, 420).resize((320, 240)) for z in zs]
    ab.reihe(bl, ['z %d' % z for z in zs]).save(os.path.join(AUS, 'nb_druck_reihe.png'))
    k = [ab.bild('nb_karte_aufheben_18900', b).resize((480, 360)) for b in (390, 450, 650)]
    ab.reihe(k, ['Karte F390 vor dem Druck (Stand z -18900)', 'F450 Modal', 'F650 danach']
             ).save(os.path.join(AUS, 'nb_karte_aufheben.png'))
    d = [ab.bild('nb_diary_aufheben', b).resize((480, 360)) for b in (390, 430, 640, 680, 820)]
    ab.reihe(d, ['Buch F390', 'F430 Leser', 'F640 Meldung', 'F680 danach', 'F820 FILE-Liste']
             ).save(os.path.join(AUS, 'nb_diary_aufheben.png'))
    print('geschrieben nach', AUS)


if __name__ == '__main__':
    main()
