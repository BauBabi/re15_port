# Runde 39 Spur G "karten" - Abnahme 1 (unabhaengig, 2026-10-18)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Kopf b1da8c01, Merge-Basis 22abaed9). Gegenstand: Nachbesserung 1
(Commits efa97816..b1da8c01) auf die Maengel M1..M5 der Abnahme 0. Der Pruefer glaubt nur, was er selbst am gebauten
Stand gemessen hat. Fortlaufend geschrieben (Sitzungslimit). Arbeitsordner der Messungen (nicht im Baum):
`<scratchpad>/G39abn1/` (eigene Skripte; die Werkzeuge der Abnahme 0 aus `<scratchpad>/G39abn0/` wurden gelesen,
ihre Dateizeiten 18:54..19:48 liegen VOR dem Beginn der Nachbesserung (~20:00) - vom Bau unberuehrt; die
VORHER-exe der Abnahme 0 (Merge-Basis 22abaed9, `G39abn0/vorher_build`) wird wiederverwendet).

## 0. Stand / Bau / gezielte Tests
- `git log master..HEAD`: 26 Commits, davon 10 der Nachbesserung 1 (efa97816, ad1f5cd7, 16fac481, 115d054c, fb5d474a,
  98cca236, b45d8061, 9ae182da, 43dd21ee, b1da8c01). Arbeitsbaum sauber.
- `git diff master...HEAD --name-only` (ohne analysis/): engine/src/karte_r39.c, engine/src/map_hint_common.c,
  engine/src/re15_inv_screen.c, engine/src/re15_map_zones.h, include/re15_karte_r39.h, Tests (test_r39_karten.c/.cmake,
  probes/r39_karten.cmake, r39_karte_b1.h, r39_karten_soll.inc, angepasst test_map_synth.c, test_r35_cut10f0.c,
  test_r35_karte.c/.cmake, test_r36_karte.c/.cmake, test_r36_kartenitems.c, test_r38_tagebuch.c). Seit der Merge-Basis
  hat master nur test_r38_itemorte.c geaendert (Spur H) - keine Ueberschneidung.
- Bau: `bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build` ->
  `=== LOCAL-BUILD-OK (build)`, "ninja: no work to do" (exe 20:54:34; der letzte Commit b1da8c01 aenderte in
  karte_r39.c nur einen Kommentar).

- Gezielter Lauf: s. Abschnitt 7 (96/96 gruen).

## 1. Punkt 1a - Blaetter 1F/2F/3F/B2 im Vollzustand gegen die Soll-Haelften der Vorlagen (eigene Messung)
Werkzeuge (Abnahme-0-Skripte, unveraendert kopiert nach G39abn1/: reg.py, vgl2.py, struktur.py; lauf.sh mit
exe-Kopie `re15_pc_g39abn1_<id>.exe`): `lauf.sh <NEU-exe> neu_S 1190 300 1190 3174 0 -11182 0 -- RE15_MAP_SHOT_SWEEP=s`
(Spielstand per probe_r36_karte karte, LOAD GAME, RE15_WINDOW_SCALE=1, alle 13 Blaetter) und am Nutzerpunkt ROOM1090
`lauf.sh <NEU-exe> neu_M 1090 300 1090 -12528 -9000 959 0 besucht:1090:-12528:959:5 -- RE15_MAP_SHOT_SWEEP=s`.
Vergleich gegen die RECHTE Haelfte (Blockmitten in der 3x-Phase der Abnahme 0), Klassen Grund/Koerper/Wand/Tuer/Treppe:

| Blatt | Kartenpixel | Klassen-Abweichungen | Ort |
|---|---|---|---|
| B2 (s01, neu_S) | 56 207 | **0** | - |
| 1F (s02, neu_S) | 56 234 | **0** | - |
| 2F (s03, neu_M = Nutzerpunkt) | 56 201 | **0** | - |
| 3F (s04, neu_S) | 56 216 | **54** | zwei Kaesten (vgl3f.py): (154,96)-(161,101) 30 px = Treppe 1170 Zone 1 jetzt (156,98) statt Vorlage (159,99); (138,145)-(146,149) 24 px = Treppe 1060 jetzt (140,147) statt Vorlage (144,147) |

- 3F-Treppe (168,106) (Mangel M1): KEINE Abweichung mehr gegen die Blockmitten der Vorlage (die Vorlage ergibt in meiner
  Abtastung y104..108 = Mitte y106). Strukturpruefung: nur noch (170,104) beruehrt die Wandecke (170,103) - wie in der
  Vorlage selbst (Abnahme 0: 3F 1 Treppenpixel an Wand VORHER = Vorlagenstand).
- Strukturpruefung (struktur.py, Kartenfeld x40..300 y40..210), NEU-exe:

| Blatt | Doppelwand-Pixel (VORHER-exe alt_S -> NEU) | Tueren nicht auf Wand | Treppenpixel an Wand |
|---|---|---|---|
| B1 (0) | 52 -> **0** | 1 -> 0 (neu_M); in neu_S 1 = der Spieler-Ring (Start 1190 Halle) liegt ueber dem Tuerbalken (126..128,115), Enden/Seiten = Ring-Pixel | 0 -> **0** (M2: Sprosse beruehrt die Unterkante nicht mehr) |
| B2 (1) | 40 -> **0** | 2 -> **0** | 6 (Sprossenenden x170/x186 wie Vorlage) |
| 1F (2) | 223 -> **0** | 8 -> **0** | 12 (Enden x119/x136/x180/x197 wie Vorlage) |
| 2F (3) | 0 -> **0** | 1 -> **0** | 6 (Enden x118/x135 wie Vorlage) |
| 3F (4) | 5 -> **0** | 2 -> **0** | 1 ((170,104) an der Ecke (170,103), wie Vorlage) |

