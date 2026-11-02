#!/usr/bin/env python3
"""Formblatt-Vorlage auf den Stand der Gegenprobe heben. Original liegt in gegenprobe/vorher/formblatt2_vorlage.md.
Liest immer das Original, damit das Skript wiederholbar ist."""
import json, os

H = os.path.dirname(os.path.abspath(__file__))
G = os.path.join(H, '..', 'gerippe')
t = open(os.path.join(H, 'vorher', 'formblatt2_vorlage.md'), encoding='utf-8').read()
prof = json.load(open(os.path.join(H, 'profil_urteil.json'), encoding='utf-8'))


def ers(a, b):
    global t
    assert a in t, a[:70]
    t = t.replace(a, b)


def block(anfang, ende, neu):
    """Ersetzt von Zeile mit anfang bis vor die Zeile mit ende."""
    global t
    i = t.index(anfang)
    j = t.index(ende, i + len(anfang))
    t = t[:i] + neu + t[j:]


ers("Stand 2026-11-01. Bindend ist die Lesung", "Stand 2026-11-01, nach der Gegenprobe (`gerippe_gegenprobe.md`, Aenderungsliste Abschnitt 10). Bindend ist die Lesung")

block("- Das Gerippe hat 30 Knochen", "## 1. Die acht Teile", """- Das Gerippe hat 30 Knochen und erklaert beide Bilder mit derselben Figur: gleiche Laengen, gleiche Dicken, nur Winkel, Standort und Gier verschieden. Ganzzahl-Probe ueber `form_emd.pose_aus_bild`: hoechstens 13 Einheiten, 1,0 Pixel.
- Restfehler (IoU sichtbares Modellteil gegen Nutzer-Teil): Schwanz 0,86, Bein links 0,78, Bein rechts 0,81, Arm rechts 0,79, Koerperstueck 0,69, Oberkoerper 0,77, Arm links 0,70, Arm rechts vorn 0,57. Kantenabstand im Median 0-3,7 Pixel. Die Zahlen von Bild 2 sind nicht mit der ersten Fassung vergleichbar: Die Oberarme zaehlen nicht mehr als Rumpf (Abschnitt 10, Punkt 1). Oberkoerper-Umriss wird zu 0,93 gedeckt, weil das rechte Bein die untere Spitze fuellt.
- Masse: Rumpf rund 2240, Arm 1910 (Oberarm 800, Unterarm 1110), Bein 1780, Schwanz 3820, Kopf 1300, Hals 500, Hand 690, Klaue 610, Fuss 900. Schulterabstand 1800, Hueftgelenke 1120.
- Pose 1 (Bild 1): auf allen vieren, Huefte Hoehe 1880, Ruecken waagerecht, Brust steigt 25 Grad, Kopf haengt 33 Grad vorn herab (verdeckt). Schwanz steigt auf 2940 und faellt zur Kamera. Blick fast laengs.
- Pose 2 (Bild 2): Rumpf gerade und 28 Grad aufgerichtet, Gier -16, also 36 Grad gegen die Bildachse gedreht. Huefte links hinten im Bild, Brust rechts, Scheitel 3140 auf der Brust. Kopf auf 56 Grad gegen die Brust zur Kamera gedreht, aufrecht, 20 Grad neben der Kamera, Rand auf Zug Q. Rechter Arm: Ellbogen unter der Rumpfkante, Unterarm auf dem gerippten Streifen, Hand am Boden, Finger zum Logo. Linker Arm von der Schulter hinter dem Kopf herab. Beine in der Hocke unter dem Bauch.
- Fest belegt: Lage und Richtung aller sichtbaren Glieder, Dicken, Kante R, Verdeckungen. Nicht belegt: Gesicht, Kopflaenge, Kieferform, Rumpflaenge, Oberarmlaenge, Fusslaenge, Lage des linken Arms in Bild 1, Lage der Beine und des Schwanzes in Bild 2, Fingerzahl.
- Gier Bild 2 ist nicht mehr gesetzt: Vier Starts von -5 bis -28 endeten bei -12 bis -15, die Linie der Handgelenke allein ergibt -14,4. Profil: -40 bis 0 gleich gut, 21 schlechter.

""")

