# SF V4 Kopf nach Nutzerbeschreibung, Runde 3 - Zahlen-Pruefung (2026-11-07)

Pruefling: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4ot3.EMD
(sha256 a1bc9309e5dd9ce8a948371ec4fef8595bd86310e3ea5e01dc3326552d5f8998, 262004 Byte; EM30_V4.EMD im selben Ordner ist bytegleich).
Vorstand EM30_V4ot2.EMD (415a921f436fe8e1..., 262004 Byte) vorhanden. Zweig r41/sichel_nb2, HEAD b0386e40f, Baum sauber (git status leer).
Eigener Code (nicht committet), alles unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/zp3/:
emdl.py (eigener EMD/MD1/TIM-Leser), diff.py (Texel-Diff ot3/ot2 + T-K1/T-K2 ueber tk12.py), emr3.py (EMR-Skelett), rend.py + kennfix.py
(Renders; nur Bild und Id-Puffer kommen aus dem gemeinsamen Offline-Zeichner b2/v4_gemein.zeichne mit posen_v4.json, dazu eigener
Kenn-Render Texel-Index je Kopfpixel und eigener Flaechen-Id-Render fuer die Seitenbestimmung), messen.py (alle Masse; Ergebnis
messung.json / messung.txt), ctest_pruefer.txt (eigener Testlauf). Die Riegel des Bauers wurden NICHT nachgestartet.

## Ergebnis

**BESTANDEN.** Alle Ziele aus Punkt 3 des Bauer-Auftrags sind mit eigenem Code gehalten; die Zahlen des Bauers sind bestaetigt (Abweichungen
nur in der zweiten Nachkommastelle, Ursache siehe Befund A). Zwei Befunde ohne sichtbare Wirkung (Abschnitt 7): (A) der Kenn-Render des
Bauers (ref2tex_v4.zonen_render / texel_lage) schneidet bei ~10 % der Kopfpixel den Index um 1 ab, dadurch ist seine Zonen-Flaeche
Haar + Kuppe mit 0,52 zu klein gezaehlt - korrekt sind 0,561 der Kopfpixel, 0,011 ueber seiner eigenen Bandgrenze 0,55 (30 Bildpunkte von
2726); (B) 3030 der 6402 geaenderten Texel liegen in den Kopf-Rechtecken, aber ausserhalb jeder UV-Flaeche (von keinem Mesh gelesen,
unsichtbar).

## 1. Texel-Diff EM30_V4ot3 gegen EM30_V4ot2 (eigener Leser, diff.py -> diff.json)

| Mass | Ist | |
|---|---|---|
| Verzeichnis (9 Sektionen, Offsets 8/4724/10244/116300/116304/116308/116708/124396/160560) | gleich | OK |
| Sektionen 0-7 (EDD, EMR, Baenke, MD1 36164 Byte) | bytegleich | OK -> Geometrie, Skelett, Clips, UV aller Meshes unveraendert |
| Sektion 8 TIM: Kopf (flags 9), CLUT-Kopf (0/480, 256 x 6) | gleich; CLUT-Daten bytegleich | OK |
| Bild-Kopf (192 x 256 Halbworte = 384 x 256 Texel 8 bpp) | gleich | OK |
| Rest nach dem TIM (Byte 261968-262004) | gleich | OK |
| geaenderte Texel | 6402, Bbox x 0-111, y 160-223 | |
| davon im Rechteck vorn (u 0-47, v 160-223) | 2989 (Bbox 0,160-47,223) | = Bauer 2989 |
| davon im Rechteck seite (u 56-111, v 160-223) | 3413 (Bbox 56,160-111,223) | = Bauer 3413 |
| ausserhalb beider Rechtecke | 0 | OK |
| in Koerperflaechen (Meshes 0-8, 10-15) / Schwanzflaechen (16-22) | 0 / 0 | OK |
| in den UV-Flaechen von Mesh 9 (eigene Rasterung: 3621 Texel, Bbox 1,163-102,220) | 3372 | |
| in den Rechtecken, aber in keiner Mesh-9-Flaeche | 3030 | Befund B (ungelesen) |
| Mesh-9-Polygone (UV, CLUT 480, TPage 128) ot3 = ot2 | gleich | OK |