- Die zwei 3F-Abweichungen sind BEWUSST (Dossier "Fuer den Nutzer", Riegel k_abw in test_r39_karten.c): die neuen Zeilen
  aus den Waenden (Abschnitt 3) projizieren die STAIR-AOTs dorthin; die Vorlage liess diese Symbole an der alten
  Bbox-Projektion stehen. Bewertung in Abschnitt 6.

## 2. Mangel M3 (Regression) - Teilzustaende an der echten exe (eigene Messung)
Lauf je Ort ALLEIN besucht und aktuell (G39abn1/allein_nb.sh -> zustand.sh = lauf.sh mit `besucht:<raum>:<x>:<z>`,
RE15_INV_FB_SHOT=karte.bmp, RE15_INV_FB_SHOT_AT=60), dieselben 15 Orte und Koordinaten wie Abnahme 0 Abschnitt 6.
Auswertung allein_vgl.py: NEU gegen die VORHER-Abzuege der Abnahme 0 (`G39abn0/al_<ort>_alt`, Merge-Basis-exe) -
(a) Wandpixel VORHER, die NEU weder Wand noch Tuer sind; (b) eigene Zaehlung "offen" = Raumkoerper (rot/gruen)
4-nachbarlich am Grund, ohne Referenzbild; (c) Klassen-Differenz des ganzen Kartenfelds.

| Ort | (a) verlorene Wandpixel (Abnahme 0 an 40328436) | (b) offen VORHER -> NEU | (c) Klassen-Diff zu VORHER |
|---|---|---|---|
| 1050 | 0 (8) | 0 -> 0 | 0 |
| 1020 | 0 (16) | 0 -> 0 | 0 |
| 1040 | 0 (17) | 0 -> 0 | 0 |
| 1070 | 0 (34) | 0 -> 0 | 0 |
| 1030 | 0 (0) | 2 -> 0 | 1 = (185,145) Loch gefuellt (Vorlage, "immer") |
| 1010 Zone 0 / Zone 1 | 0 / 0 (30 / 30) | 0 -> 0 / 0 -> 0 | 0 / 0 |
| 1080 | 0 (8) | 0 -> 0 | 0 |
| 1000 Ostraum / Nord-WC / Sued-WC | 0 / 0 / 0 (41 / 26 / 12) | 0 -> 0 je | 0 je |
| 1110 | 0 (0) | 4 -> 4 (vorbestehend, (185,115), Schwenk-Tilgung) | 0 |
| 1210 | 0 (0) | 2 -> 4 | 10 = Tuerbalken 11E0<->1210 x187 -> x186 (Vorlage); Ecken (186..187,71)/(186..187,77) Grund |
| 1220 Zone 0 / Zone 2 | 0 / 0 (16 / 16) | 0 -> 0 je | 0 je |

=> Allein besucht ist jeder korrigierte Ort jetzt klassengleich mit dem Stand VOR Runde 39 (ausser den gewollten
Aenderungen 1030-Loch und 1210-Tuer). Die Regression M3 ist an der echten exe behoben.
- 1210 allein (Ausschnitt x180..196, y66..82, W Wand, D Tuer, R aktuell, . Grund):
  ```
  VORHER          NEU
  y70 .......WWW  y70 .......WWW
  y71 ........RR  y71 ........RR
  y72 .......DRR  y72 ......DRRR
  y76 .......DoR  y76 ......DRoR
  y77 ........Ro  y77 ........Ro
  ```
  Die Eckluecken (187,71)/(187,77) bestanden vorher; jetzt steht der Balken eine Spalte weiter links (Vorlage), dadurch
  zwei zusaetzliche Grund-Nachbarn. ROOM1210 ist nur durch ROOM11E0 erreichbar (dessen Ostwand x186 ist dann gezeichnet) -
  im Spiel nicht sichtbar; Hinweis, kein Mangel.

- WEITERE Teilzustaende (zustand.sh/paare.sh, NEU-exe; paar_ausw.py = struktur.py-Doppelwand + "offen"):

| Zustand (zuletzt genannter Ort aktuell) | Doppelwand | offen | Bemerkung |
|---|---|---|---|
| 1020 allein + 1F-Plan (flag:9:89, Umriss aller Nachbarn) | 0 | 0 | Korrekturen greifen, Raum geschlossen |
| 1030 + 1050 | 0 | 0 | Korrektur r0 x187 mit Traeger 1030 |
| 1000 Sued-WC + Nord-WC | 0 | 0 | Kante y138 entfaellt, y139 steht |
| 11E0 + 1220 Zone 0 | 0 | 0 | Korrektur B2 r4 x187 mit Traeger 11E0 |
| 1210 allein + B2-Plan (flag:9:77) | 0 | 4 | die bekannten Ecken (187,71)/(187,77), s.o. |
| 1050 + 1070 | 0 | 0 | |
| 1020 + 1010 Zone 0 | 0 | 0 | |
| 1050 + 1090 Hof + 1000 Ostraum | 0 | 0 | |
| 1000 Ostraum + 1F-Plan | 0 | 0 | |

  "Tueren nicht auf Wand" in diesen Zustaenden: nur der Spieler-Ring ueber einem Balken und - VORBESTEHEND - die Tuer
  1030<->1050 `{2,4,186,152,3,4,6,1}` (re15_map_zones.h, von Runde 39 nicht geaendert): ist 1050 besucht, 1030 aber
  nicht gezeichnet, steht der Balken (186,150..154) eine Spalte links neben der 1050-Westwand x187 im Leeren. VORHER-exe
  (al_1050_alt) und NEU-exe (zs_al_1050) sind dort pixelgleich - keine Folge der Nachbesserung (Hinweis H1).
