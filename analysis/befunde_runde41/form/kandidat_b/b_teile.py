#!/usr/bin/env python3
"""Kandidat B: Knochenbau und Startvolumen. Lokalraum = Beinrahmen Pose 1 (+X Ende N, +Z Seite R, Y unten, Boden y=0).
Volumen: benannte Knoten (Kugeln), Kanten (Kegel mit Rundkappen), je Knochen. Knoten am Gelenk werden von Eltern und Kind geteilt."""
import numpy as np
from b_geom import *

# Index ist Knochen-Index im EMD. 10 Rumpf, 13/18 Schlagglied-Enden sind Pflicht aus birkin_bahnsteig.c
KNOCHEN = [  # name, eltern
    ('rumpf_f', -1),    # 0 Wurzel, Ende F
    ('fl_ober', 0),     # 1
    ('fl_unter', 1),    # 2
    ('fl_fuss', 2),     # 3
    ('fl_zeh', 3),      # 4
    ('fr_ober', 0),     # 5  Schulter S
    ('fr_unter', 5),    # 6
    ('fr_fuss', 6),     # 7
    ('fr_zeh', 7),      # 8
    ('rumpf_m', 0),     # 9
    ('rumpf', 9),       # 10 BB_TEIL_RUMPF, Kuppelfront, dreht wie ein Kopf
    ('nl_ober', 9),     # 11
    ('nl_unter', 11),   # 12
    ('nl_faust', 12),   # 13 Hieb Seite 1, aeusserer Finger
    ('nl_innen', 12),   # 14 innerer Finger B4
    ('maul_l', 10),     # 15 Lippe Seite L
    ('nr_ober', 9),     # 16
    ('nr_unter', 16),   # 17
    ('nr_faust', 17),   # 18 Hieb Seite 0
    ('nr_innen', 17),   # 19
    ('maul_r', 10),     # 20 Lippe Seite R
]
IDX = {n: i for i, (n, _) in enumerate(KNOCHEN)}
ELTERN = [e for _, e in KNOCHEN]

KUPPEL_MITTE = np.array([-420.0, -2020.0, -100.0])


def _z(x, y, z):
    return in_ebene_z(x, y, z)


