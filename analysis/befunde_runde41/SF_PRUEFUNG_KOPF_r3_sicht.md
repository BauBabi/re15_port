# SF V4 Kopf nach Nutzerbeschreibung Runde 3 - Sicht-Pruefung (Pruefer, 2026-11-07 nachts)

Gegenstand: Variante C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4.EMD
(= EM30_V4ot3.EMD, sha256 a1bc9309e5dd9ce8..., 262004 Byte, gemessen). Vorstand EM30_V4ot2.EMD (415a921f, Runde 2, vom Nutzer abgelehnt).
Referenz 2 = C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD (sha256 cfd1ac42729e5636...,
gemessen, unveraendert; Pin release/em30_hangar.sha256 gleich). Zweig r41/sichel_nb2, HEAD b0386e40f.

Der Nutzer-Satz, gegen den gelesen wird (2026-11-07 abends): "da ist links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite
sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?"

Gelesen: Leisten 01, 02, 03, 04, 08, 09 in C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/,
Runde-2-Leiste SF_belege/v4/ot2/03_kopf_nah_vorn_x4.jpg, C:/workspace/git/reAi_v2/Version_80%/Beschreibung1.png, Beschreibung2.png,
Beschreibung1_Vorlage.png, Beschreibung2_Vorlage.png, Spiellauf-Kopfbilder
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ot3/spiel_koepfe.png (RE15_FRAMEDUMP,
Ref2 / ot2 / ot3, h85 / h135), Dossier-Abschnitt "V4 Kopf nach Nutzerbeschreibung (Bauer, Runde 3)" in SF_sichel.md (Zeilen 1894-2040),
Riegel SF_belege/v4/ot3/riegel_ref2tex_V4ot3.txt (1 rot von 57 = T5, 8 INFO), ctest_v4_ot3.txt (4/4, EXIT=0).

Eigene Gegenprobe (unabhaengig von riegel_ref2tex.py): Renderer v4_gemein.zeichne (b2, Kamera 50A0/12, RDT-Licht und licht=False = Texelfarbe),
Posen posen_v4 "135" und "85", Ref2 / ot2 / ot3 je mit Id-Puffer, Zonenkarte ueber ref2tex_v4.zonen_render aus farben_ref2tex_v4.npz.
Skripte und Bilder: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sicht_ot3/
(render3.py, messen3.py -> messung3.json, kacheln3.py -> koepfe_x6_vorlage_ref2_neu.png, farbton_x6.png, texelfarben_zonen.json,
fremdton.py -> fremdton.json, spiel_V4ot3_h135_gelbgruen.png). Lab = CIE-Lab D65 (skimage), Farbton = atan2(b*, a*) in Grad
(0-40 = rot/rosa, 45-120 = gelb bis gruen), Chroma = sqrt(a*^2 + b*^2). Die Ref2-Zahlen der Runde 2 sind reproduziert (Ref2 Kopf Pose 135
L* P50 30,8, Kopf/Rumpf 1,28).

## Ergebnis: NICHT bestanden (ein Punkt der dritten Klasse: der Farbton der Haare)

Die Anordnung stimmt jetzt mit dem Nutzer-Satz ueberein: links graue Haut mit einem hellen, unbunten Augenfleck, die bis zur Nase reicht;
rechts eine Haarflaeche mit weichem Rand, nicht schwarz, kein Block, kein Loch; die Kuppe ist dunkel rotbraun statt lachsrot; der Koerper
und der Schwanz sind bytegleich; seitlich wie Beschreibung2. Uebrig ist EIN Fehler, den der Nutzer nach meiner Einschaetzung wieder ablehnt:
die Haare sind nicht braun im Sinn der Vorlage (dunkles Rotbraun, Farbton 14 Grad), sondern **oliv-khaki** (Texel 88/72/56, Farbton 71 Grad;
im Licht 66 Grad). Dieser gelbgruene Farbton kommt weder im Vorlage-Kopf noch im Ref2-Kopf vor (je 0 %) und erscheint am ganzen Wesen sonst
nur als feine Sprenkel halber Saettigung. Als Flaeche neben der roten Kuppe (23 Grad) und der grauen Haut (20 Grad) ist er genau die
"daemliche Farbmischung", die der Nutzer rügt - eine neue, fremde Farbe. Alles andere ist "wie Vorlage/Original" oder "klein".

## (1) Links: graeuliche Haut mit Auge, bis zur Nase - wie Vorlage/Original

