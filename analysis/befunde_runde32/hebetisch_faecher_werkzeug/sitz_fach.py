# -*- coding: utf-8 -*-
"""R32 — Sitz je Gegenstand in seinem UNTEREN Fach: exakte Lage-/Durchstoss-Pruefung + Schirm-Planung.

Pruefung (exakt, alle Modellpunkte des MD1, Drehung wie die Engine mit Q12-sin/cos):
  * tiefster Punkt == Fachboden (liegt AUF dem Boden, nicht darin, nicht darueber)
  * alle Punkte im offenen Fach-Inneren: Abstand vorn (x=2), hinten (x=-1258), Wand z0, Wand z1,
    Decke > 0 -> kein Durchstoss durch Boden/Waende/Decke/Rueckwand, Gegenstand ganz im Fach
    (Fach-Kasten ist konvex, Gegenstaende konvexe Huelle der Punkte -> Punktpruefung reicht)
  * die beiden Gegenstaende in VERSCHIEDENEN Faechern (Trennwand z 861..950 dazwischen) ->
    keine Ueberschneidung; Abstand der Huellen in z ausgegeben
Schirm (NUR PLANUNG, Float-Rasterer mit z-Puffer ueber Prop 0 + Deckel + beide Gegenstaende;
MASSGEBLICH ist die Framedump-Messung): sichtbare Punkte und Schwerpunkt je Gegenstand.

sitz_fach.py Gx Gz Gry Sx Sz Sry [py ...]
"""
import math, os, sys
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
sys.path.insert(0, os.path.join(H, '..', '..', 'befunde_runde31', 'hebetisch_werkzeug'))
import faecher as FA  # noqa
import r31_geo as G  # noqa
import r31_raster as RR  # noqa

SIN_TAB = None


def sin_q12(a):
    """rcossin-Naeherung: Float-sin gerundet (Vielfache von 512 exakt genug fuer die Planung;
    die C-Riegel rechnen mit re15_sin_q12/re15_cos_q12 der Engine)."""
    return int(round(math.sin((a & 4095) * 2 * math.pi / 4096) * 4096))


def dreh_q12(p, ry):
    s, c = sin_q12(ry), sin_q12(ry + 1024)
    return ((p[0] * c + p[2] * s) / 4096.0, float(p[1]), (-p[0] * s + p[2] * c) / 4096.0)


def modell(name):
    if name == 'G':
        return G.inc_md1('granate_prop.inc', 're15_granate_md1')
    return G.inc_md1('sicherung_prop.inc', 're15_sicherung_md1')


HALB_Y = {'G': 55, 'S': 26}   # Modell-Huelle y (granate_prop.inc -55..55, sicherung_prop.inc -26..26)


def lage(name, fach, x, z, ry):
    m = modell(name)
    y = fach['boden'] - HALB_Y[name]
    w = [tuple(a + b for a, b in zip(dreh_q12(p, ry), (x, y, z))) for p in m['tv']]
    xs = [p[0] for p in w]; ys = [p[1] for p in w]; zs = [p[2] for p in w]
    return dict(pos=(x, y, z), ry=ry, welt=w,
                tief=max(ys) - fach['boden'],
                vorn=fach['x_vorn'] - max(xs), hinten=min(xs) - fach['x_hinten'],
                wand0=min(zs) - fach['z0'], wand1=fach['z1'] - max(zs), decke=min(ys) - fach['decke'],
                bbox=(min(xs), max(xs), min(ys), max(ys), min(zs), max(zs)))


def bericht(name, L):
    ok = abs(L['tief']) < 1e-6 and min(L['vorn'], L['hinten'], L['wand0'], L['wand1'], L['decke']) > 0
    return ('%s Sitz %s rot_y %d: tiefster Punkt %+.2f zum Boden | Abstand vorn %.1f hinten %.1f Wand-z0 %.1f '
            'Wand-z1 %.1f Decke %.1f | Huelle x %.1f..%.1f y %.1f..%.1f z %.1f..%.1f | %s'
            % (name, L['pos'], L['ry'], L['tief'], L['vorn'], L['hinten'], L['wand0'], L['wand1'], L['decke'],
               *L['bbox'], 'IM FACH, LIEGT AUF' if ok else 'VERLETZT'))


if __name__ == '__main__':
    a = [int(v) for v in sys.argv[1:7]]
    pys = [int(v) for v in sys.argv[7:]] or [-1205]
    for raum in ('ROOM1150.RDT', 'ROOM1151.RDT'):
        fa, _ = FA.faecher(raum)
        A, B = fa[(96, 861)], fa[(950, 1715)]
        LG = lage('G', A, a[0], a[1], a[2])
        LS = lage('S', B, a[3], a[4], a[5])
        print(raum)
        print('  ' + bericht('Granate  (Fach A)', LG))
        print('  ' + bericht('Sicherung(Fach B)', LS))
        print('  Abstand der Huellen in z: %.1f (Trennwand %d..%d dazwischen)' % (LS['bbox'][4] - LG['bbox'][5], A['z1'], B['z0']))
    sz = RR.Szene('ROOM1150.RDT')
    for py in pys:
        idb = sz.render(py, 150, (a[3], LS['pos'][1], a[4], a[5]), (a[0], LG['pos'][1], a[1], a[2]))
        e = sz.auswerten(idb)
        print('PLANUNG Plattform y=%d: Granate %d Punkte Schwerpunkt-x %s bbox %s | Sicherung %d Punkte Schwerpunkt-x %s bbox %s'
              % (py, e['G'][0], None if e['G'][1] is None else round(e['G'][1], 1), e['G'][2],
                 e['S'][0], None if e['S'][1] is None else round(e['S'][1], 1), e['S'][2]))
