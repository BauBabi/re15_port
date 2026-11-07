#!/usr/bin/env python3
"""Rohbau final, Zeichnung: jede Marke an ihrer Bildstelle, in echter Form und mit eigener Farbe.
Form: Pixel ueber halber Hoehe zwischen Umgebung und Spitze innerhalb des Prueffelds (`rf_textur.marke_form`), nicht das ganze Prueffeld. Duenne Zuege als Linie (Zug Q).
Farbe: Vorgabe aus der Palette, Faktor je Marke aus zw/marken.json (Stellschleife rf_stell.py: Render durch Filter gegen Video). Kein Malen, nur Zonen und Zeichnung."""
import json
import os

import cv2
import numpy as np

from rf_kern import HIER
import form_pruefung as P
import rf_textur as RT

# Zug Q: ein durchgehender Bogen (FORMBLATT2 Abschnitt 6), duenn, nach B. Linie in Bildkoordinaten.
ZUG_Q = [(262, 225), (270, 205), (280, 186), (292, 172), (300, 178)]
ZUG_Q2 = [(300, 192), (314, 189), (330, 188), (345, 189), (353, 194), (372, 201), (384, 218), (388, 236)]
ZUG_Q3 = [(388, 236), (386, 250), (381, 262), (374, 278), (366, 290), (365, 300)]
# Farben: Vorgabe je Material (untere Haelfte der Spanne, Licht von Bild 1 hebt die Oberseiten)
FARBE = dict(rosa=(158, 118, 130), band=(102, 117, 124), linie=(88, 96, 110), rot=(92, 52, 72), warm=(172, 62, 50), kern=(150, 150, 150), z9=(85, 80, 60), streifen=(205, 175, 180), rosa_end=(150, 104, 118), weiss_end=(170, 165, 170), rot_end=(120, 40, 44))


def liste():
    """(id, Bild, Bildmaske, Farbe). Reihenfolge: dunkle Toene zuerst."""
    M = P.merkmale()
    pz = lambda ids: [M[i]['polygon'] for i in ids]   # noqa: E731
    st = {s['id']: s for s in P.stellen()}
    L = []

    def form(i, n, polys, farbe, anteil=.5, rand=4):
        m = RT.poly(polys)
        f, hi, lo = RT.marke_form(n, m, anteil, rand)
        L.append((i, n, f, FARBE[farbe]))

    def voll(i, n, polys, farbe, weite=0):
        L.append((i, n, RT.poly(polys, weite), FARBE[farbe]))

    def linie(i, n, pts, farbe, breite=2):
        m = np.zeros((P.HOEHE, P.BREITE), np.uint8)
        cv2.polylines(m, [np.array(pts, np.int32)], False, 1, breite, cv2.LINE_8)
        L.append((i, n, m, FARBE[farbe]))
    # Bild 1
    voll('Z1', 1, st['Z1']['polygone'], 'rosa')
    form('Z2', 1, st['Z2']['polygone'], 'band')
    form('Z3', 1, st['Z3']['polygone'], 'kern', .55)
    form('Z4', 1, st['Z4']['polygone'], 'band')
    form('Z5', 1, st['Z5']['polygone'], 'band', .6)
    form('Z6a', 1, st['Z6a']['polygone'], 'band')
    form('Z6b', 1, st['Z6b']['polygone'], 'linie')
    form('Z6c', 1, st['Z6c']['polygone'], 'linie')
    form('Z7', 1, st['Z7']['polygone'], 'linie')
    form('Z8', 1, st['Z8']['polygone'], 'linie')
    form('Z9', 1, st['Z9']['polygone'], 'z9', .2)
    # Schwanzende X: rosa, weiss, rot untereinander (Zeilen aus dem Video, Spalte 266: 428-444 rosa, 448-458 weiss, 462-479 rot)
    xm = RT.poly([P.teil_b1('X')['polygon']], 1)
    for i, (a, b, f) in (('Xa', (424, 446, 'rosa_end')), ('Xb', (446, 460, 'weiss_end')), ('Xc', (460, 480, 'rot_end'))):
        q = np.zeros_like(xm)
        q[a:b] = 1
        L.append((i, 1, q & xm, FARBE[f]))
    # Bild 2
    form('Q2', 2, st['Q2']['polygone'], 'rot', .55)
    linie('Qa', 2, ZUG_Q, 'linie', 3)
    linie('Qb', 2, ZUG_Q2, 'linie', 2)
    linie('Qc', 2, ZUG_Q3, 'linie', 2)
    form('Q1', 2, st['Q1']['polygone'], 'linie')
    form('Q3', 2, st['Q3']['polygone'], 'linie')
    form('Q4', 2, st['Q4']['polygone'], 'band')
    form('H8', 2, pz(['B08']), 'linie')
    form('H9', 2, pz(['B09']), 'linie')
    # G: gerippter Streifen auf ganzer Laenge: Grund warm, Rippen hell, aus dem Bild
    voll('Gg', 2, st['G']['polygone'] + pz(['B21', 'B22']), 'warm')
    m = RT.poly(st['G']['polygone'] + pz(['B17', 'B18', 'B19', 'B20', 'B21', 'B22']))
    y = cv2.GaussianBlur(P.original(2).astype(np.float32) @ P.YK, (0, 0), .7)
    L.append(('Gr', 2, ((y > 130) & (m > 0)).astype(np.uint8), FARBE['streifen']))
    form('P', 2, st['P']['polygone'], 'kern', .45)
    return L


def malen(rgb, bk, pal):
    p = os.path.join(HIER, 'zw', 'marken.json')
    k = json.load(open(p)) if os.path.exists(p) else {}
    rgb = rgb.astype(np.float32)
    # Rotbraun-Flecken der Palette, nur wo sie sichtbar sind
    for i, n, m, f in liste():
        a = bk.stelle(n, m)
        if i in ('Gg', 'Gr', 'P'):
            a = a & ~bk.vis[1]              # Vorderseite: was Bild 1 sieht, bleibt dort dunkel (Pruefstelle N2)
        rgb[a] = np.array(f, float) * float(k.get(i, 1.0))
    return np.clip(rgb, 0, 255).astype(np.uint8)
