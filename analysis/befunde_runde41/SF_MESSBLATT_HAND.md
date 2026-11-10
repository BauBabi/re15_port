# SF Messblatt Hand - linke Hand (Mesh 12) und linker Unterarm (Mesh 11), V8 Runde 6a

Stand 2026-11-10, vor jedem Bau. Modell = Spiel-Asset `EM30_HANGAR.EMD` sha256 c4976c62 (HEAD 5820af00f).
Werkzeug: `re15_port/tools/r41_sichel/v8/hand_mass.py` (Zahlen unten = dessen Ausgabe), Bild: `SF_belege/v8/16a_hand_messblatt.png`.
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; jede Zahl hier ist Bildmessung, keine `@0x`-Konstante.

Nutzer 2026-11-10 (woertlich): "Irgendwie sieht die linke Hand vom 2 Beiner Birkin Modell immer noch komisch aus, im Vergleich zum
Original Material. Ich weiß nicht genau was es ist... Sind die Krallen zu dünn? Überlappen sie? Irgendwas passt nicht...." und
"Bei drei_posen.png was du gepostet hast, sieht man es ganz eindeutig. Die Vorlage hat eine breite Hand mit breiten Unterarmt. Dadurch
wirkt es natürlich. Bei uns ist die Hand ganz klein und zierlich und wirkt dadurch verkümmert."

## 1. Was gemessen wurde

- **Vorlage:** Nutzerbilder `Version_80%/birkin20..24.png` = Freisteller der Videobilder f1476, f1481, f1477, f1479, f1483 (doppelte
  Groesse, Alpha-Maske; nur gelesen). Dort steht der (W)-linke Arm Bild-rechts frei vor dem Hintergrund. Fuenf Bilder je Mass,
  Streuung = kleinster bis groesster Wert. Alle Pixel = Videopixel (640 x 480), dieselbe Kamera (50A0 Cut 12) und derselbe Standort wie
  Pose P1 (Foto 1 = f1486). Gegenprobe an f1486 selbst (SF_MESSBLATT_FOTO.md Abschnitt 6): rote Hand x 482-511 = 29 px breit, hier
  Fingerblock 23,1-31,6 px.
- **P2 und PV (f1434-f1443, birkin11-13, Beschreibung2):** der Rumpf verdeckt den (W)-linken Arm vollstaendig. Dort gibt es **kein
  Vorlagenmass**; die Tabelle nennt fuer P2/PV nur das Modell. (In P2/PV ragt die Modellhand hinter dem Rumpf hervor, weil der Ellbogen in
  den Clips gebeugt ist - Clips sind nicht Auftrag.)
- **Verfahren:** Arm-Maske ab Unterkante der Naht (gepunktetes Band am Ellbogen), 4fach vergroessert um die Armachse gedreht, zeilenweise
  Breite quer zur Achse. E = Mitte der Naht-Unterkante, T = fernster Punkt der Maske (Spitze des Fingerblocks), W = Engstelle (kleinste
  Breite zwischen 25 % und 65 % der Strecke E-T). Modell: Knochen 11 + 12 frei gezeichnet (f_rast, RDT-Licht), E/W = projizierte
  Gelenkpunkte, Station Handgelenk nach derselben Engstellen-Regel, Hand-Stationen an der Maske von Knochen 12.
- **Stationen:** S1 Ellbogen (3 px unter der Naht / dem Gelenk), S2 Unterarm-Mitte, S3 Handgelenk (Engstelle), S4 Handwurzel (20 % der
  Handlaenge), S5 breiteste Stelle der Hand (mit Daumen), S6 Krallenwurzeln (Vorlage: Beginn der roten Zone, Farbe nur als Ort; Modell:
  Ringmitten der Fingerkrallen A/B/C), S7 Krallen-Mitte, S8 Spitzen (2 px vor dem Ende).

## 2. Breitenprofil: Vorlage | Modell | Faktor

Spalten: Einzelwerte birkin20-24 | Median (Streuung) in px | Verhaeltnis zur Ellbogenbreite S1 im selben Bild, Median (Streuung) |
Anteil an der Figurhoehe | Modell P1 / P2 / PV in px | Modell im Verhaeltnis zu seiner S1 | Faktor = Vorlage-Median / Modell P1.

