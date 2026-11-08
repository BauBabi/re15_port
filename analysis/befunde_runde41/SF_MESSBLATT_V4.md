# SF Messblatt V4: Kopf, Schwanz und Farben der Sichel-Form nach der Nutzer-Beschreibung (Pruefer, 2026-11-06)

Kein RE-Befund: die Sichel-Form gibt es auf der Disc nicht. Alles hier ist Bildmessung am 80%-Video 4 (Hangar, ROOM50A0
Cut 12) an den beiden Bildern, die der Nutzer beschriftet hat. Keine Zahl hier ist eine @0x-Konstante. Unter
Version_80%/, pri/, synchro/ ist nichts geaendert.

Grundlage: SF_LESER_V4_A.md und SF_LESER_V4_B.md (vollstaendig gelesen) und eigene Nachrechnung mit eigenem Code
(re15_port/tools/r41_sichel/v4/p_*.py). Belege des Pruefers: analysis/befunde_runde41/SF_belege/v4_pruefung/ (P_*),
der Leser: SF_belege/v4_lesung/ (A_*, B_*).

| Pruefer-Werkzeug | misst | Ausgabe |
|---|---|---|
| p_linien.py | Linienzuordnung: Maske exakt RGB (237,28,36), Skelett-Enden (Zhang-Suen), Belegung je Text x Ende, alle Permutationen mit Kreuzungen | P_linien.json, P_linien_Beschreibung{1,2}.png |
| p_bildsuche.py | Videobild der Vorlagen (ffmpeg-Index 1300..1549, mittlere Abweichung) | P_bildsuche.json (Scratchpad sfv4/pruefer) |
| p_kopf.py | Helligkeitsprofil entlang jeder Kopf-Linie ueber das Ende hinaus | P_kopf_profile.json |
| p_blob.py | Auge-/Haarfleck: Regionwachstum ab dem Extrem, Schwelle = Mitte aus Extrem und Umgebung | P_kopf_flecken.json, P_kopf.png |
| p_farben.py | Farbe je Region (21 Regionen, beide Bilder) inkl. Port-Schirm-Ziel | P_farben.json |
| p_riegelfenster.py | Zielfarben der kopfrelativen Riegel-Fenster | P_riegelfenster.json |
| p_schwanzfarbe.py | Schwanzfarbe mit Bewegungsmaske gegen den Hintergrund | P_schwanzfarbe.json, P_schwanz_maske.png, P_schwanz_knick_unten.txt |
| p_eichung.py | Eichung Video <-> Port-Schirm am gemeinsamen Raumhintergrund | P_eichung.json |
| p_port_ist.py | Port V3 r2 (Spielbild h135) gegen die Ziele, im Videoraum | P_port_ist.json |
| p_belege.py | Belegbilder | P_linien_*.png, P_kopf.png, P_schwanz.png |

## 1. Quelle und Konventionen

