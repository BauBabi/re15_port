# 06 - Massstab zwischen Raumwelt und Tuerszene

Stand 2026-09-28. Werkzeug `re15_port/tools/tor/tor_massstab.py`, Messwerte
`build/tor_1170/massstab.json`, Kontrollbilder `build/tor_1170/massstab_*.png`.
Alles hier ist GEMESSEN; ein Faktor steht in keinem Code (die Tuerszene ist eine eigene Welt).

Zeichen: `[ASM]` Instruktion gelesen, `[DATEI]` Bytes gelesen, `[MESS]` am Hintergrundbild gemessen,
`[RECH]` aus Messwerten gerechnet.

---

## 1. Ergebnis

**k = Einheiten der Tuerszene je Raumwelt-Einheit.**

| Groesse | RE1.5 (5 Tueren, 3 Raeume) | RE2 (6 Tueren, 6 Raeume) | beide (11 Tueren) |
|---|---|---|---|
| Blatthoehe im Raum | 3536 +- 135 | 3567 +- 142 | 3553 +- 133 |
| Blattbreite im Raum | 1953 +- 167 | 1640 +- 215 | 1782 +- 247 |
| Hoehe / Breite | 1,825 +- 0,226 | 2,201 +- 0,261 | 2,030 +- 0,305 |
| **k aus der Hoehe** (6602 / H) | **1,869 +- 0,069** | **1,853 +- 0,073** | **1,860 +- 0,068** |
| **k aus der Breite** (3599 / W) | **1,854 +- 0,166** | **2,223 +- 0,261** (nur Guete A: 2,362 +- 0,122) | 2,055 +- 0,287 |

"+-" ist die Streuung von Tuer zu Tuer (Standardabweichung). Fehler des Mittels fuer k aus der Hoehe
ueber alle 11 Tueren: +- 0,021. Median 1,890, kleinster Wert 1,753, groesster 1,935.

Aussagen:

1. **k aus der Hoehe ist in beiden Spielen gleich: 1,86.** RE1.5 und RE2 haben denselben
   Raummassstab (Blatthoehe 3536 gegen 3567, Spielermodell 3022 gegen 3037).
2. **In RE1.5 stimmen Hoehe und Breite ueberein** (1,869 gegen 1,854). Das Standardblatt hat mit
   6602 / 3599 = 1,834 dasselbe Seitenverhaeltnis wie die RE1.5-Tueren (1,825).
3. **In RE2 liefern Hoehe und Breite verschiedene k** (1,853 gegen 2,223 bzw. 2,362). Die
   RE2-Tueren sind im Raum schmaler (Mittel 1527 bei Guete A, 1640 mit Guete B) als die
   RE1.5-Tueren (1953), die Tuerszene zeigt aber dasselbe Blatt. Diese Werte werden nicht gemittelt.
4. **Empfehlung fuer das Tor in ROOM1170 (RE1.5): k = 1,86 +- 0,07.** Fuer Hoehe und Breite
   gemessen, fuer die Tiefe uebernommen (nicht messbar, siehe Abschnitt 9).

### Tabelle Raumwelt -> Tuerszene

| Raumwelt | Tuerszene bei k = 1,860 | Spanne (k 1,792 .. 1,928) | nur RE2-Breite (k = 2,362) |
|---|---|---|---|
| 500 | 930 | 896 .. 964 | 1181 |
| 1000 | 1860 | 1792 .. 1928 | 2362 |
| 1500 | 2791 | 2689 .. 2893 | 3543 |
| 2000 | 3721 | 3585 .. 3857 | 4724 |

Bezugsmasse zum Einordnen `[RECH]`:

| Was | Raumwelt | Tuerszene |
|---|---|---|
| Standardblatt Hoehe | 3549 | 6602 |
| Standardblatt Breite (RE1.5-Faktor 1,854) | 1941 | 3599 |
| Knauf ueber der Unterkante | 1733 | 3224 |
| Spieler, Scheitel (Modell RE1.5) | 3022 | 5621 |
| Spieler, Hueftgelenk (Skelettwurzel) | 1804 | 3355 |
| Blattdicke | 155 | 288 |

