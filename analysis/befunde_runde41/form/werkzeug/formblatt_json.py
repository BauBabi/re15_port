#!/usr/bin/env python3
"""Erzeugt analysis/befunde_runde41/form/formblatt.json. Zahlen und Begruendung: FORMBLATT.md.

    python analysis/befunde_runde41/form/werkzeug/formblatt_json.py
"""
import json
import os

FORM = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))


def lade(n):
    return json.load(open(os.path.join(FORM, n), encoding='utf-8'))


AENDERUNGEN = [
 {
  "nr": "A1",
  "stelle": "Kante Bild 2 rechts (1.4, 7)",
  "vorher": "(390,195)-(397,207)-(418,225), sicher",
  "jetzt": "(378,175)-(390,181)-(394,187)-(411,206)",
  "beleg": "Rohr endet bei (411,206): R 194 bei y=204, 117 bei y=208. Alte Linie lag dort bei y=219, auf Band Q4"
 },
 {
  "nr": "A2",
  "stelle": "Kante Bild 2 oben (0, 1.4, 3.2)",
  "vorher": "x258-418 sicher",
  "jetzt": "belegt nur x>=354, davor gelesen",
  "beleg": "Tuer ist schwarz, dort keine Verdeckung"
 },
 {
  "nr": "A3",
  "stelle": "Sohlen K2, K3, K4 (0, 1.2, 3.1, W4)",
  "vorher": "wahrscheinlich",
  "jetzt": "offen",
  "beleg": "Kantenbreite 9-26 px gegen 6-8 px am Koerper. K3 liegt auf C5, das offen ist"
 },
 {
  "nr": "A4",
  "stelle": "Fussviereck, Gier, Achse (1.2, 1.3, 3.1)",
  "vorher": "2260-2850 x 1090-1430, 15-20 Grad, [g]",
  "jetzt": "1750-2850 x 750-2050, 15-27 Grad, Gier 65-197, [s]",
  "beleg": "Folge aus A3"
 },
 {
  "nr": "A5",
  "stelle": "Rundling (1.4, 7, W7)",
  "vorher": "Umriss wahrscheinlich, Pruefpunkte auf der Aussenkante",
  "jetzt": "Masse wahrscheinlich, Umriss offen, nur linke Kante pruefen",
  "beleg": "C4 rechts Y 21-25 px, unten 9-20 px, links 7 px"
 },
 {
  "nr": "A6",
  "stelle": "Bodenschatten (1.3, W18)",
  "vorher": "Kreis R 1170-1240, wahrscheinlich",
  "jetzt": "nicht messbar",
  "beleg": "Grenze bis (900,-401), 1850 von der Mitte"
 },
 {
  "nr": "A7",
  "stelle": "Zone (345-378,362-402) (1.4, W7)",
  "vorher": "Bodenschatten",
  "jetzt": "nicht Figur, Art offen",
  "beleg": "liegt ausserhalb des alten Kreises, Boden hat eigenes Tuerkis"
 },
 {
  "nr": "A8",
  "stelle": "Ganze Figur quer (1.3)",
  "vorher": "3370",
  "jetzt": "2150-2950, Arbeitswert 2350",
  "beleg": "233 px / 69 px je 1000 rechnet eine Tiefe, die Enden liegen auf 4580 und 6900. Pruefmodell selbst 2320"
 },
 {
  "nr": "A9",
  "stelle": "Kuppel plus Schulter (1.3)",
  "vorher": "2390-3140",
  "jetzt": "1750-2400, Arbeitswert 2300",
  "beleg": "wie A8"
 },
 {
  "nr": "A10",
  "stelle": "Rand rechts Bild 1 (1.4, 7)",
  "vorher": "x=430 bei y=292",
  "jetzt": "(429,272), Kante nur y 238-278",
  "beleg": "Zeilen 292, 296: Y 30-41 flach ueber x 400-456"
 },
 {
  "nr": "A11",
  "stelle": "Haken oben (3.2, 7, W15, 6F)",
  "vorher": "oben getrennt, Luecke als Pruefpunkt",
  "jetzt": "offen, Pruefpunkt gestrichen",
  "beleg": "V (33,6,13) +-(3,3,5) im Rauschen von Tuer und Rumpf"
 },
 {
  "nr": "A12",
  "stelle": "Linke Grenze Bild 2 (1.4, W8)",
  "vorher": "Grenze x218",
  "jetzt": "Mindestausdehnung",
  "beleg": "Balken endet selbst dort, Raum R 46-57 bei x 205-215"
 },
 {
  "nr": "A13",
  "stelle": "Weisse Flaeche unten Bild 2 (1.4, W12)",
  "vorher": "Feuer",
  "jetzt": "Boden",
  "beleg": "Video R 205-215, Raum 226"
 },
 {
  "nr": "A14",
  "stelle": "Viertes Bein (0, 1.4, 6A)",
  "vorher": "ganz verdeckt",
  "jetzt": "verdeckt oder in C1, Fuss wie A2 nur bei (321,374)",
  "beleg": "Spanne 299-341 bei y=362, Fuss A 43 px, Sohle 12 Zeilen tiefer"
 },
 {
  "nr": "A15",
  "stelle": "C1 und A1 Breite (1.4)",
  "vorher": "36 gegen 34 px",
  "jetzt": "je Zeile: C1 46 auf 35, A1 33 auf 37",
  "beleg": "Halbwertkanten y=320-356"
 },
 {
  "nr": "A16",
  "stelle": "Kopf (0, 5)",
  "vorher": "keine Erhebung in beiden Bildern",
  "jetzt": "nur vor hellem Grund ausgeschlossen",
  "beleg": "Bild 1 x 250-276, Bild 2 x<354 dunkel auf dunkel"
 },
 {
  "nr": "A17",
  "stelle": "Farbton Grundhaut (0, 2.1)",
  "vorher": "wahrscheinlich",
  "jetzt": "offen",
  "beleg": "`farbe.md` Abschnitt 7: Bild 1 im Rauschen, Bild 2 Rot bis +21 unsicher"
 },
 {
  "nr": "A18",
  "stelle": "Zug Q (2.2, 2.3)",
  "vorher": "Q2, Q3, Q5 getrennt",
  "jetzt": "ein Zug, wahrscheinlich",
  "beleg": "Blau-Grat zeilenweise bis P, Stoerung nur y 242-254"
 },
 {
  "nr": "A19",
  "stelle": "Z9 Schraegstrich (2.2)",
  "vorher": "fehlte",
  "jetzt": "aufgenommen",
  "beleg": "(411,240)-(420,262), `schraegstrich_beule` aus Gesamtbau"
 },
 {
  "nr": "A20",
  "stelle": "Bauchkante (1.3, 3.1)",
  "vorher": "y=320, 890-1200",
  "jetzt": "L2 y=313, L1 y=322, 850-1310",
  "beleg": "L2-Spitze: Y 77 bei y=312, 145 bei y=316"
 },
 {
  "nr": "A21",
  "stelle": "Bein NR Ansatz h (3.1)",
  "vorher": "870-1060",
  "jetzt": "910-1260",
  "beleg": "(325,318) bei Tiefe 6900-5000, wie bei C1 gerechnet"
 },
 {
  "nr": "A22",
  "stelle": "Neigung Bein FR (3.1)",
  "vorher": "unter 1 Grad",
  "jetzt": "bis 3 Grad",
  "beleg": "Mitte 397,5 bei y=320, 399,5 bei y=348"
 },
 {
  "nr": "A23",
  "stelle": "V4 (1.4, 7, W9)",
  "vorher": "x=405",
  "jetzt": "x rund 408",
  "beleg": "Rohr liegt im Video 3 px links der Warp-Lage"
 },
 {
  "nr": "A24",
  "stelle": "Sichtfrage Bild 2 (3.2, 6B)",
  "vorher": "Fuss-Treffer als Beleg",
  "jetzt": "nur Vertraeglichkeit",
  "beleg": "Pose kann anders sein, K2 bis K4 offen"
 },
 {
  "nr": "A25",
  "stelle": "Scheitel Bild 2 (3.2)",
  "vorher": "sicher als Stelle",
  "jetzt": "wahrscheinlich",
  "beleg": "keine Verdeckung bei x=320"
 },
 {
  "nr": "A26",
  "stelle": "Marken gleiche Hoehe (2.3)",
  "vorher": "gleiche Hoehe, wahrscheinlich",
  "jetzt": "gleiche Bildzeile sicher, Hoehe offen",
  "beleg": "h folgt nur aus angenommener Tiefe"
 }
]

