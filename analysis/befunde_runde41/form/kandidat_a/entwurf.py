#!/usr/bin/env python3
"""Kandidat A, Entwurf: Knochen und Rundkegel im Modellraum.

Modellraum = Aktorraum in Pose 1 (Bild 1), alle Winkel 0:  x = zum Ende N (Vorderseite +X), y = nach UNTEN, z = zur Seite R.
Tabellen unten nutzen h = -y, weil die Masse im Formblatt als Hoehe stehen.

Jeder Rundkegel: Knochen, a = (x, h, z, r), b = (x, h, z, r). Die Werte hier sind nur der Startpunkt, passung.py rueckt sie an die
Zielmasken. Quelle der Startwerte: FORMBLATT 1.4, 3.1, Rueckprojektion mit Tiefe je Teil (DOSSIER Abschnitt Entwurf). FREI gesetzt, was das Formblatt
als frei fuehrt: Seite L, Bein FL, Kopfform unter dem Haken, Unterseite, Ende F.
"""

# name, eltern, gelenk (x, h, z)
KNOCHEN = [
    ('becken',      -1, (-500, 1900,    0)),   # 0  Wurzel, Rumpf Ende F
    ('huefte_r',     0, (-1300, 1850,  620)),  # 1  Schulter S
    ('huefte_l',     0, (-1300, 1850, -620)),  # 2
    ('fr_ober',      1, (-1400, 1600,  640)),  # 3
    ('fr_unter',     3, (-1400, 1250,  610)),  # 4  Pfeiler A1
    ('fr_fuss',      4, (-1410,  290,  600)),  # 5  Fuss A2
    ('fl_ober',      2, (-1200, 1600, -640)),  # 6  frei, Kopie von Bein FR
    ('fl_unter',     6, (-1150, 1250, -620)),  # 7
    ('fl_fuss',      7, (-1150,  290, -620)),  # 8
    ('rumpf_mitte',  0, (-150, 1950,    0)),   # 9
    ('rumpf',        9, ( 250, 1950,    0)),   # 10 BB_TEIL_RUMPF
    ('nl_ober',     10, ( 720, 1500, -560)),   # 11 Saeule B1
    ('nl_unter',    11, ( 830,  990, -640)),   # 12 Kugel B2
    ('nl_hand',     12, (1200,  620, -780)),   # 13 Ende Schlagglied Seite 1, Lappen B3, B5
    ('nl_innen',    12, ( 930,  950, -480)),   # 14 Innenglied B4
    ('nl_daumen',   13, (1350,  220, -700)),   # 15 frei: Vorderteil Hand, unter dem Logo
    ('nr_ober',     10, ( 620, 1320,  180)),   # 16 C1
    ('nr_unter',    16, ( 620,  760,  200)),   # 17 Taille, C3
    ('nr_hand',     17, (1000,  420,  560)),   # 18 Ende Schlagglied Seite 0, Rundling C4
    ('nr_daumen',   18, (1250,  180,  560)),   # 19 frei: C5
    ('hals',        10, ( 640, 1860,  160)),   # 20 Vorsprung
    ('kopf',        20, ( 880, 2060,  340)),   # 21
    ('kiefer',      21, (1000, 1980,  380)),   # 22
]

