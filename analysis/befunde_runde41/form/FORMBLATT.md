# FORMBLATT Bahnsteig-Form

Stand 2026-10-31, Gegenprobe eingearbeitet (Abschnitt 10). Einzige Formvorlagen: `bild1.png`, `bild2.png`. Dieses Blatt ersetzt keine Lesung, es entscheidet zwischen ihnen.
Pixel: Originalbild 640x480. Einheiten: Spiel. Ablesung: `formblatt_bild1.png`, `formblatt_bild2.png`. Maschinenlesbar: `formblatt.json`.

Kennung jeder Zahl: **[g]** gemessen, **[s]** geschaetzt, **[f]** frei. Sicherheit jeder Bildaussage: sicher / wahrscheinlich / offen.
Eine Spanne bei [g] heisst: gemessen, aber abhaengig von der unbekannten Tiefe des Teils.

## 0. Kurzfassung

- Massiger Vierbeiner mit einer einzigen runden Kuppel als Rumpf (Bild 1 x247-381, y187-318, sicher; Bild 2 belegt bis mindestens x=413, nach links und unten ohne gemessene Grenze, Abschnitt 1.4). Scheitel so hoch wie Leon, Rumpf tief getragen, Beine kurz und dick (wahrscheinlich).
- Rumpf liegt in Bild 1 NICHT auf: Boden ist unter ihm sichtbar, (283-325,327-368) und (345-378,329-368) (sicher). "Liegend" heisst hier: lange Achse waagerecht, Bauch auf rund einem Drittel der Hoehe.
- Drei Beine sind sichtbar, das vierte steht hinter Bein NR oder steckt in der Spalte C1 (wahrscheinlich). Sicher ist nur die Sohle K1. K2, K3 und K4 sind offen, die harten Kanten enden vor ihnen (Abschnitt 1.2). Das Fussviereck ist eine Annahme, keine Messung.
- In Bild 1 blicken wir fast laengs auf die Figur (wahrscheinlich). Die helle Zeichnung sitzt mittig auf dem kameranahen Ende, x 314-329, y 221-384. In Bild 2 fehlt sie auf (290-340,225-280) (sicher), also zeigt Bild 2 eine andere Seite (wahrscheinlich).
- Farbe: fast schwarze Haut, Farbton offen; rotbraune Mitteltoene um die Zeichnung und hellgraue Baender (wahrscheinlich). Violett, Lavendel und Blau sind Video (sicher).
- Kopf: nicht erkennbar (offen). Das ist die groesste Luecke. Ausgeschlossen ist er nur dort, wo heller Grund hinter dem Umriss liegt (Abschnitt 5). Die Kandidaten stehen in Abschnitt 6.

## 1. Koerperbau

### 1.1 Massstab

| Groesse | Wert | Kennung | Beleg |
|---|---|---|---|
| Leon Hoehe | 3005 (Bilder 2962-3009) | [g] | PL00 Clip 0, Modell durchgerechnet |
| Frau Hoehe | 2785 | [g] | Typ 0x4D Clip 0, Modell durchgerechnet |
| Bild 1 Quermassstab | 58 / 67 / 77 / 84 px je 1000 bei Bodenzeile 374 / 395 / 416 / 433 | [g] | `kamera_bild1.json`, Augenhoehe 2180, Horizont y=241 |
| Bild 2 Quermassstab | 89 / 75 / 64 / 56 px je 1000 bei Tiefe 5000 / 6000 / 7000 / 8000 | [g] | ROOM5090 Cut 11 |

### 1.2 Koerperrahmen

Ursprung O = Mitte des Fussvierecks. **Ende N** = kameranah in Bild 1 (u<0). **Ende F** = kamerafern (u>0). **Seite R** = rechts in Bild 1 (v<0). **Seite L** = links (v>0). h = Hoehe ueber Boden.
"Vorn" und "hinten" sind bewusst nicht vergeben, siehe Abschnitt 6 C.

Bild 1: O = (-18514, 0, -18726), u = (0.989, 0, -0.147), v = (0.147, 0, 0.989). [g] innerhalb der Kamera, Weltlage selbst offen (`kamera.md`).

| Kontakt | Bild | (u, v) | Tiefe | Sicherheit |
|---|---|---|---|---|
| K1 Bein FR | (398,374) | (1414, -601) | 6910 | sicher |
| K2 Bein NL | (232,433) | (-1414, 601) | 4583 | offen. Harte Kante endet bei (201,400). Der dunkle Streifen y 430-436 reicht bis x=195, ist also breiter als das Bein |
| K3 Bein NR | (312,452) | (-1430, -492) | 4236 | offen. Liegt am Unterrand von C5, und C5 ist selbst offen |
| K4 Bein NR | (352,432) | (-839, -710) | 4731 | offen. Liegt auf der weichen Aussenkante von C4 |
| K6 Bein FL, verdeckt | Viereck (328,370). Mit einem Fuss wie A2 nur (321,374) +-4 moeglich | (1141, 610), mit Fuss wie A2 (901, 623) | 7018, 6794 | offen, aus dem Viereck gerechnet |
| K5 Innenglied | (258,395) | (-307, 1043) | 5772 | offen, Fuss selbst nicht sichtbar |

K3 und K4 werden als ein Bein gefuehrt: Abstand 629 [g], das ist weniger als die Fussbreite.

Kantenbreite 10-90 in Y [g]: Pfeiler A 7, Saeule NL 8, Kugel links 6, Oberkante C5 7. Dagegen C4 rechts 21-25, C4 unten 9-20, B3 unter y=400 13-23, Block unten 12-26.
Der Engine-Schatten (ABR2, Plateau 88/88/96 bis r/R 0,65, `shadow_blob_data.h`) drueckt Boden mit e unter 90 auf das Schwarz der Figur. Dunkel und "Helligkeit wie Koerper" belegen am Boden also keinen Koerper.
Letzte harte Stellen: Bein NL (201,400), Bein NR (312,414). Tiefer ist eine Sohle moeglich, aber nicht gelesen.

Der Rahmen O, u, v haengt an K2 bis K4. Mit den lesbaren Alternativen (Sohle NR unterhalb Zeile 410, Sohle NL Zeile 400-433) gilt: Laenge 1750-2850, Quer 750-2050, Achse 15-27 Grad zur Sehachse, Gier Ende F vorn 65-197.

### 1.3 Gesamtmasse

