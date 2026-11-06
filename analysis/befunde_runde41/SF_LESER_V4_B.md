# SF V4 - Leser B: unabhaengige Lesung der Nutzer-Beschreibung (Beschreibung1/2)

Stand 2026-11-06, Runde 41, Spur SF. Leser B, eigener Code (re15_port/tools/r41_sichel/v4/b_*.py), eigene Masken.
SF_FORMBLATT_V2.md und SF_MESSBLATT_V3.md wurden fuer Werte NICHT gelesen. Nichts committet.
Belege: analysis/befunde_runde41/SF_belege/v4_lesung/B_* (Bilder + JSON). Laufausgaben: Sitzungs-Scratchpad sfv4/.

Konventionen: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens. Koordinaten in Pixeln des 640x480-Bildes
(x nach rechts, y nach unten). Lab = CIE-Lab D65 (skimage rgb2lab, sRGB). "-17" = Schwarzwert-Abzug max(v-17, 0) je Kanal.

## 0. Quellen, Bildnummern, H, Abblende

| Groesse | Wert | Beleg |
|---|---|---|
| Beschreibung1_Vorlage | = Videobild **n1485** (1-basiert wie SF_belege/roh/alle, h = n-1350 = **h135**); 0-basierter ffmpeg-Index 1484. MSE 0,21 (naechstes 9,6) | b_finde_bild.py; Gegenprobe roh/alle/f1485.png MSE 0,21 |
| Beschreibung2_Vorlage | = Videobild **n1435** (h85); 0-basiert 1434. MSE 0,53 (naechstes 340) | dito |
| Groesse der Beschriftungsbilder | Beschreibung1.png 972x480, Beschreibung2.png 832x480 (nicht 960/830) | PIL |
| Kamera | beide Bilder dieselbe Kamera (Cut 12): Wand links n1435 vs Mittel n1447..1471: k = 0,998/0,996/1,002, d < 0,5, rms ~3 | B_L2_L5.json abblende |
| **Abblende in Bild 1** | n1485 liegt in einer Ausblendung: Wand links y = k x + d mit k = **0,775 / 0,773 / 0,774** (R/G/B), d = 2,9/3,0/3,3, rms 2,5-2,8. Beginn n1475 (k 0,985), n1495 k 0,52. Bild 2 ist nicht abgeblendet. | B_L2_L5.json, B_blende.json |
| Gegenprobe Abblende | abblende-korrigierte Wand Bild 1 (156,0/156,1/159,7) = Wand Bild 2 (156,3/156,3/160,3), dE00 < 0,5 | B_L4.json |
| Schwarzwert | Letterbox oben n1435 = 16,6, n1485 = 15,8 (abgeblendet) | B_L2_L5.json |
| **H1** (Bild 1) | **299 px**: Oberkante y 140 (x 382, n1485, erste Zeile mit 5 Zeilen R-G >= 8), Sohle y 439 = Median der Unterkanten der roten Fussbloecke in n1475..1477 (439/441/439). In n1485 liegt die Sohle im Schatten/Abblende (kein Fussblock messbar). Tiefe gleich: Oberkante n1475/1476 = 144 (4 px Unterschied -> H1 Unsicherheit etwa +-4 px). | B_H.json, B_H_oberkante_sohle.png |
| **H2** (Bild 2) | **255 px**: Oberkante Buckel y 180 (x 402, n1435), Sohle y 435 = Median der Fussblock-Unterkanten n1438..1445 (433..435, x 428..435). In n1435 verdeckt der gruene Vordergrund die Fuesse. Tiefe gleich: Oberkante n1441 = 180. | dito |

## L1 Linien (beide Bilder)

Linienfarbe: alle Differenzpixel Beschriftung minus Vorlage haben exakt RGB (237,28,36) (3798 bzw. 6712 Pixel, keine
Kantenglaettung im Bildteil). Maske: Chebyshev |rgb - (237,28,36)| <= 40. Gegenprobe: Maske im Bildteil = Differenzmaske,
307200/307200 Pixel gleich. Enden = Skelett-Enden (skimage skeletonize, Sporne < 8 px verworfen); Start = Skelett-Ende im
weissen Rand neben dem Absatz (Absaetze = Textbloecke mit Zeilenabstand <= 10 px; gemessen innerhalb 10, zwischen >= 12).
Belegung = Anteil der Abtastpunkte (0,25 px) der Strecke Start->Ende mit Linienpixel im Radius 1. Zuordnung = Permutation
mit groesster Summe. Belegbilder: B_L1_linien_bild1_x3.png, B_L1_linien_bild2_x3.png, Masken B_L1_linienmaske_bild*.png.

### Bild 1 (Vorderansicht)

Drei getrennte 8-Komponenten (je eine Linie, keine Kreuzung): Auge = Komponente 1 (Start (660,112) und Ende (382,165)),
Haare = 2 ((660,138), (403,160)), Nase = 3 ((664,166), (394,177)). Die Komponenten-Trennung ist hier der harte Beleg.