- Code gelesen: karte_r39.c `traeger_gezeichnet`/`kachel_gezeichnet`/`schema_gezeichnet` spiegeln die Zeichner-Gatter
  von re15_inv_screen.c build_status; ohne Schirmzustand (Werkzeug) gilt jede Korrektur. RE-Zitate selbst
  disassembliert (Abschnitt 5).

## 3. Maengel M4/M5 - Marker auf Treppen und in der Zeichnung (eigene Messung)
### 3a. Sonde = exe (Voraussetzung fuer die Flaechenzahlen)
Ring der echten exe (ring.py der Abnahme 0, 5x5-Ring einer Blinkfarbe) gegen `probe_r39_karten marker` an DERSELBEN Lage
(spot.sh: Spielstand an der Stelle, LOAD GAME, Kartenschirm FB_SHOT). Wo LOAD GAME den Spieler versetzt (Stand direkt
auf einem STAIR-AOT ist fuer die Kollision nicht begehbar - debug.log `[walk] F0 ... pl pos=`), wurde die Sonde an der
TATSAECHLICHEN Lage nachgefragt:

| Lauf | Lage (tatsaechlich) | exe-Ring | Sonde | Blatt |
|---|---|---|---|---|
| t1060_1F_a | 1060 (25150,0,22000) | (122,143) | (123,144) | 1F |
| t1060_1F_b | 1060 (19132,0,22850) versetzt | (137,145) | (138,146) | 1F |
| t1060_2F_a | 1060 (27568,-7200,22000) versetzt | (121,143) | (122,144) | 2F |
| t1060_2F_b | 1060 (19132,-7200,22850) versetzt | (136,145) | (137,146) | 2F |
| t1060_3F | 1060 (27568,-14400,22000) versetzt | (139,146) | (140,147) | 3F |
| t10A0_1F_a | 10A0 (24168,-14400,21650) versetzt | (190,98) | (191,99) | 1F |
| t10A0_1F_b | 10A0 (27668,-10800,22350) versetzt | (198,97) | (199,98) | 1F |
| t10A0_B2 | 10A0 (27668,-1800,22350) versetzt | (186,109) | (187,110) | B2 |
| t10A0_B1_a | 10A0 (21450,-7200,22068) versetzt | (144,161) | (145,162) | B1 |
| t10A0_B1_b | 10A0 (27668,-7200,22350) versetzt | (157,161) | (158,162) | B1 |
| m1030_west | 1030 (-25000,0,-4500) | (123,131) | (124,132) Index 1 | 1F |
| m1110_gang | 1110 (-2650,0,-5500) | (183,117) | (184,118) Index 1 | 2F |
| m1210_sw | 1210 (-24500,0,-21500) | (194,116) | (195,117) **Index 0** | B2 |

=> In allen 13 Laeufen ist exe-Ring = Sonde - (1,1) (Ringsprite). Die Sonde rechnet den Kartenpfad der exe
(re15_inv_map_marker samt Klemmung) exakt nach; ihre Flaechenzahlen gelten fuer die exe.

### 3b. Treppen (M4): Marker auf der Treppe gegen das Treppensymbol
- Echte exe, Leon auf dem 1060-Lauf (25150,0,22000), Blatt 1F: Ring-Mitte **(122,143)**, Vorlagen-Symbol (122,144)
  (Sprossen x120..124) - vorher (Abnahme 0) Ring (126,143) daneben.
- Projektion der STAIR-AOT-Mitten (Aot_set 0x2C sce 12/13; Bytes selbst gelesen, z.B. ROOM1060.RDT @0x00D72
  `2c 03 0d 31 08 00 f8 5c 58 4d 8c 0a 30 11` = Band 8, (23800,19800) 2700x4400, ROOM10A0.RDT @0x00E90 Band 4,
  ROOM1170.RDT @0x013B6 Band 4) mit den neuen Zeilen, von Hand nachgerechnet (Formel s. Abschnitt 5):
  1060 1F (25150,22000) -> (123,144) / (21400,22850) -> (132,146) gegen Symbole (122,144)/(133,146); 2F (122,144)/(131,146)
  gegen (121,144)/(132,146); 3F (140,147) gegen (140,147); 10A0 1F (21450,21650) -> (184,99) gegen (183,100); B2
  (25350,22350) -> (182,110) gegen (183,112); B1 (21450,20650) -> (145,165) gegen (145,164). Alle Abstaende <= 2 px
  (Chebyshev), vorher 2..5 px. Riegel unit_r39_karten_treppen gruen (Abschnitt 7).

### 3c. Marker neben der Zeichnung (M5) - Sonde markerflaeche an JEDEM begehbaren Rasterpunkt (Dichte 1)
`probe_r39_karten markerflaeche <raum> <band> <x> <z> <y> 100 1` (echter Kollisionsloeser, Spielerradius 450):

