#!/usr/bin/env python3
"""Beschriftete Uebersichtsbilder zum Formblatt: formblatt_bild1.png, formblatt_bild2.png.
Polygone: lesung_gesamtbau.json, Kamera: kamera_bildN.json, Rechnung: FORMBLATT.md.

    python analysis/befunde_runde41/form/werkzeug/formblatt_bilder.py
"""
import json
import os
import sys

import numpy as np
from PIL import Image, ImageDraw, ImageFont

FORM = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.insert(0, os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools')))
import form_kamera as FK  # noqa: E402

S = 2
LEG = 600
FARBE = {'s': (60, 255, 90), 'w': (255, 225, 60), 'o': (255, 150, 40), 'n': (70, 225, 255),
         'z': (255, 90, 255), 'r': (255, 70, 70)}
NAME = {'s': 'sicher', 'w': 'wahrscheinlich', 'o': 'offen', 'n': 'NICHT Figur (Boden, Schatten, Feuer)',
        'z': 'Zeichnung', 'r': 'Beleg Verdeckung'}


def schrift(g, fett=False):
    return ImageFont.truetype('C:/Windows/Fonts/arialbd.ttf' if fett else 'C:/Windows/Fonts/arial.ttf', g)


def grund(name, gamma):
    a = np.array(Image.open(os.path.join(FORM, name)).convert('RGB')).astype(float) / 255
    return Image.fromarray((a ** gamma * 255).astype('uint8')).resize((640 * S, 480 * S), Image.BICUBIC)


def xy(p):
    return [(float(a) * S, float(b) * S) for a, b in p]


def text(d, p, t, f=(255, 255, 255), g=14, anker='mm'):
    d.text((p[0] * S, p[1] * S), t, font=schrift(g, True), fill=f, anchor=anker, stroke_width=2, stroke_fill=(0, 0, 0))


def poly(d, p, art, breite=2):
    d.line(xy(p) + xy(p[:1]), fill=FARBE[art], width=breite, joint='curve')


def gestrichelt(d, p, art, breite=2, an=7, aus=5):
    q = xy(p)
    for a, b in zip(q, q[1:]):
        a, b = np.array(a, float), np.array(b, float)
        L = np.linalg.norm(b - a)
        if L < 1:
            continue
        e = (b - a) / L
        s = 0.0
        while s < L:
            d.line([tuple(a + e * s), tuple(a + e * min(s + an, L))], fill=FARBE[art], width=breite)
            s += an + aus


def ring(p):
    return list(p) + list(p[:1])


def kreuz(d, p, art, r=6):
    x, y = p[0] * S, p[1] * S
    d.line([(x - r, y - r), (x + r, y + r)], fill=FARBE[art], width=2)
    d.line([(x - r, y + r), (x + r, y - r)], fill=FARBE[art], width=2)


def teile(bild):
    j = json.load(open(os.path.join(FORM, 'lesung_gesamtbau.json'), encoding='utf-8'))
    return {t['name']: t for t in j['teile'] if t['bild'] == bild}


def legende(w, kopf, zeilen):
    d = ImageDraw.Draw(w)
    x, y = 640 * S + 16, 14
    d.text((x, y), kopf, font=schrift(20, True), fill=(255, 255, 255))
    y += 32
    for art in 'swonzr':
        d.rectangle([x, y + 3, x + 12, y + 15], fill=FARBE[art])
        d.text((x + 18, y), NAME[art], font=schrift(13), fill=(225, 225, 225))
        y += 18
    y += 8
    for z in zeilen:
        if z is None:
            y += 6
            continue
        kz, t, art = z
        d.text((x, y), kz, font=schrift(13, True), fill=FARBE.get(art, (255, 255, 255)))
        d.text((x + 48, y), t, font=schrift(13), fill=(235, 235, 235))
        y += 17
    return y


def draufsicht(w, oy, kopf, punkte, linien, hoehe=215):
    """Kleine Draufsicht im Koerperrahmen: u nach oben, v nach links."""
    d = ImageDraw.Draw(w)
    x0, y0 = 640 * S + 16, oy
    d.text((x0, y0), kopf, font=schrift(13, True), fill=(255, 255, 255))
    ox, oyy = x0 + 240, y0 + 26 + hoehe // 2
    m = 0.07
    d.rectangle([x0, y0 + 22, x0 + 560, y0 + 26 + hoehe], outline=(90, 90, 90))
    for a, b, art in linien:
        d.line([(ox - a[1] * m, oyy - a[0] * m), (ox - b[1] * m, oyy - b[0] * m)], fill=FARBE[art], width=1)
    for u, v, t, art in punkte:
        px, py = ox - v * m, oyy - u * m
        d.ellipse([px - 4, py - 4, px + 4, py + 4], fill=FARBE[art])
        d.text((px + 7, py - 7), t, font=schrift(12), fill=(235, 235, 235))


def bild1():
    k = FK.lade_kamera(1)
    w = Image.new('RGB', (640 * S + LEG, 480 * S), (18, 18, 20))
    w.paste(grund('bild1.png', 0.75).convert('RGB'), (0, 0))
    d = ImageDraw.Draw(w)
    t = teile(1)
    for n, art in [('rumpf_kuppel', 's'), ('rechte_beule', 'w'), ('pfeiler_A', 's'), ('fuss_A', 'w'), ('saeule_B', 's'),
                   ('kugel_B_oben', 's'), ('kugel_B_unten', 'w'), ('innenglied_B', 'w'), ('bein_C_oben', 's'),
                   ('untermasse_C', 'w'), ('rundling_C', 'w')]:
        poly(d, t[n]['polygon'], art)
    for n in ('masse_unter_B', 'streifenfeld_unten', 'rosa_feld_unten'):
        gestrichelt(d, ring(t[n]['polygon']), 'o')
    for n in ('luecke_boden_1', 'luecke_boden_2'):
        poly(d, t[n]['polygon'], 'n')
    gestrichelt(d, ring(t['blauzone_kein_koerper']['polygon']), 'n')
    for n in ('rosa_oval', 'lichtband_1', 'lichtfleck_2', 'lichtband_3', 'dreieck_gelenk_A', 'querband_C', 'kappe_und_winkel_B', 'schraegstrich_beule'):
        poly(d, t[n]['polygon'], 'z', 2)
    for a, p in {'R': (285, 240), 'S': (416, 256), 'A1': (398, 330), 'A2': (402, 388), 'B1': (257, 322), 'B2': (219, 360),
                 'B3': (214, 402), 'B4': (266, 400), 'C1': (322, 332), 'C3': (322, 385), 'C4': (343, 426)}.items():
        text(d, p, a, (255, 255, 255), 15)
    for p, a in [((346, 277), 'Z2'), ((350, 232), 'Z1'), ((352, 296), 'Z3'), ((346, 310), 'Z4'), ((420, 284), 'Z5'),
                 ((359, 356), 'Z6'), ((226, 335), 'Z7'), ((438, 262), 'Z9')]:
        text(d, p, a, FARBE['z'], 13)
    text(d, (301, 348), 'L2', FARBE['n'], 14)
    text(d, (366, 338), 'L1', FARBE['n'], 14)
    text(d, (362, 386), 'SCH', FARBE['n'], 12)
    text(d, (268, 470), 'X', FARBE['o'], 16)
    text(d, (310, 440), 'C5', FARBE['o'], 13)
    text(d, (232, 424), 'B5', FARBE['o'], 13)
    K = {'K1': (398, 374), 'K2': (232, 433), 'K3': (312, 452), 'K4': (352, 432)}
    W = {n: FK.strahl_auf_boden(k, np.array(p, float)) for n, p in K.items()}
    K34 = (W['K3'] + W['K4']) / 2
    K6 = W['K2'] + W['K1'] - K34
    q = [FK.projiziere(k, a) for a in (W['K1'], K34, W['K2'], K6)]
    gestrichelt(d, [tuple(x) for x in q] + [tuple(q[0])], 'o', 1, 5, 4)
    for n, p in K.items():
        art = 's' if n == 'K1' else 'o'
        kreuz(d, p, art)
        text(d, (p[0] + 15, p[1] + 2), n, FARBE[art], 13)
    p6 = FK.projiziere(k, K6)
    kreuz(d, p6, 'o')
    text(d, (p6[0] + 18, p6[1] - 3), 'K6?', FARBE['o'], 13)
    f, r = W['K1'], k['R'][0]
    for h, tx in [(296, '300'), (1281, '1280'), (1593, '1590'), (2232, '2230')]:
        a = FK.projiziere(k, f + np.array([0, -h, 0]) + 650 * r)
        b = FK.projiziere(k, f + np.array([0, -h, 0]) + 850 * r)
        d.line(xy([tuple(a), tuple(b)]), fill=(255, 255, 255), width=1)
        text(d, (b[0] + 15, b[1]), tx, (255, 255, 255), 11)
    d.line(xy([(0, 241), (640, 241)]), fill=(120, 200, 255), width=1)
    text(d, (580, 234), 'Horizont y=241', (120, 200, 255), 11)
    for x, y in [(470, 405), (150, 440)]:
        a = FK.strahl_auf_boden(k, np.array([x, y], float))
        b = FK.projiziere(k, a + np.array([0, -3000, 0]))
        d.line(xy([(x, y), tuple(b)]), fill=(255, 255, 255), width=2)
        text(d, (x + 26, (y + b[1]) / 2), '3000', (255, 255, 255), 12)
    text(d, (125, 190), 'Leon 3005', (255, 255, 255), 12)
    z = [('R', 'Kuppel: Scheitel y187, h~2950, Bauch y313-322, h 850-1310', 's'),
         ('S', 'Schulter rechts: Kante nur y238-278, (429,272)', 'w'),
         ('A1', 'Bein FR Pfeiler: 33-37 px = 570-640 breit', 's'),
         ('A2', 'Bein FR Fuss: x378-426 = 760-840 breit', 'w'),
         ('B1', 'Bein NL Saeule: 61 px = 725-795 breit', 's'),
         ('B2,B3', 'Bein NL Kugel: zwei Lappen, 52 px = 620 breit', 's'),
         ('B4', 'Innenglied, moeglich auch das 4. Bein', 'w'),
         ('B5', 'Masse unter B, Ende unter dem Logo', 'o'),
         ('C1', 'Bein NR oben: 45 auf 36 px, Taille y350', 's'),
         ('C3', 'Untermasse, Oberkante (285,368)-(325,345)', 'w'),
         ('C4', 'Rundling: Masse, Aussenkante weich, Breite Obergrenze', 'w'),
         ('C5', 'Block mit Querstreifen, Boden oder Figur', 'o'),
         ('X', 'rosa-weisser Gegenstand, Zugehoerigkeit offen', 'o'),
         None,
         ('Z1', 'Rosa Stelle, Textur (181,137,150), h~2250', 'z'),
         ('Z2', 'Querband, Mitte (317,265)', 'z'),
         ('Z3,Z4', 'Weisskern (326,293), Band (320,303)', 'z'),
         ('Z5', 'Gelenkkappe Dreieck, h 1280-1590', 'z'),
         ('Z6', 'Querband an der Taille, oberstes des Quertrios', 'z'),
         ('Z7', 'Kappe und Winkel auf der Kugel', 'z'),
         ('Z9', 'Schraegstrich, 4-7 px innerhalb der Aussenkante', 'z'),
         None,
         ('K1', 'Bodenkontakt sicher. K2-K4 offen', 's'),
         ('K6?', 'aus dem Viereck, verdeckt oder in C1', 'o'),
         ('L1,L2', 'Boden sichtbar, trennt die Beine', 'n'),
         ('SCH', 'Boden, nicht Figur, Schatten oder Bodenton offen', 'n')]
    y = legende(w, 'Bild 1: Bahnsteig', z)
    punkte = [(1414, -601, 'K1', 's'), (-1414, 601, 'K2', 'o'), (-1430, -492, 'K3', 'o'), (-839, -710, 'K4', 'o'),
              (1141, 610, 'K6?', 'o')]
    linien = [((1414, -601), (-1130, -601), 'o'), ((-1414, 601), (-1130, -601), 'o'), ((1141, 610), (-1414, 601), 'o'),
              ((1141, 610), (1414, -601), 'o')]
    draufsicht(w, y + 8, 'Draufsicht Koerperrahmen: oben Ende F, unten Ende N (Kamera), links Seite L', punkte, linien, 225)
    w.save(os.path.join(FORM, 'formblatt_bild1.png'))


def bild2():
    k = FK.lade_kamera(2)
    w = Image.new('RGB', (640 * S + LEG, 480 * S), (18, 18, 20))
    w.paste(grund('bild2.png', 0.6).convert('RGB'), (0, 0))
    d = ImageDraw.Draw(w)
    t = teile(2)
    gelesen = [(258, 215), (268, 205), (278, 190), (290, 170), (300, 162), (320, 156), (345, 160), (354, 161)]
    belegt = [(354, 161), (362, 165), (370, 171), (378, 175), (390, 181), (394, 187), (411, 206)]
    gestrichelt(d, gelesen, 'w')
    d.line(xy(belegt), fill=FARBE['s'], width=3)
    gestrichelt(d, [(411, 206), (417, 225), (416, 262), (405, 290)], 'w')
    gestrichelt(d, [(258, 215), (256, 262), (222, 262), (218, 300), (232, 345), (240, 372), (280, 352), (310, 338), (330, 317)], 'w')
    gestrichelt(d, [(330, 317), (340, 345), (372, 352), (405, 352), (405, 290)], 'o')
    for n, art in [('haken', 'w'), ('grat_unten', 's'), ('lichtspitze', 'z'), ('bogen_links', 'z'), ('scheitel_sichel', 'z'),
                   ('bogen_rechts', 'z'), ('innenlinie_rechts_unten', 'z')]:
        poly(d, t[n]['polygon'], art)
    gestrichelt(d, ring(t['faecher']['polygon']), 'o')
    th = np.radians(np.arange(0, 361, 10))
    gestrichelt(d, [(239 + 43 * np.cos(a), 418 + 33 * np.sin(a)) for a in th], 'o', 2)
    for a, b in [((218, 292), (298, 292)), ((232, 347), (285, 346)), ((340, 339), (364, 336)), ((408, 225), (408, 262)),
                 ((225, 419), (262, 414))]:
        d.line(xy([a, b]), fill=FARBE['r'], width=3)
    for x0, y0, x1, y1 in [(160, 325, 228, 388), (419, 225, 462, 300), (419, 310, 505, 400)]:
        gestrichelt(d, [(x0, y0), (x1, y0), (x1, y1), (x0, y1), (x0, y0)], 'n', 1)
    gestrichelt(d, [(345, 382), (405, 382), (405, 396), (345, 396), (345, 382)], 'n', 2)
    for a, p in {'R': (328, 240), 'H': (226, 236), 'U': (266, 322), 'G': (324, 348), 'F': (262, 380), 'Y': (222, 440), 'P': (384, 306)}.items():
        text(d, p, a, (255, 255, 255), 15)
    for p, a in [((262, 186), 'Q1'), ((331, 183), 'Q2'), ((372, 226), 'Q3'), ((428, 208), 'Q4'), ((362, 262), 'Q5')]:
        text(d, p, a, FARBE['z'], 13)
    text(d, (258, 282), 'V1', FARBE['r'], 12)
    text(d, (258, 337), 'V2', FARBE['r'], 12)
    text(d, (352, 328), 'V3', FARBE['r'], 12)
    text(d, (397, 245), 'V4', FARBE['r'], 12)
    text(d, (244, 406), 'V5', FARBE['r'], 12)
    text(d, (375, 389), 'BO', FARBE['n'], 12)
    gestrichelt(d, [(139, 170), (195, 170), (195, 358), (139, 358), (139, 170)], 'n', 1)
    text(d, (167, 161), 'Frau 2785 am Spawn', (255, 255, 255), 11)
    g = np.radians(85.0)
    U, V = np.array([np.cos(g), 0, np.sin(g)]), np.array([-np.sin(g), 0, np.cos(g)])
    O = np.array([2625.0, 0, -23625.0])
    for n, (a, b) in dict(K1=(1414, -601), K2=(-1414, 601), K3=(-1430, -492), K4=(-839, -710), K6=(1141, 610)).items():
        p = FK.projiziere(k, O + a * U + b * V)
        kreuz(d, p, 'o', 5)
        text(d, (p[0] + 15, p[1] - 9), n, FARBE['o'], 11)
    d.line(xy([(0, 142), (640, 142)]), fill=(120, 200, 255), width=1)
    text(d, (585, 135), 'Horizont y~142', (120, 200, 255), 11)
    z = [('R', 'Kuppel: Scheitel (320,156), h 2860-3010', 'w'),
         ('', 'gruene Linie: Verdeckungskante; gelb gestrichelt: gelesen', 's'),
         ('H', 'Haken (205-245,215-262), gefuellt, rotbraun', 'w'),
         ('U', 'Untermasse (218-300,286-372), Mindestausdehnung', 's'),
         ('G', 'Unterstreifen (338,320)-(285,358), 15 px', 's'),
         ('F', 'V-Form (263-304,355-399), Figur oder Feuer', 'o'),
         ('Y', 'deckende runde Flaeche, Zugehoerigkeit offen', 'o'),
         ('P', 'Spitze (358-374,296-313), geclippt', 'w'),
         None,
         ('Q1', 'Bogen links (262-294,168-225)', 'z'),
         ('Q2', 'Scheitelsichel (300-345,157-195)', 'z'),
         ('Q3', 'Bogen rechts, 21 px innerhalb der Kante', 'z'),
         ('Q4', 'Randband (397-425,205-232)', 'z'),
         ('Q5', 'Innenlinie (392,240)-(362,296), Q2-Unterkante+Q3+Q5 ein Zug', 'z'),
         None,
         ('V1', 'Lichtbalken y292, x222-298 ganz verdeckt', 'r'),
         ('V2', 'Bodenlinie y347, x232-285 ganz verdeckt', 'r'),
         ('V3', 'Bodenlinie y338, x340-364 ohne Kontrast', 'r'),
         ('V4', 'Rohr x408, y225-262 fehlt, Ende bei (411,206)', 'r'),
         ('V5', 'Querfuge x225-255: Kontrast 0 statt +120', 'r'),
         ('BO', 'Boden sichtbar, R 207: dort kein Bein', 'n'),
         None,
         ('K1..', 'Fuesse aus Bild 1 in Sicht Flanke (Probe, nur K1 belegt)', 'o'),
         ('', 'duenn gestrichelt tuerkis: Feuer, nicht Figur', 'n')]
    y = legende(w, 'Bild 2: Roter Gang (ROOM5090 Cut 11)', z)
    d = ImageDraw.Draw(w)
    x0 = 640 * S + 16
    for i, s in enumerate(['Kamera von der Disc, Auge (7626,-3256,-25360).',
                           'Die Lage der Figur ist nicht gemessen: Scheitel h 3010-2860',
                           'gilt fuer Abstand 5000-8000. Quermassstab: 89 px je 1000',
                           'bei Tiefe 5000, 75 bei 6000, 64 bei 7000.',
                           'Sicht Flanke und Sicht Ende: FORMBLATT.md Abschnitt 6.']):
        d.text((x0, y + 10 + i * 17), s, font=schrift(13), fill=(225, 225, 225))
    w.save(os.path.join(FORM, 'formblatt_bild2.png'))


if __name__ == '__main__':
    bild1()
    bild2()
    print('fertig')