**Die Vorgabe ist die Nutzer-Beschriftung** auf den Original-Videobildern (Nutzer 2026-11-06 ~22:00: "Ich will EXAKT
diese Umsetzung und auch EXAKT seine Farben"). Die KI-Bilder (birkin01-24) sind keine Vorlage mehr. Sie dienen hoechstens
als Zweitquelle, wo das Original nichts aufloest, und gelten nie gegen die Beschriftung. In diesem Blatt kommt keine
KI-Zahl vor.

| Begriff | Wert | Beleg |
|---|---|---|
| Vorlage 1 (vorn) | Beschreibung1_Vorlage.png = Videobild **n1485** (1-basiert), **h135**; ffmpeg-Index 1484. Abweichung 0,149, naechstes Bild 1,497 (n1484) | p_bildsuche; A und B gleich |
| Vorlage 2 (seitlich) | Beschreibung2_Vorlage.png = **n1435**, **h85**; ffmpeg-Index 1434. Abweichung 0,304, naechstes 6,72 | dito |
| Zaehlung | n 1-basiert, h = n - 1350 (Formblatt V2 Abschnitt 0, R1 = n1435 = h85). **Leser A nennt den 0-basierten Index n1484/1434 und daraus h134/h84: um 1 zu klein.** | Formblatt V2 Abschnitt 0 |
| Kamera | beide Vorlagen = **Spielkamera 50A0 Cut 12** (ein Schnitt von n1351 bis zur Ausblendung; Wand links n1406..1431 gegen n1435: Abweichung 0,0) | Wand-Tabelle je 5 Bilder (Pruefer-Lauf) |
| Beschriftungsbilder | Beschreibung1.png 972x480, Beschreibung2.png 832x480 (nicht 960/830). Linke 640 Spalten = Vorlage + Linien; alle Differenzpixel sind exakt RGB (237,28,36): 3798 bzw. 6712 | p_linien; A und B gleich |
| Seiten | **(B)** = aus Sicht des Betrachters, **(W)** = aus Sicht des Wesens. Vorn gilt (B)-links = (W)-rechts. Seitlich blickt das Wesen nach Bild-rechts (+x); die Kamera sieht die (W)-rechte Flanke | Formblatt V2; Kopf-Vorderkante x 465 |
| Abblende Vorlage 1 | n1485 liegt in der Ausblendung (ab ~n1475, schwarz ab ~n1525). Fit an Wand links, Boden, Wand rechts gegen Median n1441..1471, Schwarzwert 17: **k = 0,766 / 0,764 / 0,768** (R/G/B), RMSE 2,0-2,4. Entblenden: I' = 17 + (I - 17) / k. Gegenprobe: Wand links entblendet 155,8/157,3/160,7 = Vorlage 2 156,0/156,9/161,1 (dE < 0,5) | eigener Fit; A 0,766 (gleich); B 0,775 (freier Achsenabschnitt, Schwarz ~10; Unterschied ~1 % in den Farben) |
| Schwarzwert | Letterbox 14,9-16,6; Schwarzwert 17 wie vorgegeben | eigene Messung |
| **H1** (Vorlage 1) | **302 px +-4**: Oberkante y 139 (+-1, erste Zeilen mit Abweichung vom Hintergrund in x 370-415), Sohle y 441-443 = Unterkante roter Fuss in n1475..1479 (in n1485 verdeckt; gleiche Tiefe angenommen) | Pruefer 303; A 304; B 299 |
| **H2** (Vorlage 2) | **253 px +-3**: Buckel-Oberkante y 185 (+-2, Spalten x 380-410), Sohle y 435-437 aus n1439..1445 (in n1435 vom gruenen Vordergrund verdeckt) | Pruefer 251; A 253; B 255 |
| Farbraum | Lab = CIE-Lab D65 aus sRGB (OpenCV bzw. skimage, gleiche Werte); dE00 = CIEDE2000 (skimage) | |

## 1a. Linienzuordnung (eigene Nachrechnung)

Belegung = Anteil der Abtastpunkte (0,5 px) der geraden Strecke Start -> Ende mit Linienpixel im 3x3-Nachbarn.
Start = Skelett-Ende im weissen Rand, Text = Zeilenblock rechts daneben (Zeilen y 106-118 / 134-143 / 161-170 bzw.
171-200 / 212-224 / 259-289 / 317-366 / 379-449).

| Bild | Text | Start | Ende (Pruefer) | Belegung | beste Alternative | A / B Ende | Entscheid |
|---|---|---|---|---|---|---|---|
| 1 | Auge | (659,112) | **(382,164)** | gerade 0,375; **gleiche 8er-Komponente** (Haken: gerade bis ~(396,143), Bogen, senkrecht nach unten) | (404,160) 0,380, andere Komponente | (381,167) / (382,165) | Komponente entscheidet; alle drei gleich |
| 1 | Haare | (659,138) | **(404,160)** | 1,000 | (382,164) 0,950 (Verlaengerung, andere Komponente) | (401,161) / (403,160) | Komponente entscheidet |
| 1 | Nase | (663,166) | **(395,177)** | 1,000 | (404,160) 0,345 | (392,178) / (394,177) | eindeutig |
| 2 | Auge von der Seite etwa hier | (651,177) | **(460,234)** | 1,000 | (468,241) 0,542 | (457,236) / (459,235) | eindeutig |
| 2 | Haare von der Seite | (667,221) | **(468,241)** | 1,000 | (460,234) 0,549 | (465,242) / (467,241) | eindeutig |
| 2 | Schwanzspitze Ende | (660,256) | **(295,343)** | 1,000 | (290,308) 0,189 | (292,345) / (294,344) | eindeutig |
| 2 | Schwanz Beginn in den Ruecken ueberfuehrend | (663,308) | **(357,243)** | 1,000 | (468,241) 0,265 | (355,242) / (357,243) | eindeutig |
| 2 | Schwanz Knick, von da an wird es breiter nach oben | (674,384) | **(290,308)** | 1,000 | (460,234) 0,198 | (288,307) / (290,308) | eindeutig |

Permutationen Bild 2: die gewaehlte Zuordnung hat Mittel 1,000 / Minimum 1,000 mit 2 Kreuzungen (die drei Schwanzlinien
kreuzen sich, wie der Nutzer sagt); die beste andere 0,818 / 0,542 (Auge und Haare getauscht). Bild 1: 0,792 (wegen
Haken) gegen 0,776, entschieden ueber die Komponenten (drei getrennte 8er-Komponenten, je eine Linie). Unter 90 % liegt
nur die gerade Strecke der Auge-Linie in Bild 1; sie ist ein Haken und als Komponente eindeutig. **A und B stimmen in
allen 8 Zuordnungen mit dem Pruefer ueberein**; die Endpunkte weichen um 1-3 px ab (Skelett gegen Endpixel).

**Die Linien sind Zeiger vom Text zum Merkmal, keine Konturen.** Daraus folgt fuer den Schwanz: Beginn, Knick und Ende
sind die drei beschrifteten Punkte; der Verlauf dazwischen kommt aus dem Bild (Abschnitt 4), nicht aus der Geraden
zwischen den Linienenden.

## 2. Kopf vorn (Vorlage 1, n1485 entblendet, H1 = 302)

Kopfbox (A, mit Pruefer-Oberkante): x 368-416, y 139-191 = 48 x 52 px = **0,159 x 0,172 H1** (B: 0,166 x 0,195 H1;
die rechte Kante (B) ist im dunklen Haarfeld nicht aufloesbar und an der Nase gespiegelt). Kopfmitte x 392 = Nase x 395
+-3. Kopfoberseite: **rote Zone** x 388-406, y 140-156 (R-G >= 15, 171 px); links davon grau-mauve. Keine helle Kuppe:
der hellste Kopfpunkt ist der Augenfleck.

| Merkmal | Lesung (Pruefer) | Lage | Groesse | Farbe (Lab, entblendet) | A / B |
|---|---|---|---|---|---|
| **Auge** | **heller, blass rosa-grauer Fleck** am Linienende. Profil entlang der Linie: am Ende L 63,8, 2-4 px weiter (Zeigerichtung) 71,5-73,2, danach Abfall auf 30 | Schwerpunkt **(376,9 / 167,2)**: **0,050 H1 (B)-links = (W)-rechts** der Kopfmitte; 0,093 H1 unter der Kopf-Oberkante; zur Nase dx -0,060 / dy -0,032 H1 | Schwelle 96,4 (Extrem 128,6, Umgebung 64,2): **8 x 10 px = 0,026 x 0,033 H1**, 56 px; kein Ring, keine Pupille aufloesbar | **46,9 / 7,2 / -1,1** (Schwarzwert ab: 40,0 / 7,4 / -1,1) | A 10 x 12 px, entbl.+SW 39,9 / 8,0 / -1,2; B 15 x 14 px, 47,2 / 7,1 / -1,1. Gleiche Aussage, Groesse schwellenabhaengig |
| **Haare** | **dunkles Feld** (B)-rechts = (W)-links neben dem Auge, unter der roten Zone, nach unten offen | x 393-409, y 152-178 (A-Blob 394-408 x 154-176); Mitte ~(401,165): 0,08 H1 (B)-rechts vom Auge, auf Augenhoehe | 15-17 x 23-27 px = 0,05 x 0,08 H1 | Fenster KV2: **13,3 / 7,4 / 1,5**; dunkelster Kern (L < 35): 9,2 / 6,2 / 0,9 | A 10,1 / 5,8 / 0,8 roh; B 10,6 / 6,2 / 1,2 |
| **Nase** | mitteldunkle Stelle ohne Kante zwischen Haarfeld und Auge, mittig-unten | Linienende (395,177): 0,01 H1 (B)-rechts der Kopfmitte, 0,126 H1 unter der Oberkante | nicht aufloesbar (Kontrast 0,6-3,7 L) | Fenster KV3: **18,2 / 7,1 / 1,3** | A: Leiste +3,7 Lum; B: kontrastlos. Gleich |
| zweites Auge | nicht beschriftet; an der Spiegelstelle (Kopfmitte + 0,05 H1) liegt das dunkle Haarfeld | - | - | - | A und B gleich |
| Maul | **nicht beschriftet**; unter der Nase eine dunkle, rotstichige Zone ohne stabile Kante | Fenster KV4 x 378-398, y 185-194 | nicht aufloesbar (B: 13 x 13 px bei L* < 11, verschmilzt ab 13) | **9,9 / 9,9 / 3,8** | A 12,8 / 7,4 / 2,5 roh; B 9,6 / 9,8 / 4,2 |

Sinn der Beschriftung, gemessen: Das eine sichtbare Auge sitzt (W)-rechts. Ueber die (W)-linke Gesichtshaelfte faellt ein
dunkles Haarfeld, das das zweite Auge verdeckt. Das deckt sich mit dem Nutzer-Satz von 20:35 "das Auge ist weiter links
und rechts ist noch etwas Haare drueber" in Betrachter-Lesart (B).

## 3. Kopf seitlich (Vorlage 2, n1435, H2 = 253)

| Merkmal | Lesung (Pruefer) | Lage | Groesse | Farbe (Lab) | A / B |
|---|---|---|---|---|---|
| **Auge** | **heller grau-rosa Fleck**. Das Linienende (460,234) liegt auf einem 2-6 px schmalen dunklen Streifen (L 31,9); in Zeigerichtung 6-10 px dahinter steigt L auf **121-130**: der Fleck | Schwerpunkt **(452,5 / 237,2)**: 0,049 H2 hinter der Kopf-Vorderkante (x 465), 0,206 H2 unter der Buckel-Oberkante, **0,79 H2 ueber der Sohle** | 10 x 14 px = **0,040 x 0,055 H2**, 77 px | **48,4 / 4,0 / 0,5** | **A: heller Fleck (wie Pruefer). B: dunkler Fleck am Linienende, verworfen** (siehe unten) |
| **Haare** | **dunkler Streifen an der Kopf-Vorderkante**, unmittelbar vor dem Augenfleck. Die Haare-Linie zeigt in ihn hinein: am Ende L 106 (gelber Pfosten), 2-6 px weiter **35-41**, 10-14 px weiter 106 (unterer Augenfleck) | x 458-465, y 228-248; 0-2 px Abstand zum Augenfleck | Breite je Zeile 2-6 px (0,008-0,024 H2), Hoehe 21 px (**0,083 H2**) | Streifen (L < 40): **12,6 / 0,6 / 1,4**; Fenster KS2 (ohne Maske): 19,1 / 1,2 / 1,1 | A 18,1 / 1,0 / 2,6 (mit Rand); B 23,4 / 0,7 / 2,3 (mit Pfosten) |
| Kopfoberseite | **rote Zone** x 418-441, y 193-211 (R-G >= 25: 236 px) | 0,03-0,10 H2 unter der Buckel-Oberkante | ~23 x 18 px | **28,7 / 17,8 / 4,9**; Fenster KS1: 27,6 / 15,3 / 3,3 | A gleich (28,8 / 17,8 / 5,0). **B "Kopf seitlich nicht rot" verworfen**: B misst die Gesichtszone, nicht die Oberseite |
| Kopf unter dem Auge | dunkel-grau | Fenster KS3 x 436-461, y 250-262 | - | 16,5 / 2,2 / 0,5 | A 16,6 / 2,0 / 0,3 |

**Entscheid A gegen B (Auge seitlich):** B nennt den dunklen Fleck am Linienende das Auge. Dieser dunkle Streifen ist aber
das Ziel der Haare-Linie, und B's eigene Haar-Region (x 455-468, y 229-249) enthaelt B's Augenpunkt (459,235). In Bild 1
endet die Auge-Linie genauso am Rand des hellen Flecks und zeigt in ihn hinein (Profil +2..+4 px: 71-73 gegen 64 am
Ende). Damit ist das Auge in beiden Ansichten der **helle Fleck** (Lab 47-48 / 4-7 / -1..0,5). Beide Ansichten passen
zusammen: das sichtbare Auge ist das (W)-rechte, die Haare haengen ueber die (W)-linke Gesichtshaelfte und bilden von der
Seite die dunkle Vorderkante.

## 4. Schwanz (Vorlage 2, n1435, H2 = 253)

Die drei Nutzerpunkte (Abschnitt 1a): **Beginn (357,243)** an der Unterseite des Ansatzes, wo der Schwanz in den
Ruecken uebergeht; **Knick (290,308)**; **Ende (295,343)**. Der Verlauf dazwischen kommt aus den Masken von A
(R-G >= 5 bzw. |I - Platte| >= 40) und B (Chroma >= 6, Bewegung n1435 gegen n1439). Beide sind gegengeprueft:
Zeilenprofile, Bewegungsmaske gegen Median n1449..1473 (P_schwanz.png, P_schwanz_maske.png).

| Mass | Vorgabe (Bildmass, Kamera 50A0/12) | Beleg |
|---|---|---|
| Beginn | (357,243): **0,767 H2 ueber der Sohle**, 0,229 H2 unter der Buckel-Oberkante, **0,15 H2 hinter** dem Buckelscheitel (Plateau x 380-410, Mitte 395). Die Schwanz-Oberseite geht dort ohne Kante in den Buckel ueber (y ~196-205); Ansatzmitte ~0,15 H2 unter der Oberkante (wie Formblatt V2 #5) | A 0,779 / 0,221; B 0,753 |
| Verlauf Beginn -> Biegung | Achse fast **waagerecht nach hinten** (A: Richtungen 149-173 Grad, x 356 -> 298, y 231-247); die Gerade Beginn -> Knick schneidet dagegen durch den Tuerraum (Hintergrund) | A, B gleich; Pruefer-Zeilen y 231-247: Rotband x 298-355 |
| **Biegung** | ~(295,252): dx **-0,245 H2**, dy +0,036 H2 vom Beginn; die Achse dreht dort um **~70-90 Grad nach unten**, innerhalb von <= 0,10 H2 Achslaenge | A (295,250) ~70 Grad; B Bogen bis 0,122 H2 neben der Sehne; Pruefer-Zeilen y 247 -> 263: Rotband x 298-334 -> 292-301 |
| Biegung -> Knick | nahezu **senkrecht** (A: 84-96 Grad), x 289-292 | A, Pruefer |
| **Knick** | (290,308): dx **-0,265 H2**, dy **+0,257 H2** vom Beginn, **0,51 H2 ueber der Sohle**. Hier ist die **engste Stelle (Taille)**: 6-8 px = 0,024-0,032 H2; die Aussenkante wechselt die Richtung | A engste 6-7 px; B Ende des roten Bogenbands |
| **"breiter nach oben"** | vom Knick **zum Ruecken hin** breiter (nicht Knick -> Ende). Breite quer zur Achse bei Anteil f der Achse Knick -> Beginn: f 0,1 **0,03 H2** (7-8 px); f 0,3 **0,065 H2** (15-18 px); f 0,5 **0,125 H2** (31-33 px); f 0,7 **0,14 H2** (32-38 px); f 0,9 geht ohne Kante in den Buckel (>= 0,09 H2) | A 7 / 17 / 31,5 / 38; B 8 / 15 / 33 / 32,5; Pruefer waagerechte Zeilen y 271 / 287: 19 / 20 px |
| Knick -> Ende | 36-38 px = **0,145 H2**, Richtung 82-84 Grad unter der Waagerechten = fast senkrecht, 6-8 Grad wieder nach vorn | A 38,2 px / 84,0 Grad; B 36,2 px / 83,7 Grad |
| Breite unter dem Knick | **0,035 H2** (9 px), Bereich 0,025-0,05 H2; etwa gleich breit, keine messbare Verjuengung | Pruefer Bewegungsmaske y 312-344: 8-12 px (direkt unter dem Knick 14); A 6-9 px; B 16 / 10 / 3,5 / 4 / 6,5 (Bewegungsmaske lueckenhaft) |
| **Ende** | (295,343): dx -0,245 H2, dy +0,397 H2 vom Beginn, **0,37 H2 ueber der Sohle**. Spitzenform nicht aufloesbar (letzte 10 % 6-13 px, stoesst an die helle Bodenkante y 348) | A, B, Pruefer |
| Achslaenge | Beginn -> Knick 137-140 px = **0,55 H2**; Knick -> Ende 0,145 H2; gesamt **0,70 H2** (Sehnen 0,369 + 0,145) | A 140,1 / 188,1 px; B 137,4 |

**Farbe Schwanz (Videoraum, roh, Vorlage 2):** zwei Toene, oben beleuchtet, unten dunkel-rot.
- Wurzel/Unterseite (Kreis r12 um (345,238), Chroma >= 6): **9,3 / 11,4 / 1,5** (A 11,6 / 10,0 / 1,0; B 12,5 / 10,7 / 1,3)
- Bogen breit, Unterseite (Kreis r10 um (305,250), Chroma >= 6): **8,7 / 8,4 / 2,2** (B 9,9 / 7,5 / 2,3)
- Bogen, beleuchtete Aussenseite (Bewegungsmaske, Chroma < 6, 835 px): **26,8 / -0,1 / 2,4** = neutrales Dunkelgrau
- Bogen gesamt (Bewegungsmaske, 1859 px): 22,4 / 3,1 / 2,2
- unter dem Knick (Bewegungsmaske x 280-300, y 312-345, 222 px): **15,2 / 3,2 / 1,6** (Chroma-Fenster 13,5 / 3,6 / 1,3;
  A Spitze 18,8 / 3,5 / 1,2; B 18,3 / 3,9 / 1,1)
- Achtung: Der helle senkrechte Streifen bei x 278-285, y 205-250 ist **Hintergrund** (L 76-85 in n1353-n1425 und
  n1449-n1473 ohne Schwanz). Wer dort Schwanzfarbe misst, misst den Tuerrahmen. Die Pruefer-Region "Schwanz Bogen
  Aussenseite hell" in P_farben.json ist deshalb verworfen; es gilt der Wert mit Bewegungsmaske.

Gesamtbild: dunkles Mauve-Grau bis Dunkelrotbraun, an der Wurzel am roetesten (a* ~11), zur Spitze grauer (a* ~3).
Kein Oliv, kein Lachsrot.

**Darstellung in der Feder-Kette (engine/src/schwanz_r41.c, Knochen 16-22, 7 Glieder) und im Gang:**
1. **Ruhe-Pose = Vorlage 2.** Das Keyframe-Ziel der Kette (Ruhe = Keyframe, keine Schwerkraft) muss in der Kamera von
   Vorlage 2 die Punkte Beginn, Biegung, Knick und Ende nach der Tabelle oben treffen (Riegel T1-T6).
2. **Gliederteilung:** Die Biegung (70-90 Grad auf <= 0,10 H2 Achse) und die Taille am Knick muessen auf Gelenke fallen.
   Bei 7 gleich langen Gliedern (~0,10 H2 je Glied bei 0,70 H2) muss das Biegegelenk allein 50-70 Grad koennen. Das
   verbietet heute SW_GLATT_MAX = 16 Grad (Glaettung je Gelenk fuer die hohen Wurzeln V1/V2/V3). Diese Grenze ist
   GESTALTUNG; sie muss so gesetzt werden (oder die Glieder an der Biegung kuerzer), dass die Ruhe-Pose die gemessene
   Biegung zeigt. Vorschlag zur Gliederung: Glied 1-2 waagerechter Ansatz (Beginn -> Biegung, 0,25 H2), Glied 3-5
   senkrecht bis zum Knick (0,25-0,30 H2), Glied 6-7 duennes Endstueck (0,145 H2).
3. **Querschnitt:** Breite je Glied nach der Breitenzeile (0,14 -> 0,125 -> 0,065 -> 0,03 H2 zum Knick, darunter
   0,035 H2). Die Taille am Knick ist die engste Stelle der ganzen Kette.
4. **Gang:** Die Feder schwingt um die Ruhe-Pose. Im Spiel bei h85 (Spielkamera wie Vorlage 2) liegen die vier Punkte
   mit der doppelten Ruhe-Toleranz (Riegel T7). SW_HANG_COS_MIN und SW_HANG_RAND duerfen den waagerechten Ansatz
   (Glied 1-2, 0-15 Grad abwaerts, also 75-90 Grad von der Senkrechten) nicht in die Senkrechte druecken.

## 5. Farben EXAKT: Ziel je Region und Umrechnung auf Texel

> **UEBERHOLT (2026-11-07, Vorgabe des Orchestrators "V4 Originaltexturen" nach dem Nutzer: "Bei der Sichel Form V4, warum sind denn
> da die Farben so extrem scheisse? ... die Form baut von Farben und Texturen her ja nun wirklich auf die Urspruengliche Birkin Form
> auf").** Die Farbeichung der Texel an den Videobildern (Umrechnung unten, F1-F19 als Texelziel, die Riegel F*, FS, FO, FR, AF, O7, O9,
> O10, T8/T9 und die Video-Farbriegel TQ/TN/TV/TR) gilt nicht mehr. Sie trug Unschaerfe, Kompression und Szenenlicht des Videos in die
> Texel (fleckige, zu rote Flaechen, weisse Sprenkel, gelbe Ausreisser). Es gilt jetzt:
> (1) Koerper = Referenz 2 unveraendert (Texel, CLUT, UV bytegleich zu EM30_HANGAR.EMD sha256 cfd1ac42);
> (2) Schwanz: Textur aus der Koerperhaut desselben Ref2-Atlas (Oberarm/Ruecken), UV entlang der Achse durchgehend, keine neue Farbe;
> (3) Kopf: Ref2-Atlas-Farben, Merkmale (Auge, Haare, Nase, rote Oberseite) an den Orten der Abschnitte 2/3/7 (K1-K10, S1-S4 gelten
> weiter, Vergleichsregel nur noch als Messverfahren), gemalt nur mit Farben der Ref2-Palette in der Machart der Original-Textur.
> Riegel: re15_port/tools/r41_sichel/v4/riegel_ref2tex.py (T-K1 bis T-K4 und die Form-Riegel), Dossier SF_sichel.md Abschnitt
> "V4 Originaltexturen". Der Text unten bleibt als Beleg der Runden 1-5 stehen.
>
> **Nachtrag Originaltexturen Runde 2 (2026-11-07, Sicht-Pruefung Runde 1 und Orchestrator Runde 2 (a)):** Auch die Zonenhelligkeit des
> Kopfes darf NICHT an Video-L*-Ziele nachgefuehrt werden - die Video-Farbziele der Kopfzeilen in Abschnitt 7 (K2 46,9/7,2/-1,1, K5
> 13,3/7,4/1,5, K7 18,2/7,1/1,3, K8 9,9/9,9/3,8, K9 22,6/15,4/5,2, S1 48,4/4,0/0,5, S3 19,1/1,2/1,1, S4 27,6/15,3/3,3, KV5 18,9/7,1/0,6)
> sind ueberholt. Abnahme gegen Referenz 2, nicht gegen das Video: Kopf-Grundton = die blasse Ref2-Haut, dunkel nur das Haarfeld neben dem
> Auge, keine violettstichigen Eintraege. Ersatzbedingungen und T-K5 siehe Abschnitt 7.
>
> **Nachtrag Kopf nach Nutzerbeschreibung, Runde 3 (2026-11-07, Nutzer nach Ansicht des Kopfes der Runde 2: "da ist links ein Auge. Linke
> Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?"):**
> Der Kopf besteht aus drei Zonen in BILDKOORDINATEN der Spielkamera 50A0 Cut 12 (wie Beschreibung1): (1) linke Bildhaelfte graeuliche Haut mit
> dem Auge (heller unbunter Fleck, Ort K4), bis zur Nase (K7 = Zonengrenze); (2) rechte Bildhaelfte Haare, braeunlich (a* > 0, b* > 0, nicht
> schwarz; etwa 25 % dunkler als der Kopf-Median - Video Haar 12,9 bei Median 17,3), eine Flaeche mit weichem, strukturfolgendem Rand;
> (3) Kopfoberseite dunkel rotbraun, hoechstens Kopf-Median + 6 L* (Video Kuppe 22,6 bei Median 17,3), nahtlos zu den Haaren. Die Video-Lab-
> Werte dieses Blatts (F1-F5, F11-F13 und die Kopfzeilen in Abschnitt 7) sind nur noch Richtung (grau / braun / dunkelrot) und Ort; Texel werden
> nicht an sie geeicht. Farbquelle: die Ref2-Palette (Zeile 480; Klassen grau / braun / kuppe / hell in ref2tex_v4.klassen_r3), keine
> Ableitungen noetig. Riegel: riegel_ref2tex.py Runde 3 (T-K4 umgeschrieben, T-K9 Zonen, T-K8 Aenderungsumfang), Dossier SF_sichel.md
> Abschnitt "V4 Kopf nach Nutzerbeschreibung (Runde 3)".

**Eichung Video <-> Port-Schirm** (p_eichung.py): Der Port zeigt in 50A0 Cut 12 dasselbe Raumbild. Fit je Kanal
Video = a + b x Port ueber statische Wand- und Bodenfelder (x < 270 oder x > 540, y 60-440), den Port vorher mit Gauss
sigma 1,5 weichgezeichnet (bei sigma 1,5 kleinster Fehler: RMSE 8,9-9,4 je Pixel; ohne Weichzeichnung 12,5-13,1):
**a = 9,42 / 11,96 / 10,94, b = 0,9979 / 0,9901 / 0,9964.** Gegenprobe: Wand links Port (geeicht) gegen Video
**dE00 0,5**. A fand an 4 Feldern der JPEG-Leiste Video ~ 13,7 + 0,94 x Port (gleiche Richtung).

**Vergleichsregel (Bauer und Pruefer gleich):** Port-Render 640 x 480 -> GaussianBlur sigma 1,5 -> V' = a + b x P je
Kanal -> Lab -> dE00 gegen das Ziel unten. Kleine Merkmale (Auge 8 x 10 px) sind im Video weichgezeichnet; der Vergleich
nach derselben Weichzeichnung ist deshalb Pflicht.

**Umrechnung auf Texel** (Schirmfarbe = Texel x Licht):
1. Port-Schirm-Ziel P = (V - a) / b je Kanal (Spalte "Schirm-Ziel" unten; fuer Vorlage 1 aus den entblendeten Werten).
2. Licht je Region: Der Bauer rendert dieselbe Pose mit einem **weissen Pruefkoerper** (alle Texel des Meshes =
   CLUT-Weiss 248/248/248, PSX-15-Bit) in derselben Kamera mit RDT-Licht Cut 12. W = Schirmwert je Region (ungeglaettet,
   Regionsmittel), Licht lambda = W / 248 je Kanal.
3. Texel-Ziel T = P / lambda je Kanal, auf die 15-Bit-Stufe gerundet (Vielfache von 8), in die 8-bpp-CLUT des Meshes.
4. Kontrolle nach dem Bau mit der Vergleichsregel. Liegt dE00 ueber dem Riegel, T nachfuehren (die Weichzeichnung mischt
   kleine Merkmale mit der Umgebung, dann muss der Kern kraeftiger sein als das Ziel).

**Ziele** (V = Videoraum; Vorlage 1 entblendet; Fenster in p_farben.py, p_riegelfenster.py, p_schwanzfarbe.py):

| Nr | Region | Bild | Fenster | Ziel Lab (V) | Schirm-Ziel RGB P | Lab P |
|---|---|---|---|---|---|---|
| F1 | Kopf rote Kuppe (KV1, ohne Maske) | 1 | x 387-406, y 140-156 | **22,6 / 15,4 / 5,2** (mit R-G >= 15: 24,2 / 19,8 / 5,4) | 78,1 / 34,1 / 39,6 (Maske) | 19,6 / 21,2 / 6,2 |
| F2 | Auge vorn (heller Fleck) | 1 | p_blob, 56 px | **46,9 / 7,2 / -1,1** | 113,7 / 96,4 / 102,9 | 42,6 / 8,2 / -1,1 |
| F3 | Haarfeld vorn (KV2) | 1 | x 393-409, y 152-178 | **13,3 / 7,4 / 1,5** | 25,0 / 11,5 / 14,1 (Kern L < 35) | 4,5 / 5,7 / 0,8 |
| F4 | Nase (KV3) | 1 | x 389-398, y 173-181 | **18,2 / 7,1 / 1,3** | 48,3 / 31,0 / 33,6 | 13,9 / 8,7 / 1,6 |
| F5 | Kinn/Maulzone (KV4) | 1 | x 378-398, y 185-194 | **9,9 / 9,9 / 3,8** | 32,2 / 10,4 / 11,6 | 5,0 / 9,9 / 2,8 |
| F6 | Brust rot | 1 | x 372-424, y 197-232, R-G >= 15 | **16,6 / 17,3 / 6,2** | 56,5 / 18,8 / 22,1 | 11,7 / 19,2 / 7,0 |
| F7 | Oberarm (W)-rechts = (B)-links | 1 | x 307-347, y 167-212 | **33,3 / 5,5 / 2,7** | 79,5 / 64,1 / 63,6 | 28,6 / 6,6 / 2,8 |
| F8 | Oberarm (W)-links = (B)-rechts | 1 | x 437-477, y 182-227 | **16,4 / 8,3 / 1,5** | 43,6 / 25,1 / 28,1 | 11,3 / 9,8 / 1,8 |
| F9 | Jeans | 1 | x 374-437, y 264-297 | **12,7 / -0,9 / -1,5** | 21,4 / 21,8 / 24,4 | 7,1 / 0,5 / -2,0 |
| F10 | Klaue (B)-rechts rot | 1 | x 482-507, y 330-356, R-G >= 15 | **11,2 / 14,0 / 2,5** | 39,3 / 10,2 / 16,2 | 6,2 / 15,0 / 2,6 |
| F11 | Kopf rote Zone oben (KS1, ohne Maske) | 2 | x 418-441, y 193-211 | **27,6 / 15,3 / 3,3** (Maske R-G >= 25: 28,7 / 17,8 / 4,9) | 86,8 / 45,7 / 50,1 (Maske) | 24,2 / 19,5 / 5,6 |
| F12 | Auge seitlich (heller Fleck) | 2 | p_blob, 77 px | **48,4 / 4,0 / 0,5** | 113,1 / 102,0 / 103,7 | 44,1 / 4,9 / 0,5 |
| F13 | Haarkante seitlich (KS2) | 2 | x 458-465, y 228-248 | **19,1 / 1,2 / 1,1** (Streifen L < 40: 12,6 / 0,6 / 1,4) | 25,2 / 20,9 / 20,3 (Streifen) | 7,1 / 1,9 / 1,0 |
| F14 | Buckel hell | 2 | x 372-414, y 195-227, L >= 80 | **60,0 / 11,0 / 1,2** | 156,2 / 127,6 / 132,8 | 56,1 / 11,8 / 1,4 |
| F15 | Schwanz Wurzel, Unterseite | 2 | Kreis r12 um (345,238), Chroma >= 6 | **9,3 / 11,4 / 1,5** | 31,6 / 8,4 / 13,9 | 4,5 / 10,9 / 1,2 |
| F16 | Schwanz Bogen, Unterseite | 2 | Kreis r10 um (305,250), Chroma >= 6 | **8,7 / 8,4 / 2,2** | 27,5 / 9,0 / 11,4 | 4,2 / 7,8 / 1,8 |
| F17 | Schwanz Bogen, Aussenseite | 2 | Bewegungsmaske y <= 300, Chroma < 6 | **26,8 / -0,1 / 2,4** | 55,9 / 52,1 / 49,3 | 21,9 / 1,3 / 2,2 |
| F18 | Schwanz unter dem Knick | 2 | Bewegungsmaske x 280-300, y 312-345 | **15,2 / 3,2 / 1,6** | 34,0 / 25,1 / 25,1 (aus RGB 43,4 / 36,8 / 35,9) | ~9 / 4 / 1,5 |
| F19 | Jeans seitlich | 2 | x 397-432, y 298-327 | **18,4 / 0,5 / -3,5** | 33,7 / 33,4 / 39,3 | 13,1 / 1,5 / -3,6 |
| F0 | Bezug Wand links | 1 und 2 | x 40-119, y 150-299 | 64,7 / 0,2..0,4 / -2,0..-2,2 | 146,7 / 146,8 / 150,3 | 60,8 / 0,7 / -1,8 |

Stichprobe gegen die Leser (gleiche oder fast gleiche Fenster): F2 B 47,2 / 7,1 / -1,1; F6 B 16,2 / 18,9 / 6,6;
F8 B 15,4 / 7,6 / 1,4; F9 A 4,3 (SW ab) gegen Pruefer 4,7; F11 A 28,8 / 17,8 / 5,0; F14 A 59,4 / 11,0 / 1,2;
F19 A 17,3 / 0,5 / -3,3; F15 A 11,6 / B 12,5. Abweichungen ueber 3 L* gibt es nur bei verschiedenen Fenstern (F7: B 23,5
mit Fenster x 305-340, y 180-230, Lichtverlauf auf dem Arm, Std 24). Deshalb gelten fuer die Riegel genau die Fenster
dieses Blattes.

## 6. Port-Ist V3 r2 (gelobter Koerper Ref2 + Kopf/Schwanz V3 r2) und Abweichungen

Pruefer: Spielbild h135 (Scratchpad sfv4/port_V3r2_h135.png, rechte Haelfte). Die Port-Pose ist aufrechter als n1485,
die Fenster sind von Hand auf die Port-Pose gesetzt (P_port_ist.json). Vergleichsregel aus Abschnitt 5:

| Region | Port Lab (V) | Ziel | dE00 | dL | Befund |
|---|---|---|---|---|---|
| Kopf rote Kuppe | 35,1 / 8,6 / 3,6 | 24,2 / 19,8 / 5,4 | **11,8** | +10,9 | grau statt rot, zu hell |
| Auge | 32,5 / 5,4 / 0,0 | 46,9 / 7,2 / -1,1 | **12,8** | -14,4 | **dunkles Auge statt hellem Fleck** |
| Haarfeld | 35,7 / 6,6 / 2,1 | 9,2 / 6,2 / 0,9 | **18,9** | +26,5 | **kein dunkles Haarfeld** |
| Nase | 35,4 / 5,1 / 1,9 | 19,0 / 7,3 / 1,4 | 12,5 | +16,4 | zu hell |
| Kinn/Maulzone | 18,6 / 7,7 / 3,3 | 9,9 / 9,9 / 3,8 | 6,0 | +8,7 | offenes Maul mit Zaehnen (Lupe) statt dunkler Zone |
| Brust rot | 26,4 / 16,0 / 8,1 | 16,6 / 17,3 / 6,2 | 7,1 | +9,8 | zu hell |
| Jeans | 18,4 / -2,2 / -5,3 | 12,7 / -0,9 / -1,5 | 5,2 | +5,7 | zu hell, zu blau |
| Wand links (Eichprobe) | 65,1 / 0,2 / -2,5 | 64,7 / 0,2 / -2,0 | 0,5 | +0,4 | Eichung gilt |

Leser (Richtung, andere Fenster): A: Kopf 5-12 L* zu hell, Auge dE 6,7, Haarstelle dE 19,4, seitlich grauer Kopf statt
roter Oberseite dE 16,7. B: Kuppe dE00 8,2, Haare 16,3, Nase 8,3, Brust rot 9,1 (Farbton 27 statt 19 Grad), Oberarme
8,4 / 9,1, Klaue 8,9, Jeans 5,8; Schwanz (Buehnen-Leiste v3/01_seite, nicht farbtreu) oben L* 18-20 statt 9-12, Wurzel
a* 4,0 statt 10,7, dE00 8,3 / 5,3 / 7,4 / 3,2 (Wurzel / breit / Knick / Spitze).

Gesamt: Der Port ist je nach Region 6-26 L* zu hell. Am Kopf ist der Kontrast umgekehrt: dunkles Auge in grauer Haut
statt hellem Fleck neben dunklem Haarfeld. Die Kopfoberseite ist grau statt rot. Die Schwanz-Form V3 r2 folgt dem
KI-Bogen (Messblatt V3 R21-R24: stetig duenner ab 0,175 H an der Wurzel); Taille am Knick und duennes, gleich breites
Endstueck fehlen. Die Ist-Form in der Vorlage-2-Kamera misst der Bauer zuerst mit T1-T6 (die v3-Leiste 01_seite ist
abgedunkelt, A: Hintergrund-Steigung ~0,6).

## 7. RIEGEL-TABELLE

> Seit 2026-11-07 (V4 Originaltexturen): die Zeilen F-Riegel Koerper, F0 Gueltigkeit, T8 und die Lichtgegenprobe als Farbziel sind
> ueberholt (Abschnitt 5). K1-K10, S1-S4, T1-T7, G1, M1-M4 gelten weiter; dazu T-K1 bis T-K4 (riegel_ref2tex.py).
>
> **Originaltexturen Runde 2 (2026-11-07):** In K2, K5, K7, K8, K9, S1, S3 und S4 sind die Video-Farbziele (dE00 zu Lab-Werten aus
> Vorlage 1/2) **ueberholt** (Abschnitt 5, Nachtrag Runde 2), ebenso das Kinn-Ziel "dunkel" (K8 9,9 L*). Orte, Fenster, Groessen,
> Komponentenzahl und die relativen Bedingungen gelten weiter. Ersatz (riegel_ref2tex.tk4, Abnahme gegen Referenz 2 = Spiel-Asset im
> selben Render):
>
> | Zeile | ueberholt | gilt jetzt |
> |---|---|---|
> | K2 | dE00 <= 6 zu 46,9/7,2/-1,1 | L* >= KV5 + 20 (wie bisher); blass: a* <= 15, b* >= 0 |
> | K5 | dE00 <= 6 zu 13,3/7,4/1,5 | KV2 L* <= Augenfleck - 25 (wie bisher) und <= Ref2-Kopf L* P50 - 8 |
> | K7 | dE00 <= 6 zu 18,2/7,1/1,3 | KV3 L* zwischen Haarfeld und Augenfleck (wie bisher) und >= Haarfeld + 8 (Nase eigenes Merkmal) |
> | K8 | dE00 <= 6 zu 9,9/9,9/3,8; Anteil L* > 35 <= 0,05 | kein dunkles Maul: KV4 (Kopf-Id erodiert) Albedo-L* P50 >= Ref2-Kopfhaut - 8; dE00 <= 10 zu Ref2 KV4 |
> | K9 / S4 | dE00 <= 6 zu 22,6/15,4/5,2 bzw. 27,6/15,3/3,3 | a* >= 11 (wie bisher); \|dL*\| zu Ref2 im selben Fenster <= 8 |
> | S1 | dE00 <= 6 zu 48,4/4,0/0,5 | 1 Komponente und Groesse wie bisher; blass: a* <= 15, b* >= 0 |
> | S3 | L* <= 22; dE00 <= 6 zu 19,1/1,2/1,1 | KS2 L* <= Ref2-Kopf L* P50 (Pose 85) - 8; Abstand zum Fleck <= 0,01 H (wie bisher) |
> | T-K5 (neu) | - | Kopf gegen Ref2 im rohen Render, Pose 135 und 85: Grundton L* P50 ausserhalb des Haarfelds (vorn KV2 + K6-Box, seitlich KS2) wie Ref2 +- 4; Grundton/Rumpf >= Ref2 - 0,2; Anteil b* < 0 <= 0,02; dunkle Textur (Albedo L* < 25) ausserhalb des Haarfelds <= Ref2 + 0,05 |
> | T-K3d (neu) | - | Schwanzwurzel Id 16: waagerecht/senkrecht \|dL*\| je Teilflaeche (ganz, obere 14 Zeilen, 4 Quadranten) in der Spanne von Rumpf Id 8 / Oberarm Id 13 |
>
> K1, K3, K4, K6, K10, S2 bleiben unveraendert. In der Tabelle unten sind die ueberholten Ziele mit **[ueberholt R2]** markiert.
>
> **Kopf nach Nutzerbeschreibung, Runde 3 (2026-11-07):** die Kopfzeilen folgen der Lesung grau links bis Nase / Haar braeunlich rechts /
> Kuppe dunkel rotbraun (Abschnitt 5, Nachtrag Runde 3). Bezug ist der Kopf-Median (L* P50 der Kopf-Id 9 im selben Bild; Videoraum bei den
> Fensterzeilen, roher Render bei T-K9). Die Video-Zahlen stehen nur noch als Richtung. Ersatz (riegel_ref2tex.tk4 / tk9 / tk8):
>
> | Zeile | Runde 2 | gilt jetzt (Runde 3) |
> |---|---|---|
> | K2 | L* >= KV5 + 20; blass | unveraendert; "hell und unbunt" (a* <= 15, b* >= 0); Video 46,9/7,2/-1,1 Richtung |
> | K5 | KV2 L* <= Auge - 25 und <= Ref2 P50 - 8 | KV2 braeunlich: L* 0,62-0,90 x Kopf-Median (Videoraum), a* > 0, b* > 0; Video 13,3/7,4/1,5 Richtung (Video 12,9 / 17,3 = 0,75) |
> | K6 | Spiegelstelle L* <= 18 | Spiegelstelle liegt in der Haarflaeche: Mittel L* <= Kopf-Median - 3, kein heller Fleck (max L* <= Auge - 15) |
> | K7 | KV3 zwischen Haar und Auge, >= Haar + 8 | Nase = Zonengrenze, mitteldunkel wie die Wange gleicher Hoehe: \|KV3 - KV5\| <= 4 (Vorlage Nase 18,2 / KV5 18,9 = -0,7) und KV3 >= Haar rechts daneben (KV3r 0,62-0,85 x 0,66-0,81) + 1 (Vorlage +4,9) |
> | K8 | Albedo >= Ref2-Kopfhaut - 8; dE00 <= 10 zu Ref2 KV4 | kein dunkles Maul in der Textur: Albedo-L* P50 >= Ref2-Kopfhaut - 8; b* >= 0. Kein Farbziel Ref2-Kinn mehr (das Kinn liegt in der grauen Zone; dE00 nur Ausgabe) |
> | K9 / S4 | a* >= 11; \|dL*\| zu Ref2 <= 8 | dunkel rotbraun: a* >= 8, b* > 0; L* <= Kopf-Median + 8 (Videoraum; roh + 6 in T-K9c); K9 zudem >= Haar KV2. Video 22,6/15,4/5,2 bzw. 27,6/15,3/3,3 Richtung |
> | S1 | 1 Komponente, Groesse, blass | unveraendert ("hell und unbunt") |
> | S3 | KS2 L* <= Ref2 P50 (85) - 8; Abstand <= 0,01 H | KS2 L* <= Kopf-Median (Pose 85, Videoraum) - 5; Abstand zum Fleck <= 0,01 H; Video 19,1/1,2/1,1 Richtung |
> | T-K5 | Grundton ausserhalb des Haarfelds (KV2 + K6-Box / KS2) | Grundton = graue Zone (ausserhalb Haar + Kuppe, Zonen = Klasse des Texel-Index, riegel_ref2tex.zonen_emd) wie Ref2 +- 4; uebrige Bedingungen gleich |
> | T-K9 (neu) | - | Zonen am rohen Render Pose 135: a) grau: Chroma P50 <= 0,5 x Rumpf Id 8, L* P50 = Ref2-Kopf +- 5, >= 0,80 der grauen Pixel links der Nase (x = Mitte KV3), >= 0,70 der Kopfpixel links der Nase grau; b) Haar: L* P50 0,62-0,90 x Kopf-Median, a* > 0, b* > 0, Flaeche Haar + Kuppe 0,40-0,55 der Kopfpixel; Rand: Sobel/8-Gradient 3-8, Grenze je Zeile Std zur Geraden > 4 px; c) Kuppe: L* P50 <= Median + 6 und >= Haar, a* >= 8, b* > 0, Anteil > 0,03; d) Augenfleck in der grauen Zone, K4 OK, KV3 zu 0,3-0,7 grau, S2 OK, KS2 roh <= Median (85) - 4; e) ganzer Kopf L* P50 >= 23, Kopf/Rumpf >= 0,9; f) Gier +120/+180 (neutral): kein Fleck L* < 15 ueber 100 px |
> | T-K8 (neu) | - | Aenderungsumfang gegen den Vorstand (EM30_V4ot2.EMD): Sektionen 0-7 und CLUT bytegleich; Texel nur in den Kopf-UV-Flaechen (Zahl, Bbox), ausserhalb 0 |
> | T-K2 | keine neue Farbe | Ref2-Farbe oder belegte Ableitung (Liste 'ableitungen' im Protokoll; Runde 3: keine) |