Der Knauf liegt 71 Raumeinheiten unter dem Hueftgelenk des Spielers: "huefthoch" heisst in der
Tuerszene rund 3200 .. 3350 Einheiten.

---

## 2. Grundlagen mit Beleg

| Was | Wert | Beleg |
|---|---|---|
| Standardblatt | x -145..143, y -6600..2, z -3599..0 = 288 x 6602 x 3599 | `[DATEI]` mesh0 von RE1.5 `DOOR00.DO2`; identische Eckenmenge in 36 von 55 RE2-Archiven (`zensus`) |
| Archive mit Standardblatt | 00 01 02 03 04 05 06 07 08 09 0B 0C 0D 11 13 15 17 18 19 1A 1B 1C 1D 20 21 22 23 24 25 29 2A 2C 2F 32 34 36 | `[DATEI]` `zensus` |
| davon hoechstens 2 Meshes (Einzelblatt) | 30 Archive (ohne 01 05 17 19 29 2C) | `[DATEI]` `zensus` |
| Tuersaetze RE2 mit Standardblatt | 365 von 552 `Door_aot_set` (0x3B) | `[DATEI]` `zensus`, Archiv = Satz + 26 |
| Projektionsabstand | H = fov >> 7 | `[ASM]` RE1.5 `@0x80021e68 lhu a0,2(v0)`, `@0x80021e6c jal 0x80066c30`, `@0x80021e70 srl a0,a0,7`; RE2 `@0x8002c138 lhu a0,2(v0)`, `@0x8002c13c jal 0x8008de24`, `@0x8002c140 srl a0,a0,7`. Ziel beider Spruenge beginnt mit `ctc2 a0,$26` (Wort 0x48C4D000) |
| Kamerasatz | 32 B je Cut, Tabelle = RDT-Zeiger +36 | `[ASM]` beide Spiele `lw v1,36(v1)` / `sll v0,v0,5` (RE1.5 `@0x80021e5c/60`, RE2 `@0x8002c12c/30`) |
| Kameraformat RE2 = RE1.5 (fov in den gemessenen RE2-Cuts 24129, 26684, 29623, 32946 = H 188, 208, 231, 257; RE1.5 durchweg 26684 = 208) | u16 flag, u16 fov, s32 Ort[3], s32 Ziel[3], u32 pri | `[DATEI]` z.B. ROOM3060 `@Datei 0x64`: `00 00 3c 68 1a b6 ff ff f8 dd ff ff c6 af ff ff 06 bb ff ff 74 e3 ff ff ba a0 ff ff f8 05 00 00` = fov 26684, Ort (-18918,-8712,-20538), Ziel (-17658,-7308,-24390) |
| Bodenhoehe | y = -Band * 1800 | `[DATEI]` `bodenzensus` ueber die Raeume `ROOMxxx0`: Ziel-y == -Ziel-Band * 1800 in 307 von 310 (RE2) und 324 von 326 (RE1.5) Tuersaetzen; Abweichler RE2 Band 0 / y -32000 (2x), -129 (1x); RE1.5 Band 0 / y -26460, -25220 |
| Tuersatz | 0x3B, 32 B: +1 aot, +4 Band, +6 x, +8 z, +10 w, +12 d, +14/16/18 Ziel, +26 Archiv, +27 Variante | `[DATEI]` z.B. ROOM3060 `@Datei 0x1DE6`: `3b 00 01 31 03 00 dd bc c7 90 b8 06 fa 05 ...` = Band 3, Rechteck (-17187,-28473,1720,1530), Archiv 07 |
| RE2-Kollision (SCA) | Kopf 16 B (s16 cx, s16 cz, u32 Anzahl+1, s32 Decke, u32 0xc5c5c5c5), Zelle 16 B (s16 x, s16 z, u16 w, u16 d, u16 Kennung, u16 Typ, u32 Boden) | `[DATEI]` ROOM1000 `@Datei 0xED8`: 21 Zellen enden genau an der naechsten Sektion 0x1038 |

---

## 3. Verfahren

Je Tuer (Verfahren A, traegt das Ergebnis):

1. Die vier Ecken des BLATTS (ohne Zarge) am vergroesserten Hintergrund naehern
   (`lupe`, Tabelle `TUEREN` im Werkzeug).