| Mass | Wert | Arbeitswert | Kennung | Sicherheit |
|---|---|---|---|---|
| Scheitelhoehe | Bild 1 2800-3060, Bild 2 2860-3010 | 2950 | [g], Arbeitswert [s] | wahrscheinlich |
| Bauchhoehe, L2 y=313, L1 y=322 | 850-1310 | 1050 | [g], [s] | wahrscheinlich |
| Rumpfdicke | Scheitel minus Bauch | 1900 | [s] | wahrscheinlich |
| Kuppelbreite quer, Bild 1 x247-381 | 1750-2300 | 2000 | [g], [s] | wahrscheinlich |
| Kuppel plus Schulter, Bild 1 x247-430, Kuppel Tiefe 5000-6000, Schulter Tiefe 6900 | 1750-2400 | 2300 | [g], [s] | wahrscheinlich |
| Ganze Figur quer, Bild 1 x197-430, in v, links Tiefe 4580-5640, rechts 6900 | 2150-2950 | 2350 | [g], [s] | wahrscheinlich |
| Fussviereck laengs | 1750-2850 | 2550 | [s] | offen |
| Fussviereck quer | 750-2050 | 1230 | [s] | offen |
| Kuppellaenge laengs | Bild 2 in Sicht Flanke, x255-418: 1970-2490, linke Grenze dabei nur gelesen | 2000 | [s] | offen |
| Gesamtlaenge mit Fuessen | | 3400 | [s] | offen |
| Bodenschatten | nicht messbar, siehe unten | | | offen |

Bodenschatten: Die rechte Grenze der Dunkelzone laeuft in (u,v) ueber (-1044,-847), (-362,-855), (3,-739), (419,-590), (900,-401). Der letzte Punkt liegt 1850 von der frueheren Kreismitte u=-795, v=336, der Kreis mass Radius 1200.
Eine Kreisform ist damit nicht belegt. Zudem hat der Boden von sich aus Tuerkis (e rund (53,65,55)) und weisse Lichtinseln, Schatten und Bodenton sind nicht trennbar.

### 1.4 Teile

Breiten bei halber Kantenhoehe. Hoehen ueber dem Fusspunkt des Teils.

| Id | Teil | Bild 1 | Mass | Kennung | Sicherheit |
|---|---|---|---|---|---|
| R | Kuppel | x247-381, y187-318 | ein Scheitel, kein Hals, keine Kerbe im freien Umriss. Stange x=280 endet an der Kante bei y=191 | [g] | sicher |
| S | Schulter Seite R | x381-430, y238-300 | Kuppe (405,238) h 2230, reicht 31 px = 530 ueber die Beinachse hinaus. Rechte Kante nur y 238-278 lesbar, (429,272). Darunter dunkler Wandsockel ohne Kante | [g] | wahrscheinlich |
| A1 | Bein FR Pfeiler | x381-418, y300-358 | 33-37 px = 570-640 breit, nach unten leicht breiter | [g] | sicher |
| A2 | Bein FR Fuss | x378-426, y356-376 | 44-48 px = 760-840 breit, Knoechel h 300 | [g] | wahrscheinlich |
| B1 | Bein NL Saeule | x226-287, y298-346 | 61 px = 725-795 breit, h 990-1530 | [g] | sicher |
| B2, B3 | Bein NL Kugel | x200-252, y341-426 | zwei Lappen, Kerbe (206,374), 52 px = 620 breit, Oberkante h 1045 | [g] | oben sicher, unten wahrscheinlich |
| B4 | Innenglied | x250-285, y346-420 | 35 px = 420-460 breit | [g] | wahrscheinlich |
| B5 | Masse unter B | y402-434 | Ende unter dem Logo | | offen |
| C1 | Bein NR oben | x300-347, y318-350 | 45 auf 36 px = 590-770 auf 470-620, schmalste Stelle y=346-350. Verjuengung nur links: rechte Kante x 342-347 ohne Knick von y=320 bis y=368 | [g] | sicher |
| C3 | Untermasse | x290-346, y353-400 | gerade Oberkante (285,368)-(325,345) | [g] | wahrscheinlich |
| C4 | Rundling | x305-372, y396-448 | Breite 62-67 px = 690-750 ist Obergrenze, Oberkante h 550. Linke Kante hart, Aussenkante weich, Abschnitt 1.2 | [g] | Masse wahrscheinlich, Umriss offen |
| C5 | Block mit Querstreifen | x288-330, y405-452 | Figur oder Boden durch Luecke. Gleiche Farbfolge lavendel, tuerkis, dunkel wie der Boden links bei x200-215, gleiche Zeilen | | offen |
| D | Bein FL | nicht von C1 trennbar | in Breite wie A angenommen. Eine Kopie von A auf K6 liegt bei x 311-345, ihre rechte Kante faellt auf die von C1 | [f] | offen |

Spaltenbreite je Zeile [g]: C1 46 px bei y=320-328, 39 bei y=340, 35 bei y=348. A1 33 bei y=320-324, 37 bei y=340-348. Gleich stark sind sie nur am unteren Ende (sicher). Das passt nur, wenn C1 auf aehnlicher Tiefe wie A1 liegt, oder mit einer Taille, siehe Abschnitt 6 E.

Bild 2 (Kamera sicher, Lage der Figur nicht gemessen):

| Id | Teil | Bild 2 | Sicherheit | Beleg |
|---|---|---|---|---|
| R | Kuppel, Kante belegt | (354,161)-(362,165)-(370,171)-(378,175)-(390,181)-(394,187)-(411,206) | sicher bis x=378, danach wahrscheinlich | Verdeckung: R faellt an der Platte von 140 auf 46, ab x=382 Video unter Raum, Rohr endet bei (411,206) |
| R | Kuppel, Kante gelesen | (258,215)-(268,205)-(278,190)-(290,170)-(300,162)-(320,156)-(345,160) | wahrscheinlich | vor der schwarzen Tuer keine Verdeckung, nur Blauanhebung der Zeichnung. Silhouette kann weiter aussen liegen: Rechts liegt die Linie 21-33 px innerhalb der Kante |
| U | Untermasse | (218-300,286-372), Mindestausdehnung | sicher | Lichtbalken y=292, x=218-298 ganz verdeckt, Bodenlinie y=347, x=232-285 ganz verdeckt. Der Balken endet selbst bei x rund 218, links davon fehlt jeder Bezug |
| | Rechter Rand | mindestens x=413, x 416-420 fuer y=225-262 | wahrscheinlich | Rohr, im Video bei x rund 408, dort unsichtbar: Video R 68-94 gegen Raum 140-194 |
| H | Haken | (205-245,215-262) | wahrscheinlich | gefuellte Flaeche, Inneres V (72,22,47), Tuer (34,7,11) |
| G | Unterstreifen | (338,320)-(285,358), 15 px | sicher | Violett auf der Kante, keine Bodenleiste |
| F | V-Form | (263-304,355-399) | offen | keine Bodennaht, Figur oder Feuer offen |
| Y | Deckende Flaeche | Ellipse um (239,418), rund 86 x 66 px | deckend: sicher, Figur: offen | Abschnitt 4, W13 |