| Text \ Ende | (403,160) | (382,165) | (394,177) |
|---|---|---|---|
| Auge (Polylinie, s.u.) | 0,953 | **1,000** | 0,929 |
| Haare | **1,000** | 0,946 | 0,338 |
| Nase | 0,349 | 0,316 | **1,000** |

- **Auge -> Ende (382,165)**. Die Linie hat einen Haken: gerade bis (396,143), Bogen ueber (386,150), dann senkrecht nach
  unten bis zur Spitze (382,165). Polylinie (approxPolyDP 2 px): (660,112)-(641,112)-(396,143)-(386,150)-(382,165),
  Belegung 1,000; die gerade Strecke Start->Spitze nur 0,384 (wegen des Hakens). Die Alternativen der Polylinien-Zeile
  sind hoch (0,95/0,93), weil nur die letzte Teilstrecke wechselt; Teilstrecke ab der Ecke (386,150): zur Spitze 1,000,
  zu (403,160) 0,295, zu (394,177) 0,232. Endpunkt = Spitze des Hakens (Strichende).
- **Haare -> (403,160)**, Belegung 1,000; beste Alternative (382,165) 0,946 (liegt fast in der Verlaengerung der
  Haare-Linie; die Haare-Komponente endet aber bei x 401 und erreicht x 382 nicht).
- **Nase -> (394,177)**, Belegung 1,000; beste Alternative (403,160) 0,349.
- Kreuzungen: keine.

### Bild 2 (Seitenansicht)

Komponenten: Auge-Linie und Haare-Linie getrennt; die drei Schwanzlinien bilden eine Komponente (zwei Kreuzungen).

| Text \ Ende | (459,235) | (467,241) | (357,243) | (290,308) | (294,344) |
|---|---|---|---|---|---|
| Auge von der Seite etwa hier | **1,000** | 0,548 | 0,198 | 0,311 | 0,234 |
| Haare von der Seite | 0,609 | **1,000** | 0,496 | 0,139 | 0,238 |
| Schwanzspitze Ende | 0,297 | 0,162 | 0,113 | 0,189 | **1,000** |
| Schwanz Beginn in den Ruecken ueberfuehrend. | 0,299 | 0,255 | **1,000** | 0,179 | 0,176 |
| Schwanz Knick, von da an wird es breiter nach oben | 0,207 | 0,182 | 0,135 | **1,000** | 0,136 |

- Auge von der Seite -> **(459,235)** (Start (651,177)); Alternative 0,548.
- Haare von der Seite -> **(467,241)** (Start (668,221)); Alternative 0,609.
- Schwanzspitze Ende -> **(294,344)** (Start (660,256)); Alternative 0,297.
- Schwanz Beginn -> **(357,243)** (Start (664,309)); Alternative 0,299.
- Schwanz Knick -> **(290,308)** (Start (675,385)); Alternative 0,207.
- Kreuzungen (gewaehlte Strecken): Spitze x Beginn bei (545,5/283,5), Spitze x Knick bei (373,9/324,8), beide im Bildteil.
  Die Linien "Spitze" und "Knick" kreuzen sich also: die Spitze-Linie endet LINKS UNTEN (294,344), die Knick-Linie
  links (290,308) - nicht umgekehrt (Belegung der vertauschten Wege 0,189 bzw. 0,136).

## L2 Was am Endpunkt liegt

Am unbeschrifteten Vorlagebild. Struktur: L* der Mitte (3x3) gegen den Ring (15x15 ohne 9x9); Schwelle = Mittelwert;
Region = 8-Komponente auf der Seite der Mitte im 25x25-Fenster. Werte 9x9-Fenster (alle Varianten und 15x15 in
SF_belege/v4_lesung/B_L2_L5.json). Bild 1 zusaetzlich "korr" = Abblende herausgerechnet ((v - d)/k je Kanal).
Nachbarbilder mit Markierung: B_L2_nachbarn_bild1_kopf_x4.png (n1480..1490), B_L2_nachbarn_bild2_x2.png (n1430..1440).

### Bild 1 vorn (Kopfbox s. L5: x 366..415, y 140..199, Mitte (390,5/169,5), 50 x 59 px)