**Render fuer alle Riegel (Bauer und Pruefer gleich):** b2.zeichne (re15_port/tools/r41_sichel/v2/b2.py: Spielkamera
50A0/12, GTE-Projektion, RDT-Licht Cut 12), 640 x 480. Posen in **re15_port/tools/r41_sichel/v4/posen_v4.json**, die der
Bauer anlegt und committet: "85" seite = Kopie von sfv2/posen_seite.json "85" (clip 1, bild 26, gier 556, x -8070,
z -21202, IoU 0,62); "135" vorn = neu angepasst an die Silhouette von n1485 (IoU >= 0,60, Verfahren wie
posen_vorn.json). Ruhe-Riegel T1-T6: Schwanzkette im Keyframe-Ziel ohne Feder. H_port = Sohle - Oberkante der
Koerper-Ids ohne 16-22 (b2.ok_sohle). Kopfbox = Bbox der Pixel mit Id 9. Farben: Vergleichsregel aus Abschnitt 5
(sigma 1,5, a/b, dE00). Lichtgegenprobe: b2-Render Ref2 in Pose posen_vorn "118" gegen RE15_FRAMEDUMP desselben
Bildes, Rumpf-Mittel (Id 7/8) dE00 <= 3; sonst Farbriegel am Spielbild messen.