Nicht als Form uebernehmen:

| Stelle | Urteil | Sicherheit |
|---|---|---|
| Bild 1 dunkle Bahn (250-300,150-190), Stange (272-283,130-190) | Hintergrund. Figur davor ist nur bei x 276-345 ausgeschlossen, bei x 250-276 dunkel auf dunkel | sicher |
| Bild 1 weisse Flecken (283-311,328-369), (345-378,325-375) | Boden | sicher |
| Bild 1 Zone (345-378,362-402) | Boden, Schatten oder Bodenton offen | nicht Figur: wahrscheinlich |
| Bild 1 Sockel (425-455,284-302) | dunkler Wandsockel, Y flach 30-40 ueber x 400-456, kein Figurrand | sicher |
| Bild 2 rote Scheibe (350-395,112-172) | Tuerplatte, Unterkante ist die Figurkante | sicher nicht Figur |
| Bild 2 weisse Lappen (160-228,325-388), (415-462,225-300), (415-505,310-400) | Feuer | sicher nicht Figur |
| Bild 2 weisse Flaeche (278-450,395-475) | Boden, vom Raum erklaert: Video R 205-215, Raum 226 | sicher nicht Figur |
| Bild 2 weisse Flaechen (540-640,265-450) | ueberstrahlte Kisten | sicher |
| Bild 2 heller Balken (278-335,405-415) | Bodenfuge, im Raum vorhanden | sicher |
| Bild 2 dunkler Keil (420-470,350-420) | folgt der Bodenkante | wahrscheinlich |

Unterkante rechts: Die Bodenlinie y=338 ist bei x=340-364 ohne Kontrast (R 0 bis +14 statt +26 bis +46), die Figur reicht dort also bis mindestens y=345 (wahrscheinlich). Ab y=380 ist Boden frei, R 207 bei (345-405,382-396) (sicher). Dazwischen weicher Uebergang ueber 40 px: Schatten oder Feuerschein, Lage der Kante offen.

## 2. Zeichnung

Alle Farben sind Texturwerte aus `farbe.md`. Schirmfarbe = Textur mal Licht. Die Engine rechnet rein diffus, Glanzlichter gibt es nicht.
Daraus folgt: **In Bild 2 ist das Licht flach, jede Linie dort ist Textur** (wahrscheinlich, das Beta-Licht selbst ist nicht belegt, passt aber zum Mantel der Frau). In Bild 1 aendert Licht die Helligkeit bis etwa Faktor 2, nicht den Farbton.

### 2.1 Materialien

| Material | Textur (Fehler) | Lage | Kennung | Sicherheit |
|---|---|---|---|---|
| Dunkle Grundhaut | (44,18,27) (+-22,19,22), alles bis (70,40,45) waere im Video gleich schwarz | ganzer Koerper | [g] | dunkel sicher, Farbton offen |
| Rotbraune Mitteltoene | (103,63,84) (+-21,17,17) | um die helle Zeichnung, A35 (335,224), A25 (265,390), B04 (309,186), B02 (277,206), Hakeninneres | [g] | wahrscheinlich |
| Rosa Stelle | (181,137,150) (+-25,20,22) | nur Z1 | [g] | Farbton wahrscheinlich, Helligkeit offen |
| Helle Baender | (126,145,156) (+-23,28,32), neutrales (140,145,150) liegt im Fehler | Z2, Z4, Z5, Z6, Q4, Haken-Rand | [g] | wahrscheinlich |
| Feine Linien | (114,123,143) (+-30,28,35) | Z7, Z8, Q1, Q3, Q5 | [g] | wahrscheinlich |
| Unterstreifen | hoechstens (225,209,229) | G | [g] Obergrenze, echte Farbe dunkler | offen |
| Weisse Kerne | nur "mindestens so hell wie Band" | Z3, P | [f] oberhalb | offen |

### 2.2 Stellen

Groessen in Bild 1 fuer Tiefe 4600-5800, bei Z5 Tiefe 6900. Breiten unter 4 px sind Obergrenzen.

| Id | Stelle | Bild | Lage am Koerper | Groesse | Kennung | Sicherheit |
|---|---|---|---|---|---|---|
| Z1 | Rosa Spindel, Kern (325,231), Flaeche (300-339,220-259), Achse +53 Grad | 1 | Ende N, Mittellinie, h 2245-2265 | 570-710 x 300-370 | [g] | wahrscheinlich |
| Z2 | Querband A02 (317,265), -31 Grad | 1 | Ende N, Mittellinie, h 1840-1925 | 390-480 x 100-130 | [g] | wahrscheinlich |
| Z3 | Weisskern A03 (326,293) | 1 | Mittellinie, h 1450-1630 | 260-330 x 150-190 | [g] | Form wahrscheinlich, Farbe offen |
| Z4 | Band A04 (320,303) | 1 | direkt unter Z3, Trennung schwach | 260-320 x 90-120 | [g] | wahrscheinlich |
| Z5 | Kappe, Keil (385-415, 277-296) | 1 | oberes Ende von Bein FR, h 1280-1590 | 515 breit, 310 hoch | [g] | sicher als Stelle |
| Z6 | Quertrio A14 (329,351), A15 (322,366), A16 (314,384), parallel, Abstaende 16 und 20 px | 1 | Bein NR, A14 sitzt an der Taille | je 230-290 x 70-90 | [g] | A14 wahrscheinlich, A15 und A16 offen |
| Z7 | Bogen A20 (241,346), Knick-Band A22/A23, Knick (249,381) | 1 | Oberseite der Kugel | Band 60-110 breit | [g] | wahrscheinlich |
| Z8 | Dreierreihe A09 (241,289), A08 (253,285), A07 (272,284), Abstaende 12 und 19 px | 1 | Uebergang Rumpf zu Bein NL | Striche 140-200 x 65-80 | [g] | Lage wahrscheinlich, Bedeutung offen |
| Z9 | Schraegstrich (411,240)-(420,262), Y 65-73 | 1 | Schulter Seite R, 4-7 px innerhalb der Aussenkante, parallel dazu | 33 x 5 px, Textur rund (85,80,60), grauer als die Wand (R/G 1,1 gegen 1,6) | [g] | wahrscheinlich |
| Q1 | Bogen links B01, B02 (262-294,168-225) | 2 | Rumpf, Abstand zur Kante nicht messbar | 9.5 px breit | [g] | wahrscheinlich |
| Q2 | Scheitelsichel, B03 (327,168), Unterkante y=195 | 2 | Scheitelzone | | [g] | wahrscheinlich |
| Q3 | Bogen rechts B06 (375,205), 35 x 4.6 px | 2 | 21 px innerhalb der Kante, Innenlinie | | [g] | wahrscheinlich |
| Q4 | Randband B07 (411,217), 34 x 6 px | 2 | 8 px innerhalb der Kante, belegt an (411,206). Beginnt bei (403,210) | | [g] | wahrscheinlich |
| Q5 | Innenlinie (392,240)-(362,296), endet in Spitze P | 2 | Rumpf Seite | | [g] | wahrscheinlich |