| Raum / Band | begehbare Punkte | neben der Zeichnung (Abnahme 0, jeder 3. Punkt) |
|---|---|---|
| 1030 | 20 967 | **0** (63/2288) |
| 1060 Band 0 / 4 / 8 | 1 386 / 890 / 890 | **0 / 0 / 0** (4/155, 4/104, 4/104) |
| 10A0 Band 1 (B2) / 4 (B1) / 8 (1F) | 1 207 / 1 220 / 828 | **0 / 0 / 0** (8/140) |
| 1110 drei Teile (+1111 Elza) | 11 438 + 1 380 + 1 656 (11 438) | **0** (8/1237) |
| 1170 Zone 1 Band 0 (3F) / Band 4 (Dach) | 2 873 / 1 656 | **0 / 0** (51/311; Dach 12) |
| 10C0 / 1120 / 10E0 | 9 335 / 10 094 / 10 474 | 0 / 0 / 0 |
| 1090 Band 5 / Band 1 / 1091 Band 5 | 11 059 / 5 178 / 11 059 | 0 / 0 / 0 |
| 1210 | 10 387 | **1 693** (16 %; SW-Kammer, offene Nutzerfrage) |

Ueber ALLE Raeume der Stage 1 (alle_raeume.py des Baus = nur Saatwahl aus den Tuer-Ankuenften; gezaehlt mit der
geprueften Sonde, jeder 3. Punkt): neben der Zeichnung nur noch 1210 207/1141 (Polizeistation) sowie 10B0 508/508,
1260 266/266 (ohne Kartenblatt, nicht Polizeistation) und 11A0 Band 1 102/444 (Blatt 6, nicht Polizeistation, von
Runde 39 nicht beruehrt - vorbestehend). Alle anderen 37 Raum/Band-Saaten: 0.

### 3d. Tuerdurchgaenge (Gesamtschiene integration_map_uebergang, RE15_UEBERGANG_LISTE=1, NEU)
`[Sprung] 151 Uebergaenge auf gleichem Blatt: Median 6 px, Mittel 17 px, schlimmster 91 px, ueber 8 px: 62` - dieselben
Kennzahlen wie nach dem ersten Bau (Dossier B1-N: 6/17/91, 62). Auszug Polizeistation: 1060 -> 10C0 5 px, 1060 -> 1120
6 px, 10A0 -> 11E0 5 px, 10A0 -> 1180 1 px, 1130 -> 1170 (Zone 1) 4 px, 1030 <-> 1040 5 px, 1070 -> 1030 9 px.
Der 1130 -> 1170-Sprung (166,89) -> (167,93) kommt aus der vorbestehenden Klemmung (re15_inv_screen.c:869
`reserve = 4` ins NOMINALE Rechteck): die Ankunft (-21225,-18870) projiziert auf y91, die Klemmung hebt auf y93
(Hinweis H2). Gegenmessung: dieselbe Schiene aus den Quellen der Merge-Basis 22abaed9 gebaut (git archive 22abaed9 ohne
shared_assets, eigener Bauordner G39abn1/vorher_tests, nur das Ziel test_map_uebergang, RDTs aus dem Baum kopiert) ->
`[Sprung] 149 Uebergaenge ... Median 6, Mittel 17, schlimmster 91, ueber 8 px: 64`. Je Tuer verglichen (ueb_vgl.py),
Blaetter 0..4 (Stage 1): VORHER n=56 Median 4 Mittel 4,9 max 20 >8 px: 3 -> NEU n=58 Median 4 Mittel 3,9 max 9 >8 px: 1.
Besser: 10A0->1180 8->1, 1180<->11B0 20/18->0/2, 1180<->1190 6/7->2/1, 11C0->11B0 6->2, 1090->1050 5->3, 1030<->1050
4->3. SCHLECHTER (+1..3 px): 1060->10C0 2->5, 1060->1120 3->6, 10C0<->10D0 5->6, 10D0<->10E0 2->5 - jeweils an einer
West-/Nordtuer eines Raums mit Wandzeile: die Formel legt den Marker auf die Wand, die vorbestehende Klemmung
`reserve = 4` (re15_inv_screen.c:869, ins NOMINALE Rechteck) schiebt ihn 4 px nach innen (Hinweis H2).

## 4. Punkt 2 - ROOM1090 Marker im aktuellen Raum (eigene Messung, Nachbesserung hat 1090 nicht beruehrt)
- Nutzerpunkt (R1090 pos=(-12528,-9000,959), nutzer/befund_marken_2026-10-17.txt), Sweep neu_M: Ring-Mitte **(183,72)**,
  ringsum ROT (Querband des Gammas y68..75); VORHER-exe (Abnahme 0, alt_M) Ring (186,83) im GRUENEN Nachbarraum.
- Sichtpruefung am ECHTEN Fenster (Skill re15-port-visual-verify): gdi.sh = Spielstand am Nutzerpunkt, LOAD GAME, Karte
  offen, `ffmpeg -f gdigrab -i hwnd=<Fenster meiner exe-Kopie>` (Direct3D-Renderer, kein SOFTWARE_RENDER, kein AUTOSHOT).
  gdigrab-Bild gegen den FB-Abzug derselben Sitzung: **320x240 deckungsgleich bis auf 13 Pixel** im Ring (181..185,70..74)
  = Blinkphase; Ring-Mitte in beiden (183,72). Das FB-Abzugsverfahren dieser Abnahme zeigt also, was der Nutzer sieht.
  Ausschnitt: `G39abn1/gdi_1090/gdi_ausschnitt_x4.png` (Gamma rot, Ring im Querband).