ers("Lesebefund Bild 2: Beide Arme kommen unter der unteren rechten Rumpfkante hervor. Der Rumpf liegt also vor den Armansaetzen.",
    """Lesebefund Bild 2: Beide Arme kommen unter der unteren rechten Rumpfkante hervor. Der Rumpf liegt also vor den Armansaetzen.

Achse des rechten Unterarms: Die rechte Kante des Streifens ist gerade, (335,326) bis (274,408). Der obere Teil liegt unter dem Rumpf, also ist nur diese Kante echt. Achse = Kante, 20 px nach links oben: (319,314) bis (258,396). Die Spitze (337,316) ist die Ellbogenkappe.""")

ers("Winkelgrenzen stehen in `gerippe.json` (`winkelgrenzen_grad`): Knie 0 bis 165, Ellbogen 0 bis 155, Kiefer 0 bis 50, Schwanz je Abschnitt +-50.",
    "Winkelgrenzen stehen in `gerippe.json` (`winkelgrenzen_grad`): Knie -2 bis 165, Ellbogen -2 bis 155 (in Beugerichtung gezaehlt), Kiefer 0 bis 50, Schwanz je Abschnitt +-50.")
ers("Hinweis zum Kopf: Dicke a Ende 750 und Laenge 1220 kommen nur aus der Vorderansicht in Bild 2. Von der Seite ist der Kopf nirgends zu sehen, die Form ist [f].",
    "Hinweis zum Kopf: Der Querschnitt (Halbachsen rund 530 hoch, 560 breit) kommt aus Zug Q in Bild 2. Laenge 1300 liegt an der Grenze des Knochens und ist [f]: Von der Seite ist der Kopf nirgends zu sehen. Das Ende darf nicht dicker sein als die Mitte. Der Kiefer haengt bei x 500 unter dem Kopf und reicht bis 92 % der Kopflaenge, Form [f].")

ers("- Verdeckt bleiben muessen Kopf und linker Arm in Bild 1, der Schwanz in Bild 2.", "- Verdeckt bleiben muessen Kopf und linker Arm in Bild 1, der Schwanz und der rechte Oberarm in Bild 2. Der linke Oberarm zaehlt in Bild 2 zum Oberkoerper-Umriss: Er liegt rechts von Zug Q hinter dem Kopf.")
ers("- 32 Starts mit gestreuter Gier,", "- Anatomie-Kosten seit der Gegenprobe (`fit.py --anatomie`): Kopf aufrecht und zur Kamera, Glieder ausserhalb des Rumpfes, Klauen am Boden, Finger und Hand mit Mindestdicke, Rumpf ohne Kragen, Wirbelsaeule glatt, Oberarm mindestens 0,72 Unterarm, Kiefer vorn.\n- Gemessene Kante R und Verdeckungen V1-V5 aus FORMBLATT als Kosten (`--kante`): belegte Kante beidseitig, gelesene Kante nur als Mindestausdehnung.\n- Der Abgabestand `ende4` entstand aus `ende3` ueber Anker im Raum (`--anker3d`, Vorlauf nur Pose 2) und Feinlaeufe, Dossier `gerippe_gegenprobe.md`.\n- 32 Starts mit gestreuter Gier,")
ers("**[v]** neben den Bildern: Arm zu Bein 1,1 und Rumpf mindestens 1600, weil beide Bilder fast laengs blicken. Gier Bild 2 = 21 (frontal an dieser Stelle, Nutzer: von vorn). Kopfmitte Bild 2 (325,230). Knoechel Bild 2 bei (247,352) und (352,330): untere linke Spitze und Verdeckung V3.",
    "**[v]** neben den Bildern: Arm zu Bein 1,1 (schwach) und Rumpf mindestens 1600, weil beide Bilder fast laengs blicken. Kopfmitte Bild 2 (325,245), Kopfrand auf Zug Q, Gesicht hoechstens 15 Grad neben der Kamera (erreicht 20). Oberarm mindestens 0,72 Unterarm. Ellbogen rechts Bild 2 (312,322), Handgelenk (257,398), Fingerspitzen (232,468), (190,462), (165,425): Achse des Streifens und Finger unter dem Logo. Hand rechts am Boden. Knie rechts (254,313), Zehen rechts (249,368), Knoechel links (352,330): untere Spitze und Verdeckung V3. Fingerspitzen links (460,392), (468,417), (437,418) sind die Spitzen des Nutzer-Umrisses, also gelesen, nicht gesetzt.")
