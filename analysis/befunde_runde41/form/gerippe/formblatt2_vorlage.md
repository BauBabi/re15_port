# FORMBLATT 2 Bahnsteig-Form

Stand 2026-11-01, nach der Gegenprobe (`gerippe_gegenprobe.md`, Aenderungsliste Abschnitt 10). Bindend ist die Lesung des Nutzers: acht freigestellte Teile, Bild 1 von hinten, Bild 2 von vorn, Gesicht, zwei Arme mit Klauen, zwei Beine, langer dicker Schwanz.
Dieses Blatt ersetzt die Anatomie aus `FORMBLATT.md` (Vierbeiner mit Kuppel, Beine NL/NR/FR, Haken, Gegenstand X). Gueltig bleiben dort Massstaebe, Hoehen, Kanten, Verdeckungen, Materialien und die Bildlage der Zeichnung.
Pixel: Originalbild 640x480. Einheiten: Spiel, Y nach unten, Boden y=0, +X vorn, +Z links der Figur. Maschinenlesbar: `gerippe.json`.

Kennung: **[g]** aus den Bildern gerechnet, **[v]** Vorgabe, keine Messung, **[f]** frei.

## 0. Kurzfassung

- Das Gerippe hat 30 Knochen und erklaert beide Bilder mit derselben Figur: gleiche Laengen, gleiche Dicken, nur Winkel, Standort und Gier verschieden. Ganzzahl-Probe ueber `form_emd.pose_aus_bild`: hoechstens 13 Einheiten, 1,0 Pixel.
- Restfehler (IoU sichtbares Modellteil gegen Nutzer-Teil): Schwanz 0,86, Bein links 0,78, Bein rechts 0,81, Arm rechts 0,79, Koerperstueck 0,69, Oberkoerper 0,77, Arm links 0,70, Arm rechts vorn 0,57. Kantenabstand im Median 0-3,7 Pixel. Die Zahlen von Bild 2 sind nicht mit der ersten Fassung vergleichbar: Die Oberarme zaehlen nicht mehr als Rumpf (Abschnitt 10, Punkt 1). Oberkoerper-Umriss wird zu 0,93 gedeckt, weil das rechte Bein die untere Spitze fuellt.
- Masse: Rumpf rund 2240, Arm 1910 (Oberarm 800, Unterarm 1110), Bein 1780, Schwanz 3820, Kopf 1300, Hals 500, Hand 690, Klaue 610, Fuss 900. Schulterabstand 1800, Hueftgelenke 1120.
- Pose 1 (Bild 1): auf allen vieren, Huefte Hoehe 1880, Ruecken waagerecht, Brust steigt 25 Grad, Kopf haengt 33 Grad vorn herab (verdeckt). Schwanz steigt auf 2940 und faellt zur Kamera. Blick fast laengs.
- Pose 2 (Bild 2): Rumpf gerade und 28 Grad aufgerichtet, Gier -16, also 36 Grad gegen die Bildachse gedreht. Huefte links hinten im Bild, Brust rechts, Scheitel 3140 auf der Brust. Kopf auf 56 Grad gegen die Brust zur Kamera gedreht, aufrecht, 20 Grad neben der Kamera, Rand auf Zug Q. Rechter Arm: Ellbogen unter der Rumpfkante, Unterarm auf dem gerippten Streifen, Hand am Boden, Finger zum Logo. Linker Arm von der Schulter hinter dem Kopf herab. Beine in der Hocke unter dem Bauch.
- Fest belegt: Lage und Richtung aller sichtbaren Glieder, Dicken, Kante R, Verdeckungen. Nicht belegt: Gesicht, Kopflaenge, Kieferform, Rumpflaenge, Oberarmlaenge, Fusslaenge, Lage des linken Arms in Bild 1, Lage der Beine und des Schwanzes in Bild 2, Fingerzahl.
- Gier Bild 2 ist nicht mehr gesetzt: Vier Starts von -5 bis -28 endeten bei -12 bis -15, die Linie der Handgelenke allein ergibt -14,4. Profil: -40 bis 0 gleich gut, 21 schlechter.

## 1. Die acht Teile

Gemessen an den Masken, geprueft an den Kanaelen (`gerippe/lauf/k_b1_unten.png`, `k_b1_oben.png`, `k_b1_arm.png`, `k_b2_linksarm.png`, `b2_hand_x5.png`, `b2_figurkanal_x3.png`).

{{TEILE}}