- Bild (eigene Kachel koepfe_x6_vorlage_ref2_neu.png Zeile 1 Spalte 3; Leiste 03/09 Spalte 4): die linke Kopfhaelfte ist grau-mauve,
  darin ein heller, unregelmaessiger, blasser Fleck; unter dem Fleck zur Mitte hin dunkleres Grau (Nase). Keine rosa Haut mehr.
- Zahlen Pose 135 (RDT-Licht, roh; messung3.json B_135_licht_*): linke Haelfte L* P50 28,9 / a* 3,3 / b* 1,2, Chroma 3,5 (Ref2 dort
  30,8 / 19,5 / 8,8, Chroma 21,5). Zonen-Anteil links: grau 0,77, Kuppe 0,10, Haar 0,05, ohne Zone 0,09 (die Pixel ohne Zone sind grau,
  30,2 / 3,3 / 1,2 - kein rosa Rest). Augenfleck: groesste Komponente L* > 45 = 127 px, Bbox 14 x 16 px (x 326-339, y 181-196),
  54,2 / 6,1 / 2,2, Hof 27,9 / 3,4 / 1,3; unregelmaessig (Fuellgrad 127 / 224 = 0,57), kein Ring, keine Pupille - wie die Vorlage
  (46,9 / 7,2 / -1,1, 8 x 10 px, Messblatt Abschnitt 2). Nase: Fenster KV3 im Videoraum 23,2 zwischen Wange 25,3 und Haar rechts 17,7
  (Riegel K7; eigene Kachel: dunkelgrauer Bereich mittig unter dem Auge).
- Grau-Zone gegen Vorlage: Vorlage links bis Nase 23,0 / 7,6 / 1,6 (Chroma 7,9, "grau-mauve"); V4 ist unbunter (Chroma 3,5), was der
  Nutzer-Wortlaut "graeulich" deckt. Texel der grauen Zone: 17 CLUT-Eintraege, L* Std 9,8 (Texel) / 12,6 (Licht) - strukturiert wie die
  Ref2-Kopfhaut (12,8).
- Widerlegung versucht (Pixel ohne Zone = rosa Rest? -> nein, grau; Auge rund aufgemalt? -> nein, 0,57 Fuellgrad, 5 Komponenten L* > 45
  mit einer grossen). Bewertung: **wie Vorlage/Original**.

## (2) Rechts: Haare als braeunliche Flaeche

Teil 1, Form und Dunkelheit: **wie Vorlage/Original**
- Zonen-Anteil rechte Haelfte: Haar 0,62, Kuppe 0,21, grau 0,17, ohne 0,01; in 4 Spaltenbaendern von der Mitte nach aussen Haar
  0,42 / 0,56 / 0,76 / 0,88 - eine Flaeche ueber die ganze rechte Seite, nach oben in die Kuppe uebergehend, kein Fenster, kein Block.
- Haar im Licht L* P50 18,4 (0,67 x Kopf-Median 27,4; Videoraum 25,0 bei Grau 34,5 = 0,72); P10 7,7 / P90 30,5, Std 8,5; 14 Texel-Eintraege
  mit hellen Flecken (104/80/72 17 %) - Struktur sichtbar, nicht schwarz (Runde 2: 6,2 = 0,26 x). Rundum (Leiste 08 +120 / +180 Grad):
  kein Loch mehr; Riegel T-K9f 2 / 0 px.
- Rand Haar/Grau: Gradient 3,56 bei Kopfmittel 3,32 (Ref2-Kopf 5,1), Randpixel 630, Kante strukturfolgend (Riegel: Std 6,9 px ueber
  61 Zeilen; Runde 2 gerade Kanten). dL Haar gegen Grau am Rand -4,3.
- Lesung "um L* 22-24" wird mit 18,4 unterschritten (Dossier Offen 2: Schattenseite des RDT-Lichts); im Videoraum 25,0. Abweichung
  klein, am Bild nicht als "schwarz" lesbar.

Teil 2, Farbton: **Abweichung, die der Nutzer wieder ablehnen wuerde**
- Bild: eigene Kachel koepfe_x6_vorlage_ref2_neu.png (Zeile 1 und 2, Spalte 3) und Leiste 03 Spalte 4: die Haarflaeche ist oliv-khaki
  (Lehm, Tarnfarbe), nicht rotbraun. farbton_x6.png (Farbton als Farbe, Saettigung = Chroma): Vorlage 1 und Referenz 2 sind am ganzen Kopf
  rot-orange (Farbton 9-34 Grad bzw. 21-25 Grad); V4 neu zeigt rechts eine **gelbgruene Insel**. Spielbild (spiel_koepfe.png V4ot3 h135):
  derselbe olivbraune Fleck rechts neben dem Augenfleck.