| Ende | Struktur | Region (bbox, Groesse, Huelle) | 9x9 roh RGB / Lab | 9x9 -17 | 9x9 korr | Lage |
|---|---|---|---|---|---|---|
| Auge (382,165) | HELL, aber schwach: L* 28,3 gegen Ring 25,8 (+2,4); Endpunkt 1,4 px innerhalb des Randes, oben rechts (B) an einem hellen Fleck | x 373..387, y 160..173; 15 x 14 px = 0,050 x 0,047 H; 114 px, Huelle 110,5 px2; Schwerpunkt (378,2/167,0) | (74,6/63,4/68,0) / (28,1/5,7/-1,0) | (57,6/46,4/51,0) / (20,5/6,0/-1,0) | (92,5/78,2/83,7) / (34,8/7,0/-1,0) | x/B 0,32, y/H 0,42; 8,5 px (0,028 H) links (B) = rechts (W) und 4,5 px ueber der Kopfmitte |
| Haare (403,160) | dunkel, fast kontrastlos (12,1 gegen 12,5); dunkler Streifen rechts (B) neben dem hellen Fleck, unter der roten Kuppe | x 391..408, y 150..172, 18 x 23 px (0,060 x 0,077 H), 231 px, Huelle 293; stoesst an den Fensterrand (Streifen laeuft weiter) | (40,1/28,9/30,6) / (12,1/5,8/1,0) | (23,1/11,9/13,6) / (4,3/4,6/0,8) | (48,1/33,5/35,3) / (14,8/7,3/1,6) | x/B 0,74, y/H 0,34; 12,5 px (0,042 H) rechts (B) = links (W), 9,5 px (0,032 H) ueber der Mitte |
| Nase (394,177) | neutral (14,7 gegen 14,1, +0,6); am Rand einer mitteldunklen Flaeche zwischen hellem Fleck und Streifen | x 382..398, y 165..183, 17 x 19 px, 183 px, Huelle 198,5; stoesst an Fensterrand | (45,0/34,0/35,6) / (14,5/5,6/0,9) | (28,0/17,0/18,6) / (6,2/5,5/0,9) | (54,3/40,0/41,8) / (17,8/7,0/1,4) | x/B 0,56, y/H 0,63; 3,5 px rechts (B), 7,5 px unter der Mitte |

Abstaende: Auge->Haare 21 px rechts (B), 5 px hoeher (21,6 px = 0,072 H); Auge->Nase 12 rechts, 12 tiefer (17,0 px =
0,057 H); Haare->Nase 9 links, 17 tiefer (19,2 px = 0,064 H).

Stabilitaet in den Nachbarbildern (Kontrast Mitte-Ring L*): Auge n1481..1484 = -1,2/-2,0/-1,7/-2,4, n1485 +2,6,
n1486..1489 = +5,4/+7,8/+10,1/+11,9; der helle Fleck wandert in den Folgebildern auf den Punkt (das Wesen bewegt sich).
Haare n1481..1489 durchweg -0,2..-3,0 (dunkel); Nase -2,4..+3,2 (nicht stabil). Das Bild loest an Haare und Nase keine
eigene Form auf; das Auge ist ein heller Fleck von etwa 15 x 14 px am linken (B) Kopfrand.

Hinweis zur Seite: Auge liegt links (B) der Kopfmitte = rechts (W), Haare rechts (B) = links (W). Der Nutzer-Satz von
20:35 ("das Auge ist weiter links und rechts ist noch etwas haare drueber") passt zum Bild in der Betrachter-Lesart (B).
Das ist eine Beobachtung, keine Entscheidung.

### Bild 2 seitlich

| Ende | Struktur | Region | 9x9 roh RGB / Lab | 9x9 -17 |
|---|---|---|---|---|
| Auge von der Seite (459,235) | DUNKEL (20,5 gegen 29,9, -9,4) - dunkler Fleck am vorderen (rechten, B) Ende einer hellen Flaeche (x 440..458, y 226..244, L* 47,5); Endpunkt 1 px innerhalb des Randes | x 447..466, y 223..247, 20 x 25 px (0,078 x 0,098 H), 261 px, Huelle 421,5; stoesst an Fensterrand | (58,6/54,7/56,2) / (23,4/2,1/-0,4) | (41,6/37,7/39,2) / (15,5/2,2/-0,4) |
| Haare von der Seite (467,241) | Endpunkt liegt genau an der Grenze zwischen einem dunklen Streifen (links) und dem hellen gelb-weissen Pfeiler (rechts); die Mitte 3x3 ist schon Pfeiler (39,1 gegen 29,3) | Der dunkle Streifen: Abweichung vom stehenden Pfeilerhintergrund >= 12 L, x 455..468, y 229..249, 196 px (14 x 21 px = 0,055 x 0,082 H). Am Punkt (464,241): n1435 = 36 L gegen 67..74 in n1381..1431 und n1439..1461 -> etwas Dunkles vom Wesen liegt dort, nur in n1433..1437. | (78,6/78,3/58,9) / (32,8/-3,6/11,6) (Fenster enthaelt den Pfeiler) | (61,6/61,3/41,9) / (25,4/-3,7/12,0) |

Auge->Haare: 8 px nach rechts (B), 6 px tiefer; 10,0 px = 0,039 H. Blickrichtung: Kopf rechts, Schwanz links -> das Wesen
blickt nach Bild-rechts (B); im Profil waere damit die rechte Koerperseite (W) zur Kamera. Wie weit es zur Kamera gedreht
ist, loest das Bild nicht auf. Damit liegt das Haar-Ende VOR (W: vorn) und etwas unter dem Auge - passend zu einem
gesenkten Kopf, dessen Schaedeldach nach vorn zeigt (Lesart, nicht gemessen). Belegbild der Pfeiler-Abweichung:
B_L5_seite_abweichung_pfeiler_x4.png.