| Teil | Gelenke und Enden im Bild |
|---|---|
| tail | Oben runde Kuppe, Scheitel y 184-187. Breite 55-60 px bei y 234-264, nimmt bis y 399 auf 26 px ab. Unten rosa Ende (250-288,420-479), laeuft aus dem Bild. Nutzer-Umriss wird dort durch den Keil unter dem Logo breiter |
| left_leg | Oben schmaler Streifen x 233-247, der Rest liegt hinter dem Schwanz. Wulst mit hellem Rand y 340-362, das ist die Knierueckseite. Kerbe (206,374). Unten Lappen bis y 447 |
| right_leg | Oberschenkel 40-47 px bis y 334. Engstelle y 349-364, 31-32 px, dort Band A14: Knie. Darunter 35-45 px, Ende y 461 |
| right_arm | Schulter oben y 243. Kappe Z5 y 277-296 sitzt am Ellbogen. Darunter gerader Zylinder, 28-34 px im Umriss, 33-37 px laut FORMBLATT, bis y 355. Hand x 377-427, Sohle y 375 |
| part_of_Body | Oberkante (337,205) bis (380,240). Rechte Kante laeuft zum Arm, Unterkante y 317, ueberlappt 1316 px mit right_leg |
| face_and_upper_body | Nutzer-Oberkante y 142, belegte Kante y 156. Breit 224 px. Kerbe links y 205-240 um den Haken. Untere Spitze (236-259,372) |
| left_arm_front | Beginnt an der Rumpfkante (403,260)-(366,297), Breite 28-36 px, Engstelle 30 px bei y 342. Hand als flacher Keil nach rechts unten, Spitzen (437,418), (468,417), (460,392) |
| right_arm_front | Spitze (337,316) kommt unter der Rumpfkante hervor. Unterarm gerippt, bis 45 px breit. Handgelenk um (262,395). Hand dunkel (158-275,385-445), weiter unter dem Logo, Fingerluecken (205-212,425-455), (258-272,430-445), Finger nach links oben bis (147,350) |

Lesebefund Bild 2: Beide Arme kommen unter der unteren rechten Rumpfkante hervor. Der Rumpf liegt also vor den Armansaetzen.

Achse des rechten Unterarms: Die rechte Kante des Streifens ist gerade, (335,326) bis (274,408). Der obere Teil liegt unter dem Rumpf, also ist nur diese Kante echt. Achse = Kante, 20 px nach links oben: (319,314) bis (258,396). Die Spitze (337,316) ist die Ellbogenkappe.

## 2. Gerippe

Rastpose: alle Winkel 0, Rumpf entlang +X, Glieder entlang +Y, Schwanz entlang -X. Mesh i haengt an Knochen i, Eltern vor Kindern. Knochen 10 = Brust, 13 und 18 = Haende, damit der Tick (Teile 10, 13, 18) ohne Aenderung trifft. Links gleich rechts. a = Halbachse Hoehe (X-Knochen) oder Tiefe (Y-Knochen), b = Halbachse Breite.

{{KNOCHEN}}

Winkelgrenzen stehen in `gerippe.json` (`winkelgrenzen_grad`): Knie -2 bis 165, Ellbogen -2 bis 155 (in Beugerichtung gezaehlt), Kiefer 0 bis 50, Schwanz je Abschnitt +-50.

Hinweis zum Kopf: Der Querschnitt (Halbachsen rund 530 hoch, 560 breit) kommt aus Zug Q in Bild 2. Laenge 1300 liegt an der Grenze des Knochens und ist [f]: Von der Seite ist der Kopf nirgends zu sehen. Das Ende darf nicht dicker sein als die Mitte. Der Kiefer haengt bei x 500 unter dem Kopf und reicht bis 92 % der Kopflaenge, Form [f].

## 3. Rechnung

Werkzeug `gerippe/`: `rig.py` Knochenbaum und Vorwaertsrechnung wie im Port, `fit.py` Optimierung, `ansicht.py`, `abgabe.py`, `merkmale.py`, `pruefpunkte.py`, `bogen.py`, `vergleich.py`, `profile.sh`, `profil_lesen.py`.

