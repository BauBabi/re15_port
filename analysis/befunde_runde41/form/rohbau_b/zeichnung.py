# -*- coding: utf-8 -*-
"""Rohbau B: Textur als Materialzonen. Grundhaut, Plattenzonen und die Zeichnung an ihrer Stelle: Sollflaechen aus FORMBLATT 2 Abschnitt 6, aus den Kameras auf die Figur gelegt.
Gemalt wird hier nicht, das kommt spaeter. Der Atlas bleibt offen dafuer: je Teil ein Streifen, U Umfang, V Laenge. textur.png ist die Vorlage, zonen.json nennt die Karten."""
import json, math, os, sys
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))


def stellen():
    """(Bild, Name, Material, Polygon). Reihenfolge: Mitteltoene zuerst, helle Zeichnung darueber."""
    import form_pruefung as P
    aus = []
    M = {m['id']: m for m in P.merkmale().values()} if isinstance(P.merkmale(), dict) else {m['id']: m for m in P.merkmale()}
    for i, mat in (('A35', 'rotbraun_mittelton'), ('A25', 'rotbraun_mittelton'), ('A26', 'rotbraun_mittelton'), ('B04', 'rotbraun_mittelton'), ('B15', 'rotbraun_mittelton'), ('B05', 'rotbraun_mittelton'),
                   ('A05', 'feine_linien'), ('A06', 'feine_linien'), ('A10', 'feine_linien'), ('A12', 'helle_baender'), ('A29', 'rosa_matt'), ('A30', 'rosa_matt'),
                   ('B08', 'helle_baender'), ('B09', 'helle_baender'), ('B10', 'feine_linien'), ('B11', 'feine_linien'), ('B12', 'feine_linien'), ('B14', 'feine_linien'), ('B13', 'weisskerne'),
                   ('B17', 'untere_streifen'), ('B18', 'untere_streifen'), ('B19', 'untere_streifen'), ('B20', 'untere_streifen'), ('B21', 'feine_linien'), ('B22', 'feine_linien'), ('B23', 'feine_linien'), ('B24', 'feine_linien')):
        m = M[i]
        aus.append((m['bild'], i, mat, m['polygon']))
    for s in P.stellen():
        for q in s['polygone']:
            aus.append((s['bild'], s['id'], 'matt' if s['id'] in ('Z6b', 'Z6c') else s['material'], q))
    # Z1 Rosa Spindel: Flaeche und Achse aus FORMBLATT 2.2 und bewertung_bild.md (44 x 21 px, Mitte (318,240), +53 Grad)
    c = math.cos(math.radians(53))
    sn = math.sin(math.radians(53))
    el = [(318 + 22 * math.cos(u) * c + 10.5 * math.sin(u) * sn, 240 - 22 * math.cos(u) * sn + 10.5 * math.sin(u) * c) for u in np.linspace(0, 2 * math.pi, 24, endpoint=False)]
    aus.insert(0, (1, 'Z1f', 'rosa_stelle', el))
    return aus


ZUG_Q = [[(300, 192), (314, 189), (345, 189), (353, 194), (372, 201), (384, 218), (388, 236)], [(383, 260), (374, 278), (363, 290), (365, 302)]]      # FORMBLATT 2.2 Zug Q, Blau-Grat


def textur(f, fig, form, flaechen, karten, B):
    """-> RGB TEX_H x TEX_B."""
    PAL = dict(B.PAL)
    PAL['z9'] = (85, 80, 60)
    PAL['grundhaut_dunkel'] = (36, 22, 32)      # innerhalb des Fehlerbands der Palette (44,18,27 +-22,19,22), in Bild 1 trifft der Filter damit die Dunkelheit
    PAL['matt'] = (92, 100, 112)                # Untergrenze feine_linien: Bewertung Bild, matte Querstreifen A15, A16
    PAL['rosa_matt'] = (158, 118, 130)          # Untergrenze rosa_stelle: Schwanzende, Licht hebt es
    PAL['untere_streifen'] = (205, 175, 195)
    H, W = B.TEX_H, B.TEX_B
    rgb = np.zeros((H, W, 3), np.uint8)
    rgb[:] = PAL['grundhaut_dunkel']
    P, N, ID = B.rastern_uv(f, flaechen)
    # Zonen: Ring-Tags
    for ti, V, q, Nz, tag in flaechen:
        if tag is None:
            continue
        cv2.fillConvexPoly(rgb, np.round(q).astype(np.int32), tuple(int(c) for c in PAL[tag]), lineType=cv2.LINE_8)
    # Zeichnung an ihrer Stelle
    st = stellen()
    for n in (1, 2):
        px, sicht = B.pixel_der_texel(f, fig, P, ID, n, N)
        _px0, sicht0 = B.pixel_der_texel(f, fig, P, ID, n)
        y, x = np.nonzero(sicht)
        y0, x0 = np.nonzero(sicht0)
        for bild, i, mat, poly in st:
            if bild != n:
                continue
            karte = np.zeros((480, 640), np.uint8)
            cv2.fillPoly(karte, [np.round(np.array(poly, float)).astype(np.int32)], 1)
            if i in ('P', 'B13'):           # Spitze P sitzt auf der schmalen Wangenspitze, deren Flaechen fast laengs zum Blick stehen
                s = karte[px[y0, x0, 1], px[y0, x0, 0]] > 0
                rgb[y0[s], x0[s]] = PAL[mat]
                continue
            s = karte[px[y, x, 1], px[y, x, 0]] > 0
            rgb[y[s], x[s]] = PAL[mat]
        if n == 2:
            karte = np.zeros((480, 640), np.uint8)
            for zug in ZUG_Q:
                cv2.polylines(karte, [np.array(zug, np.int32)], False, 1, 5)
            s = karte[px[y, x, 1], px[y, x, 0]] > 0
            rgb[y[s], x[s]] = PAL['feine_linien']
    return rgb