Stabilitaet (Kontrast Mitte-Ring): Auge n1433..1437 = -10,3/-9,9/-9,4/-9,1/-6,8 (stabil dunkel), Haare +9,6..+9,8 (Pfeiler).
n1431/1432/1438/1439 weichen ab (Bewegung/Vordergrund).

## L3 Schwanz (Bild 2, n1435)

### Textlesung (Polygonzug aus den drei Linienenden)

- Beginn (357,243) -> Knick (290,308): 93,3 px = **0,366 H**, Richtung 135,9 Grad im Bild (nach links unten).
- Knick (290,308) -> Ende (294,344): 36,2 px = **0,142 H**, Richtung 83,7 Grad (fast senkrecht nach unten, leicht rechts).
- Richtungsaenderung am Knick 52,2 Grad (Innenwinkel 127,8 Grad).
- Austritt am Ruecken: y 243 = 192 px = **0,753 H ueber der Sohle** (Sohle 435); laengs: Beginn liegt an der Unterkante des
  Buckels, 45 px = 0,176 H links (B) bzw. hinter (W) dem hoechsten Buckelpunkt (x 402), 5,8 px rechts (B) des Schwerpunkts
  (351,2/243,7) des dunklen Flecks dort (x 345..361, y 231..255, dunkel, -3,9 L*).
- Knick: y 308 = 0,498 H ueber der Sohle. Ende: y 344 = 0,357 H ueber der Sohle.

### Masken (Gegenprobe, B_L3_schwanz_maske_x4.png)

- M1 (rote Flaeche): Chroma R-(G+B)/2 >= 6 im Fenster x 262..362, y 200..300, Oeffnung 1, Komponente durch (300,255): 3241 px.
  Sie bildet ein BOGEN-Band vom Ruecken (x 362, y 206..238) nach links (y 242..258 bei x 290..335) und dann nach unten
  (x 287..299, y 258..290). Chroma im Band 9..26, ausserhalb 0..4 (Probe-Tabelle n1435, Raster 3 x 4 px, Konsolenausgabe dieser Lesung; Maske im Belegbild).
- M2 (bewegter unterer Teil): |L(n1435) - L(n1439)| >= 10, Fenster x 272..312, y 286..352, Schliessung 2, Teile >= 15 px: 544 px.
  Begruendung: entlang x 292, y 322..344 ist n1435 = 32/30/26/24/35, in n1410..1474 sonst 42..46/14..17/15..17/30..33/69..71
  -> dort liegt nur in n1433..1437 etwas (der untere Schwanzteil), danach nicht mehr.
- Maskenachse (kuerzester Weg, Kosten 1/(1+Abstand)): Beginn->Knick 137,4 px (Sehne 93,3), Knick->Ende 40,4 px (Sehne 36,2).
  **Die Maske verlaeuft nicht auf der Sehne Beginn->Knick, sondern als Bogen bis 31,2 px (0,122 H) links-oben (B) der Sehne**:
  vom Beginn erst nach links (fast waagerecht, Achse (342,234) -> (319,243)), dann nach links unten ((297,258)), dann
  senkrecht nach unten ((291,283) -> (279,304)) zum Knick. Die gerade Textlinie schneidet durch den dunklen Tuerraum
  (x 320..350, y 250..290; dort Chroma 3..4 = Hintergrund, in n1410..1440 unveraendert). Beides berichtet, nicht geglaettet.
- Anschluss der Textpunkte an die Maske: Beginn 5,0 px, Knick 2,8 px, Ende 0,0 px.
- Am Knick endet das rote Band (Chroma >= 6 bis y ~290) und es beginnt der schmale, kaum rote untere Teil (M2).

### Breite senkrecht zur Maskenachse (Schnitt durch die geschlossene Maske; zweite Spalte 2 x Abstandstransformation)

Knick -> Beginn ("von da an wird es breiter nach oben"):

| f (Knick=0 -> Beginn=1) | Achspunkt | Breite px | Breite H | 2*DT px |
|---|---|---|---|---|
| 0,1 | (279,304) | 8,0 | 0,031 | 6,3 |
| 0,3 | (291,283) | 15,0 | 0,059 | 11,7 |
| 0,5 | (297,258) | 33,0 | 0,129 | 21,3 |
| 0,7 | (319,243) | 32,5 | 0,127 | 30,0 |
| 0,9 | (342,234) | 53,0 | 0,208 | 22,6 |

f 0,9 ist aufgeblaeht: dort geht das Band ohne Kante in den Buckel ueber (Maske oben bis y 206 = Ruecken); belastbar ist
dort nur 2*DT = 22,6 px (0,089 H) als Untergrenze.

Knick -> Ende (verlangt in der Aufgabe):

| f (Knick=0 -> Ende=1) | Achspunkt | Breite px | Breite H | 2*DT px |
|---|---|---|---|---|
| 0,1 | (285,313) | 16,0 | 0,063 | 7,2 |
| 0,3 | (285,320) | 10,0 | 0,039 | 8,5 |
| 0,5 | (289,326) | 3,5 | 0,014 | 2,8 |
| 0,7 | (291,334) | 4,0 | 0,016 | 2,8 |
| 0,9 | (291,342) | 6,5 | 0,025 | 5,7 |

