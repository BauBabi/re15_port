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

Achse des rechten Unterarms: Die rechte Kante des Streifens ist gerade, (335,326) bis (274,408). Der obere Teil liegt unter dem Rumpf, also ist nur diese Kante echt. Achse = Kante, 20 px nach links oben: (319,314) bis (258,396). Die Spitze (337,316) ist die Ellbogenkappe.

## 2. Gerippe

Rastpose: alle Winkel 0, Rumpf entlang +X, Glieder entlang +Y, Schwanz entlang -X. Mesh i haengt an Knochen i, Eltern vor Kindern. Knochen 10 = Brust, 13 und 18 = Haende, damit der Tick (Teile 10, 13, 18) ohne Aenderung trifft. Links gleich rechts. a = Halbachse Hoehe (X-Knochen) oder Tiefe (Y-Knochen), b = Halbachse Breite.

| Nr | Knochen | Eltern | Versatz | Laenge | Dicke a (Anfang/Mitte/Ende) | Dicke b | Bild 1 | Bild 2 |
|---|---|---|---|---|---|---|---|---|
| 0 | huefte | -1 | 0, 0, 0 | 449 | 603/715/862 | 934/916/916 | part_of_Body | face_and_upper_body |
| 1 | schwanz1 | 0 | -50, -757, 0 | 592 | 312/322/333 | 312/322/333 | tail | - |
| 2 | schwanz2 | 1 | -592, 0, 0 | 598 | 340/342/338 | 340/342/338 | tail | - |
| 3 | schwanz3 | 2 | -598, 0, 0 | 625 | 337/273/224 | 337/273/224 | tail | - |
| 4 | schwanz4 | 3 | -625, 0, 0 | 662 | 224/187/166 | 224/187/166 | tail | - |
| 5 | schwanz5 | 4 | -662, 0, 0 | 678 | 169/176/187 | 169/176/187 | tail | - |
| 6 | schwanz6 | 5 | -678, 0, 0 | 663 | 201/216/231 | 201/216/231 | tail | - |
| 7 | wirbel | 0 | 399, 0, 0 | 894 | 852/928/985 | 922/813/692 | part_of_Body | face_and_upper_body |
| 8 | oberschenkel_l | 0 | -500, 382, 558 | 953 | 245/208/199 | 245/208/199 | left_leg | - |
| 9 | oberschenkel_r | 0 | -500, 382, -558 | 953 | 245/208/199 | 245/208/199 | right_leg | - |
| 10 | brust | 7 | 894, 0, 0 | 897 | 1005/851/623 | 705/698/693 | part_of_Body | face_and_upper_body |
| 11 | oberarm_l | 10 | 742, 205, 898 | 797 | 331/300/280 | 331/300/280 | - | face_and_upper_body |
| 12 | unterarm_l | 11 | 0, 797, 0 | 1113 | 305/265/260 | 305/265/260 | - | left_arm_front |
| 13 | hand_l | 12 | 0, 1113, 0 | 689 | 298/183/113 | 233/379/508 | - | left_arm_front |
| 14 | klaue_l1 | 13 | 689, 0, 153 | 612 | 185/118/62 | 185/118/62 | - | left_arm_front |
| 15 | klaue_l2 | 13 | 689, 0, 0 | 612 | 185/118/62 | 185/118/62 | - | left_arm_front |
| 16 | oberarm_r | 10 | 742, 205, -898 | 797 | 331/300/280 | 331/300/280 | right_arm | - |
| 17 | unterarm_r | 16 | 0, 797, 0 | 1113 | 305/265/260 | 305/265/260 | right_arm | right_arm_front |
| 18 | hand_r | 17 | 0, 1113, 0 | 689 | 298/183/113 | 233/379/508 | right_arm | right_arm_front |
| 19 | klaue_r1 | 18 | 689, 0, 153 | 612 | 185/118/62 | 185/118/62 | right_arm | right_arm_front |
| 20 | klaue_r2 | 18 | 689, 0, 0 | 612 | 185/118/62 | 185/118/62 | right_arm | right_arm_front |
| 21 | hals | 10 | 897, 257, 0 | 503 | 453/481/512 | 453/481/512 | - | face_and_upper_body |
| 22 | kopf | 21 | 503, 0, 0 | 1298 | 515/532/519 | 492/556/361 | - | face_and_upper_body |
| 23 | kiefer | 22 | 500, 400, 0 | 695 | 233/183/143 | 370/302/324 | - | face_and_upper_body |
| 24 | unterschenkel_l | 8 | 0, 953, 0 | 829 | 212/165/141 | 212/165/141 | left_leg | - |
| 25 | fuss_l | 24 | 0, 829, 0 | 900 | 147/213/320 | 167/192/208 | left_leg | - |
| 26 | unterschenkel_r | 9 | 0, 953, 0 | 829 | 212/165/141 | 212/165/141 | right_leg | - |
| 27 | fuss_r | 26 | 0, 829, 0 | 900 | 147/213/320 | 167/192/208 | right_leg | - |
| 28 | klaue_l3 | 13 | 689, 0, -153 | 612 | 185/118/62 | 185/118/62 | - | left_arm_front |
| 29 | klaue_r3 | 18 | 689, 0, -153 | 612 | 185/118/62 | 185/118/62 | right_arm | right_arm_front |

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

