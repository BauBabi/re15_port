# Leser B: Formbeschreibung Riesenform (Bahnsteig 5080 / Zug 5090)

Stand 2026-11-05 11:40. Ich habe nur gelesen und gemessen (Python: PIL/numpy/cv2, Werte in `scratchpad/leser/leser_b/*.json`) und die Bilder erst danach angesehen.
Pixel = Original 640x480 (Bild 2 = `vlcsnap-2026-10-31-00h29m02s830.png`, Bild 1 = `...00h28m45s774.png`), y zeigt nach unten.
Kennung: **[O]** im Original gemessen, **[K]** nur in KI-Bildern, **[N]** Vorgabe oder Beschriftung des Nutzers, **[g]** geschaetzt (mit Rechenweg).
Seiten: (W) = aus Sicht des Wesens, (B) = aus Sicht des Betrachters.

## 0. Material und Messstand

| Befund | Wert | Kennung |
|---|---|---|
| `Vorderseite_Beschreibung.png` | Messstand = Datei vom 2026-11-05 **11:17:07**, sha1 `226c793a...`, 1599x960 (Bild x 0-1279, rechts weisse Leinwand fuer die Schrift). Der Nutzer hat die Datei WAEHREND des Lesens neu gespeichert ("korrigiert."). Die erste Messung vor 11:17 hatte 7 Linien, die jetzige 8 (neu ist Linie 6). Kopie: `leser_b/vb_snapshot.png`. Um 11:38 unveraendert | Messung |
| Bildinhalt | bytegleich (MAE 0,0) mit `vlcsnap-2026-11-04-16h27m04s664.png` (KI-Fassung von Bild 2) | Messung |
| KI -> Original | ECC affin 0,996: Massstab 2,000, Versatz (-0,02 / -0,85) KI-px. Original-px = KI-px / 2 (Fehler <= 0,5 px) | Messung |
| Linienfarbe | exakt RGB (237,28,36), 8 Linien, Strich 5,0-5,4 px, Endpunkte per Skelett | Messung |
| Original-Video | `Biohazard 1.5 Final Build.mp4` ist eine Diashow aus Standbildern zu je 24 Videobildern. Bild 1 = 114,55-115,32 s, Bild 2 = 116,15-116,92 s (MAE 0,0). Die Standbilder davor, dazwischen und danach zeigen andere Raeume. **Es gibt genau zwei Originalbilder der Riesenform und keine Bewegung.** | [O] |

Die Grob-Lesung im Auftrag passt nicht zur Datei von 11:17:07: Dort sind (559,799) und (906,759) vertauscht. Nach dem Linienverlauf gehoert (559,799) zu "Ende, untere Vorderseite vom Maul" und (906,759) zu "linke Klaue". Neu ist Linie 6 "Spitze - Obere Vorderseite vom Maul".

## 1. Kopf und Maul (Bild 2)

### 1.1 Linien-Endpunkte des Nutzers [N], gemessen

| Nr | Beschriftung | Endpunkt KI-px | Original-px | liegt in Nutzer-Teil | Farbe Original (RGB, Y) |
|---|---|---|---|---|---|
| 1 | rechtes Auge | (447,542) | (223,5 / 271) | face_and_upper_body | (39,7,19) Y 18 |
| 2 | linkes Auge | (578,538) | (289 / 269) | face_and_upper_body | (34,8,12) Y 16 |
| 3 | Oberseite vom Maul | (624,662), Knick (638,603) | (312 / 331), Knick (319 / 301,5) | Oberkiefer, 3,6 px vor dem Unterkiefer | (54,7,31) Y 24 |
| 4 | offenes Maul nach rechts gerichtet | (660,652) | (330 / 326) | Unterkiefer (oberes Bandende) | (183,70,126) Y 110 |
| 5 | Ende, untere Vorderseite vom Maul | (559,799) | (279,5 / 399,5) | Unterkante des Unterkiefers (1 px) | (193,90,89) Y 121 |
| 6 | Spitze - Obere Vorderseite vom Maul | (492,743) | (246 / 371,5) | Oberkiefer, tiefster Punkt (Maske bei x 247: y 372) | (75,15,25) Y 34 |
| 7 | linke Klaue | (906,759) | (453 / 379,5) | Hintergrund, 2 px neben left_arm_front | (146,121,123) |
| 8 | rechte Klaue | (564,868) | (282 / 434) | Boden, 5,4 px neben right_arm_front | (210,186,184) |