# Knochen, a, b.  Reihenfolge frei.
R = [
    # Rumpf: eine Kuppel aus Kugeln (Metaballs), Laengsachse x
    ('rumpf',       (-120, 2030, -50, 880), (-120, 2030, -50, 880)),
    ('rumpf',       ( 200, 1990,  10, 860), ( 200, 1990,  10, 860)),
    ('rumpf',       ( 280, 2250, -220, 560), ( 280, 2250, -220, 560)),    # Schulterkuppe Seite L
    ('rumpf',       ( 280, 2250,  300, 560), ( 280, 2250,  300, 560)),    # Schulterkuppe Seite R
    ('rumpf',       ( 420, 1560, -300, 520), ( 420, 1560, 200, 520)),     # Brustboden
    ('rumpf_mitte', (-300, 2030, -30, 880), (-300, 2030, -30, 880)),
    ('rumpf_mitte', (-300, 2400, -30, 520), (-300, 2400, -30, 520)),      # Scheitel
    ('becken',      (-620, 1960,   0, 820), (-620, 1960,   0, 820)),
    ('becken',      (-1000, 1900,  -30, 690), (-1000, 1900, -30, 690)),
    ('becken',      (-800, 1500, -300, 500), (-800, 1500, 300, 500)),     # Bauch hinten
    # Schulter S, Seite R und Spiegel
    ('huefte_r',    (-1330, 1760,  730, 400), (-1330, 1900,  640, 400)),
    ('huefte_l',    (-1330, 1760, -730, 400), (-1330, 1900, -640, 400)),
    ('fr_ober',     (-1400, 1560,  640, 400), (-1400, 1280,  620, 330)),
    ('fl_ober',     (-1200, 1560, -640, 400), (-1150, 1280, -620, 330)),
    # Bein FR
    ('fr_unter',    (-1405, 1300,  610, 320), (-1410,  380,  600, 310)),
    ('fr_fuss',     (-1410,  280,  600, 270), (-1410,  160,  600, 200)),
    ('fr_fuss',     (-1250,  140,  380, 150), (-1250,  130,  820, 150)),
    ('fr_fuss',     (-1600,  140,  420, 130), (-1600,  130,  780, 130)),
    # Bein FL
    ('fl_unter',    (-1150, 1300, -620, 320), (-1150,  380, -620, 310)),
    ('fl_fuss',     (-1150,  280, -620, 270), (-1150,  160, -620, 200)),
    ('fl_fuss',     (-1000,  140, -440, 150), (-1000,  130, -800, 150)),
    ('fl_fuss',     (-1350,  140, -440, 130), (-1350,  130, -800, 130)),
    # Bein NL: Saeule B1, Lappen B2, B3, Innenglied B4, Masse B5
    ('nl_ober',     ( 720, 1500, -560, 410), ( 830, 1020, -640, 400)),
    ('nl_ober',     ( 760, 1300, -380, 300), ( 860, 1000, -420, 300)),
    ('nl_unter',    ( 900,  960, -640, 300), (1200,  640, -800, 300)),
    ('nl_unter',    ( 980,  820, -860, 220), (1200,  640, -900, 220)),
    ('nl_hand',     (1250,  520, -780, 300), (1300,  300, -730, 300)),
    ('nl_hand',     (1250,  380, -930, 200), (1320,  220, -880, 200)),
    ('nl_innen',    ( 930,  950, -480, 210), (1130,  330, -340, 210)),
    ('nl_daumen',   (1350,  200, -700, 220), (1450,  160, -600, 180)),
    # Bein NR: C1, Taille, C3, Rundling C4, C5
    ('nr_ober',     ( 620, 1320,  180, 320), ( 620,  800,  190, 230)),
    ('nr_ober',     ( 620, 1200,  330, 220), ( 620,  800,  260, 190)),
    ('nr_unter',    ( 700,  700,  220, 290), ( 950,  430,  460, 330)),
    ('nr_unter',    ( 800,  560,  100, 220), ( 950,  420,  330, 300)),
    ('nr_hand',     (1000,  380,  560, 330), (1120,  260,  640, 300)),
    ('nr_hand',     ( 920,  320,  760, 220), (1050,  250,  800, 220)),
    ('nr_daumen',   (1250,  170,  560, 190), (1420,  150,  500, 150)),
    # Vorsprung: Hals + Brustwulst (Z3, Z4), Kopf (Z1), Kiefer (Z2). Haken = Profil des Kopfs in Bild 2
    ('hals',        ( 600, 1880,  120, 400), ( 840, 1700,  300, 300)),
    ('hals',        ( 860, 1600,  380, 240), ( 930, 1480,  380, 200)),
    ('kopf',        ( 980, 2170,  380, 260), (1090, 2130,  420, 230)),
    ('kopf',        ( 900, 2250,  360, 200), (1140, 2260,  420, 130)),    # Schaedeldach, Oberkante Haken
    ('kiefer',      (1040, 1920,  400, 200), (1110, 1850,  420, 160)),
]

# Lagen laut FORMBLATT 3.1, 3.2 (Ende N vorn), Startwerte
LAGE = {1: (-18514.0, 0.0, -18726.0), 2: (2625.0, 0.0, -23625.0)}
GIER = {1: 2145.0, 2: 1280.0}
# Pose 2, Startwinkel (Achsenvektor, Bogenmass, um y nach unten: + dreht +x nach -z, also zur Seite L).
# Grund: Zwischen beiden Kameras liegen im Modellraum nur rund 65 Grad. Der Stapel Z1-Z4 faellt in Bild 2 aber nicht auf (FORMBLATT 2.3, 6B).
# Der Vorsprung ist also zur Frau gewandt, von der Kamera weg; der Haken ist seine abgewandte Seite. Das ist Deutung, kein Messwert.
W2_START = {'hals': (0.0, 0.78, 0.0), 'kopf': (0.0, 0.70, 0.0), 'huefte_r': (0.0, 0.0, 0.14), 'fr_ober': (0.0, 0.0, 0.10), 'fr_fuss': (0.0, 0.0, -0.24)}
GIER2_SOLL = 1280.0