| Bild | Teil | IoU sichtbar | Teil gedeckt | Figur deckt Teil | Ueberstand | Kante Median px | Kante 90 % px |
|---|---|---|---|---|---|---|---|
| 1 | tail | 0.86 | 0.93 | 0.96 | 0.01 | 1.0 | 5.7 |
| 1 | left_leg | 0.78 | 0.94 | 0.94 | 0.04 | 1.9 | 6.1 |
| 1 | right_leg | 0.81 | 0.85 | 0.93 | 0.00 | 0.0 | 3.9 |
| 1 | right_arm | 0.79 | 0.90 | 0.92 | 0.02 | 1.0 | 6.1 |
| 1 | part_of_Body | 0.69 | 0.78 | 0.98 | 0.04 | 1.9 | 12.6 |
| 2 | face_and_upper_body | 0.77 | 0.81 | 0.93 | 0.02 | 0.0 | 8.5 |
| 2 | left_arm_front | 0.70 | 0.90 | 0.95 | 0.11 | 3.7 | 11.0 |
| 2 | right_arm_front | 0.57 | 0.64 | 0.67 | 0.07 | 0.0 | 10.5 |

Ganzzahl-Probe: Pose ueber `form_emd.pose_aus_bild` nachgerechnet, 13 Einheiten, 1,0 px.

Unabhaengig nachgerechnet (`gegenprobe/render.py`, echte Kegelstuempfe als Vierecke, nur `gerippe.json`): Schwanz 0,87, Bein links 0,75, Bein rechts 0,81, Arm rechts 0,79, Koerperstueck 0,71, Oberkoerper 0,76, Arm links 0,71, Arm rechts vorn 0,55.

Pruefpunkte aus FORMBLATT Abschnitt 7:

| Punkt | Modell |
|---|---|
| B1 Scheitel y, Soll 187 | [187, 187, 195] |
| B1 Flanke (348, 205) Abstand px | 5.0 |
| B1 Flanke (366, 215) Abstand px | 2.2 |
| B1 Flanke (378, 233) Abstand px | 1.0 |
| B1 Rand links x bei y=232, Soll 247 | [252, 339] |
| B1 Arm x bei y=324, Soll 381-414 | [381, 411] |
| B1 Arm x bei y=344, Soll 381-418 | [381, 410] |
| B1 Bein rechts x bei y=322, Soll 300-347 | [301, 345] |
| B1 Bein rechts x bei y=346, Soll 308-344 | [307, 344] |
| B1 Sohle Hand y unter x=398, Soll 374 | 377 |
| B1 frei L1: Figurpixel | 231 von 1586 |
| B1 frei L2: Figurpixel | 292 von 1064 |
| B1 frei SCH: Figurpixel | 0 von 1120 |
| B2 Kante belegt Abstand px | [0.0, 1.0, 1.0, 0.0, 1.4, 0.0, 1.0] |
| B2 Kante gelesen Abstand px | [27.7, 26.2, 23.9, 19.4, 16.5, 7.2, 1.0] |
| B2 V1 Anteil auf Figur | 1.0 |
| B2 V2 Anteil auf Figur | 0.95 |
| B2 V3 Anteil auf Figur | 1.0 |
| B2 V4 Anteil auf Figur | 1.0 |
| B2 V6 Anteil auf Figur | 0.38 |
| B2 V5 Anteil auf Figur | 1.0 |
| B2 BO Anteil auf Figur | 0.0 |