- Zahlen (fremdton.json): Haar-Zone Pose 135 Texelfarbe: Farbton P10/50/90 = 44 / 71 / 71 Grad, Anteil 45-120 Grad = 0,75; im RDT-Licht
  41 / 70 / 75, Anteil 0,77. Median 31,8 / 4,1 / 12,2 (b* = 3 x a*; haeufigster Eintrag 88/72/56 mit 46 %). Vorlage 1 Haarfeld (Fenster
  KV2, entblendet): Farbton P50 14 Grad, Anteil 45-120 Grad = 0,000, Lab 13,1 / 6,8 / 1,6 (a* = 4 x b*). Ref2-Kopf: Anteil 45-120 Grad =
  0,000 (Posen 135 und 85, Licht und Texel). Vorlage-Kopf gesamt 0,07 (Rauschen am Rand), Vorlage-2-Kopf 0,08.
- Am uebrigen Wesen gibt es den Farbton nur als Sprenkel: Ref2 Pose 135 Texelfarbe Id 8 (Rumpf) 15 % der bunten Pixel, Id 7 28 %, Id 15
  100 % (177 px), jeweils Lab um 28-38 / 1,9 / 6,5 = Chroma 6,8 - die blassen Tupfen der Ref2-Haut, halb so gesaettigt wie der Haar-Eintrag
  (12,8) und nie als Feld. Eine zusammenhaengende Flaeche dieses Tons (975 px = 36 % des Kopfes) hat die Referenz nirgends.
- Grenze Haar/Kuppe: dE00 3,9, aber Farbton-Sprung 43 Grad (Licht) / 38 Grad (Texel); Grenze Haar/Grau Farbton-Sprung 43 / 38 Grad.
  Die Lesung verlangt "Farbrichtung rotbraun, nahtlos zu den braunen Haaren" - rot (23 Grad) neben gelbgruen (71 Grad) ist nicht nahtlos.
- Ursache, aus dem Dossier belegt (Abschnitt Farbquelle): Klasse "braun" = a* > 0, b* > 0, **b* >= 0,6 a***, L* 12-40, Chroma <= 17. Die
  Bedingung b* >= 0,6 a* schliesst die rotbraunen Eintraege (a* > b*) aus und laesst nur gelbbraune zu; die Eintragswahl mit a*/b*-Gewicht
  0,15 zieht dann zum Eintrag 88/72/56. Die rotbraunen Ref2-Eintraege, die die Lesung meint (z. B. 96/64/56 = 30,5 / 12,8 / 10,5, 39 Grad,
  in der Klasse nur 5 %; 80/40/40 = 21,5 / 18,6 / 8,4 in "kuppe"), und die nach Lesung (4) erlaubten abgedunkelten Hautfarben (Ref2-Haut
  Farbton 25 Grad, L* gesenkt) wurden nicht genommen ("keine Ableitungen angelegt").
- Riegel, die es haetten fangen muessen: T-K9b prueft a* > 0 und b* > 0 - das erfuellt auch Oliv. Es gibt kein Mass fuer den Farbton
  gegen Vorlage oder Ref2-Haut (z. B. b* <= a* oder Farbton <= 40 Grad), obwohl die Vorlage a* = 4 x b* zeigt.
- Widerlegung versucht: Ist es nur das RDT-Licht? Nein, Texelfarbe 71 Grad. Ist Oliv in der Referenz-Haut heimisch? Nur als Tupfen halber
  Saettigung (s. o.). Ist es bei Spielgroesse unsichtbar? Nein, im FRAMEDUMP-Kopf (spiel_koepfe.png, spiel_V4ot3_h135_gelbgruen.png) hebt
  sich der olive Fleck neben dem hellen Augenfleck ab; der Nutzer hat in Runde 2 am gleichen Bild den Kopf beurteilt.

## (3) Kuppe: dunkel rotbraun statt lachsrot - Abweichung klein

- Zahlen: Kuppe Pose 135 L* P50 31,8 / a* 17,4 / b* 7,3 (Kopf-Median 27,4 + 4,4; Lesung hoechstens + 6), Farbton 23 Grad; Pose 85 35,1 /
  17,1 / 7,1. Runde 2: 38,0 / 22,5 / 7,7 (abgelehnt). Vorlage oben rot 21,4 / 14,6 / 5,3 (Chroma 15,2; Messblatt K9 22,6 / 15,4 / 5,2).
  Nicht leuchtend, nicht lachsrot: erfuellt.