## 2. T-K1 / T-K2 gegen Referenz 2 EM30_HANGAR.EMD (cfd1ac42...; tk12.run auf ot3, emr3.py)

| Mass | Soll | Ist (eigener Code) | |
|---|---|---|---|
| Sektionen 0, 1, 3-6 | bytegleich | bytegleich | OK |
| Sektion 2 EMR | Knochen 0-15 gleich | Knochentabelle gleich; relpos nur Knochen 16-22 anders; 912 Keyframes: Kopfbytes 0 Abweichungen, Winkel nur Knochen 16-22 anders | OK |
| T-K1a Meshes 0-8, 10-15 (tv, tn, tf, tu, qv, qn, qf, qu) | bytegleich | 0 abweichende Bloecke | OK |
| T-K1b CLUT | Zeilen 480-485 bytegleich, keine neuen Zeilen | 6 x 256 wie Ref2, 0 Zeilen / 0 Eintraege abweichend | OK: **0 neue CLUT-Eintraege, 0 Ableitungen** |
| T-K1c Koerpertexel | bytegleich | 0 von 34431 (eigene Rasterung) abweichend; alle 22936 Aenderungen gegen Ref2 ausserhalb der Koerperflaechen | OK |
| T-K2 Kopf + Schwanz | 15-Bit-Farbe in der Ref2-CLUT oder belegte Ableitung | 13977 Texel (nur Zeile 480), 160 Farben, 0 ausserhalb der Ref2-CLUT, 0 ausserhalb der Ref2-KOERPER-Farben (strenger) | OK |
| Kopf-Texel (Mesh-9-Flaechen) | | 25 Indizes = 25 Farben, alle in Ref2-Koerperfarben; alle 6656 Rechteck-Texel in den Klassenlisten grau/braun/kuppe/hell des Protokolls (0 fremde) | OK |
| Dreiecke | Kopf <= 118, Schwanz <= 212 | Kopf 109; Schwanz 33+22+44+22+22+22+33 = 198 | OK |

## 3. Zonen am eigenen Render (messen.py; Bild/Id-Puffer b2-Zeichner, Spielkamera 50A0 Cut 12, RDT-Licht; Lab = skimage rgb2lab;
Videoraum = Gauss sigma 1,5 + Eichung a/b aus Messblatt Abschnitt 5, selbst implementiert)

Zonen je Bildpunkt = eigene Klasse des gelesenen Texel-Index (CLUT-Zeile 480 -> Lab: grau Chroma <= 7,5 und L* >= 26; braun a*, b* > 0,
b* >= 0,6 a*, L* 12-40, Chroma <= 17; kuppe a* 12-22, b* >= 0,4 a*, L* 20-36, Chroma > 17). Gegenprobe unabhaengig vom Index: Klasse aus der
Texelfarbe des Renders ohne Licht (grau C <= 7,5 / braun a, b > 0, C <= 17 / rot C > 17). Pose 135: Kopfbox 325-380 x 154-221, 2726 Kopfpixel,
H_port 364; Nase x = Mitte KV3 = 354,1; Kopfmitte 352,5.

