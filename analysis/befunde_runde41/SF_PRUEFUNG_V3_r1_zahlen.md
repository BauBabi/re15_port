# SF V3 - Zahlen-Pruefung Runde 1 (2026-11-06)

Pruefgegenstand: r41/sichel_nb2 HEAD b0db7dddb (Commits 1e873ab17, 30675ab59, dafe5dc77, b0db7dddb), EMD
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/emd/EM30_V3.EMD`.
Vorgabe: `analysis/befunde_runde41/SF_MESSBLATT_V3.md` Abschnitt 8 (R1-R32) und die Zusatzzeilen R25b/R26b des Bauers.

**Ergebnis: NICHT BESTANDEN** - allein wegen Punkt 5: Im Generator-Diff stehen Konstanten ohne Quellenangabe. Alle 34
Riegel-Zeilen habe ich mit eigenem Code nachgemessen, alle liegen in der Toleranz. Der Ref2-Koerper ist unveraendert,
die 4 Tests laufen gruen, und die Gegenprobe mit V1 ist an Augen und Schwanzfarbe rot. Daneben gibt es zwei Befunde
ausserhalb der Riegel: Die Schwanzspitze reicht im Spiellauf unter den Boden, und in Pose h83 hat der Schwanz 4 Knicke.

## Eigene Werkzeuge (nicht committet, keine Aenderung am Code)

`re15_port/tools/r41_sichel/v3/PZ_emd.py` (eigener EMD/MD1/TIM-Parser, Diff), `PZ_emr.py` (EMR-Bitfelder je Knochen),
`PZ_render.py` (Renders nur ueber die Zeichen-Infrastruktur BF.nah_raster/EA.render/b2.zeichne), `PZ_mess.py` (eigene
Messung: skimage rgb2lab D65 + deltaE_ciede2000, Augen/Maul/Flecken, geodaetische Mittellinie per skimage MCP, Dicke auf
der Normalen, Winkel, Muster), `PZ_machart.py` (R29-R32), `PZ_gang.py` (debug.log der Spiellaeufe), `PZ_gang_maske.py`
(Schwanzkette als projiziertes Rohr). riegel_v3.py/P_*.py wurden NICHT ausgefuehrt.
Laufausgaben: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/pz/`
(m_V3.json, m_V1.json, emd_v3.json, gang_*.json, gangmaske_*.json, Bilder flecken_V3.png, knicke_V3.png,
schulterauge_vorn.png, vorn_kopf.png, boden.jpg, boden_zoom.jpg, ctest.log, build.log).

## 1. Riegel nachgemessen (Bauer | Pruefer eigen)

Kopf: Nahansicht 1200x960, H = 3479 px; R3-R9 auf H 569 (INTER_AREA). Schwanz: Spielkamera 640x480, H = 265 px, Posen h85 / h92.

