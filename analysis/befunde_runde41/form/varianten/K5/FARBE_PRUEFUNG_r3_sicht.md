# K5c Farbe Runde 3: Sicht-Pruefung (Pruefer "Auge des Nutzers", 2026-11-06)

Gegenstand: Tafeln 01-07 in `varianten/K5/farbe/` (Stand 22:46-22:51), die zehn Spielbilder `farbe/spiel/` (K5c neu Runde 3 gegen K4 heute,
`spiel_lauf.log` 10/10 rc=0, gdigrab Versuch 1), Dossier `FARBE.md` (Riegel 28/33, ROT: K1, K2, K4, B2, B9), die beiden Originale
`Version_80%/vlcsnap-2026-10-31-00h28m45s774.png` (Bild 1) und `...00h29m02s830.png` (Bild 2). Massstab: Nutzer 2026-11-06 ("passt von den Farben
und von der Struktur ueberhaupt nicht ... nicht zum Rest des Spiels ... wie ein Pokemon"), frueher E8/E10/E11/E12/E14/E15 (Haut mehrfarbig, Wunden
fleischig, PSX-Stil, aus einem Guss, geordnet, Farben aus dem Screenshot, kein Cel-Shading). Verglichen mit EM30_HANGAR, RE2-G5, RE2-G4 auf denselben
Tafeln. Nichts am Code geaendert, nichts committet. Seiten (W) = Wesen, (B) = Betrachter. Lab = CIE-Lab D65 (OpenCV float), gemessen an Tafel- bzw.
Spielbild-Pixeln. Ausschnitte (2x/3x nearest) und Messskripte:
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/r3sicht/`
(`crops.py`, `mess.py`, `mess2.py`; `03_seite_G4_K5c_K4_x2.png`, `03_34vorn_*`, `03_34hinten_*`, `05_ruhe_*_x3.png`, `06_kopf_*_x3.png`,
`04_k5cneu_koerper_x2.png`, `04_em30_x2.png`, `sp_*_x3.png`, `orig_bild1_figur_x3.png`, `orig_bild2_figur_x3.png`).

## 0. Ergebnis

**Nicht bestanden** (ein Punkt in der Klasse "Abweichung, die der Nutzer wieder ablehnen wuerde"), aber mit grossem Abstand zu K5c heute und zu
Runde 2: Farbfamilie, Glanzfreiheit, Mehrtoenigkeit, Kiefer, Augen und Blau-Klasse sind jetzt "wie Spielfamilie". Was bleibt, ist die **Struktur**:

- **P1 (A): Fleisch und Haut sind Farbflaechen ohne gemalte Binnenzeichnung.** EM30 und G4 zeigen je Feld eine gemalte Form (Adern, Muskelstraenge,
  Knochenplatten, Wundrand mit hellem feuchtem Kern und dunkelrotem Rand). K5c neu zeigt im Atlas nur weiche Farbwolken (Rot, Schwarzrot, Blaugrau),
  im Render eine gleichmaessig dunkelrote Flanke mit geraden Polygonkanten und im Spiel 5080 ein flaches dunkelrotes Viereck. Gemessen: Rotflaeche
  K5c neu L* Median 15,5, Streuung 8,9, a*-Streuung 3,6 - G4 26,0 / 12,6 / 6,9, EM30 23,4 / 13,4 / 6,3 (03 Seite, gleiches Licht). Die Schaerfe
  ist dabei NICHT das Problem (Atlas-Texelschritte p90 7,5 gegen EM30 9,5; siehe 2.1): Es fehlt die gezeichnete Anatomie, nicht die Aufloesung.
  Das ist genau das "von der Struktur" und das "Wunden fleischig" (E10) des Nutzers; nach sechs Ablehnungen auf dieser Achse (E8, E10, E12, E13,
  E15, E22, heute) rechne ich mit einer siebten, wenn die Flaechen so bleiben.

Alles andere ist "wie Spielfamilie" oder "Abweichung klein" (Tabelle 1).

## 1. Befunde

Klassen: **F** = wie Spielfamilie, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

| # | Punkt | Soll (Spielfamilie / Original / Nutzer) | Ist K5c neu Runde 3 (gemessen / gesehen) | Klasse |
|---|---|---|---|---|
| P1 | Binnenzeichnung der Fleisch-/Hautflaechen | EM30-Atlas (`04_em30_x2.png`): Adern 1-2 Texel breit, 20-60 Texel lang, auf hellem Fleisch; Schulterwunde mit hellem Kern und dunklem Rand. G4 (06 rechts): Knochenplatten, Streifen, rosa Fleischhoehlen. Nutzer E10: "Wunden fleischig", E12: "Detail gemalt mit Richtung (Muskelstraenge, Falten, Adern)" | Atlas K5c neu (`04_k5cneu_koerper_x2.png`): 46 Felder aus weichen Farbwolken, keine Linie, keine Ader, kein Wundrand, keine Richtung; Kopffeld eine dunkelrote Wolke mit zwei hellen senkrechten Baendern. Render 05 Seite (`05_ruhe_34vorn_seite_34hinten_x3.png`): Flanke = zusammenhaengende dunkelrote Flaeche mit gerader Oberkante und Polygon-Stufen, innen gleichmaessig. Rotflaeche (a* > 15) 05 Seite: L* 15,6 (sd 8,9), a* 19,4 (sd 3,6), b* 6,6 - G4 03 Seite 26,0 (12,6) / 21,1 (6,9) / 11,9, EM30 23,4 (13,4) / 19,5 (6,3) / 8,6. Spiel 5080 Ruhe Flanke (310-357, 117-157): L* 12,1 sd 5,3, a* sd 4,5 (`sp_spiel_ruhe_r5080_s1_x3.png`: flaches Viereck mit geraden Kanten an Schulter und Huefte). 03 3/4 hinten: harte senkrechte Naht rot/grau an der Huefte | **A** |
| P2 | Farbfamilie / Machart gegen EM30, G5, G4 | G4 03: L* 17-22, C* 9,6-12,6; EM30 24,5-29 / 9,9-12,5; dunkle Grundhaut mit Fleischstufen | 03: L* 15,9-18,2, C* 9,7-10,3; dunkles Rotbraun, Fleischrot an Bauch/Flanke/Schulter, Blaugrau an den Gliedern, Krallen beige-rot. Kein Weiss, kein Magenta, kein Creme. Neben G4 dieselbe Palette (K4 heute: C* 4,0-5,0, grau mit Weissflecken) | F |
| P3 | Glanz / Neutralweiss (Engine rein diffus) | Familie L* > 85: 0,00-0,21 % | 03/05: 0,00 %, 06 3/4 vorn 0,22 % (Kieferband); keine silbrigen Flecken, keine Punkte (`03_34vorn_G4_K5c_K4_x2.png`) | F |
| P4 | Mehrtoenigkeit der Haut (E10 "mehr als eine Farbe") | Familie sigma_ab je Teil 7,4-10,7; Haut Violett/Rotbraun/Blaugrau/Fleisch | A2 7,6; im Render vier Tonfamilien sichtbar: Rotbraun (Ruecken/Kopf), Fleischrot (Bauch/Flanke), Blaugrau (Glieder, 05 Seite 35 % der Figur, C* 5,1, b* -4,1), Beige (Krallen, Nackenfleck). Die Toene liegen aber nebeneinander als Flaechen, nicht ineinander (siehe P1) | F (Farben) |
| P5 | Texeldichte / "hoeher aufgeloest" (E8) | EM30 gleichmaessig grob; Familie HF 0,04-0,26 | A10 0,64-2,33 unveraendert; Atlas-Texelschritte (Tafel 04, Texelraster, nur genutzte Texel): K5c neu Koerper Mittel 2,77, p90 7,5, Anteil > 12: 3,8 % - EM30 3,19 / 9,5 / 6,9 %, K5c heute 3,01 / 7,6 / 4,0 %. K5c neu ist also eher weicher als EM30, nicht feiner; kein Foto-Korn, keine Wiederholung (B6 0,04 %) | F |
| P6 | Kiefer / Maul gegen RE2-G4 | G4: dunkles Maulinneres, rosa Zahnfleisch, beige Zaehne, keine Weissbaender | 06 unten vorn (`06_kopf_untenvorn_x3.png`): Maulinneres dunkelrot/orange, Zahnreihen beige getrennt, Zahnflecken im Band (B8 14,1 %); A8 6,9 / 25,3 / 39,6 %. Kein Creme mehr. Im Spiel 5090 Pose 2 ist das Band als pinkgrauer Streifen unter dem Kopf lesbar (`sp_spiel_pose2_r5090_s11_x3.png`, x 330-520, y 640-720) | F |
| P7 | Kinnspitze hell-rosa (r2 P5) | G4 hellste 5 %: L* 51,8 / p90 57,1, a* 3,8 (beige) | 06 Seite K5c neu hellste 5 %: L* 55,3 / p90 73,5, a* 13,3 (rosa-weiss gesprenkelt am Kinn, `06_kopf_seite_x3.png`). Quelle: Zonenversatz Unterkiefer L* +43,3 auf das Bild-2-Band, das nach `farbe.md` Oberflaeche PLUS Feuerschein ist (Obergrenze). Damit ist Licht in Texel gebacken, wie frueher der Glanz; in Hangar-Licht ist das Kinn die hellste Stelle des Modells. Im Spiel 5090 unauffaellig | K |
| P8 | Schwanzspitze rosa | Bild 1 unten links (185-270, 395-460) enthaelt rot/rosa Pixel (a* 15,3, L* 37, b* -18, n 683) neben hell-neutralen (Boden, L* 78) und hellblauen; Identitaet offen (Nutzerfrage 6: Fuss, Schwanzspitze oder Schatten); `farbe.md` fuehrt A29/A30 als "nicht als Haut gewertet" | 05 hinten: rosa Kegel L* 46,7 / C* 18,9 auf einer Figur mit L* Median 11,0 (0,6 % der Figur). Spiel 5080 Ruhe: einzige Pixel mit L* > 35 und a* > 12 (n 157, x 439-458, y 261-277, L* 39,4, a* 16,5) bei Figur-Median 8,3 - die hellste und bunteste Stelle der Figur. In der Kette 01 Z.2 ist der Anteil richtig (4,28 % gegen Original 4,36 %), die Helligkeit aber 20 L* zu hoch (62,0 gegen 42,2). Rosa ist screenshot-belegt (E11/E14), zu hell und die Zuordnung unentschieden | K |
| P9 | Blaugraue Glieder (Bauer O4) | `farbe.md`: Blau gehoert dem Video, helle Baender neutral hellgrau (140,145,150); G4 Glieder schwarzbraun | 05 Seite: 35 % der Figur b* < -3 bei C* 5,1 (fast neutral); im neutralen Licht liest man kuehles Grau neben rotbraunem Rumpf (zwei Materialien), im Spiel 5080 grau-schwarz, kein Blau (Blau-Klasse 0,0 %). Vertretbar als neutrale Baender; Zweiteilung Rumpf/Glieder ist eine Lesbarkeitsfrage, kein Farbfehler | K |
| P10 | Pfotenkappen / Krallen (r2 P9) | G4 Krallen knochenbeige mit roten Spitzen; Fuesse Haut wie Koerper | 05 3/4 vorn / vorn (`05_ruhe_vorn_x3.png`): beide Vorderpfoten (W) hellgrau-beige mit rotbraunen Krallen, Hinterpfoten blaugrau; wirkt wie helle Socken, kleiner als r2 (kein Teal mehr) | K |
| P11 | Kopf gegen G4-Kopf | G4: Schaedel mit wenigen Flaechen (Stirn, Schnauze, Augenhoehle), Platten, Nasenhoecker | 06 3/4 vorn / Seite: dunkelrote Wolke, zwei dunkle Augenbeulen (E24 dunkel: richtig), keine Platte, kein Nasenhoecker, zwei helle senkrechte Baender am Oberkopf (Bauer O7, im Render schwach). Liest sich als Kopf, aber ohne Zeichnung (Teil von P1) | K |
| P12 | Zonen hell/dunkel/rot am richtigen Ort (Bild 1) | Bild 1: rosa Stelle Z1 oben auf dem Ruecken (300-360, 250-300: a* 9,4, L* p90 57,8), Glieder heller als Ruecken, Z5 helles Dreieck rechter Arm (W) | 01 Z.2 K5c neu: rosa-violette Schulter/Ruecken oben links am richtigen Ort, Glieder dunkelblau, Pfoten hell, Schwanzspitze rosa unten. Z5 als scharfes helles Dreieck nicht erkennbar (K3 dE max 4,9 sagt im Mittel ok). K5 Hell-Klasse Ruecken 11,7 % | F |
| P13 | Bild 2 (5090 Cut 11) | Original: dunkle Masse, Unterkieferband hell mit Zahnflecken, Klauen rot | 02 Z.1/Z.2 und Spiel (`sp_spiel_pose2_r5090_s11_x3.png`): rotbraune Masse im Feuerschein, Klauen rotbraun, Kieferband schwach lesbar; K6-K10 gruen. Das Original ist dort ebenso dunkel | F |
| P14 | Augen | E24 dunkel, Original Lichtpunkt schwach | K10 14,0 / 15,4; nirgends als Punkt sichtbar | F |
| P15 | Blau-Klasse / Saeume (r1 P2, r2 P1) | Familie 0,0 %; EM30 Rot im Umrissband 11,3 % | B3 0,03 / 0,02 / 0,00 %; Rot liegt als Flaeche an Bauch/Flanke, nicht mehr als Netz; rote Polygonkanten im Spiel 5080 nur noch an der Hinterbein-Innenseite (`sp_spiel_ruhe_r5080_s1_x3.png`, x 500-620, y 380-520). B9 17,8 % = Schwanzspitze + Krallen + Bauchkante | F / K (Kante) |
| P16 | Schwanz aus einem Stueck (E9) | keine Stufe | A11 0,1; 05 Seite/hinten keine Gliedgrenze | F |

## 2. Belege

### 2.1 Schaerfe ist nicht die Luecke (Tafel 04, `mess.py`)

Texelraster aus Tafel 04 (2 px je Texel, jeder zweite Pixel), Hintergrund ausmaskiert, |dL*| zum Nachbartexel:

| Atlas | Texel | Mittel | p50 | p90 | Anteil > 6 | Anteil > 12 | mittlerer Laplace-Betrag |
|---|---|---|---|---|---|---|---|
| EM30_HANGAR | 81146 | 3,19 | 0,00 | 9,52 | 17,6 % | 6,9 % | 8,89 |
| K5c neu R3 Koerper | 80478 | 2,77 | 0,00 | 7,51 | 15,6 % | 3,8 % | 8,08 |
| K5c neu R3 Kopf/Kiefer | 27801 | 2,94 | 0,00 | 7,88 | 16,1 % | 5,1 % | 8,52 |
| K5c heute Koerper | 79719 | 3,01 | 0,82 | 7,63 | 16,6 % | 4,0 % | 8,53 |

K5c neu liegt in der Familie (etwas weicher als EM30: p90 -21 %, starke Schritte halb so haeufig). "Hoeher aufgeloest" (E8) trifft nicht mehr zu.
Der Unterschied zu EM30/G4 ist die Form der Schritte: bei EM30 bilden die starken Schritte Linien (Adern, Falten), bei K5c neu Wolkenraender.

### 2.2 Fleisch flach (03/05, `mess2.py`)

Rotklasse a* > 15, gleiches Licht 0,55 + 0,45 N.L: K5c neu 03 Seite L* 15,5 (sd 9,1), a* sd 3,6, Rotanteil 16,4 %; G4 26,0 (12,6), 6,9, 19,2 %;
EM30 23,4 (13,4), 6,3, 3,9 %. Die Menge Rot stimmt mit G4, die Zeichnung im Rot hat 2/3 der Helligkeits- und die Haelfte der Farbstreuung.
Bild 1 selbst zeigt an Z1 einen hellen Kern (L* p90 57,8 bei Median 14,7): das Original-Fleisch ist nicht flach.

### 2.3 Spiel 5080 (Figurfenster = Differenz K5c neu / K4 heute > 6, 3x3 geoeffnet, 11428 px)

| | L* Median | L* p90 | C* Median | a* Median | Rot (a* > 15) | L* > 50 |
|---|---|---|---|---|---|---|
| K4 heute | 8,0 | 20,2 | 2,7 | 1,5 | 0,0 % | 0,4 % |
| **K5c neu R3** | 8,3 | 25,4 | 6,1 | 5,3 | 6,1 % | 0,3 % |

Im Hangar-Licht bleibt die Figur dunkel (wie Bild 1 verlangt); sichtbar werden rotbraune Flanke, graue Glieder und die rosa Schwanzspitze (P8).
Die Texturzeichnung (P1) kommt dort kaum an - sie entscheidet sich in 03/05/06 und im Feuerlicht 5090.

## 3. Fairness der Tafeln

| Pruefung | Ergebnis |
|---|---|
| Gamma | `generator/kf_tafeln.py`: grep gamma / pow( / **0. trifft nur Beschriftungen ("kein Gamma"); jede Kachel traegt "roh". Keine Aufhellung, auch nicht auf K5c |
| 01/02 | Disc-Licht x k (1,06/0,95/0,82 bzw. 1,20/1,12/1,00) fuer K4 und K5c gleich, in der Kopfzeile genannt; Z.2 beide durch denselben Videofilter. Einschraenkung steht in der Kopfzeile: Z.1 heller als das Spielfenster 07 (Figur L* ~15 gegen 8,3) |
| 03/05/06 | gleicher Renderer, gleiches Licht 0,55 + 0,45 N.L, Umkugel 0,46, gleiche Buehne; L*/C*/L*>85/px je Kachel. Figurgroesse K5c 15-17 k px gegen G4 35-39 k px (in der Kopfzeile genannt): K5c wirkt dadurch etwas feiner, als es ist; mit 2.1 gemessen ist der Effekt klein |
| 04 | gleiche Texelskala (2 px je Texel) fuer alle drei Atlanten; Palettenfelder aus denselben k-Mitteln |
| 06 | Zeile 1 Original / Original T / K5c neu Kette / K5c heute Kette, alle 3-fach, derselbe Ausschnitt; Zeilen 2-4 gleiches Licht wie 03 |
| 07 | echtes Fenster, gdigrab, kein AUTOSHOT/SOFTWARE_RENDER, K4 und K5c in derselben Szene/Pose/Kamera; `spiel_lauf.log` 10/10 rc=0 |
| Benennung | "K4 heute" (01/02/03/07) und "K5c heute" (04/06) sind dieselbe Bestandstextur (K5c heute = K4-Textur + K5c-Kopf); nicht irrefuehrend, aber zwei Namen |

Urteil: fair. Die beiden Einschraenkungen aus r1/r2 (01-Helligkeit gegen Spiel, halbe Pixelzahl in 03) bestehen weiter und sind beschriftet.

## 4. Was fuer "bestanden" fehlt (Sicht, keine Zahlenvorgabe)

1. In die Rot- und Hautflaechen eine gemalte Zeichnung legen, wie EM30/G4 sie haben: Wunden mit hellem feuchtem Kern (Bild 1 Z1: L* bis ~58) und
   dunkelrotem Rand, Adern/Muskelstraenge mit Richtung entlang der Glieder, am Kopf wenige Platten/Stirn/Schnauze. Ziel in Zahlen: Rotflaeche
   L*-Streuung und a*-Streuung in der Familie (>= 12 / >= 6, Tabelle 2.2), Schritte > 12 L* wie EM30 (~7 %), und zwar als Linien, nicht als Korn.
2. Flaechengrenzen des Fleischs von den Polygonkanten loesen (Bauchkante 03 3/4 hinten, Schulter-Viereck Spiel 5080).
3. Kinn (P7) und Schwanzspitze (P8) auf die Kettenhelligkeit des Originals bringen (Kinn-Versatz ohne Feuerschein; Spitze -20 L* in der Kette);
   Frage 6 an den Nutzer bleibt offen.
4. Pfotenkappen (P10) und Nackenfleck/Kopfbaender (P11, O7) pruefen.
