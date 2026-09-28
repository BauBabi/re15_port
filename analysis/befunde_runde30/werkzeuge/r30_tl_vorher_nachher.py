# -*- coding: utf-8 -*-
"""Runde 30 Nachschliff tischlicht: VORHER (exe cac33993) gegen NACHHER (exe mit der Wahl
Lichtsatz Cut 2 fuer obj 5/6) - was hat sich im Bild geaendert, und NUR das?

Laeufe (r30_tl_lauf.sh):
  basis_c<N>[_null]     Ausgangs-exe, Cut N, Spieler weit weg      (Zensus)
  n_c<N>[_null]         neue exe, dito
  v_c<N>_sp[_null]      Ausgangs-exe, Spieler im Bild (Cut 2: -22664,-18450 rot 2048 vor dem
                        Tisch; Cut 6: -22664,-18649 rot 2048, Kopf ueber der Karte; Cut 4:
                        -17600,-19400 rot 3072 - in Cut 4 ist in keiner begehbaren Lage eine Figur
                        zu sehen, dort wird das ganze Bild verglichen)
  n_c<N>_sp[_null]      neue exe, dito
  mess_c6_nurdiary/_nurkarte   Mess-exe, Bild 450 = Lichtsatz 2 (Kandidat L2)
  n_c6_nurdiary/_nurkarte      neue exe

    python r30_tl_vorher_nachher.py [abzug]
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
LAEUFE = os.path.join(REPO, 'build', 'r30_tischlicht', 'laeufe')
ABZUG = os.path.join(REPO, 'analysis', 'befunde_runde30', 'irons-diary-welt')


def lade(lauf, b):
    return np.array(Image.open(os.path.join(LAEUFE, lauf, 'f_%06d.ppm' % b)).convert('RGB')).astype(int)


def diff(a, b):
    return np.abs(a - b).sum(axis=2) > 0


def luma(rgb):
    return 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2]


def vergleich():
    print('== Spieler weit weg, alle Cuts: VORHER (basis) gegen NACHHER (n)')
    for c in range(9):
        for b in (400, 500, 600):
            V, N = lade('basis_c%d' % c, b), lade('n_c%d' % c, b)
            d = diff(V, N)
            prop = diff(V, lade('basis_c%d_null' % c, b))
            print('Cut %d F%d: %d Pixel verschieden, davon ausserhalb der Prop-Pixel %d'
                  % (c, b, int(d.sum()), int((d & ~prop).sum())))
    for c in (2, 6):
        for b in (400, 500, 600):
            print('Nullbild Cut %d F%d vorher/nachher: %d Pixel verschieden'
                  % (c, b, int(diff(lade('basis_c%d_null' % c, b), lade('n_c%d_null' % c, b)).sum())))
    print('== Spieler im Bild (Cut 2 / 4 / 6)')
    for c in (2, 4, 6):
        for b in (400, 500, 600):
            V, N = lade('v_c%d_sp' % c, b), lade('n_c%d_sp' % c, b)
            V0, N0 = lade('v_c%d_sp_null' % c, b), lade('n_c%d_sp_null' % c, b)
            F0 = lade('basis_c%d_null' % c, b) if c != 4 else lade('basis_c4', b)
            spieler = diff(V0, F0)          # Figur = Nullbild mit Figur gegen Nullbild ohne
            prop = diff(V, V0)              # Props (vorher) = mit gegen ohne Props
            prop_n = diff(N, N0)
            d = diff(V, N)
            print('Cut %d F%d: Figur %d Pixel, davon verschieden %d | ganzes Bild verschieden %d, '
                  'ausserhalb der Prop-Pixel %d | Nullbild mit Figur vorher/nachher %d | '
                  'Prop-Pixel vorher %d nachher %d'
                  % (c, b, int(spieler.sum()), int((d & spieler).sum()), int(d.sum()),
                     int((d & ~(prop | prop_n)).sum()), int(diff(V0, N0).sum()),
                     int(prop.sum()), int(prop_n.sum())))
    print('== Mittel-RGB der Props nachher (Spieler weit weg), gegen den Kandidaten L2 der Mess-exe')
    for c in (2, 6):
        N0 = lade('n_c%d_null' % c, 500)
        for prop, name in (('nurdiary', 'Buch'), ('nurkarte', 'Karte')):
            for lauf, b, null in (('n_c%d_%s' % (c, prop), 500, N0),
                                  ('mess_c%d_%s' % (c, prop), 450, lade('mess_c%d_null' % c, 450)),
                                  ('mess_c%d_%s' % (c, prop), 530, lade('mess_c%d_null' % c, 530))):
                F = lade(lauf, b)
                m = diff(F, null)
                rgb = F[m].mean(axis=0)
                print('Cut %d %-5s %-22s F%d: %5d Pixel Mittel-RGB (%.1f,%.1f,%.1f) Y %.1f'
                      % (c, name, lauf, b, int(m.sum()), rgb[0], rgb[1], rgb[2], luma(rgb)))
            same = int(diff(lade('n_c%d_%s' % (c, prop), 500), lade('mess_c%d_%s' % (c, prop), 450)).sum())
            print('   neue exe F500 gegen Mess-exe Lichtsatz 2 (F450): %d Pixel verschieden' % same)


def abzug():
    """Vorher/Nachher nebeneinander, Cut 2 und Cut 6 (Spieler weit weg, Bild 500)."""
    def kachel(lauf, box, z, titel):
        im = Image.open(os.path.join(LAEUFE, lauf, 'f_000500.ppm')).convert('RGB').crop(box)
        im = im.resize((im.width * z, im.height * z), Image.NEAREST)
        out = Image.new('RGB', (im.width, im.height + 16), (24, 24, 24))
        out.paste(im, (0, 16))
        ImageDraw.Draw(out).text((4, 2), titel, fill=(255, 255, 0))
        return out
    for c, box, z in ((6, (390, 240, 900, 500), 1), (2, (360, 330, 600, 420), 2)):
        a = kachel('basis_c%d' % c, box, z, 'Cut %d VORHER (Lichtsatz Cut %d, RDT @0x%05X)'
                   % (c, c, 0x398 + 40 * c))
        b = kachel('n_c%d' % c, box, z, 'Cut %d NACHHER (Lichtsatz Cut 2, RDT @0x003E8)' % c)
        n = kachel('n_c%d_null' % c, box, z, 'Cut %d Nullbild (gemalt, ohne Props)' % c)
        w = a.width + b.width + n.width + 8
        o = Image.new('RGB', (w, a.height), (24, 24, 24))
        o.paste(a, (0, 0)); o.paste(b, (a.width + 4, 0)); o.paste(n, (a.width + b.width + 8, 0))
        p = os.path.join(ABZUG, 'tischlicht_cut%d_vorher_nachher.png' % c)
        o.save(p)
        print('Abzug', p, o.size)
    # ganze Bilder 320x240 (Cut 2 / Cut 6, vorher / nachher)
    reihe = []
    for c in (2, 6):
        for lauf, t in (('basis_c%d' % c, 'VORHER'), ('n_c%d' % c, 'NACHHER')):
            im = Image.open(os.path.join(LAEUFE, lauf, 'f_000500.ppm')).convert('RGB').resize((320, 240), Image.BOX)
            k = Image.new('RGB', (320, 256), (24, 24, 24)); k.paste(im, (0, 16))
            ImageDraw.Draw(k).text((4, 2), 'Cut %d %s' % (c, t), fill=(255, 255, 0))
            reihe.append(k)
    o = Image.new('RGB', (4 * 320 + 12, 256), (24, 24, 24))
    for i, k in enumerate(reihe):
        o.paste(k, (i * 324, 0))
    p = os.path.join(ABZUG, 'tischlicht_uebersicht.png')
    o.save(p)
    print('Abzug', p, o.size)


if __name__ == '__main__':
    if 'abzug' in sys.argv[1:]:
        abzug()
    else:
        vergleich()