| Riegel | Soll | Bauer | Pruefer | ok |
|---|---|---|---|---|
| R1 Kopfaugen | genau 1 | 1 | 1 kompakter dunkler Fleck ueber dem Maul auf Mesh 9 (Mitte (652,426), 0,0207 x 0,0098 H); die 8 weiteren Flecken liegen alle im Maul oder darunter (flecken_V3.png) | ja |
| R2 Schulterauge | -0,10..-0,13 / +0,04..+0,07 H | -0,117 / +0,059 | -0,1173 / +0,0586 | ja |
| R3 Kopfauge | 0,017-0,025 x 0,008-0,014 H | 0,0176 x 0,0105 | 0,0176 x 0,0105 (10 x 6 px bei H 569) | ja |
| R4 Form | B/H 1,5-2,5 | 1,67 | 1,67 | ja |
| R5 L* Auge/Haut | 0,35-0,55 | 0,475 | 0,477 | ja |
| R6 Lidring | hell <= 0,10; Lid/Haut 0,70-1,00 | 0,019; 0,806 | 0,019; 0,815 | ja |
| R7 Sclera/Kern | L80 <= 0,05; hell 0,05-0,25; Kern x 0,4-0,7 y 0,55-0,8 | 0,000; 0,186; (0,44 0,73) | 0,000; 0,182; Schwerpunkt (0,44 0,73), hellster Punkt (0,45 0,75) | ja (knapp, s. u.) |
| R8 Pupille/Haut | <= 0,12 | 0,060 | 0,060 | ja |
| R9 Schulterauge | L 0,30-0,50; hell <= 0,05; 0,8-1,3 x | 0,462; 0,043; 1,28 | 0,462; 0,043; 1,26 | ja (knapp) |
| R10 Maul | 0,075-0,095 x 0,038-0,055 H | 0,0805 x 0,0428 | 0,0805 x 0,0428 | ja |
| R11 Maulform | B/H 1,5-2,3 | 1,88 | 1,88 | ja |
| R12 Maullage | 0,04-0,07 H; Versatz <= 0,012 H | 0,0535; 0,0067 | 0,0528; 0,0073 | ja |
| R13 Maul innen | L 0,20-0,40; da >= +4 | 0,213; +5,9 | 0,208; +5,9 | ja (knapp) |
| R14 Zaehne | oben >= 5, unten >= 3, <= 0,005 H, L <= Haut+20, a* 12-20 | 11/4; 0,0032; 43,0 (32,4); 13,8 | 11/4; 0,0032; L 43,0 (Haut 33,0); a* 13,8 | ja |
| R15 L*/Haut h85/h92 | 0,50-0,72 | 0,580 / 0,594 | 0,578 / 0,593 | ja |
| R16 C*/Haut | 0,45-0,90 | 0,55 / 0,53 | 0,547 / 0,531 | ja |
| R17 dh | abs <= 10 Grad | -7,3 / -8,4 | -7,3 / -8,4 | ja |
| R18 Rot | <= 0,8 x Haut | 0,225 (0,375) / 0,219 (0,381) | 0,225 (0,374) / 0,219 (0,381) | ja |
| R19 dE00 | 6-11 | 10,1 / 10,0 | 10,12 / 10,06 (Lab Schwanz [16,4 8,0 2,8], Haut [28,4 13,9 7,0]) | ja |
| R20 Muster | AK <= 0,25; Rest-Std a* <= 2,0 | 0,10; 1,67 / 0,01; 1,65 | AK-Maximum 0,21 bei 0,049 H / 0,21 bei 0,045 H; Rest-Std 1,28 / 1,08 (Querschnittsmittel) | ja |
| R21 Dicke s 0,2 | 0,15-0,20 H | 0,164 / 0,155 | 0,161 / 0,156 | ja |
| R22 Dicke s 0,5 | 0,07-0,10 H | 0,085 / 0,081 | 0,087 / 0,077 | ja |
| R23 Dicke s 0,9 | 0,028-0,045 H | 0,036 / 0,034 | 0,037 / 0,033 | ja |
| R24 Verlauf | kein Anstieg > 0,01 H | 0,000 / 0,000 | 0,000 / 0,000 | ja |
| R25 Glaette | <= 36 Grad | 27,4 / 24,4 | 26,0 / 21,6 | ja |
| R26 Knicke | <= 3 | 2 / 1 | 1 / 1 (s 0,10 / 0,09 = Austritt am Buckel) | ja |
| R27 Austritt | 0,10-0,16 H | 0,145 / 0,146 | 0,144 / 0,147 | ja |
| R28 Koerper | = Ref2 | gleich, Mesh 8 +1 Viereck | siehe Abschnitt 2 | ja |
| R29 Dreiecke Schwanz | <= 212 | 160 | 160 (Mesh 16/22 je 30, 17-21 je 20) | ja |
| R30 Kopf | <= 118; <= 2 | 109; 2 | 109; 2 (Mesh 8: 131 -> 133) | ja |
| R31 Texeldichte | Schwanz 0,045-0,090; Kopf 0,05-0,091 | 0,046-0,082; 0,073 | 0,046 / 0,053 / 0,054 / 0,059 / 0,069 / 0,080 / 0,082; Mesh 9 0,072 (nur Kopfbild-Flaechen 0,067) | ja (Glied 0 knapp) |
| R32 Palette | 8 bpp, Haut-CLUT, C* <= Haut | 8 bpp; 482; 29,7 <= 63,3 | 8 bpp (TPage-Bits 1); nur Zeile 482; C* max 29,7 <= 63,3 (P95 21,3 <= 33,1) | ja |
| R25b/R26b Gang h84-h132 | Raum <= 25 Grad, <= 3 ueber 20 | 16,2; 0 | 16,2; 0 (49 Takte, debug.log sfv3/r/V3) | ja |
| R25b/R26b Halten h16-h70 | dito | 16,2; 0 | 16,2; 0 (55 Takte) | ja |

Knappe Zeilen, an der Messschwelle: R7 Kern y 0,73 bei 6 px Augenhoehe (mit 5 px Hoehe laege er bei 0,90 = rot);
R9 1,26 (Grenze 1,3); R13 0,208 (Grenze 0,20); R31 Glied 0 0,046 (Grenze 0,045).

Weitere Zahlen ohne eigene Riegel-Zeile:
- Schwanz von vorn h118 / h125: 0 / 3 Bildpunkte (640er).
- Ende: 0,05 H vor der Spitze 0,033 / 0,029 H (KI 0,028 / 0,025).
- Wurzel: s 0,1 0,175 / 0,167 H (Richtwert 0,17-0,21).
- Maulbreite / Kopfbreite in der Maulzeile 0,92 (Vorgabe 0,75-0,95).