Die Linien enden kurz VOR dem Merkmal (Klauen 2-6 px, Augen 8-11 px). In ihrer Verlaengerung liegt jeweils das Merkmal (Abschnitte 1.2 und 2.1).

### 1.2 Augen

| | Nutzer-Endpunkt | heller Fleck in der Linien-Verlaengerung | Prominenz Y Original | KI 27m04 / KI 26m29 | Kennung |
|---|---|---|---|---|---|
| rechtes Auge (W) = links im Bild (B) | (223,5/271) | **(217,275)**, 7,6 px weiter | 11,5 = Rauschniveau (im dunklen Kopf liegen 3 weitere Flecke mit 11,9-12,6, z.B. (258,263)) | 31,5 / 45,5 | Fleck [O] schwach, Deutung [N] |
| linkes Auge (W) = rechts im Bild (B) | (289/269) | **(280,275)**, 10,8 px weiter, RGB (82,36,52) | 23,6, staerkster Fleck im dunklen Kopf (x 200-345, y 250-320) | 44,2 / 86,2 | [O] |

- Beide Flecke liegen auf gleicher Hoehe (dy 0), Abstand **63 px**. Beide liegen OBERHALB des Nutzer-Teils Oberkiefer.png: das rechte 5 px ueber dessen Ecke (217,280), das linke rund 25 px ueber dessen Oberkante (bei x 280: y 300).
- Die KI verstaerkt beide Flecke 2- bis 4-fach zu "Augen". Einen roten Glanz zeigt das Original nicht (Fleckfarben dunkelrot-violett).

### 1.3 Oberkiefer = dunkler Keil [O]

Schwelle Y < 50 (Gauss 1,0), Zeilen von der Schnauzenachse x 247 aus:

| Zeile y | 324 | 336 | 348 | 360 | 372 |
|---|---|---|---|---|---|
| Keil x von-bis (Breite) | 213-323 (110) | 222-311 (89) | 229-287 (58) | 229-261 (32) | 236-260 (24) |

- Linker Rand 20,6 Grad, rechter Rand 58,6 Grad gegen die Senkrechte (Fit y 324-368), Schnittpunkt (237,379). Die Nutzer-Spitze 6 (246,371,5) liegt 11,8 px davon und genau am tiefsten Punkt von Oberkiefer.png.
- Der rechte Keilrand ist zugleich die Innenkante des hellen Bandes (Abstand 2-4 px). Linie 3 "Oberseite vom Maul" endet auf diesem Rand: Ihr letztes Stueck laeuft vom Knick (319,301,5) senkrecht nach unten, die Verlaengerung trifft nach 6 px das Band. Die Oberseite des Mauls ist also die Oberlippe bzw. der Zahnrand des Oberkiefers an der Maulspalte.
- Links grenzt der Keil an eine weisse Fleckgruppe (161-221, 329-386, RGB (209,199,196), 1837 px). Sie liegt in KEINEM Nutzer-Teil (13 px Ueberlappung).

### 1.4 Unterkiefer = helles Band [O]

- Nutzer-Teil Unterkiefer.png: Hauptachse 38,5 Grad steigend (von unten links nach oben rechts), 115 px lang, Schwerpunkt (293,6/364,0).
- 5 helle Flecke auf dem Band (Prominenz 20-60): (257,385), (288,370), (292,352), (307,348), (325,333). Abstaende 34 / 18 / 16 / 23 px. Sie liegen in beiden KI-Bildern an denselben Stellen (+-2 px), sind also nicht erfunden. (E20 nannte 6 Flecke im Abstand 16 px.)
- Profil quer zum Band: innen dunkel-violett (R 50-100, G 6-20, Y 18-28), Band hell rosa-weiss (Y 90-200), aussen (rechts unten) eine **tiefrote Zone** von 20-35 px Breite (R 100-200, G 10-35, B 20-50, Y 30-80), dahinter Boden bzw. linke Klaue.
- Punkt 5 liegt an der Band-UNTERkante bei x 279. Das aeusserste untere Bandende liegt 20 px weiter links (243-261, 374-403).