ers("Ganzzahl-Probe: Pose ueber `form_emd.pose_aus_bild` nachgerechnet, 13 Einheiten, 0,7 px.", "Ganzzahl-Probe: Pose ueber `form_emd.pose_aus_bild` nachgerechnet, 13 Einheiten, 1,0 px.\n\nUnabhaengig nachgerechnet (`gegenprobe/render.py`, echte Kegelstuempfe als Vierecke, nur `gerippe.json`): Schwanz 0,87, Bein links 0,75, Bein rechts 0,81, Arm rechts 0,79, Koerperstueck 0,71, Oberkoerper 0,76, Arm links 0,71, Arm rechts vorn 0,55.")

ers("Profile (`lauf/q_*.pt`): eine Groesse fest, alles andere 500 Schritte neu gerechnet, Ausgang `m4`. Zahl = Bildanteil der Kosten, kleiner ist besser. Die Laeufe am Ausgangswert liegen bei 0,74-0,75. Unterschiede unter rund 0,05 werte ich nicht.",
    "Profile (`lauf/t_*.pt`): eine Groesse fest, alles andere 500 Schritte neu gerechnet, Ausgang `ende4`, Anatomie-Kosten an. Zahl = Bildanteil der Kosten, kleiner ist besser. Der Bildanteil liegt hoeher als in der ersten Fassung, weil die Oberarme nicht mehr zaehlen. " + prof['kopf'])
block("| Groesse | Urteil | Spanne | Kennung |", "Gegenprobe: Die alte Spanne der Gier Bild 1", prof['tabelle'] + "\n\n")
ers("Gegenprobe: Die alte Spanne der Gier Bild 1 (5,7-17 Grad aus K2-K4) ueberlappt -20 bis +10 nur am Rand. Sie hing an Sohlen, die dort als offen standen und an der Anatomie mit vier Beinen.",
    "Die alte Spanne der Gier Bild 1 (5,7-17 Grad aus K2-K4) trifft den Abgabewert 7,3, obwohl sie an Sohlen hing, die dort als offen standen. Beleg ist das nicht.")
ers("Dunkelzone am Boden in Bild 2 (300-400,335-378): Das Rotrestbild zeigt dort Dunkelheit bis y 378. Das linke Bein steht im Abgabestand in dieser Zone [v]. Ob Bein oder Schatten, sagt das Bild nicht. V3 spricht fuer Figur (wahrscheinlich).",
    "Dunkelzone am Boden in Bild 2 (300-400,335-378): Das Rotrestbild zeigt dort Dunkelheit bis y 378. Der linke Fuss steht im Abgabestand in dieser Zone [v]. Ob Bein oder Schatten, sagt das Bild nicht. V3 spricht fuer Figur (wahrscheinlich).")

