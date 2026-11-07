# Farbe V3, Leser A: G4-Atlas und Teile-Zuordnung K5c <- RE2-G4 (2026-11-07)

Auftrag: Orchestrator, Spur Form, Farbe Versuch 3. Nutzerwortlaut 2026-11-07: "schwarz rot ... verbrannt", "Gesicht vorne ... in flammenfarben".
Nur Lesung und Probe: kein Bau, kein EMD/TIM geschrieben, kein Commit. Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL). Gemessen wird, was die
G4-Texel sind und wie sie sich auf K5c legen lassen. Seiten: (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, OpenCV float (RGB/255), wie in V1/V2.

Werkzeuge (Scratchpad, nicht versioniert): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/A/`
- `kern3.py`: Texel -> 3D je Mesh, Inseln, Flächeninhalt
- `inseln.py` / `k5inseln.py`: UV-Inseln -> `g4_inseln.json`, `k5c_inseln.json`
- `felder.py`: -> `g4_felder.json`
- `uebertrag.py`: Verfahren (b1)
- `uvprojekt.py`: Verfahren (b2)
- `probe.py` / `probe2.py`: Proben
- `voll.py` / `voll2.py`: ganze Figur
- `stufen3.py`: Posterisierung
- `feuer.py`: G2-Ziele -> `feuer.json`

Lader: `formfarbe/A/lade.py` (Parser `re15_port/tools/emd_ansichtsblatt.py`, `birkin_bahnsteig.modell`, `form_emd.modell`). Raster: `form_buehne.rastern`.

Render überall gleich, für alle Modelle:
- Licht 0,35 + 0,65 max(0, N.L), weiß, von oben links vorn (`formfarbe/A/render.py`)
- roh, ohne Gamma; Umkugel 0,46 der Kachel
- G4 Clip 0, K5c Clip 2 (Ruhe)

Belegbilder (`varianten/K5/farbe_lesung/`): `V3A_0_uebersicht_g4_k5c.png`, `V3A_1_g4_atlas_felder_x3.png`, `V3A_2_probe_bein.png`,
`V3A_3_probe_kopf.png`, `V3A_4_ganze_figur.png`.

## 0. Kurzfassung

1. **K5c ist nicht aus G4 abgeleitet, G4 hat ein anderes Gerippe.**
   - G4: 27 Knochen, 1427 Flächen, Vierbeiner ohne Schwanz und ohne eigenen Kieferknochen.
   - K5c (= J + K5-Kopf): 32 Knochen, Ringbau aus `rohbau_final` (DOSSIER Abschnitt 3: "Ring-Bauweise ... je Teil ein benannter Streifen, U Umfang, V Länge").
   - Es gibt keine Eckpunkt-Entsprechung. Verfahren (a), die UV-Übernahme per Eckpunkt, geht darum bei **keinem** Teil. Alles läuft über (b), die Abbildung entlang der Längsachse.
2. **Die Kette der Teile passt trotzdem 1:1.** Das gilt für den Rumpf (Becken - Lende - Brust), die Hinterbeine (Oberschenkel - Unterschenkel - Fuß - Krallen),
   die Vorderbeine (Oberarm - Unterarm - Fuß - Krallen) und den Kopf. Nur Schwanz und Hals haben im G4 kein Gegenstück.
3. **Die Neuabtastung in die vorhandene K5c-UV (b1) posterisiert wie Versuch 2.**
   - Anteil der Fleisch-Nachbarpaare mit dL = 0: Körper 54,2 %, Kopf 73-79 %. Zum Vergleich: G4 selbst 17,2 %, V2 52,9 %.
   - Quelle `stufen3.json`; Maß wie `formfarbe2/v2r2sicht/stufen.py`.
   - **Empfohlen ist (b2), die UV-Projektion.** Jede K5c-Ecke bekommt die G4-UV ihres Gegenpunkts. Die G4-Texel kommen 1:1 (bytegleich, mit ihren CLUTs) in den K5c-Atlas.
   - Am Bein zeigt (b2) die G4-Adern durchgehend und ohne Blockringe (`V3A_2_probe_bein.png`).
4. **Der Kopf ist das Problemteil.**
   - Größe: Das G4-Kopffeld F1 hat 1814 Texel. Es müsste eine K5c-Kopf- und Kieferfläche von 15,8 Mio. Einheiten² decken (G4-Kopf: 2,2 Mio.).
     Das ergibt eine lineare Vergrößerung von 3,1; alle anderen Teile liegen bei 0,6-1,9.
   - Farbe: Der G4-Kopf ist oben **hell und knochenfarben** (beige Schnauze, L\* p95 67), nicht schwarz-rot (`V3A_3_probe_kopf.png`).
5. **G2 gemessen:**
   - Bild 1, "Kopf vorn": Ziel ist ein rosa-magenta Texel (133,81,104), Lab (40,9 / 25,2 / -3,9), Farbton -9 Grad.
   - Bild 2, "Maul": Ziel ist ein heller, cremefarbener Texel (234,208,189), Lab (84,7 / 6,1 / 12,7), Farbton 64 Grad.
   - **Den Farbton aus Bild 1 hat kein G4-Feld**: Alle G4-Rottöne liegen bei 23-44 Grad.
   - Für Bild 2 passen die hellen G4-Felder auf dE 3,7-3,9: F1 (Schnauze), F8 (Stachelkranz), F15/F21 (Krallen).
   - Die gesättigten gelb-orangen "Feuer"-Texel des G4 liegen vor allem in F8 (33 %) und im großen Auge F16 (16,5 %).

## 1. RE2-G4 (Typ 0x34): Aufbau und Atlas

**Quelle und Format:**
- Datei `re15_port/shared_assets/RE2/CDEMD0.EMS`, Typ 0x34; EMD @0x677000 (273 812 Byte), TIM @0x65e800 (99 872 Byte) (`lade.re2`).
- Atlas 384 x 256 Texel, 8 bpp, 3 Seiten zu je 128 Texel Breite, 3 CLUT-Zeilen.
- **Jede Fläche nutzt die CLUT ihrer Seite**: 1427 von 1427 Flächen erfüllen `uv[5]&0xF == CLUT-Zeile - 480`.
- Jede Seite nutzt 255-256 Indizes. Die drei CLUTs sind verschieden (je Paar nur 2-4 gleiche Einträge). Zusammen hat der genutzte Atlas 648 Farben.

**Knochen** (Eltern in Klammern bzw. aus der Kette; Lage in Clip 0 Bild 0; Welt: x vorn, y unten, z links (W)):

| Knochen | Rolle |
|---|---|
| 0 | Becken (Wurzel) |
| 1-4 | Hinterbein rechts (W, -Z): Oberschenkel, Unterschenkel, Fuß, Krallen |
| 5-8 | Hinterbein links (W, +Z); 6 und 7 nutzen die Texel von 2 und 3 |
| 9 | Lende |
| 10 | Brust (2788 lang, 13 188 Texel, Seiten 0 und 2) |
| 11-14 | Vorderbein links (W): Oberarm, Unterarm, Fuß, Krallen |
| 15-19 | Vorderbein rechts (W): Schulter, Oberarm mit Auge, Unterarm, Fuß (Texel von 13), Krallen |
| 20-22 / 23-25 | kleine Arme rechts/links an der Brust (Hand rot) |
| 26 | Kopf (hängt an 10, 1181 lang, kein eigener Kiefer, Maul an der Unterseite) |

**Atlasfelder** (UV-Inseln gruppiert; `felder.py`, `g4_felder.json`; Bild `V3A_1_g4_atlas_felder_x3.png`, Feldgrenzen cyan).
Farbklassen:
- dunkel = L\* < 20 ohne Rot
- rot = a\* > 15, C\* > 20, Farbton -40..70 Grad (V1-A3)
- hell = L\* > 60, C\* < 35
- Feuer = L\* > 50, C\* > 30, Farbton 20-85 Grad

| Feld | Körperteil (Inseln) | Atlas x0,y0-x1,y1 | Texel | Lab Mittel (sd L\*) | C\* | dunkel / rot / hell / Feuer % | Rot-Farbton | Farbcharakter |
|---|---|---|---|---|---|---|---|---|
| F1 | Kopf: Schnauze, Flanke mit Zahnreihe, Auge (26/0, 26/102) | 0,1-79,35 | 1814 | (30,3, 9,1, 6,4) (19,6) | 13,1 | 39 / 22 / 7,4 / 1,2 | 34 Grad | oben beige-rosa Knochenschnauze, Flanke schwarz mit hellen Zahnspitzen, kleines rotes Auge |
| F2 | Kopf: Maulinneres (26/5) | 91,139-104,183 | 517 | (35,8, 42,3, 20,4) (10,9) | 47,6 | 0 / 99,6 / 0,8 / 3,9 | 25 Grad | gesättigtes Rot mit hellen Flecken |
| F3 | Brust: Rücken, Knochenrippen (10/200) | 8,44-72,90 | 2225 | (24,2, 7,6, 4,3) (14,5) | 10,3 | 48 / 17 / 0,8 / 0,4 | 34 Grad | beige Rippen mit dunklen Zwischenräumen, eine rote Ader |
| F4 | Brust: Flanke links (W) (10/41) | 6,100-74,138 | 1925 | (21,1, 10,7, 4,7) (16,4) | 13,2 | 56 / 31 / 1,3 / 2,4 | 32 Grad | schwarze Knoten, rote Adernetze, rosa Rippenränder |
| F5 | Brust: Flanke rechts (W) (10/90) | 3,147-73,186 | 2168 | (18,0, 7,0, 2,1) (13,9) | 9,6 | 69 / 19 / 0,8 / 3,0 | 33 Grad | wie F4, dunkler |
| F6 | Bauch (10/223) | 1,196-35,252 | 1422 | (18,8, 8,7, 3,2) (15,0) | 11,3 | 65 / 23 / 1,0 / 3,4 | 34 Grad | schwarz mit roten Adern, rosa Rand |
| F7 | Brust vorn: strahlenförmiges Fleisch um den Kopfansatz (10/84) | 65,177-126,254 | 2991 | (39,9, 46,5, 26,9) (12,5) | 54,6 | 0 / 99 / 1,6 / 5,2 | 33 Grad | offene Wunde, rote Strahlen mit rosa Rippen, 175 Farben |
| F8 | Brust vorn: Stachelkranz (10/229, 207 Flächen) | 44,199-58,251 | 561 | (56,4, 17,5, 21,7) (22,7) | 28,7 | 0 / 56 / 23 / **33,2** | 44 Grad | Verlauf dunkelrot -> orange -> hellgelb-weiß, flammenförmig |
| F9 | Brust vorn oben + Kleinteil (10/130, 10/36) | 2,95-122,125 | 550 | (32,1, 13,6, 10,1) | 17,9 | 19 / 44 / 2,4 / 1,6 | 34 Grad | rotbraun |
| F10 | Lende (9) | 81,38-126,99 | 1738 | (5,0, 1,7, -2,7) (3,5) | 3,7 | 99 / 0,9 / 0 / 0 | - | fast schwarz, 21 Farben |
| F11 | Becken (0) | 256,0-304,153 | 3711 | (8,5, 0,8, -0,8) (6,0) | 1,9 | 94 / 0 / 0 / 0 | - | schwarzgrau, Muskelwülste |
| F12 | Hinterbein Oberschenkel (1; 5 ohne Auge) | 308,0-382,54 | 2610 | (15,8, 6,3, 2,4) (11,0) | 7,3 | 72 / 14 / 0,2 / 0,1 | 23 Grad | schwarz mit Adernbaum, eine rosa Wundöffnung (5/13) |
| F13 | Hinterbein Unterschenkel (2 = 6) | 296,59-330,114 | 1298 | (14,4, 2,2, 0,6) (6,6) | 3,1 | 79 / 0,9 / 0 / 0 | - | schwarzgrau |
| F14 | Hinterbein Fuß (3 = 7) | 298,58-382,155 | 2589 | (20,7, 9,0, 3,7) (11,9) | 10,6 | 57 / 26 / 0,2 / 0,5 | 23 Grad | schwarz, Zehen rosa-rot gestreift |
| F15 | Hinterkrallen (4, 8) | 230,201-252,253 | 783 | (45,8, 14,2, 14,7) (23,3) | 22,6 | 9 / 30 / 18 / 9,3 | 27 Grad | Wurzel rot, Spitze gelblich-weiß |
| F16 | großes Auge (5/0 Hüfte, 10/47+58 Schulter, 16/132 Oberarm) | 274,159-342,243 | 2322 | (37,3, 42,9, 30,7) (13,2) | 53,4 | 3 / 94 / 1,3 / 16,5 | 34 Grad | rot-orange Strahlen, gelb-olive Iris, schwarze Pupille |
| F17 | Vorderbein Oberarm (11 = 15) | 128,168-160,253 | 2192 | (13,8, 5,3, 0,7) (7,3) | 7,1 | 78 / 12 / 0 / 0,2 | 29 Grad | schwarz, Längsader |
| F18 | Vorderbein Oberarm rechts (16 ohne Auge) | 130,1-251,84 | 5195 | (13,9, 9,3, 3,2) (9,0) | 11,6 | 73 / 25 / 0,1 / 0,4 | 28 Grad | schwarz mit kräftigen roten Längsadern, rosa Wundoval |
| F19 | Vorderbein Unterarm (12, 17) | 128,90-211,164 | 4831 | (13,5, 4,7, 0,3) (5,4) | 6,6 | 83 / 8 / 0 / 0,1 | 28 Grad | schwarz, verzweigte Längsadern |
| F20 | Vorderfuß (13 = 18) | 177,214-214,253 | 1216 | (26,1, 18,4, 7,5) (14,5) | 21,1 | 34 / 61 / 1 / 4,5 | 24 Grad | rot-rosa Zehen, dunkle Kerben |
| F21 | Vorderkrallen (14, 19, 13/1) | 217,196-252,253 | 902 | (48,0, 14,4, 14,5) (24,7) | 22,6 | 7 / 33 / 21 / 10,0 | 26 Grad | wie F15 |
| F22-F24 | kleine Arme (20/23, 21/24), Hand (22/25) | 217,89-253,237 | 1262 / 1469 / 896 | (17,1, 4,6, -1,1) / (26,5, 11,5, 1,7) / (38,0, 24,4, 14,0) | 6-30 | - | 17-23 Grad | grau, graurot, rote Hand |

**Summe G4** (genutzte Texel; `stufen3.json` "G4 gesamt"):
- 45 898 Texel, Lab (20,9 / 12,2 / 6,0), C\* 15,3, Rotanteil 28,6 %, Rot-Farbton im Median 30,6 Grad.
- Fleisch-Nachbarpaare nach dL: = 0 17,2 % / 0-5 34,9 % / 5-10 25,6 % / >= 10 22,4 %; 420 Farben im Fleisch.
- Das deckt sich mit `FARBE_PRUEFUNG_v2_r2_sicht.md` Zeile 81 (17,1 / 34,2 / 25,6 / 23,0; 420).

**Lesart:**
- Der G4-Körper ist **schwarz (L\* 5-15), das Rot sitzt in Adern.** Damit trifft G4 "schwarz-rot verbrannt" (G1) an Rumpf und Gliedern.
- Die Wundflächen F7, F16, F2 und F20 sind gesättigt rot (C\* 47-55) mit Farbton 23-34 Grad, also rot-orange, nicht magenta.
- Knochen, Zähne und Krallen sind hell gelblich: F1 oben, F3-Rippen, F8, F15, F21. Kopf (F1) und Rückenrippen (F3) sind deshalb hell und passen nicht zu G1.

## 2. K5c: Herkunft und Entsprechung zu G4

**Modell:** `varianten/K5/K5c/FORM.EMD`, sha256 a71f8d08e8d45b2f... (nachgemessen). 32 Knochen. Atlas 512 x 256 mit 4 Seiten und 4 CLUT-Zeilen;
Seite 3 trägt Kopf, Kiefer und Krallen mit CLUT 3.

**Herkunft** (K5.md Abschnitt 1, STUFE3.md Abschnitt 2): J_dicker + K5-Kopf + Krallen wie K4. J ist der Ringbau aus `rohbau_final` (DOSSIER Abschnitt 3:
"Ring-Bauweise, keine Metaballs ... Atlas: je Teil ein benannter Streifen, U Umfang, V Länge"). Das Gerippe stammt aus einer eigenen Bildpassung (`gerippe/`), nicht aus G4.

**Belege gegen eine Ableitung aus G4:**
- Knochenzahl 32 gegen 27.
- Versätze verschieden: K5c Bein 8 bei (-500,382,558), G4 Hinterbein 5 bei (-484,142,515).
- K5c-Schwanz 1-6 hat kein G4-Gegenstück.
- Eckpunkt- und Flächenzahlen je Mesh verschieden, z. B. Oberschenkel: K5c 9 ist ein Ring-Mesh mit 240 Flächen, G4 1 hat 36 Dreiecke + 13 Vierecke.
- Aus G4 stammen nur **Clip-Daten** (STUFE3.md Abschnitt 3: Bildzahl, EDD-Bits, Spur), keine Geometrie.

**Die Entsprechung ist trotzdem gut:**
- Beide Gerippe haben eine Ruhepose ohne Bindedrehung (EMD). Die lokalen Achsen sind gleich: x vorn, y unten, z links (W).
- Rechts (W) ist in beiden Modellen -Z. K5c: K5.md "+Z = links (W)". G4: Teile 1-4 bei z -738..-917, Teile 15-19 bei z -745..-1355.

| Rolle | G4 | K5c |
|---|---|---|
| Rumpfkette | Becken 0 / Lende 9 / Brust 10 | Hüfte 0 / Wirbel 7 / Brust 10 |
| Hinterbein rechts (W) | 1-2-3-4 | 9-26-27 + Zehen |
| Hinterbein links (W) | 5-6-7-8 | 8-24-25 + Zehen |
| Vorderbein rechts (W) | 15+16-17-18-19 | 16-17-18 + Klauen |
| Vorderbein links (W) | 11-12-13-14 | 11-12-13 + Klauen |
| Kopf | 26 | 22 + 23 |

**Gemeinsam genutzte K5c-Felder** (wichtig für jede Übertragung in die vorhandene UV; `k5c_inseln.json`):
- Feld 'kralle' (414-479, 181-217, Seite 3): 8 Klauen-Meshes und Teile von 13/18.
- Zehenfeld (294-319, 222-231): 25 und 27.
- Wer diese Teile unterschiedlich färben will, muss neue Felder anlegen. Das ist erlaubt, die UV darf sich ändern.

## 3. Teile-Zuordnung und Verfahren

**Grundlage der Abbildung** (`uebertrag.py`):
- Je Teil ein Rahmen im lokalen Mesh-Raum. Die Längsachse ist die Hauptachse der Eckpunkte, distal positiv.
- Winkelbezug ist "oben" (-y). Bei senkrechten Teilen (|a.y| > 0,7) gilt "vorn" (+x). Quelle und Ziel nutzen dieselbe Regel.
- Jeder Oberflächenpunkt bekommt einen Winkel theta um die Achse und eine Lage T = 0..1 entlang der Kette. Der Mittelpunkt wird je T-Abschnitt bestimmt, damit gebogene Teile stimmen.
- Kappen der Quelle (|N.a| > 0,8) werden nicht verwendet.
- Ketten (Rumpf, Schwanz) haben ein durchgehendes T. Das Muster wiederholt sich also **nicht je Glied** (E9 "aus einem Guss").

**Verfahren:**
- (a) UV-Übernahme per Eckpunkt: **nirgends möglich** (Abschnitt 2).
- (b1) Neuabtastung: Jedes K5c-Texel bekommt in der vorhandenen K5c-UV die Farbe des nächsten G4-Quelltexels in (theta, T).
- (b2) **UV-Projektion (empfohlen):**
  - Der G4-Atlas wird mit seinen 3 CLUTs bytegleich in freie K5c-Seiten kopiert.
  - Jede K5c-Fläche bekommt die G4-UV der Gegenpunkte ihrer Ecken. Alle drei Ecken liegen in **einer** G4-Insel, nämlich der Insel am Flächenschwerpunkt.
  - Geometrie, Normalen, Indizes, Knochen und Clips bleiben unverändert (Riegel "Geometrie-Diff 0"). Es ändern sich nur UV, TPAGE und CLUT.

| K5c-Teil (Mesh) | G4-Quelle (Mesh / Feld) | Begründung (Rolle, Form, Achse) | Verf. | Texel Ziel / Quelle | Dichte K5c / G4 / wirksam (sqrt Texel je Einheit²) | lin. Vergr. | (b2) Flächen mit UV-Kante > 20 Texel |
|---|---|---|---|---|---|---|---|
| Rumpf: Hüfte, Wirbel, Brust (0, 7, 10) | Becken, Lende, Brust (0, 9, 10) ohne F7, F8, F9, F16 | gleiche Kette ab der Wurzel, Längsachse x | b2 | 19 988 / 19 067 | 0,019 / 0,029 / 0,018 | 1,02 | 12,3 % |
| Schwanz (1-6) | Vorderbein links, Oberarm + Unterarm (11+12 = F17+F19) | kein G4-Schwanz; längstes kegeliges Glied ohne Auge, Adern längs (Kanten zur Längsachse im Median 3,7 Grad, MESSBLATT V2 Tab. 1) | b2, Kette | 9 050 / 5 569 | 0,022 / 0,040 / 0,017 | 1,27 | 3,2 % |
| Hals (21) | Brust 10, vorderes Drittel (T 0,70-1,0), ohne F7/F8/F9/F16 | Übergang Brust -> Kopf | b2 | 2 804 / ca. 4 085 | 0,016 / 0,016 / 0,019 | 0,83 | **65 %** (Offen 3) |
| Hinterbein links (W): 8 / 24 / 25 | 5 (ohne Auge F16) / 6 (= 2) / 7 (= 3) | gleiche Rolle und Seite | b2 | 1 632 / 2 316 / 2 096 | 0,023-0,059 / 0,031-0,040 | 0,67 / 1,34 / 0,90 | 2,1 / 0 / 2,8 % |
| Hinterbein rechts (W): 9 / 26 / 27 | 1 / 2 / 3 | gleiche Rolle und Seite | b2 (Probe 1) | 1 632 / 2 316 / 2 096 | 0,022-0,074 / 0,024-0,040 | 1,10 / 1,34 / 0,90 | 4,9 / 0 / 0 % |
| Zehen (an 25, 27) | Hinterkrallen 4/8 (F15) | Krallenrolle | b2 | - | - | - | - |
| Vorderbein links (W): 11 / 12 / 13 | 11 / 12 / 13 (F17 / F19 / F20) | gleiche Rolle und Seite | b2 | 3 224 / 2 576 / 3 195 | 0,023-0,035 / 0,031-0,042 | 1,27 / 0,85 / 1,58 | 12,5 / 2,5 / 14,3 % |
| Klauen links: 14, 15, 28, 30 | 14 (F21) | Krallen | b2 je Klaue | 2 391 / 669 | 0,041 / 0,040 / 0,022 | 1,89 | 30 % |
| Vorderbein rechts (W): 16 / 17 / 18 | 15+16 (ohne Auge) / 17 / 18 (= 13) | gleiche Rolle und Seite; 15+16 zusammen = Oberarm | b2 | 3 224 / 2 576 / 4 342 | 0,019-0,032 / 0,031-0,058 | 0,59 / 0,73 / 1,85 | **41 %** / 9 / 14 % |
| Klauen rechts: 19, 20, 29, 31 | 19 (F21) | Krallen | b2 je Klaue | 2 793 / 808 | 0,034 / 0,044 / 0,018 | 1,86 | 26 % |
| Kopf + Kiefer (22, 23) | Kopf 26 (F1 außen, F2 Maulinneres) | einziger G4-Kopf; Kiefer im Kopfrahmen (Versatz Knochen 23: (784,420,0)), Achse x | b2 (Probe 2) | 22 200 / 2 331 | 0,038 / 0,032 / **0,012** | **3,09** | 4,4 % |
| Augen (22, Insel 44), Zähne (Seite 3, 384,180,24,14) | Auge: F1-Auge oder F16; Zähne: F1-Spitzen, F8 oder F21 | eigene kleine Felder, nicht zylindrisch | eigenes Feld | 180 / - | - | - | - |
| nicht verwendet | kleine Arme 20-25 (F22-F24); Augen F16 auf der Haut | G4-eigene Merkmale: K5c hat keine Ärmchen, ein Schulter-/Hüftauge zeigt kein Bahnsteig-Bild | - | - | - | - | - |

**Erklärungen und Vergleichswerte:**
- Familienspanne der Texeldichte: RE2-Bosse 0,024-0,042 je Einheit (Mittel bzw. flächengewichtet; VARIANTEN.md Zeile 16/111). K5c heute: 0,042 / 0,030 (FARBE_LESER_A.md, T3).
- "Wirksam" heißt: die G4-Texel des Quellfelds, verteilt auf die K5c-Fläche. Genau diese Texel nutzt (b2). Alle Teile liegen bei 0,017-0,082, **der Kopf bei 0,012**.
- Zahlen: `voll_stat.json` (Dichten), `voll2_stat.json` (UV-Kanten).
- "UV-Kante > 20 Texel" bedeutet: Die Fläche spannt über eine G4-Inselnaht (zylindrischer Saum der Quelle) oder über ein ausgeschlossenes Loch.
  An diesen Stellen zeigt die Probe schwarze Dreiecke (`V3A_4_ganze_figur.png` Zeile 4, rechte Schulter und Hals).

## 4. Proben

Alle Blätter sind 1,5-fach vergrößert; alle Zeilen haben dasselbe Licht.

### 4.1 Probe 1: rechtes Hinterbein (W), 9/26/27 <- G4 1/2/3 (`V3A_2_probe_bein.png`)
Zeilen: G4-Quelle | K5c heute | (b1) | (b2). Spalten: Seite rechts (W), von hinten (az -90), 3/4 von hinten oben.
- **Gesehen:**
  - (b2) hat die G4-Machart: schwarze Haut, rote Adern als zusammenhängende Linien längs des Oberschenkels, rosa-rote Zehen.
  - (b1) hat dieselben Farben, zeigt aber am Oberschenkelende quer laufende schwarze Blockringe und zerrissene Adern (Nachbar-Abtastung über Inselgrenzen).
- **Gemessen:** Flächen mit UV-Kante > 20 Texel: 4,9 / 0 / 0 % (9/26/27). Vergrößerung 1,10 / 1,34 / 0,90.

### 4.2 Probe 2: Kopf + Kiefer, 22/23 <- G4 26 (`V3A_3_probe_kopf.png`)
Spalten: Seite rechts (W), 3/4 vorn oben, von unten.
- **Gesehen:**
  - Die Anordnung stimmt: helle Schnauze oben, dunkle Flanke mit Zahnreihe, kleines rotes Auge vorn, rotes Maul unten.
  - Der Kopf wird aber **hell beige-rosa** statt schwarz-rot.
  - Die G4-Texel stehen 3-fach vergrößert; unten sieht man grobe Bänder.
  - Augenhöcker und Zähne von K5c bekommen Schnauzenfarbe.
- **Gemessen:**
  - (b1), Atlas Kopfseite (x >= 384): Lab (35,2 / 16,3 / 9,8), Rotanteil 41,5 %, Fleisch mit dL = 0 73,0 %, 94 Farben im Fleisch (`stufen3.json`).
  - (b2): UV-Kante > 20 Texel bei 4,4 % der Flächen.
- **Kieferunterseite:**
  - In (b2) liegt die ganze Kiefer-Unterseite im G4-Maul (F2, rot), weil der G4 sein Maul unten hat.
  - In (b1) ist die Trennung nach Normalen schon eingebaut (`voll.py`): Kieferboden und Gaumen -> F2, Außenseite -> F1. In (b2) fehlt sie noch.

### 4.3 Ganze Figur, Machbarkeit (`V3A_4_ganze_figur.png`)
Zeilen: G4 | K5c heute | (b1) | (b2). Spalten: 3/4 vorn, Seite rechts (W), 3/4 hinten links (W), von vorn.
- **Gesehen:**
  - (b2) liest sich als dunkles, schwarz-rotes Tier mit roten Vorderfüßen und hellen Krallen, ohne Glanzflecken und ohne Magenta.
  - Kopf und Hals sind hell.
  - An der rechten Schulter und am Hals gibt es schwarze Löcher (Nahtflächen, Abschnitt 3).
- **Gemessen**, (b1), Körper (x < 384):
  - Lab (17,2 / 7,4 / 3,5), C\* 9,9, Rotanteil 19,2 %, Rot-Farbton im Median 29,5 Grad (G4: 30,6).
  - Aber: Fleisch mit dL = 0 bei **54,2 %** (G4 17,2 %, V2 52,9 %).
  - Ursache: Die Neuabtastung legt ein Quelltexel auf mehrere Zieltexel (Streckung in einer Richtung). Die harten Stufen bleiben, die Zwischenstufen fallen weg.
- (b2) hat dieses Problem im Atlas nicht: Die Texel sind bytegleich mit G4, die dL-Verteilung ist die des G4. Auf dem Schirm wirken die Texel dann so groß,
  wie die Spalte "Vergrößerung" in Abschnitt 3 angibt.

## 5. G2: Felder für "Gesicht vorn" und "Maul" (`feuer.py`, `feuer.json`)

**So sind die Ziele gemessen:**
- Der Ort wird am Video gewählt, nur Pixel in `maske_bild1/2.png`.
- Die Farbe stammt aus der Rückrechnung `form/farbe_bild1_echt.png` / `farbe_bild2_echt.png` (Filtermodell farbe.md).
- Texel = Schirmfarbe / Raumlicht (`farbe_palette.json`, licht): Bild 1 Figur (0,487, 0,432, 0,374), Bild 2 (0,385, 0,342, 0,307).

| Ziel | Auswahl | Video RGB | Schirm (rückgerechnet) RGB / Lab | **Texel-Ziel RGB / Lab** | Farbton / C\* | Schwerpunkt |
|---|---|---|---|---|---|---|
| Bild 1, Kopf vorn | Box x 300-345, y 225-265; die obersten 10 % nach Video-a\* (180 von 1800 px) | (142,66,123) | (65,35,39) / (17,9, 14,9, 3,1) | **(133,81,104) / (40,9, 25,2, -3,9)** | -9 Grad / 25,5 | (315,247) |
| Bild 2, Maul | Box x 255-345, y 310-420; die obersten 10 % nach Video-L\* (559 von 5592 px) | (208,123,126) | (90,71,58) / (31,9, 6,2, 10,8) | **(234,208,189) / (84,7, 6,1, 12,7)** | 64 Grad / 14,1 | (295,363) |

**Abstand der G4-Felder zu den Zielen** (Auszug; alle Felder in `feuer.json`). Angaben: dE76 zum Texel-Ziel als Minimum und als Anteil der Feldtexel mit dE < 10 bzw. < 20;
dazu der kleinste Abstand der Schirmfarbe des Feldes im jeweiligen Raumlicht.

| Feld | Bild 1: dE min / Anteil < 20 % | Bild 1: Schirm-dE min | Bild 2: dE min / < 10 % / < 20 % | Bild 2: Schirm-dE min | Feuer-Anteil, Feuer-Lab |
|---|---|---|---|---|---|
| F1 Kopf/Schnauze | 15,8 / 9,5 | 8,0 | **3,7 / 1,4 / 7,3** | 1,7 | 1,2 %, (53,0, 32,0, 17,9) |
| F2 Maulinneres | 15,8 / 3,9 | 8,0 | 26,6 / 0 / 0 | 11,0 | 3,9 % |
| F7 strahlenförmiges Fleisch | 16,5 / 1,2 | 8,1 | 9,1 / 0 / 0,1 | 4,3 | 5,2 %, (54,5, 43,3, 28,1) |
| F8 Stachelkranz | 18,0 / 2,1 | 8,9 | **3,7 / 6,6 / 18,0** | 1,6 | **33,2 %**, (65,6, 25,4, 30,0) |
| F16 großes Auge | 24,8 / 0 | 13,4 | 11,9 / 0 / 0,1 | 5,4 | 16,5 %, (55,4, 37,6, 28,3) |
| F15 / F21 Krallen | 13,9 / 9,2-9,6 | 6,8 | 3,9 / 0,5-0,7 / 11,7-15,0 | 1,5 | 9-10 % |
| F12 / F14 / F20 Bein, Fuß | **9,5** / 9-26 | **5,0** | 26,8-29,0 / 0 / 0 | 11-12 | 0,1-4,5 % |

**Lesart** (gemessen, nicht entschieden):
1. **Bild 1 verlangt Rosa-Magenta, kein Feuerorange.**
   - Das Ziel liegt bei Farbton -9 Grad, alle G4-Rottöne bei 23-44 Grad. Kein G4-Feld kommt unter dE 9,5.
   - Ein gesättigtes Feuer-Texel wie der Feuerteil von F8 (65,6 / 25,4 / 30,0) ergäbe im Bild-1-Licht im hellsten Zehntel etwa Schirm-Lab (44,9 / 2,9 / 13,6).
     Das ist gelblich-grau, nicht die gemessene Magenta-Stelle.
   - Sicherheit des Farbtons: "wahrscheinlich" (Kanalbalance Bild 1, farbe.md Zeile 51).
2. **Bild 2 verlangt ein helles, wenig buntes Creme** (Farbton 64 Grad, C\* 14).
   - Das passt zu G4-Knochen, -Zähnen und -Krallen: F1, F8, F15, F21 mit dE 3,7-3,9.
   - Feuerfarben im engen Sinn (C\* > 30) liegen weiter weg: F7 dE 9,1, F16 dE 11,9.
   - Sicherheit: "Obergrenze", weil der Feuerschein in der Rückrechnung nicht ganz abgezogen ist (farbe.md Zeile 152).
3. **Abgleich mit dem Nutzerwortlaut** ("Gesicht vorne ... in flammenfarben leuchtet"):
   - Feuercharakter haben im G4 zwei Felder: F8 (Verlauf dunkelrot - orange - hellgelb, wie eine Flamme) und F16 (rot-orange mit gelber Iris).
   - Fürs Maul (Bild 2, hell) sind F8 und F1 die nächsten Texel.
   - Für die Stelle in Bild 1 hat der G4 keine passende Farbe.
   - Ob dort G4-Feuer (Bild des Nutzers) oder Magenta (Messung) gilt, entscheidet der Messblatt-Prüfer (G2 gegen die Messung Bild 1).
4. **E24 "Augenstellen dunkel" gegen G2:**
   - Der Bild-1-Schwerpunkt (315,247) liegt 43 px von L1 (280,275) und 101 px von L2 (217,275) entfernt (Augen, K5.md). Die Leuchtstelle ist also nicht das Auge.
   - Ob sie im K5c-Raum auf Stirn, Schnauze oder Hinterkopf liegt, misst erst eine Rückprojektion mit der Kamera von Bild 1. Die gehört nicht zu dieser Lesung.

## 6. Empfehlung an den Bauer (Leser A)

1. **Verfahren (b2), UV-Projektion, für Rumpf, Glieder und Schwanz.**
   - Die G4-Seiten 0-2 samt CLUT 0-2 bytegleich in die K5c-TIM übernehmen. K5c hat 4 CLUT-Zeilen, G4 braucht 3; Zeile 3 bleibt frei für Kopf und Feuer.
   - Flächen-CLUT = G4-Seite.
   - Riegel: dL-Verteilung im Fleisch wie beim G4 (17 / 35 / 26 / 22 %) statt der 54 % dL = 0 aus (b1).
2. **Nahtflächen lösen** (UV-Kante > 20 Texel: Hals 65 %, rechter Oberarm 41 %, Krallen 26-30 %). Zwei Wege:
   - die Ecken in der Nachbarinsel suchen, die zur Fläche passt, oder
   - die Quellinsel je Fläche nach der kleinsten UV-Spannweite wählen.
   - Ohne das bleiben schwarze Dreiecke.
3. **Kopf nicht nur aus F1** (Vergrößerung 3,1, helle Schnauze). Vorschlag:
   - Schädeldach, Hinterkopf und Hals aus den schwarz-roten Brustflanken F4/F5 (Dichte passt besser).
   - Schnauzenflanke mit Zahnreihe aus dem dunklen Teil von F1.
   - Kieferboden und Gaumen aus F2 (rot); Kiefer außen dunkel (F1-Flanke).
   - Zähne aus den F1-Spitzen oder aus F21.
4. **Gesicht vorn und Maul (G2) als eigenes kleines Feld.**
   - Kandidaten: F8 (Feuerverlauf) und F16 (Rot-Orange mit gelber Iris). Für Bild 2 liegen messbar F8 und F1 am nächsten.
   - Den Farbton aus Bild 1 (-9 Grad) hat der G4 nicht; die Abwägung gehört ins Messblatt V3.
5. **Nicht übertragen:**
   - Augen F16 auf Hüfte, Schulter und Oberarm.
   - Kleine Arme F22-F24.
   - Rückenrippen F3: hell, sie lesen sich auf K5c als beige Flecken an der Schulter (`V3A_4` Zeile 3).
   - Strahlenförmiges Fleisch F7 nur, wenn vorn an der Brust eine Wunde gewünscht ist.

## 7. Offen

1. Rückprojektion der Bild-1-Leuchtstelle (315,247) auf K5c in Pose Bild 1: Welche Fläche ist es – Schnauze oben, Stirn oder Hinterkopf?
2. Kopfquelle und G2-Farbe: Bild des Nutzers ("Feuerfarbe") gegen Messung Bild 1 ("Rosa-Magenta"), Abschnitt 5. Entscheidet der Messblatt-Prüfer.
3. Halsquelle: Brust-T 0,70-1,0 erzeugt die meisten Nahtflächen (65 %). Alternative: nur die Flanken F4/F5.
4. Die (b2)-Probe trennt Kieferboden und Außenseite noch nicht. Die PSX-Grenzen je Fläche sind nicht vollständig geprüft: Die UV bleibt innerhalb einer 128er-Seite, die Spannweite reicht aber am Hals bis 54 Texel.
5. Render-Maße B10-B17 (`n2_riegel.py`) an einer gebauten Variante. Diese Lesung hat nur Atlasmaße und Sichtproben.