### 1.5 Maulspalt

| Mass | Wert | Kennung |
|---|---|---|
| Winkel rechter Keilrand / Bandachse | 31,4 / 38,5 Grad, Differenz **7 Grad**. Zwischen Keil und Band ist **keine dunkle Luecke** sichtbar | [O] |
| Winkel am oberen Bandende (4) zwischen den Richtungen zu 6 und zu 5 | **27 Grad** im Bild | [O], Punkte [N] |
| fruehere Lesung (E20, K.md Abschnitt 3) | 50-56 Grad, gemessen an der OBERKANTE von Oberkiefer.png. Diese Kante ist nach den Augen nicht die Kopfoberlinie, denn die Augen liegen darueber | Widerspruch |
| Oeffnung in 3D | aus einem Standbild nicht messbar | offen |

### 1.6 Schnauzenlaenge zu Kopfbreite

| Mass | Bild-px | Einheiten bei Tiefe d (u = px * d / f) | Kennung |
|---|---|---|---|
| Augenabstand | 63 | 698 (d 5000) bis 822 (d 5881) | Flecke [O], d [g] |
| Keilbreite bei y 324 (erste Zeile unter den Augen mit beiden Raendern) | 110 | - | [O] |
| Augenmitte (248,5/275) -> Spitze 6 | 96,5, 1,5 Grad neben der Senkrechten | >= 1009 (d 5000) bis >= 1187 (d 5881), nur UNTERGRENZE wegen Verkuerzung | [O]/[N] |
| Verhaeltnis Schnauze / Augenabstand im Bild | 1,53 (Tripo VORNE: 1,48) | in 3D groesser, wenn die Schnauze zur Kamera zeigt | [O] |

Zur Tiefe d: Kamera Bild 2 = ROOM5090 Cut 11 (kamera.md, sicher), fx 451, fy 478, Blick 13,1 Grad nach unten. Die Frau steht in rund 6690 Tiefe (aus Augpunkt, Ziel und ihrem Standort in kamera.md), der J-Koerper hinter dem Oberkiefer in 5881 (K.md Abschnitt 1). Der Kopf liegt davor; 5000-5881 ist geschaetzt.
Zum Vergleich K4 (`K4_kroko/kopf.json`): Augenwuelste bei z +-0,52 Wk = +-199, also **Augenabstand 397**, Schaedelbreite 764, Auge -> Spitze 1620-520 = 1100.

### 1.7 Blickrichtung des Kopfes

- **Kein Profil: Der Kopf zeigt frontal zur Kamera.** Belege [O]: Beide Augenflecke sind sichtbar und liegen waagerecht (dy 0), die Schnauzenspitze sitzt 2,5 px neben der Augenmitte genau darunter.
- Die Schnauze zeigt zur Kamera und nach unten (verkuerzt): Spitze 96,5 px unter der Augenlinie, die Kamera schaut von oben (13 Grad).
- Das Maul ist zur linken unteren Bildseite offen: Beide Kieferenden (6 oben, 5 unten) liegen links unterhalb des Maulwinkels (4). Das ist "nach rechts" (W), also zur Frau hin, und deckt sich mit "offenes Maul (schraeg nach links)" (B) in `image_3 (1).png`. Die sichtbare Spalte mit Zahnreihe liegt rechts der Schnauze (B), also auf der linken Kopfseite (W).

### 1.8 Vereinbarkeit mit der frueheren Lesung