| Mass | Vorgabe | Toleranz | Messmethode am Port-Render |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | EM30_HANGAR.EMD sha256 cfd1ac42... (Commit 84e8a4d97, Pin release/em30_hangar.sha256) | Meshes 0-8 und 10-15 (Ecken, Normalen, Primitive inkl. UV), Skelett 0-15, EDD/EMR **byte-gleich**; geaendert werden duerfen nur Mesh 9 (Kopf), Meshes 16-22 (Schwanz), TIM-Texel und CLUT | birkin_formen MD1-Serializer: Byte-Vergleich je Mesh gegen Ref2 (wie R28 V3) |
| G2 Gang/Szene unveraendert | Ref2 | Clips und Keyframes der Knochen 0-15 und der Szenenablauf 50A0/50E0 gleich; Tests gruen | ctest -R "unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form" (Muster wie im Auftrag) |
| K1 Augen vorn | 1 heller Fleck im Kopf | genau 1 Komponente (p_blob: hellstes 3x3 in der Kopfbox, Schwelle Mitte aus Extrem und Median 21x21) mit L*-Maximum im Fleck - L*-Median 21x21 >= 20 (Original 56,2 - 26,7 = 29,5) | Pose 135, Kopfbox Id 9, nach sigma 1,5 |
| K2 Augenfleck Farbe | 46,9 / 7,2 / -1,1 **[ueberholt R2]** | dE00 <= 6 **[ueberholt R2]**; L* >= KV5 (Kopf (B)-links unter dem Auge, Anteile 0,05-0,35 x 0,62-0,85; Ziel 18,9 / 7,1 / 0,6) + 20 | wie K1, Mittel der Fleckmaske |
| K3 Augenfleck Groesse | 0,026 x 0,033 H | Breite 0,016-0,036 H, Hoehe 0,023-0,043 H | Bbox der Fleckmaske / H_port |
| K4 Augenlage vorn | 0,050 H (B)-links = (W)-rechts der Kopfmitte, 0,093 H unter der Kopf-Oberkante | 0,035-0,065 H links; 0,07-0,12 H unter OK | Fleck-Schwerpunkt minus Kopfbox-Mitte (x) bzw. Kopfbox-Oberkante (y), / H_port |
| K5 Haarfeld vorn | dunkles Feld (B)-rechts | Fenster KV2 (Anteile 0,52-0,85 x 0,25-0,75 der Kopfbox): dE00 <= 6 zu 13,3 / 7,4 / 1,5 **[ueberholt R2]** und L* <= Augenfleck - 25 | Anteils-Fenster auf der Port-Kopfbox |
| K6 zweites Auge verdeckt | Spiegelstelle dunkel | im Spiegelfenster (Fleckbox an der Kopfmitte gespiegelt) L* <= 18 | dito |
| K7 Nase | mitteldunkel, mittig | Fenster KV3 (0,44-0,62 x 0,66-0,81): dE00 <= 6 zu 18,2 / 7,1 / 1,3 **[ueberholt R2]**; L* zwischen Haarfeld und Augenfleck | dito |
| K8 Kinn/Maulzone | dunkel, rotstichig, keine Struktur **[ueberholt R2: Ref2-Haut, kein dunkles Maul]** | Fenster KV4 (0,21-0,62 x 0,88-1,05): dE00 <= 6 zu 9,9 / 9,9 / 3,8; Anteil Pixel mit L* > 35 <= 0,05 (keine hellen Zaehne) **[ueberholt R2]** | dito, nach sigma 1,5 |
| K9 Kopf-Oberseite rot vorn | 22,6 / 15,4 / 5,2 **[ueberholt R2]** | Fenster KV1 (0,40-0,80 x 0,02-0,32): dE00 <= 6 **[ueberholt R2]**, a* >= 11 | dito |
| K10 Kopfbox vorn | 0,159 x 0,172 H | Breite 0,14-0,18 H, Hoehe 0,15-0,20 H | Bbox Id 9, Pose 135 |
| S1 Auge seitlich | 1 heller Fleck, 48,4 / 4,0 / 0,5 **[ueberholt R2]** | dE00 <= 6 **[ueberholt R2]**; Groesse 0,028-0,052 x 0,040-0,070 H | p_blob in der Kopfbox, Pose 85 |
| S2 Augenlage seitlich | 0,049 H hinter der Kopf-Vorderkante, 0,79 H ueber der Sohle | 0,03-0,07 H; 0,76-0,82 H | Schwerpunkt gegen Vorderkante (max x Id 9) und Sohle |
| S3 Haarkante seitlich | dunkler Streifen vor dem Auge | Fenster KS2 (Anteile 0,84-0,96 x 0,52-0,80 der Kopfbox seitlich): L* <= 22 und dE00 <= 6 zu 19,1 / 1,2 / 1,1 **[ueberholt R2]**; Abstand zum Augenfleck <= 0,01 H | Anteils-Fenster, Pose 85 |
| S4 Kopf-Oberseite rot seitlich | 27,6 / 15,3 / 3,3 **[ueberholt R2]** | Fenster KS1 (0,11-0,53 x 0,04-0,29): dE00 <= 6 **[ueberholt R2]**, a* >= 11 | dito |
| T1 Beginn | 0,767 H ueber der Sohle, 0,15 H hinter dem Buckelscheitel | +-0,03 H je Achse | Unterkante des Schwanzansatzes: unterster Pixel von Id 16 an der Grenze zum Rumpf (Id 7/8); Buckelscheitel = Mitte der obersten Zeile der Koerper-Ids |
| T2 Biegung | dx -0,245 H, dy +0,036 H zum Beginn; Drehung 70-90 Grad | +-0,03 H; Drehung 60-100 Grad innerhalb <= 0,10 H Achslaenge | Mittellinie der Schwanz-Ids (P_schwanz.mittellinie aus v3), Richtung je 0,05 H |
| T3 Knick | dx -0,265 H, dy +0,257 H zum Beginn | +-0,03 H | Stelle minimaler Breite der Mittellinie zwischen Biegung und Ende |
| T4 Ende | dx -0,245 H, dy +0,397 H zum Beginn; 0,37 H ueber der Sohle | +-0,03 H | unterster Mittellinienpunkt |
| T5 Breite nach oben | f 0,1 / 0,3 / 0,5 / 0,7 (Knick -> Beginn): 0,03 / 0,065 / 0,125 / 0,14 H | +-0,015 / +-0,02 / +-0,025 / +-0,025 H; monoton steigend (kein Abfall > 0,01 H) | Breite quer zur Mittellinie, sichtbare Schwanz-Ids im vollen Render |
| T6 Endstueck | Breite 0,035 H, Laenge 0,145 H, Richtung 82-84 Grad | Breite 0,025-0,05 H, Laenge 0,12-0,17 H, Richtung 75-95 Grad; Taille am Knick <= 0,035 H und die schmalste Stelle der Kette | dito, Knick -> Ende |
| T7 Gang h85 | T1-T4 im laufenden Spiel | +-0,06 H je Punkt | RE15_FRAMEDUMP h85 oder b2-Render mit dem Feder-Stand des Laufs |
| T8 Schwanzfarbe | F15 9,3 / 11,4 / 1,5; F16 8,7 / 8,4 / 2,2; F17 26,8 / -0,1 / 2,4; F18 15,2 / 3,2 / 1,6 | dE00 <= 6 je Region; Wurzel a* >= 7; Endstueck a* <= 6 | F15 Kreis r12 um T1 + (-12 px, -5 px), F16 Kreis r10 um T2 + (+10 px, 0), Unterseite = Chroma >= 6, Aussenseite = Schwanz-Ids mit Chroma < 6 oberhalb T2, F18 Ids zwischen T3 und T4; Vergleichsregel |
| F-Riegel Koerper | F6-F10, F14, F19 | dE00 <= 5 je Region und Betrag dL <= 4; Mittel ueber alle F-Riegel dE00 <= 4 | Koerper-Ids: Brust Id 8 mit R-G >= 15; Oberarm (W)-rechts Id 13, (W)-links Id 10; Jeans Ids 0/1/4 mit b* < 0; Klaue Id 15 mit R-G >= 15; Buckel Id 8 mit L >= P70; Vergleichsregel |
| F0 Gueltigkeit | Wand links | dE00 <= 2 nach Eichung, sonst ist der Render nicht farbvergleichbar | Fenster x 40-119, y 150-299 (Render mit Raumbild oder Spielbild) |
| M1 Dreiecke Schwanz | Richtwert EM30 Arm rechts 13-15: 212 | Meshes 16-22 zusammen <= 212 (Viereck = 2) | Zaehlung im MD1 |
| M2 Dreiecke Kopf | EM30 Kopf 109 | Mesh 9 <= 118 | dito |
| M3 Farbtiefe | 8 bpp | 8-bpp-TIM, Texel 1:1 aus der Haut-CLUT; keine Farben ausserhalb der CLUT; kein Weichzeichner in der Textur (die sigma-1,5-Weichzeichnung ist nur Messverfahren) | TIM-Kopf, CLUT-Zeilen der UVs |
| M4 Texeldichte | wie Haut | Kopf und Schwanz 0,045-0,091 Texel/E. (EM30-Haut 0,046-0,087) | sqrt(Texelflaeche / Modellflaeche) je Mesh (machart_v3.py) |
| (Gegenprobe) | Original gegen die eigenen Riegel | K6 Spiegelfenster L* 12,9 (<= 18), K8 Anteil L* > 35 = 0,000 (L* max 20,3), K1 29,5 (>= 20), K2 46,9 >= 18,9 + 20, K5 13,3 <= 46,9 - 25, K9/S4 a* 15,4 / 15,3 >= 11, T8 a* 11,4 / 3,2: das Original besteht alle Kopf- und Farbriegel | P_riegel_gegenprobe_original.txt, P_riegelfenster.json |
| A1 Abnahme am Bild | Nutzer-Beschriftung | Leiste: Spalten Original (n1485 entblendet / n1435), Port neu, Port V3 r2; Zeilen vorn h135 ganz + Kopf x6, seitlich h85 ganz + Kopf x6 + Schwanz x4, mit den Nutzerpunkten eingezeichnet. Der Pruefer urteilt am Bild; die Zahlen sind Pflicht, reichen aber nicht | Leiste + Sichtpruefung |