- ECHTER WEG Leon (Muster test_r36_klettern.cmake Lauf A: RE15_DEBUG_JUMP=10f0@250, (9,71)=1, Tritt schieben, klettern,
  Tuer von oben; Kartenschirm 40 Bilder nach Ankunft): `[room] PC loaded room1090.rdt`, climb.txt T1270 `room=1090
  pos=(-13600,-9000,1300) band=5`, Abzug Blatt 2F, Ring **(182,72)** im Rot (`G39abn1/echt_1090_leon/karte_x2.png`).
- ECHTER WEG Elza (Lauf B: RE15_TITLE_SHOT_CHAR=1 RE15_GOTO_ROOM=10F1, Weg durch den Raum, Tritt, Tuer):
  `[pl] Spieler-Familie PL04`, `PC loaded room1091.rdt`, T1251 `room=1091 pos=(-13600,-9000,1300) band=5`, Blatt 2F, Ring
  **(182,72)** im Rot (`echt_1091_elza3`). (Ein erster Elza-Abzug zeigte 3F ohne Marker: Messfehler meines Laufs - mit
  RE15_INPUT_SCRIPT_BASIS=spiel beginnt das Skript nach dem Raumwechsel neu und drueckte bei F91 "hoch" = Blattwechsel;
  mit Abzug vor F91 steht 2F mit Ring. Kein Befund.)
- Flaeche (Sonde = exe, Abschnitt 3a): ROOM1090 Band 5 0 von 11 059, Band 1 0 von 5 178, ROOM1091 (Elza) Band 5 0 von
  11 059 begehbaren Rasterpunkten neben der Zeichnung (jeder Punkt).

## 5. RE-Gate (selbst disassembliert / gelesen)
| Behauptung (Diff/Dossier) | Werkzeug | Befund |
|---|---|---|
| Zeilen-Tabelle @0x800768b0+8i: 1030 @0x800768C8 = (108,194,2428,2229); 1060 @0x800768E0, 10A0 @0x80076900, 1110 @0x80076938, 1090 @0x800768F8 = Stub; 1170 @0x80076968 = (100,206,2280,2268) | `re15_disasm.py bytes` (info/Re1.5/PSX.EXE) | `6c 00 c2 00 7c 09 b5 08`; viermal `00 00 00 00 01 00 01 00`; `64 00 ce 00 e8 08 dc 08` - **stimmt**. Dazu 1210 @0x800769B8 = `b1 00 8c 00 c0 09 ca 08` = (177,140,2496,2250) = die (unveraenderte) Port-Zeile von 1210 |
| Formel FUN_800473f8 | `re15_disasm.py dis 0x8004741c 70` | `addiu v0,v0,32000` @0x80047428, x10 (`sll/addu/sll` @0x8004742c-48), `lhu` sx @0x80047444 (0x800768b4+8i), `sra t0,t0,20` @0x8004745c, `addiu t0,t0,5` @0x80047460, /10 ueber `0x66666667` @0x80047450/54, `lh` ox @0x800474e8; z: `lhu` sy @0x8004747c, `sra v1,v1,20` @0x800474a8, +5 @0x800474ac, `subu v1,zero,v1` @0x800474b0, `lhu` oy @0x80047518 - **stimmt**. Damit von Hand nachgerechnet: 1060 1F x 26900 -> 119, 19800 -> 136, z 18200 -> 134, 26950 -> 155; 1110 x 12650 -> 146, -3250 -> 186, z -6650 -> 114, 5100 -> 144; 1170 Z1 3F neun Waende, Reste <= 1 px wie angegeben |
| Traeger-Gatter "unbesucht nur mit Plan", RE2 @0x8006E66C / @0x8006E744, Umriss-Zeile 498 @0x8006E71C | `re2_disasm.py dis 0x8006E640 70` (info/re2leon/PSX.EXE) | `jal 0x80077360` @0x8006E66C (Bit-Test Plan, 0x800d4924); ohne Plan Besucht-Test @0x8006E73C und `beq v0,zero,0x8006e768` @0x8006E744 = nicht zeichnen; mit Plan + unbesucht `addiu s5,zero,498` @0x8006E71C - **stimmt** |
| Hinweis-/Zielkachel immer gezeichnet, RE2 FUN_8006F1C4 @0x8006F4E8-0x8006F608 | `re2_disasm.py dis 0x8006F4D0 80` | `bne s4,a2,0x8006f518` @0x8006F4E8: der Ziel-Index umgeht die Besucht-/Plan-Tests, beide Blinkphasen enden in `jal 0x8008f828` @0x8006F608 - **stimmt** |
| SCA-Waende der neuen Zeilen (Datei-Offsets) | eigenes sca.py (RDT +0x20, Kopf 24 B, 12 B je Zelle) | alle 29 zitierten Offsets (1060 @0x004B8/4D0/4E8/4C4; 1030 @0x01A70/A40/A10/AE8/A4C/A34/A1C; 1110 @0x00734/71C/788/728/7A0; 10A0 @0x005DC/5E8/5D0/630; 1170 @0x00B70/B94/B64/BF4/BD0/B58/BE8/B7C/BDC) liegen auf Zellgrenzen und tragen genau die zitierte Wandflaeche - **stimmt** |
| STAIR-AOTs | `xxd` der RDTs | ROOM1060 @0x00D72 `2c 03 0d 31 08 00 f8 5c 58 4d 8c 0a 30 11` (Band 8, (23800,19800) 2700x4400), ROOM10A0 @0x00E90 (Band 4), ROOM1170 @0x013B6 (Band 4) / @0x0137A (Band 0) - **stimmt** |