| Fruehere Lesung | Neue Messung | Urteil |
|---|---|---|
| Oberkiefer.png = dunkle Schnauzenmasse | Keil = Oberkiefer, tiefster Maskenpunkt = Spitze 6 | vereinbar |
| Oberkiefer.png: "links breit = Schnauze vorn, rechts schmal = Hals" (K.md 3), Schnauze nach links | Augen oben bei x 217/280, Spitze UNTEN bei (246,371,5): Die Schnauze zeigt nach unten zur Kamera | **Widerspruch** (Richtung) |
| Unterkiefer.png = rotes Band mit Zaehnen | Band mit 5 Flecken, Punkte 4 und 5 darauf, Punkt 3 an seiner Innenkante | vereinbar |
| Maulwinkel 50-56 Grad im Bild | 27 Grad (Konstruktion 4-6-5) bzw. 7 Grad (Keilrand/Band) | Widerspruch, weil die Bezugslinie die Maskenoberkante war |
| K4 im Spiel (zurueckgerechnet aus `stufe3/nachbesserung2/kopf/bild2_masken_alt_neu.png`): sichtbarer Kopf x 223-356, y 283-366, Spitze links (223,329), Schaedel rechts oben (330-356, 283-296) | Die Augen bei y 275 liegen AUSSERHALB des K4-Kopfs (bei x 280 beginnt K4 erst um y 300). K4-Augen [g] um (301,302) (41 % ab Schaedelende, lineare Interpolation) | **Widerspruch** (Augenlage, Kopfrichtung) |
| K4 Augenabstand 397 Einheiten | Das Bild verlangt rund 700-820 | **Widerspruch**: Der K4-Kopf ist rund halb so breit wie gemessen (nur wenn die Augendeutung gilt) |
| K4 tiefster Punkt (243,366) | Spitze 6 (246,371,5), 6 px entfernt | vereinbar (einziger Deckungspunkt) |
| Zielkonflikt NB2-K1: Kopfursprung hoechstens rund 40 px vom J-Halsende (342,280) | Die Augenmitte liegt 93,6 px davon | mit J-Hals und J-Rumpfpose **nicht erreichbar**; Hals oder Rumpflage muss sich aendern |

## 2. Klauen

### 2.1 Lage [N] + [O]

| Beschriftung | Endpunkt | Verlaengerung trifft | Gemessene Spitzen (Kontur des Nutzer-Teils) | Seite (B) |
|---|---|---|---|---|
| linke Klaue | (453/379,5) | left_arm_front nach 3 px bei (450,381) | (439,418), (468,416) unten; Flanke (464,392) | rechts im Bild |
| rechte Klaue | (282/434) | right_arm_front nach 6 px bei (276,434) (dunkle Hand 158-275 x 385-445) | (274,443) deutlich; (229,351) nach oben; Ende (148,349); (249,476) und (197,476) nur im Nutzer-Schnitt unter dem Logo (K.md 2) | links unten im Bild |

### 2.2 Seitenkonvention der Klauen

- Lesart Wesenssicht: "linke Klaue" = linke Hand des Wesens = rechts im Bild. **Passt** zur Linienlage, zu den Augen (rechtes Auge (W) links im Bild), zu den alten Freisteller-Namen (left_arm_front rechts, right_arm_front links) und zu `image_3 (1).png`.
- Lesart Betrachtersicht: "linke Klaue" = links im Bild. **Passt nicht**: Die Linie "linke Klaue" endet rechts (x 453), die Linie "rechte Klaue" links (x 282).
- Die Reihenfolge der Schilder (oben/unten am rechten Rand) sagt nichts ueber die Seite.

### 2.3 Krallenzahl

| Quelle | rechte Klaue (W) | linke Klaue (W) | Hinterfuesse | Kennung |
|---|---|---|---|---|
| Original Bild 2 | 1 deutliche Spitze und 1 nach oben, 2 weitere nur im Nutzer-Schnitt unter dem Logo; Zahl nicht entscheidbar | 2 Spitzen (+1 Flanke) | Bild 1: keine Kralle aufloesbar | [O] |
| Tripo VORNE | 5 Konturspitzen <= 60 Grad (4 lange nach unten, 1 seitlich) | 3 Konturspitzen (im Bild 3-4) | LINKS: 3 kleine Krallen an einem Fuss; HINTEN: keine Spitzen (Klotzfuesse) | [K] |

### 2.4 Korrektur "lange helle Dinge = Boden" [N], gemessen in Bild 1

| Nutzer-Teil | Pixel | hell/bodenfarbig (naeher an Boden (171,206,207) als an Figur (34,33,65)) | Lage | bleibt Fuss |
|---|---|---|---|---|
| fuss_links | 1029 | 280 (27 %): Querstreifen (195-217, 415-428), Fleck (196-205, 438-447), RGB um (120,152,177) | x 194-217, y 401-447 | 756 px, Y-Median 85 |
| fuss_rechts | 1237 | 255 (21 %): Laengsstreifen (291-316, 407-428), Streckung 2,3, 31 Grad, RGB (112,142,186) | x 293-316, y 409-428 | 982 px, Y-Median 56 |