2. Jede Kante verfeinern: an jedem Pixel der mittleren 70 % das Gradientenmaximum quer zur Kante,
   Gerade durch die Maxima (Ausreisser ueber 1 Pixel verworfen). Wo eine dunkle Fuge das Blatt
   umgibt, gilt die blattseitige Flanke der Fuge. Ecken = Schnittpunkte der vier Geraden.
3. Ein Rechteck in einer senkrechten Ebene, Unterkante auf Bodenhoehe, mit 5 Groessen
   (x0, z0, Richtung, Breite, Hoehe) an die 8 Eckkoordinaten anpassen. Kamera = Gleitkommafassung
   der Port-Blickmatrix (`tor_kamera.Kamera(exakt=True)`), Bildmitte (160,120).
4. k aus der Hoehe = 6602 / H, k aus der Breite = 3599 / W.

Was das Verfahren prueft und was nicht:

- Der **Restfehler** (rms) sagt, ob das Viereck im Bild ein Rechteck in einer Wand sein KANN
  (3 ueberzaehlige Messwerte). 9 von 11 Tueren liegen unter 0,5 Pixel.
- Den **Massstab** traegt allein die Annahme "Blattunterkante = Bodenhoehe". Sie ist je Tuer nur
  ueber die Gegenproben in Abschnitt 6 abgesichert.
- Die angegebene Unsicherheit je Tuer ist die Fortpflanzung von 1 Pixel je Eckkoordinate.

Wertung: eine Tuer zaehlt, wenn die Kamera mindestens 1000 ueber dem Boden steht (sonst traegt der
Bodenanker nicht), der Restfehler hoechstens 1,0 Pixel ist und die Blattgrenze eindeutig ist.

---

## 4. Messwerte

Spalten: Guete, Raum/Cut, Kamerasatz und H, Tuersatz `@Datei`, Archiv/Variante, Band, Kamerahoehe
ueber dem Boden, Blatt im Bild (Breite x Hoehe in Pixeln), H und W in Raumeinheiten, H/W,
k Hoehe, k Breite, Restfehler, Abstand der Blattebene zur naechsten AOT-Kante / SCA-Kante.

### 4.1 RE2 Retail `[MESS]`

| G | Raum | Kamera | Satz | Arch | Band | Kam.-Hoehe | Pixel | H | W | H/W | k Hoehe | k Breite | rms | AOT / SCA |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| A | ROOM60B0 c4 | 0xE4 H 231 | `0x01196` | 1D/1 | 0 | 2898 | 55,5 x 120,4 | 3412 +- 18 | 1585 +- 36 | 2,152 | 1,935 | 2,270 | 0,04 | +104 / -835 |
| A | ROOM3060 c0 | 0x64 H 208 | `0x01DE6` | 07/1 | 3 | 3312 | 41,6 x 101,6 | 3492 +- 21 | 1506 +- 57 | 2,318 | 1,890 | 2,389 | 0,14 | +28 / -20 |
| A | ROOM5050 c3 | 0xC4 H 231 | `0x00B2E` | 07/0 | 0 | 1098 | 54,0 x 136,3 | 3710 +- 49 | 1426 +- 36 | 2,601 | 1,780 | 2,524 | 0,16 | +111 / -968 |
| A | ROOM20E0 c3 | 0xC4 H 208 | `0x01716` | 1C/1 | 0 | 2970 | 39,8 x 87,4 | 3597 +- 28 | 1589 +- 43 | 2,264 | 1,835 | 2,265 | 0,09 | +898 / -302 |
| B | ROOM2040 c0 | 0x64 H 257 | `0x018DE` | 09/0 | 0 | 4482 | 57,3 x 112,5 | 3441 +- 18 | 1697 +- 39 | 2,028 | 1,919 | 2,121 | 0,78 | schraeg |
| B | ROOM40A0 c8 | 0x164 H 208 | `0x02522` | 1C/0 | 1 | 3168 | 52,6 x 98,7 | 3750 +- 23 | 2037 +- 44 | 1,841 | 1,760 | 1,767 | 0,47 | +500 / -1256 |

### 4.2 RE1.5 `[MESS]`

