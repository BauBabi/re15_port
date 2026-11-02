# FORMBLATT 2 Bahnsteig-Form

Stand 2026-11-01. Bindend ist die Lesung des Nutzers: acht freigestellte Teile, Bild 1 von hinten, Bild 2 von vorn, Gesicht, zwei Arme mit Klauen, zwei Beine, langer dicker Schwanz.
Dieses Blatt ersetzt die Anatomie aus `FORMBLATT.md` (Vierbeiner mit Kuppel, Beine NL/NR/FR, Haken, Gegenstand X). Gueltig bleiben dort Massstaebe, Hoehen, Kanten, Verdeckungen, Materialien und die Bildlage der Zeichnung.
Pixel: Originalbild 640x480. Einheiten: Spiel, Y nach unten, Boden y=0, +X vorn, +Z links der Figur. Maschinenlesbar: `gerippe.json`.

Kennung: **[g]** aus den Bildern gerechnet, **[v]** Vorgabe, keine Messung, **[f]** frei.

## 0. Kurzfassung

- Das Gerippe hat 30 Knochen und erklaert beide Bilder mit derselben Figur: gleiche Laengen, gleiche Dicken, nur Winkel, Standort und Gier verschieden. Ganzzahl-Probe ueber `form_emd.pose_aus_bild`: hoechstens 13 Einheiten, 0,7 Pixel.
- Restfehler (IoU sichtbares Modellteil gegen Nutzer-Teil): Schwanz 0,85, Bein links 0,76, Bein rechts 0,82, Arm rechts 0,78, Koerperstueck 0,70, Oberkoerper 0,89, Arm links 0,83, Arm rechts vorn 0,71. Kantenabstand im Median 0-2,3 Pixel.
- Masse: Rumpf rund 2040, Arm 1700, Bein 1550, Schwanz 3340, Kopf 1220, Hals 390, Hand 450, Klaue 675, Fuss 890. Schulterabstand 1630, Hueftgelenke 860.
- Pose 1 (Bild 1): auf allen vieren, Huefte Hoehe 1850, Ruecken waagerecht, Schwanz steigt auf 2890 und faellt zur Kamera bis auf den Boden. Blick fast laengs.
- Pose 2 (Bild 2): Hinterteil tiefer (Huefte 1420), Rumpf gedreht, Kopf hoch und zur Kamera (Scheitel 3160), beide Haende vorn am Boden, Schwanz liegt hinter der Figur.
- Fest belegt: Masse und Lage der sichtbaren Glieder, Schwanzlaenge, Arm- und Beinlaenge auf rund 20 %, Bild 1 Standort. Nicht belegt: Rumpflaenge, Gesicht, Kieferform, Kopflaenge, Lage des linken Arms in Bild 1, Lage der Beine und des Schwanzes in Bild 2, Fingerzahl.
- Der beste freie Start (ohne Vorgabe) fiel von selbst auf Gier 15 und Kopfmitte (330,243). Gesetzt ist nur frontal 21 und (325,230).

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

## 2. Gerippe

Rastpose: alle Winkel 0, Rumpf entlang +X, Glieder entlang +Y, Schwanz entlang -X. Mesh i haengt an Knochen i, Eltern vor Kindern. Knochen 10 = Brust, 13 und 18 = Haende, damit der Tick (Teile 10, 13, 18) ohne Aenderung trifft. Links gleich rechts. a = Halbachse Hoehe (X-Knochen) oder Tiefe (Y-Knochen), b = Halbachse Breite.

{{KNOCHEN}}

Winkelgrenzen stehen in `gerippe.json` (`winkelgrenzen_grad`): Knie 0 bis 165, Ellbogen 0 bis 155, Kiefer 0 bis 50, Schwanz je Abschnitt +-50.

Hinweis zum Kopf: Dicke a Ende 750 und Laenge 1220 kommen nur aus der Vorderansicht in Bild 2. Von der Seite ist der Kopf nirgends zu sehen, die Form ist [f].

## 3. Rechnung

Werkzeug `gerippe/`: `rig.py` Knochenbaum und Vorwaertsrechnung wie im Port, `fit.py` Optimierung, `ansicht.py`, `abgabe.py`, `merkmale.py`, `pruefpunkte.py`, `bogen.py`, `vergleich.py`, `profile.sh`, `profil_lesen.py`.

