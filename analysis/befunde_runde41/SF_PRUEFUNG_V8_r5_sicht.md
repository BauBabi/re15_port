# SF Pruefung V8 Runde 5 - Sicht (Auge des Nutzers)

Stand: 2026-11-10. Zweig `r41/sichel_v8`, HEAD `5820af00f`.
Gepruefte Datei: `C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD`,
sha256 `c4976c6214c74fb1ac341fc0ed4ec0768e15c34b0b1939c53b68e04aabd9526a` (= Pin `release/em30_hangar.sha256`, selbst nachgerechnet).

## Ergebnis

**Nicht bestanden.** Die dunkle Luecke zwischen Ruecken und Kopf (P5) ist geschlossen, und die Schwanz-Unterseite ist
fleischig, die Oberseite grau (P4). Aber das, was die Luecke schliesst, faellt selbst auf: Der Kopf sitzt von vorn in
einem hellen, kantigen Kragen mit gestreckter Streifentextur, links neben dem Gesicht steht in allen 35 Frontbildern
F344-F378 ein blaeulich-weisser Keil, und in zwei aufeinanderfolgenden Bildern (F313, F314) liegt der Kragen ueber
rund der Haelfte des Gesichts. Genau diese Dinge nennt der Auftrag als Ausschluss (Kragen, Segel, Spitze, gestreckte
Textur, etwas ueber dem Gesicht).

| Punkt | Bewertung |
|---|---|
| (1) Koerperbau | wie gewuenscht |
| (1c) Luecke Ruecken-Kopf geschlossen | wie gewuenscht (Posen, F262-F310, F332, F344-F378) |
| (1c) Einzelbild F311 | Abweichung klein - Einzelbild, Entscheidung beim Nutzer |
| (1c) Schliessendes faellt nicht auf | **Abweichung, die der Nutzer ablehnen wuerde** |
| (1b) Unterseite fleischig, Oberseite grau | Abweichung klein |
| (2) Huegel, Linie zum Kopf, von vorn keine Kappe | Seite wie gewuenscht; von vorn **Abweichung, die der Nutzer ablehnen wuerde** (selbe Ursache wie 1c) |
| (3) Rechte Seite, keine Platte, keine Keile | Adern/Platte wie gewuenscht; Keil neben dem Gesicht **Abweichung, die der Nutzer ablehnen wuerde** (selbe Ursache wie 1c) |
| (4) Gesicht und linke Hand wie V7 | wie gewuenscht (Gesicht selbst gemessen, linke Hand vom Bauer uebernommen) |
| (5) Szene in Bewegung | Abweichung klein |
| (6) Fortschritt gegenueber V7 | **Abweichung, die der Nutzer ablehnen wuerde** |

Die Eintraege der dritten Klasse haben EINE Ursache (der Kragen um den Kopf), es sind nicht mehrere getrennte Fehler.

## Eigene Laeufe