**Zug Q:** Unterkante Q2, Q3, Q5 und P sind wahrscheinlich eine Linie, Blau-Grat zeilenweise verfolgt: (300,192)-(314,189)-(345,189)-(353,194)-(372,201)-(384,218)-(388,236), Stoerung y 242-254 bei x 393-398, weiter (383,260)-(374,278)-(363,290)-(365,302). Laenge rund 200 px.

Punkte A10 (291,208), A05, A06, B10 (280,275), B11 (258,263), B12 (217,274): Art nicht erkennbar, kein Auge belegbar (offen).

### 2.3 Regeln fuer die Textur

- Baender sind nicht gleichmaessig. Gipfel auf Bein NR bei 28, 60, 16 px Abstand, Bein FR unterhalb Z5 ganz ohne Band (sicher). Keine Ringelung malen.
- Gemessen ist gleiche Bildzeile: Z5, Z3/Z4 und Z8 liegen in y 277-310 (sicher). Gleiche Hoehe h rund 1300-1700 folgt nur, wenn jede Marke auf der Tiefe ihres Beins liegt (offen).
- Stapel Z1, Z2, Z3, Z4, Z6 liegt auf einer Senkrechten: x 321, 317, 326, 320, 329, 322, 314, also 15 px Streuung ueber 150 Zeilen (sicher). Das ist die Mittellinie oder ein Glied, siehe Abschnitt 6 D.
- Rumpfinneres in Bild 2 (290-340,225-280) ist V (40,6,16), also Grundhaut ohne Zeichnung (sicher).
- Zug Q als durchgehende Linie malen, nicht als drei Stuecke (wahrscheinlich).

## 3. Die beiden Posen

### 3.1 Pose 1, Bild 1

Vier Beine am Boden, Rumpf waagerecht, Neigung nicht messbar. Figur steht fast laengs zur Kamera: Achse N nach F liegt 15-27 Grad rechts der Sehachse [g], Spanne aus K2 bis K4.
Gier fuer den Lader: 2145 wenn Ende N vorn, 97 wenn Ende F vorn [s], Spanne Ende F vorn 65-197 (Vorderseite +X, Vorwaerts = (cos G, -sin G)).

| Gelenk | Bild | h | Kennung | Sicherheit |
|---|---|---|---|---|
| Scheitel | (300-322,187) | 2800-3060 | [g] | sicher als Stelle |
| Rumpf rechte Flanke frei | (348,205) (354,209) (360,212) (366,215) (372,225) (378,233) | | [g] | sicher |
| Bauchkante | L2-Spitze (293,313), L1 (362-381,322). Keine Gerade: L2 ist ein Dreieck zwischen zwei Gliedern | 850-1310 | [g] | wahrscheinlich |
| **Bein FR** Schulterkuppe | (405,238) | 2230 | [g] | wahrscheinlich |
| Bein FR oberes Gelenk | Spitze (400,277), Basis (398,296) | 1590, 1280 | [g] | Stelle sicher, Gelenk wahrscheinlich |
| Bein FR Unterglied | Achse x=397,5-399,5, y=320-356, linke Kante x=381 | | [g], Neigung bis 3 Grad gegen die Senkrechte in der Bildebene | sicher |
| Bein FR Knoechel | (398,356) | 300 | [g] | wahrscheinlich |
| Bein FR Sohle | K1 (398,374), Fussspitzen links (377,362), rechts (426,362) | 0 | [g] | sicher |
| **Bein NL** Ansatz | (257,298) | 1530 | [g] | wahrscheinlich |
| Bein NL Saeule unten | (257,346) | 990 | [g] | wahrscheinlich |
| Bein NL Kugel | Oberkante (232,341), Kerbe (206,374) | 1045 | [g] | oben sicher |
| Bein NL Sohle | K2 (232,433), harte Kante nur bis (201,400) | 0 | [g] | offen |
| **Bein NR** Ansatz | (325,318) | 910-1260 fuer Tiefe 6900-5000 | [g] | sicher als Stelle |
| Bein NR Taille | (326,350) mit Querband A14 | 390-850 | [g] | Taille sicher, Gelenk wahrscheinlich |
| Bein NR Unterteil | Oberkante (285,368)-(325,345), Rundling Oberkante (335,396) | Rundling 550 | [g] | wahrscheinlich |
| Bein NR Sohle | K3 (312,452), K4 (352,432), harte Kante nur bis (312,414) | 0 | [g] | offen |
| **Bein FL** | verdeckt oder in C1, Fuss (328,370), mit Fuss wie A2 (321,374) | 0 | [s] | offen |
| Kopf | nicht erkennbar | | | offen |

Beugung in Blickrichtung ist bei allen Beinen nicht messbar. Bein FR wirkt nur in der Bildebene gerade.

### 3.2 Pose 2, Bild 2

Lesbar sind nur Kuppel und wenige Linien. Kein Bodenkontakt ist sicher. Standort und Gier haengen an der Sicht aus Abschnitt 6 B.

| Stelle | Bild | Aussage | Sicherheit |
|---|---|---|---|
| Scheitel | (320,156) | h 3010 bei Abstand 5000, 2960 bei 6000, 2910 bei 7000, 2860 bei 8000 [g] | wahrscheinlich, Lage nur aus Blauanhebung und Plattenkante (354,161), vor Tuerschwarz keine Verdeckung |
| Rumpf | Kante wie 1.4 | Kuppel wie in Bild 1, gleiche Hoehe im Fehler | wahrscheinlich |
| Schulter Seite R | Randband Q4, Rand x416-420 | gleiche Struktur wie S in Bild 1 nur in Sicht Flanke | offen |
| Unterglied | G (338,320)-(285,358), weiter bis (278,398) | schraeg nach links unten, 45 Grad, unten 62 Grad | sicher als Stelle |
| Haken | (215,222)-(243,218)-(245,236)-(222,262) | Anschluss oben nicht entscheidbar: (246-255,222-258) V (33,6,13) +-(3,3,5), freie Tuer (29-37,6-8,8-14), Rumpf (40,6,16). Unten Band y 258-272, V (37,12,25), bis (262,265) | Form wahrscheinlich, oben offen |
| Spitze P | (358-374,296-313) | Ende der Innenlinie, weisser Kern | wahrscheinlich |
| Fuesse | keine sicher | Probe unten | offen |