- Je Knochen eine Kette von Ellipsoiden (zusammen 124), die den Kegelstumpf mit Dickenprofil bilden, projiziert ueber die Matrix P aus `form_kamera.py`. Daraus Abstand je Pixel und weiche Tiefe.
- Gemeinsam fuer beide Bilder: Laengen, Versaetze, Dicken, links gleich rechts. Je Bild: Standort, Gier, alle Winkel.
- Ziel: Teil-Masken (je Abschnitt entlang der Mittellinie gleich gewichtet, damit duenne Stellen zaehlen), sichtbares Teil innerhalb der Maske, Leerflaechen aus `ziel_bild*.png`, K1 auf (398,374), Haende und Fuesse am Boden und flach, Winkelgrenzen, Stetigkeit der Dicken.
- Verdeckt bleiben muessen Kopf und linker Arm in Bild 1, der Schwanz in Bild 2. Bei doppelt beschrifteten Pixeln zaehlt das vordere Teil (Schwanz vor Beinen vor Koerper).
- 32 Starts mit gestreuter Gier, Lage, Neigung, Gliedern und Massen (`lauf/k91-94`), davon zwei Reihen mit grosser Streuung. Beste Starts danach feiner gerechnet, Stride 2. Aeltere Laeufe (`lauf/a`, `b`, `c`, `f`, `g`, `pr_`) hatten einen Fehler in der weichen Tiefe und sind verworfen, Beschreibung im Dossier. `f5` dient nur noch als Probe fuer die Kopfgroesse.

**[v]** neben den Bildern: Arm zu Bein 1,1 und Rumpf mindestens 1600, weil beide Bilder fast laengs blicken. Gier Bild 2 = 21 (frontal an dieser Stelle, Nutzer: von vorn). Kopfmitte Bild 2 (325,230). Knoechel Bild 2 bei (247,352) und (352,330): untere linke Spitze und Verdeckung V3.

Restfehler je Teil:

{{REST}}

Ganzzahl-Probe: Pose ueber `form_emd.pose_aus_bild` nachgerechnet, 13 Einheiten, 0,7 px.

Pruefpunkte aus FORMBLATT Abschnitt 7:

{{PRUEF}}

## 4. Was die Bilder festlegen

Profile (`lauf/q_*.pt`): eine Groesse fest, alles andere 500 Schritte neu gerechnet, Ausgang `m4`. Zahl = Bildanteil der Kosten, kleiner ist besser. Die Laeufe am Ausgangswert liegen bei 0,74-0,75. Unterschiede unter rund 0,05 werte ich nicht.

{{PROFILE}}

| Groesse | Urteil | Spanne | Kennung |
|---|---|---|---|
| Schwanzlaenge | festgelegt | 2900-3400, gute Starts 3160-3490 | [g] |
| Bein | festgelegt | 1400-1800, gute Starts 1580-1960 | [g] |
| Arm | nach oben festgelegt | bis 2000, ab 2300 deutlich schlechter, gute Starts 1750-2070 | [g] |
| Huefte Tiefe Bild 1 | festgelegt | 4800-5600 | [g] |
| Gier Bild 1 | festgelegt | -20 bis +10 Grad | [g] |
| Huefte Tiefe Bild 2 | eng | 6500-8000 | [g] |
| Kopfmitte Bild 2 | eng, wenn der Kopf die obere Rundung fuellt | x 290-325, y 190-230 | [g] |
| Schulter-Halbabstand | eng | 810-1020 | [g] |
| Dicken Arm, Bein, Schwanz | festgelegt, wo sichtbar | Schwanz 270-310 Wurzel, 155-210 Mitte, bis 250 Ende, Unterarm 230-320, Bein 140-220 | [g] |
| Gier Bild 2 | nicht festgelegt | -30 bis +45 gleich gut | [v] 21 |
| Rumpflaenge | nicht festgelegt | 1200-2800 gleich gut | [v] mindestens 1600 |
| Rechte Hand Bild 2 | nicht festgelegt | Boden oder bis Hoehe 650: 0,87 gegen 0,85 | [v] Boden |
| Beine, Schwanz Bild 2, linker Arm Bild 1 | nicht sichtbar | | [f] |
| Hals, Kiefer, Kopfform seitlich, Fusslaenge, Fingerzahl | nicht sichtbar | | [f] |

Gegenprobe: Die alte Spanne der Gier Bild 1 (5,7-17 Grad aus K2-K4) ueberlappt -20 bis +10 nur am Rand. Sie hing an Sohlen, die dort als offen standen und an der Anatomie mit vier Beinen.