| G | Raum | Kamera | Satz | Arch | Band | Kam.-Hoehe | Pixel | H | W | H/W | k Hoehe | k Breite | rms | AOT / SCA |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| A | ROOM1000 c3 | 0xC0 H 208 | `0x00BDE` | 00/0 | 0 | 1052 | 60,3 x 116,5 | 3478 +- 51 | 1840 +- 52 | 1,891 | 1,898 | 1,956 | 0,24 | -412 / -12 |
| A | ROOM1000 c6 | 0x120 H 208 | `0x00BFE` | 00/0 | 0 | 3011 | 68,4 x 114,3 | 3541 +- 19 | 2098 +- 38 | 1,688 | 1,864 | 1,715 | 0,27 | +85 / -515 |
| A | ROOM1010 c0 | 0x60 H 208 | `0x008AE` | 00/0 | 0 | 2856 | 65,0 x 106,9 | 3468 +- 21 | 2060 +- 45 | 1,683 | 1,904 | 1,747 | 0,59 | +163 / -387 |
| A | ROOM1010 c4 | 0xE0 H 208 | `0x008CE` | 00/0 | 0 | 3419 | 69,6 x 110,5 | 3427 +- 18 | 2051 +- 33 | 1,670 | 1,927 | 1,754 | 0,49 | schraeg |
| A | ROOM2060 c0 | 0x60 H 208 | `0x00D9A` | 00/0 | 0 | 1461 | 46,9 x 112,1 | 3767 +- 44 | 1716 +- 67 | 2,195 | 1,753 | 2,097 | 0,26 | -49 / -599 |

In RE1.5 tragen alle Tuersaetze Archiv 0 (einziges Archiv `DOOR00`).

### 4.3 Blattecken im Bild (TL, TR, BR, BL; kontinuierlich, Pixel i deckt [i, i+1))

| Tuer | Naeherung von Hand | verfeinert = Messwert |
|---|---|---|
| ROOM60B0 c4 | (126,0 31,0) (186,0 31,5) (182,0 148,5) (130,5 152,0) | (126,13 29,83) (185,77 30,85) (182,23 148,35) (131,09 152,92) |
| ROOM3060 c0 | (120,5 40,3) (166,5 40,3) (166,5 143,8) (125,3 141,0) | (121,05 42,32) (165,86 41,38) (164,77 146,08) (126,74 140,64) |
| ROOM5050 c3 | (79,5 31,0) (133,5 36,5) (131,5 167,0) (80,0 170,0) | (80,72 30,38) (134,30 36,29) (133,55 168,25) (79,46 171,04) |
| ROOM20E0 c3 | (93,0 75,5) (133,0 76,0) (134,5 162,5) (96,5 164,0) | (93,86 75,70) (134,62 76,82) (136,13 161,86) (97,48 165,33) |
| ROOM2040 c0 | (214,5 10,0) (280,5 13,0) (258,5 123,0) (206,5 121,0) | (217,37 11,36) (281,34 13,88) (257,11 122,86) (206,56 124,15) |
| ROOM40A0 c8 | (116,0 14,0) (170,0 14,0) (167,0 113,0) (119,0 113,0) | (112,49 11,66) (169,48 13,40) (167,78 111,15) (119,68 111,06) |
| ROOM1000 c3 | (135,5 57,5) (195,5 51,5) (196,5 172,0) (135,0 171,5) | (136,49 58,55) (195,48 52,59) (196,37 173,79) (135,06 170,42) |
| ROOM1000 c6 | (153,5 36,0) (227,5 36,0) (220,5 153,0) (154,5 153,0) | (155,80 37,11) (230,12 36,98) (219,82 150,97) (157,38 151,31) |
| ROOM1010 c0 | (62,5 55,0) (131,0 55,0) (131,5 163,5) (70,5 164,5) | (63,21 56,91) (130,70 57,18) (133,07 163,20) (70,64 164,35) |
| ROOM1010 c4 | (166,5 49,0) (238,5 49,0) (229,5 161,0) (165,5 156,0) | (166,46 50,01) (240,72 50,02) (228,73 162,18) (164,00 158,11) |
| ROOM2060 c0 | (140,5 66,0) (186,5 58,5) (187,5 176,0) (140,5 174,0) | (142,06 66,51) (187,87 58,98) (189,29 176,95) (142,02 172,80) |