| Ziel (Punkt 3 des Auftrags) | Soll | Bauer | eigene Messung | |
|---|---|---|---|---|
| Zonen-Anteile Kopf 135 (Index) | - | grau 0,438 / braun 0,358 / Kuppe 0,159 (Rest 0,045 'sonst') | **grau 0,439 / braun 0,391 / Kuppe 0,170 / sonst 0,000** | Befund A |
| Zonen-Anteile Kopf 135 (Pixelfarbe, indexfrei) | keine vierte Farbe | - | grau 0,441 / braun 0,389 / rot 0,170 / sonst 0,000 (ot2: 0,205 / 0,293 / 0,503; Ref2: 0 / 0,365 / 0,635) | OK |
| (a) graue Zone Chroma P50 | <= 0,5 x Rumpf Id 8 (7,79 -> 3,9) | 3,5 | 3,50 (Texel-P50 34,8/3,4/1,3) | OK |
| (a) graue Zone L* P50 | um Ref2-Kopf 30,8 (+- 5) | 27,9 | 27,9 (Pose 85: 30,6; Albedo P50 34,8, Anteil Albedo < 25: 0,000) | OK |
| (a) Lage: grau links der Nase | >= 0,80 der grauen Pixel; >= 0,70 der Kopfpixel links der Nase grau | 0,83 / 0,73 | 0,911 / 0,795; 0,877 der grauen Pixel links der Kopfmitte | OK |
| (b) Haarflaeche L* P50 | 0,62-0,90 x Kopf-Median 27,4 (17,0-24,7; "um 22-24") | 18,4 (0,67) | 18,3 = 0,666 x Median | OK (dunkler als 22-24, Befund C) |
| (b) Haar a* / b* | > 0 / > 0 | 3,3 / 7,5 | 3,2 / 7,7 (Texel-P50 31,8/4,1/12,2) | OK |
| (b) Flaeche Haar + Kuppe | 0,40-0,55 der Kopfpixel | 0,52 | **0,561** (Haar 0,391 + Kuppe 0,170) | Befund A: +0,011 ueber Band = 30 px |
| (b) Lage Haar rechts | rechte Bildhaelfte | - | 0,816 der Haar+Kuppe-Pixel rechts der Nase, 0,878 rechts der Kopfmitte; eine Flaeche: groesste Komponente 0,981, Bbox 335-380 x 154-221, Fuellgrad 0,49 | OK |
| (b) Rand grau <-> Haar | Gradient (Sobel/8, L*) 3-8; Grenze je Zeile Std zur Geraden > 4 px | 3,2 (704 px); 6,9 px (61 Zeilen) | 3,81 (361 Randpixel, 1-px-Rand); L*-Sprung 3-px-Baender 8,0; Std 5,35 px (56 Zeilen, x 335-360); Oberkante je Spalte Std 7,15 px | OK |
| (c) Kuppe L* P50 | <= Median + 6 (33,4), >= Haar | 31,8 | 31,8 (Pose 85: 34,7 <= 37,8) | OK |
| (c) Kuppe Farbton | rotbraun a* >= 8, b* > 0 | 17,4 / 7,3 | 17,4 / 7,3; Anteil 0,170; 0,987 im oberen Kopfdrittel, 0,717 rechts der Mitte | OK |
| (c) Kuppe nahtlos zum Haar | - | - | Grenze Kuppe/Haar: L* 33,2 / 28,9 (3-px-Baender, Sprung 4,3), Randgradient 1,9; Kuppe/grau Sprung -1,0; Videoraum KV1 - KV2 = 9,3 (Vorlage 22,6 - 12,9 = 9,7) | OK |
| (d) Auge K1 | 1 Komponente, Kontrast >= 20 | 1; 20,2 | 1; 20,2 | OK |
| (d) Auge K2 | L* >= KV5 + 20, a* <= 15, b* >= 0 | 54,5/4,9/1,9 (KV5 25,3) | 54,5/4,9/1,9; KV5 25,3/1,3/1,1 | OK |
| (d) Auge K3 / K4 | 0,016-0,036 x 0,023-0,043 H; 0,035-0,065 H links, 0,07-0,12 H unter OK | 0,030 x 0,033; 0,054 / 0,095 | 0,030 x 0,033 H (102 px); 0,054 H links; 0,095 H unter OK; Schwerpunkt 332,8 / 188,5 | OK |
| (d) Auge in der grauen Zone | ja | Zone 1 | Fleckmaske 100 % grau; lokal (366 / -300 / -85) = (W)-rechts | OK |
| Nase K7 (KV3, Videoraum) | \|KV3 - KV5\| <= 4; KV3 >= KV3r + 1; KV3 zu 0,3-0,7 grau | 23,2; KV5 25,3; KV3r 17,7; 0,37 | 23,2/1,1/5,1; -2,1; +5,5; grau 0,322 | OK |
| K5 Haarfeld KV2 (Videoraum) | 0,62-0,90 x Kopf-Median 32,6 | 28,0 | 28,0/4,6/7,2 = 0,859 x Median | OK (nahe Oberkante) |
| K6 Spiegelstelle | <= Median - 3 (29,6); max <= 39,5 | 23,1 / 31,6 | 23,1 / 31,6 (Box 367-377 x 183-194, 0,91 Haarzone) | OK |
| K8 Kinn KV4 | Albedo P50 >= 29,2; b* >= 0 | 31,9; 22,0/0,7/2,8 | 31,9; 22,0/0,7/2,8 | OK |
| K9 Kuppe KV1 (Videoraum) | a* >= 8, b* > 0, L* <= Median + 8 (40,6), >= KV2 | 37,3/14,4/6,7 | 37,3/14,4/6,7 | OK |
| K10 Kopfbox | 0,14-0,18 x 0,15-0,20 H | 0,154 x 0,187 | 0,154 x 0,187 H | OK |
| S1 Auge seitlich (Pose 85) | 1 Komp.; 0,028-0,052 x 0,040-0,070 H; a* <= 15, b* >= 0 | 1; 0,045 x 0,049; 53,3/4,6/1,9 | 1 Komp., Kontrast 20,4; 0,045 x 0,049 H; 53,3/4,6/1,9; 100 % grau | OK |
| S2 Augenlage seitlich | 0,03-0,07 H hinter Vorderkante; 0,76-0,82 H ueber Sohle | 0,051 / 0,817 | 0,051 / 0,817 (Kopfbox 438-479 x 185-236, H 264) | OK |
| KS2 Haarkante seitlich | Videoraum L* <= Median 37,0 - 5; roh <= 31,8 - 4; Abstand zum Fleck <= 0,01 H | 23,1; 14,9; 0,000 | 23,1/0,7/1,3; roh 14,9; 0,000 H; Fenster 472-477 x 212-226 zu 100 % graue Zone, Albedo P50 27,8, Licht (weisser Pruefkoerper) L* 40,5 | OK (Befund D) |
| KS1 Kuppe seitlich | a* >= 8, b* > 0, L* <= 45,0 | 34,5/10,1/4,0 | 34,5/10,1/4,0 | OK |
| (e) ganzer Kopf 135 | L* P50 >= 23; Kopf/Rumpf >= 0,9 | 27,4; 0,99 | 27,4; 0,988 (Ref2 30,8; 1,284); L* < 20: 0,323; b* < 0: 0,000 | OK |
| (e) ganzer Kopf 85 | dito | 31,8; 0,99 | 31,8; 0,991 (Ref2 30,8; 1,049) | OK |
| (f) Rundum Pose 85 Gier +120 / +180, neutral | groesster Fleck L* < 15 <= 100 px | 2 / 0 | +120: 2 px (3 dunkle von 1335); +180: 0 px (0 von 784); Kopf-P50 31,8 (ot2: 494 / 360 px) | OK |

