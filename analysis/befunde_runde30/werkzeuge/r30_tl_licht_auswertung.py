# -*- coding: utf-8 -*-
"""Runde 30 Nachschliff tischlicht: welcher VORHANDENE Lichtsatz von ROOM1150/1151 bringt Irons
Diary (obj 5) und Memory Card (obj 6) am naechsten an die GEMALTE Umgebung?

Laeufe (r30_tl_lauf.sh, Mess-exe mit r30_tl_messhaken.patch):
    mess_c<C>_null       RE15_FORCE_CUT=C, Bits (9,54)/(9,55): keine Props (Nullbild)
    mess_c<C>_nurdiary   nur das Buch (Bit 55 gesetzt), RE15_IRONS_LICHT_MESS=400/20
    mess_c<C>_nurkarte   nur die Karte (Bit 54 gesetzt), dito
Messhaken: ab Bild 400 nehmen obj 5/6 den Lichtsatz ((bild-400)/20) % 9 -> Bild 410+20*L traegt
Lichtsatz L (L = 0..8). Alle anderen Objekte, der Spieler und das Bild bleiben unberuehrt.

ZIELMASS (festgelegt VOR der Auswertung der Kandidaten):
  Referenz = die gemalten Gegenstaende auf dem Tisch, gemessen im NULLBILD desselben Cuts:
    K = das gemalte Klemmbrett (Brett samt Papier),  B = das gemalte Buch (Deckel des Stapels).
  Masken: EIN Satz Polygone, die Ecken in Cut 6 abgelesen (320er-Koordinaten, Nullbild
  mess_c6_null), ueber den Sehstrahl auf die Tischplatte y = -1520 gelegt (Dossier 2.3, zwei
  Verfahren) und mit derselben Engine-Matrix nach Cut 2 projiziert (r30_idw_geom.py,
  kamera_1150.txt). Ein 960er-Pixel c zaehlt, wenn seine 320er-Mitte (c+0.5)/3 in dem um
  SCHRUMPF zur Polygonmitte verkleinerten Polygon liegt (gegen Randpixel des Holzes).
  Spanne je Kanal = [min(K,B), max(K,B)] der Mittel-RGB.
  Abstand eines Props = euklidischer RGB-Abstand seines Mittel-RGB zur Spanne (0 = innerhalb);
  Wahl = kleinste Summe ueber beide Props und beide Cuts (2 und 6 - die einzigen Cuts, in
  denen die Props zu sehen sind, r30_tl_zensus.py); bei Gleichstand der kleinere Abstand zur
  Mitte der Spanne.

    python r30_tl_licht_auswertung.py [schrumpf ...]     (Standard 0.10; Robustheit: 0 0.1 0.25)
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
LAEUFE = os.path.join(REPO, 'build', 'r30_tischlicht', 'laeufe')
AUSG = os.path.join(REPO, 'build', 'r30_tischlicht')
sys.path.insert(0, HIER)
os.environ.setdefault('R30_IDW_AUS', 'C:/workspace/git/reAi_v2/build/r30_irons-diary-welt')
from r30_idw_geom import lade_kameras, projiziere, auf_ebene  # noqa: E402

TISCH_Y = -1520.0
# Ecken in Cut 6 (320er, Nullbild mess_c6_null, abgelesen am 6-fach vergroesserten 5-px-Raster),
# im Uhrzeigersinn ab oben links.
K6 = [(176.3, 105.8), (211.3, 105.3), (219.7, 149.7), (181.7, 150.3)]
B6 = [(244.7, 101.3), (284.2, 116.3), (275.0, 152.0), (235.0, 135.0)]
LICHT = list(range(9))


def bild_l(L):
    return 410 + 20 * L


def lade(lauf, b):
    return np.array(Image.open(os.path.join(LAEUFE, lauf, 'f_%06d.ppm' % b)).convert('RGB')).astype(int)


def polygone():
    cams = lade_kameras()
    out = {6: {'K': K6, 'B': B6}, 2: {}}
    for name, P in (('K', K6), ('B', B6)):
        q = []
        for sx, sy in P:
            w, _ = auf_ebene(cams[6], sx, sy, TISCH_Y)
            p = projiziere(cams[2], w)
            q.append((p[0], p[1]))
        out[2][name] = q
    return out


def innen(P, x, y):
    """Punkt-in-Polygon (Strahlverfahren), P = Liste (x,y)."""
    n = len(P)
    ins = np.zeros(x.shape, bool)
    j = n - 1
    for i in range(n):
        xi, yi = P[i]
        xj, yj = P[j]
        c = ((yi > y) != (yj > y)) & (x < (xj - xi) * (y - yi) / (yj - yi + 1e-12) + xi)
        ins ^= c
        j = i
    return ins


def maske(P, schrumpf, shape=(720, 960)):
    m = np.mean(np.array(P), axis=0)
    Q = [tuple(m + (1.0 - schrumpf) * (np.array(p) - m)) for p in P]
    yy, xx = np.mgrid[0:shape[0], 0:shape[1]]
    return innen(Q, (xx + 0.5) / 3.0, (yy + 0.5) / 3.0), Q


def luma(rgb):
    return 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2]


def abstand_spanne(rgb, lo, hi):
    d = np.maximum(lo - rgb, 0) + np.maximum(rgb - hi, 0)
    return float(np.sqrt((d ** 2).sum()))


def referenz(schrumpf, overlay=False, null_lauf='mess_c%d_null', null_bild=490):
    polys = polygone()
    ref = {}
    for c in (2, 6):
        N = lade(null_lauf % c, null_bild)
        r = {}
        for name in ('K', 'B'):
            m, Q = maske(polys[c][name], schrumpf)
            r[name] = (N[m].mean(axis=0), int(m.sum()), Q)
        lo = np.minimum(r['K'][0], r['B'][0])
        hi = np.maximum(r['K'][0], r['B'][0])
        ref[c] = dict(r=r, lo=lo, hi=hi, mid=(lo + hi) / 2.0)
        if overlay:
            im = Image.fromarray(N.astype(np.uint8))
            d = ImageDraw.Draw(im)
            for name, col in (('K', (255, 255, 0)), ('B', (0, 255, 255))):
                Q = r[name][2]
                d.polygon([(x * 3, y * 3) for x, y in Q], outline=col)
            im.save(os.path.join(AUSG, 'referenzmasken_c%d.png' % c))
    return ref


def drucke_referenz(ref, schrumpf):
    print('== Referenz (Nullbild, Schrumpf %.2f)' % schrumpf)
    for c in (2, 6):
        R = ref[c]
        for name, lang in (('K', 'Klemmbrett'), ('B', 'Buch')):
            rgb, n, Q = R['r'][name]
            print('Cut %d %-10s %5d Pixel  Mittel-RGB (%5.1f,%5.1f,%5.1f)  Y %5.1f  Polygon %s'
                  % (c, lang, n, rgb[0], rgb[1], rgb[2], luma(rgb),
                     ' '.join('(%.1f,%.1f)' % q for q in Q)))
        print('Cut %d Spanne R %.1f..%.1f  G %.1f..%.1f  B %.1f..%.1f   Y %.1f..%.1f'
              % (c, R['lo'][0], R['hi'][0], R['lo'][1], R['hi'][1], R['lo'][2], R['hi'][2],
                 luma(R['lo']), luma(R['hi'])))


def prop_mittel(F, N):
    m = np.abs(F - N).sum(axis=2) > 0
    rgb = F[m].mean(axis=0) if m.any() else np.array([np.nan] * 3)
    return rgb, int(m.sum())


def auswertung(schrumpf, overlay=False):
    ref = referenz(schrumpf, overlay)
    drucke_referenz(ref, schrumpf)
    tab = {}
    for L in LICHT:
        b = bild_l(L)
        zeile = {}
        for c in (2, 6):
            N = lade('mess_c%d_null' % c, b)
            for prop in ('nurdiary', 'nurkarte'):
                rgb, n = prop_mittel(lade('mess_c%d_%s' % (c, prop), b), N)
                R = ref[c]
                zeile[(c, prop)] = (rgb, n, abstand_spanne(rgb, R['lo'], R['hi']),
                                    float(np.sqrt(((rgb - R['mid']) ** 2).sum())))
        tab[L] = zeile
    print('== Kandidaten: Mittel-RGB der Props je Lichtsatz L (Bild 410+20L), d = Abstand zur '
          'Spanne (0 = innerhalb)')
    for L in LICHT:
        z = tab[L]
        summe = sum(v[2] for v in z.values())
        mitte = sum(v[3] for v in z.values())
        teile = []
        for c in (2, 6):
            for prop, k in (('nurdiary', 'Buch'), ('nurkarte', 'Karte')):
                rgb, n, d, dm = z[(c, prop)]
                teile.append('C%d %s (%3.0f,%3.0f,%3.0f) Y%3.0f n=%d d=%.1f'
                             % (c, k, rgb[0], rgb[1], rgb[2], luma(rgb), n, d))
        print('L%d: Summe d %.1f | Summe Mitte %.1f | %s' % (L, summe, mitte, ' | '.join(teile)))
    rang = sorted(LICHT, key=lambda L: (round(sum(v[2] for v in tab[L].values()), 3),
                                        sum(v[3] for v in tab[L].values())))
    print('RANG (Summe d, dann Summe Mitte): ' + ' '.join('L%d' % L for L in rang))
    # ZWEITES MASS = das Wort des Auftrags: "mittlere HELLIGKEIT des gemalten Klemmbretts/der
    # gemalten Buecher als Zielmass". Helligkeit Y = 0.299 R + 0.587 G + 0.114 B (Rec. 601) des
    # Mittel-RGB; Spanne = [Y(Buch), Y(Klemmbrett)]; dY = Abstand zur Spanne (0 = innerhalb),
    # mit Vorzeichen: negativ = dunkler als das dunkelste gemalte Objekt.
    print('== Helligkeit Y: Props je Lichtsatz, dY = vorzeichenbehafteter Abstand zur Spanne')
    for c in (2, 6):
        R = ref[c]
        print('Cut %d Spanne Y %.1f (Buch) .. %.1f (Klemmbrett)'
              % (c, luma(R['r']['B'][0]), luma(R['r']['K'][0])))
    def dy(y, lo, hi):
        return y - lo if y < lo else (y - hi if y > hi else 0.0)
    ysum = {}
    for L in LICHT:
        teile, s = [], 0.0
        for c in (2, 6):
            R = ref[c]
            lo = min(luma(R['r']['B'][0]), luma(R['r']['K'][0]))
            hi = max(luma(R['r']['B'][0]), luma(R['r']['K'][0]))
            for prop, k in (('nurdiary', 'Buch'), ('nurkarte', 'Karte')):
                y = luma(tab[L][(c, prop)][0])
                d = dy(y, lo, hi)
                s += abs(d)
                teile.append('C%d %s Y %.1f dY %+.1f' % (c, k, y, d))
        ysum[L] = s
        print('L%d: Summe |dY| %.1f | %s' % (L, s, ' | '.join(teile)))
    print('RANG Helligkeit (Summe |dY|): ' + ' '.join('L%d' % L for L in sorted(LICHT, key=lambda L: ysum[L])))
    return ref, tab


if __name__ == '__main__':
    sch = [float(a) for a in sys.argv[1:]] or [0.10]
    for i, s in enumerate(sch):
        auswertung(s, overlay=(i == 0))