- Damit entfallen die "tuerkisen Querbaender der Sohle" und die Pose "Sohle frontal zur Kamera" (Nachtrag zu E16). Die Fuesse werden kleiner und dunkler.
- Grenze der Messung: Derselbe Farbtest trifft auch die bekannten hellen Flecke auf dem Koerper (bein_rechts 11 %, part_of_Body 13 %). Die Farbe allein entscheidet also nicht; fuer die Fusszone entscheidet der Satz des Nutzers.

## 3. Koerperbau nach Tripo (nur Silhouette) [K]

Masken aus `image_0.png` (Hintergrund schwarz, Schwelle 30, Schrift entfernt).

| Ansicht | H x B px | B/H | Breite bei 5/10/20/30 % Hoehe (je / H) | Beinluecke von unten |
|---|---|---|---|---|
| VORNE | 451 x 488 | 1,08 | 0,22 / 0,33 / 0,58 / 0,70 | Oberkante bei 58 % |
| HINTEN | 425 x 462 | 1,09 | 0,25 / 0,35 / 0,55 / 0,65 | Oberkante bei 56 % |
| LINKS (Laenge) | 419 x 476 | 1,14 | 0,25 / 0,48 / 0,66 / 0,89 | - |
| RECHTS | - | - | **Spiegelbild von LINKS** (MAE 8,9 gegen 33,4 ungespiegelt), also keine eigene Information | - |
| Original Bild 2 (Nutzer-Teile) | 336 x 326 | 0,97 | 0,24 / 0,35 / 0,52 / 0,57 | Unterkante am Bildrand abgeschnitten |
| Original Bild 1 (maske_bild1 = 255) | 278 x 236 | **0,85** | 0,22 / 0,41 / 0,53 / 0,65 | Schwanz laeuft aus dem Bild |

- **Ruecken-Kuppel:** Tripo VORNE/HINTEN und das Original zeigen in beiden Bildern einen gerundeten Scheitel. Das Breitenprofil weicht bis 30 % Hoehe um hoechstens 0,06-0,13 ab. Die Kuppel ist im Original und in Tripo belegt.
- **Vier Beine:** Tripo HINTEN zeigt 4 Fuesse (je Seite 2, Bodenkontakt x 24-180 und 332-485) und den Schwanz in der Mitte. Im Original Bild 1 sind zwei Beine, ein Arm (Nutzer-Teile left_leg, right_leg, right_arm) und der Schwanz zu sehen, der vierte Arm ist verdeckt. "Auf allen vieren" ist eine Lesung (FORMBLATT2), der Vierbeiner ist Vorgabe [N].
- **Hoehe/Laenge/Breite:** Tripo H : L : B rund 1 : 1,14 : 1,08. Fuer die Laenge gibt es im Original keinen Beleg, weil eine Seitenansicht fehlt. Die Rueckansicht ist im Original schmaler (B/H 0,85 gegen Tripo 1,09): **Tripo spreizt die Beine weiter.**
- **Schwanz:** Bei Tripo haengt er aus der Rueckenmitte (Oberkante der Beinluecke bei 56 % der Hoehe) bis knapp ueber den Boden (bei y 420 noch 50 px breit, Fuesse bis 443). Im Original gibt es einen Schwanz [N] (E5 und Nutzer-Teil tail). Dort ist sein Scheitel bei y 184-187 aber der **oberste Punkt der Silhouette** in Bild 1 (Figur ab y 182-188): Der Schwanz steigt hoch. Der Tripo-Schwanzansatz in der Mitte widerspricht dem.
- **Von Tripo erfunden (kein Beleg im Original):** Facetten-/Kristallflaechen und Farben (Tripo-Licht), duenne dunkle Stangen an den Flanken (HINTEN, LINKS), diagonale Strebe unter dem Bauch (LINKS), Klotz-Hinterfuesse mit 3 Krallen, gespreizter Stand, Seitenproportionen, glimmende rote Augen, Maulform (laut Nutzer falsch).
- Kopf in Tripo VORNE: Augen (200,210) / (320,212), Abstand 120 = 0,25 der Breite, Augen bei 43 % der Hoehe (Original Bild 2: 40 %). Die Schnauzenspitze (238,389) sitzt unter der Augenmitte, Verhaeltnis 1,48 (Original 1,53). Die 2D-Anordnung entspricht also Bild 2.