Lesung "breiter nach oben": Der Schwanz wird vom Knick zum RUECKEN hin breiter (8 -> 15 -> 33 px, 0,03 -> 0,13 H).
"Oben" ist in Bildkoordinaten die Richtung Beginn (y kleiner) und am Wesen die Richtung zur Wurzel am Ruecken (W: nach
oben-hinten). Zwischen Knick und Ende (das Ende liegt UNTER dem Knick) gibt es keine Verbreiterung nach oben: dort
3,5..16 px (0,014..0,063 H), die letzten 10 % 6,5 px (0,025 H; Fleck am Ende 25 x 14 px, x 282..306, y 332..345, dunkel
-3,6 L*). Form der Spitze: das Bild zeigt einen kleinen dunklen Fleck, keine aufgeloeste Spitze -> Spitzenform nicht
aufloesbar. Unterer Teil (M2) ist eine Bewegungsmaske, keine Silhouette: Breiten dort sind Groessenordnung, nicht Kante.

## L4 Farben

Alle Regionen mit Fenster/Maske in B_L4.json (Spalte "beschreibung"), eingefaerbt in B_L4_regionen_bild1_x3.png,
B_L4_regionen_bild2_x3.png. Port V3 r2 vorn = Spielbild des Laufs sfv3/r/V3r2 (RE15_FRAMEDUMP, nb2neu/c6.port,
registriert auf Videokoordinaten), h135; Regionen B_L4_regionen_port_V3r2_h135_x2.png. Port-Kopfbox von Hand x 338..378,
y 155..213 (B_L4_port_kopf_lupe_x6.png; die automatische Oberkante traf im Port den Lampenrand); Kopf-Merkmale im Port an
denselben Relativlagen wie im Video. Port-Schwanz: Leiste v3/01_seite, Spalte V3, Zeile h85 (Buehnen-Render, nicht
Spielbild; Band +-7 px um die gelbe Mittellinie, Linienfarben entfernt, B_L4_port_schwanz_leiste_h85.png) - das
Spielbild V3r2 h85 zeigt eine andere Pose ohne sichtbaren Schwanz. Der Port blendet bei h135 NICHT ab (Wand 138 konstant
h85..h135), daher Vergleich Port gegen Video "korr".

### Bild 1 vorn (n1485); RGB-Mittel, Lab, (Streuung RGB-std)