| Mass | Vorlage einzeln (px) | Vorlage px | Vorlage / S1 | / H | Modell px P1 / P2 / PV | Modell / S1 | Faktor |
|---|---|---|---|---|---|---|---|
| S1 Ellbogen | 34,0 36,1 34,1 34,8 40,8 | 34,8 (34,0-40,8) | 1,00 | 0,114 | 33,5 / 22,2 / 24,2 | 1,00 | 1,04 |
| S2 Unterarm-Mitte | 32,5 34,5 33,5 33,1 39,1 | 33,5 (32,5-39,1) | 0,96 (0,95-0,98) | 0,110 | 36,5 / 21,2 / 22,5 | 1,09 / 0,95 / 0,93 | 0,92 |
| **S3 Handgelenk** | 20,5 21,1 20,0 21,5 25,5 | **21,1 (20,0-25,5)** | **0,60 (0,58-0,62)** | 0,068 | **12,2 / 8,8 / 11,0** | 0,36 / 0,40 / 0,45 | **1,73** |
| **S4 Handwurzel** | 26,6 34,5 26,8 31,4 34,8 | **31,4 (26,6-34,8)** | 0,85 (0,78-0,96) | 0,098 | **13,5 / 12,5 / 14,2** | 0,40 / 0,56 / 0,59 | **2,33** |
| **S5 Hand breiteste Stelle** | 37,8 46,6 38,0 44,5 50,6 | **44,5 (37,8-50,6)** | **1,24 (1,11-1,29)** | 0,139 | **14,5 / 8,5 / 16,8** | 0,43 / 0,38 / 0,69 | **3,07** |
| S6 Krallenwurzeln | 37,8 46,1 37,9 43,5 50,6 | 43,5 (37,8-50,6) | 1,24 (1,11-1,28) | 0,136 | 12,5 / 5,8 / 7,5 | 0,37 / 0,26 / 0,31 | 3,48 |
| S7 Krallen-Mitte | 19,6 23,9 18,2 21,8 29,1 | 21,8 (18,2-29,1) | 0,63 (0,53-0,71) | 0,068 | 8,0 / 4,0 / 4,8 | 0,24 / 0,18 / 0,20 | 2,73 |
| S8 Spitzen | 9,0 6,2 6,9 5,1 8,5 | 6,9 (5,1-9,0) | 0,20 (0,15-0,26) | 0,023 | 5,2 / 2,2 / 0,9 | 0,16 / 0,10 / 0,04 | 1,33 |
| Unterarm groesste Breite | 37,0 36,6 35,6 37,2 41,8 | 37,0 (35,6-41,8) | 1,04 (1,01-1,09) | 0,121 | 43,5 / 30,2 / 32,5 | 1,30 / 1,36 / 1,34 | 0,85 |
| Unterarm-Laenge E-W | 43,2 49,8 42,4 46,5 44,7 | 44,7 (42,4-49,8) | 1,27 (1,10-1,38) | 0,144 | 37,7 / 38,2 / 34,8 | 1,13 / 1,72 / 1,44 | 1,19 |
| **Hand-Laenge W-Spitze** | 56,9 59,5 58,0 59,8 68,5 | **59,5 (56,9-68,5)** | **1,68 (1,65-1,72)** | 0,191 | **43,2 / 34,8 / 34,5** | 1,29 / 1,57 / 1,43 | **1,38** |
| Hand ohne Krallenzone (W bis S6) | 31,8 30,5 33,5 31,8 37,9 | 31,8 (30,5-37,9) | 0,93 (0,84-0,98) | 0,106 | 27,6 / 22,4 / 21,2 | 0,82 / 1,01 / 0,88 | 1,15 |
| Krallenzone (S6 bis Spitze) | 25,1 29,0 24,5 28,0 30,6 | 28,0 (24,5-30,6) | 0,75 (0,72-0,80) | 0,088 | 15,6 / 12,4 / 13,3 | 0,47 / 0,56 / 0,55 | 1,79 |
| Handbreite S5 / Handlaenge | 0,66 0,78 0,66 0,74 0,74 | 0,74 (0,66-0,78) | - | - | 0,34 / 0,24 / 0,49 | - | 2,2 |
| Spitze ueber der Sohle | 90,5 82,0 91,5 101,5 79,5 | 90,5 (79,5-101,5) | - | 0,301 | 73,0 / 110,0 / 136,0 | - (0,253 H in P1) | 1,24 |
| Handgelenk ueber der Sohle | 146,8 140,3 148,7 160,3 147,5 | 147,5 (140,3-160,3) | - | 0,490 | 110,7 / 142,9 / 159,6 | - (0,383 H in P1) | 1,33 |
| Ellbogen ueber der Sohle | 190,0 190,0 191,0 206,5 192,0 | 191,0 (190,0-206,5) | - | 0,629 | 147,9 / 167,8 / 165,2 | - (0,512 H in P1) | 1,29 |