Gegenprobe ot2 mit demselben Code (messung.json 'ot2'): Kopf 135 L* P50 23,9, Kopf/Rumpf 0,865, Haar L* 5,7 (0,24 x Median), Haar+Kuppe
0,30, grau 0,087 (Pixelfarbe: rot 0,503), K5 12,9, K9 38,0 > 35,6, Rundum 494 / 360 px - die abgelehnte Fassung faellt an denselben Stellen
wie beim Bauer.

## 4. Seitenbestimmung (eigener Flaechen-Id-Render und reine Geometrie, rend.py / messen.py)

- Geometrie ohne Render: Nasenspitze des Kopf-Meshes (482 / -126 / -29 lokal, KOPF_V4) projiziert in Pose 135 nach Bild (343,4 / 213,3);
  derselbe Punkt + 200 E. lokal z nach (359,0 / 208,8) = **Bild-rechts**; + 200 E. lokal x (vorn) hat Sichttiefe 4609 statt 4730 = naeher an
  der Kamera. Der Kopf blickt also zur Kamera, und lokal +z ist die (W)-linke Seite des Wesens -> **Bild-rechts = (W)-links**.
- Flaechen-Render Mesh 9 (Maler-Reihenfolge, Dreieck -> Ecken ueber das UV-Tripel, Deckung 2722 von 2726 Kopfpixeln): lokale z der
  Flaechenschwerpunkte, Pose 135: links der Kopfmitte Median 47,3 E., rechts 218,0 E. (Bauer per Texel 14,3 / 215,4; gleiches Vorzeichen,
  Schwerpunkt-Mass groeber). Graue Zone z-Median 48,7, Haarzone 218,0; Augenfleck lokal (366 / -300 / -85) auf der (W)-rechten Seite
  (Bauer 336 / -331 / -43).