- Je Knochen eine Kette von Ellipsoiden (zusammen 124), die den Kegelstumpf mit Dickenprofil bilden, projiziert ueber die Matrix P aus `form_kamera.py`. Daraus Abstand je Pixel und weiche Tiefe.
- Gemeinsam fuer beide Bilder: Laengen, Versaetze, Dicken, links gleich rechts. Je Bild: Standort, Gier, alle Winkel.
- Ziel: Teil-Masken (je Abschnitt entlang der Mittellinie gleich gewichtet, damit duenne Stellen zaehlen), sichtbares Teil innerhalb der Maske, Leerflaechen aus `ziel_bild*.png`, K1 auf (398,374), Haende und Fuesse am Boden und flach, Winkelgrenzen, Stetigkeit der Dicken.
- Verdeckt bleiben muessen Kopf und linker Arm in Bild 1, der Schwanz und der rechte Oberarm in Bild 2. Der linke Oberarm zaehlt in Bild 2 zum Oberkoerper-Umriss: Er liegt rechts von Zug Q hinter dem Kopf. Bei doppelt beschrifteten Pixeln zaehlt das vordere Teil (Schwanz vor Beinen vor Koerper).
- Anatomie-Kosten seit der Gegenprobe (`fit.py --anatomie`): Kopf aufrecht und zur Kamera, Glieder ausserhalb des Rumpfes, Klauen am Boden, Finger und Hand mit Mindestdicke, Rumpf ohne Kragen, Wirbelsaeule glatt, Oberarm mindestens 0,72 Unterarm, Kiefer vorn.
- Gemessene Kante R und Verdeckungen V1-V5 aus FORMBLATT als Kosten (`--kante`): belegte Kante beidseitig, gelesene Kante nur als Mindestausdehnung.
- Der Abgabestand `ende4` entstand aus `ende3` ueber Anker im Raum (`--anker3d`, Vorlauf nur Pose 2) und Feinlaeufe, Dossier `gerippe_gegenprobe.md`.
- 32 Starts mit gestreuter Gier, Lage, Neigung, Gliedern und Massen (`lauf/k91-94`), davon zwei Reihen mit grosser Streuung. Beste Starts danach feiner gerechnet, Stride 2. Aeltere Laeufe (`lauf/a`, `b`, `c`, `f`, `g`, `pr_`) hatten einen Fehler in der weichen Tiefe und sind verworfen, Beschreibung im Dossier. `f5` dient nur noch als Probe fuer die Kopfgroesse.

**[v]** neben den Bildern: Arm zu Bein 1,1 (schwach) und Rumpf mindestens 1600, weil beide Bilder fast laengs blicken. Kopfmitte Bild 2 (325,245), Kopfrand auf Zug Q, Gesicht hoechstens 15 Grad neben der Kamera (erreicht 20). Oberarm mindestens 0,72 Unterarm. Ellbogen rechts Bild 2 (312,322), Handgelenk (257,398), Fingerspitzen (232,468), (190,462), (165,425): Achse des Streifens und Finger unter dem Logo. Hand rechts am Boden. Knie rechts (254,313), Zehen rechts (249,368), Knoechel links (352,330): untere Spitze und Verdeckung V3. Fingerspitzen links (460,392), (468,417), (437,418) sind die Spitzen des Nutzer-Umrisses, also gelesen, nicht gesetzt.

Restfehler je Teil:

{{REST}}

Ganzzahl-Probe: Pose ueber `form_emd.pose_aus_bild` nachgerechnet, 13 Einheiten, 1,0 px.

Unabhaengig nachgerechnet (`gegenprobe/render.py`, echte Kegelstuempfe als Vierecke, nur `gerippe.json`): Schwanz 0,87, Bein links 0,75, Bein rechts 0,81, Arm rechts 0,79, Koerperstueck 0,71, Oberkoerper 0,76, Arm links 0,71, Arm rechts vorn 0,55.

Pruefpunkte aus FORMBLATT Abschnitt 7:

{{PRUEF}}

## 4. Was die Bilder festlegen

Profile (`lauf/t_*.pt`): eine Groesse fest, alles andere 500 Schritte neu gerechnet, Ausgang `ende4`, Anatomie-Kosten an. Zahl = Bildanteil der Kosten, kleiner ist besser. Der Bildanteil liegt hoeher als in der ersten Fassung, weil die Oberarme nicht mehr zaehlen. Unterschiede unter rund 0,1 werte ich nicht. Einzelne Laeufe bleiben in Nebenminima haengen (Tiefe Bild 2 = 7000), die Zahl ist dort Obergrenze.

{{PROFILE}}