Probe (kein Messwert) in Sicht Flanke: O = (2625, 0, -23625), Achse u = (cos 85, sin 85) in (x,z). Gier 1081 wenn Ende N vorn, 3129 wenn Ende F vorn. [s]
Fuesse aus Bild 1 fallen dann auf K1 (433,413), K2 (206,387), K3 (208,437), K4 (259,442), K6 (376,368). Die Figur haette rund 2630 Abstand zur Frau, Ende N zeigt zu ihr.
Diese Treffer zeigen nur Vertraeglichkeit: Bild 2 ist eine andere Szene, die Pose kann anders sein, und K2 bis K4 sind offen.

## 4. Widersprueche und Entscheidungen

| Nr | Widerspruch | Entscheidung | Begruendung am Bild |
|---|---|---|---|
| W1 | Mensch 2750 (Tiefe) gegen 3020 (Kamera) | Leon 3005, Frau 2785 | Modelle gerechnet. Alle Tiefe-Masse in Spieleinheiten sind rund 10 % zu klein und hier neu gerechnet |
| W2 | Kamera Bild 1: Tiefe f 440, Hoehe 1983, Hauptpunkt 320, gegen `kamera.md` 408/428, 2180, 391 | `kamera.md` | Raumgeometrie aus 5 Cuts. Der Hauptpunkt bleibt dort offen, Hoehe und Horizont tragen den Massstab auf rund 4 % |
| W3 | Groesse: Bild 1 0,94-1,01 Mensch, Bild 2 1,08-1,11 (Tiefe) | kein Widerspruch | Mit Kamera aus der Disc: 2800-3060 gegen 2860-3010 Einheiten. Die Differenz kam aus zwei Menschmassen |
| W4 | Kontakte: Gesamtbau 3 sicher + 1 wahrscheinlich, Tiefe 1 sicher + 3 wahrscheinlich, Nutzer vier Beine | Vier Beine. K1 sicher, K2 bis K4 offen, viertes verdeckt oder in C1 | Nur K1 hat eine lesbare Sohle (374-376). K2 bis K4 liegen auf weichen Kanten oder auf offenen Teilen (Abschnitt 1.2). Das Fussviereck fuehrt auf (328,370), dort liegt C3 davor |
| W5 | "Liegt nicht auf" (Gesamtbau, Tiefe) gegen "eher liegend" (Nutzer) | Beides gilt | L1 (345-378,329-368), L2 (283-325,327-368) zeigen Boden unter dem Rumpf (sicher). Waagerechter Rumpf, Bauch h 1050 von 2950 |
| W6 | Zeichnung: "die meisten duerften Licht sein", Farbe: Oberflaeche | Oberflaeche | Kein Glanz in der Engine (`re15_light.h`), Bild 2 hat flaches Licht und zeigt die Linien trotzdem |
| W7 | Zone (345-378,362-402): Fremd "Koerper mit Fremdfarbe", Gesamtbau "Bodenschatten" | Nicht Figur, Schatten oder Bodenton offen | Y 75-93, Kante rechts 25 px. Harte Koerperkanten 6-8 px. **`maske_bild1.png` schliesst diese Zone faelschlich ein.** C4 ab y=396 bleibt Masse (Y 34), linke Kante hart, Aussenkante 9-25 px, also nicht als Umriss lesen |
| W8 | Linke Grenze Bild 2: Gesamtbau x262 willkuerlich, Fremd x~230, Tiefe x207 | Unten mindestens bis x218, oben nicht begrenzt, Haken eigener Teil | V1 endet, weil der Balken dort endet, Raum R 57 bei x=215, 46 bei x=205. Die Blauanhebung im Tuerschwarz y=205 x~258, y=250 x~255 liegt im Rauschen |
| W9 | Rechter Rand Bild 2: Fremd "nicht erkennbar", Gesamtbau x426 | mindestens x=413, x 416-420 wahrscheinlich | Rohr bei (408,225-262) fehlt, Feuer beginnt bei x 415-421 und kann davor liegen |
| W10 | Unterkante rechts Bild 2: Fremd laesst sie an (330,317) enden, Tiefe (370,368) | bis mindestens y=345, Rest offen | Bodenlinie y=338 ohne Kontrast x=340-364; ab y=380 R 207 |
| W11 | Frau: Versatz (+8,+15) (Kamera), Kopf y=187 (Tiefe), Spawn (Farbe) | Spawn (600,0,-25300) | Ihr Haar ist dunkel, die Kopfoberkante fehlt im Video. Kopf bei (168,185) mit 2785 Hoehe ergaebe Fuss (195,436), das passt nicht. Video y 165-355, Modell y 169-358 |
| W12 | Weisse Flaechen: Gesamtbau Raumlicht, Zeichnung Effekt, Fremd Feuer, Stelle (591,246) "nicht im Raum" | Feuer, rechts unten Kisten, unten Mitte Boden | Echtes Spielfenster Cut 11 zeigt Flammen in diesen Zonen, Kisten deckungsgleich (`kamera.md`). Lage der Flammen weicht ab, Art stimmt. (278-450,395-475) erklaert schon der Raum |
| W13 | Flaeche unten links Bild 2: Tiefe "kaum dunkler, offen", kein Eintrag bei Fremd | deckend, kein Schatten | Helle Querfuge: Kontrast 0 +-5 bei x=225-255, erwartet +114..+134. Bei x>=275 sichtbar +72..+104. Kante 12-18 px. Im Spielfenster liegt dort kein Objekt. Teil der Figur: offen |
| W14 | Weisser Riegel (254-278,447-459): Logo oder Bildinhalt | Bildinhalt | Ueber alle 174 Dias Minimum (1,1,4). Nur das Wasserzeichen geprueft, die Dias sind keine Formquelle |
| W15 | Haken: Gesamtbau "Anschluss nicht sichtbar", Fremd "haengt ueber Stufe an der Figur", Zeichnung "17 px getrennt" | Oben offen, unten verbunden | Messung siehe 3.2 |
| W16 | Oberkante Bild 1: Gesamtbau x339@y206, Fremd (326,206) | sicher erst ab x=348 | Spalten: (348,205) bis (378,233). Links davon stoert das Hintergrundband y~195: x=306-345 Kante y 188-206, offen |
| W17 | Merkmale zwischen den Bildern: Zeichnung "keines belegbar", Gesamtbau "Beule und Schraegglied wahrscheinlich" | keines sicher | Zuordnung haengt an Abschnitt 6 B. Sicher gemeinsam sind nur Kuppelform und Scheitelhoehe |
| W18 | Schatten: Tiefe R 1112 | Nicht messbar | Grenze reicht bis u=900, v=-401, ausserhalb jedes Kreises um die alte Mitte, Abschnitt 1.3 |