- Aber: die Kuppe ist eine **flache Fuellung**: 4 Eintraege, 104/64/64 (49 %) + 112/72/72 (45 %) = 94 %; L* P10 = P50 = 31,8, P90 35,1,
  Std 2,5 (Texel 2,3) - gegen graue Zone 12,6, Ref2-Kopf 12,8, Vorlage-Rotzone 9,7. Form = Ellipse (Generator: Halbachsen x 1,3), Grenze zur
  grauen Zone dE00 11,1 (a* 17 gegen 4,5), Randgradient 2,2 (weich, weil L* gleich). Am Bild (Kachel x6, Spiellauf h135) ein glattes
  dunkelrotes Kaeppchen; bei Spielgroesse eine rote Kuppe wie im Video. Das Verhaeltnis Chroma Kuppe/Grau ist 5,4 (18,9 / 3,5), in der
  Vorlage 1,9 (15,2 / 7,8) - die Kuppe faellt relativ staerker auf als im Video, weil die graue Zone unbunter ist als die Vorlage.
- Bewertung: **Abweichung klein** - Helligkeit und Farbrichtung stimmen, die Flaechenfuellung widerspricht der "Machart der Original-Textur
  (Struktur sichtbar, keine Flaechenfuellung)", ist aber bei Spielgroesse nicht der Punkt, an dem der Nutzer anstoesst. Mit rotbraunen Haaren
  (Punkt 2) wuerde die Kuppe zugleich "nahtlos".

## (4) EINE Sache statt Mischung aus vier Farben - Abweichung, die der Nutzer wieder ablehnen wuerde (Folge von 2)

- k-means (4 Cluster, Lab, Kopfpixel Pose 135, Licht): V4 neu 0,37 grau 29,4 / 3,8 / 3,9 (45 Grad) | 0,36 dunkel 13,4 / 2,2 / 4,8 (65 Grad,
  Schatten + Haar) | 0,20 rot 32,7 / 16,4 / 7,6 (25 Grad) | 0,07 hell 50,8 / 5,3 / 1,9. Vorlage 1: 0,34 16,7 / 6,8 / 1,7 (14 Grad) | 0,28
  9,8 / 7,4 / 2,2 (17 Grad) | 0,28 26,9 / 11,2 / 2,6 (13 Grad) | 0,10 45,0 / 5,5 / 3,6 (34 Grad). Ref2: alle vier Cluster 22-25 Grad.
  Die Vorlage ist EIN Farbton (13-34 Grad) in vier Helligkeiten; V4 neu hat drei Farbtoene (20 / 23 / 65-71 Grad). Das ist, was der Nutzer
  "Farben mischen" nennt - jetzt nicht mehr rosa/weiss/schwarz/lachs, sondern grau/weiss/oliv/dunkelrot.
- Chroma-Anteile Kopf: unbunt (< 5) 0,41, bunt (> 12) 0,26; Vorlage 0,14 / 0,15. Der Kontrast zwischen grauer Haut und roter Kuppe ist
  damit groesser als im Video, bleibt aber in der Farbrichtung der Vorlage. Entscheidend ist der dritte Farbton (Oliv).
- Bewertung: **dritte Klasse**, allein wegen des Haar-Farbtons; mit rotbraunen Haaren (Farbton <= 40 Grad, L* wie jetzt) waere der Kopf
  ein Farbton in drei Helligkeiten plus hellem Auge - wie die Vorlage.

## (5) Machart gegen die Original-Birkin des Koerpers - Abweichung klein

- Graue Zone 17 Eintraege, Std 12,6; Haar 14 Eintraege, Std 8,5 mit hellen Tupfen; Kuppe 4 Eintraege, Std 2,5 (flach, s. (3)). Kopf-Gradient
  3,32 gegen Ref2-Kopf 5,1 (glatter), Vorlage-Video 2,36 (unschaerfer). Kein Comic-Auge, kein Block, keine Sprenkel fremder Farbe ausser (2).
- Ganzer Kopf Pose 135: L* P50 27,4, Kopf/Rumpf 0,99 (Ref2 30,8 / 1,28; Runde 2 23,9 / 0,87). Der Kopf ist etwas dunkler als der Rumpf; im
  Video ist er ebenfalls dunkler als der Rumpf. In Ordnung.

## (6) Seitlich wie Beschreibung2 - wie Vorlage/Original

- Bild (Kachel x6 Zeile 3 Spalte 3; Leiste 04 / 02 Spalte 4): grauer Kopf, heller Augenfleck vorn, dunkle Vorderkante davor, dunkelrote
  Oberseite hinten-oben, unten im Schatten. Vorlage 2: Kopf 24,2 / 5,4 / 1,8 (Chroma 6,2), rote Zone hinten-oben 28,7 / 17,8 / 4,9, Auge
  48,4 / 4,0 / 0,5, Haarkante 12,6 / 0,6 / 1,4.