| Groesse | Urteil | Spanne | Kennung |
|---|---|---|---|
| Arm (Ober- + Unterarm) | nach unten festgelegt | ab 1900, 1700 deutlich schlechter, oben bis 2300 flach. Abgabe 1910 | [g] |
| Bein | nach unten festgelegt | ab 1600, 1400 deutlich schlechter. Abgabe 1780 | [g] |
| Schwanzlaenge | nach unten festgelegt | ab 3400, 3000 etwas schlechter, bis 4200 flach. Abgabe 3820 | [g] |
| Rumpflaenge | nach unten festgelegt | ab 1800, oben bis 2800 flach. Abgabe 2240 | [g] unten, sonst frei |
| Huefte Tiefe Bild 1 | nach unten festgelegt | ab 5600, 5200 schlechter. Abgabe 5840 | [g] |
| Gier Bild 1 | festgelegt | -5 bis +10 Grad, Abgabe 7 | [g] |
| Gier Bild 2 | nach oben festgelegt | -40 bis 0 gleich gut, 21: +0,7, 45: +1,1. Die alte Vorgabe 21 faellt damit | [g] oben, Abgabe -16 aus Handlinie |
| Huefte Tiefe Bild 2 | eng, unruhig | 6500-8000, Abgabe 7200 | [g] |
| Kopfmitte Bild 2 | festgelegt, solange der Kopf in Zug Q steht | x 290-325, y 230-245 | [v] Zug Q |
| Schulter-Halbabstand | eng | 810-1020 aus der ersten Fassung, nicht neu profiliert, Abgabe 898 | [g] |
| Dicken Arm, Bein, Schwanz | festgelegt, wo sichtbar | Schwanz 310-340 Wurzel, 170-220 Mitte, bis 230 Ende, Unterarm 260-305, Bein 140-245 | [g] |
| Rechte Hand Bild 2 | gestuetzt | Hand am Boden passt zum Unterarm ohne Laengenaenderung | [v] Boden |
| Oberarm | nicht sichtbar | Summe Arm belegt, Teilung nicht | [v] mindestens 0,72 Unterarm |
| Beine, Schwanz Bild 2, linker Arm Bild 1 | nicht sichtbar | | [f] |
| Hals, Kiefer, Kopfform seitlich, Fusslaenge, Fingerzahl | nicht sichtbar | | [f] |

Die alte Spanne der Gier Bild 1 (5,7-17 Grad aus K2-K4) trifft den Abgabewert 7,3, obwohl sie an Sohlen hing, die dort als offen standen. Beleg ist das nicht.

Dunkelzone am Boden in Bild 2 (300-400,335-378): Das Rotrestbild zeigt dort Dunkelheit bis y 378. Der linke Fuss steht im Abgabestand in dieser Zone [v]. Ob Bein oder Schatten, sagt das Bild nicht. V3 spricht fuer Figur (wahrscheinlich).

## 5. Posen

Beide in `gerippe.json` als Lage, Gier, Wurzel und Winkel je Knochen.

Pose 1, Bild 1: Lage {{LAGE1}}, Gier {{GIER1}}, Wurzel Hoehe {{WURZEL1}}.

{{POSE1}}

Beide Ellbogen fast gestreckt, Arm rund 35 Grad nach hinten geneigt, Schultern stehen rund 1100 vor den Handgelenken. Das folgt aus dem Unterarm: Bild 1 zeigt 934 senkrecht, Bild 2 verlangt 1100 Laenge. Fuss rechts Ferse gehoben, Spitze bei (317,406). Schwanzende (237,489) unter dem Bild, Hoehe 449, liegt also nicht auf [f]. Schultern Tiefe 7705 und 7973, Rumpf nicht mehr verdreht.

Pose 2, Bild 2: Lage {{LAGE2}}, Gier {{GIER2}}, Wurzel Hoehe {{WURZEL2}}.

{{POSE2}}

Rechte Schulter liegt rund 1040 naeher an der Kamera als die linke. Beide Handgelenke stehen hinter ihren Schultern, Ellbogen links 37 Grad gebeugt, rechts 62 Grad, Knie 74 Grad. Haende rund 2550 auseinander. Huefte rund 1600 von der Frau (Spawn), rechte Hand rund 2450. Rechter Oberarm ist um 98 Grad nach aussen gedreht, das ist das Ende des Bereichs.

## 6. Zeichnung auf dem Koerper

Aus dem Tiefenbild des Abgabestands, Hoehe auf der Modelloberflaeche. Die Spalte Hoehe haengt an der Tiefe des Modells, in Bild 1 auf rund 100 genau. Zuordnung neu gerechnet, gegenueber `ende3` geaendert: G liegt jetzt auf dem Unterarm, Zug Q ist Kopfrand, P liegt in einer Luecke.

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

