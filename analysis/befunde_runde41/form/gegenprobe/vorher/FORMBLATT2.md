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

| Teil | Bild | Schnitte (Lage: von-bis) | Mittellinie, je zweiter Stuetzpunkt (x,y) Breite |
|---|---|---|---|
| face_and_upper_body | 2 | Zeile 147: 288-332, 177: 253-373, 207: 237-409, 237: 241-425, 267: 202-399, 297: 210-360, 327: 227-321, 357: 236-271 | (393,233) 61 (367,233) 92 (342,233) 125 (317,235) 150 (302,255) 129 (286,275) 116 (272,296) 110 (265,320) 70 (254,343) 38 (243,366) 16 |
| left_arm_front | 2 | Zeile 265: 400-406, 295: 367-411, 325: 380-415, 355: 390-425, 385: 399-456, 415: 437-468 | (402,264) 4 (394,290) 31 (396,316) 36 (402,342) 30 (411,367) 35 (431,384) 33 (452,399) 10 |
| left_leg | 1 | Zeile 269: 233-242, 299: 235-246, 329: 226-249, 359: 213-251, 389: 199-230, 419: 195-217 | (237,272) 12 (240,298) 12 (237,324) 24 (236,350) 29 (222,372) 26 (208,394) 21 (206,420) 22 |
| part_of_Body | 1 | Zeile 210: 337-353, 240: 309-377, 270: 299-369, 300: 291-380 | (326,215) 4 (342,238) 52 (336,264) 68 (351,283) 40 (371,301) 18 |
| right_arm | 1 | Zeile 247: 379-397, 277: 377-405, 307: 384-417, 337: 383-417, 367: 377-418 | (401,256) 10 (390,275) 27 (400,303) 34 (398,332) 32 (405,359) 22 |
| right_arm_front | 2 | Spalte 148: 349-366, 178: 369-460, 208: 376-458, 238: 372-477, 268: 362-430, 298: 348-376, 328: 316-337 | (331,318) 4 (319,340) 10 (300,357) 19 (280,372) 31 (264,392) 32 (242,404) 58 (218,405) 48 (193,408) 44 (175,394) 39 (162,372) 17 (149,351) 4 |
| right_leg | 1 | Zeile 289: 298-333, 319: 298-344, 349: 312-343, 379: 302-342, 409: 292-330, 439: 288-328 | (314,305) 42 (325,329) 41 (326,356) 30 (320,383) 34 (313,409) 34 (307,435) 40 |
| tail | 1 | Zeile 189: 287-315, 219: 251-320, 249: 245-301, 279: 245-293, 309: 247-288, 339: 249-283, 369: 252-282, 399: 256-281, 429: 244-293, 459: 216-282 | (298,207) 45 (281,226) 53 (273,249) 56 (269,274) 49 (268,300) 44 (266,325) 36 (266,350) 33 (267,376) 30 (268,401) 26 (270,426) 38 (258,448) 50 (236,461) 34 (213,471) 14 |

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