bau = lade('lesung_gesamtbau.json')
pal = lade('farbe_palette.json')


def polygon(bild, name):
    for t in bau['teile']:
        if t['bild'] == bild and t['name'] == name:
            return t['polygon']
    raise KeyError(name)


def wert(v, ken, sich, arbeit=None, einheit='Spieleinheiten'):
    d = {'wert': v, 'kennung': ken, 'sicherheit': sich, 'einheit': einheit}
    if arbeit is not None:
        d['arbeitswert'] = arbeit
    return d


def teil(id_, name, bild, quelle, bereich, sich, mass=None, kennung='g'):
    return {'id': id_, 'name': name, 'bild': bild, 'bereich_px': bereich, 'sicherheit': sich, 'kennung': kennung,
            'mass': mass or {}, 'polygon': polygon(bild, quelle) if quelle else None}


mat = {}
for m in pal['materialien']:
    mat[m['name']] = {k: m.get(k) for k in ('titel', 'oberflaeche_rgb', 'oberflaeche_fehler', 'oberflaeche_von', 'oberflaeche_bis', 'art', 'sicherheit')}
mat['grundhaut_dunkel']['sicherheit'] = 'dunkel: sicher, Farbton: offen'
lage = {'grundhaut_dunkel': 'ganzer Koerper', 'rotbraun_mittelton': 'um die helle Zeichnung, Hakeninneres', 'rosa_stelle': 'nur Z1',
        'helle_baender': 'Z2, Z4, Z5, Z6, Q4, Haken-Rand', 'feine_linien': 'Z7, Z8, Q1, Q3, Q5', 'untere_streifen': 'Unterstreifen G, Obergrenze',
        'weisskerne': 'Z3, Spitze P, nur Mindesthelligkeit'}
for k, v in lage.items():
    mat[k]['lage'] = v