ers("Pose 1, Bild 1: Lage (-19108, 0, -18539), Gier 4080 (358,6 Grad), Wurzel Hoehe 1854.", "Pose 1, Bild 1: Lage {{LAGE1}}, Gier {{GIER1}}, Wurzel Hoehe {{WURZEL1}}.")
ers("Hand rechts endet bei (403,351), Fuss rechts Ferse gehoben, Spitze bei (318,421). Schwanzende (238,488), knapp unter dem Bild, Hoehe 230.",
    "Beide Ellbogen fast gestreckt, Arm rund 35 Grad nach hinten geneigt, Schultern stehen rund 1100 vor den Handgelenken. Das folgt aus dem Unterarm: Bild 1 zeigt 934 senkrecht, Bild 2 verlangt 1100 Laenge. Fuss rechts Ferse gehoben, Spitze bei (317,406). Schwanzende (237,489) unter dem Bild, Hoehe 449, liegt also nicht auf [f]. Schultern Tiefe 7705 und 7973, Rumpf nicht mehr verdreht.")
ers("Pose 2, Bild 2: Lage (937, 0, -23690), Gier 239 (21 Grad), Wurzel Hoehe 1422.", "Pose 2, Bild 2: Lage {{LAGE2}}, Gier {{GIER2}}, Wurzel Hoehe {{WURZEL2}}.")
ers("Rechte Schulter liegt rund 960 naeher an der Kamera als die linke, der Rumpf ist gegen den Kopf gedreht. Haende rund 2960 auseinander. Huefte rund 1650 von der Frau (Spawn), rechte Hand rund 3030.",
    "Rechte Schulter liegt rund 1040 naeher an der Kamera als die linke. Beide Handgelenke stehen hinter ihren Schultern, Ellbogen links 37 Grad gebeugt, rechts 62 Grad, Knie 74 Grad. Haende rund 2550 auseinander. Huefte rund 1600 von der Frau (Spawn), rechte Hand rund 2450. Rechter Oberarm ist um 98 Grad nach aussen gedreht, das ist das Ende des Bereichs.")