| Region | n | roh RGB / Lab | -17 Lab | korr RGB / Lab | korr -17 Lab | Port V3r2 h135 RGB / Lab | dE00 Port-korr (Port-roh) |
|---|---|---|---|---|---|---|---|
| Kopf-Kuppe (x 376..405, y 140..152) | 234 | (76,0/48,2/49,4) / (23,3/12,9/4,5) std 14-15 | (15,6/13,7/5,1) | (94,4/58,5/59,5) / (28,9/16,0/6,0) | (21,5/16,8/6,6) | (103,0/80,0/76,3) / (36,3/9,0/5,9) | 8,2 (10,7) |
| Augenumfeld (15x15 um (382,165)) | 225 | (71,7/60,4/64,8) / (26,8/5,8/-0,9) | (19,1/6,1/-0,9) | (88,7/74,2/79,4) / (33,2/7,1/-0,8) | (25,8/7,4/-0,8) | (86,9/72,0/76,2) / (32,2/7,1/-0,2) | 0,9 (4,4)* |
| Auge-Struktur (heller Fleck, 2 px erodiert) | 47 | (98,5/86,5/91,5) / (38,1/5,9/-1,1) | (30,9/6,1/-1,1) | (123,3/108,0/113,9) / (47,2/7,1/-1,1) | (40,3/7,3/-1,1) | - | - |
| Haare (L2-Region, 2 px erodiert) | 81 | (32,0/22,9/24,4) / (8,8/4,9/0,7) | (2,2/2,8/0,3) | (37,5/25,7/27,3) / (10,6/6,2/1,2) | (3,3/4,2/0,8) | (86,5/76,4/78,5) / (33,5/4,7/0,3) | 16,3 (17,3) |
| Nase (9x9) | 81 | (45,0/34,0/35,6) / (14,5/5,6/0,9) | (6,2/5,5/0,9) | (54,3/40,0/41,8) / (17,8/7,0/1,4) | (9,7/7,5/1,7) | (81,1/49,5/53,0) / (24,5/14,9/3,8) | 8,3 (10,8) |
| Maul/Kinn dunkle Zone (x 378..398, y 186..194, L*korr<13) | 172 | (34,5/19,6/19,8) / (8,0/7,7/2,8) | (1,8/4,2/1,4) | (40,8/21,5/21,4) / (9,6/9,8/4,2) | (2,9/6,4/2,3) | (31,8/19,4/20,2) / (7,5/6,5/1,8) | 3,8 (1,4) |
| Brust/Rumpf hell (x 295..465, y 160..250, a*<10, L*>=P70) | 3377 | (76,8/67,9/67,0) / (29,7/3,6/1,9) | (22,1/3,7/2,0) | (95,3/84,0/82,4) / (36,8/4,3/2,7) | (29,5/4,5/2,8) | (97,8/87,8/85,2) / (38,2/3,5/3,0) | 1,6 (7,0) |
| Brust/Rumpf rot (dito, a*korr>=15) | 996 | (54,6/24,9/27,7) / (13,1/15,0/4,8) | (5,5/14,4/4,1) | (66,6/28,4/31,5) / (16,2/18,9/6,6) | (8,7/19,4/7,1) | (104,2/42,4/41,9) / (26,2/27,7/14,1) | **9,1** (12,2) |
| Oberarm (B) rechts = (W) links (x 452..482, y 185..225) | 999 | (41,3/29,7/31,7) / (12,6/6,1/0,9) | (4,7/4,8/0,8) | (49,6/34,5/36,7) / (15,4/7,6/1,4) | (7,2/7,9/1,5) | (80,0/57,5/56,2) / (26,6/9,7/4,7) | 8,4 (10,6) |
| Oberarm (B) links = (W) rechts (x 305..340, y 180..230) | 1504 | (53,0/44,2/44,3) / (19,0/4,0/1,4) | (11,0/4,3/1,6) | (64,7/53,3/53,0) / (23,5/5,0/2,1) | (15,7/5,3/2,3) | (80,7/48,0/46,7) / (23,8/14,8/7,1) | 9,1 (10,8) |
| Unterarm/Klaue rot (R-G>18, x 380..515, y 330..365) | 792 | (44,9/27,2/30,3) / (12,2/9,4/1,4) | (4,5/7,7/1,3) | (54,2/31,3/35,0) / (15,0/11,8/2,2) | (6,9/12,3/2,4) | (67,7/20,8/20,8) / (14,4/22,6/12,0) n=56 | 8,9 (10,8) |
| Jeans (x 365..430, y 265..320, b*<-2) | 1757 | (25,3/28,5/32,5) / (10,4/-0,4/-3,3) | (3,1/-0,1/-2,1) | (28,8/33,0/37,8) / (12,5/-0,6/-3,9) | (4,6/-0,2/-3,2) | (42,3/49,6/59,4) / (20,3/-0,4/-7,2) | 5,8 (7,3) |
| Bezug Boden (x 40..250, y 380..440) | 11799 | (51,7/53,3/57,7) / (22,3/0,4/-3,0) | (14,4/0,4/-3,2) | (63,0/65,1/70,3) / (27,6/0,4/-3,5) | (19,9/0,4/-3,7) | (55,1/53,2/60,8) / (22,7/2,6/-4,4) | 4,7 (3,2) |
| Bezug Wand links (x 20..180, y 120..300) | 27789 | (123,8/123,6/126,9) / (52,0/0,7/-1,8) | (45,2/0,7/-1,8) | (156,0/156,1/159,7) / (64,5/0,6/-1,9) | (58,0/0,7/-1,9) | (147,5/147,3/151,6) / (61,2/0,9/-2,2) | 2,8 (8,5) |
| Bezug Wand rechts (x 520..630, y 160..320) | 16799 | (93,4/98,0/107,9) / (41,5/0,4/-6,2) | (34,4/0,5/-6,4) | (116,7/123,0/135,1) / (51,5/0,3/-7,4) | (44,7/0,3/-7,6) | (111,7/115,8/127,3) / (48,8/0,9/-6,8) | 2,9 (7,0) |

\* Augenumfeld-Port ist ein Fenster an der Relativlage (351,180) im Port-Kopf (dort Stirn/Auge des Port-Kopfbilds);
die kleine Abweichung ist ein Mittelwert ueber 13x13 px und sagt nichts ueber die Form.

Weissabgleich-Bezug: Port-Wand/-Boden gegen Video-korr dE00 2,8 / 2,9 / 4,7 (Port etwas dunkler, Boden leicht roetlicher
a* 2,6 gegen 0,4). Am Wesen liegen die Abweichungen darueber: Rot (Brust) Port L* +10, a* +8,8, b* +7,5 (Video-Rot ist
dunkler und blaeulicher: Farbton h = atan2(b*,a*) Video 19 Grad, Port 27 Grad); Arme Port deutlich roter
(a* 9,7..14,8 gegen 5,0..7,6) und heller; Klauen im Port gesaettigter (a* 22,6 gegen 11,8) und gelber (b* 12 gegen 2,2);
Jeans im Port heller und blauer (L* 20,3 / b* -7,2 gegen 12,5 / -3,9); Haare im Port hell (L* 33,5) statt fast schwarz (10,6).

### Bild 2 seitlich (n1435, nicht abgeblendet)