Haken H: Von 696 px liegen 393 im Nutzer-Umriss (untere Haelfte), der Balken oben ausserhalb. Im Abgabestand liegen Huefte und Schwanzwurzel dort, 75 px Schwanz sichtbar. Das ist keine Zuordnung. Der Haken kann Zeichnung auf dem Rumpf sein oder ein eigenes Stueck, die Bilder sagen es nicht.

Gesicht, erkennbar:
- Zug Q als ein Bogen: (262,225), (290,172), Sichel bis (345,189), (372,201), (388,236), dann nach unten bis P (366,304). Rund 125 px breit, 110 px hoch.
- Blaufleck B03 (327,168) darueber.
- Innenflaeche (290-340,225-280) ohne Zeichnung.
- Schwacher Fleck (305-335,265-290) im Blaukanal.

Lesung ab der Gegenprobe: Der Bogen ist ein Kreis um (325,245), Radius 62 px, und wird als Kopfrand gefuehrt [v]. Links faellt er mit der Silhouette zusammen, oben und rechts liegt die Silhouette rund 30 px weiter aussen: Dort stehen Brust und linke Schulter hinter dem Kopf. Damit fuellt der Kopf die Kuppel nicht mehr allein. Ob der Bogen Stirnrand, Kopfrand oder Band ist, zeigen die Bilder weiter nicht.

Gesicht, frei: Augen, Nase, Maul, Zaehne, Kiefer, Hoerner. Zweite Lesung bleibt offen: Kopf links unten bei B10-B12. Sie widerspricht der Rumpfkante nicht, wurde aber nicht neu gerechnet.

Einzeltabelle aller Punkte:

{{MERKMALE}}

## 7. Was im Bild noch abweicht

Bild 1:
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

Beide: Keine Zeichnung, kein Schatten, Glieder als Kegel. Das Gerippe zeigt Masse und Lage, nicht die Haut.

## 8. Grenzen

- Kamera Bild 1 ist geschaetzt (`kamera.md`), Massstab auf rund 4 %.
- Die Nutzer-Umrisse sind Handschnitte. IoU unter 1 ist dort kein Fehler. Oberkante Bild 2 ist grosszuegiger als die Messkante.
- Kopf Bild 2 ist gesetzt, nicht gemessen. Die Gier ist frei gerechnet, aber die Bilder erlauben sie in der Spanne aus Abschnitt 4.
- Rechter Oberarm und Hals stehen in Pose 2 am Ende ihres Bereichs (98 und 60 Grad), Unterarm links und Unterschenkel am Drehlimit 12 Grad. Moeglich, aber kein Spielraum fuer Animation aus dieser Pose.
- Kopf, Fuss und Hals laufen in ihre Laengengrenzen (1300, 900). Gemessen ist das nicht.
- Der Abgabestand ist eine Loesung von mehreren guten. Rumpf und Kopf gehoeren dem Modellbau.
- Port-Festwerte aus `werkzeug.md` (Clips, Kasten, Schatten) sind nicht geprueft.

## 9. Dateien

| Datei | Inhalt |
|---|---|
| `gerippe.json` | Knochen, Eltern, Versaetze, Dicken, Pose 1 und 2, Zuordnung, Restfehler, Merkmale |
| `gerippe_bild1.png`, `gerippe_bild2.png` | Kegelstuempfe ueber den Bildern, Teile farbig |
| `gerippe_rundum.png` | Seite, vorn, oben fuer Pose 1, Pose 2, Rast |
| `gerippe_rundblick.png` | Echte Kegelstuempfe, sechs Seiten je Pose (`gegenprobe/render.py`) |
| `gerippe_gegenprobe.md` | Dossier der Gegenprobe, Belege zu Abschnitt 10 |
| `gegenprobe/` | Eigene Nachrechnung, Laufskripte, Originale in `vorher/` |
| `gerippe/` | Werkzeug und `DOSSIER.md` |
| `gerippe/lauf/ende4.pt` | Abgabestand |
| `gerippe/lauf/ende3.pt` | Stand vor der Gegenprobe |
| `gerippe/lauf/t_*.pt`, `profile_q.json` | Profile vom Abgabestand |
| `gerippe/lauf/q_*.pt`, `profile_q_alt.json`, `vergleich_k.txt` | Profile und Streuung der ersten Fassung |