| Nr | Knochen | Eltern | Versatz | Laenge | Dicke a (Anfang/Mitte/Ende) | Dicke b | Bild 1 | Bild 2 |
|---|---|---|---|---|---|---|---|---|
| 0 | huefte | -1 | 0, 0, 0 | 426 | 610/652/692 | 711/798/937 | part_of_Body | face_and_upper_body |
| 1 | schwanz1 | 0 | -50, -772, 0 | 553 | 271/282/293 | 271/282/293 | tail | - |
| 2 | schwanz2 | 1 | -553, 0, 0 | 554 | 300/304/306 | 300/304/306 | tail | - |
| 3 | schwanz3 | 2 | -554, 0, 0 | 565 | 307/252/209 | 307/252/209 | tail | - |
| 4 | schwanz4 | 3 | -565, 0, 0 | 575 | 209/176/155 | 209/176/155 | tail | - |
| 5 | schwanz5 | 4 | -575, 0, 0 | 563 | 158/169/186 | 158/169/186 | tail | - |
| 6 | schwanz6 | 5 | -563, 0, 0 | 532 | 208/231/251 | 208/231/251 | tail | - |
| 7 | wirbel | 0 | 376, 0, 0 | 865 | 692/745/819 | 929/876/950 | part_of_Body | face_and_upper_body |
| 8 | oberschenkel_l | 0 | -499, 280, 431 | 726 | 219/189/179 | 219/189/179 | left_leg | - |
| 9 | oberschenkel_r | 0 | -499, 280, -431 | 726 | 219/189/179 | 219/189/179 | right_leg | - |
| 10 | brust | 7 | 865, 0, 0 | 748 | 869/876/777 | 1078/790/692 | part_of_Body | face_and_upper_body |
| 11 | oberarm_l | 10 | 813, 63, 814 | 601 | 274/350/416 | 274/350/416 | - | left_arm_front |
| 12 | unterarm_l | 11 | 0, 601, 0 | 1096 | 320/248/233 | 320/248/233 | - | left_arm_front |
| 13 | hand_l | 12 | 0, 1096, 0 | 452 | 236/276/320 | 232/276/290 | - | left_arm_front |
| 14 | klaue_l1 | 13 | 452, 0, 87 | 675 | 213/148/94 | 213/148/94 | - | left_arm_front |
| 15 | klaue_l2 | 13 | 452, 0, 0 | 675 | 213/148/94 | 213/148/94 | - | left_arm_front |
| 16 | oberarm_r | 10 | 813, 63, -814 | 601 | 274/350/416 | 274/350/416 | right_arm | right_arm_front |
| 17 | unterarm_r | 16 | 0, 601, 0 | 1096 | 320/248/233 | 320/248/233 | right_arm | right_arm_front |
| 18 | hand_r | 17 | 0, 1096, 0 | 452 | 236/276/320 | 232/276/290 | right_arm | right_arm_front |
| 19 | klaue_r1 | 18 | 452, 0, 87 | 675 | 213/148/94 | 213/148/94 | right_arm | right_arm_front |
| 20 | klaue_r2 | 18 | 452, 0, 0 | 675 | 213/148/94 | 213/148/94 | right_arm | right_arm_front |
| 21 | hals | 10 | 748, 100, 0 | 388 | 472/444/417 | 472/444/417 | - | face_and_upper_body |
| 22 | kopf | 21 | 388, 0, 0 | 1219 | 416/554/750 | 469/499/483 | - | face_and_upper_body |
| 23 | kiefer | 22 | 0, 50, 0 | 550 | 235/175/128 | 340/291/186 | - | face_and_upper_body |
| 24 | unterschenkel_l | 8 | 0, 726, 0 | 826 | 184/161/140 | 184/161/140 | left_leg | - |
| 25 | fuss_l | 24 | 0, 826, 0 | 893 | 143/176/218 | 154/167/195 | left_leg | - |
| 26 | unterschenkel_r | 9 | 0, 726, 0 | 826 | 184/161/140 | 184/161/140 | right_leg | - |
| 27 | fuss_r | 26 | 0, 826, 0 | 893 | 143/176/218 | 154/167/195 | right_leg | - |
| 28 | klaue_l3 | 13 | 452, 0, -87 | 675 | 213/148/94 | 213/148/94 | - | left_arm_front |
| 29 | klaue_r3 | 18 | 452, 0, -87 | 675 | 213/148/94 | 213/148/94 | right_arm | right_arm_front |

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

| Bild | Teil | IoU sichtbar | Teil gedeckt | Figur deckt Teil | Ueberstand | Kante Median px | Kante 90 % px |
|---|---|---|---|---|---|---|---|
| 1 | tail | 0.85 | 0.93 | 0.96 | 0.02 | 1.0 | 5.6 |
| 1 | left_leg | 0.76 | 0.91 | 0.91 | 0.04 | 2.1 | 5.7 |
| 1 | right_leg | 0.82 | 0.86 | 0.94 | 0.01 | 0.0 | 3.8 |
| 1 | right_arm | 0.78 | 0.91 | 0.94 | 0.03 | 2.3 | 5.7 |
| 1 | part_of_Body | 0.70 | 0.79 | 0.98 | 0.06 | 1.9 | 13.8 |
| 2 | face_and_upper_body | 0.89 | 0.94 | 0.96 | 0.02 | 1.0 | 8.4 |
| 2 | left_arm_front | 0.83 | 0.93 | 0.96 | 0.01 | 1.4 | 5.1 |
| 2 | right_arm_front | 0.71 | 0.77 | 0.85 | 0.01 | 0.0 | 4.6 |

Ganzzahl-Probe: Pose ueber `form_emd.pose_aus_bild` nachgerechnet, 13 Einheiten, 0,7 px.

