#!/usr/bin/env python3
"""Teile-Zahlen aller Rohbauten mit gleicher Zuordnung, dazu Bildvergleich je Teil und Loecher im Umriss. Kein RE-Wert."""
import json
import os
import sys
from pathlib import Path

import cv2
import numpy as np
from scipy import ndimage as ndi

H = Path(__file__).resolve().parent
F = H.parent
sys.path.insert(0, str(F.parent.parent.parent / 're15_port' / 'tools'))
import form_pruefung as P       # noqa: E402

TEILE = {1: ('tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'), 2: ('face_and_upper_body', 'left_arm_front', 'right_arm_front')}
KURZ = dict(tail='tail', left_leg='l_leg', right_leg='r_leg', right_arm='r_arm', part_of_Body='body', face_and_upper_body='face', left_arm_front='l_arm_f', right_arm_front='r_arm_f')


def teil_zahlen():
    print('Teil gleiche Zuordnung, streng = Oberkoerper ohne Bein / bein = rechtes Bein zaehlt mit')
    for var in ('streng', 'bein'):
        print('---', var, ' IoU / Deck / Innen / Verdeckt')
        for r in 'ABC':
            j = json.load(open(H / 'mess' / (r + '_' + var) / 'teile.json', encoding='utf-8'))
            z = []
            for n in (1, 2):
                b = j['bild%d' % n]['teile']
                z.append('T%.3f m%.3f' % (j['bild%d' % n]['trefferquote'], j['bild%d' % n]['iou_mittel']))
                for t in TEILE[n]:
                    q = [x for x in b if x['teil'] == t][0]
                    z.append('%s %.2f/%.2f/%.2f/%.2f' % (KURZ[t], q['iou'], q['deckung'], q['innen'], q['verdeckt']))
            print(r, ' | '.join(z))


def loecher():
    print('--- Loecher im Umriss (eingeschlossene Nicht-Figur-Flecken ab 12 px, Bildlage)')
    for n in (1, 2):
        for r in 'ABC':
            m = cv2.imread(str(H / ('maske_%s_bild%d.png' % (r, n))), 0) > 0
            voll = ndi.binary_fill_holes(m)
            lab, k = ndi.label(voll & ~m)
            z = []
            for i in range(1, k + 1):
                yy, xx = np.nonzero(lab == i)
                if yy.size >= 12:
                    z.append('%d px um (%d,%d)' % (yy.size, xx.mean(), yy.mean()))
            print('Bild', n, r, 'Flaeche', int(m.sum()), '; '.join(z) if z else 'keine')


def struktur():
    """NCC des weichgezeichneten Luma Original gegen Einsatz je Teil. Kein Urteil, nur Anhalt: Das Bild entscheidet."""
    print('--- NCC Luma je Teil, Sigma 3 (Formen und Flecken), Einsatz gegen Original')
    for n in (1, 2):
        o = P.original(n).astype(np.float32) @ P.YK
        logo = cv2.dilate(P.logo_maske().astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
        for r in 'ABC':
            e = cv2.imread(str(H / ('eng_%s_bild%d.png' % (r, n))))[:, :, ::-1].astype(np.float32) @ P.YK
            z = []
            for t in TEILE[n]:
                w = P.teil_roh(t) & ~logo
                a, b = cv2.GaussianBlur(o, (0, 0), 3)[w], cv2.GaussianBlur(e, (0, 0), 3)[w]
                z.append('%s %.2f (Y %d/%d, Streu %d/%d)' % (KURZ[t], np.corrcoef(a, b)[0, 1], a.mean(), b.mean(), a.std(), b.std()))
            print('Bild', n, r, ' | '.join(z))


if __name__ == '__main__':
    teil_zahlen()
    loecher()
    struktur()