- Suche im hinzugefuegten Code (`git diff master...HEAD -- re15_port | grep '^+'`) nach deferred/tunable/interim/for now/
  faithful/plausibel/TODO/FIXME/geschaetzt: einziger Treffer "GEMESSEN statt geschaetzt" (Kommentar). Kein neuer getenv in
  engine/include/platform.
- Jede neue/geaenderte Konstante der Nachbesserung traegt Adresse/Datei-Offset bzw. die Kennzeichnung PORT-ERGAENZUNG /
  PORT-KORREKTUR (1030: ausgelieferte Zeile mit belegter Abweichung zur eigenen Kunst, Beta->Retail) / NUTZER-VORGABE
  (Vorlagen-Symbole, M1/M2). Die Traeger je Korrektur sind datengetrieben (Kachel-Index 4 in MAP0x.PIX; Riegel
  unit_r39_karten_teil (a) prueft alle 15 bedingten Eintraege). **Gate haelt.**
- Erklaert der Fix den Befund? M3 ja (Gatter = Zeichner-Gatter; allein besucht klassengleich mit dem Stand vor Runde 39;
  Ort+Traeger ohne Doppelwand). M4 ja (Wandzeilen statt Bbox; Symbol <-> Projektion <= 2 px; exe = Sonde). M5 ja fuer
  1030/1060/10A0/1110/1170 Z1 (0 Punkte neben der Zeichnung); 1210 bewusst offen (Abschnitt 6).