## 4. Abgleich: was traegt das Original, was nur die KI

| Aussage | Original | nur KI | Nutzer |
|---|---|---|---|
| zwei Augen, waagerecht, 63 px | linkes deutlich (23,6), rechtes auf Rauschniveau (11,5) | KI macht beide klar (bis 86) | ja (Beschriftung) |
| Kopf frontal, Schnauze nach unten zur Kamera | ja (Augen, Spitze darunter) | - | ja (Punkte 1, 2, 6) |
| Oberkiefer = dunkler Keil, Spitze (246,371,5) | ja (Keilraender, Schnittpunkt 12 px daneben) | - | ja |
| Unterkiefer = helles Band mit 5 Flecken | ja (Prominenz 20-60) | Zahnformen, orange Punktreihe am Band | ja (E20) |
| tiefrote Zone aussen am Band | ja (Farbe) | - | nicht beschriftet |
| Maul offen zur linken Bildseite | Geometrie der Punkte | - | "nach rechts gerichtet" (Seite unklar, Frage 2) |
| Kuppel oben | ja (beide Bilder) | - | - |
| Schwanz | Saeule mit rundem Scheitel und rosa Ende (Bild 1) | Tripo: haengt aus der Mitte | ja (E5, tail) |
| Krallen spitz, wenige je Hand | ja (2 bzw. 1-4) | Tripo: 3-5 lange Kristallkrallen | ja (7, 8) |
| helle Laengsflecken an den Fuessen | vorhanden, bodenfarbig | KI zeigt sie deutlicher als Platten | = Boden |
| weisse Fleckgruppe links neben der Schnauze (161-221, 329-386) | vorhanden | KI: weisse runde Formen | keinem Teil zugeordnet |

## 5. Offene Fragen an den Nutzer

1. **Klauen-Seite bestaetigen:** Die Linien zeigen "linke Klaue" rechts im Bild und "rechte Klaue" links unten, also aus Sicht des Wesens wie bei den Augen. Stimmt das? (Aus Sicht des Betrachters waeren die Linien vertauscht.)
2. **"offenes Maul nach rechts gerichtet":** rechts aus Sicht des Wesens (Maul zur Frau, Kieferenden links unten im Bild; so liegen die Punkte 4/5/6) oder aus Sicht des Betrachters (die Maulspalte liegt im Bild rechts der Schnauze)? Davon haengt die Kopfdrehung ab.
3. **Rechtes Auge:** Ist der schwache Fleck (217,275) gemeint? Im Original ist er nicht vom Rauschen zu trennen; nur das linke Auge (280,275) ist klar.
4. **Tiefrote Zone rechts unter dem Band** (20-35 px breit): Ist das die Aussenseite des Unterkiefers oder Koerper/Licht dahinter? Davon haengt die Dicke des Unterkiefers ab.
5. **Weisse Fleckgruppe links neben der Schnauze** (161-221, 329-386): Gehoert sie zum Wesen (z.B. Zaehne der anderen Kieferseite) oder zur Frau bzw. zum Hintergrund? Bisher liegt sie in keinem Teil.

## 6. Belege

- Beleg-Tafel Kopf: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/leser_b/tafel_kopf.png` (A: Vorderseite_Beschreibung.png, B: Bild 2 Original; je mit Nutzer-Endpunkten, Augenflecken, Keilraendern, Bandachse, Zahnflecken, Maulspalt-Konstruktion und den Umrissen von Oberkiefer, Unterkiefer und K4).
- Beleg-Tafel Klauen/Boden/Tripo: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/leser_b/tafel_klauen_boden_tripo.png` (C: Bild 2 Klauen, D: Bild 1 Fusszone mit Bodenanteil, E: Tripo mit Spitzen).
- Messwerte: `leser_b/lines.json` (Linien), `punkte.json` (Endpunkte), `keil.json` (Keilzeilen), `tips.json` (Spitzen), `kuppel.json`, `tripo_image0.json`, `fuss_rest.json`, `k4_lage.json`; Skripte `leser_b/*.py`. Messstand der Nutzerdatei: `leser_b/vb_snapshot.png`.