## 4. Was die Bilder festlegen

Profile (`lauf/t_*.pt`): eine Groesse fest, alles andere 500 Schritte neu gerechnet, Ausgang `ende4`, Anatomie-Kosten an. Zahl = Bildanteil der Kosten, kleiner ist besser. Der Bildanteil liegt hoeher als in der ersten Fassung, weil die Oberarme nicht mehr zaehlen. Unterschiede unter rund 0,1 werte ich nicht. Einzelne Laeufe bleiben in Nebenminima haengen (Tiefe Bild 2 = 7000), die Zahl ist dort Obergrenze.

| Groesse | Wert: Bildanteil der Kosten |
|---|---|
| Arm (Ober- + Unterarm) | 1500: 4.35, 1700: 4.11, 1900: 3.79, 2100: 3.84, 2300: 3.94 |
| Bein (Ober- + Unterschenkel) | 1400: 4.18, 1600: 3.87, 1800: 3.97, 2000: 3.76, 2200: 3.77 |
| Rumpf (Huefte bis Brust) | 1500: 4.11, 1800: 3.83, 2100: 3.80, 2400: 3.78, 2800: 3.83 |
| Schwanz gesamt | 3000: 3.89, 3400: 3.78, 3800: 3.81, 4200: 3.82 |
| Huefte Tiefe Bild 1 | 4800: 4.59, 5200: 4.01, 5600: 3.83, 6000: 3.79 |
| Huefte Tiefe Bild 2 | 6000: 4.22, 6500: 3.94, 7000: 4.42, 7500: 3.84, 8000: 4.11 |
| Gier Bild 1 (Grad) | -20: 4.04, -5: 3.80, 10: 3.77, 25: 3.91 |
| Gier Bild 2 (Grad) | -40: 3.82, -25: 4.00, -15: 3.78, 0: 3.91, 21: 4.50, 45: 4.91 |
| Kopfmitte x Bild 2 | 290: 3.80, 310: 3.72, 325: 3.78, 345: 4.11, 365: 4.21 |
| Kopfmitte y Bild 2 | 215: 3.87, 230: 3.75, 245: 3.78, 260: 3.94, 280: 4.06 |

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

Pose 1, Bild 1: Lage (-18545, 0, -18368), Gier 83 (7,3 Grad), Wurzel Hoehe 1882.

| Gelenk | Bild | Hoehe | Tiefe |
|---|---|---|---|
| huefte | (303,262) | 1882 | 5842 |
| schwanz1 | (306,208) | 2635 | 5817 |
| schwanz2 | (287,221) | 2436 | 5273 |
| schwanz3 | (273,252) | 2053 | 4806 |
| schwanz4 | (268,299) | 1563 | 4404 |
| schwanz5 | (270,357) | 1055 | 3980 |
| schwanz6 | (266,423) | 642 | 3442 |
| wirbel | (316,262) | 1871 | 6224 |
| brust | (336,266) | 1760 | 7084 |
| hals | (349,255) | 1909 | 8000 |
| kopf | (355,259) | 1830 | 8488 |
| kiefer | (359,288) | 1222 | 8660 |
| oberarm_r | (392,254) | 1939 | 7705 |
| unterarm_r | (398,294) | 1280 | 7240 |
| hand_r | (394,357) | 378 | 6560 |
| oberarm_l | (304,259) | 1842 | 7973 |
| unterarm_l | (307,297) | 1169 | 7541 |
| hand_l | (324,353) | 307 | 6936 |
| oberschenkel_r | (320,300) | 1452 | 5192 |
| unterschenkel_r | (326,366) | 836 | 4466 |
| fuss_r | (306,436) | 390 | 3757 |
| oberschenkel_l | (249,286) | 1580 | 5513 |
| unterschenkel_l | (236,350) | 891 | 4832 |
| fuss_l | (209,420) | 292 | 4247 |