Beispiel, dass die Verfeinerung die Hand korrigiert: ROOM1000 c6, rechte Fuge. Dunkelstes Pixel je
Zeile: y 40 -> x 230, y 60 -> x 229, y 80 -> x 226, y 100 -> x 225, y 120 -> x 223, y 140 -> x 221.
Die Hand hatte oben 227,5 angesetzt, die Verfeinerung findet 230,12.

Kontrollbilder je Tuer: `massstab_<spiel>_<raum>_c<cut>_blatt.png` (Blatt 5-fach, rot = verfeinerte
Kanten, gruen = angepasstes Rechteck, gelb = Naeherung), `..._ecken.png` (die vier Ecken 10-fach),
`..._boden.png` (Vollbild 3-fach mit AOT-Rechteck gruen und SCA-Zellen blau auf Bodenhoehe).

---

## 5. Ausgeschlossene Tueren und Ausreisser

| Tuer | Kam.-Hoehe | H | W | k Hoehe | k Breite | rms | Ursache |
|---|---|---|---|---|---|---|---|
| RE2 ROOM5050 c0 | 2790 | 3348 +- 26 | 1349 +- 49 | 1,972 | 2,669 | 0,40 | Wiederholprobe: dieselbe Tuer wie 5050 c3 aus einer zweiten Kamera |
| RE2 ROOM21A0 c0 | 3960 | 3468 +- 24 | 1973 +- 45 | 1,904 | 1,824 | 1,15 | mehrere Zargenlinien, Blattgrenze nicht eindeutig |
| RE2 ROOM20C0 c0 | 486 | 4275 +- 207 | 2024 +- 119 | 1,544 | 1,779 | 0,12 | Kamera nur 486 ueber dem Boden |
| RE2 ROOM6140 c0 | 450 | 4006 +- 158 | 1588 +- 77 | 1,648 | 2,266 | 0,67 | Kamera nur 450 ueber dem Boden |
| RE2 ROOM2020 c0 | 522 | 3606 +- 120 | 1471 +- 93 | 1,831 | 2,447 | 0,36 | Kamera nur 522 ueber dem Boden |
| RE1.5 ROOM1130 c6 | 4122 | 3707 +- 16 | 2142 +- 34 | 1,781 | 1,680 | 0,57 | gemessen ist der Aussenrand der Zarge, nicht das Blatt |
| RE1.5 ROOM1140 c6 | 838 | 3361 +- 64 | 1566 +- 51 | 1,964 | 2,299 | 0,46 | Kamera nur 838 ueber dem Boden, Unterkante dunkel |
| RE1.5 ROOM3090 c0 | 5058 | 3600 +- 22 | 1827 +- 36 | 1,834 | 1,969 | 1,54 | Viereck passt nicht zu einem Rechteck in der Wand |

Zwei der drei RE2-Tueren mit tiefer Kamera liefern die groessten Hoehen ueberhaupt (4006, 4275), die
dritte 3606. Die gerechnete Unsicherheit ist dort 120 bis 207 Einheiten statt 18 bis 51: 1 Pixel an
der Unterkante verschiebt die Blattebene um mehrere Hundert Einheiten. Sie sind KEIN Hinweis auf
groessere Tueren.

Innerhalb der gezaehlten Tueren liegen die kleinsten k aus der Hoehe bei ROOM2060 (1,753),
ROOM40A0 (1,760) und ROOM5050 (1,780). ROOM2060 und ROOM5050 gehoeren zu den drei niedrigsten
gezaehlten Kameras (1461, 1098; die dritte, ROOM1000 c3 mit 1052, liefert 1,898); bei ROOM40A0 ist
die Unterkante schwach (Guete B). Eine einzelne Ursache fuer die drei kleinen Werte ist nicht belegt.

---

## 6. Gegenproben am Verfahren