Arbeitsordner: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s`
(im Folgenden `<P>`; Kacheln unter `<P>/k/`, vollstaendige Pfade in der Belegliste am Ende).

- Szenenlauf mit Einzelbildern (Werkzeug `re15_port/tools/r41_sichel/v4/lauf_v4.sh`, F225-F450, 960x720):
  `<P>/szene_fd/fd/f000225.ppm` bis `f000450.ppm`. state.log 1131 Zeilen, bytegleich zum V7-Lauf, Ende "ROOM50A0 Szene Ende: Cut 4", EXIT 0.
- Echtes Fenster per gdigrab, roh, 30 Bilder/s: `<P>/gdi/gdi_pruef5.mp4`, Einzelbilder `<P>/gdi/g/g0000.png` bis `g0164.png`.
  Zuordnung per Bildvergleich gemessen: gdigrab-Bild i = Szenenbild F(i+218). Gueltig ist i = 20..151 (F238-F369);
  danach lag ein fremdes Fenster darueber. Ein zweiter gdigrab-Lauf war ganz verdeckt und ist verworfen; kein dritter
  Versuch (haette den Nutzer am Bildschirm gestoert). Damit ist der Abschnitt F370-F398 nur aus den Einzelbildern beurteilt,
  nicht aus dem echten Fenster.
- Vergleichsstaende: V7 (`319bbd17`) aus `.../scratchpad/sfv4/r/V7_szene/fd/`, V8 Runde 4 (`10d02173`) aus
  `.../scratchpad/sfv8/pr4s/szene_fd/fd/` (nur bis F380 vorhanden).
- Angesehen: alle 15 Tafeln des Bauers `analysis/befunde_runde41/SF_belege/v8/01_darstellung.png` bis `15_ruecken_kopf.png`,
  `runde5/versuch_F311.png`, `runde5/versuch_F332.png`, `runde5/vorlage_luecke.png`, sowie
  `C:/workspace/git/reAi_v2/Version_80%/Darstellung.png`, `Beschreibung1.png`, `Beschreibung2.png`.
- Nicht selbst nachgefahren (vom Bauer uebernommen): ctest 8/8 und 1013/1013, V8-KETTE-OK, Riegel 18 von 19 gruen,
  S5 "Gesicht bytegleich V7 (Mesh 9, 6501 Texel)". Kein eigener Bau.

Alle Koordinaten und Pixelzahlen gelten fuer 960x720.

## Seite

"Rechte Seite" = die Flanke, die in PV (n1439) und P2 (Foto 2, n1435) zur Kamera zeigt; von vorn liegt sie links im Bild.

## Antworten

### (1) Koerperbau wie die Vorlage - wie gewuenscht

PV und P2 folgen den Linien aus `Darstellung.png`: Der Schwanz kommt breit und ohne Absatz aus dem Ruecken und laeuft
im Bogen bis zum Boden (Tafeln `01_darstellung.png`, `07_bogen_jedes7.jpg`). Im eigenen Lauf dasselbe Bild
(`<P>/k/uebersicht.png`, `<P>/k/gdi_seite.png`). Kein Befund.

### (1c) Luecke zwischen Ruecken und Kopf (P5)

**Geschlossen - wie gewuenscht.** Die dunkle Kerbe aus Runde 4 ist in den Foto-Posen PV/P1/P2 (Tafel `15_ruecken_kopf.png`)
und in den Szenenbildern F262-F310, F332 und F344-F378 weg (`<P>/k/c3_kopf_a.png`, `c3_kopf_b.png`, `c3_kopf_e.png`,
`c3_kopf_f.png`: je V7 | Runde 4 | Runde 5d). Ein neuer Huegel zwischen Ruecken und Kopf entsteht in den Seitenansichten nicht.

**F311 - Abweichung klein - Einzelbild, Entscheidung beim Nutzer.** Links neben dem Gesicht bleibt ein dunkler Keil
(Bauer: 55-63 Bildpunkte, 31-37 tief, 5,8-6,3 breit; Soll aus der Vorlage 21 / 7 / 3,2). Nur dieses eine Bild
(= gdigrab g0093), in Echtzeit 1/30 s. Kachel `<P>/k/c3_kopf_c.png` (F311-F314) und `<P>/k/gdi_sturz.png` (g0092-g0097).
F332 ist zu (Bauer 6 / 3 / 1,0; eigene Kachel `c3_kopf_e.png`).

**Das Schliessende faellt auf - Abweichung, die der Nutzer ablehnen wuerde.** Vier Befunde:

1. **Kragen um den Kopf, die ganze Frontansicht lang.** Von vorn sitzt der Kopf in einem hellen, kantigen Rahmen aus
   grossen flachen Flaechen mit gestreckter Streifentextur; ueber dem Scheitel bildet er einen Bogen mit Spitze.
   Sichtbar F262-F310 (49 Bilder, 1,6 s; g0044-g0092) und F344-F378 (35 Bilder, 1,2 s; im echten Fenster g0126-g0151).
   V7 hat dort die geschlossene graue Rueckenmasse, Runde 4 eine weiche Flaeche ohne Rahmen.
   Kacheln: `<P>/k/c3_keil.png` (F352, F366: V7 | Runde 4 | Runde 5d), `c3_kopf_f.png` (F346/F352/F360/F372),
   `gdi_kopf_vorn.png` (g0130-g0150), `gdi_kopf_seite.png` (g0046-g0090). Der Bauer schreibt selbst "Kragen bleibt sichtbar".
2. **Blaeulich-weisser Keil links neben dem Gesicht.** In 35 von 35 Bildern F344-F378 liegen 28 bis 57 Bildpunkte
   helles Blaeulich-Weiss (B >= R-6, B > 120, R > 110) neu gegenueber V7 im Kasten (420,200)-(640,400).
   Als Gesamtzahl im selben Kasten: V7 69-76 (das sind die Glanzlichter im Gesicht), Runde 4 92-106, Runde 5d 99-125 -
   nach diesem Mass ist das helle Blaeulich-Weiss neben dem Kopf nicht weniger geworden als in Runde 4.
   Sichtbar als etwa 50 x 10 Bildpunkte grosse Klinge mit dunkelblauem Strich darin (`c3_keil.png`, `gdi_kopf_vorn.png`).
3. **Helles spitzes Segel links ueber dem Kopf** in F278, F306, F309 (im echten Fenster gesehen in g0054, g0062, g0090), in F311 eine helle Spitze
   rechts vom Kopf (`c3_kopf_a.png`, `c3_kopf_b.png`, `c3_kopf_c.png`). Von hinten faecherartig gestreckte Streifen
   (F314, F322-F340; `c3_kopf_d.png`, `c3_kopf_e.png`).
4. **Der Kragen liegt in zwei aufeinanderfolgenden Bildern ueber dem Gesicht.**
   F313, Kasten (596,252)-(640,290): 673 von 1442 Bildpunkten (47 %) anders als V7, vom Gesichtsrot 235 von 499 (47 %) verdeckt.
   F314, Kasten (422,310)-(476,368): 1055 von 2693 (39 %) anders, vom Gesichtsrot 163 von 310 (53 %) verdeckt.
   F312 und F315: 0 %. Kacheln `<P>/k/kopfdeck_b.png` (F311-F314, magenta = neu verdeckt), `c3_f313.png`, `c3_f314.png`.
   Die Pruefung S16(c) des Bauers ("Einsatz in keiner der 177 Ansichten ueber Gesichts-Bildpunkten von V7") prueft nur den
   Einsatz, nicht den Kragen - deshalb ist sie gruen, obwohl das Gesicht hier verdeckt wird.
   In Echtzeit sind das 2/30 s im Sturz; allein waere das ein Einzelbild-Befund, zusammen mit 1-3 nicht.

In den Frontbildern selbst (F270, F278, F306, F352, F372) liegt nichts ueber dem Gesicht: die Kopfoeffnung ist dort
bytegleich V7 (`<P>/k/kopfdiff_a.png`, `kopfdiff_all.png`, gruen = bytegleich).

### (1b) Unterseite des grossen Schwanzes (P4) - Abweichung klein

Die Richtung stimmt mit dem Nutzerwort: Unterseite rot-fleischig, Oberseite grau, in PV/P2 und in der Szene
(`<P>/k/schwanz_unten.png`, `video_schwanz.png`, `gdi_seite.png`). Kein aufgemalter Einzelfleck.

Gemessen (Lab; Modell: rote Flaeche mit a* > 22 und L* < 45, die von V7 abweicht; Vorlage: die rote Klinge):

| Quelle | L* | a* (Median) |
|---|---|---|
| Vorlage, nach vorn geschwungene Klinge f1370-f1374 | 33-36 | 19 (90 % bis 22-25) |
| Vorlage f1416 / f1420 | 25-26 | 20 |
| Modell Runde 5d, Szene F256-F330 (1,3k-4k Bildpunkte je Bild) | 16-19 | 23-26 |
| Modell Runde 5d, F386-F396 (bis 7,9k Bildpunkte) | 11-17 | 23-26 |

Das Modell ist dunkler und gleichmaessiger gesaettigt als die Vorlage und hat deren helle Flecken nicht. Das allein
ist kein Ablehnungsgrund. Was den Uebergang "nicht ruhig" macht:

- Grau-gruene linsenfoermige Schlitze/Baender und dunkle Pixelbloecke an den Segmentgrenzen der geschwungenen Klinge
  (F262-F278, im echten Fenster g0044-g0056; `<P>/k/schwanz_block.png`, `folge.png`, `video_folge.png`).
- **Leuchtend rote "Tonne mit grauen Reifen" oben auf dem Ruecken in F315-F324** (im echten Fenster g0097-g0105, 0,3 s): dort zeigt
  die Unterseite der Schwanzwurzel zur Kamera; in V7 und Runde 4 ist die Stelle grau.
  Kacheln `<P>/k/c3_kopf_d.png` (F316-F324), `gdi_tonne.png` (g0096-g0107), `kopfdeck_c.png`.
- Im Leon-Schnitt F380-F398 zieht eine rote Sichel quer durchs Bild, V7 grau (`<P>/k/sichel_schnitt.png`). Das ist die
  gewuenschte fleischige Unterseite; nur aus den Einzelbildern beurteilt (siehe Eigene Laeufe).
- F328-F342: ruhiger dunkel-lila Rand, kein Befund.

### (2) Ruecken mit kleinem roten Huegel, glatte Linie zum Kopf, von vorn kein Kamm/Kappe/Grat

Seitlich (PV, P2, Tafel `15_ruecken_kopf.png`): kleiner roter Huegel, Linie laeuft glatt und geschlossen zum
Kopf - **wie gewuenscht**.
Von vorn: Der Kragen aus (1c) woelbt sich als heller Bogen mit Spitze ueber den Kopf (`c3_keil.png`, `c3_kopf_f.png` F372,
`gdi_kopf_vorn.png`). Das ist eine Kappe von vorn - **Abweichung, die der Nutzer ablehnen wuerde**, selbe Ursache wie (1c).

### (3) Rechte Seite mit Original-Adern, von vorn keine graue Platte, keine harte Kante, keine weissen Keile

- Original-Adern auf der rechten Flanke vorhanden (Tafeln `05_adern_rechts.png`, `10_adern_rechts_original.png`) - wie gewuenscht.
- Die graue Brustplatte und die weissen Keile RECHTS vom Gesicht aus Runde 4 sind weg (Tafel `09_vorn_kopf.png`;
  eigene Kachel `c3_kopf_f.png`) - wie gewuenscht.
- Neu: der blaeulich-weisse Keil LINKS neben dem Gesicht in allen 35 Frontbildern (Zahlen unter 1c Nr. 2) und die harte
  Kante des Kragens - **Abweichung, die der Nutzer ablehnen wuerde**, selbe Ursache wie (1c).
- Klein: ein blasser spitzer Keil in der Flanke in PV (Tafel `05_adern_rechts.png`).

### (4) Gesicht und linke Hand wie V7 - wie gewuenscht

Gesicht: In den Frontbildern F270/F278/F306/F352/F372 ist die Kopfoeffnung bytegleich V7 (eigene Kacheln `kopfdiff_a.png`,
`kopfdiff_all.png`). Der Bauer meldet S5 gruen (Mesh 9, 6501 Texel bytegleich) - uebernommen.
Die Verdeckung in F313/F314 ist kein geaendertes Gesicht, sondern Kragen davor; gewertet unter (1c).
Linke Hand: nicht eigens pixelweise verglichen; im echten Fenster (`gdi_kopf_vorn.png`, dunkelgrauer Arm rechts im Bild)
ohne sichtbaren Unterschied zu V7, Riegel des Bauers gruen - uebernommen.

### (5) Szene in Bewegung - Abweichung klein

Ablauf unveraendert (state.log bytegleich V7). Kein Durchstossen, keine aufklaffende Naht gesehen
(`<P>/gdi/gdi_pruef5.mp4`, `<P>/k/gdi_dreh.png`, `gdi_vorn.png`). In Echtzeit faellt auf: der helle Rahmen um den Kopf
waehrend der Frontansicht (gewertet unter 1c) und fuer 0,3 s die rote Tonne auf dem Ruecken (1b). F311 und F313/F314
sind in Echtzeit nicht einzeln zu erkennen.

### (6) Klarer Fortschritt gegenueber V7, den der Nutzer annehmen wuerde - Abweichung, die der Nutzer ablehnen wuerde

Besser als V7 und Runde 4: Luecke zu, Unterseite fleischig, graue Platte und rechte weisse Keile weg, Koerperbau haelt.
Aber die Beanstandung P5 ("Luecke zwischen Ruecken und Kopf") ist durch ein neues auffaelliges Teil am Kopf ersetzt -
an der Stelle, auf die der Nutzer seit mehreren Runden schaut. So wuerde ich es dem Nutzer nicht als fertig vorlegen.

## Punkte fuer die naechste Runde

1. Kragen flach machen oder weglassen: kein heller Bogen mit Spitze ueber dem Kopf, kein flaches Segel links
   (F278, F306, F309), keine gestreckten Streifen (UV der grossen Dreiecke), kein blaeulich-weisser Keil links
   neben dem Gesicht (F344-F378; Mass: neu gegenueber V7 im Kasten (420,200)-(640,400) heute 28-57 Bildpunkte, Soll 0).
2. Der Kragen darf in F313 und F314 nicht vor dem Gesicht liegen (heute 47 % / 53 % des Gesichtsrots verdeckt, Soll 0).
   Die Schranke S16(c) muss den Kragen mitpruefen, nicht nur den Einsatz.
3. F311: Keil links vom Gesicht (55-63 / 31-37 / 5,8-6,3 gegen Soll 21 / 7 / 3,2).
4. Schwanz-Unterseite: graue Baender/Schlitze und dunkle Bloecke an den Segmentgrenzen (F262-F278) beruhigen;
   Unterseite der Schwanzwurzel in F315-F324 (rote Tonne mit grauen Reifen) daempfen oder die grauen Reifen entfernen.
5. Einsatz unter dem Kiefer ist als Knoten zu sehen (Bauer nennt es selbst).

## Belegliste (eigene Kacheln, absolute Pfade)

Ordner: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/`

- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/c3_keil.png` - Kragen und Keil von vorn, F352/F366, V7 | Runde 4 | Runde 5d
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/c3_kopf_f.png` - F346/F352/F360/F372
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/gdi_kopf_vorn.png` - echtes Fenster g0130-g0150
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/gdi_kopf_seite.png` - echtes Fenster g0046-g0090
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/kopfdeck_b.png` - F311-F314, Verdeckung des Gesichts
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/c3_f313.png` und `c3_f314.png` im selben Ordner - F313, F314 gross
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/c3_kopf_c.png` - F311-F314
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/gdi_sturz.png` - echtes Fenster g0092-g0097 (= F310-F315)
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/c3_kopf_d.png` und `gdi_tonne.png` im selben Ordner - rote Tonne F316-F324 / g0096-g0107
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/sichel_schnitt.png` - Leon-Schnitt F380-F398, V7 gegen Runde 5d
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/kopfdiff_a.png` und `kopfdiff_all.png` im selben Ordner - Gesicht bytegleich V7 (gruen)
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/k/schwanz_unten.png`, dazu `schwanz_block.png`, `folge.png`, `video_folge.png` im selben Ordner - Schwanz-Unterseite
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5s/gdi/gdi_pruef5.mp4` - Echtzeit-Aufnahme des echten Fensters