def start():
    """-> knoten {name: (p, r)}, kanten [(knochen, a, b)], gelenk {knochen: knotenname oder Punkt}."""
    N, E = {}, []

    def n(name, p, r):
        N[name] = (np.array(p, float), float(r))

    def kante(b, a, c=None):
        E.append((b, a, c or a))
    # Kuppel: Kugel plus Stellknoten. Fest in der Pose 2 (Gruppe rumpf_m)
    c = KUPPEL_MITTE
    n('d0', c, 1050)
    i = 0
    for az in range(0, 360, 60):
        for el, rr in ((-48, 520), (0, 560), (48, 480)):
            a, e = np.radians(az), np.radians(el)
            d = np.array([np.cos(a) * np.cos(e), np.sin(e), np.sin(a) * np.cos(e)])
            n('d%d' % (i + 1), c + d * (1040 - rr), rr)
            i += 1
    n('dt', c + np.array([0, -600, 0]), 430)
    n('du', c + np.array([0, 560, 0]), 480)
    for k in N:
        kante('rumpf_m', k)
    # Bein FR mit Schulter S
    n('fr_h', (-1255, -2010, 700), 300)
    n('fr_s', (-1270, -1850, 860), 390)
    n('fr_k', _z(399, 296, 700), 310)
    n('fr_a', _z(399, 350, 640), 300)
    n('fr_f', (-1414, -170, 620), 200)
    n('fr_t1', (-1330, -95, 270), 95)
    n('fr_t2', (-1330, -95, 960), 95)
    n('fr_t3', (-1120, -95, 620), 105)
    n('fr_t4', (-1640, -110, 620), 125)
    kante('fr_ober', 'fr_h', 'fr_k')
    kante('fr_ober', 'fr_s', 'fr_k')
    kante('fr_unter', 'fr_k', 'fr_a')
    kante('fr_fuss', 'fr_a', 'fr_f')
    kante('fr_fuss', 'fr_f', 'fr_t4')
    kante('fr_zeh', 'fr_f', 'fr_t1')
    kante('fr_zeh', 'fr_f', 'fr_t2')
    kante('fr_zeh', 'fr_f', 'fr_t3')
    # Bein FL: oben Spiegel von FR, Fuss nach FORMBLATT 1.2 K6 mit Fuss wie A2 bei (321,374)
    n('fl_h', (-1255, -2010, -900), 300)
    n('fl_s', (-1270, -1850, -1060), 390)
    n('fl_k', (-1160, -1280, -860), 300)
    n('fl_a', (-960, -340, -640), 290)
    n('fl_f', (-901, -170, -623), 200)
    n('fl_t1', (-820, -95, -960), 95)
    n('fl_t2', (-820, -95, -290), 95)
    n('fl_t3', (-610, -95, -623), 105)
    n('fl_t4', (-1130, -110, -623), 125)
    kante('fl_ober', 'fl_h', 'fl_k')
    kante('fl_ober', 'fl_s', 'fl_k')
    kante('fl_unter', 'fl_k', 'fl_a')
    kante('fl_fuss', 'fl_a', 'fl_f')
    kante('fl_fuss', 'fl_f', 'fl_t4')
    kante('fl_zeh', 'fl_f', 'fl_t1')
    kante('fl_zeh', 'fl_f', 'fl_t2')
    kante('fl_zeh', 'fl_f', 'fl_t3')
    # Bein NL: Schulter in der Kuppel, Ellenbogen unter der Saeule, Handwurzel, zwei Finger
    n('nl_s', (134, -1950, -560), 400)
    n('nl_e', _z(254, 332, -590), 360)
    n('nl_w', _z(236, 358, -760), 260)
    n('nl_a1', _z(216, 388, -880), 230)
    n('nl_a2', _z(226, 406, -760), 220)
    n('nl_i1', _z(267, 356, -560), 220)
    n('nl_i2', _z(269, 402, -470), 195)
    kante('nl_ober', 'nl_s', 'nl_e')
    kante('nl_unter', 'nl_e', 'nl_w')
    kante('nl_faust', 'nl_w', 'nl_a1')
    kante('nl_faust', 'nl_a1', 'nl_a2')
    kante('nl_innen', 'nl_e', 'nl_i1')
    kante('nl_innen', 'nl_i1', 'nl_i2')
    # Bein NR
    n('nr_s', (-73, -1900, 446), 340)
    n('nr_e', _z(326, 346, 400), 225)
    n('nr_w', _z(322, 372, 520), 270)
    n('nr_a1', _z(328, 402, 540), 250)
    n('nr_a2', _z(345, 425, 640), 240)
    n('nr_i1', _z(304, 388, 400), 175)
    n('nr_i2', _z(312, 424, 430), 150)
    kante('nr_ober', 'nr_s', 'nr_e')
    kante('nr_unter', 'nr_e', 'nr_w')
    kante('nr_faust', 'nr_w', 'nr_a1')
    kante('nr_faust', 'nr_a1', 'nr_a2')
    kante('nr_innen', 'nr_w', 'nr_i1')
    kante('nr_innen', 'nr_i1', 'nr_i2')
    gelenk = dict(fr_ober='fr_h', fr_unter='fr_k', fr_fuss='fr_a', fr_zeh='fr_f', fl_ober='fl_h', fl_unter='fl_k', fl_fuss='fl_a', fl_zeh='fl_f',
                  nl_ober='nl_s', nl_unter='nl_e', nl_faust='nl_w', nl_innen='nl_e', nr_ober='nr_s', nr_unter='nr_e', nr_faust='nr_w', nr_innen='nr_w')
    return N, E, gelenk


# Knochen mit fest im Rumpf bleibender Lage (Stelle, um die sie in Pose 2 drehen)
FEST = {'rumpf_f': (-940.0, -2000.0, -100.0), 'rumpf_m': tuple(KUPPEL_MITTE), 'rumpf': tuple(KUPPEL_MITTE), 'maul_l': (1000.0, -2000.0, 200.0), 'maul_r': (1000.0, -2000.0, 200.0)}