Pruefpunkte aus FORMBLATT Abschnitt 7:

| Punkt | Modell |
|---|---|
| B1 Scheitel y, Soll 187 | [186, 186, 194] |
| B1 Flanke (348, 205) Abstand px | 3.2 |
| B1 Flanke (366, 215) Abstand px | 3.6 |
| B1 Flanke (378, 233) Abstand px | 2.2 |
| B1 Rand links x bei y=232, Soll 247 | [249, 339] |
| B1 Arm x bei y=324, Soll 381-414 | [380, 414] |
| B1 Arm x bei y=344, Soll 381-418 | [380, 418] |
| B1 Bein rechts x bei y=322, Soll 300-347 | [305, 348] |
| B1 Bein rechts x bei y=346, Soll 308-344 | [308, 342] |
| B1 Sohle Hand y unter x=398, Soll 374 | 376 |
| B1 frei L1: Figurpixel | 178 von 1586 |
| B1 frei L2: Figurpixel | 223 von 1064 |
| B1 frei SCH: Figurpixel | 0 von 1120 |
| B2 Kante belegt Abstand px | [1.4, 1.0, 1.4, 0.0, 3.6, 2.8, 5.4] |
| B2 Kante gelesen Abstand px | [14.0, 20.6, 26.0, 22.1, 16.0, 8.1, 4.5] |
| B2 V1 Anteil auf Figur | 1.0 |
| B2 V2 Anteil auf Figur | 0.88 |
| B2 V3 Anteil auf Figur | 0.72 |
| B2 V4 Anteil auf Figur | 1.0 |
| B2 V6 Anteil auf Figur | 0.0 |
| B2 V5 Anteil auf Figur | 1.0 |
| B2 BO Anteil auf Figur | 0.0 |

## 4. Was die Bilder festlegen

Profile (`lauf/q_*.pt`): eine Groesse fest, alles andere 500 Schritte neu gerechnet, Ausgang `m4`. Zahl = Bildanteil der Kosten, kleiner ist besser. Die Laeufe am Ausgangswert liegen bei 0,74-0,75. Unterschiede unter rund 0,05 werte ich nicht.

| Groesse | Wert: Bildanteil der Kosten |
|---|---|
| Arm (Ober- + Unterarm) | 1400: 0.75, 1600: 0.73, 1800: 0.75, 2000: 0.76, 2300: 0.97, 2600: 1.22 |
| Bein (Ober- + Unterschenkel) | 1200: 0.85, 1400: 0.77, 1600: 0.75, 1800: 0.77, 2100: 0.90, 2500: 1.16 |
| Rumpf (Huefte bis Brust) | 1200: 0.74, 1500: 0.74, 1800: 0.74, 2100: 0.75, 2400: 0.75, 2800: 0.78 |
| Schwanz gesamt | 2400: 0.82, 2900: 0.75, 3400: 0.74, 3900: 0.86, 4400: 0.84 |
| Huefte Tiefe Bild 1 | 4400: 0.79, 4800: 0.75, 5200: 0.74, 5600: 0.76, 6000: 0.83 |
| Huefte Tiefe Bild 2 | 5500: 0.98, 6000: 0.83, 6500: 0.78, 7000: 0.74, 7500: 0.70, 8000: 0.75 |
| Gier Bild 1 (Grad) | -35: 0.82, -20: 0.76, -5: 0.74, 10: 0.73, 25: 0.80, 40: 0.92 |
| Gier Bild 2 (Grad) | -30: 0.77, 0: 0.72, 21: 0.75, 45: 0.76, 70: 0.84, 95: 0.92 |
| Kopfmitte x Bild 2 | 260: 0.93, 290: 0.73, 325: 0.74, 360: 0.92, 390: 1.05 |
| Kopfmitte y Bild 2 | 190: 0.75, 230: 0.74, 270: 0.83, 310: 1.00 |

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

