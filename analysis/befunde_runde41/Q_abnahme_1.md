# Spur Q "restpunkte" - Abnahme 1

Baum `.claude/worktrees/r41_restpunkte`, Zweig r41/restpunkte, Kopf 0f9a1797e, Merge-Basis 91cd9ec82. master ist weiter (Spur R u.a.), `git diff master` zeigt deren Stand als Loeschung; gelesen wurde `git diff 91cd9ec82..HEAD`. `git merge-tree --write-tree HEAD master`: nur ein Tree-Hash, kein Konflikt. engine/, include/, platform/ seit Abnahme 0 unveraendert (`git diff 6887d9b88..HEAD --stat` leer), geaendert sind Masken, Riegel, Werkzeuge, Dossiers.
Bau: `local_build.sh configure` + `build`, `LOCAL-BUILD-OK`, ninja: no work to do. Scratch: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/Q1abn` (kurz `Q1abn`): Werkzeuge `tools/`, Laeufe `runs/`, Bilder `bilder/`.

**Ergebnis: BESTANDEN.** Alle Punkte erfuellt, alle Gates halten, kein Mangel. Mangel M1 der Abnahme 0 ist behoben: die Stelle des Mangels steht jetzt auf 0 von 96 (Stand davor mit derselben exe 63 von 96), an 28 eigenen Stellen Leon und Elza 0 sichtbar, auf dem echten Weg ueber die Treppe 0 Figurpunkte auf dem Teil. Hinweise H1 bis H7 sind keine Maengel der Spur; H1 ist ein Nebenbefund aus dem echten Weg (Treppenende in ROOM6010/6011) und gehoert in die Release Notes oder eine eigene Spur.

## Urteile
| Punkt | Urteil | Kern |
|---|---|---|
| 1 Restloch ROOM6010/6011 Cut 5 Teil 8 schliessen (Abnahme M 5 Mangel 1) | **erfuellt** | Teil-8-Punkte |A - A0| > 24 an der Stelle des Mangels 0 von 96, Negativkontrolle 63, eigene Stellen Leon 1689 / 0, Elza 1168 / 0, 0 uebermalt, kein Platz mit Figur vor dem Teil, echter Weg 0 auf dem Teil. |
| 2 Teile-Bericht, 8 Stellen, 4040 C12 | **erfuellt** | 0 Abweichungen gegen pl_a3, pl_a3 = MASKS bis auf die 4 Dateien von Teil 8, 8 Stellen + 70 Restbilder da, 4040 C12 an der exe ohne HINTER-Stelle. Nachtrag zu T8 steht im Bericht (H4). |
| 3 Riegel Elza-Kette, Ursache der stillen Enden | **erfuellt** | Riegel gruen, 2031 -> 20B1 -> 2051 als PL04; stilles Ende selbst reproduziert (rv=139, letzte Zeile F750), mit Schutz 0 von 8. |

## Punkt 1 - Restloch ROOM6010/6011 Cut 5 Teil 8
Eigene Werkzeuge, eigener Parser, exe-Kopien `re15_pc_q1abn_*`, je Lauf eigener Ordner, Umgebung nicht geerbt. Rezept wie Mangel 1 der Abnahme M 5: Sprung, RE15_PLAYER_POS, RE15_FORCE_CUT=5, Bild 120, Laeufe mit / ohne Masken, Figur auf (20000,20000) mit / ohne Masken. Figur F = |B - D| > 24, sichtbar = |A - A0| > 24.

**Binaerdateien** (`tools/bin2.py`, nicht geom.py/schliessen_r41): Sektion 5 126 Rechtecke, R0..R112 gleich zum Basisstand, 10 uebrige Sektionen roh gleich, Lesezeiger trifft jedes Sektionsende. Teil 8: 426 Punkte, gleiche Punktmenge und Farbindizes wie Basis, Tiefen R113..R125 = 105..93, kein Bildpunkt doppelt, im Atlas keine Ueberlappung, Fenster R0..R112 bytegleich, CLUT gleich. ROOM6010 = ROOM6011, `_PRI05.TIM` ebenso. Bild ohne Figur: A0 gegen A0 des Stands davor 0 Abweichungen, D gegen `probe_bg_dump` 0.

**Negativkontrolle des eigenen Laeufers** (`RE15_CD_ROOT=Q1abn/abn0`, die 4 Dateien aus 6887d9b88, dieselbe exe):
| Stelle | Figur auf Teil | sichtbar Stand davor | sichtbar HEAD |
|---|---|---|---|
| Leon (-19912,-19720) Stelle des Mangels | 96 | 63 | 0 |
| Leon (-19812,-19620) | 141 | 50 | 0 |
| Elza (-19612,-19720) | 114 | 14 | 0 |
| Elza (-20012,-19820) | 20 | 20 | 0 |
Dazu HEAD: Leon (-20012,-19820) 61 / 0, (-19912,-19320) 77 / 0. Uebermalt ausserhalb des Teils je 0.

**Eigene 28 Stellen** (Gitter x -20262..-19062, z -20490..-19590, weder Liste des Bau-Agenten noch Abnahme 0, `Q1abn/ausw_q1.txt`): Leon 1689 Figurpunkte auf dem Teil, **0 sichtbar**, 0 uebermalt ausserhalb; Elza 1168, **0**, 0.

**Echter Weg ohne RE15_FORCE_CUT** (Tuer aus 5080/5081 `RE15_FIRE_AOT=0`, Wegplan, beide Treppen-Handler, Kamera 7 -> 11 -> 5, `tools/gang.sh`, `tools/gang_ausw.py`):
| Weg | Bilder in Kamera 5 | auf dem Teil | uebermalt | naechster Abstand |
|---|---|---|---|---|
| Leon Aufstieg West x -19730 | 43 | 0 | 0 | 8,9 |
| Leon Aufstieg x -19050, oben, Abstieg x -19400 | 140 | 0 | 0 | 4,2 |
| Leon Abstieg westlichste Spur x -19460, jedes Bild | 45 | 0 | 0 | 2,2 |
| Elza Aufstieg West x -19725 | 35 | 0 | 0 | 15,6 |
| Elza Aufstieg x -19030, danach oben (H1) | 160 | 0 | 0 | 32,6 |
| Elza Abstieg westlichste Spur, jedes Bild | 34 | 0 | 0 | 2,8 |
Erzwungene Kamera 5, ganze Treppe, jedes Bild (`tools/tr.sh`): Leon und Elza je 131 Bilder, 0 auf dem Teil, 7,2 / 10,8 wie Dossier.

**Ebene.** `wange_r41.py --ppm`: 0 Proben neben dem Sprung, alle Werte wie Dossier. Eigene Rechnung (`tools/tri2.py`, Kanten je Zeile selbst gesucht, eigene Kamera aus dem RDT): Unterkante x -20596, Oberkante x -20568 bei z -20500, dx/dz ~ 0, dy/dz -0,832 / -0,850, ein Bildpunkt Fehler = rund 30 Einheiten. Deck-Ecke aus Kamera 4 + 5 selbst trianguliert: (-20319,-7218,-16953), Hoehe trifft Band 4, Lage trifft die Westkante der Band-4-Zelle @0x00938 (-20284). Der Baukoerper reicht also westlich ueber die Zelle @0x0092C hinaus, die Ebene haelt.
`test_r37_masken36 fern 344 2 72 270 0`: gesamt 344, Stage 6 0. `neustellen 6010 5 113` und `6011`: `H -18212 -19320 0 54 ... | V -`.
**Keine Figur vor dem Teil, vollstaendig gezaehlt** (`tools/davor.py`, alle 18 181 Band-0-Plaetze, Zylinder Hoehe 3000, Radius 350, Kamera aus dem RDT): 269 Plaetze treffen Teil 8, x -20512..-17712, 0 davon haben einen Treffer VOR der Tiefe des Teils (Ebene x -20591 liegt westlich aller Plaetze). `neustellen` sieht davon nur 9, siehe Hinweis H5.
**Teil 8 sitzt richtig:** 0 Punkte links der Unterkante, 232 auf der Flaeche, 194 rechts der Oberkante (`tools/teilbild.py`), Farbabstand Atlas zu Hintergrund Mittel 9.

## Punkt 2 - Teile-Bericht, 8 Stellen, 4040 C12
Skript der Abnahme 0 gegen pl_a3: Sektionen 81, 267 / 103 112, 277 / 144 225, Abweichungen 0; Summenzeile gleich; Liste 70 Sektionen, 70 Restbilder, keines fehlt; die 8 Stellen stehen in der Liste. Die eine Skript-Zeile ROOM30A0 C0 ist die Sonderzeile "roter Ordner".
Neu gemessen: von den 150 Masken-Dateien des Laufs pl_a3 sind 146 bytegleich zu MASKS, die 4 anderen sind genau die 4 Dateien von Teil 8. Damit ist pl_a3 wirklich der ausgelieferte Stand.
T8-Zeile traegt den Stand der Nachbesserung als Nachtrag (Hinweis H4). Szenen: 104, Art M 38, alle mit A0, Fenster und Stellen gegen Basis unveraendert, nur t4040_12/e weg (`Q1abn`, Vergleich der Diff-Zeilen ohne A0).
4040 C12: dritter Kandidat (-3800,4478) an der exe: Figur 387, X 45..67, Teil X 75..105, 0 auf dem Teil.

## Punkt 3 - Elza-Riegel, stille Enden
`integration_r41_ventilkette_kette_elza` Passed 250 s, `integration_r41_ventilkette_kette` Passed 235 s; Log Elza: PL04, `room2031`, `room20b1`, `room2051`, `EXIT_AT: Bild 60 in Raum 2051`, Wegplan = Riegel (`kette.txt` gegen `Q41/kette_L.txt` diff leer).
**Stilles Ende selbst reproduziert** (`tools/ke.sh`, Rezept des Riegels ohne gdb, 4 Laeufe gleichzeitig, `Q1abn/ke.txt`):
| exe | Laeufe | rv=139 | Bild |
|---|---|---|---|
| ohne Schutz (Stand vor Spur Q) | 8 | 1 (`Segmentation fault`, Lauf o6) | letzte Zeile `[walk] F750 cut=10 ... pos=(-26539,-1...`, also genau das Bild aus INT_belege/riegel_kette_laeufe.txt; die 7 anderen `EXIT_AT: Bild 60 in Raum 2050` |
| ohne Schutz, gdb mit `_NO_DEBUG_HEAP=1` | 6 | 0 | diese Serie traf die Halde nicht, Abnahme 0 hatte hier 4 von 6 |
| mit Schutz (Baum) | 8 | 0 | je 16 Zeilen `[gpu-grenze]` (nur die ersten 16 je Prozess werden geschrieben), alle `EXIT_AT: Bild 60` |
Der Absturz bleibt also zufaellig (Lage der Halde), wie im Dossier. Zusammen mit Dossier (ohne 6 von 19, Endstand 0 von 12) und Abnahme 0 (ohne 4 von 6, mit 0 von 9): ohne Schutz 11 von 39, mit Schutz 0 von 29, Riegel nicht mitgezaehlt. Der Fehlzugriff selbst (srcy = -15 in `SDL_BlitTriangle_Slow`) stammt aus Abnahme 0; der Code ist seither unveraendert (`git diff 6887d9b88..HEAD` leer unter engine/platform/include).

## Punkte 4/5
Stehen nicht in der Punkteliste dieser Abnahme, sind aber im Diff. Riegel gator(_elza) und original(_elza) gruen, Teil-Laeufe selbst gelesen: Leon und Elza Lauf 1 Tod + `[save] saved`, 1 x `in Wasserzone gesetzt`, Lauf 2 `CONTINUE: resumed in room 2080/2081`, 0 t=23, Lauf 3 `inv ... 34:1`, Lauf 4 0 t=23; original: 2 x `Sce_em_set type=0x25`, `EM23 loaded` 1, 0 verworfen. RE2 ROOM40A0 selbst gelesen (`info/re2leon/PL0/RDT/ROOM40A0.RDT`): @0x2550 `06 00 64 00`, @0x2554 `21 04 33 00`, @0x25A0 `44 00 00 23 ...`, main @0x00A4 `46 05`, @0x00AE `22 04 33 01`.

## Gates
- **RE-Gate haelt.** Selbst disassembliert (`re15_disasm.py`, info/Re1.5/PSX.EXE), Zitat traegt: @0x80025640 `4b58002d` AVSZ3; @0x8002565c `sra v1,v1,4`; @0x80066c70 `addiu t0,zero,341`, @0x80066c74 `48c8e800` (CTC2 Register 29 = ZSF3), damit bucket = (1023*vz)>>16; @0x80039650 `lh a0,2(s3)` vor `addu a0,v0,a0` / `jal 0x8006b538`; @0x8003928c `lbu v0,7(a0)`. Zelle @0x0092C im RDT selbst gelesen: x -19919..-17054, z -24100..-17655, floor 0x13. Stair-Record `slot=9/10 corner=-24300 extent=6895` aus eigenem `stair_dbg.log`.
  Konstanten des Diffs seit Abnahme 0: Tiefen 105..93 und x -20591 sind Messung, Herleitung und Riegel liegen vor (`wange_r41.py`, `unit_r41_restpunkte_wange`), im Dossier als Messung gekennzeichnet; `stufen_r41.py --wange -20591 --trocken` rechnet die ausgelieferten 13 Rechtecke samt Fenster genau nach. Teil-Einsatz selbst ist Entscheid M-E2. Kein "deferred/tunable/interim/for now/faithful/plausibel/TODO" in den neuen Zeilen, kein neuer Schalter.
  Der Fix erklaert den Befund: die Tiefe kam aus der Zelle @0x0092C statt aus der Wange, mit der Tiefe der Wange ist die Stelle des Mangels 63 -> 0 von 96, dieselbe exe, nur die 4 Dateien anders.
- **Vertrag haelt.** Pfade: `shared_assets` nur die 4 Dateien unter `MASKS/`, keine `release/`, `platform/android/`, `pri/`, `synchro/`, keine CMakeLists (Riegel in `probes/r41_restpunkte.cmake`). Keine neuen Bits, keine Nachrichten-Ids. `pri_hashes` betrifft nur Stage 1. Neue Laeufer: `cmake_minimum_required`, eigene Praefixe `r41qt_`, `r41qte_`, `r41q1`, eigene Laufordner, `re15_start_spiel` ohne geerbte Umgebung, `m` fuer Linux vorhanden.
- **Szenen nicht aufgeweicht:** alle 40 geaenderten Zeilen der Gruppen a1..d2 gegen Basis: Stellen, Fenster, SMIN gleich, dazu nur A0; weg sind nur t4040_12 und t4040_12e.

## Gezielte Gruppen
| Gruppe | Regex | Ergebnis |
|---|---|---|
| Spur | `r41_restpunkte\|ventilkette_kette` | 16/16, 1050 s |
| Masken-Unit + Alligator-Unit | `^(unit_r41_masken36\|unit_r37_masken36\|unit_pri_\|unit_r36_pri\|unit_r37_pri1100\|unit_sld_atlas\|unit_bss_room_source\|prop_bss_chunk\|unit_r38_itemorte\|unit_r39_optik\|unit_stage6_hintergrund\|unit_r36_kartenbits\|unit_r41_gatorszene\|unit_r41_alligator2\|unit_r41_kanister2080\|unit_r41_re2gator)` | 97/97, 221 s |
| Masken-Integration | `^integration_r41_masken36\|^integration_pri_masken` -j 3 | 15/15, 2376 s |
| Wiederholung | `^integration_r41_masken36_(zelle\|zellen3d)$` -j 2, Last durch die beiden anderen Gruppen | 2/2 |
| Alligator, Wasser | `^integration_r41_(gatorszene\|alligator2\|kanister2080\|re2gator\|wasser20b0)\|^integration_r38_wasser\|^unit_r41_(gatorszene\|alligator2\|kanister2080\|re2gator)` -j 3 | 92/92, 1471 s |
| Quer | `^(unit_\|prop_\|probe_)` -j 6 | 1056/1056, 830 s |

## Maengel
Keine.

## Hinweise (keine Maengel der Spur)
- **H1 Nebenbefund aus dem echten Weg, vorbestehend: Treppenende ROOM6010/6011 schiebt die Figur seitlich aus der Treppe.** Ohne RE15_FORCE_CUT, Tuer aus 5080/5081, Treppen-Handler, Wegplan:
  | Figur | Spur | Ende |
  |---|---|---|
  | Leon | Aufstieg x -19727 | (-20752,-7200,-18208), steht ausserhalb des Gelaenders (`Q1abn/bilder/oben_L.png`) |
  | Leon | Aufstieg x -19050 | (-19039,-7200,-17187), richtig |
  | Leon | Aufstieg x -17805 | (-17797,-7200,-17187), richtig |
  | Leon | Abstieg x -19401 | (-20387,-1800,-23153) |
  | Elza | Aufstieg x -19030 | (-20387,-7200,-18781), Wegplan haengt danach an der Ecke (-20387,-18752) |
  | Elza | Aufstieg x -18491 | (-18491,-7200,-17187), richtig |
  | Elza | Aufstieg x -17785 | (-16586,-7200,-18804) |
  Leon x -19727 und Elza x -19030 ergeben auf der exe ohne Spur Q dieselben Werte (Laeufe m1L, m2E), `stair_common.c` steht nicht im Diff. Passt zu `stair_common.c:180`: `if (fwd >= 0)` nimmt auch einen seitlichen Schub (fwd = 0) an; die Figur steht am Ende der Steigung noch in der Band-4-Zelle @0x0095C, Elza frueher als Leon. Bei Elza ist nur ein schmaler Streifen in der Treppenmitte sicher. Mit dem Original nicht verglichen.
- **H2 Flatternde Szenen des Bau-Agenten nicht reproduziert.** d4040_07, d5020_06, d6000_05a je 6 x mit, 6 x ohne Masken auf der exe mit Spur Q und auf der exe ohne Spur Q (72 Laeufe, `tools/fl.sh`): je Art ein einziger Bildhash im Fenster, und beide exe liefern dieselben Hashes. `zelle` und `zellen3d` zweimal gruen. Was der Bau-Agent sah, kommt nicht aus Spur Q; die Ursache bleibt ungeklaert, bei der Integration einzeln nachfahren, wie er schreibt.
- **H3 Die Wange ist in Kamera 11 nicht zu sehen.** Die triangulierten Kanten laufen dort ueber die Zugfenster, im Hintergrund steht nichts davor (`Q1abn/bilder/linien_c11.png`); das Band im Bild passt zur Ostwange (gerechnet (142,135), (101,96) gegen die Bildkante), wie Dossier und Abnahme 0 schreiben. Die Ebene stuetzen also nur Kamera 4 + 5 und die Deck-Ecke. Fuer Cut 5 aendert das nichts, weil keine Figur davor steht.
- **H4 Kopfzeile im Teile-Bericht:** `ROOM6010 Cut 5: neue Rechtecke 114` ist der Stand des Laufs, ausgeliefert sind 112. Die T8-Zeile sagt das im Nachtrag (`Sektion 112 neue Rechtecke`), die Kopfzeile allein stimmt nicht.
- **H5 Riegel sehen weniger als ihr Text.** `unit_r41_restpunkte_teil8_davor` (`neustellen`) zaehlt nur Plaetze mit mindestens 8/9 der Figur im Bild: 9 der 269 Plaetze auf Teil 8, die Aussage "V -" stimmt, belegt ist sie erst durch `tools/davor.py`. `integration_r41_restpunkte_treppe` faehrt nur den Aufstieg x -19700 (7,2 / 10,8 Bildpunkte); der echte Abstieg auf der westlichsten Spur x -19460 kommt bis 2,2 / 2,8 Bildpunkte heran, ebenfalls ohne Beruehrung.
- **H6 Neben dem Teil bleibt die Figur sichtbar, nur mit RE15_FORCE_CUT:** auf der Aussenflaeche ausserhalb des Teils hoechstens 18 Punkte ((-19912,-19320), wie Dossier), rechts vom Teil auf den Stufen bis 272 Punkte ((-19062,-19890), Figur unter der Treppe). Das ist nicht Teil 8; im Spiel steht dort nie Kamera 5 (RVD 5 nur aus 11, Abnahme 0).
- **H7 Dossier Q:** die Abschnitte des Erststands (Punkt 1 "Umsetzung", "Punkt 1 - Riegel am Endstand") nennen noch R113..R127 und die Schranke 1188; die Berichtigung steht im selben Abschnitt und in "Nachbesserung 1". UEBERGABE 3b zieht der Orchestrator nach.

## Nicht geprueft
PSX-Bau, volle Suite (nach Auftrag nicht im Spurbaum), Linux-Lauf, Original am Emulator (Teil 8 ist im Original nicht eingesetzt). Die 68 Riegel mit RE15_SOFTWARE_RENDER liefen nur in Abnahme 0, der Code ist seither unveraendert; hier liefen davon die Ketten- und Alligator-Riegel.