## 2. Ref2-Koerper und Spiel-Asset (eigener Parser PZ_emd/PZ_emr, Bezug EM30_HANGAR.EMD)

- Spiel-Asset sha256 cfd1ac42729e5636... = Pin `release/em30_hangar.sha256`. `git status` zeigt keine Aenderung in shared_assets.
- Neubau aus HEAD:
  - Ref2 (`birkin_formen.py --aus pz/emd_ref2`) cfd1ac42... byte-gleich.
  - V1 (`--variante v1_spitz`) 8cea0513... gleich.
  - V3 (`--variante v3`) 9a0d36305bfb8575... = gemeldete EMD = Lauf-Overlay `sfv3/r/V3/cd/EMD/EM30_HANGAR.EMD`.
- MD1, Meshes 0-15: Vertices, Normalen, Tri- und Quad-Faces 0 Abweichungen (max |dv| 0).
  - Mesh 8: 69 -> 73 Vertices/Normalen und 27 -> 28 Vierecke, die 27 alten Vierecke unveraendert.
  - Mesh 9: Geometrie gleich, nur 48 Tri-UVs und 13 Quad-UVs (CLUT 485 -> 485/486) geaendert.
- EMR (Abschnitt 2): Knochen 0-15 sind gleich in Bindversatz, Kinderliste und allen 912 Keyframes (Bitfelder und
  12-Byte-Kopf). Abweichend sind nur die Knochen 16-22 (Schwanz). EDD (Abschnitt 1) und die Abschnitte 0, 3-6 sind
  byte-gleich.
- TIM: 2650 Texel geaendert (Bbox x 0-53, y 156-219). Davon liegen 0 in einer UV-Flaeche der Ref2-Meshes 0-15. Die
  CLUT-Zeilen 480-485 sind unveraendert. Zeile 486 ist neu und wird nur von Mesh 9 benutzt.
- **Vertex-Diff der Nicht-Kopf-/Nicht-Schwanz-Meshes = 0.**

## 3. Tests

`bash re15_port/tools/local_build.sh build` meldete "ninja: no work to do" und LOCAL-BUILD-OK; die exe entspricht also HEAD.
ctest der 4 Pflichttests: unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41 und integration_r41_sichel_form
**4/4 bestanden** (64,85 s, Protokoll pz/ctest.log).

## 4. Gegenprobe V1 SPITZ (eigene Messung)

Kopf rot:
- R3 0,007 x 0,007 H
- R4 1,00
- R5 0,215
- R6 Lid hell 0,867, Lid/Haut 1,774
- R8 0,144
- R9 aequiv 4,41 x
- R10 0,040 x 0,047 H
- R11 0,85

Schwanzfarbe rot:
- R15 0,836 / 0,810
- R16 1,466 / 1,413
- R18 0,874 > 0,8 x 0,369
- R19 5,79 / 5,82

**Die Gegenprobe ist erfuellt.** Abweichung zur Bauer-Messung: R20 V1 misst bei mir Rest-Std 1,76 (beim Bauer 3,95).
Mein Querschnittsmittel glaettet die Ringe. Fuer V3 aendert das nichts.

## 5. Konstanten im Generator-Diff (git diff cd68e9799..HEAD -- birkin_formen.py, v3/)

SCHWANZ_V3, KOPF_V3 und KOPF_V3_ZEILE tragen Quellen (Messblatt-Zeile, Probe-Nummer oder PORT-WAHL). Die Farben der
Zeile 486 habe ich gegen die Lab-Zitate nachgerechnet; Abweichung <= 1,4 L*. **Ohne Quellenangabe sind:**

| Datei:Zeile | Konstante | Mangel |
|---|---|---|
| birkin_formen.py:1953-1957 | Rampenmitte 0,55 / 0,5 / 0,5 und Rauschgewicht 0,25 / 0,25 / 0,3 (Kuppe/Haut/rot) | keine Quelle |
| birkin_formen.py:1968 | `ey / 0.6` (Achsverhaeltnis Maulkern) | keine Quelle (mit tief = 0,0 wirkungslos) |
| birkin_formen.py:1972 | `abs(ex) <= 0.97` (Schlundbreite) | Begruendung "ganze Maulbreite", Zahl ohne Beleg |
| birkin_formen.py:1977-1978 | Zahnform `clip(.., 0.35, 1.0)`, `d_in >= -0.002 * Hp` | keine Quelle |
| birkin_formen.py:969 | v-Lage der Spitzen-UV `0.92` | keine Quelle |
| birkin_formen.py:2055 | Ader-Schwelle `Lf < 0.6 * med` | Probe 5 begruendet das Uebermalen, nicht den Wert 0,6 |
| birkin_formen.py:2110 | Nachbarschaft `< 400.0` E. fuer die Lichtnormale | Probe 9 begruendet den Weg, nicht 400 |
| birkin_formen.py `_rausch_feld` | Abtastung `(yy * 3)`, `(xx * 2)` | keine Quelle |
| KOPF_V3 `rauschen=1.3` | "Probe 8 mit 0,9 wirkte die Kuppe glatt grau" | Augenurteil ("wirkte"), keine Zahl |
| KOPF_V3 `zonen.kuppe_bis=0.012`, `rot_ab=-0.004` | zitiert K9/K15 | K15 nennt 0,04-0,08 H unter Kopf-OK, die Zahlen 0,012 / -0,004 stehen nicht im Messblatt |
| schwanz_r41.c (ausserhalb des Pruefbefehls) | SW_GLATT_MIN 12 Grad, SW_GLATT_RAND 3 Grad | keine Messung; MAX 16 ist nur gegen 22 geprobt |