| Gelenk | Bild | Hoehe | Tiefe |
|---|---|---|---|
| huefte | (300,267) | 1854 | 5259 |
| schwanz1 | (306,206) | 2620 | 5284 |
| schwanz2 | (289,215) | 2472 | 4754 |
| schwanz3 | (275,244) | 2149 | 4295 |
| schwanz4 | (269,291) | 1701 | 3936 |
| schwanz5 | (270,352) | 1194 | 3657 |
| schwanz6 | (265,420) | 678 | 3414 |
| wirbel | (309,267) | 1824 | 5630 |
| brust | (322,272) | 1709 | 6482 |
| hals | (346,255) | 1940 | 7132 |
| kopf | (354,256) | 1921 | 7502 |
| kiefer | (354,258) | 1876 | 7481 |
| oberarm_r | (388,252) | 2003 | 6808 |
| unterarm_r | (396,289) | 1416 | 6824 |
| hand_r | (393,357) | 324 | 6728 |
| oberarm_l | (311,251) | 1999 | 7537 |
| unterarm_l | (313,283) | 1412 | 7641 |
| hand_l | (319,343) | 324 | 7539 |
| oberschenkel_r | (316,297) | 1557 | 4680 |
| unterschenkel_r | (326,358) | 981 | 4252 |
| fuss_r | (305,435) | 482 | 3583 |
| oberschenkel_l | (249,284) | 1676 | 4800 |
| unterschenkel_l | (236,347) | 979 | 4612 |
| fuss_l | (208,422) | 232 | 4322 |

Hand rechts endet bei (403,351), Fuss rechts Ferse gehoben, Spitze bei (318,421). Schwanzende (238,488), knapp unter dem Bild, Hoehe 230.

Pose 2, Bild 2: Lage (937, 0, -23690), Gier 239 (21 Grad), Wurzel Hoehe 1422.

| Gelenk | Bild | Hoehe | Tiefe |
|---|---|---|---|
| huefte | (272,259) | 1422 | 7089 |
| schwanz1 | (259,209) | 2167 | 7009 |
| schwanz2 | (267,213) | 2022 | 7559 |
| schwanz3 | (276,223) | 1772 | 8088 |
| schwanz4 | (286,237) | 1435 | 8587 |
| schwanz5 | (296,253) | 1038 | 9049 |
| schwanz6 | (307,269) | 626 | 9454 |
| wirbel | (269,265) | 1441 | 6719 |
| brust | (281,275) | 1545 | 5895 |
| hals | (315,271) | 1761 | 5296 |
| kopf | (318,243) | 2115 | 5065 |
| kiefer | (322,246) | 2091 | 5065 |
| oberarm_r | (262,297) | 1649 | 4760 |
| unterarm_r | (284,351) | 1107 | 4774 |
| hand_r | (229,444) | 307 | 4492 |
| oberarm_l | (363,241) | 1992 | 5725 |
| unterarm_l | (384,280) | 1479 | 5955 |
| hand_l | (402,346) | 465 | 6442 |
| oberschenkel_r | (257,275) | 1020 | 7656 |
| unterschenkel_r | (249,298) | 875 | 6996 |
| fuss_r | (247,352) | 245 | 6619 |
| oberschenkel_l | (305,263) | 1234 | 7598 |
| unterschenkel_l | (325,284) | 1089 | 7016 |
| fuss_l | (352,330) | 383 | 7132 |

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

