"""Grundlage v2: JEDE Beschriftung in JEDEM Nutzerbild als eigene Zone (Koordinaten im beschrifteten Bild).
Bauarten: flut/huelle/loecher/band wie v1 (g_bau = Kopie von zonen_bau.py); neu: fleck (Klasse am Linienende), wachsen (Farbgebiet neben dem
Linienende), schleifen (je geschlossene Nutzer-Schleife eine Zone), zehen (vorderste Zehenflaechen), Modellzonen im Kopf (Modellbedeutung)."""
HT = 'Haut (Teil benannt, kein Farbwort)'


def Z(nr, id_, datei, wort, farbe, bau, material, **kw):
    return dict(nr=nr, id=id_, datei=datei, wort=wort, farbe=farbe, bau=bau, material=material, **kw)


R, KO, SW, LB, RB, FU, BV = ('Rücken.png', 'Kopf_von_hinten.png', 'Schwanz_hinten.png', 'linkes_bein_hinten.png', 'rechtes_bein_hinten.png',
                            'linker_rechter_Fuß_hinten.png', 'Bein_Vorne_rechts.png')
VO, GE, HA, VL = 'Vorne_Oberkörper.png', 'Gesicht vorne.png', 'vorne_rechte_Hand_und_Kralle.png', 'Vorderes_Linkes_Bein_mit_fuß.png'
FUSS_WORT = 'linker Fuß mit Teil der Fußkralle vorne. Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts wäre es sicher genauso'
ZONEN = [
    # ------------------------------------------------------------------ Bild 1 (Rueckansicht)
    Z('1', 'oberkoerper_hinten', R, 'Oberkörper', 'rot', ('flut', dict(waende=['rot'], saaten=[(400, 200)], dil=7)), HT,
      teile=[0, 7, 10], namensteile=[0, 7, 10]),
    Z('2', 'wunde_hinten', R, 'fleischige Wunde/Haut', 'grau', ('flut', dict(waende=['rot', 'grau', 'gruen'], saaten=[(290, 110)])),
      'fleischige Wunde/Haut', eltern='oberkoerper_hinten'),
    Z('3', 'musterung_rechts_hinten', R, 'Oberkörper Musterung rechts', 'gruen',
      ('flut', dict(waende=['rot', 'grau', 'gruen'], saaten=[(290, 165)])), 'Musterung', eltern='oberkoerper_hinten'),
    Z('4', 'musterung_links_hinten', R, 'Oberkörper Musterung links', 'gruen', ('huelle', dict(farbe='gruen', box=(129, 118, 152, 150))),
      'Musterung', eltern='oberkoerper_hinten'),
    Z('5', 'kopf_hinten', KO, 'Kopf', 'rot', ('flut', dict(waende=['rot'], saaten=[(480, 180)], dil=7)), HT, teile=[22, 23],
      namensteile=[22, 23]),
    Z('6', 'auge_hinten', KO, 'wahrscheinlich etwas Auge', 'gruen', ('huelle', dict(farbe='gruen', box=(505, 190, 520, 206))), 'Auge',
      teile=[22], eltern='kopf_hinten'),
    Z('7', 'mund_hinten', KO, 'Mund?', 'lila', ('huelle', dict(farbe='lila', box=(494, 200, 513, 216))), 'Maul', teile=[22, 23],
      eltern='kopf_hinten'),
    Z('8', 'schwanz', SW, 'Schwanz der in den Rücken übergeht', 'rot', ('flut', dict(waende=['rot', 'blau'], saaten=[(620, 120), (612, 300)])),
      HT, teile=[0, 1, 2, 3, 4, 5], namensteile=[1, 2, 3, 4, 5]),
    Z('9', 'schwanz_muster', SW, 'Schwanz Muster / Maserung', 'gruen',
      ('huelle_n', dict(farbe='gruen', teile=[(580, 180, 596, 222), (605, 175, 650, 231), (589, 329, 636, 443), (629, 445, 652, 470)])),
      'Musterung/Maserung', eltern='schwanz'),
    Z('10', 'zipfel', SW, 'Zipfel - separater Schwanzabschnitt am Schluss', 'blau',
      ('flut', dict(waende=['blau', 'rot'], saaten=[(612, 540)], box=(480, 440, 660, 597))), HT, teile=[6], namensteile=[6]),
    Z('11', 'zipfel_musterung', SW, 'Schwanz Musterung', 'orange',
      ('huelle_komp', dict(farbe='orange', box=(540, 470, 660, 575), min_px=100)), 'Musterung', eltern='zipfel'),
    Z('12', 'rotes_fleisch_zipfel', SW, 'verschieden farbiges rotes Fleisch wie oben am Becken', 'lila',
      ('wachsen_n', dict(enden=[(642, 458), (632, 496), (624, 530), (618, 575)], tol=14, r_max=22)), 'rotes Fleisch', eltern='zipfel'),
    Z('13', 'bein_hinten_links', LB, 'linkes Bein', 'rot', ('flut', dict(waende=['rot'], saaten=[(345, 290), (320, 380)],
      sehnen=[((364, 134), (370, 200)), ((370, 200), (372, 330)), ((372, 330), (379, 358))], box=(260, 125, 385, 450))), HT,
      teile=[8, 24, 25], namensteile=[8, 24], abschnitte='lila',
      abschnitt_wort='linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß'),
    Z('14', 'weisse_musterung_bein_hinten_links', LB, 'weiße Musterung', 'gruen', ('fleck', dict(ende=(352, 209), klasse='weiss')),
      'weiße Musterung', eltern='bein_hinten_links', z='Z1'),
    Z('15', 'weisse_musterung_bein_hinten_links_strich2', LB, 'weiße Musterung (zweiter grüner Strich, ohne eigenen Text)', 'gruen',
      ('fleck', dict(ende=(366, 197), klasse='weiss')), 'weiße Musterung', eltern='bein_hinten_links', z='Z5'),
    Z('16', 'braeunliche_musterung_bein_hinten_links', LB, 'bräunliche Musterung', 'dunkelrot',
      ('wachsen', dict(ende=(340, 244), tol=18, r_max=22)), 'bräunliche Musterung', eltern='bein_hinten_links'),
    Z('17', 'bein_hinten_rechts', RB, 'rechtes Bein hinten', 'rot', ('flut', dict(waende=['rot'], saaten=[(300, 300), (290, 420)],
      box=(228, 160, 400, 478))), HT, teile=[9, 26, 27], namensteile=[9, 26], abschnitte='lila',
      abschnitt_wort='rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß.'),
    Z('18', 'weisse_musterung_bein_hinten_rechts', RB, 'weiße Musterung', 'gruen', ('fleck', dict(ende=(315, 219), klasse='weiss')),
      'weiße Musterung', eltern='bein_hinten_rechts', z='Z3'),
    Z('19', 'braeunliche_musterung_bein_hinten_rechts', RB, 'bräunliche Musterung', 'dunkelrot',
      ('wachsen', dict(ende=(303, 242), tol=20, r_max=26, vorlauf=6)), 'bräunliche Musterung', eltern='bein_hinten_rechts'),
    Z('20', 'fuss_hinten_links', FU, FUSS_WORT, 'rot', ('huelle', dict(farbe='rot', box=(455, 375, 545, 500))), HT, teile=[25],
      namensteile=[25]),
    Z('21', 'fusskrallen_hinten_links', FU, FUSS_WORT, 'rot', ('zehen', dict(mesh=25, marken_box=(510, 378, 545, 460))), 'Kralle',
      teile=[25], eltern='fuss_hinten_links'),
    Z('22', 'fusskrallen_hinten_rechts', FU, 'Rechts wäre es sicher genauso (aus demselben Text)', 'rot', ('zehen', dict(mesh=27, marken_box=None)),
      'Kralle', teile=[27]),
    Z('23', 'vorderbein_rechts', BV, 'rechtes Bein und Fuß', 'rot', ('flut', dict(waende=['rot', 'hellblau'], saaten=[(450, 300), (430, 375)],
      box=(395, 185, 520, 395))), HT, teile=[16, 17, 18], namensteile=[16, 17, 18], abschnitte='lila',
      abschnitt_wort='wahrscheinliche Bein Abschnitte'),
    Z('24', 'weisse_musterung_vorderbein_rechts', BV, 'weiße Musterung', 'gruen',
      ('flut', dict(waende=['rot', 'gruen'], saaten=[(440, 210)], box=(395, 185, 520, 240))), 'weiße Musterung', eltern='vorderbein_rechts'),
    Z('25', 'fuss_kralle_vorderbein_rechts', BV, 'Fuß und Kralle', 'hellblau', ('loecher', dict(farbe='hellblau', box=(470, 340, 512, 380))),
      'Fuß: Haut (Teil benannt, kein Farbwort); Kralle: im Modell hier nicht sichtbar', teile=[18], eltern='vorderbein_rechts'),
    # ------------------------------------------------------------------ Bild 2 (Vorderansicht)
    Z('26', 'oberkoerper_vorn', VO, 'Oberkörper', 'rot', ('flut', dict(waende=['rot'], saaten=[(250, 150)])), HT, teile=[10, 21],
      namensteile=[10, 21], ohne_kopf=True, z='Z4'),
    Z('27', 'fleisch_vorn', VO, 'großer fleischfarbener Teil', 'gruen', ('flut', dict(waende=['rot', 'gruen'], saaten=[(300, 150), (340, 112)])),
      'fleischfarben', teile=[10, 21], ohne_kopf=True, eltern='oberkoerper_vorn', z='Z4'),
    Z('28', 'gesicht_vorn', GE, 'Vorderseite Gesicht mit Maul nach unten rechts blickend', 'rot',
      ('flut', dict(waende=['rot', 'hellblau'], saaten=[(520, 300), (600, 440)], dil=5)), HT, teile=[22, 23], namensteile=[22, 23], z='Z4'),
    Z('29', 'augen_vorn', GE, 'Augen', 'gruen', ('fleck_n', dict(enden=[(441, 245), (568, 248)], klasse='auge')), 'Auge', teile=[22],
      eltern='gesicht_vorn', z='Z2'),
    Z('30', 'maul_vorn', GE, 'geöffnetes Maul', 'hellblau', ('flut', dict(waende=['hellblau'], saaten=[(600, 440)], box=(510, 325, 700, 520))),
      'Maul', teile=[22, 23], modell=['Maulinneres_lage'], eltern='gesicht_vorn', z='Z4'),
    Z('31', 'unterkiefer_vorn', GE, 'Teil des Unterkiefers', 'lila', ('flut_band', dict(waende=['rot', 'hellblau'], saaten=[(650, 415)],
      box=(595, 330, 705, 500), band_farbe='lila', band_box=(600, 350, 700, 480), band_r=9)), HT, teile=[23],
      modell=['Unterkiefer_aussen'], eltern='gesicht_vorn', z='Z4'),
    Z('32', 'arm_hand_krallen_vorn_rechts', HA, 'vorderer rechter Arm mit Krallen', 'rot', ('flut', dict(waende=['rot', 'hellblau', 'lila'],
      saaten=[(345, 440), (360, 520), (360, 600), (312, 592), (440, 545)], box=(290, 395, 470, 620))), HT,
      teile=[16, 17, 18, 19, 20, 29, 31], namensteile=[16, 17, 18, 19, 20, 29, 31]),
    Z('33', 'arm_vorn_rechts', HA, 'vorderer rechter Arm', 'braun',
      ('flut', dict(waende=['rot', 'lila'], saaten=[(345, 440)], box=(290, 395, 470, 620))), HT, teile=[16, 17, 18],
      eltern='arm_hand_krallen_vorn_rechts'),
    Z('34', 'hand_vorn_rechts', HA, 'vordere rechte Hand mit Krallen', 'lila',
      ('flut', dict(waende=['rot', 'lila', 'hellblau'], saaten=[(360, 520)], box=(290, 395, 470, 620))), HT, teile=[18, 19, 20, 29, 31],
      eltern='arm_hand_krallen_vorn_rechts'),
    Z('35', 'krallen_vorn_rechts', HA, 'Krallen', 'hellblau', ('flut', dict(waende=['rot', 'hellblau'], saaten=[(360, 600), (312, 592), (440, 545)],
      box=(290, 500, 470, 620))), 'Kralle', teile=[19, 20, 29, 31], eltern='hand_vorn_rechts'),
    Z('36', 'vorderbein_links', VL, 'Vorderes linkes Bein mit Krallen', 'rot', ('flut', dict(waende=['rot', 'hellblau', 'lila'],
      saaten=[(420, 250), (450, 440)], box=(335, 85, 575, 550))), HT, teile=[11, 12, 13, 14, 15, 28, 30],
      namensteile=[11, 12, 13, 14, 15, 28, 30]),
    Z('37', 'beinabschnitt_vorn_links', VL, 'Beinabschnitt', 'grau',
      ('flut', dict(waende=['rot', 'lila', 'hellblau'], saaten=[(424, 284)], box=(335, 85, 575, 550))), HT, teile=[11, 12],
      eltern='vorderbein_links'),
    Z('38', 'fussabschnitt_vorn_links', VL, 'Fußabschnitt', 'lila',
      ('flut', dict(waende=['rot', 'lila', 'hellblau'], saaten=[(450, 440)], box=(335, 85, 575, 550))), HT, teile=[13, 14, 15, 28, 30],
      eltern='vorderbein_links'),
    Z('39', 'krallen_vorn_links', VL, 'Exakt 4 Krallen', 'hellblau', ('loecher', dict(farbe='hellblau', box=(378, 438, 572, 546))), 'Kralle',
      teile=[13, 14, 15, 28, 30], eltern='fussabschnitt_vorn_links'),
]
# Modellgliederung im Kopf (Z4): Modellteile OHNE eigenes Nutzerwort. Material daher "nicht belegt" (Pruefer r1 Nr. 9: nichts dazuerfinden).
# (nr, id, Beschreibung (unser Text, KEIN Nutzerwort), Material, Modellmenge, Bezug-Nutzerzone)
NB = 'nicht belegt (kein Wort von Ihnen)'
MODELLZONEN_KOPF = [
    ('40', 'augenhoehle', 'Flächen rund um die Augen, die dieselbe rote Texturstelle wie die heutige Maul-Farbe benutzen (Modellgliederung)', NB, 'Augenhoehle', 'augen_vorn'),
    ('41', 'zaehne', 'Zähne (Modellgliederung, im "geöffneten Maul")', NB, 'Zahn', 'maul_vorn'),
    ('42', 'lippe', 'Oberlippe (Modellgliederung nach Lage: Rand der Maul-Öffnung am Kopf, auch bei geschlossenem Maul außen)', NB, 'Lippe_oben', 'maul_vorn'),
    ('44', 'hals_unterseite', 'Hals-Unterseite (Modellgliederung)', 'Haut (nicht belegt)', 'Hals_unten', None),
    # Runde 3 (Pruefer r2 Nr. 11): Flaechen von Kopf 22 aus der FARBliste "Maulinneres", die nach LAGE aussen liegen (g_maul.py);
    # darunter die 6 Aussenflaechen der Kopf-Unterseite, die die roten Hals-Streifen machen (Runde 4: im ganzen Modell gemessen, g_kopfunterseite.py)
    ('45', 'kopf_unterseite_maulfarbe', 'Kopf-Unterseite hinter dem Unterkiefer: Außenflächen, die heute die rote Maul-Farbe tragen (Modellgliederung; die roten Hals-Streifen)',
     'Haut (nicht belegt)', 'Maulfarbe_aussen', None),
]