block("Aus dem Tiefenbild des Abgabestands, Hoehe auf der Modelloberflaeche.", "Haken H: Von 696 px", """Aus dem Tiefenbild des Abgabestands, Hoehe auf der Modelloberflaeche. Die Spalte Hoehe haengt an der Tiefe des Modells, in Bild 1 auf rund 100 genau. Zuordnung neu gerechnet, gegenueber `ende3` geaendert: G liegt jetzt auf dem Unterarm, Zug Q ist Kopfrand, P liegt in einer Luecke.

| Merkmal | Teil, Lage | Hoehe | Seite | Sicherheit |
|---|---|---|---|---|
| Z1 rosa Stelle | Koerperstueck, Hinterseite der Huefte, oben | 2070-2430 | rechts der Mitte 380-460 | wahrscheinlich |
| Z2 | Koerperstueck, Hinterseite der Huefte unter Z1 | 1880 | rechts 445 | wahrscheinlich |
| Z3, Z4 | oberes Ende rechter Oberschenkel, Rueckseite | 1560, 1470 | rechts 700 | wahrscheinlich |
| A05, A06 | Hinterseite Huefte, innen neben dem Schwanz | 1680, 1400 | rechts 230, Mitte | offen |
| Z5 Dreieck | rechter Arm, Ellbogenkappe (Modell (398,294)), Rueckseite | 1300-1580 | rechts 1000 | sicher als Stelle, Ellbogen wahrscheinlich |
| Z6 | A14 am unteren Ende rechter Oberschenkel, die zwei unteren auf der Wade | 1050, 940, 810 | rechts 1000 | A14 wahrscheinlich, sonst offen |
| Z7 | Bogen A20 am linken Knie, Rueckseite. Knickband (248,372), (249,381) liegt am Spalt Bein/Schwanz, (242,383) linke Wade | 1000, 890-970, 610 | links 100 | Bein oder Schwanz offen |
| Z8 | (241,289) linker Oberschenkel, (253,285), (272,284) quer auf dem Schwanz, Abschnitt 4 | 1580, 1720 | Mitte | Lage wahrscheinlich, Bedeutung offen |
| Z9 | ausserhalb aller Nutzer-Teile, 7-10 px neben dem Arm | | | offen, kann Hintergrund sein |
| A10 | Oberseite Schwanz, Abschnitt 2 | 2580 | rechts 280 | offen |
| X | Schwanzende, rosa | 720 und tiefer | Mitte | Nutzer |
| Zug Q | Kopfrand [v]: Abstand zum Modellrand Median 1 px. Die Punkte liegen genau auf der Kante, darum nennt die Tabelle teils Kopf, teils Brust | 1900-2850 | | wahrscheinlich |
| Q4 Randband | linke Schulter, hinter dem Kopf | 2240 | links | wahrscheinlich |
| P Spitze | im Modell Luecke zwischen Kiefer, Arm und Rumpf, 7 px neben der Figur. Heller Fleck kann Hintergrund sein | | | Stelle wahrscheinlich, Art offen |
| G | rechter Unterarm, obere Kante (312,339), (285,358). (338,320) liegt auf dem Kiefer | 880-1060 | | sicher als Stelle |
| B10, B11 | (280,275) Kopf, (258,263) Mittelrumpf | 1600-2230 | | offen |
| B12 | (217,274) am Modell Schwanzende, 42 px sichtbar, sonst Huefte | | | offen |
| Haken H | Huefte und Mittelrumpf, im Umriss | 1550-2060 | rechts 700 | offen, siehe unten |

""")
ers("Haken H: Von 696 px liegen 393 im Nutzer-Umriss (untere Haelfte), der Balken oben ausserhalb. Das Modell deckt 467 px mit der Brust. Das ist keine Zuordnung. Der Haken kann Zeichnung auf dem Rumpf sein oder ein eigenes Stueck, die Bilder sagen es nicht.",
    "Haken H: Von 696 px liegen 393 im Nutzer-Umriss (untere Haelfte), der Balken oben ausserhalb. Im Abgabestand liegen Huefte und Schwanzwurzel dort, 75 px Schwanz sichtbar. Das ist keine Zuordnung. Der Haken kann Zeichnung auf dem Rumpf sein oder ein eigenes Stueck, die Bilder sagen es nicht.")
block("Der Bogen liegt 24 px innerhalb des Kopfumrisses im Abgabestand", "Gesicht, frei: Augen", "Lesung ab der Gegenprobe: Der Bogen ist ein Kreis um (325,245), Radius 62 px, und wird als Kopfrand gefuehrt [v]. Links faellt er mit der Silhouette zusammen, oben und rechts liegt die Silhouette rund 30 px weiter aussen: Dort stehen Brust und linke Schulter hinter dem Kopf. Damit fuellt der Kopf die Kuppel nicht mehr allein. Ob der Bogen Stirnrand, Kopfrand oder Band ist, zeigen die Bilder weiter nicht.\n\n")
ers("Gesicht, frei: Augen, Nase, Maul, Zaehne, Kiefer, Hoerner. Die Punkte B10-B12 liegen im Abgabestand nicht auf dem Kopf. Liegt der Kopf links unten, treffen sie ihn, aber das kostet 0,93 gegen 0,74.",
    "Gesicht, frei: Augen, Nase, Maul, Zaehne, Kiefer, Hoerner. Zweite Lesung bleibt offen: Kopf links unten bei B10-B12. Sie widerspricht der Rumpfkante nicht, wurde aber nicht neu gerechnet.")