Figurhoehe: Vorlage 299,5 / 303,5 / 303,5 / 319,0 / 310,0 px, Modell P1 289, P2 258, PV 254 px.

Profil in Zehnteln (Vorlage-Median px | Modell P1 px):
- Unterarm 10-100 % der Strecke E-W: Vorlage 35,6 36,6 36,6 35,6 33,5 30,0 27,0 24,1 22,5 22,0 | Modell 34,5 39,5 42,5 40,5 36,5 33,2 30,5 27,5 24,2 **15,0**
- Hand 0-90 % der Strecke W-Spitze: Vorlage 21,8 25,8 31,4 36,4 40,5 43,5 44,5 31,1 20,2 12,6 | Modell 25,8* 20,5 13,5 13,5 13,5 14,8 13,5 12,2 7,8 5,8
  (* am Gelenk schneidet die schraeg stehende Hand-Achse das Unterarm-Ende; Engstelle des Modells 12,2 px bei S3.)

**Lesart:** Der Unterarm der Vorlage ist die ersten 40 % ein gleich dicker Zylinder (1,03 x S1) und laeuft dann gerade auf 0,60 x S1 am
Handgelenk zu; direkt danach wird die Hand WIEDER breit (0,85 x S1 nach einem Fuenftel, 1,24 x S1 an der breitesten Stelle mit Daumen).
Das Modell hat am Ellbogen dieselbe Breite (33,5 gegen 34,8 px), eine Wulst im ersten Drittel (43,5 gegen 37,0 px), und ab dem Handgelenk
bleibt es bei 12-15 px bis zu den Spitzen: es gibt keine Hand, die breiter waere als das Handgelenk. Die Hand ist 1,38-mal zu kurz und
3,1-mal zu schmal; das Verhaeltnis Breite / Laenge ist 0,34 statt 0,74.

## 3. Daumen, Finger, Krallen

Vorlage (fuenf Bilder, px in Kamera P1):

| Mass | einzeln | Median (Streuung) | / S1 |
|---|---|---|---|
| Daumengabel ab Handgelenk | 24,5 22,4 24,4 23,1 30,5 | 24,4 (22,4-30,5) | 0,72 |
| Daumen Laenge ab Gabel | 16,9 17,7 18,4 19,4 17,1 | 17,7 (16,9-19,4) | 0,50 |
| Daumen Dicke an der Gabel | 12,0 10,8 10,0 13,1 16,1 | 12,0 (10,0-16,1) | 0,35 |
| Daumen Winkel zur Handachse | -23,7 -24,0 -26,0 -22,9 -22,1 Grad | -23,7 (zum Koerper = Bild-links) | - |
| Fingerblock Laenge ab Gabel | 32,4 37,1 33,6 36,6 38,0 | 36,6 (32,4-38,0) | 0,99 |
| Fingerblock Breite an der Gabel | 23,6 28,9 23,1 27,4 31,6 | 27,4 (23,1-31,6) | 0,77 |
| Abstand Daumen - Fingerblock (Mitten) an der Wurzel / an der Daumenspitze | 18,8-25,4 / 23,2-30,8 | 20,8 / 28,9 | 0,60 / 0,77 |
| freier Spalt an der Daumenspitze | 12,2 16,9 15,0 15,4 14,6 | 15,0 (12,2-16,9) | 0,44 |