## 8. Verhaeltnis zu Messblatt V3 und Nachtrag (N1-N27)

Ueberholt (KI-Quelle oder durch die Beschriftung widerlegt):
- **V3 Abschnitt 1, K3-K11 und Riegel R3-R8** (Mandelauge 0,02 x 0,01 H, dunkle Pupille, Lid, Glanzkern): das
  Original-Auge ist ein **heller** Fleck (K1-K4, S1-S2).
- **K15 / Formblatt V2 #18 "helle Kuppe"** und **N5 (helle Kuppe L* 38-50)**: die Kopfoberseite ist im Original **rot**
  (F1/F11, K9/S4).
- **N1-N4** (Auge dunkel eingebettet mit hellem Kern): ersetzt durch K1-K3. Wie das Auge gebaut wird (Fleck, oder Kern
  plus Rand), ist frei; gemessen wird nach der Weichzeichnung.
- **N6/N7** (Augenlage 0,24-0,36 bzw. -0,032..-0,016 H): ersetzt durch K4 (0,050 H links, 0,035-0,065).
- **N9-N12** (Haarzone relativ zur Kuppe, 3-6 Straehnen, Straehnen-Kontrast): Straehnen sind im Original nicht
  aufloesbar; ersetzt durch K5/K6 (dunkles Feld, absolute Farbe). **N13** (Auge frei von Haaren) bleibt.