Beide Ellbogen fast gestreckt, Arm rund 35 Grad nach hinten geneigt, Schultern stehen rund 1100 vor den Handgelenken. Das folgt aus dem Unterarm: Bild 1 zeigt 934 senkrecht, Bild 2 verlangt 1100 Laenge. Fuss rechts Ferse gehoben, Spitze bei (317,406). Schwanzende (237,489) unter dem Bild, Hoehe 449, liegt also nicht auf [f]. Schultern Tiefe 7705 und 7973, Rumpf nicht mehr verdreht.

Pose 2, Bild 2: Lage (776, 0, -23713), Gier 3917 (344,3 Grad), Wurzel Hoehe 1543.

| Gelenk | Bild | Hoehe | Tiefe |
|---|---|---|---|
| huefte | (268,249) | 1543 | 7201 |
| schwanz1 | (253,204) | 2189 | 7355 |
| schwanz2 | (245,220) | 1850 | 7851 |
| schwanz3 | (240,237) | 1475 | 8354 |
| schwanz4 | (237,252) | 1084 | 8884 |
| schwanz5 | (233,264) | 712 | 9464 |
| schwanz6 | (228,270) | 415 | 10083 |
| wirbel | (279,241) | 1730 | 6877 |
| brust | (311,221) | 2144 | 6202 |
| hals | (359,247) | 1968 | 5559 |
| kopf | (344,240) | 2142 | 5104 |
| kiefer | (331,284) | 1816 | 4665 |
| oberarm_r | (292,262) | 1900 | 5149 |
| unterarm_r | (312,322) | 1143 | 5449 |
| hand_r | (257,397) | 340 | 5281 |
| oberarm_l | (399,223) | 2113 | 6187 |
| unterarm_l | (384,276) | 1353 | 6542 |
| hand_l | (410,349) | 318 | 6685 |
| oberschenkel_r | (234,287) | 953 | 7234 |
| unterschenkel_r | (253,313) | 860 | 6436 |
| fuss_r | (295,348) | 295 | 6676 |
| oberschenkel_l | (291,274) | 991 | 7864 |
| unterschenkel_l | (342,278) | 1031 | 7518 |
| fuss_l | (352,330) | 300 | 7329 |

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