## 10. Aenderungsliste der Gegenprobe

Belege und Zahlen im Dossier `gerippe_gegenprobe.md`. Vorher-Stand in `gegenprobe/vorher/`.

| Nr | Fund | Beleg | Aenderung |
|---|---|---|---|
| 1 | Rechter Arm Bild 2 lief nicht auf dem Nutzer-Teil: Oberarm lag vor der Brust, Handgelenk 57 px neben (262,395), Hand zeigte von der Kamera weg. Metrik verdeckte das: Oberarm-Pixel im Oberkoerper-Umriss wurden dem Rumpf gutgeschrieben. IoU wirklich 0,51, nicht 0,71 | `gegenprobe/render.py`, `richtung.py`, Abschnitt 1 dieses Blatts | Oberarm zaehlt in Bild 2 zu keinem Teil, bleibt verdeckt (114 px statt 6211). Ellbogen an die Spitze, Unterarm auf die Achse des Streifens, Hand am Boden, Finger zum Logo. `rig.py`, `fit.py`, `ansicht.py` |
| 2 | Kopf Bild 2 lag 59 Grad auf der Seite, a 750 an der Klassengrenze, Ende dicker als Mitte (Trichter). Nur so fuellte er die Kuppel | `gegenprobe/kopfachsen.py`, Seitenblick `gerippe_rundblick.png` | Kopf aufrecht (Roll 3 Grad), Rand auf Zug Q [v], Kuppel kommt von Brust und linker Schulter. Grenze a 680, Ende nicht dicker als Mitte |
| 3 | Gier Bild 2 = 21 war Vorgabe und erzwang krummen Rumpf (Huefte +21, Brust -17). Handlinie ergibt -14,4 | Rechnung Dossier, Profil Abschnitt 4 | Gier frei, Ergebnis -15,7. Rumpf gerade. Kopf dreht zur Kamera |
| 4 | Bein im Rumpf Pose 2: rechter Oberschenkel 45 %, Unterschenkel 24 %, linker Oberschenkel 17 % | `gegenprobe/ae1.txt` | Kosten Glieder ausserhalb des Rumpfes. Jetzt 5 %, 0 %, 3,5 %. Hocke, Knie seitlich |
| 5 | Ellbogen Pose 1 auf +12 Grad Ueberstreckung, beide | `gegenprobe/ae1.txt` | Grenze Ellbogen und Knie auf hoechstens 2 Grad. Oberarm 601 auf 800 (Vorgabe mindestens 0,72 Unterarm), Rumpf Pose 1 nicht mehr 24 Grad verdreht |
| 6 | Finger schwebten: linke Klauen Pose 2 bis 350 ueber dem Boden, Spitzen 20 px neben dem Umriss. Finger Pose 1 standen bis Hoehe 760 | `gegenprobe/ae1.txt`, Maske | Linke Spitzen im Bild auf (461,394), (465,413), (438,415), zwei am Boden (Hoehe 76, 102), die aeussere bei 348. Rechte Hand: Handballen am Boden, Knoechel hoch, Spitzen Hoehe 173-184. Pose 1 Spitzen 15-55 am Boden |
| 7 | Kiefer steckte im Hals (52-60 % im Rumpf), Spitze 670 hinter der Schnauze | `gegenprobe/ae1.txt` | Kiefer bei x 500 unter dem Kopf, reicht bis 92 % Kopflaenge. [f] |
| 8 | Oberarm wurde zur Schulter duenner (274 auf 416): Rest der vom Rumpf verdeckten Maske | Abschnitt 2 alte Fassung | Oberarm 331/300/280, Unterarm 305/265/260. Bild 1 deckt der Rumpf die Innenseite |
| 9 | Kante R und Verdeckungen waren nur Pruefpunkte: belegte Kante 0-5 px, gelesene 4-26 px, V2 0,88 | `pruefpunkte.py` | Als Kosten: belegte Kante 0-1,4 px. Gelesene Kante nur Mindestausdehnung, siehe Abschnitt 7. V2 0,95 |
| 10 | Hals, Fusslaenge: laufen wie vorher an Grenzen | Abschnitt 8 | Nicht geaendert, vermerkt |

Nicht geaendert: Links und rechts sind nicht vertauscht. Beide Posen sind dieselbe Figur. Lage von Schwanz, beiden Beinen und rechtem Arm in Bild 1, Teile-Lesung, Kameras, Knochenbaum, Knochennummern.