## 5. Was die Bilder nicht zeigen

Alles hier ist **frei** [f]. Der Modellbau darf es setzen, es ist aber nicht belegt.

| Stelle | Stand |
|---|---|
| Kopf, Gesicht, Augen, Maul, Zaehne | nicht erkennbar. Keine Erhebung belegt ist nur fuer Bild 1 x 276-345 (Panel, Stange) und Bild 2 x>=354 (Platte). Bild 1 x 250-276 und Bild 2 x<354 sind dunkel auf dunkel: Dort waere auch eine Erhebung unsichtbar |
| Seite L (v>0) | in keiner Deutung sichtbar. Vorschlag: Seite R spiegeln |
| Ende F | sichtbar nur, falls Sicht Ende stimmt |
| Unterseite | nur Bauchkante, Flaeche nie |
| Oberseite | nur streifend: Auge Bild 1 h 2180, Bild 2 h 3256, also rund 300 ueber dem Scheitel |
| Bein FL | ganz verdeckt, Laenge und Haltung frei |
| Fussform, Zehen, Krallen | nicht erkennbar. Umriss Fuss A: Spitzen (377,362), (426,362), Sohle bis y=376, Richtung nicht ablesbar |
| Sohlen Bein NL und NR | nicht erkennbar, Abschnitt 1.2 |
| Beugung in Blickrichtung | bei allen Gliedern unbekannt |
| Oberflaechenart | glatt, Haut, Fell: nicht erkennbar unter rund 4 px |
| Grundhaut unter e~10 | nicht aufloesbar |
| Zone unter dem Logo | (0-253,436-480) in beiden Bildern |
| Umriss x=250-300 oben in Bild 1 | dunkel auf dunkel, +-10 px |
| Zwischen Kuppel und Unterstreifen rechts in Bild 2 | (300-345,300-320): Raum dort ebenso dunkel, Figur nicht pruefbar |

## 6. Konkurrierende Deutungen

Arbeitsannahme in **fett**. Sie ist die Deutung mit den wenigsten Widerspruechen, kein Beweis.

### A. Das vierte Bein

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Bein FL steht hinter Bein NR oder steckt in C1** | Viereck trifft (328,370), dort deckt C3. Der Punkt faellt auf die Achse von C1 (x~326). Eine Kopie von A hat dort die rechte Kante x=345 wie C1. Erklaert, warum nur drei Beine frei stehen: Beine NR und FL fluchten in der Kamera | Bein selbst nicht trennbar. Parallelogramm ist Annahme, die Fernbreite ist damit nicht gemessen. Ein Fuss wie A2 (43 px) passt erst ab Sohlenzeile 374 in die dunkle Spanne (299-341 bei y=362). Bei (328,370) stuende er 11 px ueber L1 vor |
| Innenglied B4 ist das vierte Bein | B4 ist ein eigener Streifen x250-285, dunkle Trennung x252-256 | Ergibt kein Viereck: Fuss laege je nach Bodenzeile 375-405 bei v 730-1480 statt 610, u -570 bis 480 statt 1140. Dann fehlt bei Seite L fern jede Stuetze |

### B. Was Bild 2 zeigt

Pruefmodell aus Bild 1 (Ellipsoide, in `formblatt.json`, nur Messhilfe). Suche ueber Standort und Gier, Ziel aus den Verdeckungen. a = Gier der Achse N nach F in (x,z).

| Sicht | a | Ueberdeckung | Dafuer | Dagegen |
|---|---|---|---|---|
| **Sicht Flanke (Seite R)** | 60-100 | 0,73-0,90 | Lage aus der Silhouette-Suche, nicht an Fuessen angepasst: K6 (376,368) trifft Tiefe-Kandidat F1 (370,368). K3 und K4 fallen in Flaeche Y, auch wenn Y aus dem Suchziel genommen wird: (238,419), (288,421). K2 (206,387) liegt unter Feuer, 35 px links der Unterkante (240,372), nicht pruefbar. Sehstrahl Z1 trifft bei Tiefe 4600-4800 den Haken (218-234,236-239). Scheitel (308,159). Zeichnung Ende N liegt abgewandt | Kuppel muss dann rund 2000 lang sein, also fast rund. Beine FR und FL stehen in Feuerzonen, Bein FR nicht pruefbar |
| Sicht Ende (Ende F) | 330-350 | 0,74-0,92 | Beste Kuppelform. Breite wie in Bild 1 (134 gegen 150 px bei gleicher Tiefe). Schulter faellt auf den Haken (222,254). Zeichnung Ende N abgewandt | Bein FL stuende bei (369,461), davor (345-405,382-396) sichtbarer Boden R 207: dort kein dunkles Bein. Bein FR stuende bei (256,474) und muesste das Band (210-250,376-384) kreuzen, dort R 162 gegen Masse R 50 |

Verworfen: Gleiches Ende wie Bild 1 (a 120-200). Ueberdeckung bei a 160-200 nur 0,64-0,84, bei a 120-150 aber bis 0,86: Der Grund ist also nicht die Silhouette, sondern Z1 bis Z4 waeren sichtbar. Innenflaeche (290-340,225-280) ist V (40,6,16), ein Band dieser Textur stuende dort als rund V (97,57,84). Gegenflanke (a 250-290): 0,63-0,80.
Die Silhouette allein trennt die beiden Sichten nicht. Mit dem vierten Bein liegen a 50-150 und 330-350 bei 0,73-0,77. Getrennt wird nur ueber Einzelstellen.
Die Fuss-Argumente beider Zeilen setzen die Pose aus Bild 1 und K2 bis K4 voraus, beides ist nicht belegt. Die Sichtfrage bleibt offen, Sicht Flanke ist nur Arbeitsannahme. Die Verwerfung der Zeichnungsseite haengt nicht an den Fuessen und bleibt.

### C. Welches Ende ist vorn

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Ende N vorn** | Zeichnung und Rosa dort. In Sicht Flanke zeigt es zur Frau. Gegenstand X liegt dort | Kein Gesicht belegt. Wahl haengt an Sicht Flanke |
| Ende F vorn | Kein Bildbefund nennt Ende F direkt. Bleibt moeglich, falls der Kopf nicht sichtbar ist | Dann waeren Rosa und Stapel Rueckseite |