j = {
    'stand': '2026-10-31',
    'ausfuehrlich': 'FORMBLATT.md',
    'formvorlagen': ['bild1.png', 'bild2.png'],
    'kennung': {'g': 'gemessen', 's': 'geschaetzt', 'f': 'frei'},
    'sicherheit': ['sicher', 'wahrscheinlich', 'offen'],
    'lesart': 'Pixel im Originalbild 640x480. Spanne bei g: gemessen, aber abhaengig von der unbekannten Tiefe des Teils. h = Hoehe ueber Boden, y nach unten negativ im Spiel.',
    'gegenprobe': {'dossier': 'formblatt_gegenprobe.md', 'aenderungsliste': 'FORMBLATT.md Abschnitt 10, hier Feld aenderungsliste'},
    'massstab': {
        'leon_hoehe': wert(3005, 'g', 'sicher'),
        'leon_hoehe_bilder': [2962, 3009],
        'frau_hoehe': wert(2785, 'g', 'sicher'),
        'bild1_px_je_1000_quer': {'374': 58.3, '395': 67.5, '416': 76.7, '433': 84.1},
        'bild2_px_je_1000_quer': {'5000': 89.0, '6000': 74.6, '7000': 64.1, '8000': 56.4},
        'kamera': ['kamera_bild1.json', 'kamera_bild2.json'],
        'augenhoehe_bild1': 2180,
        'augenhoehe_bild2': 3256,
    },
    'koerperrahmen': {
        'erklaerung': 'Ursprung Mitte Fussviereck. Ende N kameranah in Bild 1 (u<0), Ende F kamerafern (u>0), Seite R rechts in Bild 1 (v<0), Seite L links (v>0). Vorn und hinten sind nicht vergeben.',
        'bild1': {'O': [-18514, 0, -18726], 'u': [0.989, 0, -0.147], 'v': [0.147, 0, 0.989], 'kennung': 's', 'sicherheit': 'offen, haengt an K2 bis K4',
                  'spanne_aus_sohlen': {'laenge': [1750, 2850], 'quer': [750, 2050], 'achse_zur_sehachse_grad': [15, 27], 'gier_ende_f_vorn': [65, 197]}},
    },
    'kontakte': [
        {'id': 'K1', 'bein': 'FR', 'bild': [398, 374], 'uv': [1414, -601], 'tiefe': 6910, 'kennung': 'g', 'sicherheit': 'sicher'},
        {'id': 'K2', 'bein': 'NL', 'bild': [232, 433], 'uv': [-1414, 601], 'tiefe': 4583, 'kennung': 'g', 'sicherheit': 'offen', 'letzte_harte_kante': [201, 400]},
        {'id': 'K3', 'bein': 'NR', 'bild': [312, 452], 'uv': [-1430, -492], 'tiefe': 4236, 'kennung': 'g', 'sicherheit': 'offen', 'letzte_harte_kante': [312, 414]},
        {'id': 'K4', 'bein': 'NR', 'bild': [352, 432], 'uv': [-839, -710], 'tiefe': 4731, 'kennung': 'g', 'sicherheit': 'offen', 'letzte_harte_kante': [312, 414]},
        {'id': 'K6', 'bein': 'FL', 'bild': [328, 370], 'uv': [1141, 610], 'tiefe': 7018, 'kennung': 's', 'sicherheit': 'offen',
         'mit_fuss_wie_a2': {'bild': [321, 374], 'fehler_px': 4, 'uv': [901, 623], 'tiefe': 6794}},
        {'id': 'K5', 'bein': 'Innenglied', 'bild': [258, 395], 'uv': [-307, 1043], 'tiefe': 5772, 'kennung': 'g', 'sicherheit': 'offen'},
    ],
    'gesamtmasse': {
        'scheitelhoehe': {'bild1': [2800, 3060], 'bild2': [2860, 3010], 'arbeitswert': 2950, 'kennung': 'g, Arbeitswert s', 'sicherheit': 'wahrscheinlich'},
        'bauchhoehe': {'wert': [850, 1310], 'arbeitswert': 1050, 'kennung': 'g, Arbeitswert s', 'sicherheit': 'wahrscheinlich', 'kante': 'L2 y=313, L1 y=322'},
        'rumpfdicke': {'arbeitswert': 1900, 'kennung': 's', 'sicherheit': 'wahrscheinlich'},
        'kuppelbreite_quer': {'wert': [1750, 2300], 'arbeitswert': 2000, 'kennung': 'g, Arbeitswert s', 'sicherheit': 'wahrscheinlich'},
        'kuppel_plus_schulter': {'wert': [1750, 2400], 'arbeitswert': 2300, 'kennung': 'g, Arbeitswert s', 'sicherheit': 'wahrscheinlich', 'bezug': 'in v, Kuppel Tiefe 5000-6000, Schulter Tiefe 6900'},
        'figur_quer': {'wert': [2150, 2950], 'arbeitswert': 2350, 'kennung': 'g, Arbeitswert s', 'sicherheit': 'wahrscheinlich', 'bezug': 'in v, links Tiefe 4580-5640, rechts 6900'},
        'fussviereck_laengs': {'wert': [1750, 2850], 'arbeitswert': 2550, 'kennung': 's', 'sicherheit': 'offen'},
        'fussviereck_quer': {'wert': [750, 2050], 'arbeitswert': 1230, 'kennung': 's', 'sicherheit': 'offen'},
        'kuppellaenge': {'wert': [1970, 2490], 'arbeitswert': 2000, 'kennung': 's', 'sicherheit': 'offen'},
        'gesamtlaenge': {'arbeitswert': 3400, 'kennung': 's', 'sicherheit': 'offen'},
        'bodenschatten': {'sicherheit': 'offen', 'hinweis': 'nicht messbar, kein Kreis belegt', 'rechte_grenze_uv': [[-1044, -847], [-362, -855], [3, -739], [419, -590], [900, -401]]},
    },
    'teile_bild1': [
        teil('R', 'Kuppel', 1, 'rumpf_kuppel', [247, 187, 381, 318], 'sicher'),
        teil('S', 'Schulter Seite R', 1, 'rechte_beule', [381, 238, 430, 300], 'wahrscheinlich', {'kuppe_h': 2230, 'ueber_beinachse': 530, 'rechte_kante': [429, 272], 'rechte_kante_lesbar_y': [238, 278]}),
        teil('A1', 'Bein FR Pfeiler', 1, 'pfeiler_A', [381, 300, 418, 358], 'sicher', {'breite_px': [33, 37], 'breite': [570, 640]}),
        teil('A2', 'Bein FR Fuss', 1, 'fuss_A', [378, 356, 426, 376], 'wahrscheinlich', {'breite': [760, 840], 'knoechel_h': 300}),
        teil('B1', 'Bein NL Saeule', 1, 'saeule_B', [226, 298, 287, 346], 'sicher', {'breite_px': 61, 'breite': [725, 795], 'h': [990, 1530]}),
        teil('B2', 'Bein NL Kugel oben', 1, 'kugel_B_oben', [200, 341, 252, 390], 'sicher', {'breite_px': 52, 'breite': 620, 'oberkante_h': 1045}),
        teil('B3', 'Bein NL Kugel unten', 1, 'kugel_B_unten', [200, 374, 252, 426], 'wahrscheinlich', {'kerbe': [206, 374]}),
        teil('B4', 'Innenglied', 1, 'innenglied_B', [250, 346, 285, 420], 'wahrscheinlich', {'breite_px': 35, 'breite': [420, 460]}),
        teil('B5', 'Masse unter B', 1, 'masse_unter_B', [205, 402, 262, 437], 'offen', {}, ''),
        teil('C1', 'Bein NR oben', 1, 'bein_C_oben', [300, 318, 347, 350], 'sicher', {'breite_px': [45, 36], 'breite_oben': [590, 770], 'breite_taille': [470, 620], 'rechte_kante_x': [342, 347], 'rechte_kante_y': [320, 368], 'hinweis': 'Verjuengung nur links, Bein FL kann in dieser Spalte stecken'}),
        teil('C3', 'Untermasse', 1, 'untermasse_C', [290, 353, 346, 400], 'wahrscheinlich', {'oberkante': [[285, 368], [325, 345]]}),
        teil('C4', 'Rundling', 1, 'rundling_C', [305, 396, 372, 448], 'Masse wahrscheinlich, Umriss offen', {'breite_px_hoechstens': [62, 67], 'breite_hoechstens': [690, 750], 'oberkante_h': 550, 'kantenbreite_px': {'links': 7, 'rechts': [21, 25], 'unten': [9, 20]}}),
        teil('C5', 'Block mit Querstreifen', 1, 'streifenfeld_unten', [288, 405, 330, 452], 'offen', {}, ''),
        teil('X', 'rosa-weisser Gegenstand', 1, 'rosa_feld_unten', [250, 420, 288, 479], 'offen', {}, ''),
    ],
    'nicht_figur_bild1': [
        {'id': 'L1', 'polygon': polygon(1, 'luecke_boden_2'), 'art': 'Boden sichtbar', 'sicherheit': 'sicher'},
        {'id': 'L2', 'polygon': polygon(1, 'luecke_boden_1'), 'art': 'Boden sichtbar', 'sicherheit': 'sicher'},
        {'id': 'SOCKEL', 'bereich_px': [425, 284, 455, 302], 'art': 'dunkler Wandsockel, kein Figurrand', 'sicherheit': 'sicher'},
        {'id': 'SCH', 'polygon': polygon(1, 'blauzone_kein_koerper'), 'art': 'Boden, Schatten oder Bodenton offen', 'beleg': 'Y 75-93, Kante rechts 25 px, harte Koerperkanten 6-8 px', 'sicherheit': 'nicht Figur: wahrscheinlich',
         'hinweis': 'maske_bild1.png schliesst (345-378,362-402) faelschlich als Figur ein'},
    ],
    'umriss_bild1': polygon(1, 'gesamtumriss'),
    'teile_bild2': [
        {'id': 'R', 'name': 'Kuppel', 'kante': [[258, 215], [268, 205], [278, 190], [290, 170], [300, 162], [320, 156], [345, 160], [354, 161], [362, 165], [370, 171], [378, 175], [390, 181], [394, 187], [411, 206]],
         'kante_belegt': [[354, 161], [362, 165], [370, 171], [378, 175], [390, 181], [394, 187], [411, 206]], 'kante_belegt_sicherheit': 'sicher bis x=378, danach wahrscheinlich',
         'kante_gelesen': [[258, 215], [268, 205], [278, 190], [290, 170], [300, 162], [320, 156], [345, 160]], 'kante_gelesen_sicherheit': 'wahrscheinlich, vor schwarzer Tuer keine Verdeckung',
         'sicherheit': 'siehe Teilkanten', 'beleg': 'R-Abfall an der Platte, Video unter Raum ab x=382, Rohr endet bei (411,206)'},
        {'id': 'U', 'name': 'Untermasse', 'bereich_px': [218, 286, 300, 372], 'sicherheit': 'sicher', 'beleg': 'V1, V2', 'hinweis': 'Mindestausdehnung, der Balken endet selbst bei x rund 218'},
        {'id': 'H', 'name': 'Haken', 'bereich_px': [205, 215, 245, 262], 'polygon': polygon(2, 'haken'), 'sicherheit': 'Form wahrscheinlich, Anschluss oben offen', 'verbindung_unten': {'y': [258, 272], 'bis': [262, 265]}},
        {'id': 'G', 'name': 'Unterstreifen', 'polygon': polygon(2, 'grat_unten'), 'linie': [[338, 320], [285, 358]], 'breite_px': 15, 'sicherheit': 'sicher'},
        {'id': 'F', 'name': 'V-Form', 'bereich_px': [263, 355, 304, 399], 'polygon': polygon(2, 'faecher'), 'sicherheit': 'offen'},
        {'id': 'Y', 'name': 'deckende Flaeche', 'ellipse': {'mitte': [239, 418], 'halbachsen': [43, 33]}, 'sicherheit': 'deckend sicher, Figur offen'},
        {'id': 'P', 'name': 'Spitze', 'bereich_px': [358, 296, 374, 313], 'polygon': polygon(2, 'lichtspitze'), 'sicherheit': 'wahrscheinlich'},
        {'id': 'rand_rechts', 'mindestens_x': 413, 'x': [416, 420], 'y': [225, 262], 'sicherheit': 'wahrscheinlich', 'beleg': 'V4', 'hinweis': 'Feuer ab x 415-421 kann davor liegen'},
        {'id': 'unterkante_rechts', 'mindestens_y': 345, 'x': [340, 364], 'sicherheit': 'wahrscheinlich', 'beleg': 'V3'},
    ],
    'verdeckungen_bild2': [
        {'id': 'V1', 'stelle': [[222, 292], [298, 292]], 'aussage': 'Lichtbalken ganz verdeckt', 'sicherheit': 'sicher'},
        {'id': 'V2', 'stelle': [[232, 347], [285, 346]], 'aussage': 'Bodenlinie ganz verdeckt', 'sicherheit': 'sicher'},
        {'id': 'V3', 'stelle': [[340, 339], [364, 336]], 'aussage': 'Bodenlinie ohne Kontrast: R 0 bis +14 statt +26 bis +46', 'sicherheit': 'wahrscheinlich'},
        {'id': 'V4', 'stelle': [[408, 225], [408, 262]], 'aussage': 'Rohr fehlt: Video R 68-94, Raum 140-194', 'sicherheit': 'wahrscheinlich'},
        {'id': 'V6', 'stelle': [[411, 204], [411, 208]], 'aussage': 'Rohr endet: R 194 bei y=204, 117 bei y=208, Kante der Figur', 'sicherheit': 'wahrscheinlich'},
        {'id': 'V5', 'stelle': [[225, 419], [262, 414]], 'aussage': 'Querfuge ohne Kontrast: 0 +-5 statt +114 bis +134, bei x>=275 sichtbar', 'sicherheit': 'sicher'},
        {'id': 'BO', 'stelle': [[345, 382], [405, 396]], 'aussage': 'Boden sichtbar, R 207', 'sicherheit': 'sicher'},
    ],
    'zeichnung': {
        'regel': 'Textur mal Licht, rein diffus. In Bild 2 ist das Licht flach, jede Linie dort ist Textur.',
        'licht': {'bild1': pal['licht']['bild1']['faktor_figur'], 'bild2': pal['licht']['bild2']['faktor']},
        'materialien': mat,
        'stellen': [
            {'id': 'Z1', 'name': 'Rosa Spindel', 'bild': 1, 'punkt': [325, 231], 'bereich_px': [300, 220, 339, 259], 'achse_grad': 53, 'lage': 'Ende N, Mittellinie', 'h': [2245, 2265], 'groesse': [[570, 710], [300, 370]], 'material': 'rosa_stelle', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Z2', 'name': 'Querband A02', 'bild': 1, 'punkt': [317, 265], 'achse_grad': -31, 'lage': 'Ende N, Mittellinie', 'h': [1840, 1925], 'groesse': [[390, 480], [100, 130]], 'material': 'helle_baender', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Z3', 'name': 'Weisskern A03', 'bild': 1, 'punkt': [326, 293], 'lage': 'Mittellinie', 'h': [1450, 1630], 'groesse': [[260, 330], [150, 190]], 'material': 'weisskerne', 'sicherheit': 'Form wahrscheinlich, Farbe offen'},
            {'id': 'Z4', 'name': 'Band A04', 'bild': 1, 'punkt': [320, 303], 'lage': 'direkt unter Z3', 'groesse': [[260, 320], [90, 120]], 'material': 'helle_baender', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Z5', 'name': 'Kappe', 'bild': 1, 'bereich_px': [385, 277, 415, 296], 'lage': 'oberes Ende Bein FR', 'h': [1280, 1590], 'groesse': [515, 310], 'material': 'helle_baender', 'sicherheit': 'sicher als Stelle'},
            {'id': 'Z6', 'name': 'Quertrio', 'bild': 1, 'punkte': [[329, 351], [322, 366], [314, 384]], 'abstaende_px': [16, 20], 'lage': 'Bein NR, A14 an der Taille', 'groesse': [[230, 290], [70, 90]], 'material': 'helle_baender', 'sicherheit': 'A14 wahrscheinlich, A15 und A16 offen'},
            {'id': 'Z7', 'name': 'Bogen und Knickband', 'bild': 1, 'punkte': [[241, 346], [248, 372], [242, 383]], 'knick': [249, 381], 'lage': 'Oberseite der Kugel', 'bandbreite': [60, 110], 'material': 'feine_linien', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Z8', 'name': 'Dreierreihe', 'bild': 1, 'punkte': [[241, 289], [253, 285], [272, 284]], 'abstaende_px': [12, 19], 'lage': 'Uebergang Rumpf zu Bein NL', 'groesse': [[140, 200], [65, 80]], 'material': 'feine_linien', 'sicherheit': 'Lage wahrscheinlich, Bedeutung offen'},
            {'id': 'Z9', 'name': 'Schraegstrich', 'bild': 1, 'linie': [[411, 240], [420, 262]], 'lage': 'Schulter Seite R, 4-7 px innerhalb der Aussenkante', 'groesse_px': [33, 5], 'textur_rund': [85, 80, 60], 'sicherheit': 'wahrscheinlich'},
            {'id': 'Q1', 'name': 'Bogen links', 'bild': 2, 'bereich_px': [262, 168, 294, 225], 'breite_px': 9.5, 'material': 'feine_linien', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Q2', 'name': 'Scheitelsichel', 'bild': 2, 'bereich_px': [300, 157, 345, 195], 'material': 'rotbraun_mittelton', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Q3', 'name': 'Bogen rechts', 'bild': 2, 'punkt': [375, 205], 'groesse_px': [35, 4.6], 'abstand_zur_kante_px': 21, 'material': 'feine_linien', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Q4', 'name': 'Randband', 'bild': 2, 'punkt': [411, 217], 'beginn': [403, 210], 'groesse_px': [34, 6], 'abstand_zur_kante_px': 8, 'material': 'helle_baender', 'sicherheit': 'wahrscheinlich'},
            {'id': 'Q5', 'name': 'Innenlinie', 'bild': 2, 'linie': [[392, 240], [362, 296]], 'material': 'feine_linien', 'sicherheit': 'wahrscheinlich'},
        ],
        'zug_q': {'bild': 2, 'aussage': 'Unterkante Q2, Q3, Q5 und P sind wahrscheinlich eine Linie', 'linie': [[300, 192], [314, 189], [345, 189], [353, 194], [372, 201], [384, 218], [388, 236], [383, 260], [374, 278], [363, 290], [365, 302]],
                  'stoerung_y': [242, 254], 'stoerung_x': [393, 398], 'sicherheit': 'wahrscheinlich'},
        'regeln': [
            'Keine gleichmaessige Ringelung: Gipfel auf Bein NR bei 28, 60, 16 px, Bein FR unter Z5 ohne Band (sicher).',
            'Gleiche Bildzeile y 277-310 fuer Z5, Z3/Z4, Z8 (sicher). Gleiche Hoehe h 1300-1700 nur, wenn jede Marke auf der Tiefe ihres Beins liegt (offen).',
            'Stapel Z1-Z4, Z6 auf einer Senkrechten x 314-329 ueber 150 Zeilen (sicher).',
            'Rumpfinneres Bild 2 (290-340,225-280) ohne Zeichnung, V (40,6,16) (sicher).',
            'Zug Q als durchgehende Linie malen (wahrscheinlich).',
        ],
        'punkte_ohne_deutung': ['A10 (291,208)', 'A05', 'A06', 'B10 (280,275)', 'B11 (258,263)', 'B12 (217,274)'],
    },
    'posen': {
        'pose1': {
            'bild': 1,
            'lader_gier': {'ende_n_vorn': 2145, 'ende_f_vorn': 97, 'kennung': 's', 'spanne_ende_f_vorn': [65, 197]},
            'lage': {'O': [-18514, 0, -18726], 'sicherheit': 'Weltlage offen, Rahmen offen'},
            'achse_zur_sehachse_grad': [15, 27],
            'gelenke': [
                {'gelenk': 'Scheitel', 'bild': [[300, 187], [322, 187]], 'h': [2800, 3060], 'kennung': 'g', 'sicherheit': 'sicher als Stelle'},
                {'gelenk': 'Bauchkante', 'bild': [[293, 313], [362, 322], [381, 322]], 'h': [850, 1310], 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'FR Schulterkuppe', 'bild': [405, 238], 'h': 2230, 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'FR oberes Gelenk Spitze', 'bild': [400, 277], 'h': 1590, 'kennung': 'g', 'sicherheit': 'Stelle sicher, Gelenk wahrscheinlich'},
                {'gelenk': 'FR oberes Gelenk Basis', 'bild': [398, 296], 'h': 1280, 'kennung': 'g', 'sicherheit': 'Stelle sicher, Gelenk wahrscheinlich'},
                {'gelenk': 'FR Unterglied', 'achse_x': [397.5, 399.5], 'y': [320, 356], 'neigung_bildebene_grad_bis': 3, 'kennung': 'g', 'sicherheit': 'sicher'},
                {'gelenk': 'FR Knoechel', 'bild': [398, 356], 'h': 300, 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'FR Sohle', 'bild': [398, 374], 'h': 0, 'fussspitzen': [[377, 362], [426, 362]], 'kennung': 'g', 'sicherheit': 'sicher'},
                {'gelenk': 'NL Ansatz', 'bild': [257, 298], 'h': 1530, 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'NL Saeule unten', 'bild': [257, 346], 'h': 990, 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'NL Kugel Oberkante', 'bild': [232, 341], 'h': 1045, 'kennung': 'g', 'sicherheit': 'sicher'},
                {'gelenk': 'NL Sohle', 'bild': [232, 433], 'h': 0, 'kennung': 'g', 'sicherheit': 'offen'},
                {'gelenk': 'NR Ansatz', 'bild': [325, 318], 'h': [910, 1260], 'kennung': 'g', 'sicherheit': 'sicher als Stelle'},
                {'gelenk': 'NR Taille', 'bild': [326, 350], 'h': [390, 850], 'kennung': 'g', 'sicherheit': 'Taille sicher, Gelenk wahrscheinlich'},
                {'gelenk': 'NR Rundling Oberkante', 'bild': [335, 396], 'h': 550, 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'NR Sohle', 'bild': [[312, 452], [352, 432]], 'h': 0, 'kennung': 'g', 'sicherheit': 'offen'},
                {'gelenk': 'FL Fuss', 'bild': [328, 370], 'bild_mit_fuss_wie_a2': [321, 374], 'h': 0, 'kennung': 's', 'sicherheit': 'offen'},
            ],
            'nicht_messbar': ['Beugung in Blickrichtung aller Glieder', 'Neigung des Rumpfs', 'Kopf'],
        },
        'pose2': {
            'bild': 2,
            'hinweis': 'Kein Bodenkontakt sicher. Standort und Gier sind Proben, keine Messung. Fuss-Treffer zeigen nur Vertraeglichkeit: andere Szene, Pose kann anders sein, K2 bis K4 offen.',
            'staende': [
                {'gelenk': 'Scheitel', 'bild': [320, 156], 'h_je_abstand': {'5000': 3009, '6000': 2960, '7000': 2910, '8000': 2861}, 'kennung': 'g', 'sicherheit': 'wahrscheinlich, vor Tuerschwarz keine Verdeckung'},
                {'gelenk': 'Unterglied', 'linie': [[338, 320], [285, 358], [278, 398]], 'richtung_grad': [45, 62], 'kennung': 'g', 'sicherheit': 'sicher als Stelle'},
                {'gelenk': 'Haken', 'linie': [[215, 222], [243, 218], [245, 236], [222, 262]], 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
                {'gelenk': 'Spitze', 'bereich_px': [358, 296, 374, 313], 'kennung': 'g', 'sicherheit': 'wahrscheinlich'},
            ],
            'probe_sicht_flanke': {'O': [2625, 0, -23625], 'achse_a_grad': 85, 'lader_gier': {'ende_n_vorn': 1081, 'ende_f_vorn': 3129},
                                   'fuesse_bild': {'K1': [433, 413], 'K2': [206, 387], 'K3': [208, 437], 'K4': [259, 442], 'K6': [376, 368]}, 'kennung': 's', 'sicherheit': 'offen'},
            'probe_sicht_ende': {'O': [2625, 0, -23625], 'achse_a_grad': 340, 'lader_gier': {'ende_n_vorn': 2276, 'ende_f_vorn': 228},
                                 'fuesse_bild': {'K1': [256, 474], 'K2': [351, 351], 'K3': [284, 349], 'K4': [268, 367], 'K6': [369, 461]}, 'kennung': 's', 'sicherheit': 'offen'},
        },
    },
    'widersprueche': [
        {'nr': 'W1', 'thema': 'Menschmass', 'entscheidung': 'Leon 3005, Frau 2785', 'beleg': 'Modelle gerechnet'},
        {'nr': 'W2', 'thema': 'Kamera Bild 1', 'entscheidung': 'kamera.md', 'beleg': 'Raumgeometrie aus 5 Cuts'},
        {'nr': 'W3', 'thema': 'Groesse zwischen den Bildern', 'entscheidung': 'kein Widerspruch', 'beleg': '2800-3060 gegen 2860-3010'},
        {'nr': 'W4', 'thema': 'Zahl der Kontakte', 'entscheidung': 'vier Beine, K1 sicher, K2-K4 offen, viertes verdeckt oder in C1', 'beleg': 'K2-K4 auf weichen Kanten oder offenen Teilen'},
        {'nr': 'W5', 'thema': 'liegend oder getragen', 'entscheidung': 'beides: waagerecht, Bauch h 1050, nicht aufliegend', 'beleg': 'L1, L2 zeigen Boden'},
        {'nr': 'W6', 'thema': 'Glanz oder Oberflaeche', 'entscheidung': 'Oberflaeche', 'beleg': 'kein Glanz in der Engine, Bild 2 flaches Licht'},
        {'nr': 'W7', 'thema': 'Zone (345-378,362-402)', 'entscheidung': 'nicht Figur, Schatten oder Bodenton offen', 'beleg': 'Y 75-93, Kante rechts 25 px'},
        {'nr': 'W8', 'thema': 'linke Grenze Bild 2', 'entscheidung': 'unten mindestens bis x218, oben nicht begrenzt, Haken eigener Teil', 'beleg': 'V1 endet mit dem Balken'},
        {'nr': 'W9', 'thema': 'rechter Rand Bild 2', 'entscheidung': 'mindestens x=413, x 416-420 wahrscheinlich', 'beleg': 'V4'},
        {'nr': 'W10', 'thema': 'Unterkante rechts Bild 2', 'entscheidung': 'bis mindestens y=345, Rest offen', 'beleg': 'V3, BO'},
        {'nr': 'W11', 'thema': 'Standort Frau', 'entscheidung': 'Spawn (600,0,-25300)', 'beleg': 'Haar dunkel, Video y 165-355, Modell y 169-358'},
        {'nr': 'W12', 'thema': 'weisse Flaechen', 'entscheidung': 'Feuer, rechts unten Kisten, unten Mitte Boden', 'beleg': 'Spielfenster Cut 11, Raumfilter'},
        {'nr': 'W13', 'thema': 'Flaeche unten links Bild 2', 'entscheidung': 'deckend, kein Schatten, Figur offen', 'beleg': 'V5'},
        {'nr': 'W14', 'thema': 'weisser Riegel (254-278,447-459)', 'entscheidung': 'Bildinhalt, kein Logo', 'beleg': 'Minimum (1,1,4) ueber 174 Dias'},
        {'nr': 'W15', 'thema': 'Anschluss Haken', 'entscheidung': 'oben offen, unten verbunden', 'beleg': 'V (33,6,13) im Rauschen, Band V (37,12,25)'},
        {'nr': 'W16', 'thema': 'Oberkante Bild 1', 'entscheidung': 'sicher erst ab x=348', 'beleg': 'Hintergrundband y~195'},
        {'nr': 'W17', 'thema': 'Merkmale zwischen den Bildern', 'entscheidung': 'keines sicher', 'beleg': 'nur Kuppelform und Scheitelhoehe gemeinsam'},
        {'nr': 'W18', 'thema': 'Radius Schatten', 'entscheidung': 'nicht messbar', 'beleg': 'Grenze bis u=900, v=-401'},
    ],
    'nicht_gezeigt_frei': ['Kopf, Gesicht, Augen, Maul, Zaehne; Erhebung nur bei Bild 1 x 276-345 und Bild 2 x>=354 ausgeschlossen', 'Sohlen Bein NL und NR', 'Seite L', 'Ende F, ausser Sicht Ende stimmt', 'Unterseite', 'Oberseite nur streifend',
                           'Bein FL', 'Fussform, Zehen, Krallen', 'Beugung in Blickrichtung', 'Oberflaechenart', 'Grundhaut unter e~10',
                           'Zone unter dem Logo (0-253,436-480)', 'Umriss x=250-300 oben in Bild 1', 'Bild 2 (300-345,300-320)'],
    'deutungen': {
        'viertes_bein': [
            {'name': 'Bein FL hinter Bein NR oder in C1', 'arbeitsannahme': True, 'dafuer': 'Viereck trifft die Verdeckung und die Achse von C1, rechte Kante einer Kopie von A x=345 wie C1', 'dagegen': 'Bein nicht trennbar, Parallelogramm ist Annahme, Fuss wie A2 passt erst ab Sohlenzeile 374'},
            {'name': 'Innenglied ist das vierte Bein', 'arbeitsannahme': False, 'dafuer': 'eigener Streifen x250-285', 'dagegen': 'kein Viereck: v 730-1480, u -570 bis 480, Seite L fern ohne Stuetze'},
        ],
        'sicht_bild2': [
            {'name': 'Sicht Flanke (Seite R)', 'a_grad': [60, 100], 'ueberdeckung': [0.73, 0.90], 'arbeitsannahme': True,
             'dafuer': 'K6 trifft F1 (370,368); K3, K4 in Y, auch ohne Y im Suchziel; Z1 trifft den Haken; Scheitel passt; Zeichnung abgewandt', 'dagegen': 'Kuppel fast rund; Beine FR, FL in Feuerzonen; Fuss-Treffer setzen Pose aus Bild 1 und K2-K4 voraus', 'hinweis': 'Sichtfrage offen, nur Arbeitsannahme'},
            {'name': 'Sicht Ende (Ende F)', 'a_grad': [330, 350], 'ueberdeckung': [0.74, 0.92], 'arbeitsannahme': False,
             'dafuer': 'beste Kuppelform, Schulter auf dem Haken', 'dagegen': 'Bein FL stuende vor sichtbarem Boden R 207, Bein FR muesste das Band (210-250,376-384) R 162 kreuzen; beides setzt Pose aus Bild 1 voraus'},
            {'name': 'gleiches Ende wie Bild 1', 'a_grad': [120, 200], 'verworfen': True, 'grund': 'Zeichnung waere sichtbar'},
            {'name': 'Gegenflanke', 'a_grad': [250, 290], 'ueberdeckung': [0.63, 0.80], 'verworfen': True},
        ],
        'vorderende': [{'name': 'Ende N vorn', 'arbeitsannahme': True}, {'name': 'Ende F vorn', 'arbeitsannahme': False}],
        'stapel': [{'name': 'Vorsprung am Ende N', 'arbeitsannahme': True}, {'name': 'Mittelnaht', 'arbeitsannahme': False}, {'name': 'Oberglied Bein NR', 'arbeitsannahme': False}],
        'bein_nr': [{'name': 'durchgehendes Bein mit Taille', 'arbeitsannahme': True}, {'name': 'liegender Unterarm', 'arbeitsannahme': False}],
        'haken': [{'name': 'Profil des Vorsprungs', 'arbeitsannahme': True}, {'name': 'Schulter Seite R', 'arbeitsannahme': False}, {'name': 'erhobenes Glied', 'arbeitsannahme': False}],
        'gegenstand_x': [{'name': 'Koerperteil', 'arbeitsannahme': False}, {'name': 'Treffer- oder Blutwirkung', 'arbeitsannahme': False}, {'name': 'offen lassen', 'arbeitsannahme': True}],
        'flaeche_y': [{'name': 'Fuss Bein NR', 'arbeitsannahme': True}, {'name': 'eigenes Objekt', 'arbeitsannahme': False}, {'name': 'Schatten', 'verworfen': True, 'grund': 'Fuge verschwindet ganz'}],
        'unterstreifen': [{'name': 'Band auf Bein NR', 'arbeitsannahme': True}, {'name': 'eigener Strang', 'arbeitsannahme': False}],
    },
    'pruefpunkte': {
        'bild1': {'kamera': 'kamera_bild1.json', 'punkte': {
            'sohle_fr': [398, 374], 'kappe_spitze': [400, 277], 'kappe_basis': [398, 296], 'schulterkuppe': [405, 238], 'scheitel_y_bei_x_300_322': 187,
            'flanke_rechts': [[348, 205], [366, 215], [378, 233]], 'rand_links_x_bei_y_224_242': 247, 'rand_rechts': [429, 272],
            'bein_fr_x_bei_y_324': [381, 414], 'bein_fr_x_bei_y_344': [381, 418], 'bein_nr_x_bei_y_322': [300, 347], 'bein_nr_x_bei_y_346': [308, 344], 'bein_nl_x_bei_y_318_342': [226, 287],
            'rundling_linke_kante': [[306, 405], [312, 414]], 'stange_sichtbar_bis': [280, 191]}, 'boden_frei': ['L1', 'L2', 'SCH']},
        'bild2': {'kamera': 'kamera_bild2.json', 'kante': 'teile_bild2 R kante_belegt', 'figur_deckt': ['V1', 'V2', 'V3', 'V4', 'V6'], 'boden_frei': [[345, 382, 405, 396], [278, 402, 336, 416]],
                  'haken': [205, 215, 245, 262]},
    },
    'pruefmodell': {
        'hinweis': 'NUR Messhilfe aus Bild 1, keine Formvorgabe. Ellipsoide im Koerperrahmen: mitte (u, y, v), y negativ nach oben; halbachsen (laengs, hoch, quer).',
        'teile': [
            {'name': 'kuppel', 'mitte': [102, -2023, 149], 'halbachsen': [1250, 911, 960]},
            {'name': 'schulter', 'mitte': [1297, -1689, -796], 'halbachsen': [500, 556, 417]},
            {'name': 'pfeiler_fr', 'mitte': [1386, -649, -624], 'halbachsen': [300, 677, 288]},
            {'name': 'fuss_fr', 'mitte': [1412, -104, -639], 'halbachsen': [500, 129, 389]},
            {'name': 'saeule_nl', 'mitte': [-995, -1194, 516], 'halbachsen': [400, 327, 380]},
            {'name': 'kugel_nl', 'mitte': [-1310, -492, 764], 'halbachsen': [400, 488, 303]},
            {'name': 'innenglied', 'mitte': [-1024, -455, 365], 'halbachsen': [400, 458, 216]},
            {'name': 'bein_nr_oben', 'mitte': [3, -872, 63], 'halbachsen': [300, 244, 313]},
            {'name': 'untermasse_nr', 'mitte': [-738, -525, -209], 'halbachsen': [400, 304, 355]},
            {'name': 'rundling_nr', 'mitte': [-931, -146, -545], 'halbachsen': [400, 264, 369]},
            {'name': 'pfeiler_fl', 'mitte': [1113, -649, 587], 'halbachsen': [300, 677, 288]},
            {'name': 'fuss_fl', 'mitte': [1139, -104, 572], 'halbachsen': [500, 129, 389]},
        ],
    },
}

j['aenderungsliste'] = AENDERUNGEN

with open(os.path.join(FORM, 'formblatt.json'), 'w', encoding='utf-8') as f:
    json.dump(j, f, ensure_ascii=False, indent=1)
print('geschrieben', len(json.dumps(j)))