| Merkmal | Bild | Punkt | Knochen | Nutzer-Teil | Hoehe | vor Huefte | links(+)/rechts(-) der Mitte | Abstand zum Modell px |
|---|---|---|---|---|---|---|---|---|
| Z1 | 1 | (325,231) | huefte | part_of_Body | 2300 | -199 | -423 | 0 |
| Z1o | 1 | (331,223) | wirbel | part_of_Body | 2414 | 240 | -375 | 0 |
| Z1u | 1 | (312,250) | huefte | part_of_Body | 2078 | -434 | -328 | 0 |
| Z2 | 1 | (317,265) | huefte | part_of_Body | 1907 | -437 | -388 | 0 |
| Z3 | 1 | (326,293) | oberschenkel_r | right_leg | 1621 | -677 | -567 | 0 |
| Z4 | 1 | (320,303) | oberschenkel_r | right_leg | 1529 | -794 | -532 | 0 |
| A05 | 1 | (302,280) | huefte | part_of_Body | 1735 | -469 | -206 | 0 |
| A06 | 1 | (290,299) | huefte | part_of_Body | 1500 | -378 | -16 | 0 |
| A10 | 1 | (291,208) | schwanz2 | tail | 2528 | -839 | -235 | 0 |
| Z5sp | 1 | (400,277) | oberarm_r | right_arm | 1637 | 1377 | -1165 | 0 |
| Z5ba | 1 | (398,296) | oberarm_r | right_arm | 1348 | 1368 | -1128 | 0 |
| Z6a | 1 | (329,351) | unterschenkel_r | right_leg | 1111 | -1099 | -717 | 0 |
| Z6b | 1 | (322,366) | unterschenkel_r | right_leg | 1009 | -1260 | -691 | 0 |
| Z6c | 1 | (314,384) | unterschenkel_r | right_leg | 891 | -1422 | -661 | 0 |
| Z7a | 1 | (241,346) | unterschenkel_l | left_leg | 1041 | -862 | 400 | 0 |
| Z7b | 1 | (248,372) | schwanz5 | tail | 1037 | -1672 | -114 | 2 |
| Z7k | 1 | (249,381) | schwanz5 | tail | 969 | -1699 | -137 | 2 |
| Z7c | 1 | (242,383) | unterschenkel_l | left_leg | 635 | -851 | 404 | 5 |
| Z8a | 1 | (241,289) | oberschenkel_l | left_leg | 1642 | -721 | 461 | 0 |
| Z8b | 1 | (253,285) | schwanz4 | tail | 1773 | -1452 | -71 | 0 |
| Z8c | 1 | (272,284) | schwanz4 | tail | 1792 | -1495 | -289 | 0 |
| Z9a | 1 | (411,240) | oberarm_r | right_arm | 2199 | 1677 | -1325 | 11 |
| Z9b | 1 | (420,262) | oberarm_r | right_arm | 1851 | 1730 | -1461 | 9 |
| X | 1 | (268,450) | schwanz6 | tail | 561 | -2022 | -464 | 0 |
| Q1a | 2 | (286,183) | kopf | face_and_upper_body | 2893 | 3324 | 539 | 0 |
| Q1b | 2 | (277,206) | kopf | face_and_upper_body | 2726 | 3337 | 463 | 0 |
| Q2 | 2 | (327,168) | kopf | face_and_upper_body | 2994 | 3269 | 858 | 0 |
| Q2u | 2 | (330,190) | kopf | face_and_upper_body | 2851 | 3405 | 879 | 0 |
| Q3 | 2 | (375,205) | kopf | face_and_upper_body | 2737 | 3358 | 1222 | 0 |
| Q4 | 2 | (411,217) | kopf | face_and_upper_body | 2598 | 3058 | 1552 | 1 |
| Q5a | 2 | (392,240) | kopf | face_and_upper_body | 2482 | 3371 | 1357 | 0 |
| Q5b | 2 | (362,296) | brust | face_and_upper_body | 1548 | 1955 | 1254 | 0 |
| Zug1 | 2 | (300,192) | kopf | face_and_upper_body | 2836 | 3394 | 650 | 0 |
| Zug2 | 2 | (345,189) | kopf | face_and_upper_body | 2854 | 3378 | 992 | 0 |
| Zug3 | 2 | (372,201) | kopf | face_and_upper_body | 2765 | 3352 | 1199 | 0 |
| Zug4 | 2 | (388,236) | kopf | face_and_upper_body | 2516 | 3389 | 1323 | 0 |
| Zug5 | 2 | (374,278) | kopf | face_and_upper_body | 2167 | 3301 | 1234 | 0 |
| P | 2 | (366,304) | oberarm_l | left_arm_front | 1223 | 1320 | 1361 | 0 |
| G1 | 2 | (338,320) | brust | face_and_upper_body | 1325 | 2109 | 977 | 0 |
| G2 | 2 | (312,339) | oberarm_r | right_arm_front | 1340 | 2648 | 697 | 0 |
| G3 | 2 | (285,358) | oberarm_r | right_arm_front | 1223 | 2835 | 430 | 0 |
| B10 | 2 | (280,275) | brust | face_and_upper_body | 1849 | 2217 | 347 | 0 |
| B11 | 2 | (258,263) | brust | face_and_upper_body | 1904 | 1966 | 72 | 0 |
| B12 | 2 | (217,274) | brust | face_and_upper_body | 1649 | 1558 | -522 | 0 |
| H1 | 2 | (230,222) | brust | face_and_upper_body | 2193 | 1126 | -403 | 5 |
| H2 | 2 | (244,236) | brust | face_and_upper_body | 2138 | 1678 | -125 | 0 |
| H3 | 2 | (226,256) | brust | face_and_upper_body | 1881 | 1594 | -381 | 0 |

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