### D. Der helle Stapel Z1 bis Z4

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Vorsprung am Ende N (Kopf oder Hals zur Kamera)** | Sehstrahl Z1 trifft bei u rund -1100 den Haken. Hoehe Z1 rund 430, Hakenhoehe rund 425 [s]. Rosa nur hier | Nur in Sicht Flanke. Kein Umriss sichtbar, da vor dunkler Kuppel. Haken ist rotbraun, nicht rosa: das passt nur, wenn Bild 2 die Seite zeigt |
| Mittelnaht auf der Rumpffront | Mitte x 320, Kuppelmitte x 315, Senkrechte ueber 150 Zeilen | Erklaert den Haken nicht |
| Oberglied von Bein NR | gleiche x-Spanne wie C1 und Quertrio, Gesamtbau nennt das | Glied y 221-440 waere rund 2550 lang, Bein FR bis Kuppe nur 2230 |

### E. Bau von Bein NR

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Durchgehendes Bein mit Taille** | Oberkante C3 trifft die Taille bei (325,345), Band A14 dort. Frei stehend zwischen L1 und L2 (sicher). Rechte Kante x 342-347 ohne Knick von y=320 bis y=368, die Taille gibt es nur links | Taille 470-620 bei Tiefe 5000-6900, duenner als Fuss 690-750 |
| Liegender Unterarm | Unterteil laeuft am Boden zur Kamera, Taille waere Ellenbogen h 390-640. Passt zu "eher liegend". Dann K3/K4 Hand, kein Fuss unter dem Rumpf | Dann ist das Fussviereck falsch, Laenge 2550 waere Armlaenge plus Rumpf. Am linken Bein keine Taille |

### F. Haken

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Profil des Vorsprungs** | wie Stapel-Vorsprung, sitzt an der Masse | Nur in Sicht Flanke. Anschluss oben offen |
| Schulter Seite R | Sicht Ende, Punkt (222,254) | Nur in Sicht Ende. Band A11 (387,243) landet 25-35 px unter der Hakenoberkante |
| Erhobenes Glied | Spitze zur Frau, gefuellt | Dann fehlt ein Bein am Boden oder es gaebe fuenf Glieder |

### G. Gegenstand X (250-288,420-479) in Bild 1

Kein Logo (W14), kein Boden (Cr +46), Kappe (97,55,110) und dunkles Band (46,28,53) rahmen ihn. Laeuft unten aus dem Bild.

| Deutung | Dafuer | Dagegen |
|---|---|---|
| Koerperteil am Boden | liegt mittig zwischen K2 und K3, rosa wie Z1, Rand koerperfarben | ragt, falls am Boden, bis Tiefe 3700, rund 500 vor K3 |
| Treffer- oder Blutwirkung | rot, nah an der Figur | keine Beobachtung dazu im Bild |
| **Offen lassen** | nichts entscheidet es | |

### H. Deckende Flaeche Y in Bild 2

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Fuss von Bein NR** | K3, K4 fallen in Sicht Flanke hinein, sind selbst aber offen. G und V-Form laufen auf ihren rechten Rand zu (278,425). Rosa Fleck (262-268,428-442) und B23 (257,385) am Rand | Rund 86 px Breite = 970 Einheiten bei Tiefe 5000, Rundling hoechstens 690-750. Band (210-250,376-384), R 162, trennt sie im Bild von U (R 50) |
| Eigenes Objekt des Prototyps | im Bild freistehend | Im Spielfenster nichts dergleichen |
| Schatten | rund, Kante 12-18 px | Fuge verschwindet ganz. Auch unter dem Plateau 88/88/96 bliebe sie sichtbar: Rot e 53-88 neben 11-19 |

### I. Unterstreifen G

| Deutung | Dafuer | Dagegen |
|---|---|---|
| **Helles Band auf Bein NR** | In Sicht Flanke verbinden Probe-Lagen von C1 und C3 genau diese Strecke. Bild 1 hat an gleicher Kante eine Lichtlinie (302,372)-(288,388) | Gipfel unregelmaessig bei 12, 24, 14, 34 px |
| Eigener Strang | 100 px lang, dünn, geknickt | Liegt auf der Kante der Masse, frei haengt nichts |

## 7. Pruefpunkte fuer ein Modell

Bild 1, Kamera `kamera_bild1.json`:

| Punkt | Soll |
|---|---|
| Sohle Bein FR | (398,374) |
| Kappe Spitze / Basis | (400,277) / (398,296) |
| Schulterkuppe | (405,238) |
| Scheitel | y=187 bei x 300-322 |
| Flanke rechts | (348,205), (366,215), (378,233) |
| Rand links | x=247 bei y 224-242 |
| Rand rechts | (429,272), kein Pruefpunkt unter y=278 |
| Bein FR Kanten | x 381-414 bei y=324, 381-418 bei y=344 |
| Bein NR Kanten | x 300-347 bei y=322, 308-344 bei y=346 |
| Bein NL Kanten | x 226-287 bei y 318-342 |
| Rundling | linke Kante (306,405), (312,414). Aussenkante nicht pruefen, sie ist weich |
| Boden frei | L1, L2, dazu (345-378,362-402) |
| Stange | sichtbar bis y=191 bei x=280 |

Bild 2, Kamera `kamera_bild2.json`:

| Punkt | Soll |
|---|---|
| Kante | belegte Punkte aus 1.4, gelesene nur als Anhalt |
| Figur deckt | V1 (222-298,292), V2 (232-285,347), V3 (340-364,338), V4 (408,225-262) |
| Boden frei | (345-405,382-396), Fuge bei x>=278, y 402-416 |
| Haken | (205-245,215-262) |

## 8. Grenzen dieses Blatts

- Das Pruefmodell ist grob. Es zeigt Vertraeglichkeit, keine Form. Seine Ueberdeckungen ueberlappen sich.
- Weltlage Bild 1 ist offen. Masse im Koerperrahmen haengen nur an Augenhoehe und Horizont.
- Bild 2: Tiefe der Figur nicht gemessen, alle Einheiten dort sind Spannen.
- Licht der Beta ist nicht belegt, nur vertraeglich mit dem Mantel der Frau und mit Leon.
- Neue Messwerte stammen aus dem Restbild Video minus gefilterter Hintergrund (`form_video.video_filter`). Filter-MAE dort 15-21, Einzelwerte unter rund 10 V sind Rauschen.
- Am Boden von Bild 1 heisst dunkel nicht Koerper, Abschnitt 1.2. Alle Sohlen ausser K1 sind offen, also auch O, u, v, Fussviereck und Gier.
- Bild 2 steht vor schwarzer Tuer: Links von x=354 gibt es oben keine gemessene Silhouette.

## 9. Dateien