- Pose 85 zeigt die (W)-rechte Kopfseite (lokal z Median -173 links / -97 rechts der Kopfmitte); Kopfpixel lesen zu 0,648 das Seitenbild,
  zu 0,352 das Vorderbild.
- KS2 (Pose 85, 90 Fensterpixel): alle 90 lesen Texel des **Vorderbilds** (u <= 47; 84 verschiedene Texel, 0 Seitenbild) - eigener
  korrigierter u/v-Kenn-Render. Dieselben Texel erscheinen in Pose 135 bei Bbox 333-351 x 183-207, Schwerpunkt (340,8 / 194,4), also
  zwischen Augenfleck (332,8) und Nase (354,1), und sind dort zu 0,96 graue Zone (Flaechen-Render: 650 px, Bbox 325-354 x 173-211).
  Die Aussage des Bauers (77 Texel, Bbox 333-350 x 183-207, Schwerpunkt 340,5 / 194,4) ist bestaetigt.

## 5. Spiel-Asset, Pin, Commits, Tests

- re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD sha256 cfd1ac42729e56365847db42b352337d7f0c06fdcc1ef3413d0aee32d4a97a95 = release/em30_hangar.sha256;
  letzter Commit auf beiden Pfaden 84e8a4d97 (unveraendert). `git diff --stat 1a51ec7bc..HEAD -- Version_80% pri synchro re15_port/shared_assets release` leer.
- Commits 3094f4bc1, d97ac025a, e276ead92, 5dcabea8f, b0386e40f auf r41/sichel_nb2 vorhanden (22 Dateien: Werkzeuge v4/, Messblatt, Dossier,
  Leisten 01-04/08/09, Belege ot3). Leisten 05/07 der Runde 2 liegen in SF_belege/v4/ot2/.
- Eigener Testlauf `ctest --test-dir C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/build -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form"`:
  **"100% tests passed, 0 tests failed out of 4"**, Total Test time (real) = 64.48 sec, EXIT=0 (zp3/ctest_pruefer.txt).
- Spiellauf des Bauers (SF_belege/v4/ot3/lauf_V4ot3.txt): EXIT=0, "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)", 23 Meshes / 23 Knochen / 21 Clips.

## 6. Abweichungen zum Bauer (alle klein, Ursache A)

| Mass | Bauer | eigen |
|---|---|---|
| Zonen 135 grau / braun / Kuppe | 0,438 / 0,358 / 0,159 | 0,439 / 0,391 / 0,170 |
| Flaeche Haar + Kuppe | 0,52 | 0,561 |
| graue Pixel links der Nase / Kopf links grau | 0,83 / 0,73 | 0,911 / 0,795 |
| Haar L* | 18,4 | 18,3 |
| Randgradient / Kante Std | 3,2 (704 px) / 6,9 px (61 Zeilen) | 3,81 (361 px, 1-px-Rand) / 5,35 px (56 Zeilen) |
| KV3 grau | 0,37 | 0,322 |
| Seitenbestimmung z links / rechts | 14,3 / 215,4 (Texel) | 47,3 / 218,0 (Flaechenschwerpunkte) |

## 7. Befunde (fuer Sicht-Pruefung und Orchestrator)