- **N18-N25 und R10-R14** (Lippenlinie, Zahnreihen, Lippen, Lippenglanz, Maulgroesse, Zaehne): KI-Quelle. Das Maul ist
  nicht beschriftet und im Original nicht aufloesbar; ersetzt durch K8 (dunkle, rotstichige Zone ohne helle Zaehne).
  **N14-N17** (kein Oval, fast geschlossen, kleine schwarze Flaeche, zerklueftet) bleiben als Obergrenzen gueltig.
  **N23/N24** (rotes Kinn a* >= 12, L* 14-22): ersetzt durch K8 (L* 9,9, a* 9,9).
- **R15-R19** (Schwanzfarbe relativ zur Port-Haut): ersetzt durch die absoluten Ziele F15-F18 / T8.
- **R20** (Muster) bleibt als Gestaltung, kein Original-Beleg.
- **R21-R24** (Dicke 0,175 / 0,085 / 0,035 H ab der Wurzel, stetig duenner): ersetzt durch T5/T6 (Taille am Knick, gleich
  breites Endstueck).
- **R25/R26** (Glaette <= 36 Grad je 0,05 H, <= 3 Knicke): ersetzt durch T2 (eine Biegung 70-90 Grad). SW_GLATT_MAX ist
  anzupassen (Abschnitt 4).