| Region | n | roh RGB / Lab (RGB-std) | -17 Lab | Port | dE00 |
|---|---|---|---|---|---|
| Kopf: Auge-Umfeld (9x9 um (459,235)) | 81 | (58,6/54,7/56,2) / (23,4/2,1/-0,4) std 27 | (15,5/2,2/-0,4) | - | - |
| Kopf: helle Flaeche vor dem Auge (x 440..458, y 226..244, L*>30) | 78 | (120,1/110,3/111,6) / (47,5/4,0/0,7) | (40,6/4,2/0,7) | - | - |
| Haare seitlich (dunkler Streifen, 1 px erodiert) | 115 | (58,4/55,3/52,4) / (23,4/0,7/2,3) std 26-28 (Randmischung mit Pfeiler) | (15,5/0,8/2,4) | - | - |
| Buckel/Rumpf hell (x 330..440, y 185..300, a*<10, L*>=P70) | 1213 | (110,8/94,8/94,3) / (41,7/6,3/2,7) | (34,6/6,6/2,8) | - | - |
| Buckel/Rumpf rot (a*>=15) | 230 | (114,1/72,6/80,7) / (35,8/18,9/2,6) | (28,7/19,6/3,0) | - | - |
| Jeans (x 380..450, y 280..340, b*<-2) | 968 | (29,6/32,9/39,4) / (12,6/0,3/-4,9) | (4,6/0,4/-4,2) | - | - |
| Schwanz nahe Ruecken (Maske, Kreis r15 um (357,243)) | 164 | (47,0/26,9/31,0) / (12,5/10,7/1,3) | (4,7/9,2/1,3) | (54,5/45,3/44,6) / (19,6/4,0/2,0) | 8,3 |
| Schwanz breiter Teil (Maske, Kreis r12 um (308,250)) | 415 | (38,1/23,5/24,4) / (9,9/7,5/2,3) | (2,9/4,9/1,4) | (56,0/40,2/40,7) / (18,1/7,5/2,5) | 5,3 |
| Schwanz am Knick (Kreis r8 um (290,308)) | 27 | (53,7/47,2/45,9) / (20,1/2,7/1,9) | (12,1/2,9/2,0) | (56,7/37,9/40,9) / (17,5/9,4/1,6) | 7,4 |
| Schwanz Spitze (M2, Kreis r8 um (294,344)) | 73 | (51,1/42,8/43,3) / (18,3/3,9/1,1) | (10,2/4,2/1,2) | (57,9/49,7/53,8) / (21,7/4,5/-1,2) | 3,2 |
| Bezug Boden | 11799 | (65,2/65,8/71,5) / (28,0/1,1/-3,5) | (20,4/1,1/-3,7) | Spielbild h85 (22,7/2,6/-4,4) | 4,4 |
| Bezug Wand links | 27789 | (156,3/156,3/160,3) / (64,6/0,8/-2,1) | (58,1/0,8/-2,1) | (61,2/0,9/-2,2) | 2,9 |

Schwanz ohne Chroma-Auswahl (Kreis r5 um die Achsstationen, B_L4_schwanz_kreise.json) - Gegenprobe, weil M1 nach Chroma
ausgewaehlt ist und a* damit nach oben verzerrt:
Knick->Beginn f0,1 (279,304) Lab (15,3/0,3/1,2); f0,3 (291,283) (8,6/6,1/2,5); f0,5 (297,258) (13,3/8,4/3,0);
f0,7 (319,243) (9,3/8,1/1,7); f0,9 (342,234) (9,2/13,0/1,4). Knick->Ende f0,1 (22,0/2,4/1,8); f0,3 (19,7/2,2/1,4);
f0,5 (12,7/3,8/0,8); f0,7 (9,5/4,9/1,6); f0,9 (14,4/2,7/1,4).
Lesung: oberer/breiter Schwanzteil dunkel rotbraun (L* 9..13, a* 6..13, b* 1..3), zur Wurzel am Ruecken roeter
(a* 13); unterer Teil ab dem Knick grau-dunkel und kaum rot (a* 2..5). Port-Schwanz (Buehne) ist im oberen Teil deutlich
heller (L* 18..20 gegen 9..13) und weniger rot an der Wurzel (a* 4,0 gegen 10,7).

## L5 Kopfform

### Vorn (n1485)

- Kopfbox: x 366..415, y 140..199 -> **50 x 59 px = 0,167 x 0,197 H, B/H 0,85**. Seiten = Spalten, deren Oberkante
  hoechstens 10 px unter dem Minimum (140) liegt (links davon Schulter mit Oberkante 152..160, rechts 168); Unterkante =
  Median der Spalten, in denen a* (5x5) die halbe Hoehe zwischen Gesichts-Plateau und Brust-Rot erreicht (y 199).
- Profil durch die Kopfmitte (x 382..398, Werte roh, B_L2_L5.json "profil"): y 140..152 rote Kuppe (a* bis 17,7 bei y 148,
  L* 16..28) - die Kuppe ist ROT, nicht hell; y 158..180 Gesichtszone grau-rosa (a* 4..6, L* 15..18); heller Fleck (Auge)
  links (B) bei x 373..387, y 160..173 (L* bis 38 roh / 47 korr); y 184..194 dunkelste Zone (L* 7,6..9,6); ab y 196 steigt a*
  zum Brust-Rot (a* 14 ab y 204).
