# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: ABZUEGE der Abnahme am gebauten Spiel fuers Dossier.

Liest die Framedumps unter build/r30_irons-diary-welt/bau/ (r30_idw_bau_lauf.sh /
r30_idw_bau_laden.sh) und schreibt nach analysis/befunde_runde30/irons-diary-welt/:

  bau_cut2_marken.png      Nutzerbild "irons items.png" (Marken rot/blau) neben dem Framedump
                           Cut 2 (Lauf c2, F600) und dem Nullbild (c2_null), Tischausschnitt x4
  bau_cut6.png             Framedump Cut 6 mit Props (c6) und Nullbild (c6_null), Ausschnitt x2
  bau_spieler_vor_tisch.png  Cut 2, Spieler auf der Standlinie (c2_spieler) gegen das Nullbild
  bau_karte_aufheben.png   Karte: vor dem Druck, Modal, danach (karte_aufheben)
  bau_diary_aufheben.png   Diary: vor dem Druck, Leser, Meldung, danach, FILE-Liste (diary_aufheben)
"""
import os

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
BAU = os.path.join(REPO, 'build', 'r30_irons-diary-welt', 'bau')
AUS = os.path.join(REPO, 'analysis', 'befunde_runde30', 'irons-diary-welt')


def bild(lauf, b):
    return Image.open(os.path.join(BAU, lauf, 'f_%06d.ppm' % b)).convert('RGB')


def reihe(bilder, titel, h=None):
    h = h or max(b.size[1] for b in bilder)
    w = sum(b.size[0] for b in bilder) + 8 * (len(bilder) - 1)
    out = Image.new('RGB', (w, h + 16), (24, 24, 24))
    d = ImageDraw.Draw(out)
    x = 0
    for b, t in zip(bilder, titel):
        out.paste(b, (x, 16))
        d.text((x + 2, 2), t, fill=(255, 255, 0))
        x += b.size[0] + 8
    return out


def main():
    os.makedirs(AUS, exist_ok=True)
    # Cut 2 mit den Marken des Nutzers (320er Ausschnitt x 110..190, y 100..150)
    box320 = (110, 100, 190, 150)
    # das Nutzerbild ist unversioniert und liegt im Hauptbaum (nicht im Arbeitsbaum)
    kand = [os.path.join(REPO, 'irons items.png'),
            os.path.join(os.environ.get('R30_HAUPTBAUM', 'C:/workspace/git/reAi_v2'), 'irons items.png')]
    pfad = next(p for p in kand if os.path.exists(p))
    nutzer = Image.open(pfad).convert('RGB').crop(box320)
    nutzer = nutzer.resize((nutzer.size[0] * 6, nutzer.size[1] * 6), Image.NEAREST)
    box960 = tuple(v * 3 for v in box320)
    mit = bild('c2', 600).crop(box960)
    ohne = bild('c2_null', 600).crop(box960)
    mit = mit.resize((mit.size[0] * 2, mit.size[1] * 2), Image.NEAREST)
    ohne = ohne.resize((ohne.size[0] * 2, ohne.size[1] * 2), Image.NEAREST)
    reihe([nutzer, mit, ohne], ['Nutzerbild (Marken)', 'Bau Cut 2 F600', 'Nullbild (9,54)+(9,55)']
          ).save(os.path.join(AUS, 'bau_cut2_marken.png'))
    # Cut 6
    box = (380, 280, 700, 480)
    reihe([bild('c6', 600).crop(box).resize((640, 400), Image.NEAREST),
           bild('c6_null', 600).crop(box).resize((640, 400), Image.NEAREST)],
          ['Bau Cut 6 F600', 'Nullbild']).save(os.path.join(AUS, 'bau_cut6.png'))
    # Spieler vor dem Tisch
    box = (330, 250, 570, 450)
    reihe([bild('c2_spieler', 600).crop(box).resize((480, 400), Image.NEAREST),
           bild('c2_spieler_null', 600).crop(box).resize((480, 400), Image.NEAREST),
           bild('c2', 600).crop(box).resize((480, 400), Image.NEAREST)],
          ['Spieler vor dem Tisch + Props', 'Spieler, ohne Props', 'Props, Spieler weit weg']
          ).save(os.path.join(AUS, 'bau_spieler_vor_tisch.png'))
    # Aufheben
    k = [bild('karte_aufheben', b).resize((480, 360)) for b in (390, 450, 650)]
    reihe(k, ['Karte F390 vor dem Druck', 'F450 Modal 0x21', 'F650 danach']
          ).save(os.path.join(AUS, 'bau_karte_aufheben.png'))
    d = [bild('diary_aufheben', b).resize((480, 360)) for b in (390, 430, 640, 680, 800)]
    reihe(d, ['Diary F390', 'F430 Leser', 'F640 Meldung', 'F680 danach', 'F800 FILE-Liste']
          ).save(os.path.join(AUS, 'bau_diary_aufheben.png'))
    print('geschrieben nach', AUS)


if __name__ == '__main__':
    main()
