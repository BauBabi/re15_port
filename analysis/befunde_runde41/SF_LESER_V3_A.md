# SF Leser V3 A: Vorlage Augen/Kopf und Schwanz der Sichel-Form, gemessen an den KI-Freistellern (2026-11-06)

Kein RE-Befund. Die Sichel-Form gibt es auf der Disc nicht. Quelle der Gestaltung ist die KI-Fassung (Nutzerentscheid F4,
PORT-WAHL); jede Zahl unten ist eine Bildmessung mit Datei und Pixelkoordinaten, keine @0x-Konstante. Geaendert wurde nichts
ausser den neuen Werkzeugen (re15_port/tools/r41_sichel/v3/A_*.py), diesem Blatt und den Belegbildern. Nicht committet.

Quelle Vorlage: `C:/workspace/git/reAi_v2/Version_80%/birkin01-24.png` (Alpha = Maske). Zuordnung und Versatz im 1280x960-Bild
aus SF_LESER_B.md Z.39-62; nachgeprueft fuer birkin11/12/13 gegen _slp-Bild n1433/n1435/n1442: mittlere Abweichung 0,0
(Nachbarbilder 7,7-13,1). H = Hoehe der Alpha-Bbox des Freistellers (Oberkante Buckel/Kopf bis Sohle): birkin12 505 px
(Formblatt R1: 504), birkin19 570 px (R2: 569). Koordinaten = Freisteller-Pixel (fuer 1280er Bildkoordinaten den Versatz
addieren). Seiten: (W) = Sicht des Wesens, (B) = Sicht des Betrachters; von vorn B-links = W-rechts (Sichelseite).
Farben: RGB-Mittel und CIE-Lab D65 (OpenCV sRGB), Flaechen 2 px erodiert; Delta-E als CIEDE2000 (dE00), dazu dE76.
"geschaetzt" = Grenze per Augenmass an der Lupe gesetzt.