| Merkmal | Bild | Punkt | Knochen | Nutzer-Teil | Hoehe | vor Huefte | links(+)/rechts(-) der Mitte | Abstand zum Modell px |
|---|---|---|---|---|---|---|---|---|
| Z1 | 1 | (325,231) | huefte | part_of_Body | 2312 | -193 | -458 | 0 |
| Z1o | 1 | (331,223) | huefte | part_of_Body | 2426 | 23 | -451 | 0 |
| Z1u | 1 | (312,250) | huefte | part_of_Body | 2066 | -441 | -380 | 0 |
| Z2 | 1 | (317,265) | huefte | part_of_Body | 1877 | -431 | -445 | 0 |
| Z3 | 1 | (326,293) | oberschenkel_r | right_leg | 1564 | -700 | -700 | 0 |
| Z4 | 1 | (320,303) | oberschenkel_r | right_leg | 1468 | -868 | -698 | 0 |
| A05 | 1 | (302,280) | huefte | part_of_Body | 1682 | -461 | -230 | 0 |
| A06 | 1 | (290,299) | huefte | part_of_Body | 1403 | -293 | 55 | 0 |
| A10 | 1 | (291,208) | schwanz2 | tail | 2576 | -790 | -280 | 0 |
| Z5sp | 1 | (400,277) | unterarm_r | right_arm | 1581 | 1643 | -1005 | 0 |
| Z5ba | 1 | (398,296) | unterarm_r | right_arm | 1295 | 1384 | -1035 | 0 |
| Z6a | 1 | (329,351) | oberschenkel_r | right_leg | 1049 | -1299 | -1013 | 0 |
| Z6b | 1 | (322,366) | unterschenkel_r | right_leg | 936 | -1454 | -1003 | 0 |
| Z6c | 1 | (314,384) | unterschenkel_r | right_leg | 813 | -1635 | -1000 | 0 |
| Z7a | 1 | (241,346) | oberschenkel_l | left_leg | 996 | -1301 | 138 | 0 |
| Z7b | 1 | (248,372) | schwanz5 | tail | 965 | -1979 | -463 | 3 |
| Z7k | 1 | (249,381) | schwanz5 | tail | 894 | -2007 | -493 | 2 |
| Z7c | 1 | (242,383) | unterschenkel_l | left_leg | 609 | -1375 | 79 | 4 |
| Z8a | 1 | (241,289) | oberschenkel_l | left_leg | 1578 | -848 | 468 | 0 |
| Z8b | 1 | (253,285) | schwanz4 | tail | 1717 | -1509 | -203 | 0 |
| Z8c | 1 | (272,284) | schwanz4 | tail | 1741 | -1540 | -470 | 0 |
| Z9a | 1 | (411,240) | oberarm_r | right_arm | 2201 | 2205 | -1077 | 7 |
| Z9b | 1 | (420,262) | oberarm_r | right_arm | 1815 | 2132 | -1256 | 10 |
| X | 1 | (268,450) | schwanz6 | tail | 717 | -2733 | -1191 | 0 |
| Q1a | 2 | (286,183) | brust | face_and_upper_body | 2663 | 1252 | -397 | 0 |
| Q1b | 2 | (277,206) | kopf | face_and_upper_body | 2703 | 2984 | -1568 | 0 |
| Q2 | 2 | (327,168) | brust | face_and_upper_body | 2853 | 1762 | -131 | 0 |
| Q2u | 2 | (330,190) | kopf | face_and_upper_body | 2763 | 2827 | -901 | 0 |
| Q3 | 2 | (375,205) | brust | face_and_upper_body | 2513 | 2480 | -19 | 0 |
| Q4 | 2 | (411,217) | oberarm_l | face_and_upper_body | 2236 | 2106 | 956 | 0 |
| Q5a | 2 | (392,240) | brust | face_and_upper_body | 2134 | 2561 | 174 | 0 |
| Q5b | 2 | (362,296) | kiefer | face_and_upper_body | 1729 | 2868 | -530 | 0 |
| Zug1 | 2 | (300,192) | kopf | face_and_upper_body | 2807 | 3074 | -1397 | 0 |
| Zug2 | 2 | (345,189) | brust | face_and_upper_body | 2665 | 2195 | -209 | 0 |
| Zug3 | 2 | (372,201) | brust | face_and_upper_body | 2553 | 2450 | -37 | 0 |
| Zug4 | 2 | (388,236) | hals | face_and_upper_body | 2197 | 2600 | 71 | 0 |
| Zug5 | 2 | (374,278) | kopf | face_and_upper_body | 1892 | 2887 | -389 | 0 |
| P | 2 | (366,304) | fuss_l | frei | 588 | 746 | 1444 | 7 |
| G1 | 2 | (338,320) | kiefer | face_and_upper_body | 1756 | 3264 | -1154 | 0 |
| G2 | 2 | (312,339) | unterarm_r | right_arm_front | 1064 | 2202 | -699 | 0 |
| G3 | 2 | (285,358) | unterarm_r | right_arm_front | 883 | 2114 | -1024 | 0 |
| B10 | 2 | (280,275) | kopf | face_and_upper_body | 2233 | 3238 | -1714 | 0 |
| B11 | 2 | (258,263) | wirbel | face_and_upper_body | 1605 | 812 | -628 | 0 |
| B12 | 2 | (217,274) | schwanz6 | frei | 176 | -3533 | 524 | 0 |
| H1 | 2 | (230,222) | huefte | face_and_upper_body | 2058 | 145 | -709 | 0 |
| H2 | 2 | (244,236) | wirbel | face_and_upper_body | 1936 | 580 | -706 | 0 |
| H3 | 2 | (226,256) | huefte | face_and_upper_body | 1551 | 26 | -761 | 0 |

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