- **R27** (Austritt 0,12 H unter OK): ersetzt durch T1 (Beginn-Unterkante 0,229 H unter OK; Ansatzmitte ~0,15 H).
- **R28** wird G1 (jetzt sind auch TIM/CLUT aller Teile frei, weil alle Farben neu geeicht werden).
- **R29-R32** bleiben als M1-M4. **R1/K1** (ein Kopfauge) bleibt (K1).
- **R2/K2/R9/K12-K14** (Schulterauge): nicht Gegenstand der Beschriftung und vom Nutzer nicht beanstandet; Stand V3 r2
  bleibt, seine Umgebung folgt F7.
- **N26** (Abnahme Seite an Seite) wird A1, mit dem Original statt der KI.
- **N27** (PSX-Machart Kopf) bleibt (M2-M4); "Haare als Straehnen" entfaellt. Das Haarfeld ist Texel auf Mesh 9; bei
  Bedarf hoechstens 2 flache Dreiecke an Knochen 9 fuer die Vorderkante (M2 beachten).

## 9. Offen (nur, was kein Bild beantwortet)

1. **Maul vorn**: im Original nicht aufloesbar (dunkle Zone; darunter helle Flecken y 188-200, Zaehne oder Schluesselbein
   nicht trennbar). Vorgabe K8 + N14-N17; keine Nutzerfrage noetig, solange keine hellen Zaehne sichtbar sind.