- **Sichtbare Seite:** die Kamera sieht den HANDRUECKEN (grau-violett, Adern), der Daumen liegt koerperseitig (Bild-links), der Fingerblock
  aussen. Die Hand haengt fast senkrecht (Achse W-Spitze -5 bis -10 Grad gegen die Senkrechte, Unterarm +1 bis +6 Grad): Hand und Unterarm
  bilden eine fast gerade Linie.
- **Krallen in der Vorlage:** einzeln aufloesbar sind nur der Daumen (hell-grauer Keil am Daumenende) und EIN heller Krallenkeil mitten im
  roten Fingerblock (in allen fuenf Bildern, etwa 10-12 px lang, 2-3 px dick, entlang der Handachse nach unten). Die uebrigen Fingerkrallen
  verschmelzen zum roten Keil (Fingerblock, an der Wurzel 23-32 px breit, an der Spitze 5-9 px). **Nichts kreuzt sich**; alle Finger zeigen
  entlang der Handachse nach unten, der Daumen 22-26 Grad davon weg zum Koerper. Der Fingerblock wird von 27 px (Gabel) auf 7 px (Spitze)
  schmaler: die Krallen laufen ZUSAMMEN, nicht auseinander.
- **Wo die Spitzen enden:** 0,30 H ueber der Sohle (79,5-101,5 px) = Unterkante der Hose / knapp ueber dem Knie; Handgelenk auf 0,49 H
  (Schritt), Naht/Ellbogen auf 0,63 H.

Modell heute (px, Wurzel = Ringmitte, Laenge Wurzel-Spitze, Dicke = groesste Ringweite, Winkel zur Handachse):

| Kralle | P1 Laenge / Dicke / Winkel | P2 | PV |
|---|---|---|---|
| T Daumen (Zeile 491) | 12,1 / 5,8 / -15,5 | 10,5 / 3,1 / -2,3 | 9,8 / 3,2 / 1,3 |
| A (492) | 18,1 / 11,2 / 18,1 | 18,8 / 6,0 / 20,8 | 18,9 / 5,8 / 23,5 |
| C (492) | 18,1 / 7,6 / 6,3 | 18,2 / 4,0 / 14,1 | 17,9 / 4,0 / 17,5 |
| B (492) | 18,2 / 5,6 / 51,3 | 12,4 / 2,9 / 32,6 | 13,3 / 2,9 / 27,1 |

- Abstand der Wurzeln A-C / C-B: P1 0,4 / 4,1 px, Spitzen 3,8 / 9,9 px: die Krallen sitzen auf einem Fleck und spreizen (B 51 Grad).
- **Kreuzungen (Strecken Wurzel-Spitze im Bild):** P1 A-B und B-C, P2 A-C und B-C, PV A-C. Vorlage: keine.
- Die Hand haengt im Modell -27,6 Grad schraeg (P1), der Unterarm +9,2 Grad: Knick 37 Grad am Handgelenk (Vorlage 9-14 Grad). Das ist die
  Clip-Pose (Gelenkdrehung Knochen 12), kein Mesh-Mass.

## 4. Ursache an den Mesh-Zahlen