Werkzeuge (alle in `re15_port/tools/r41_sichel/v3/`): A_augen.py (Augen/Maul), A_kopfbox.py (Kopfbox, Baender),
A_schwanz.py (Schwanz KI), A_original.py (Original-Video), A_port_v1.py + A_port_mass.py + A_port_schwanz_frei.py
(Port V1 rendern/messen, Werkzeuge v2/b2.py), A_belege.py (Belegbilder), A_lupe.py/A_kacheln.py (Pruefansichten).
Laufausgaben/JSON: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/A_*.json`.

Belegbilder (`analysis/befunde_runde41/SF_belege/v3_lesung/`):
- `A_1_augen_x8.png` Augen-Ausschnitte x8 mit Masken: KI Kopfauge h118/h125/h126/h85(Seite), KI Schulterauge h118/h125, V1 Kopf-/Schulterauge
- `A_2_schwanz_masken.png` Schwanzmasken, Mittellinie, Wurzelschnitt, Dicken bei 0,1-0,9, Knicke: KI h83/h85/h92 (Bogen), KI h69/h70 (roter Schwanz), V1 h85
- `A_3_farbfelder.png` Farbfelder KI-Haut / KI-Bogen / KI-roter Schwanz / Original-Haut / Original-Bogen / V1-Haut / V1-Schwanz, dazu Augenfarben KI und V1
- `A_4_original_masken.png` Original n1433/n1435/n1442 (Schwarzwert 17 ab) mit Schwanz- und Hautmaske, daneben das KI-Bild mit derselben Maske
- `A_5_maul.png` Maul KI h112/h118/h125/h126 (Zahnflecken, Huelle) und V1

Port-Ist V1 = `EM30_V1_SPITZ.EMD` (scratchpad sfv2/emd), gerendert mit b2.zeichne (Spielkamera 50A0/12, RDT-Licht,
Posen sfv2/posen_seite.json, posen_vorn.json, Knochen-Kennung je Bildpunkt: Schwanz = Knochen 16-22, Kopf = 9) und
"Kopf vorn nah" wie leisten_v2.leiste_rundum (neutrales Licht, 960x960, 0,9 px je Modelleinheit).

## 1. Augen und Kopf

| Mass | Wert KI (Bild, Koordinaten) | Original-Video | Port V1 | Sicherheit |
|---|---|---|---|---|
| A1 Augen auf dem Kopf | **1**, mittig ueber dem Maul; je genau ein Kopfauge in birkin16-24 (vorn/3/4, h105-h132) und birkin11/12/13 (Seite); birkin14/15 nicht sauber maskiert (A_1; Lupen sfv3/A_lk_sheet.png, A_lseite.png) | Kopf im Original nicht aufgeloest (Formblatt V2 #16) | 1 auf der Kopfplatte + 1 Schulterauge (A_1 unten) | sicher |
| A2 Lage Augenmitte in der Kopfbox | x 0,33 / 0,37 / 0,34 (h118/h125/h126) der Box zwischen den Rillen zu den Schultern; y 0,35 / 0,32 der Hoehe Oberkante..Kinn (h118 Box x 166-226, y 2-114; h125 x 184-242, y 1-127; Kinn geschaetzt) | - | Kopfplatte: Auge im oberen Drittel, mittig (A_v1_kopfnah) | Seiten gemessen (dunkelste Spalte), Kinn geschaetzt |
| A2 relativ zur Maulmitte | dx +3,6 / -8,7 / -7,2 px, dy -33,9 / -35,0 / -35,5 px = **0,06 H ueber der Maulmitte**, waagerecht ueber dem Maul (h118/h125/h126; Maulmitte = Huelle der Zahnflecken) | - | Auge ueber dem Maul, Abstand ~ halbe Maulhoehe | wahrscheinlich |
| A3 Auge gesamt | **mandelfoermig, B/H ~1,9**: 11x6, 13x7, 13x7, 14x8, 15x7, 15x8 px (h118, h125, h126, h130, h128, h132) = **0,019-0,024 H breit, 0,010-0,013 H hoch**; 0,18-0,25 der Kopfbreite. Seite (h83/h85): 8x5 / 9x7 px | - | **rund, B/H 1,0**: 59x59 px im Nahrender = 66 Einheiten = 0,017 H (H 3830-3900 E., Formblatt V2 Abschnitt 0); im Spielbild h118 Ring 5x6 px bei H 285 = 0,018x0,021 H; 0,17 der Kopfbreite | sicher (Maske A_1) |
| A3 Pupille | dunkelster Kern (L* <= Lmin+25 % der Spanne zur Haut) = 0,33-0,54 der Augenflaeche (h112-h132), dunkle Pixel unter der Schwelle gesamt 0,64-0,85; Pupille ~0,6 der Augenbreite (geschaetzt); kein abgesetzter heller Irisring | - | dunkle Mitte ~1/3 des Durchmessers, Kreuz/Punkt | sicher |
| A4 Pupille | RGB (26,13,18) / (24,16,20) / (26,16,21); Lab [5,4 6,4 -0,7] / [5,9 4,5 -1,4] / [6,3 4,9 -1,4] (h118/h125/h126) | - | Lab [6,0 7,7 3,0], RGB ~(36,15,16) | sicher |
| A4 Iris/Ring | **kein heller Ring**. Mittelton im Auge Lab [26,8 7,4 -1,7] RGB (74,60,67) (h118) | - | **heller Ring** Lab [54,4 3,8 5,1] (Nahrender), 54 % der Augenflaeche | sicher |
| A4 Lidrand (1 px Ring um die Maske) | Lab [39,8 7,1 -0,7] / [38,3 7,1 -0,9] / [34,4 7,0 -1,1]; RGB (105,91,96) (h118) | - | dunkelroter Aussenrand (ein Texel) | sicher |
| A4 Haut um das Auge (Ring 4-7 px) | Lab [50,7 7,7 0,8] / [44,8 7,5 0,3] / [43,9 7,5 0,0]; RGB (136,117,121) = helle grauviolette Flaeche | - | Lab [22,5 10,6 5,3] (Nahrender) | sicher |
| A4 Helligkeit Auge/Haut | **0,48 / 0,47 / 0,46** (L*-Mittel Auge / Haut; h112-h132: 0,38-0,51); Auge dunkler als Haut | - | **1,69** (Kopf), 2,13 (Schulter): Auge heller als Haut | sicher |
| A4 Weiss (L* > 80) im Auge | **0-1 px = 0,00-0,02**; keine Sclera. Anteil "heller als Haut" (Kern) 0,05-0,21 | - | 0,00 mit L*>80 (Ring erreicht L* 54-60); Anteil heller als Haut+8: **0,54** | sicher |
| A5 Glanzlicht | **ja**: heller Kern 2-3 x 3-4 px mittig-unten in der Mandel (x 0,49-0,60, y 0,62-0,72 der Augenbox); h118 x 185-187 y 41-43, L* 69, RGB (178,166,174); h125 x 205-207 y 41-44, L* 60, RGB (147,144,147); h109 zusaetzlich weisser Punkt L* 93 RGB (246,232,234) bei (193,26) am B-rechten Augenrand. Ob Glanz oder helle Iris: bei 3 px nicht trennbar | - | kein Glanzlicht; Ring + Kreuz | Lage sicher, Deutung geschaetzt |
| A6 Schulterauge | **ja**, auf der Schulter **links (B) = rechts (W)** (Sichelseite), sichtbar h101-h132 (birkin15-24; h130 nicht gefunden), in den Seitenansichten h83-h92 nicht erkennbar. Lage: 0,32-0,35 der Koerperbox-Breite von B-links, **0,12-0,13 H unter der Oberkante**; 0,11 H links (B) und 0,05 H tiefer als das Kopfauge (h118: (121,70) gegen (186,41)). 12-16 x 8-12 px = **0,021-0,025 H breit, 0,013-0,019 H hoch**, mandelfoermig, ohne hellen Kern (0-2 px). Lab Auge [15,3 6,0 2,6], Pupille [7,0 5,5 2,4], Lidrand [27,6 6,2 2,7], Haut [35,2 6,9 2,6]; L Auge/Haut 0,35-0,46 | - | 1 Ringauge B-links, gleiche Machart wie das Kopfauge (hell, rund), L Auge/Haut 2,13 | sicher |
| A7 Maul Breite/Hoehe | **waagerecht breit, B/H 1,5-1,8**: Huelle der Zahnflecken 50x33 / 51x29 / 48x28 px = **0,079-0,088 H breit, 0,046-0,058 H hoch** (h118/h125/h126); h109/h112 34x26 / 33x25 = 0,06 x 0,045 H. 0,83-0,88 der Kopfbreite | - | **senkrechtes Oval, B/H ~0,5**: ~122x240 px im Nahrender = 0,035 x 0,069 H (geschaetzt aus A_v1_kopfnah_crop) | Huelle gemessen; h128-h132 nicht trennbar |
| A7 Maul innen | dunkel Lab [12,1 13,4 4,7] (h118), [11,0 11,5 4,1] (h125); Anteil L*<18 in der Huelle 0,50-0,75; rote Lippen/Zahnfleisch Lab [27-36, 21-25, 6-8] | - | Lab [5,6 7,6 3,0] | sicher |
| A7 Zaehne | viele kleine helle Spitzen, je 1-2 x 1-3 px: Zahnflecken oben/unten 7/5 (h118), 6/2 (h125), 8/2 (h126), 9/2 (h112); Lab [57-62, 15-17, 4-5] (kein reines Weiss, rosa-grau); Form: kurze spitze Kegel in zwei schraegen Reihen | - | 2 durchgehende helle Zahnboegen (oben/unten), Lab [71,2 9,2 8,8] | Zaehlung geschaetzt (Flecken-Zaehlung, 1-2 px) |
| A8 Kopfbreite | 60 / 58 / 57 px = **0,094-0,105 H** (h118/h125/h126); h109/h112 0,12-0,125 H | Kopf nicht aufgeloest | ~0,097 H (Kopfplatte, geschaetzt) | Seiten gemessen |
| A8 Kopfhoehe | Oberkante bis Kinnunterkante 112 / 126 px = **0,20-0,21 H** (Kinn geschaetzt y 114 / 127) | - | Knochen 9 im Spielbild 30x50 px = 0,105 x 0,175 H | geschaetzt |
| A8 helle Kuppe / rote Zone (Kopfspalten, Baender ab Oberkante) | 0-0,05 H: L* 23, a* 7-8, rot (a*>15) 0,07-0,08; 0,05-0,10 H: L* 25, rot 0,01, hell (L*>50) 0,09-0,11 = **helle Kuppe mit Auge**; **0,10-0,25 H: rot 0,39-0,55, a* 14-15 = Maul + rotes Kinn**; ab 0,25 H rot 0,00-0,03 (h118/h125/h126) | - | Kopfplatte rosa, Maul mittig | sicher (A_kopfbox.json) |

## 2. Schwanz

**Die KI zeigt zwei verschiedene Schwaenze.** (a) Der **grosse Bogen** ueber dem Ruecken in h83-h97 (birkin11-14,
Seitenansicht): dunkelgrau mit rotbrauner Unterseite, grobe schuppige Textur, Ende = Hand mit Fingern. (b) In der
Halteszene h61-h73 (birkin05-10) ein **duenner roter Schwanz** mit einem hellen Band und spitzer Klinge am Ende.
Der Nutzer spricht vom "grossen Schwanz" = (a). Gemessen sind h83, h85, h92 (Bogen) und h61, h69, h70 (roter Schwanz).
Wurzelschnitt (Bogen): Kerbe = oberster Hintergrundpunkt der Luecke unter dem Bogen -> Naht = erste Stelle der Oberkontur,
ab der der Rumpf hell wird (mittleres L* 4-14 px unter der Kante > 40; Schwanz 20-35, Rumpf 43-56, Profil im Lauf).

| Mass | Wert KI (Bild, Koordinaten) | Original-Video (n1433-n1442) | Port V1 | Sicherheit |
|---|---|---|---|---|
| S1 Lab Schwanz (Bogen) | **[18,5 6,4 1,4]** RGB (55,43,44) h85; [18,6 6,0 1,4] h83; [16,7 7,1 2,3] h92 | **[10,5-12,7, 3,9-5,4, 1,8-2,4]** (Schwarzwert 17 ab) | **[24,3 21,4 9,1]** RGB (92,44,46) h85 (h83/h92 gleich +-0,6) | sicher |
| S1 Lab Koerperhaut (obere 0,45 H, ohne Schwanz/Jeans) | **[29,0 9,5 2,8]** RGB (86,65,66) h85; [29,8 10,0 2,9] h83; [25,8 8,6 1,9] h92 | [20,8-25,4, 7,6-9,2, 3,0-4,1] | [27,5 13,7 6,9] RGB (89,57,56) | sicher |
| S1 Jeans / Sichel | Jeans [16,6 -0,8 -8,2] (h85); Sichel [8,7 -1,9 3,9] (birkin19 x 10-105 y 250-440) | - | Jeans [28,0 -2,9 -12,2] | sicher |
| S1 dE Schwanz-Haut | **dE00 8,2 / 9,0 / 6,6** (h85/h83/h92), dE76 11,0 / 12,0 / 9,2; Schwanz-Jeans dE00 13,0; Schwanz-Sichel 13,2 | **dE00 7,3-10,4** (alle 10 Bilder); im KI-Bild mit derselben Maske 7,5-10,2 | dE00 5,5 / 6,7 / 5,3 (h85/h83/h92) | sicher |
| S1 Helligkeit L* Schwanz/Haut | **0,64 / 0,62 / 0,65** (dunkler als Haut) | **0,46-0,51** | **0,88** | sicher |
| S1 Buntheit C* / Farbton h | Schwanz C* 6,6, h 12 Grad; Haut C* 9,9, h 16 Grad: **gleicher Farbton, weniger bunt** | Schwanz C* 4,7, h 24 Grad; Haut C* 9,8, h 24 Grad: **gleicher Farbton, halb so bunt, halb so hell** | Schwanz **C* 23,3**, h 23 Grad; Haut C* 15,3, h 27: **Schwanz bunter als die Haut** | sicher |
| S1 Anteil rote Pixel (a* > 15) | Schwanz **0,05-0,11**, Haut 0,14-0,16 | r-g > 20: Schwanz 0,07-0,27, Haut 0,39-0,59 | Schwanz **0,85-0,92**, Haut 0,30-0,37 | sicher |
| S1 roter Schwanz der Halteszene (zum Vergleich) | Lab [22,9 22,7 5,5] RGB (88,39,48) h70; [23,6 20,0 7,0] h69; [21,3 16,4 4,2] h61; rot 0,62-0,76; dE00 zur Haut 6,4-9,8 | nicht gemessen | **V1-Schwanz gegen diesen roten Schwanz: dE00 1,6-2,9; gegen den grossen Bogen: dE00 12,5; gegen Original-Bogen: 15,9** | sicher |
| S2 Muster | **Marmorierung/Schuppentextur, keine Querbaender**: a* entlang der Mittellinie nach Trendabzug (31 px) Autokorrelation max 0,16-0,19 (Rest-Std a* 0,8-1,1); L* 0,19-0,24. **Laengszonen**: Haelften der Breite a* 7,6 / 5,1 (h85), 6,9 / 4,2 (h83): Oberseite grau, Unterseite rotbraun (A_3). Roter Schwanz h61-h70: ein einzelnes helles Band bei 0,50-0,65 der sichtbaren Laenge (a* faellt von 20-30 auf 3-8), sonst rot | im Original nicht aufloesbar (L* 10-13 bei Hintergrund 9-12) | **Querringe** sichtbar (08_rundum_V1, Schwanz nah); in der Spielkamera AK a* 0,17-0,32, L* 0,20-0,44, Rest-Std a* 1,9-3,5 = 2-3fach KI | KI sicher; V1-AK schwach, Ringe per Augenschein |
| S3 Dicke bei 0,1/0,2/0,3/0,5/0,7/0,9 der Bogenlaenge | **h85: 0,099 / 0,171 / 0,160 / 0,100 / 0,056 / 0,043 H** (50/86/81/51/28/22 px); h83: 0,098 / 0,168 / 0,189 / 0,122 / 0,076 / 0,085; h92: 0,090 / 0,150 / 0,158 / 0,092 / 0,052 / 0,036. Bogenlaenge 0,85 / 0,88 / 0,69 H | Formblatt V2 #7/#8 (Original 0,135 H bei 0,2, Innenhaelfte dunkel) | **0,176 / 0,153 / 0,140 / 0,121 / 0,034 / 0,032 H** (h85 ohne Verdeckung); Sprung von 0,10 auf 0,03 H bei 0,68-0,70 | sicher |
| S3 relativ zum Oberarm | Oberarm (W-links = B-rechts, Zeile am Nietenarmband): 63 px = 0,11 H (birkin19 y 221, x 278-340), 61 px = 0,10 H (birkin20 y 217). Schwanz/Oberarm h85: 0,9 / 1,6 / 1,5 / 0,9 / 0,5 / 0,4 | - | 1,7 / 1,5 / 1,3 / 1,2 / 0,3 / 0,3 | Oberarm-Zeile geschaetzt |
| S4 Glaette | Tangentenwinkel (Sehne +-0,01 H) Wurzel -> Ende h85: 151 -> 144 -> 128 -> 149 -> 147 -> 114 -> 107 -> 84 -> 57 Grad; gesamt ~95 Grad. **Max. Aenderung je 0,05 H: 36 Grad (h85), 29 (h83), 33 (h92)**; **Knicke > 20 Grad/0,05 H: 3 / 2 / 3** bei 0,26/0,46/0,81 (h85): Wurzelbogen, Ellbogen-artige Biegung bei ~0,46, Handgelenk bei ~0,8 | - | max **27-75 Grad je 0,05 H**, Knicke **2-5** (h83: 75 Grad bei 0,74; h85: 5 Knicke 0,45-0,88), Hakenende | sicher (Mittellinie A_2) |
| S5 Ende | **Seite h83-h92: Hand mit Fingern/Kralle** (Breite steigt bei 0,75-0,85 von 26-31 auf 33-39 px, dann Finger; rote Fingerspitze h83 (60,345)). **Halteszene h61-h73: spitze rote Klinge** nach dem hellen Band, verjuengt auf den letzten ~0,03 H unter 50 % der Mittelbreite (h69 Spitze (572,468), h70 (524,471)). h97 (birkin14): rotes, stumpf-spitzes Ende bei (0-60, 260-310). Vorderansichten h109-h132: Schwanz nicht frei sichtbar | Formblatt V2 #17: spitz/stumpf im Original nicht aufloesbar | spitzer Haken | sicher je Bild; die Bilder sind uneinig |
| S5 verjuengtes Ende | Bogen: letzte 0,08-0,10 H (ab 0,89 der Laenge; das ist die Hand); roter Schwanz: 0,024-0,033 H | - | ab 0,68-0,70 der Laenge (~0,22 H) duenn | sicher |
| S6 Wurzel | Kerbe (150,112) / (163,113) / (164,113) = **0,22 H unter OK**; Naht an der Oberkontur (209,9) / (212,11) / (189,14) = **0,02-0,03 H unter OK**; Schnitt Kerbe->Naht 119 / 113 / 102 px = **0,20-0,26 H** (Schnittmitte 0,12 H unter OK). Oberkontur laeuft ohne Stufe vom Buckel in den Schwanz | - | Mittellinienstart 0,08-0,10 H unter OK (h83-h92, ohne Verdeckung) | sicher |
| S6 Austrittsrichtung | erste 0,05 H der Mittellinie: 150 / 152 / 139 Grad (Bildwinkel, y nach unten) = nach hinten (B-links) und 30-40 Grad abwaerts; Rueckenlinie an der Naht steigt nach vorn um ~8 Grad (Oberkontur (212,8)->(260,1)), Schwanz setzt sie also um ~20-30 Grad abwaerts fort | - | 160-168 Grad = fast waagerecht nach hinten | wahrscheinlich |

### Original-Video n1433-n1442 (Antwort auf die Frage "dieselbe Haut?")

Masken = KI-Masken des naechsten Freistellers (birkin11 fuer n1433-1434, birkin12 fuer n1435-1438, birkin13 fuer n1439-1442),
halbiert, 3 px erodiert; Schwarzwert 17 abgezogen. Beleg `A_4_original_masken.png` (die Masken liegen auf Bogen und Rumpf).

| n | Schwanz Lab | Haut Lab | dE00 | L*-Verh. | Hintergrund neben dem Schwanz | KI gleiche Maske dE00 / L*-Verh. |
|---|---|---|---|---|---|---|
| 1433 (exakt) | [12,2 4,0 1,9] | [25,2 9,2 4,1] | 10,4 | 0,48 | [10,9 1,2 1,8] | 10,2 / 0,59 |
| 1434 | [12,7 3,9 1,8] | [25,4 9,2 4,0] | 10,4 | 0,50 | [11,0 1,1 1,8] | 10,0 / 0,60 |
| 1435 (exakt) | [12,5 4,3 1,9] | [24,4 8,9 4,0] | 9,5 | 0,51 | [11,9 1,2 1,6] | 9,2 / 0,61 |
| 1436 | [12,1 4,6 1,9] | [23,5 9,0 3,9] | 9,0 | 0,51 | [11,0 1,3 1,5] | 8,7 / 0,61 |
| 1437 | [11,8 4,8 2,1] | [23,1 8,7 3,5] | 8,6 | 0,51 | [10,7 1,2 1,5] | 8,6 / 0,60 |
| 1438 | [11,4 4,8 2,1] | [22,9 8,6 3,2] | 8,7 | 0,50 | [10,2 1,3 1,5] | 8,6 / 0,59 |
| 1439 | [10,7 5,4 2,2] | [23,1 8,4 3,2] | 8,9 | 0,46 | [10,4 1,1 1,8] | 9,1 / 0,57 |
| 1440 | [10,7 5,4 2,2] | [22,8 8,3 3,3] | 8,6 | 0,47 | [10,4 1,3 1,9] | 8,9 / 0,57 |
| 1441 | [10,6 4,9 2,3] | [21,8 8,1 3,1] | 8,2 | 0,49 | [9,7 1,6 2,0] | 8,4 / 0,59 |
| 1442 (exakt) | [10,5 5,2 2,4] | [20,8 7,6 3,0] | 7,3 | 0,50 | [9,2 2,0 2,1] | 7,5 / 0,61 |

Befund: Im Original hat der Bogen **denselben Farbton wie die Koerperhaut (h 24 Grad gegen 24 Grad)**, aber **halbe
Helligkeit (L* 0,46-0,51) und halbe Buntheit (C* 4,7 gegen 9,8)**; kein Rot-Ueberschuss (Rotanteil kleiner als in der Haut).
Das ist dieselbe Hautfamilie, stark abgedunkelt (dunkle Hautpartie oder Schatten, aus dem Video nicht trennbar). Die KI
zeigt dasselbe Verhaeltnis etwas heller (L*-Verh. 0,57-0,61, dE00 7,5-10,2): KI und Original stimmen ueberein. Einschraenkung:
der Bogen liegt nur 0,6-1,8 L* ueber dem Hintergrund daneben (a* 4-5 gegen 1-2); die Werte gelten als "wahrscheinlich".

## 3. Abweichungen Port V1 gegen Vorlage (KI)

1. **Schwanzfarbe (grosser Bogen):** V1 Lab [24,3 21,4 9,1], RGB (92,44,46) = gesaettigtes Lachsrot, C* 23,3; Vorlage Bogen
   Lab [18,5 6,4 1,4], RGB (55,43,44) = dunkles Grau-Braun, C* 6,6. **dE00 V1-Bogen 12,5** (zum Original-Bogen 15,9).
   V1 ist bunter als die eigene Haut (C* 23 gegen 15, Rotanteil 0,85-0,92 gegen 0,30-0,37); die Vorlage ist weniger bunt
   als die Haut (C* 6,6 gegen 9,9, Rotanteil 0,05-0,11 gegen 0,14-0,16) und dunkler (L*-Verh. 0,62-0,65, Original 0,46-0,51; V1 0,88).
   V1 hat fast genau die Farbe des **kleinen roten Schwanzes der Halteszene** (dE00 1,6-2,9), nicht die des grossen Bogens.
2. **Muster:** V1 Querringe (rote Ringbaender); Vorlage keine Querbaender (AK 0,16-0,19), sondern grobe Schuppen-/Marmortextur
   mit grauer Oberseite und rotbrauner Unterseite (a* 7-8 gegen 4-5).
3. **Form/Dicke:** V1 0,176 / 0,153 / 0,140 / 0,121 / 0,034 / 0,032 H; Vorlage h85 0,099 / 0,171 / 0,160 / 0,100 / 0,056 / 0,043 H.
   V1 ist an der Wurzel (0,1) 1,8x zu dick und faellt bei 0,68-0,70 in einem Schritt von ~0,10 auf 0,03 H; die Vorlage wird
   von 0,3 bis 0,9 gleichmaessig duenner und schwillt am Ende zur Hand (0,07-0,08 H bei 0,8-0,85).
4. **Glaette:** V1 max 27-75 Grad je 0,05 H, 2-5 Knicke, Hakenende; Vorlage max 29-36 Grad, 2-3 weite Biegungen
   (Wurzel, ~0,46, Handgelenk ~0,8), Gesamtdrehung ~95 Grad. Der 75-Grad-Knick (h83 bei 0,74) hat in der Vorlage kein Gegenstueck.
5. **Ende:** V1 spitzer Haken; Vorlage Seite = Hand mit Fingern/Kralle, Halteszene = spitze Klinge. Offen, welches gilt (unten).
6. **Wurzel:** V1 Mittellinie beginnt 0,08-0,10 H unter OK und tritt fast waagerecht aus; Vorlage: breite Wurzel von der
   Oberkontur (0,02-0,03 H) bis zur Kerbe (0,22 H), Austritt 30-40 Grad abwaerts, Oberkontur ohne Stufe.
7. **Kopfauge Form:** V1 rund (B/H 1,0) mit hellem Ring und Kreuz; Vorlage Mandel (B/H ~1,9), fast ganz dunkel.
8. **Kopfauge Helligkeit:** V1 Auge/Haut **1,69** (heller als die Haut), Anteil hell 0,54; Vorlage **0,46-0,48**, kein Weiss
   (L*>80: 0-1 px), nur ein 2-3 x 3-4 px heller Kern (L* 60-69) mittig-unten. Groesse passt (V1 0,017-0,021 H, Vorlage
   0,019-0,024 H breit), Hoehe nicht (Vorlage 0,010-0,013 H).
9. **Haut um das Auge:** Vorlage helle grauviolette Kuppe (L* 44-51, a* 7,5, b* 0) um ein dunkles Auge; V1 dunkle rosa
   Platte (L* 22,5 im Nahrender) um ein helles Auge: Kontrast umgekehrt.
10. **Schulterauge:** gleiche Seite (B-links = W-rechts). V1 hell (Auge/Haut 2,13, Ring); Vorlage dunkel (0,35-0,46), ohne Kern,
    12-16 x 8-12 px = 0,021-0,025 x 0,013-0,019 H, 0,12-0,13 H unter der Oberkante, 0,05 H tiefer als das Kopfauge.
11. **Maul:** V1 senkrechtes Oval (B/H ~0,5, ~0,035 x 0,069 H) mit zwei durchgehenden weissen Zahnboegen (L* 71);
    Vorlage waagerecht breit (B/H 1,5-1,8, 0,079-0,088 x 0,046-0,058 H, 0,83-0,88 der Kopfbreite), viele kleine
    rosa-graue Zahnspitzen (L* 57-62, 1-2 px; oben 6-9, unten 2-5), rote Lippen (a* 21-25), darunter rotes Kinn
    (rote Zone 0,10-0,25 H unter der Oberkante, Anteil 0,39-0,55).

## 4. Offen / nicht belegt

- **Welcher Schwanz gilt?** Die KI zeigt den grossen dunkelgrauen Bogen mit Hand (h83-h97) und den duennen roten Schwanz mit
  Klinge (h61-h73). V1 hat die Farbe des roten (dE00 1,6-2,9). Die Rueckmeldung "grosser Schwanz, falsche Farbe" passt zum
  Bogen; Entscheidung beim Nutzer.
- Ende Bogen: Hand mit Kralle (KI-Seitenansicht) gegen spitz (Halteszene); im Original nicht aufloesbar (Formblatt V2 #17).
- Kinn-Unterkante (Kopfhoehe 0,20-0,21 H) und die Oberarm-Messzeile sind geschaetzt.
- Ob der helle Kern im Auge ein Glanzlicht oder eine helle Iris ist: bei 3 px nicht trennbar.
- Schulterauge in den Seitenansichten h83-h92 und in h130 nicht gefunden; ob verdeckt oder fehlend, ist offen.
- Zahnzahl ist eine Fleckenzaehlung bei 1-2 px Zahngroesse (geschaetzt), h128-h132 nicht trennbar.
- Original-Bogen nur knapp ueber dem Hintergrund (L* +0,6 bis +1,8): Farbwerte "wahrscheinlich", nicht sicher.
- Port-V1-Schwanzmuster: die Querringe sind im Augenschein klar (08_rundum_V1.jpg), die Autokorrelation in der Spielkamera
  ist schwach (0,17-0,44); fuer den Port-Vergleich zaehlt die Rest-Streuung a* 1,9-3,5 gegen 0,8-1,1 in der Vorlage.
- Feld auge_weiss_anteil im JSON: KI = Anteil L*>80 (0,00); Port = Anteil "heller als Haut+8" (0,54), weil der V1-Ring im
  Render nur L* 54-60 erreicht (L*>80: 0,00). Mit gleichem Kriterium "heller als Haut" hat die KI 0,05-0,21 (nur der Kern).
- Hinweis zum Scratchpad: Beim ersten Bildexport habe ich n1431-n1444 in den fremden Ordner sfv3/orig/ geschrieben (Leser B).
  Nachgeprueft: n1429/1430/1433/1435/1445 dort und in meinem Export sind bitgleich (mittlere Abweichung 0,0), die
  ueberschriebenen Dateien haben denselben Inhalt. Meine Bilder liegen jetzt in sfv3/A_orig/ und sfv3/A_slp/.
