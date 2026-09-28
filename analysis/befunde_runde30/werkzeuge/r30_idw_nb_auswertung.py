# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt, NACHBESSERUNG: Auswertung der Laeufe aus r30_idw_nb_abnahme.sh.

Konvention (korrigiert): ein 960er-Pixel c hat die stetige 320er-Koordinate (c + 0.5) / 3.
Pixel p deckt [p, p+1) - dieselbe Konvention wie die Marken (marken.txt: rot (152.5;126.5),
blau (140.0;126.5)) und wie die Projektion 160 + H*vx/vz. Die erste Abnahme rechnete
(c + 0.5)/3 - 0.5 (r30_idw_bau_auswertung.py bis zu dieser Nachbesserung) - ein halbes Pixel
Versatz in x UND y gegen die Marken.

    python r30_idw_nb_auswertung.py [bild|druck|aufheben|laden|bestand ...]
"""
import os
import re
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
BAU = os.path.join(REPO, 'build', 'r30_irons-diary-welt', 'bau')
PRUEF = os.path.join(REPO, 'build', 'r30_pruef_irons-diary-welt', 'laeufe')
ROT = dict(x0=146, x1=158, y0=120, y1=132, mx=152.5, my=126.5)
BLAU = dict(x0=135, x1=144, y0=121, y1=131, mx=140.0, my=126.5)


def lade(lauf, b, basis=BAU):
    return np.array(Image.open(os.path.join(basis, lauf, 'f_%06d.ppm' % b)).convert('RGB')).astype(int)


def diff(a, b):
    return np.abs(a - b).sum(axis=2) > 0


def huelle(maske, F, marke=None):
    ys, xs = np.nonzero(maske)
    if not len(xs):
        return '0 Pixel'
    cx = ((xs.min() + xs.max()) / 2.0 + 0.5) / 3.0
    cy = ((ys.min() + ys.max()) / 2.0 + 0.5) / 3.0
    rgb = F[ys, xs].mean(axis=0)
    s = ('%d Pixel, Huelle320 x %.2f..%.2f y %.2f..%.2f, Mitte (%.2f ; %.2f), Mittel-RGB (%.0f,%.0f,%.0f)'
         % (len(xs), xs.min() / 3.0, (xs.max() + 1) / 3.0, ys.min() / 3.0, (ys.max() + 1) / 3.0,
            cx, cy, rgb[0], rgb[1], rgb[2]))
    if marke:
        qx, qy = xs // 3, ys // 3
        inn = ((qx >= marke['x0']) & (qx <= marke['x1']) & (qy >= marke['y0']) & (qy <= marke['y1'])).sum()
        mitte_in = marke['x0'] <= cx < marke['x1'] + 1 and marke['y0'] <= cy < marke['y1'] + 1
        s += (', %d in der Marke, Mitte %s der Marke, %.2f px neben der Markenmitte (%.1f;%.1f)'
              % (inn, 'IN' if mitte_in else 'AUSSERHALB', np.hypot(cx - marke['mx'], cy - marke['my']),
                 marke['mx'], marke['my']))
    return s


def zeilen(lauf, muster):
    p = os.path.join(BAU, lauf, 'debug.log')
    if not os.path.exists(p):
        return ['(kein debug.log)']
    out = []
    for z in open(p, encoding='utf-8', errors='replace'):
        if re.search(muster, z):
            out.append(z.rstrip())
    return out


def bild():
    print('== BILD (Tuer-/Sprung-Weg, Spieler weit weg)')
    for cut in (2, 6):
        for b in (400, 500, 600):
            N = lade('nb_c%d_null' % cut, b)
            D = lade('nb_c%d_nurdiary' % cut, b)
            K = lade('nb_c%d_nurkarte' % cut, b)
            A = lade('nb_c%d' % cut, b)
            print('Cut %d F%d  beide: %d Pixel' % (cut, b, int(diff(A, N).sum())))
            print('   Diary: ' + huelle(diff(D, N), D, ROT if cut == 2 else None))
            print('   Karte: ' + huelle(diff(K, N), K, BLAU if cut == 2 else None))
    print('== Spieler vor dem Tisch (Cut 2, -22664,-18450 rot 2048)')
    for b in (400, 500, 600):
        P, P0 = lade('nb_c2', b), lade('nb_c2_null', b)
        S, S0 = lade('nb_c2_spieler', b), lade('nb_c2_spieler_null', b)
        prop = diff(P, P0)
        sp = diff(S0, P0)
        o = prop & sp
        oben = o & ~diff(S, S0)
        rest = prop & ~sp
        print('F%d: Prop %d, Spieler %d, Ueberschneidung %d: Figur obenauf %d, Prop obenauf %d; '
              'Prop ausserhalb der Figur %d, davon mit Spieler sichtbar %d'
              % (b, int(prop.sum()), int(sp.sum()), int(o.sum()), int(oben.sum()),
                 int((o & diff(S, S0)).sum()), int(rest.sum()), int((rest & diff(S, S0)).sum())))


def druck():
    print('== DRUCK (Viereck F400, Stand -22664,z, Blick -X)')
    for z in (-18900, -18800, -18720, -18649, -18463, -18462, -18275, -17950):
        zz = zeilen('nb_druck_%d' % z, r'\[irons-tisch\] F4[01]\d')
        erst = [x for x in zz if 'modal=1' in x or 'menue=1' in x]
        was = '-'
        if erst:
            e = erst[0]
            was = ('KARTE (Modal)' if 'modal=1' in e else 'BUCH (Leser)') + ' ab ' + e.split()[1]
        print('z %6d: %s' % (z, was))
        for x in zz[:3]:
            print('     ' + x)


def aufheben():
    print('== AUFHEBEN (Cut 6 erzwungen)')
    for lauf in ('nb_karte_aufheben', 'nb_diary_aufheben'):
        print(lauf)
        for x in zeilen(lauf, r'\[irons-tisch\] F|filed|FILE|liste'):
            print('   ' + x)
    # Prop-Feld aus den Einzel-Laeufen Cut 6 (Spieler weit weg), verglichen mit dem Nullbild
    # an DERSELBEN Spielerlage
    N6 = lade('nb_c6_null', 600)
    feld_k = diff(lade('nb_c6_nurkarte', 600), N6)
    feld_d = diff(lade('nb_c6_nurdiary', 600), N6)
    for lauf, null, feld, name, bilder in (
            ('nb_karte_aufheben', 'nb_karte_null_gleichpos', feld_k, 'Kartenfeld', (390, 650)),
            ('nb_diary_aufheben', 'nb_diary_null_gleichpos', feld_d, 'Buchfeld', (390, 820))):
        for b in bilder:
            try:
                F, N = lade(lauf, b), lade(null, b)
            except FileNotFoundError as e:
                print('   %s F%d: fehlt (%s)' % (lauf, b, e))
                continue
            d = diff(F, N)
            print('   %s F%d gegen %s: im %s (%d Pixel) %d abweichend, im ganzen Bild %d'
                  % (lauf, b, null, name, int(feld.sum()), int((d & feld).sum()), int(d.sum())))


def laden():
    print('== LADE-WEG (CONTINUE, Cut 2 erzwungen)')
    for r in ('1150', '1151'):
        for b in (100, 200, 300):
            F, N = lade('nb_laden' + r, b), lade('nb_laden%s_gen' % r, b)
            d = diff(F, N)
            print('ROOM%s F%d: %d Pixel gegen den Lauf mit beidem genommen' % (r, b, int(d.sum())))
            spalte320 = (np.arange(d.shape[1]) // 3)[None, :]   # Trennung zwischen den Marken: 145
            print('   rot (x320 >= 145): ' + huelle(d & (spalte320 >= 145), F, ROT))
            print('   blau (x320 < 145): ' + huelle(d & (spalte320 < 145), F, BLAU))
        for lauf in ('nb_laden%s' % r, 'nb_laden%s_gen' % r, 'nb_laden%s_karte' % r,
                     'nb_laden%s_diary' % r):
            print(lauf)
            for x in zeilen(lauf, r'CONTINUE: resumed|Boot-Weg|\[irons-tisch\] F')[:8]:
                print('   ' + x)


def bestand():
    print('== BESTAND')
    for r in ('1140', '1100', '1110'):
        for b in (400, 600):
            F = lade('nb_neu_' + r, b)
            for ref in ('neu_' + r, 'basis_' + r):
                try:
                    R = lade(ref, b)
                except FileNotFoundError:
                    continue
                print('ROOM%s F%d gegen %s: %d Pixel' % (r, b, ref, int(diff(F, R).sum())))
    for nb, ref in (('nb_c2_null', 'basis_c2'), ('nb_c6_null', 'basis_c6'),
                    ('nb_c2_spieler_null', 'basis_c2_spieler')):
        for b in (400, 600):
            try:
                print('%s F%d gegen %s (exe de5ae65a, vor dem Bau): %d Pixel'
                      % (nb, b, ref, int(diff(lade(nb, b), lade(ref, b)).sum())))
            except FileNotFoundError as e:
                print('%s F%d: fehlt (%s)' % (nb, b, e))


if __name__ == '__main__':
    teile = sys.argv[1:] or ['bild', 'druck', 'aufheben', 'laden', 'bestand']
    for t in teile:
        globals()[t]()