1. **Mesh 12 ist auf 0,55 / 0,54 / 0,55 geschrumpft** (`tools/birkin_formen.py` FORMEN hangar skal 12; Dossier NB2neu.2 "Linke Klauenhand
   kleiner, nach dem Video"). Ausdehnung heute x 343, y 594, z 235 E. (mit den in V7 verlaengerten Krallenspitzen) gegen das Original
   EM30 x 588, y 846, z 313. Der Ring am Handansatz ist 110 E. breit (Original 200), der Handgelenk-Ring des Unterarms 248 E.: die Hand
   ist am Ansatz schmaler als das Ende des Unterarms - daher die Einschnuerung S3 (12,2 px statt 21,1).
2. **Die breite Achse der Hand zeigt zur Kamera.** Breit ist die Hand in x (Daumen bei +x, 588 E. im Original), duenn in z (313 E.;
   die Krallen kruemmen sich nach -z, der Handruecken ist +z). Kamera im Raum von Knochen 12: P1 (0,80 / 0,42 / -0,42), P2 (0,96 / 0,24 /
   0,16), PV (0,88 / 0,40 / 0,23) - in allen drei Posen steht sie auf der +x-Seite (Azimut um die Handachse 118 / 80 / 75 Grad). Man sieht
   die Daumenkante und in P1 schraeg die Handflaeche, nie den Handruecken. Die Vorlage zeigt den Handruecken mit dem Daumen zum Koerper;
   dazu muss das Mesh um die Handachse gedreht werden (Handruecken +z auf Azimut 118 Grad: der Daumen +x zeigt dann im Bild P1 nach
   (-89, -27) px je 1000 E. = zum Koerper, ohne Spiegelung).
3. **Mesh 11 (Unterarm) = Original x (1,229 / 0,80 / 1,229):** breitester Ring bei y 236 (516 E.), von dort gerade Verjuengung auf den
   Handgelenk-Ring y 627 (248 E.), Spitze v17 bei y 674, Gelenk zur Hand bei y 659. Im Bild P1 stimmt der Verlauf von 50 % bis 90 % mit der
   Vorlage (36,5-24,2 gegen 33,5-22,5 px); zu breit ist die Wulst bei 20-40 % (42,5 gegen 36,6 px, Faktor 0,86), zu schmal das Ende
   (100 %: 15,0 gegen 22,0 px). Die "Spindel" entsteht aus Wulst + duennem Ende + fehlender Hand, nicht aus der Mitte des Unterarms.
4. **Massstab P1** (px je 1000 E.): Knochen 12 y 84,6, x 60,9, z 81,5; Knochen 11 y 60,3 (zur Kamera geneigt), x 73,3, z 74,1.
   Vorlage-Handlaenge 59,5 px = 0,80 x Original-Hand (875 E. x 0,0846 = 74,0 px bei 1,0). Vorlage-Handbreite mit Daumen 44,5 px =
   0,93 x Original (588 E. x 0,0815 = 47,9 px bei 1,0, breite Achse in der Bildebene). Vorlage-Handgelenk 21,1 px = 287 E. (heute 248).

## 5. Widerspruch, der bleibt (fuer "offen")

Die Armkette des Modells haengt in P1 tiefer als in der Vorlage: Ellbogen 0,51 H statt 0,63 H, Handgelenk 0,38 H statt 0,49 H ueber der
Sohle (Skelett + Clip-Pose, nicht Auftrag). Die heutige kleine Hand endet deshalb fast auf Vorlagenhoehe (0,25 H gegen 0,30 H) - das war der
Grund fuer die Verkleinerung in NB2neu.2. Eine Hand in Vorlagengroesse (59,5 px ab Handgelenk) endet im Modell etwa 0,07-0,09 H tiefer als
in der Vorlage (auf Kniehoehe). Der Nutzer-Befund P6 gilt der Groesse und Breite der Hand; der Bau folgt deshalb dem Breitenprofil und nennt
die Endhoehe der Spitzen und die Durchdringungs-Keyframes als Zahl (Riegel S17 g).

## 6. Bauplan (Zielzahlen fuer Runde 6a)

- Mesh 12 = Original-EM30-Hand (CDEMD0.EMS Typ 0x30, Mesh 12), Massstab aus Abschnitt 4.4 (Laenge 0,80, Breite 0,80-1,0 als Varianten),
  um die Handachse gedreht (Varianten um Azimut 118 Grad), Krallen-Zeilen 491/492 und Flaechen wie V7 (Topologie gleich), UV unveraendert.
  Die Original-Krallen sind bei 0,80 laenger als die V7-Krallen (A 22, C 26, B 17, T 16 px gegen 18 / 18 / 18 / 12).
- Mesh 11: Wulst y 236 schmaler (Faktor 0,86 aus 3.), Handgelenk-Ring und Ende breiter (287 E. statt 248), Ende ueberlappt den Handansatz.
- Auswahl nach Zahl: Abweichung des Breitenprofils S1-S8 in P1 gegen die Vorlage-Mediane, Handbreite/Handlaenge, Kreuzungen, danach
  Endhoehe und Durchdringung.
- Riegel S17: Toleranz je Mass = Streuung der fuenf Vorlagebilder (Spalte "Vorlage px", kleinster bis groesster Wert) +- 1 px Rasterkante.