## 6. Befunde ausserhalb der Riegel

1. **Schwanzspitze unter dem Boden im Spiellauf V3.**
   - debug.log `sfv3/r/V3`: In der Halte-Szene (clip 11), Takte h32-h36, h60, h65-h71 und h84 (14 Takte), liegen die
     Gelenke 6/7 bis **612 E.** (h70, ~0,16 H) unter Birkin-y.
   - V1 und Ref2: 0 Takte; das tiefste Gelenk bleibt dort >= 100 E. ueber dem Boden.
   - Ursache: glaetten() in schwanz_r41.c, Nachtrag "Boden nur ohne neuen Knick". Der Code laesst die Spitze lieber
     unter den Boden, als einen Knick zu erzeugen.
   - Das steht nicht in Dossier SV3.5. Bilder: pz/boden.jpg, pz/boden_zoom.jpg. Ob das Spielbild die Spitze im Boden
     zeigt, ist nicht abschliessend geprueft: Die Gelenkmarken liegen gegen das gerenderte Bild versetzt.
2. **Pose h83 (Leiste 01, keine Riegel-Pose):**
   - 4 Knicke > 20 Grad (s 0,12 / 0,27 / 0,69 / 0,83), max 35,6 Grad je 0,05 H. Die Spitze haengt sichtbar abgeknickt
     herab (pz/knicke_V3.png).
   - KI h83 in Port-Aufloesung: 3 Knicke.
3. **Gang im Bild** (Schwanzkette als Rohr projiziert, ohne Verdeckung, Wurzel-15 % ausgenommen):
   - Leistenbilder h84 / h90 / h96: 17,0 / 15,5 / 18,4 Grad, je 0 Knicke.
   - h102 / h108 / h120: 45,0 / 49,1 / 32,1 Grad.
   - Ueber den ganzen Gang: 8 von 49 Takten > 36 Grad (Spitze 55,6 Grad bei h107), Knicke <= 2.
   - In h102-h120 ist der Schwanz im Leistenbild hinter dem Koerper verdeckt (Leiste 04, Spalte V3). Die Winkel dort
     sind Verkuerzung, im Raum hoechstens 16,2 Grad.
   - Die Leiste selbst druckt fuer V3 h102 "Bild ... 4 > 20".
4. **Schulterauge in der Spielkamera:**
   - Sichtbar mit 418 / 456 Bildpunkten (h118 / h125), Auge L*/Haut 0,41 / 0,44.
   - Das Viereck hat Texeldichte 0,032 Texel/E. (Haut 0,047-0,128). Es zeigt sich als groeber Fleck mit erkennbaren
     Kanten (pz/schulterauge_vorn.png).
   - R31 misst je Mesh (Mesh 8 0,051) und erfasst das nicht.
5. Riegel-Definition: R25b/R26b (Raum <= 25 Grad) und die R1-Zusatzfilter in riegel_v3.py (0,008 H Zusammenfassung,
   Ring 0,004-0,012 H) stehen nicht im Messblatt. Sie sind Erweiterungen des Bauers.

## Abweichungen

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| (5) Quellen der Generator-Konstanten | jede Konstante mit Quelle | ~12 Zeichen-Konstanten ohne Quelle (Tabelle Abschnitt 5) | Nachweis fehlt -> nicht bestanden |
| Spitze unter dem Boden (Spiellauf) | nicht unter dem Boden (Ref2/V1: >= 100 E. darueber) | bis 612 E. darunter, 14 Takte | Regression durch glaetten(), nicht dokumentiert |
| h83 Knicke | <= 3 (R26-Massstab) | 4, max 35,6 Grad | keine Riegel-Pose, Hinweis |
| Schulterauge Texeldichte | 0,045-0,090 (Haut) | 0,032 | Luecke im Riegel R31, Hinweis |