- Maul: der Nutzer hat kein Maul beschriftet. Das Bild zeigt unter der Nase eine dunkle Zone: bei L*korr < 11 ein Fleck
  13 x 13 px (x 378..390, y 186..198, 88 px, Fuellgrad 0,52); bei Schwelle 13 verschmilzt sie mit der Umgebung (38 x 28 px),
  bei 15 mit der halben Kopfbox -> keine stabile Kante, Form NICHT AUFLOESBAR (B_L5_maulzone.json). In n1481..1489 ist
  die Zone y 186..192 durchweg 2,9..8,9 L* dunkler als die Gesichtszone y 164..178. Ob das ein Maul, eine Kieferkante
  oder ein Schatten ist, gibt das Bild nicht her. Rund-ovale Form ist nicht zu sehen.

### Seitlich (n1435)

- Vorderkante des Kopfes: x 468 (rechtes Ende des dunklen Streifens vor dem Pfeiler, Abweichung vom Hintergrund >= 12 L,
  bbox x 455..468, y 229..249). Hoehe der sichtbaren Kopfvorderseite 21 px = 0,082 H.
- Helle Flaeche vor dem Auge x 440..458, y 226..244 (L* 47,5), Auge-Ende (459,235) am vorderen Rand davon (dunkler Fleck).
- Hinterkante, Oberkante, Unterkante des Kopfes: NICHT AUFLOESBAR - der Kopf geht ohne Helligkeits- oder Farbsprung in
  Buckel und Schulter ueber; eine seitliche Kopfbox laesst sich deshalb nicht messen, nur die Vorderseite (s.o.).
- Rote Zone seitlich: Buckel/Rumpf rot (a* >= 15) nur in kleinen Flecken (230 px; laut B_L4_regionen_bild2_x3.png oben am
  Buckel um x 385..440, y 190..215 und ein Fleck um (375,260)); der Kopf selbst ist im Seitenbild nicht rot
  (a* Auge-Umfeld 2,1, helle Flaeche 4,0, Haarstreifen 0,7).

## Offene Punkte / nicht aufloesbar

1. Sohle in beiden Vorlagebildern verdeckt (Bild 1 Schatten + Abblende, Bild 2 Vordergrund) - H aus Nachbarbildern derselben
   Kamera (Bild 1: n1475..1477, Bild 2: n1438..1445), Tiefengleichheit ueber die Oberkante geprueft (4 px bzw. 0 px).
2. Bild 1 ist abgeblendet (k 0,774). Wer "EXAKT seine Farben" aus Bild 1 nimmt, muss "korr" verwenden, sonst sind alle
   Farben um etwa 23 % zu dunkel. Die Korrektur ist an der Wand gegen Bild 2 bestaetigt (dE00 < 0,5).
3. Form von Haare und Nase in Bild 1: Kontrast am Punkt 0,4 bzw. 0,6 L*, Regionen laufen ueber das 25x25-Fenster hinaus -
   nur Lage und Farbe belegt, keine Umrissform.
4. Spitze des Schwanzes: nur ein dunkler Fleck 25 x 14 px; keine Spitzenform. Der untere Teil ist nur ueber die
   Bewegung (n1435 gegen n1439) abgrenzbar.
5. Breite am Uebergang in den Ruecken (f 0,9): ohne Kante zum Buckel, nur Untergrenze 22,6 px (0,089 H).
6. Textlesung und Maske weichen beim oberen Schwanzteil ab (Bogen bis 0,122 H neben der geraden Linie Beginn->Knick).
   Der Nutzer hat Punkte gesetzt, keinen Verlauf; der Verlauf zwischen Beginn und Knick folgt im Bild dem roten Bogen.
7. Port-Schwanzfarben stammen aus dem Buehnen-Render der Leiste (JPEG, Messlinien entfernt), nicht aus einem Spielbild;
   ein Spielbild V3r2 mit sichtbarem Schwanz in der Videopose gibt es im Lauf nicht (h85 andere Pose).
8. Seitliche Blickrichtung: Profil nach Bild-rechts gemessen; Drehung zur Kamera nicht aufloesbar -> "rechte Seite (W)"
   gilt nur unter der Profil-Annahme.

## Werkzeuge (Leser B, re15_port/tools/r41_sichel/v4/)

b_finde_bild.py (Bildsuche), b_linien.py + b_linien_lib.py (Maske, Skelett-Enden), b_zuordnung.py (Belegungstabelle,
Kreuzungen, Belegbild), b_komponenten.py (Komponenten-Zugehoerigkeit), b_blende.py (Abblende-Reihe), b_hoehe.py (H),
b_common.py, b_messen.py (L2, L5, Abblende), b_schwanz.py (L3), b_farben.py (L4 inkl. Port), b_belege.py (Kreisproben,
Nachbarbilder, H-Beleg), b_maulzone.py (L5 dunkle Zone).