## 6. Bewertung der offenen bzw. abweichenden Stellen
- **1210 Suedwest-Kammer (B2), ECHTE exe mit echtem Laufen** (G39abn1/w1210b: Leon im Flur (-19500,0,-21700), Blick
  West, `RE15_INPUT_SCRIPT=W1,U4,W5`): debug.log `F120 pos=(-19575,0,-21700)` .. `F210 pos=(-25500,0,-21700)` = durch die
  Oeffnung in die Kammer (SCA: Flur-Westwand @0x01BC8 endet bei z -20746, Kammer-Ostwand @0x01C40 beginnt bei z -22520).
  Kartenschirm: Ring **(191,117)**, 10 px links neben der Flur-Westwand x201 im Blau. Flaeche: 1 693 von 10 387
  begehbaren Rasterpunkten (16 %). Das ist dieselbe Fehlerklasse wie Nutzerpunkt 2 ("Positionsmarker ... ausserhalb des
  aktuellen Raums"). Original: Zeile @0x800769B8 + MAP02.PIX r3 zeichnen die Kammer ebenfalls nicht (RE1.5 unfertig);
  die Vorlage mapB2_correct.png laesst die Stelle leer. Der Bau hat das als Entscheidungsfrage gestellt (Memory
  reai-v2-original-oder-nicht: keine Wahlfragen; Beta->Retail: RE2 zeichnet jede begehbare Flaeche). Bis zur
  Aufloesung ist der Wortlaut "natuerlich muss der Spielermarker da entsprechend passen" fuer B2 nicht erfuellt ->
  Mangel N1.
  ```
  x 183..216      (o = Ring um (191,117); W/R = Flur ROOM1210, aktuell)
  y115 .......ooo........WRRRRRRRRRRW
  y117 ......o.o.o.......WRRRRRRRRRRW
  y119 .......ooo........WRRRRRRRRRRW
  ```
- **3F: zwei Treppensymbole bewusst anders als map3F_correct.png** (1060 (144,147) -> (140,147); 1170 Zone 1 (159,99)
  -> (156,98); die dritte vom Bau genannte (168,106) ist in meiner Blockmitten-Abtastung KEINE Abweichung). Gemessen:
  mit den Wandzeilen liegen die Laeufe bei x140 (1060 Band 8 @0x00D72) bzw. (156,95..101) (1170 @0x0137A/@0x0138E); auf
  den Vorlagen-Positionen stuende der Marker auf der Treppe 3..4 px neben dem Symbol. Die Verschiebung entspricht der
  Korrektur, die der Nutzer selbst auf 1F/2F an denselben Laeufen vorgenommen hat (x127 -> x122, x126 -> x121), und ist im
  Dossier "Fuer den Nutzer" offengelegt. Bewertung: vom zweiten Teil desselben Wortlauts ("Spielermarker passt")
  gedeckt - kein Mangel, Hinweis H3. Bilder: `G39abn1/3F_1060_links_soll_neu.png`, `G39abn1/3F_1170_links_soll_neu.png`
  (je: Vorlage links | Vorlage rechts | NEU, x6).

## 7. Tests (gezielt, KEINE volle Suite im Spurbaum)
- Gezielter Lauf, dieselben 96 Tests wie der Bau (alle Karten-/Hinweis-/Inventar-Gruppen, die der Diff beruehrt; mit
  `ctest -N` gegen alle Tests mit karte/map/hinweis/hint im Namen abgeglichen - die nicht enthaltenen sind PRI-/Biss-/
  Griff-Riegel ohne Kartenbezug): Regex wie im Dossier (Abschnitt "Nachbesserung 1 - Bau und gezielte Tests"),
  `-j 4 --timeout 900 --output-on-failure` -> **100% tests passed, 0 tests failed out of 96** (697 s, parallel zu meinen
  exe-Laeufen). Log: G39abn1/ctest_ziel.log.
- `integration_r39_karten` einzeln mit -V nachgefahren: S Blatt 1/2/3 je 0 Abweichungen, Blatt 4 0 Abweichungen ausserhalb
  der drei Kaesten (je 15/15 Sprossenpixel), Doppelwand 0/0/0/0/0, M Ring (183,72) rot=36 besucht=0, TN/TW/TZ je
  "0 Koerperpixel am Grund" -> Passed (81,9 s).
- Riegel messen (selbst gegengeprueft): der `offen`-Zaehler der Sonde (Laeufe TN/TW/TZ) auf die REGRESSIONS-Abzuege der
  Abnahme 0 (Stand 40328436, G39abn0/al_*_neu): 1000 Nord-WC 23, 1070 34, 1010 Z0 30, 1020 15, 1040 14, 1050 1, 1000 Ost 38,
  Sued-WC 9, 1080 8, 1220 Z0 16 -> ROT; auf meine NEU-Abzuege je 0. unit_r39_karten_teil/_treppen/_marker/_b1/_1090 mit -V:
  alle PASS-Zeilen deckungsgleich mit den Zahlen dieser Abnahme (z.B. Treppen 1/1, 1/1, 0, 1/1, 1/2, 0/0, 0/1 px).
- Kein Riegel fuer die 1210-Kammer (bewusst offen) - siehe N1.

## 8. Vertrag / Pfade / Nebenwirkungen
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots/Ereignisse belegt (Diff gelesen).
- Pfade (`git diff master...HEAD --name-only`): engine/src/karte_r39.c, map_hint_common.c, re15_inv_screen.c,
  re15_map_zones.h, include/re15_karte_r39.h, Tests unter tests/unit und tests/integration (*.cmake-Laufskripte, eine
  Sonde probes/r39_karten.cmake); KEINE release/, platform/android/, shared_assets/PSX/, keine CMakeLists.txt.
- Gemeinsame Dateien: die Nachbesserung aendert in re15_inv_screen.c nur die fuenf vorhandenen "Runde 39 Spur G"-Aufrufe
  (Argument `st` durchgereicht, keine neue Zeile); map_hint_common.c unveraendert gegenueber Abnahme 0 (3 Haken).
- Linux-/Parallel-Festigkeit der neuen Laeufe: `cmake_minimum_required` im -P-Skript, exe-Kopie
  `re15_pc_r39g_karten_<id>` mit LAST_EXT, eigener Laufordner je Lauf, `m` gelinkt (probes/r39_karten.cmake). **haelt.**
- Nebenwirkungen: Sweep aller 13 Blaetter NEU gegen VORHER (22abaed9): Blaetter 5..12 je **0** Pixel verschieden (andere
  Stages unberuehrt); gegen den Stand der Abnahme 0 (40328436) nur B1 30 px (Treppe M2) und 3F 60 px (drei Treppen).
  Elza (PL04) im echten Weg 10F1 -> 1091 wie Leon. ROOM1000 "Rot nur fuer die Zone, in der man steht" unveraendert
  (Abnahme 0 Abschnitt 8).
- Master-Stand: seit der Merge-Basis hat master nur test_r38_itemorte.c (Spur H) geaendert - keine Ueberschneidung.

## 9. Urteile
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1a Blaetter 1F/2F/3F/B2 nach den Vorlagen ("keine Doppelwaende, Tuer genau auf Wand, Treppen nicht in der Wand") | **erfuellt** | echte exe, Vollzustand: B2/1F/2F 0 Klassen-Abweichungen gegen die rechten Haelften, 3F nur die zwei begruendeten Treppensymbole (H3); Doppelwand 0 auf B1..3F, alle Tueren auf Wand, keine Treppe in einer Wand. Teilzustaende (M3): 15 Orte allein klassengleich mit dem Stand vor Runde 39, 0 verlorene Wandpixel (vorher 8..41); 9 weitere Teilzustaende (Paare, Plan) ohne Doppelwand und ohne offene Seite; 3F-Raeume 1130/1120/1140 allein ebenso. gdigrab = FB-Abzug |
| 1b "natuerlich muss der Spielermarker da entsprechend passen" (Polizeistation) | **teilweise** | Marker in jeder begehbaren Lage auf der Zeichnung: 1030/1060/10A0/1110/1170 Z1/10C0/1120/10E0/1090 je 0 (jeder Rasterpunkt), Treppen <= 2 px am Symbol (echte exe 1060: Ring (122,143) auf (122,144)); Tuerspruenge Stage 1 Mittel 4,9 -> 3,9 px, max 20 -> 9. ABER ROOM1210 Suedwest-Kammer: echte exe, Leon laeuft hinein -> Ring (191,117) 10 px neben der Zeichnung, 1 693/10 387 Punkte (N1) |
| 1c B1 neu ("passt nichts zusammen mit dem wie man sich bewegt") | **erfuellt** | Abnahme 0 (Gang 2,13..2,29 px/1000, Tuersprung <= 6 px, 0 Punkte neben der Zeichnung) unveraendert bestaetigt (unit_r39_karten_b1 mit -V, Sweep B1 Doppelwand 0); M2 behoben: keine Sprosse beruehrt eine Wand (vorher Unterkante y168) |
| 1 gesamt | **teilweise** | wegen 1b (N1) |
| 2 ROOM1090 Leons Marker im aktuellen Raum | **erfuellt** | Nutzerpunkt Ring (183,72) im Rot (vorher (186,83) im gruenen Nachbarraum), gdigrab des echten Fensters bestaetigt; echter Weg Leon 10F0 -> Klettern -> Tuer: Ring (182,72) im Rot; echter Weg Elza 10F1 -> 1091 (PL04): Ring (182,72) im Rot; Flaeche 0 von 11 059 / 5 178 |

## 10. Maengel (nummeriert, nachpruefbar)
- **N1 (Punkt 1b; Zeichnung oder ausdrueckliche Nutzerentscheidung noetig):** ROOM1210 (B2) Suedwest-Kammer - begehbar
  (SCA: Oeffnung zwischen dem Ende der Flur-Westwand @0x01BC8 bei z -20746 und dem Beginn der Kammer-Ostwand @0x01C40
  bei z -22520), aber weder im Original (MAP02.PIX r3, Zeile @0x800769B8) noch in mapB2_correct.png gezeichnet.
  Pruefen: Spielstand `probe_r36_karte karte re15_card.mcr 1210 -19500 0 -21700 2048 besucht:11E0:0:0
  besucht:1210:-19500:-21700`, LOAD GAME, `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U4,W5`, Karte bei
  `RE15_INV_OPEN_AT=200#1210` -> debug.log `pos=(-25500,0,-21700)`, Ring (191,117) im Blau links der Flurwand x201.
  Flaeche 1 693 von 10 387 Rasterpunkten (`probe_r39_karten markerflaeche 1210 0 -25800 -2200 0 100 1`). Gleiche
  Fehlerklasse wie Nutzerpunkt 2. Der Bau hat die Wahl an den Nutzer gegeben; nach Memory reai-v2-original-oder-nicht
  ("keine Wahlfragen") und Beta->Retail (RE2 zeichnet jede begehbare Flaeche) ist das Dazuzeichnen der Kammer samt
  Durchgang (Methode wie B1 / ROOM1000 aus der Kollision) der vom Wortlaut gedeckte Weg; alternativ bestaetigt der Nutzer
  ausdruecklich "B2 bleibt wie die Vorlage". Bis dahin ist Punkt 1 teilweise.

## 11. Hinweise (kein Mangel)
- **H1 (vorbestehend):** Tuer 1030<->1050 `{2,4,186,152,3,4,6,1}` - ist 1050 besucht und 1030 nicht gezeichnet, steht der
  Balken (186,150..154) eine Spalte links neben der 1050-Wand x187 im Leeren (VORHER- und NEU-Abzug pixelgleich,
  G39abn0/al_1050_alt gegen G39abn1/zs_al_1050; ebenso Zustand 1050+1070). Im Vollzustand (Vorlage) korrekt auf der einen
  Wand x186.
- **H2 (vorbestehend, mit den Wandzeilen sichtbarer):** Klemmung `reserve = 4` ins NOMINALE Kachel-Rechteck
  (re15_inv_screen.c:869): an West-/Nordwaenden bleibt der Marker 4 px von der Wand (z.B. 1060 1F Marker x 123..135 bei
  Waenden x119/x136; Ankunft 1130 -> 1170 y91 -> y93), an Ost-/Suedwaenden reicht er bis an die Wand. Folge: vier
  Tuerpaare springen 1..3 px weiter als vorher (1060->10C0 2->5, 1060->1120 3->6, 10C0<->10D0 5->6, 10D0<->10E0 2->5),
  bei insgesamt kleineren Spruengen (Abschnitt 3d). Im Dossier offen benannt.
- **H3:** 3F weicht an zwei Treppensymbolen bewusst von map3F_correct.png ab (Abschnitt 6) - dem Nutzer zur Bestaetigung
  vorlegen (Dossier "Fuer den Nutzer" tut das).
- **H4 (neu, kosmetisch):** ROOM1130 (3F r4) allein gezeichnet: die IMMER-Korrektur verschiebt die Westlinie x144 -> x145
  bzw. x152 -> x153, die Eckpixel der Quer-Linien bleiben stehen -> drei 1-px-Wandnasen (144,80), (144,119), (152,158)
  (G39abn1/zs_al3_1130). Im Vollzustand von den Nachbarn verdeckt; Raum geschlossen, keine Doppelwand.
- **H5:** ROOM1170 Zone 1 liess sich auch von mir nicht in der echten exe abziehen (CONTINUE mit (3,125)=1 feuert
  `DOOR FIRE slot=2 ... spawn=(-26214,0,-3861)` nach ROOM1240; Tuer 1130 slot 3 loeste in drei Standlagen nicht aus).
  Die Sonde ist an 13 Lagen auf fuenf Blaettern (B1, B2, 1F, 2F, 3F) als identisch zur exe belegt (Abschnitt 3a); ihre 1170-Z1-Zahlen
  (0/2873, Dach 0/1656) gelten.
- **H6 (vorbestehend, nicht Polizeistation):** 11A0 Band 1 (Blatt 6) 102/444 Punkte neben der Zeichnung - von Runde 39
  nicht beruehrt.

## 12. Ergebnis
**bestanden = false.** Punkt 2 erfuellt; Punkt 1 teilweise (1a und 1c erfuellt, 1b wegen N1: ROOM1210-Kammer). Die
Maengel M1..M4 der Abnahme 0 und M5 ausser dem 1210-Teil sind an der echten exe nachweislich behoben (M3-Regression weg,
Treppen am Symbol, Marker in der Zeichnung). Alle Gates (RE, Vertrag, Pfade, gezielte Tests 96/96) halten.