| Probe | Ergebnis | Befund |
|---|---|---|
| Wiederholprobe: dieselbe Tuer aus zwei Kameras (ROOM5050 c3 / c0) | H 3710 / 3348, W 1426 / 1349; Blattebene x -10617 / -11167 | **10 % in der Hoehe, 5,5 % in der Breite.** Das aus c3 gewonnene Rechteck, in c0 projiziert, liegt oben 7,7 bis 9,1 Pixel und unten 4,0 bis 4,5 Pixel ueber dem Blatt im Bild, seitlich 1,2 bis 3,0 Pixel links davon. Die beiden Kamerasaetze und die beiden Bilder beschreiben nicht exakt dieselbe Tuer |
| Blattebene gegen naechste AOT-Kante (9 achsparallele Tueren) | +104, +28, +111, +898, +500, -412, +85, +163, -49 | 6 von 9 innerhalb 165 Einheiten = hoechstens 2,4 % der Entfernung |
| Blattebene gegen naechste SCA-Kante | -835, -20, -968, -302, -1256, -12, -515, -387, -599 | Die Kollisionszellen liegen 12 bis 1256 Einheiten von der Blattebene entfernt. **Die Kollision taugt nicht als Wandebene**; nur ROOM3060 (-20) und ROOM1000 c3 (-12) treffen |
| RE1.5: Laenge des AOT-Rechtecks entlang der Wand | ROOM1000 c6: Blatt 2088..4123, AOT 2200..4200; ROOM1010 c0: Blatt 5985..8008, AOT 5900..7900; ROOM1000 c3: Blatt -818..1036, AOT -900..1100; ROOM2060 c0: Blatt 3392..5066, AOT 2950..4950 | Bei 3 von 4 achsparallelen RE1.5-Tueren liegen beide Enden des 2000 langen Tuer-Rechtecks hoechstens 112 Einheiten neben den Blattenden. ROOM2060 passt nicht (Blatt 1674 breit, um 116 ueber das Rechteck hinaus). Ein Hinweis auf den Bodenmassstab, kein Beweis |
| Richtung auf die naechste Achse festgelegt (4 statt 5 Groessen) | H aendert sich um hoechstens 0,03 %, W um -4,8 % .. +1,7 %; rms steigt bei ROOM2040, ROOM1000 c6, ROOM1010 c0/c4 auf 1,5 .. 2,4 | **Die Hoehe haengt nicht an der Richtung.** Die Breite schon |
| Port-Matrix (ganzzahlig, SquareRoot0) statt Gleitkomma | H hoechstens 0,25 %, W hoechstens 0,67 % anders | vernachlaessigbar |
| Handecken statt verfeinerter Ecken | H hoechstens 1,74 %, W hoechstens 5,14 % anders | die Verfeinerung wirkt vor allem auf die Breite |
| Gegentuer im Nachbarraum | zu jeder der 11 Tueren gefunden; Ziel-y = 0 bzw. -5400 (ROOM3060, Band 3) bzw. -1800 (ROOM40A0, Band 1) | Bodenhoehe bestaetigt |

---

## 7. Gegenprobe ohne Bild: Spielerhoehe

| Was | Wert | Beleg |
|---|---|---|
| Trefferkasten des Spielers | Versatz (0,-1530,0), Radius 450, halbe Hoehe 1530 -> y -3060 .. 0 | `[DATEI]` RE1.5 `PSX.EXE` `@0x80073e94` = Datei 0x64694: `00 00 06 fa 00 00 c2 01 fa 05 c2 01`; im Port `re15_port/engine/src/re15_damage.c:3448-3449` und `:3460`, Feld `re15_port/include/re15_actor.h:105` |
| Spielermodell RE1.5 | Skelettwurzel (0,-1804,0); Grundhaltung (alle Winkel 0) y -1215 .. 1807 um die Wurzel = **3022** | `[DATEI]` `re15_port/shared_assets/PSX/PLD/PL00.PLD`, EMR `@Datei 0xC60`, Wurzel `@Datei 0xC68`: `00 00 f4 f8 00 00` |
| Spielermodell RE2 | Skelettwurzel (0,-1810,0); Grundhaltung 3037 | `[DATEI]` `info/re2leon/PL0/PLD/PL00.PLD`, EMR `@Datei 0x59C`, Wurzel `@Datei 0x5A4`: `00 00 ee f8 00 00` |
| Blatt / Spieler ohne Massstab | 6602 / 3022 = 2,185 | `[RECH]` - eine Tuer von 2,2 Koerperhoehen gibt es nicht; der Faktor ist also deutlich groesser als 1 |
| Knauf / Spieler ohne Massstab | 3224 / 3022 = 1,067 | `[RECH]` - der Knauf laege ueber dem Scheitel |
| mit k = 1,86 | Blatt 3549 = 1,17 Koerperhoehen; Knauf 1733 = 0,57 Koerperhoehen, 71 unter dem Hueftgelenk | `[RECH]` |
| Aufstellhoehe in der Tuerszene | Blattunterkante 3790 unter der Kameraachse | `[DATEI]` RE2 `DOOR00.DO2` Skript 1 `@Datei 0x5036`: `4d 00 00 00 01 00 a0 0a 10 00 d0 07 ce 0e 00 08 ...` = Ort (2000,3790,2048) |
| Kamerahoehe der Tuerszene in Raumeinheiten | 3790 / 1,86 = 2038 = 0,67 Koerperhoehen | `[RECH]` - Brusthoehe, nicht Augenhoehe |