Dunkelzone am Boden in Bild 2 (300-400,335-378): Das Rotrestbild zeigt dort Dunkelheit bis y 378. Das linke Bein steht im Abgabestand in dieser Zone [v]. Ob Bein oder Schatten, sagt das Bild nicht. V3 spricht fuer Figur (wahrscheinlich).

## 5. Posen

Beide in `gerippe.json` als Lage, Gier, Wurzel und Winkel je Knochen.

Pose 1, Bild 1: Lage (-19108, 0, -18539), Gier 4080 (358,6 Grad), Wurzel Hoehe 1854.

{{POSE1}}

Hand rechts endet bei (403,351), Fuss rechts Ferse gehoben, Spitze bei (318,421). Schwanzende (238,488), knapp unter dem Bild, Hoehe 230.

Pose 2, Bild 2: Lage (937, 0, -23690), Gier 239 (21 Grad), Wurzel Hoehe 1422.

{{POSE2}}

Rechte Schulter liegt rund 960 naeher an der Kamera als die linke, der Rumpf ist gegen den Kopf gedreht. Haende rund 2960 auseinander. Huefte rund 1650 von der Frau (Spawn), rechte Hand rund 3030.

## 6. Zeichnung auf dem Koerper

Aus dem Tiefenbild des Abgabestands, Hoehe auf der Modelloberflaeche. Die Zuordnung Knochen blieb in den drei Loesungen `ende3`, `m1`, `m2` gleich, ausser wo vermerkt. Die Spalte Hoehe haengt an der Tiefe des Modells, in Bild 1 auf rund 100 genau.

| Merkmal | Teil, Lage | Hoehe | Seite | Sicherheit |
|---|---|---|---|---|
| Z1 rosa Stelle | Koerperstueck, Hinterseite der Huefte, oben | 2080-2410 | rechts der Mitte 330-420 | wahrscheinlich |
| Z2 | Koerperstueck, Hinterseite der Huefte unter Z1 | 1910 | rechts 390 | wahrscheinlich |
| Z3, Z4 | oberes Ende rechter Oberschenkel, Rueckseite | 1620, 1530 | rechts 530-570 | wahrscheinlich |
| A05, A06 | Hinterseite Huefte, innen neben dem Schwanz | 1740, 1500 | rechts 200, Mitte | offen |
| Z5 Dreieck | rechter Arm, Ellbogen (Modell (396,289)), Rueckseite | 1350-1640 | rechts 1130-1170 | sicher als Stelle, Ellbogen wahrscheinlich |
| Z6 | A14 am rechten Knie, die zwei unteren auf der Wade | 1110, 1010, 890 | rechts 660-720 | A14 wahrscheinlich, sonst offen |
| Z7 | Bogen A20 am linken Knie, Rueckseite. Knickband (248,372), (249,381) liegt am Spalt Bein/Schwanz, (242,383) linke Wade | 1040, 970-1040, 640 | links 400 | Bein oder Schwanz offen |
| Z8 | (241,289) linker Oberschenkel, (253,285), (272,284) quer auf dem Schwanz, Abschnitt 3 bis 4 | 1640, 1780 | Mitte | Lage wahrscheinlich, Bedeutung offen |
| Z9 | ausserhalb aller Nutzer-Teile, 9-11 px neben dem Arm | | | offen, kann Hintergrund sein |
| A10 | Oberseite Schwanz, Abschnitt 2 | 2530 | rechts 230 | offen |
| X | Schwanzende, rosa | 560 und tiefer | Mitte | Nutzer |
| Zug Q | Vorderseite Kopf: Q1 ihre rechte Seite, Q2 oben, Q3 ihre linke Seite, Q5 oben ebenso. Q5 unten geht auf Brust oder linken Oberarm | 2480-2990 | | wahrscheinlich |
| Q4 Randband | linker Kopfrand, in `m2` linker Oberarm | 2600 | links | wahrscheinlich |
| P Spitze | Innenseite linker Oberarm unten, in `m2` Brust | 1220 | links | Stelle wahrscheinlich, Art offen |
| G | rechter Arm um den Ellbogen, Unterkante, Teil oben auf der Brust | 1220-1340 | | sicher als Stelle |
| B10, B11, B12 | Brust, ihre rechte Seite, kein Auge belegbar | 1650-1900 | | offen |
| Haken H | nicht zugeordnet | | | offen, siehe unten |