- **A (Werkzeug, nicht EMD): Abschneidefehler im Kenn-Render.** b2.rastern_ids multipliziert die Texelfarbe mit (w0 + w1 + w2) x 255 / 255;
  die baryzentrische Summe liegt bei einem Teil der Bildpunkte um ~4e-8 unter 1, der uint8-Cast schneidet dann jeden Kanalwert um 1 ab.
  Gemessen mit einer Kenn-Konstante 200 im G-Kanal: 274 von 2726 Kopfpixeln (Pose 135) und 108 von 1151 (Pose 85) lesen 199. Betroffen
  sind alle Kenn-Renders ohne Korrektur, also ref2tex_v4.zonen_render (Zone 1 -> 0 'sonst', 2 -> 1, 3 -> 2) und texel_lage (Bild/u/v um 1
  verschoben) und damit riegel_ref2tex.tk9/zonen_bild sowie die Zonengeometrie des Generators (augen_gewicht, kuppe_gewicht, fenster_texel,
  geometrie_r3 - dort Mediane, Wirkung <= 1 Texel). Folge im Riegel: Haar + Kuppe 0,52 statt 0,561 (ueber der eigenen Bandgrenze 0,55 um
  0,011 = 30 Bildpunkte), KV3 grau 0,37 statt 0,32, 'sonst' 4,5 %. Am Bild unsichtbar (die EMD-Texel selbst sind korrekt, s. Abschnitt 2:
  alle Rechteck-Texel in den Klassenlisten). Empfehlung: Kenn-Renders mit Pruefkonstante oder Index x 1 ohne Lichtfaktor; T-K9b-Band
  neu bewerten (0,561 = "rechte Bildhaelfte" im Wortsinn, Nutzer-Lesung erfuellt).
- **B: 3030 geaenderte Texel ohne Flaeche.** Der Generator fuellt die beiden Rechtecke 48 x 64 / 56 x 64 ganz; nur 3372 der 6402 geaenderten
  Texel liegen in einer UV-Flaeche von Mesh 9 (3621 Flaechen-Texel, 249 davon zufaellig gleich ot2). Die uebrigen 3030 liest kein Mesh
  (Koerper 0, Schwanz 0, Kopf 0) - unsichtbar, kein Defekt; nur die T-K8-Formulierung "nur in den Kopf-UV-Flaechen" meint die Rechtecke.
- **C: Haar im Render 18,3 L* statt "um 22-24".** Die Haar-Texel sind 31,8 L* (0,91 x grau 34,8); die Haarseite liegt im Schatten des
  RDT-Lichts: weisser Pruefkoerper L* P50 grau 95,8 / Haar 65,1 / Kuppe 97,6. 0,666 x Kopf-Median liegt im Band 0,62-0,90; im Videoraum
  ist KV2 mit 28,0 = 0,859 x Median eher hell. Beide Seiten des Richtwerts 0,75 - OK, Sichtpruefung entscheidet am Bild.
- **D: KS2 ist graue Zone, kein Haar.** Die seitliche Haarkante (Beschreibung2) liegt auf Vorderbild-Texeln zwischen Auge und Nase
  (Abschnitt 4); braun dort widerspraeche Lesung (1). Der dunkle Streifen entsteht aus Augenhof (Albedo 27,8 gegen 37,9 der grauen Zone)
  und streifendem Licht (40,5): Videoraum 23,1/0,7/1,3 unbunt - die Vorlage misst dort ebenfalls unbunt dunkel (19,1/1,2/1,1). Keine
  sichtbare Abweichung von der Vorlage; die Lesung "Grenze seitlich an der gemessenen Haarkante" ist geometrisch nur so erfuellbar.
- **E (INFO): Nasenebene.** Die Zonengrenze geht durch die Mitte des Messblatt-Fensters KV3 (Bild x 354, lokal z 185), nicht durch die
  Nasenspitze des Kopf-Meshes (lokal -29, Bild x 343,4 = 11 px weiter links, 0,33 der Kopfbox-Breite). Das folgt dem Auftrag ("Ort wie
  gemessen", K7); zwischen Mesh-Nase und Zonengrenze liegt noch graue Haut (KV3 zu 0,32 grau = Grenze im Fenster).
- **F (INFO): K5 nahe Oberkante** (0,859 bei Band bis 0,90) und **Flaeche nahe Oberkante** (A). Beides Bauer-Baender, keine Nutzer-Zahlen.
