# Leser A: Formbeschreibung Riesenform (Kopf/Maul, Klauen, Koerperbau), 2026-11-05

Nur gelesen und gemessen, keine Datei unter Version_80%, pri/, synchro/, re15_port veraendert. Werkzeuge (Python/PIL/numpy/cv2) und Tafeln:
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/A/` (Skripte in `py/`, Messwerte als JSON daneben).

## 0. Grundlagen

| Punkt | Wert (gemessen) |
|---|---|
| Fassung von `Vorderseite_Beschreibung.png` | Stand 2026-11-05 11:17:07, SHA256 `f352bb85...ef83`. Die Fassung von 11:10 hatte die Linie "Spitze - Obere Vorderseite vom Maul" noch nicht; gelesen ist nur die neue. |
| Bildteil der Vorlage | x 0-1279 = `vlcsnap-2026-11-04-16h27m04s664.png` (mittlere Abweichung 4,3; abweichende Pixel = nur die roten Linien, Farbe (237,28,36)). Rechts x 1280-1598 weisse Beschriftung. |
| KI-Bild zu Original | ECC affin: Skala 2,0003 / 1,9999, Versatz +0,47 / -0,35 px, Korrelation 0,998. Umrechnung: Original = (KI - 0,5) / 2. Alle Koordinaten unten in **Bild 2 Original (640x480)**, wenn nicht anders gesagt. |
| Original-Video | `Biohazard 1.5 Final Build.mp4`: Bild 1 = 114,97 s, Bild 2 = 116,24 s. Beide sind Standbilder einer Diashow: Bild 2 steht 24 Bilder lang (116,14-116,90 s) mit mittlerer Abweichung <= 0,33. Keine Bewegung, also keine zusaetzliche Ansicht. Die Nachbarszenen (Schnitte ab 113,5 s bei Bild 7, 31, 55, 79, 103, 127, 151) zeigen die Riesenform nicht. |
| Kamera Bild 2 | aus der Disc (ROOM5090 Cut 11, `kamera.md`): blickt 13,1 Grad nach unten (Auge (7626,-3256,-25360), Ziel (-1504,-986,-21970)), fx 451, fy 478. |
| Kennung der Herkunft | **[O]** Original-Screenshot/Video, **[KI]** KI-Hochrechnung oder Tripo, **[N]** Vorgabe des Nutzers. Seiten: **(B)** aus Sicht des Betrachters, **(W)** aus Sicht des Wesens. |

### 0a. Zuordnung Linie -> Beschriftung (Tafel `beleg_linien_zuordnung.png`)

Jede Linie beginnt im Seitenfeld auf Hoehe ihrer Beschriftung. Die Linien "Ende, untere Vorderseite" (Seitenfeld y 524) und "linke Klaue" (y 602) laufen fast parallel (Steigung -0,364 / -0,374) und kreuzen sich nicht. Gekreuzt werden beide nur von "Spitze - Obere Vorderseite" (y 683, fast waagerecht).

| Beschriftung | Start im Seitenfeld (KI) | Ende im Bild (KI) | Ende Bild 2 |
|---|---|---|---|
| Ende, untere Vorderseite vom Maul | (1315,524) | **(558,799)** | **(279,399)** |
| linke Klaue | (1326,602) | **(905,759)** | **(452,379)** |

**Widerspruch zu NUTZER_ENTSCHEIDE E25/E27 und zur Auftragslesung:** Dort sind diese beiden Enden vertauscht. E25 nennt "linke Klaue ~x 560 / y 800"; E27 nennt "Spitze 'Ende, untere Vorderseite vom Maul' weit rechts unten" und folgert daraus "lange Schnauze nach rechts (von der Frau weg)". Gemessen liegt (558,799) am unteren linken Ende des Unterkiefer-Bandes. (905,759) liegt an der Aussenkante der linken Hand (W), rechts im Bild. Die Folgerung in E27 zur Schnauzenrichtung steht damit auf der vertauschten Koordinate (Abschnitt 1.3/1.4).

## 1. Kopf und Maul (Bild 2, frontal)

### 1.1 Linienenden des Nutzers (Vorlage) und was an der Stelle liegt

Linienende = Ende der roten Linie im Bild (Hough + Ausgleichsgerade ueber die Linienpixel, Ende als Mittel der Pixel im letzten 2,5-px-Abschnitt; Linie rund 4 KI-px breit, Ende +-1 px Original).

| Beschriftung [N] | Ende KI (1280er) | Ende Bild 2 | Lage zu den alten Nutzer-Teilen (E20) | Port-Kopf K4 hat dort (Tiefenpuffer, Pose Bild 2) |
|---|---|---|---|---|
| rechtes Auge (W) | (446,2 / 542,3) | (222,8 / 270,9) | 10,2 px ueber `Oberkiefer` (aussen) | Knochen 7 Wirbel |
| linkes Auge (W) | (577,0 / 538,3) | (288,2 / 268,9) | 31,8 px ueber `Oberkiefer` (aussen) | Knochen 10 Brust |
| Oberseite vom Maul | Knick (633,0 / 605,8), Strichende (624 / 662) | Knick (316,2 / 302,6), Strichende (311,8 / 330,8) | Strichende 4,2 px in `Oberkiefer`, 3,6 px neben `Unterkiefer` (auf der Grenze) | Knick: 16 Oberarm r; Strichende: 22 Kopf |
| offenes Maul nach rechts gerichtet | (659,2 / 652,3) | (329,4 / 325,9) | 5,6 px in `Unterkiefer` (dessen oberes rechtes Ende (B)) | 23 Kiefer |
| Spitze - Obere Vorderseite vom Maul | (491,2 / 743,0) | (245,4 / 371,2) | 2,0 px in `Oberkiefer`, untere linke Ecke (B) des Schnitts | 18 Hand r |
| Ende, untere Vorderseite vom Maul | (558,0 / 799,3) | (278,8 / 399,4) | 1,4 px in `Unterkiefer`, dessen unteres linkes Ende (B) | 18 Hand r |
| linke Klaue | (905,4 / 759,2) | (452,4 / 379,4) | 2,2 px neben `left_arm_front` (Aussenkante) | 28 Klaue l3 |
| rechte Klaue | (563,0 / 867,7) | (281,2 / 433,6) | 4,5 px neben `right_arm_front` | nichts (Hintergrund) |

### 1.2 Augen [O] mit Pruefung gegen Rauschen

| Punkt | Bild 2 Original | KI 16h27m04 | Bewertung |
|---|---|---|---|
| Lichtpunkt bei linkem Auge (W), Bild-rechts (B) | (280,275), Y 48, +30 ueber dem Median 15x15, RGB (82,36,52); 10,3 px vom Linienende | (279,275) +48, als Ring gezeichnet [KI] | deutlich: Rang 2 von 177 lokalen Maxima in der dunklen Masse (nur (289,174) am Buckel staerker) |
| Lichtpunkt bei rechtem Auge (W), Bild-links (B) | (217,275), Y 35, +12,5; 8,0 px vom Linienende | (218,274) +33 [KI verstaerkt] | schwach: Rang 10 von 177, gleich stark wie (258,263) mit +13,5. Im Original vom Rauschen nicht zu trennen; nur die KI hebt ihn hervor. |

- Beide Punkte liegen genau auf y 275, Abstand **63 px**, Augenlinie waagerecht (Linienenden: 65,4 px, -1,8 Grad).
- Farbe: dunkles Rotviolett, kein leuchtendes Rot wie bei Tripo [KI].
- Kopfrand: Die dunkle Masse grenzt Bild-links (B) gegen den hellen Kittel der Frau bei x 205-210 (y 240-300; Y 104-125 -> 17-23). In Augenhoehe (y 276) liegt die Kante bei **x 206**, 11 px links vom rechten Auge (W). Die andere Kopfseite (W links, B rechts) ist nicht messbar: dunkel auf dunkel (Y Median 19,5, IQR 17-24).

### 1.3 Kopfachse, Schnauze, Maulspalt (Bild-px; alles Projektion)

| Mass | Wert | Herleitung |
|---|---|---|
| Kopfmittellinie | Augenmitte (248,5 / 275) -> Spitze (245,4 / 371,2): 96,2 px, **1,9 Grad** gegen die Senkrechte | Spitze steht fast genau unter der Augenmitte |
| Blickrichtung des Kopfes | **frontal zur Kamera**, Schnauze nach unten (zum Boden und/oder zur Kamera; Aufteilung aus einem Bild nicht bestimmbar) | beide Augen sichtbar, Augenlinie waagerecht, Spitze mittig darunter. Kein Profil. |
| Schnauze (Augenlinie -> Spitze) / Augenabstand | 96 / 63 = **1,53** | gemessen; untere Grenze fuer das 3D-Verhaeltnis, wenn die Augenlinie bildparallel liegt (Verkuerzung kann nur verkleinern) |
| Schnauze / Kopfbreite | 96 / 85 = 1,13 | **abgeleitet**: Kopfbreite 85 px nur mit Annahme Symmetrie um x 248,5 (rechte Kante (W) x 206 gemessen, linke nicht) |
| Augenmitte -> Unterkiefer-Ende | (+30,2 / +124,4) | Unterkiefer-Ende liegt 30 px rechts (B) der Mittellinie |
| Oberkiefer (Verlauf) | von der Oberseite (311,8 / 330,8) bzw. (329,4 / 325,9) zur Spitze (245,4 / 371,2): 148,6-151,6 Grad, also nach links unten (B), 78-96 px. Unterkante des dunklen Oberkiefers = Grenze zum hellen Band: (258,374) -> (266,358) -> (299,347) -> (327,322) (Polygon des Schnitts) | dunkle Flaeche Y ~20 gegen helles Band (Spalte x 320-335, y 320-335: Y 23 -> 84-150) |
| Unterkiefer (Verlauf, Ende) | helles, rot geripptes Band (Schnitt `Unterkiefer`): von (342,320) nach (262,405), Achse 141,5 Grad, Schwerpunkt (293,6 / 364,0), 109 x 37 px (K.md). Vorderes Ende laut Nutzer (278,8 / 399,4), hinteres Ende am "offenen Maul" (329,4 / 325,9) | Rand des Bandes: Y 25 -> 68-155 (gemessen in Zeile 330/335) |
| Maulspalt im Bild | Scheitel "offenes Maul" (329,4/325,9): Richtung Spitze 151,6 Grad, Richtung Ende 124,5 Grad -> **27,1 Grad**; Scheitel Strichende (311,8/330,8): **32,9 Grad** | aus den Linienenden; 3D-Oeffnung nicht bestimmbar |
| Maul vorn offen | Spitze -> Ende: 43,7 px, 40 Grad unter der Waagerechten nach rechts (B) | Abstand der beiden "Vorderseiten" |
| Lage des offenen Mauls zur Mittellinie | Band-Schwerpunkt **+45 px rechts (B)**; Bandenden +14 / +94 px rechts | ganzes Band liegt rechts (B) der Kopfmittellinie = Seite links (W) |

**"Oberseite vom Maul" (Knickstrich):** 28,5 px lang, 9 Grad gegen die Senkrechte. Entlang des Strichs ist keine Kante: Y flach 22-25 (Zeilen 300-315, x 308-317). Erst das Strichende trifft die Oberkante des Bandes (Y 25 -> 68 bei (314,335)). Ob der Strich nur auf (311,8 / 330,8) zeigt oder eine Strecke markiert, ist im Bild nicht entscheidbar (Frage 3).

**"offenes Maul nach rechts gerichtet", beide Lesarten.** Gemessen ist eine einzige Lage: Mundwinkel oben rechts (B) bei (329,326), beide Kieferspitzen unten links (B) bei (245,371) und (279,399). Beide Lesarten beschreiben diese Lage, nur aus verschiedener Sicht:
- (W) rechts des Wesens = Bild-links, wie E24 fuer die ganze Datei festlegt. Vom markierten Punkt (329,326) laufen Ober- und Unterkiefer nach links unten (B), unter 151,6 und 124,5 Grad. Das Maul zeigt also zur rechten Seite des Wesens, auf die Seite der Frau (x 140-210), und nach unten. Das passt zur Messung.
- (B) Bild-rechts: Der helle Mauleinblick liegt ganz rechts (B) der Kopfmittellinie. Passt auch, meint aber den Ort des Einblicks, nicht die Richtung.
- **Nicht** vereinbar mit der Messung ist E27 "lange Schnauze nach rechts (von der Frau weg), Spitze weit rechts unten". Die Spitzen liegen links unten (B), 35-105 px rechts der Frau und links vom Mundwinkel. E27 stuetzt sich auf die vertauschte Koordinate aus Abschnitt 0a.
- Die rechte Kieferhaelfte (W) waere bei symmetrischem Kopf im Spiegelbereich x 154-254 zu suchen. Dort liegen 1040 von 1653 gesaettigt weissen px. Sie haben dieselbe Farbe wie das Feuer- und Bodenlicht: (215,207,204) gegen (212,207,204) rechts und (212,211,211) am Boden. Der Unterkiefer ist dagegen rosa, (218,150,187) an seiner hellsten Stelle. Dort ist keine Kieferform lesbar. Die Kopfachse ist nur 1,9 Grad geneigt; als Dreh nach Bild-links zeigt sich der Kopf in den Augen nicht.

### 1.4 Vereinbarkeit mit der frueheren Lesung (E20-Schnitte, K.md, Kopf K4)

| Fruehere Aussage | Neuer Befund | Urteil |
|---|---|---|
| `Unterkiefer.png` = Unterkiefer mit Zahnreihe (E20; K.md: 6 helle Flecken, Abstand 16 px) | Unterkiefer-Ende und "offenes Maul" liegen in diesem Schnitt an seinen beiden Enden | **vereinbar** |
| `Oberkiefer.png` = Oberkiefer; "links breit = Schnauze vorn, rechts schmal = hinten zum Hals" (K.md 3) | Spitze = untere linke Ecke des Schnitts (passt). Die **Augen liegen 10-32 px UEBER dem Schnitt**: Der Kopf reicht nach oben ueber den Schnitt hinaus. Der Hinterkopf liegt oben, nicht rechts beim Hals (342,280). | **Widerspruch** in der Kopfachse: senkrecht (Augen oben, Spitze unten) statt waagerecht (Spitze links, Schaedel rechts) |
| Maulwinkel 50-56 Grad im Bild (K.md 3: Oberkante des Schnitts gegen Unterkieferachse) | Die Oberkante des Schnitts ist nach den Augen nicht die Schnauzenoberseite. Aus den Nutzerpunkten folgen 27-33 Grad. | **ueberholt** |
| K4 (STUFE3 NB2-K2): Schnauzenspitze, Kiefer, Gelenk | K4-Spitze (228,5 / 354,9) 23,5 px neben "Spitze"; K4-Kieferspitze (253,7 / 404,4) 25,5 px neben "Ende"; K4-Kiefergelenk (316,0 / 326,4) 6,1 px neben dem Strichende | Kieferlage **ungefaehr vereinbar** |
| K4: Augen und Kopfrichtung | K4-Augenhoecker (300,0 / 263,3) und (283,4 / 260,9): nur **16,8 px** auseinander und in Pose Bild 2 **beide verdeckt** von Knochen 10 (Brust; Tiefe 5158/5488 gegen Puffer 4822/4759). K4-Mittellinie 34,3 Grad nach links geneigt, also 3/4-Profil. Original: 63 px, frontal, 1,9 Grad. | **Widerspruch** |
| J/K4 Koerperpose Bild 2 (FORMBLATT2: Huefte links hinten, Brust rechts) | An den Augenpunkten zeigt K4 Huefte (0, bei (217,275)), Brust (10, bei (280,275)) und Wirbel (7, rechtes Linienende); an Spitze und Unterkiefer-Ende die rechte Hand (18) | **Widerspruch**: der neue Kopf sitzt dort, wo J/K4 Rumpf und Hand haben. Das erklaert den gemessenen "Zielkonflikt" in STUFE3 NB2-K1 (Hals endet bei (342,280), Masse 85-125 px links unten) |
| E27 (2026-11-05): "lange Schnauze vom Wesen aus nach rechts (von der Frau weg), Spitze 'Ende, untere Vorderseite' weit rechts unten" | "Ende" liegt bei (279,399), nicht (452,379). Beide Spitzen liegen links unten (B), vom Mundwinkel (329,326) aus zur rechten Seite des Wesens, also zur Frau hin. | **Widerspruch** (Koordinatentausch, Abschnitt 0a) |
| "kurze" Krokodilschnauze K4 | K4 in 3D: Augenhoecker -> Spitze 1155 Einheiten, Augenhoecker-Abstand 397, Verhaeltnis 2,9. Original im Bild: 1,53 (Projektion). Gleich nur, wenn die Schnauze rund 58 Grad zur Kamera zeigt (cos = 1,53 / 2,9). | aus einem Bild **nicht entscheidbar** (Frage 5) |

**Zur Frage des Nutzers "Erkennst du jetzt das alligatorartige Maul korrekt?":** Nach den Linien [N] und den Messungen [O] ist der Kopf ein frontal zur Kamera gerichteter Kopf. Oben sitzen zwei Augen, 63 px auseinander, auf y 275. Die Schnauze zeigt nach unten, ihre Spitze liegt bei (245,371) senkrecht unter der Augenmitte. Das Maul steht offen. Der helle Unterkiefer mit Zahnflecken laeuft vom vorderen Ende (279,399) nach rechts oben (B) bis zum Mundwinkel bei rund (312-329, 326-331). Das ist eine andere Kopfrichtung als in K4 (3/4-Profil mit verdeckten Augen).

## 2. Klauen

| Beschriftung [N] | Linienende | Bildbefund [O] | Lesart (W) | Lesart (B) |
|---|---|---|---|---|
| linke Klaue | (452,4 / 379,4), Bild-rechts | Aussenkante der Hand: Zeile 379 dunkel -> hell zwischen x 450 (Y 112) und 456 (Y 190). Die Spitzen der dunklen Flaeche (Y < 100, gegen hellen Boden) liegen tiefer: (406,391), (435,407), Ende (465,412). Spitzen des Nutzer-Schnitts (468,417), (439,418) sind 40,7 / 40,9 px vom Linienende entfernt. | linke Klaue (W) = Bild-rechts: **passt** zur Linie | linke Klaue (B) waere die Klaue Bild-links; die Linie zeigt aber nach Bild-rechts: Beschriftung **vertauscht** |
| rechte Klaue | (281,2 / 433,6), Bild-links | 4,5 px neben `right_arm_front`; naechste Spitze des Schnitts (274,443) 11,9 px. Zwei weitere Spitzen des Schnitts (249,476), (197,476) liegen unter dem Logo. | **passt** | **vertauscht** |

Lage der Beschriftungen im Seitenfeld: "linke Klaue" oben (y 611), "rechte Klaue" unten (y 885). Die zugehoerigen Linienenden liegen in derselben Reihenfolge (379 ueber 434). Die Lesart (W) stimmt mit der Augenbeschriftung ueberein. Sie stimmt auch mit `image_3 (1).png` ueberein, dort steht "rechte Klaue" Bild-links und "linke Klaue" Bild-rechts.
**Entschieden durch E25 (Nutzer: "aus Sicht des Wesens"):** Die Linien passen ohne Tausch. Zu beachten: Die Lage, die E25 fuer "linke Klaue" nennt (~x 560 / y 800), ist das Ende der Linie "Ende, untere Vorderseite vom Maul". Die Linie "linke Klaue" endet bei KI (905,759) = Bild 2 (452,379) (Abschnitt 0a).
Zu E26 "Fuesse enden in kraeftigen Krallen wie in den Tripo-Ansichten": Die Hinterfuesse zeigen bei Tripo [KI] 3 kleine Zehen. Im Original [O] sind in Bild 1 an den Fussschnitten nur dunkle Reste lesbar, Krallen nicht (Abschnitt unten). Diese Form der Hinterfuesse hat nur KI-Beleg.

**Zahl der Krallen**

| Quelle | Klaue rechts (W) = Bild-links | Klaue links (W) = Bild-rechts |
|---|---|---|
| Original [O], Nutzer-Schnitte (K.md 2, Spitzen >= 12 px) | 4 Spitzen + Wurzelende, davon 2 unter dem Logo -> im Bild frei messbar 2: (274,443), (229,351) | 2: (468,417), (439,418) |
| Original [O], eigene Messung (dunkle Flaeche Y < 100, nach unten zeigend, Prominenz >= 8 px) | vor hellem Grund nicht trennbar (Logo, dunkler Boden) | 2 + Flaechenende: (406,391), (435,407), (465,412) |
| Tripo `image_0` VORNE [KI] (Ecken < 80 Grad im Umriss) | 5 Spitzen | 3 Spitzen |
| Tripo LINKS/RECHTS [KI] | vordere Klaue 2 grosse Spitzen, zweiter Fuss mit 3 kleinen Zehen | gleich |

Die wirkliche Krallenzahl je Fuss ist im Original nicht bestimmbar: 2 Spitzen liegen unter dem Logo, die Unschaerfe betraegt rund 2 px, rechts ist der Grund dunkel. Belegt sind 2-3 sichtbare Spitzen je Hand. Die 5 gespreizten Krallen von Tripo haben keinen Beleg.

**Korrektur "helle lange Dinge = Boden" [N]** (Bild 1, Nutzer-Teile E16; Tafel `beleg_bild1_boden.png`):

| Bereich | Messung | Folge |
|---|---|---|
| `fuss_rechts` (286,403 / 40x51, 1237 px) | hellblaues Band x 291-316, y 407-429: 338 px (27 %), RGB (102,135,177), Achse 35 Grad, rund 31 x 15 px; liegt ganz im Schnitt | **kein Fuss mehr**. Es bleiben 899 dunkle px (x 288-325, y 403-452) |
| `fuss_links` (192,395 / 28x53, 1029 px) | 383 px (37 %) hell (Teil der grossen Bodenflaeche x 185-228, y 340-431, RGB (167,179,194)); das Band y 412-429 laeuft ueber den Schnittrand in den Boden weiter (197 tuerkise px bis 12 px links daneben); 334 px unter dem Logo | **kein Fuss mehr**. Dunkel bleiben nur 312 px (x 195-218, y 398-431) |
| E16 "tuerkise Querbaender = Sohlenstruktur", Nachtrag "Sohlen frontal sichtbar" | Grundlage entfaellt | Fuss = nur der dunkle Rest. Die Bandfolge lavendel/tuerkis/dunkel in FORMBLATT C5 (x 288-330, y 405-452, "offen") ist Boden. |
| Helle Flecken in Beinhoehe: `bein_links` (232-247 / 341-351, 81 px), `bein_rechts` oben (rund 315-335 / 285-310) | liegen weit ueber Fusshoehe | vom Nutzersatz nicht erfasst, offen (Frage 7) |
| Bild 2 | Die Klauenschnitte enthalten ausser den Logo-Buchstaben nichts Helles: 785 px mit Y > 170, davon 779 im Logo. Die weissen Flecken links neben der Schnauze (162-226 / 329-385) liegen in keinem Nutzer-Teil. In `16h26m29` [KI] sind sie als lange helle Finger gezeichnet (Augenschein). | dort keine Aenderung |

## 3. Koerperbau nach Tripo (nur Umriss und Proportion, [KI])

Umriss aus `image_0.png`, je Viertel Schwelle max(RGB) > 28, groesste Flaeche; Tafel `tripo_mass.png`.

| Ansicht | Breite x Hoehe px | B/H | Befund |
|---|---|---|---|
| VORNE | 488 x 451 | 1,08 | Kuppel ueber dem Kopf; Augen (200,210)/(320,212), Abstand 120 = 0,25 der Breite, Augenhoehe 0,42 der Hoehe von oben; Kopf mittig (Augenmitte bei 0,51 der Breite); zwei Vordergliedmassen mit grossen Klauen |
| HINTEN | 462 x 425 | 1,09 | drei Bodenkontakte: linker Hinterfuss, Schwanzspitze mittig (x 750-811), rechter Hinterfuss; Schwanz haengt unter der Kuppel mittig bis zum Boden |
| LINKS / RECHTS | 476 x 419 / 471 x 418 | 1,14 / 1,13 | vier Gliedmassen (vorn lange Arme mit grossen Klauen, hinten Blockbeine); Kopf vorn tief; dunkle Gurte oder Stangen am Rumpf |

Abgleich der Proportionen mit dem Original [O]:

| Merkmal | Tripo [KI] | Original [O] | Urteil |
|---|---|---|---|
| Augenhoehe im Umriss von oben (Bild 2 frontal) | 0,42 | 0,396 (Vereinigung der Nutzer-Teile Bild 2: 326 x 336, Augen y 275) | aehnlich |
| Lage des Kopfes in der Breite | 0,51 (mittig, symmetrisch) | 0,32 (Augenmitte links der Figurmitte) | **Tripo hat symmetrisiert** |
| Augenabstand / Breite | 0,25 | 0,19 | abweichend |
| Hoechster Punkt in Rueckansicht | Kuppelspitze, Schwanz haengt darunter | Nutzer-Teil `tail` reicht bis y 184 und liegt damit ueber `part_of_Body` (204); der Schwanz steigt ueber den Ruecken und faellt zur Kamera bis y 478 | **Widerspruch** zur Schwanzlage |
| Schwanz ja/nein | ja | ja (`tail`, E5 [N]) | belegt |
| Vier Beine | ja | Nutzer [N] vier; FORMBLATT: drei sichtbar, das vierte verdeckt | belegt durch den Nutzer, im Bild 3 sichtbar |
| Maul | Band kreuzt die Mittellinie, Schwerpunkt **17 px links (B)** (`image_0`) bzw. 43 px links (`image_3 (1)`), unteres Ende links | Band ganz **rechts (B)**, Schwerpunkt +45 px | **Tripo falsch**, wie der Nutzer sagt; Tafel `beleg_tripo_gegen_original_maul.png` |
| Augen | leuchtend rot | dunkles Rotviolett, Y 48 / 35 | Rot ist Tripo-Licht [KI] |
| Gurte/Stangen | vorhanden | Bild 1, x 190-340, y 250-470: 0 gerade Kanten >= 30 px (Canny 20/60, Hough) | **erfunden** |
| Klauen | 3-5 lange gespreizte Spitzen | 2-3 sichtbare Spitzen je Hand | Zahl und Spreizung **erfunden** |
| Fuesse | Blockfuesse | dunkler Rest der Fussschnitte (Abschnitt 2) | Form nicht belegt |

## 4. Was stuetzt das Original, was nur die KI?

| Aussage | [O] Original | nur [KI] | [N] Nutzer |
|---|---|---|---|
| Zwei Augen auf y 275, 63 px auseinander | linkes Auge (W) deutlich (+30); rechtes (W) nur auf Rauschniveau (+12,5) | `16h27m04` verstaerkt beide, zeichnet einen Augenring | beide beschriftet |
| Kopf frontal, Schnauze nach unten, Spitze unter der Augenmitte | Spitze (245,371) ist die Ecke des dunklen Oberkiefer-Schnitts; Kopfmitte 1,9 Grad | - | Linie "Spitze" |
| Offenes Maul: Winkel oben rechts (B), Spitzen unten links (B) = Maul zur rechten Seite des Wesens | helles geripptes Band (Y bis 155) zwischen (329,326) und (279,399); dunkler Oberkiefer bis (245,371) | Tripo: Band kreuzt die Mitte nach links (anders gelegen) | "nach rechts gerichtet" (W nach E24) |
| Schnauze nach rechts im Bild, von der Frau weg (E27) | **nein**: keine Spitze rechts vom Mundwinkel | - | E27 = Lesung des Orchestrators auf vertauschter Koordinate |
| Unterkiefer-Ende (279,399) | Ende des hellen Bandes | - | Linie "Ende" |
| Oberseite vom Maul als Strecke | keine Kante entlang des Strichs | - | Knickstrich |
| Krallen 2-3 je Hand sichtbar | ja | Tripo 3-5, `16h26m29` schwarze Dornen (Augenschein) | Klauen beschriftet |
| Helle Baender in den Fussschnitten = Boden | laufen in den Boden weiter (fuss_links), gleiche Farbe wie der Boden | - | Nutzersatz |
| Kuppelruecken, vier Beine, Schwanz | Bild 1: Schwanz hoechster Punkt, drei Beine sichtbar | Tripo: Kuppel hoechster Punkt, Schwanz haengt | Schwanz, vier Beine |
| Rueckansicht Bild 1 zeigt den Kopf | nein (FORMBLATT: "Kopf nicht erkennbar") | `16h26m21`/`16h26m59`: kein Kopf erkennbar (Augenschein) | - |

## 5. Offene Fragen an den Nutzer (nur Unentscheidbares)

1. "offenes Maul nach rechts gerichtet" (nach E24 aus Sicht des Wesens): Heisst das, das Maul zeigt vom Mundwinkel rechts im Bild (329,326) nach links unten zur Spitze (245,371), zur Seite der Frau? So liegt es gemessen. Oder meinst du eine Schnauze, die nach rechts im Bild von der Frau weg zeigt (so liest es E27)? Dafuer zeigt das Bild keine Spitze rechts.
2. (entfallen: Klauen aus Sicht des Wesens, E25; Linien passen ohne Tausch)
3. "Oberseite vom Maul" (Knickstrich): Zeigt die Linie nur auf das Strichende (312,331), also die Oberkante des hellen Bandes? Oder markiert der kurze senkrechte Strich (316,303)-(312,331) eine Strecke (welche Kante)?
4. Rechtes Auge (Wesen): Im Original ist dort nur ein sehr schwacher Lichtpunkt (217,275), gleich stark wie andere Flecken. Ist die Lage aus dem KI-Bild uebernommen, oder siehst du es auch im Original?
5. Schnauzenlaenge: Der Kopf blickt frontal, die Schnauze zeigt nach unten zur Kamera. Im Bild ist sie 1,5 Augenabstaende lang. Soll sie im Modell so kurz bleiben, also senkrecht nach unten? Oder lang wie bei einem Krokodil und stark zur Kamera geneigt (dann rund 2,9 Augenabstaende wie K4)? Das Bild allein entscheidet das nicht.
6. Kopfhaltung und Koerper in Bild 2: Der Kopf liegt mit den Augen dort, wo das Port-Modell Huefte und Brust hat. Darf die Koerperpose in Bild 2 (Wagen 5090) dafuer geaendert werden, so dass das Wesen der Kamera und der Frau zugewandt steht?
7. Gilt "helle Dinge = Boden" auch fuer die hellen Flecken in Beinhoehe (`bein_links` 232-247/341-351, `bein_rechts` 315-335/285-310)? Oder nur fuer die Fussschnitte?

## 6. Beleg-Tafeln (scratchpad `.../leser/A/`)

| Datei | Inhalt |
|---|---|
| `beleg_linien_zuordnung.png` | jede rote Linie farbig vom Seitenfeld bis zum Ende, mit Beschriftung und Koordinate (Beleg fuer Abschnitt 0a) |
| `beleg_vorderseite.png` | Vorlage 1599x960 mit gemessenen Linienenden (gruen), Augenpunkten (cyan), Kopfmittellinie (gelb), Maulkeil (magenta), alten Schnitten Oberkiefer/Unterkiefer (orange/rosa), K4-Punkten (blau x) |
| `beleg_vorderseite_kopf_zoom.png` | dasselbe, Kopfbereich 2x |
| `beleg_bild2_original.png` | Bild 2 Original 2x mit denselben Messlinien |
| `beleg_bild2_kopf_x4.png` | Bild 2 Original, Kopf 4x (x 190-480, y 240-450) |
| `beleg_tripo_gegen_original_maul.png` | Tripo VORNE gegen Bild 2: Augenlinie, Mittellinie, Maulband-Schwerpunkt (17 px links gegen 45 px rechts) |
| `beleg_bild1_boden.png` | Bild 1 Fussbereich 4x: helle Bodenanteile in fuss_links/fuss_rechts rot |
| `vergleich_ki_orig_kopf.png` | Original / KI 16h27m04 / KI 16h26m29 nebeneinander mit den Punkten |
| `kopf_orig_gamma.png` | Original Kopfbereich, Gamma 0,5, 4x, mit Raster |
| `tripo_mass.png`, `tripo_spitzen.png`, `tripo_klauen_zoom.png` | Tripo-Umrisse, Bodenkontakte, Krallenspitzen |
| `py/*.json` | Messwerte: `punkte_orig.json`, `punkte_teile.json`, `k4_proj.json`, `tripo_image0.json`, `hellstreifen_bild1.json`, `spitzen_tripo.json` |