Die Gegenprobe liefert keinen eigenen Wert fuer k, schliesst aber k = 1 und k >= 3 aus und passt zu
1,86: eine Tuer von 1,17 Koerperhoehen mit dem Knauf auf Huefthoehe.

---

## 8. Nachmessen

```
T=re15_port/tools/tor/tor_massstab.py
python $T alles                 # Zensus, alle Tueren, Auswertung, Gegenprobe, schreibt JSON + Bilder
python $T messen 3060           # eine Tuer (Raumname)
python $T zensus                # Archive mit Standardblatt, Tuersaetze
python $T bodenzensus           # Ziel-y gegen Ziel-Band
python $T gegenprobe 1.86       # Spielerhoehe, Trefferkasten
python $T wiederholprobe        # ROOM5050 aus Cut 3 und Cut 0
python $T lupe re15 1010 4 155 35 255 175 5 hell     # Ausschnitt mit Pixelraster
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80021e54 9
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x8002c120 10
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py bytes 0x80073e94 16
python re15_port/tools/tor/tuerskript_dump.py dis info/re2leon/COMMON/DOOR/DOOR00.DO2 1
```

---

## 9. Offen

1. **Bodenanker.** Ob die Blattunterkante wirklich auf Bodenhoehe liegt (Schwelle, Spalt), ist je
   Tuer nicht belegt. Liegt sie e ueber dem Boden, ist das Blatt um e / Kamerahoehe kleiner. Die
   AOT-Gegenprobe begrenzt das auf rund 2,4 %, aber nur fuer 6 von 9 Tueren.
2. **Kamerasatz gegen gerendertes Bild.** Die Wiederholprobe ROOM5050 weicht um 10 % ab. Welche der
   beiden Kameras nicht zum Bild passt, ist nicht geklaert. Das begrenzt die Genauigkeit einer
   einzelnen Tuer auf etwa +- 5 % und ist der Grund, warum nur das Mittel vieler Tueren zaehlt.
3. **Blatt gegen Zarge.** Wo keine Fuge zu sehen ist, kann die gemessene Kante bis 1 Pixel daneben
   liegen (1,5 bis 2,5 % der Breite). Die Breite ist deshalb unsicherer als die Hoehe.
4. **Warum RE2-Tueren schmaler sind** als das Standardblatt, ist nicht untersucht. Gemessen ist nur,
   DASS sie es sind (4 Tueren Guete A: 1426 .. 1589).
5. **Tiefenrichtung.** Die Blattdicke ist im Hintergrund nicht messbar. 155 Raumeinheiten fuer 288
   Tuereinheiten ist gerechnet, nicht gemessen.
6. **Formkennung der RE2-Kollisionszellen** (unteres Halbbyte der Kennung) ist nicht entschluesselt.
   In den Kontrollbildern ist jede Zelle als umschliessendes Rechteck gezeichnet.
7. **Das Tor selbst** ist hier nicht vermessen. Fuer ROOM1170 gilt: Cut 12 steht 56 ueber dem
   Boden, dort traegt der Bodenanker nicht; Cut 0 und Cut 11 sind zu pruefen.
8. **Doppeltueren** (Archive 01 05 17 19 29 2C) sind nicht gemessen.