- Zahlen Pose 85: Kopf 31,8 / 3,3 / 1,2 (Chroma 3,5); grau 0,56 links / 0,83 rechts der Kopfmitte, Kuppe 0,35 links (hinten-oben), Haar-Zone
  nur 15 px (aus dieser Sicht nicht sichtbar - richtig, die Vorlage zeigt seitlich die graue (W)-rechte Flanke). Augenfleck 1 Komponente,
  110 px (x 458-472, y 209-224), 55,0 / 6,1 / 2,2; Riegel S1 0,045 x 0,049 H, S2 0,051 / 0,817 H, S3 KS2 23,1 Videoraum, Abstand 0,000 H.
  Kuppe 35,1 / 17,1 / 7,1. Das Oliv der Vorderansicht ist seitlich nicht zu sehen (Anteil 45-120 Grad im Kopf 0,000 Licht / 0,018 Texel).
- Bewertung: **wie Vorlage/Original**.

## (7) Ausser dem Kopf alles unveraendert - wie Original

- Koerper gegen Ref2 (Pixel gleicher Koerper-Id 0-8 / 10-15, beide Renders): Pose 135 38790 px, 0 abweichend; Pose 85 16175 px, 0 abweichend.
- Schwanz (Ids 16-22) gegen Runde 2: Pose 85 4698 px, 0 abweichend. Kopf gegen Runde 2: 2489 von 2726 px geaendert (gewollt).
- Byte-Diff EM30_V4ot2 -> EM30_V4ot3: 6402 Byte in 190 Laeufen, alle in 0x36f50-0x3ce3f = TIM-Pixeldaten (TIM-Sektion ab 0x27330 laut
  Verzeichnis; CLUT am TIM-Anfang unberuehrt, Sektionen 0-7 bytegleich). Deckt sich mit Riegel T-K8 (vorn 2989, seite 3413 Texel, ausserhalb 0).
- Spiel-Asset EM30_HANGAR.EMD cfd1ac42 und Pin unveraendert; Engine unveraendert (Bau LOCAL-BUILD-OK, ctest 4/4). Geometrie unveraendert
  (T5 0,098 H rot wie seit Runde 5 - nach Vorgabe nicht angefasst).
- Bewertung: **wie Original**.

## Was in Runde 3 gegenueber Runde 2 richtig geworden ist
Grau links statt rosa (Chroma 3,5 statt 14,7), Haarflaeche statt Block (Rand Std 6,9 px, Gradient 3,6 statt 10,4), kein Schwarz (18,4 statt
6,2-9), kein Loch rundum (2 / 0 px statt 494 / 360), Kuppe dunkel (31,8 statt 38,0), Nase als Grenze, Seitenansicht stimmig, Koerper und
Schwanz bytegleich. Die Bauarbeit der Runde 3 hat den Nutzer-Satz in Form und Helligkeit umgesetzt; offen ist nur der Farbton der Haare.

## Was der Bauer fuer eine Abnahme aendern muesste (ein Punkt, mechanisch, keine Experimente)
1. Haar-Farbton rotbraun wie Vorlage und Kuppe: Klasse "braun" in ref2tex_v4.klassen_r3 so fassen, dass a* >= b* bzw. Farbton <= 40 Grad gilt
   (die Vorlage hat a* = 4 x b*; Lesung (2) "warm-dunkel", (3) "nahtlos zu den braunen Haaren", Kuppe "rotbraun"), Helligkeit wie jetzt
   (Texel um L* 30-32, Render 0,67-0,75 x Median). Kandidaten aus der Ref2-Zeile 480: 96/64/56 (30,5 / 12,8 / 10,5, 39 Grad), 88/56/48,
   80/48/40 u. ae.; reicht die Zeile nicht, nach Lesung (4) abgedunkelte Ref2-Hautfarben (Farbton ~25 Grad, L* gesenkt) als belegte Ableitung.
   Riegel T-K9b um "Farbton Haar <= 40 Grad bzw. b* <= a*" ergaenzen (Vorlage 14 Grad, Ref2-Kopf 21-25 Grad), damit das messbar bleibt.
2. (Nicht abnahmerelevant, klein) Kuppe mit der oertlichen Ref2-Struktur modulieren statt 2 Eintraegen zu 94 %, falls ohne Risiko moeglich.

Alles andere (Auge, graue Zone, Nase, Haarflaeche in Form und Dunkelheit, Kuppe in Helligkeit, Seite, Koerper, Schwanz) ist in dieser
Fassung in Ordnung und soll so bleiben.