Haken H: Von 696 px liegen 393 im Nutzer-Umriss (untere Haelfte), der Balken oben ausserhalb. Das Modell deckt 467 px mit der Brust. Das ist keine Zuordnung. Der Haken kann Zeichnung auf dem Rumpf sein oder ein eigenes Stueck, die Bilder sagen es nicht.

Gesicht, erkennbar:
- Zug Q als ein Bogen: (262,225), (290,172), Sichel bis (345,189), (372,201), (388,236), dann nach unten bis P (366,304). Rund 125 px breit, 110 px hoch.
- Blaufleck B03 (327,168) darueber.
- Innenflaeche (290-340,225-280) ohne Zeichnung.
- Schwacher Fleck (305-335,265-290) im Blaukanal.

Der Bogen liegt 24 px innerhalb des Kopfumrisses im Abgabestand, Rand also frei. Mit dem kleineren Kopf aus `f5` traf er den Umriss auf 5 px. Ob er Stirnrand, Kopfrand oder Band auf dem Rumpf ist, zeigen die Bilder nicht.

Gesicht, frei: Augen, Nase, Maul, Zaehne, Kiefer, Hoerner. Die Punkte B10-B12 liegen im Abgabestand nicht auf dem Kopf. Liegt der Kopf links unten, treffen sie ihn, aber das kostet 0,93 gegen 0,74.

Einzeltabelle aller Punkte:

{{MERKMALE}}

## 7. Was im Bild noch abweicht

Bild 1:
- Schwanz unten: Ende ist ein runder Stab, der Keil unter dem Logo wird gedeckt, aber nicht in seiner Form.
- Hand rechts: rechte Spitze (427,362) fehlt. Bei y 362 ist das Modell 36 px breit, der Nutzer-Umriss 48 px.
- Lappen und Kerbe der Beine fehlen, Kegel sind glatt. Die Masse (218-250,395-440) zwischen Bein und Schwanz ist nur zur Haelfte gedeckt.
- Luecken zwischen den Beinen etwas zu klein: L1 178 von 1586 px, L2 223 von 1064 px belegt.
- Koerperstueck: Kante 14 px im schlechtesten Zehntel.

Bild 2:
- Oberkante links folgt dem Nutzer-Umriss, nicht der gelesenen Kante: 14-26 px darueber, dunkel auf dunkel.
- Spitze des rechten Arms (325-337,316-335) deckt die Brust, nicht der Arm. Daher IoU 0,71.
- Hand rechts: Finger sind runde Kegel, die Hand unter dem Logo ist rund, Fingerluecken fehlen.
- Fuesse sichtbar: rechter Fuss in der unteren linken Spitze, linkes Bein grau zwischen den Armen. Das ist Vorgabe [v], kein Nutzer-Teil.
- Kleine Schwanzwurzel oben links, 177 px, sollte verdeckt sein.

Beide: Keine Zeichnung, kein Schatten, Glieder als Kegel. Das Gerippe zeigt Masse und Lage, nicht die Haut.

## 8. Grenzen

- Kamera Bild 1 ist geschaetzt (`kamera.md`), Massstab auf rund 4 %.
- Die Nutzer-Umrisse sind Handschnitte. IoU unter 1 ist dort kein Fehler. Oberkante Bild 2 ist grosszuegiger als die Messkante.
- Gier und Kopf Bild 2 sind gesetzt, nicht gemessen. Die Bilder erlauben beide in den Spannen aus Abschnitt 4.
- Der Abgabestand ist eine Loesung von mehreren guten. Rumpf und Kopf gehoeren dem Modellbau.
- Port-Festwerte aus `werkzeug.md` (Clips, Kasten, Schatten) sind nicht geprueft.

## 9. Dateien

| Datei | Inhalt |
|---|---|
| `gerippe.json` | Knochen, Eltern, Versaetze, Dicken, Pose 1 und 2, Zuordnung, Restfehler, Merkmale |
| `gerippe_bild1.png`, `gerippe_bild2.png` | Kegelstuempfe ueber den Bildern, Teile farbig |
| `gerippe_rundum.png` | Seite, vorn, oben fuer Pose 1, Pose 2, Rast |
| `gerippe/` | Werkzeug und `DOSSIER.md` |
| `gerippe/lauf/ende3.pt` | Abgabestand |
| `gerippe/lauf/q_*.pt`, `profile_q.json`, `vergleich_k.txt` | Profile, Streuung der Starts |