| Datei | Inhalt |
|---|---|
| `formblatt.json` | alle Zahlen, Polygone, Pruefmodell, Pruefpunkte, Aenderungsliste |
| `formblatt_gegenprobe.md` | unabhaengige Pruefung mit Belegen |
| `formblatt_bild1.png`, `formblatt_bild2.png` | beschriftete Uebersicht |
| `werkzeug/formblatt_bilder.py` | erzeugt die Uebersichtsbilder |
| `lesehilfen/fb_b2_restrot_x3.png`, `fb_b2_restblau_x3.png` | Restbild: Verdeckung, Rand |
| `lesehilfen/fb_b2_untenlinks_video_raum_filter.png` | Flaeche Y gegen Raum |
| `lesehilfen/fb_b2_verdeckung_ncc_x2.png` | Verdeckungskarte |
| `lesehilfen/fb_b1_gegenstand_x8.png`, `fb_b1_stuetze_x8.png`, `fb_b2_haken_x8.png` | Ausschnitte |
| `lesehilfen/fb_logo_alle_dias_min_x3.png` | Wasserzeichen ueber alle Dias |
| `lesehilfen/gp_*.png` | Belegbilder der Gegenprobe |

## 10. Aenderungsliste

Aus der Gegenprobe, Belege in `formblatt_gegenprobe.md`.

| Nr | Stelle | Vorher | Jetzt | Kernbeleg |
|---|---|---|---|---|
| A1 | Kante Bild 2 rechts (1.4, 7) | (390,195)-(397,207)-(418,225), sicher | (378,175)-(390,181)-(394,187)-(411,206) | Rohr endet bei (411,206): R 194 bei y=204, 117 bei y=208. Alte Linie lag dort bei y=219, auf Band Q4 |
| A2 | Kante Bild 2 oben (0, 1.4, 3.2) | x258-418 sicher | belegt nur x>=354, davor gelesen | Tuer ist schwarz, dort keine Verdeckung |
| A3 | Sohlen K2, K3, K4 (0, 1.2, 3.1, W4) | wahrscheinlich | offen | Kantenbreite 9-26 px gegen 6-8 px am Koerper. K3 liegt auf C5, das offen ist |
| A4 | Fussviereck, Gier, Achse (1.2, 1.3, 3.1) | 2260-2850 x 1090-1430, 15-20 Grad, [g] | 1750-2850 x 750-2050, 15-27 Grad, Gier 65-197, [s] | Folge aus A3 |
| A5 | Rundling (1.4, 7, W7) | Umriss wahrscheinlich, Pruefpunkte auf der Aussenkante | Masse wahrscheinlich, Umriss offen, nur linke Kante pruefen | C4 rechts Y 21-25 px, unten 9-20 px, links 7 px |
| A6 | Bodenschatten (1.3, W18) | Kreis R 1170-1240, wahrscheinlich | nicht messbar | Grenze bis (900,-401), 1850 von der Mitte |
| A7 | Zone (345-378,362-402) (1.4, W7) | Bodenschatten | nicht Figur, Art offen | liegt ausserhalb des alten Kreises, Boden hat eigenes Tuerkis |
| A8 | Ganze Figur quer (1.3) | 3370 | 2150-2950, Arbeitswert 2350 | 233 px / 69 px je 1000 rechnet eine Tiefe, die Enden liegen auf 4580 und 6900. Pruefmodell selbst 2320 |
| A9 | Kuppel plus Schulter (1.3) | 2390-3140 | 1750-2400, Arbeitswert 2300 | wie A8 |
| A10 | Rand rechts Bild 1 (1.4, 7) | x=430 bei y=292 | (429,272), Kante nur y 238-278 | Zeilen 292, 296: Y 30-41 flach ueber x 400-456 |
| A11 | Haken oben (3.2, 7, W15, 6F) | oben getrennt, Luecke als Pruefpunkt | offen, Pruefpunkt gestrichen | V (33,6,13) +-(3,3,5) im Rauschen von Tuer und Rumpf |
| A12 | Linke Grenze Bild 2 (1.4, W8) | Grenze x218 | Mindestausdehnung | Balken endet selbst dort, Raum R 46-57 bei x 205-215 |
| A13 | Weisse Flaeche unten Bild 2 (1.4, W12) | Feuer | Boden | Video R 205-215, Raum 226 |
| A14 | Viertes Bein (0, 1.4, 6A) | ganz verdeckt | verdeckt oder in C1, Fuss wie A2 nur bei (321,374) | Spanne 299-341 bei y=362, Fuss A 43 px, Sohle 12 Zeilen tiefer |
| A15 | C1 und A1 Breite (1.4) | 36 gegen 34 px | je Zeile: C1 46 auf 35, A1 33 auf 37 | Halbwertkanten y=320-356 |
| A16 | Kopf (0, 5) | keine Erhebung in beiden Bildern | nur vor hellem Grund ausgeschlossen | Bild 1 x 250-276, Bild 2 x<354 dunkel auf dunkel |
| A17 | Farbton Grundhaut (0, 2.1) | wahrscheinlich | offen | `farbe.md` Abschnitt 7: Bild 1 im Rauschen, Bild 2 Rot bis +21 unsicher |
| A18 | Zug Q (2.2, 2.3) | Q2, Q3, Q5 getrennt | ein Zug, wahrscheinlich | Blau-Grat zeilenweise bis P, Stoerung nur y 242-254 |
| A19 | Z9 Schraegstrich (2.2) | fehlte | aufgenommen | (411,240)-(420,262), `schraegstrich_beule` aus Gesamtbau |
| A20 | Bauchkante (1.3, 3.1) | y=320, 890-1200 | L2 y=313, L1 y=322, 850-1310 | L2-Spitze: Y 77 bei y=312, 145 bei y=316 |
| A21 | Bein NR Ansatz h (3.1) | 870-1060 | 910-1260 | (325,318) bei Tiefe 6900-5000, wie bei C1 gerechnet |
| A22 | Neigung Bein FR (3.1) | unter 1 Grad | bis 3 Grad | Mitte 397,5 bei y=320, 399,5 bei y=348 |
| A23 | V4 (1.4, 7, W9) | x=405 | x rund 408 | Rohr liegt im Video 3 px links der Warp-Lage |
| A24 | Sichtfrage Bild 2 (3.2, 6B) | Fuss-Treffer als Beleg | nur Vertraeglichkeit | Pose kann anders sein, K2 bis K4 offen |
| A25 | Scheitel Bild 2 (3.2) | sicher als Stelle | wahrscheinlich | keine Verdeckung bei x=320 |
| A26 | Marken gleiche Hoehe (2.3) | gleiche Hoehe, wahrscheinlich | gleiche Bildzeile sicher, Hoehe offen | h folgt nur aus angenommener Tiefe |