2. **Spitzenform** des Schwanzes: nicht aufloesbar (stoesst an die helle Bodenkante); Endstueck gleich breit (T6).
3. **Schwanz in Vorlage 1**: dunkles Band x 297-333, y 340-376; Schwanz oder Schlagschatten ist aus dem Einzelbild nicht
   entscheidbar. Kein Riegel vorn.
4. **Lage in der Tiefe**: Vorlage 2 zeigt eine Seite; ob Biegung und Endstueck seitlich (z) versetzt sind, zeigt das Bild
   nicht. Die Riegel gelten in derselben Kamera und Pose (posen_v4 "85"), daher reicht die Bildlage.
5. **Pose 135**: Port-Pose und Original-Pose sind bei h135 verschieden (Port aufrechter). Die Kopf-Riegel beziehen sich
   auf die Kopfbox, die Farbriegel auf Regionen. Die neue Pose "135" passt der Bauer an (IoU >= 0,60).
6. Keine Nutzerfrage: Die Beschriftung beantwortet Auge, Haare, Nase und die Schwanzpunkte eindeutig (Belegung 1,000
   gegen hoechstens 0,82). Wo ein Leser abwich (Auge seitlich, Kopf seitlich rot, Bildnummer), entscheidet die Messung
   oben.

## Nachtrag Runde 5 (Entscheid O7 des Orchestrators, 2026-11-07, eingetragen vom Bauer)

**F6 und F7 werden an der Fensterlage dieses Blatts gemessen, nicht ueber die ganze Id.** F7 gilt dazu nur zusammen mit der Sicht-Box
des unteren Oberarms (Riegel FS Oberarm (B)-links unten, Box 300-345 x 213-250 der Sicht-Pruefung V4 r1/r2).

| Riegel | bisher (Wortlaut Abschnitt 7 "Koerper-Ids") | ab Runde 5 | Messung am Port (riegel_v4.py) |
|---|---|---|---|
| F6 | Id 8 mit R-G >= 15, ganz | rote Id-8-Bildpunkte (R-G >= 15) im Fenster x 372-424, y 197-232 | Zeile "F F6 (Fensterlage, O7)"; "F F6 Messblatt-Wortlaut (Id ganz)" nur noch INFO |
| F7 | Id 13 ganz | obere 58 % der Zeilen von Id 13 (Lage des Fensters y 167-212 im Original-Oberarm y 160-250) UND FS Oberarm (B)-links unten | Zeile "F F7 (Fensterlage, O7)"; "F F7 Messblatt-Wortlaut (Id ganz)" nur noch INFO |
| F-Mittel | ueber F6-F10, F14, F19 | dieselben Zeilen in der Lesart oben | Zeile "F Mittel" |

Begruendung (Zahlen aus dem Original, Vorlage 1 entblendet):
- F6: Das Fenster liegt auf der Brustmitte (16,6 / 17,3 / 6,2). Die roten Bildpunkte des ganzen Oberkoerpers (Polygon der Sicht-Pruefung V4 r3
  + Rumpfbox 352-448 x 196-292) haben im Original 19,6 / 11,1 / 3,1 (dE00 zum Fenster 5,3, Betrag dL 3,0). Wer "Id 8 ganz" auf das Fensterziel bringt,
  faerbt den ganzen Rumpf so rot wie die Brustmitte; Lauf r4 Koerper-Eichung 1 ergab damit Rot Mitte / aussen 1,81 und Schultern a* > 12
  0,24 (Original 2,75-2,91 / 0,00-0,05) - das widerspricht der Rot-Verteilung des Originals (Sicht-Pruefung V4 r3 1.5 / r4 1.5).
- F7: Id 13 besteht aus der Fensterlage (1031 px, Original 33,3 L*) und dem unteren Oberarm (1150 px, Sicht-Box Original 11,0 L*). Fuer
  "Id 13 ganz" mit |dL| <= 4 (>= 29,3 L*) muesste der untere Teil (29,3 x 2181 - 33,3 x 1031) / 1150 = 25,7 L* haben: dL +14,7 gegen die
  Sicht-Box. Der ganze Oberarm (B)-links misst im Original 17,3 L* (Sicht-Pruefung V4 r2 1.4, Box 300-345 x 186-250).
- Die Original-Gegenprobe (Abschnitt 7 letzte Zeile) bleibt gueltig: an der Fensterlage misst das Original genau die Fensterwerte.