block("Bild 1:\n- Schwanz unten:", "Beide: Keine Zeichnung,", """Bild 1:
- Schwanz unten: Ende ist ein runder Stab, der Keil unter dem Logo wird gedeckt, aber nicht in seiner Form.
- Hand rechts: Sohle y 377 gegen 374, rechte Spitze (427,362) ist jetzt gedeckt.
- Lappen und Kerbe der Beine fehlen, Kegel sind glatt. Die Masse (218-250,395-440) zwischen Bein und Schwanz ist nur zur Haelfte gedeckt.
- Luecken zwischen den Beinen zu klein: L1 231 von 1586 px, L2 292 von 1064 px belegt (vorher 178 und 223). Darin sichtbar: linker Arm und Hand 235 px, Kopf und Kiefer 215 px, duerfen eigentlich nicht zu sehen sein.
- Koerperstueck: Kante 13 px im schlechtesten Zehntel.

Bild 2:
- Hand rechts deckt den Nutzer-Umriss zu 0,67. Es fehlt der Sporn nach links oben bis (147,350), die Flaeche links unter dem Logo und die Spitze rechts (279,420). Drei Finger, der Umriss zeigt mindestens zwei Lappen und einen Rest.
- Fingerluecke (205-212,425-455) liegt im Modell bei rund x 212, Finger enden bei y 462-468, Umriss bei 476.
- Arm links: Hand 11 px Ueberstand am Keil, weil beide Haende gleich gross sind und der rechte Umriss die Groesse vorgibt.
- Kleines Loch im Rumpf bei (258-275,297-312), V2 zu 0,95 gedeckt.
- Oberkante links liegt am Nutzer-Umriss, 16-28 px ausserhalb der gelesenen Kante. Dunkel auf dunkel, FORMBLATT laesst das zu.
- Linker Fuss sichtbar zwischen den Armen, rechter Fuss in der unteren Spitze, rechtes Bein in der linken Bucht. Das ist Vorgabe [v], kein Nutzer-Teil.
- Schwanzwurzel 75 px oben links, Schwanzende 42 px bei B12, rechter Oberarm 114 px. Sollten verdeckt sein.

""")

block("- Gier und Kopf Bild 2 sind gesetzt", "- Der Abgabestand ist eine Loesung", "- Kopf Bild 2 ist gesetzt, nicht gemessen. Die Gier ist frei gerechnet, aber die Bilder erlauben sie in der Spanne aus Abschnitt 4.\n- Rechter Oberarm und Hals stehen in Pose 2 am Ende ihres Bereichs (98 und 60 Grad), Unterarm links und Unterschenkel am Drehlimit 12 Grad. Moeglich, aber kein Spielraum fuer Animation aus dieser Pose.\n- Kopf, Fuss und Hals laufen in ihre Laengengrenzen (1300, 900). Gemessen ist das nicht.\n")
ers("| `gerippe_rundum.png` | Seite, vorn, oben fuer Pose 1, Pose 2, Rast |", "| `gerippe_rundum.png` | Seite, vorn, oben fuer Pose 1, Pose 2, Rast |\n| `gerippe_rundblick.png` | Echte Kegelstuempfe, sechs Seiten je Pose (`gegenprobe/render.py`) |\n| `gerippe_gegenprobe.md` | Dossier der Gegenprobe, Belege zu Abschnitt 10 |\n| `gegenprobe/` | Eigene Nachrechnung, Laufskripte, Originale in `vorher/` |")
ers("| `gerippe/lauf/ende3.pt` | Abgabestand |", "| `gerippe/lauf/ende4.pt` | Abgabestand |\n| `gerippe/lauf/ende3.pt` | Stand vor der Gegenprobe |")
ers("| `gerippe/lauf/q_*.pt`, `profile_q.json`, `vergleich_k.txt` | Profile, Streuung der Starts |", "| `gerippe/lauf/t_*.pt`, `profile_q.json` | Profile vom Abgabestand |\n| `gerippe/lauf/q_*.pt`, `profile_q_alt.json`, `vergleich_k.txt` | Profile und Streuung der ersten Fassung |")

t = t.rstrip() + "\n\n" + open(os.path.join(H, 'aenderungsliste.md'), encoding='utf-8').read().rstrip() + "\n"
open(os.path.join(G, 'formblatt2_vorlage.md'), 'w', encoding='utf-8', newline='\n').write(t)
print('Vorlage', len(t))
