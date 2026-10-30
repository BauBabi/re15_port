# Spur M masken36 — Abnahme 1 (unabhaengig, 2026-10-24, nach Nachbesserung 1)

Baum `.claude/worktrees/r41_masken36` (Zweig r41/masken36, merge-base master 4c667c97, Kopf 11e89aa8; Nachbesserung 1 =
ee7149ed..11e89aa8, 26 Commits). Pruefer: Abnahme 1. Massstab: Wortlaut AUFTRAG.md (Punkte 1/2, Nachtrag 11), VERTRAG.md §1.3/§2,
CLAUDE.md RE-Gate, Maengel 1-5 der Abnahme 0 (`M_abnahme_0.md`). Bericht wird fortlaufend geschrieben (Sitzungslimit).
Messordner (Scratchpad): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/m41abn1/`.

## Stand
- [x] Lektuere AUFTRAG (inkl. Nachtrag 11), VERTRAG, Dossier (NB1-Abschnitte), Abnahme 0; `git log master..HEAD`, Diff gelesen
- [x] Bau: `local_build.sh configure` OK, `build` "ninja: no work to do" (exe 14:57:44 juenger als gen/masken36_ausschluss_r37.inc
      14:57:26; `git status` sauber)
- [x] Statische Gegenproben NB1 (eigener Leser `stat_nb1.py`, s.u.)
- [x] Gezielte Tests (unit 36/36; Integration s. Abschnitt Tests)
- [x] Punkt 1 an der exe (eigene DAVOR/HINTER-Stellen, 3D-Pruefung aller 43 Cuts, formgenaue Gegenprobe)
- [x] Punkt 2 an der exe (2030/2031 C15 Spinne, 2030/2040 C7 Gelaender vorher/nachher, VOR-Stellen 3D, OFFEN-8-Stichprobe)
- [x] RE-Gate (3 Adressen selbst disassembliert + Zellen-Bytes)
- [x] Vertrag
- [x] Urteil: **nicht bestanden** (Punkt 1 teilweise, Punkt 2 erfuellt; Gates halten)

## Messmittel (eigene, Scratchpad m41abn1)
- `run.sh` Szenen-Laeufer: Messkopie `exe/re15_pc_m41abn1.exe` (md5 77207dcd..., = Spurbaum-exe 14:57:44), RE15_DEBUG_JUMP=<raum>@250,
  RE15_PLAYER_POS, RE15_FORCE_CUT, RE15_FRAMEDUMP=119-120/1, RE15_EXIT_AT=120#<raum>, RE15_WINDOW_SCALE=1, wahlweise RE15_NO_PRI
  (B), RE15_TITLE_SHOT_CHAR=1 (Elza, Raum xxx1 im Log geprueft), RE15_GEGNER_POS, RE15_TRILOG.
- `tri.py`/`ana1.py`: letztes Bild aus RE15_TRILOG (Bildanfang = Wechsel auf Slot 0; Folge je Bild Slot 0 -> Gegner -> Props),
  eigener Dreiecks-Rasterer (vorderstes Dreieck = kleinstes z je Punkt). **uebermalt** = |A-B| > 24 (A = mit Masken, B = RE15_NO_PRI);
  **Figurpunkt** = vorderstes Dreieck aus Slot 0 (Spielfigur; Slot 4/5.. = RE15_TIM_SLOT_PROP, Gegner eigene Slots).
  Zusaetzlich "innen" = Figurpunkte nach 1 Punkt Erosion (Rasterkanten des eigenen Rasterers ausgeklammert).
- `maske.py`/`schluessel.py`: gezeichnete Masken wie der Lader (R15M-Sektion, Ausschluss-Tabelle, draw_count, Atlas deckend =
  CLUT-Farbe != 0) -> je Punkt kleinster Schluessel (depth+1)*65536/1023 (include/re15_pri.h re15_pri_mask_camera_z); Gegenprobe,
  ob ein A/B-Unterschied auf einem Figurpunkt ueberhaupt von einer Maske VOR der Figur kommen kann (Schluessel <= Dreieck-z).

## Statische Gegenprobe NB1 (eigener Leser `stat_nb1.py`, vor = ee7149ed, nach = 11e89aa8)
- Ausschluss-Tabelle: 148 -> 128 Zeilen; **wieder aufgenommen 367 Rechtecke (Leon) + 367 (Elza), neu ausgeschlossen 0**;
  ausgeschlossene Leon-Rechtecke gesamt 1553 -> 1186 (= 530 alt + 656 Saum laut Bau-Agent).
- MSK Stage 3..6 (24 Dateien, 12 Paare Leon/Elza): **0 Bytes ausserhalb der Tiefenfelder** (+4 im Satz) geaendert; jedes der 367
  wieder aufgenommenen Rechtecke hat eine neue, fernere Tiefe; kein Rechteck mit neuer Tiefe ist weiter ausgeschlossen, keines war
  vorher drin (d.h. "--zurueck" wirkt wie beschrieben). ROOM2030.MSK: nur Cut 7 (Neubau 07_01 geteilt, 923 Bytes ausserhalb der
  Tiefenfelder = neue Rechtecke); ROOM2040.MSK: nur Cut 7 (37 -> 33 Rechtecke).
- Gezeichnete Generator-Punkte Leon Stage 3..6 (Vereinigung der deckenden Atlaspunkte der nicht ausgeschlossenen Rechtecke):
  **vor NB1 780 626 (nur in ausgeschlossenen 450 317) -> nach NB1 889 591 (341 352)** - Zahlen des Bau-Agenten bitgenau bestaetigt.
- `unit_r41_masken36_loecher -V`: Kerben VORHER 62 432 / NACHHER 11 494 (18,4 %); Blit max Mittel 10,66.
- `unit_r37_masken36_ausschluss` (rechnet die Tabelle aus den Dateien neu, 652 s): Passed - Tabelle passt zu den MSK.

## Tests (gezielt, Spurbaum)
1. `ctest -R "unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_r38_itemorte|unit_r39_optik|unit_r34n_e_dokumente|unit_r37_fahrstuhl|unit_bss|prop_bss|unit_stage6_hintergrund" -j 3`
   -> **`100% tests passed, 0 tests failed out of 36`** (Protokoll m41abn1/ctest_unit.log).
2. `ctest -R "integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r37_itemband|integration_r37_fahrstuhl_n1|integration_r38_spinnen_echtlauf|integration_r36_dachkiste|integration_r38_affen3|integration_r35_granate|integration_r35_zgirl_5090|integration_r37_sperre1050" -j 2 --timeout 1800`
   -> **`100% tests passed, 0 tests failed out of 26`** (4300 s, unter Last anderer Spuren; Protokoll m41abn1/ctest_integ.log). Darin
   die Spur-Riegel `integration_r41_masken36_stage2` (505 s, mit den NB1-Szenen v/m2030_15, v2031_15, m2030_07, m2040_07) und
   `integration_r41_masken36_zelle` (446 s, 16 Szenen), alle R37-Masken-Riegel (stage3..6, nb2), alle PRI-/Optik-/Itemorte-Riegel und
   die Verhaltens-Riegel der beruehrten Raeume (2030: fahrstuhl_n1, itemband(_lift), spinnen_echtlauf; 3040: affen3 x3; 3050/2040:
   dachkiste, itemband; 50B0: sperre1050; 5090: granate, zgirl_5090). Kein roter Fenster-Haken, keine Wiederholung noetig.

## Punkt 1 - die 367 wieder aufgenommenen Rechtecke (NB1 Mangel 1)
### 1a Eigene statische 3D-Pruefung aller 43 Cuts (`scan_davor.py`)
Eigene Kamera aus dem RDT-Kamerasatz (+0x24, 32 B je Cut: fov +2, P +4, T +0x10; gegen das exe-Log ROOM2040 C7 geprueft: VR/VT bis
0,4 % gleich, Kameraort (-17265,-7311,-9105) identisch), eigener SCA-Leser (alle soliden Zellen u0&1, Typ 1..9, als Quader vom
Bandboden bis 2400 darueber - grosszuegig), Plaetze `test_r37_masken36 plaetze` (jeder 9.), Figur 45 Punkte (3 x 3 x 5).
Gezaehlt: Figurpunkte, die ein NB1-Rechteck als vorderste Maske verdeckt (Schluessel <= vz), und davon die, deren Sehstrahl
KEINE solide Zelle kreuzt (= Uebermalung ohne Gegenstand davor).
| Cuts | Plaetze geprueft | im Bild | Plaetze mit NB1-Verdeckung | davon ohne Zelle davor |
|---|---|---|---|---|
| 43 | 41 029 | 24 463 | 990 (3050 C5 229, 4040 C6 203, 6010 C6 159, 4040 C7 88, 5100 C3 87, 5100 C2/C7 je 66, ...) | **0** |

### 1b Eigene Stellen an der exe (nicht die Szenen des Bau-Agenten; `scan_orte.py` waehlt je Cut die Plaetze mit den meisten
NB1-verdeckten Figurpunkten (HINTER) bzw. mit der groessten Ueberlappung ohne Verdeckung (DAVOR); Bild 120, A/B + Schluessel)
| Szene | Platz (Band 0) | Leon | unter Maske | verdeckt (Schluessel) | A/B uebermalt | verdeckende Masken (NB1 = wieder aufgenommen) | Sicht / Geometrie |
|---|---|---|---|---|---|---|---|
| ih3050_05 HINTER | (-5600,-6768) | 256 | 256 | 256 | 229 | 0, 3 (beide NB1) | ganz verdeckt; Strahl kreuzt Typ-1-Zellen x -20700..-12050 z -13300..-11750 / x -14050..-12050 z -12850..-3150 (Maschine links) - Leon steht dahinter, `bilder/ih3050_05.png` |
| id3050_05 DAVOR | (-7600,-14568) | 491 | 457 | **0** | **0** | - | frei |
| ih4040_06 HINTER | (-13600,-6522) | 316 | 316 | 288 | 225 | 0, 4 (4 NB1) | Strahl durch die Raute Typ 2 x -14337..-1048 z -17417..-4128 (L1-Abstand 0,65 < 1 bei s 0,73) = Mittelblock; Leon hinter dem Block |
| id4040_06 DAVOR | (-12400,-7922) | 383 | 383 | **0** | **0** | - | frei |
| ih4040_07 HINTER | (-13800,-6322) | 160 | 160 | 146 | 102 | 0, 2 (2 NB1) | dto. Mittelblock |
| ih4040_10 HINTER | (-15000,-12322) | 254 | 99 | 86 | 64 | 12, 22..25 (NB1) | - |
| id4040_10 DAVOR | (-14200,-11722) | 268 | 115 | 1 | 1 | 24 | 1 Kantenpunkt: Leon z 17970 < Schluessel 18002, also Raster-Kante, keine Uebermalung |
| ih5020_07 HINTER | (-13733,-13725) | 1035 | 818 | 702 | 541 | 27, 32, 76, 77 (NB1), 30 | hinter der Treppe, Kopf ueber dem Handlauf frei, `bilder/ih5020_07.png` |
| id5020_07 DAVOR | (-11933,-16525) | 2317 | 1459 | **0** | **0** | - | frei |
| id30A0_03 DAVOR | (-22343,15863) | 3906 | 1329 | **0** | **0** | - | frei (Prop Slot 4 von ALTEN Rechtecken 0,1,3,4,7,10,70,71 gedeckt, nicht NB1) |
| ih5100_07 HINTER | (-14330,-2200) | 1811 | 983 | 983 | 772 | 4..7, 14..21 (NB1) | Unterkoerper hinter der Brüstung links, Oberkoerper frei, `bilder/ih5100_07.png` |
| id5100_07 DAVOR | (-12130,-4600) | 4886 | 2201 | **0** | 8 | - | 8 Punkte zwischen Arm und Koerper: Schluessel 6278 > Leon z ~4100 = Atlasfarbe auf Hintergrund, keine Uebermalung (`bilder/id5100_07_zoom.png`) |
| ih5100_02 HINTER | (-23730,-19600) | 983 | 492 | 492 | 438 | 12, 16 (NB1), 20, 36..41 | - |
| id5100_02 DAVOR | (-20730,-23600) | 3605 | 1484 | **0** | **0** | - | frei |
| ih4040_11 (einziger Rest-Kandidat aus 1c) | (-1800,2478) | 925 | 559 | 1 | **0** | 14 | keine sichtbare Uebermalung |
**Ergebnis:** an 7 eigenen DAVOR-Stellen 0 uebermalte Leon-Punkte durch Masken (die 1 + 8 Punkte sind nachweislich Rasterkanten /
Atlasfarbe); an 7 eigenen HINTER-Stellen verdecken die wieder aufgenommenen Rechtecke Leon dort, wo eine solide Zelle davor liegt.
Zusaetzlich gruen: `integration_r41_masken36_zelle` (16 Szenen des Bau-Agenten) in meinem Lauf (s. Tests).

### 1c Formgenaue Gegenprobe (`scan_form.py`: Zellen als Rechteck/Raute(Typ 2, re15_collision.c push_diag2)/Kreis/Dreieck(4..7))
Die 13 Cuts mit NB1-Verdeckung aus 1a: 10 244 Plaetze im Bild, 990 mit NB1-Verdeckung, **1 Platz mit 1 Figurpunkt ohne Form davor**
(4040 C11 (-1800,2478)) - an der exe 0 uebermalte Punkte (Tabelle 1b). Hinweis: alle Szenen mit RE15_FORCE_CUT; ob der Cut an
der Stelle im Spiel aktiv ist, ist damit nicht gesagt (z.B. 4040 C6 hinter dem Mittelblock).

### 1d Was offen bleibt (Wortlaut "Masken ... schliessen")
- Statisch (eigene Zaehlung, s.o.): 530 alte + 656 Saum-Rechtecke (Leon) weiter ausgeschlossen, 341 352 Punkte nur in
  ausgeschlossenen Rechtecken; `unit_r41_masken36_loecher`: 11 494 offene Kerben (18,4 % von vorher).
- Ganze Cuts ohne jede Maske im Spiel (alle Rechtecke ausgeschlossen, .inc): 3040 C1 19/19, 3050 C6 19/19, 30E0 C6 35/35,
  30E0 C7 53/53 (3050 C15 70/70 = Original-NULL-Sektion, zu Recht). In 3050 C6 ist das Gelaender an der Treppe ein echter
  Gegenstand (NB1_rest_ROOM3050.png, rot).
- Echte Gegenstaende teilweise/gar nicht gezeichnet (Restliste des Bau-Agenten, von mir in den Bildern nachgesehen): 5020 C6
  grosse X-Kiste rechts (rot; im neuen Uebersichtsbild `M_bilder/uebersicht/STAGE5/5020/06_port.png` fehlt sie), 6000 C5 Lok und
  Gittertraeger links, 6000 C7/C8 Treppe/Traeger, 4080 C5/C6 Wandpaneele (Wand, zu Recht), 5020 C11 Boden (zu Recht).
  Eine sichtbare Fehlverdeckung im natuerlichen Spiel habe ich dafuer NICHT nachgewiesen (6000 C5 (22233,-1784): Leon am
  Gittertraeger, Strahl kreuzt nur eine Band-2-Zelle - nicht belegt; die Stellen mit FORCE_CUT sind nicht gleich Spielstellen).
- Der Bau-Agent fuehrt Punkt 1 selbst als TEILWEISE offen und legt die Restliste fuer Nutzer-Freistellungen vor
  (`M_bilder/NB1_rest/`, 14 Raeume, 52 Cuts).

## Punkt 2 / Mangel 3 der Abnahme 0 - ROOM2030/2031 Cut 15 (4400,3850) Band 3 (Behauptung: "die Punkte sind die Spinne")
| Szene | Figur (Slot 0) | uebermalt auf Figur (innen) | uebermalt andere | Schluessel-Gegenprobe |
|---|---|---|---|---|
| k2030_15 Leon, Spinne da | 1499 | 6 (0) | 306, alle Slot 11 (Spinne) | Leon z unter Masken 6905..8268 < Schluessel 8520..9353 -> 0 verdeckbar; Spinne z 8807..10513, 381 verdeckt |
| e2030_15 Elza (2031), Spinne da | 1037 | 4 (0) | 359, alle Slot 11 | Elza z 6899..8251 < 8520 -> 0 |
| g2030_15 Leon, Spinne weg (RE15_GEGNER_POS 1,-20000,-20000) | 1500 | **0** | 0 | 0 |
| h2031_15 Elza, Spinne weg | 1032 | **0** | 0 | 0 |
Die 6 bzw. 4 Unterschiedspunkte auf "Figur" liegen nur auf der Umrisskante des eigenen Rasterers und dort ist der Schluessel jeder
Maske groesser als Leons Dreieck-z - eine Maske kann dort nicht ueber Leon liegen; es sind Spinnenpunkte am Rand. Bild
`bilder/k2030_15.png` (blau Leon frei, gelb = Spinnenbeine hinter dem Handlauf). **Behauptung bestaetigt: die Reling uebermalt
Leon/Elza in C15 nicht; die 12/24 Punkte der Abnahme 0 waren die Spinne hinter Leon und der Reling (Mass |B-C| zaehlte jede Figur).**

## Punkt 2 / Mangel 4 der Abnahme 0 - ROOM2030 C7 / ROOM2040 C7 (Behauptung: "keine VOR-Stelle mit Figur im Bild", Handlauf behoben)
**VOR-Stellen (eigene 3D-Pruefung `vor3d.py`):** Kamera aus dem debug.log (VR/VT, H aus `proj sx`), Plaetze = `test_r37_masken36
plaetze <raum> 7` (Flutfuellung + Kamerazonen + Akteur-Cull), Figur = 36 Punkte (3 x 3 x 4, +-250, Hoehe 50..1700), Gelaender =
SCA-Zelle als Quader ueber Band 3 (Hoehe 1875 bzw. 1800 wie auswahl.json). VOR = kein Strahl Kamera -> Figurpunkt schneidet einen
Gelaender-Quader.
| Cut | Kamera (Log) | Plaetze Band 3 | im Bild | VOR | ganz HINTER | teils (Kopf ueber dem Handlauf) | Figurpunkte OHNE Gelaender davor, die eine Maske verdecken wuerde |
|---|---|---|---|---|---|---|---|
| 2030 C7 | (-23,-8311,-2042) | 2505 | 841 | **0** | 666 | 175 | **0** |
| 2040 C7 | (-17265,-7311,-9105) | 4526 | 1626 | **0** | 477 | 1149 | **0** |
(2040 C7: die 57 Plaetze auf der Kameraseite der Zelle, z < -6500, liegen alle bei x 3700..4100 ausserhalb des Bilds.) Die
Baender ueber der Kamera (2030 Band 6/9) liegen im Bild oberhalb, Band 0 unter dem Gelaender (Strahl durch den Quader = HINTER).
**Behauptung bestaetigt** - die VOR-Pruefung dieser Cuts ist leer, und auch an den teils verdeckten Plaetzen wird kein Figurpunkt
ohne Gelaender davor von einer Maske getroffen.
**Fix an der exe (Leon = vorderstes Dreieck, A/B + Schluessel):**
| Szene | Figur | unter Maske | verdeckt (Schluessel) | A/B uebermalt (innen) | Masken-Tiefen | Bau-Agent |
|---|---|---|---|---|---|---|
| n2040_07 Leon (-17500,-5600) B3 | 5439 | 359 | **359** | 321 (257) | 38..41 | 355/355 |
| f2041_07 Elza | 4677 | 188 | **188** | 164 (135) | 38..41 | 188/188 |
| n2030_07 Leon (4500,9150) B3 | 420 | 297 | **83** (Masken 22..26 = Nordgelaender) | 57 (32) | 170..178 Gelaender, 246..256 Pult 07_02 | 83/297 |
| f2031_07 Elza | 304 | 200 | **57** | 43 (22) | dto. | 57/200 |
Jeder A/B-Unterschied auf der Figur hat einen Schluessel <= Dreieck-z (0 ohne). Die offenen Punkte in 2030 C7 liegen unter 07_02
(Schluessel 15 800..16 464 > Leon z 11 471..12 611 = Pult HINTER Leon) - richtig. Uebermalte Props in 2040 C7 (Slots 5/7/8 =
oid 1/3/4, z 1700 / 850 / -1850 nordlich der Zelle z -6050) und 2030 C7 (oid 1 (6725,-5400,12050)) liegen hinter dem Gelaender;
die Spinnen in 2030 C7 hinter dem Gelaender (Bild `bilder/n2030_07.png`). **Behauptung bestaetigt.**
**Vorher/Nachher an derselben Stelle (eigener Lauf):** die exe mit den MASKS von ee7149ed (ROOM2040.MSK + _PRI07.TIM/.STAND ueber
RE15_CD_ROOT=m41abn1/vor_cd; Log `[pri] cut=7 ... masks=37`, nachher 33): Leon (-17500,-5600) gleiche 5439 Figurpunkte, uebermalt
VORHER 134 (83 innen, nur der Pfosten) -> NACHHER 321 (257 innen, Pfosten + Handlauf in Hueft-Hoehe), Bild
`bilder/c7_2040_vor_nach.png`. Der Fix erklaert den Befund (Handlauf-Tiefe vorher hinter Leon).

## Punkt 2 - OFFEN 8 (HINTER-Deckung der Nutzer-Masken), eigene Stichprobe an der exe
| Stelle (Liste des Bau-Agenten, offen gross) | Leon | A/B uebermalt | Sicht (Bild `bilder/o*.png`) |
|---|---|---|---|
| 11A0 C6 (-8375,350) Band 1 | 4850 | 54 (Schuhspitzen) | Leon NEBEN dem Block, nicht dahinter -> die 2798 offenen Punkte sind richtig offen |
| 20A0 C5 (20200,-12100) Band 2 | 2603 | 102 (Waffenhand hinten, Beinkante) | Leon auf dem Mauersims (Zellen Band 0/1 x 18550..20550); Maske 05_02 (Ebene 0) deckt die ferne Hand |
| 2030 C2 (1000,-9550) Band 3 | 7492 | 35 (Fuss) | Leon vorn, die Spinne hinter der Reling ist verdeckt |
Die grossen "offen"-Zahlen stammen aus Stellen, an denen Leon NICHT hinter dem Gegenstand steht (Standlinien-Modell). Zu 20A0 C5:
alle Messungen laufen mit RE15_FORCE_CUT. Im Gehen ueber den Sims (Start (20200,-15100) Band 2, RE15_INPUT_SCRIPT=U2, Gier 3072:
z -15025 -> -11101 ueber z -12100) blieb das Spiel durchgehend auf **Cut 0** (`[walk] F30..F150 cut=0`); Cut 5 zeigt Leon auf dem
Sims dort nicht - die 102 Punkte sind ein Artefakt der erzwungenen Kamera, kein Spielfehler. (Ein Stehen am Sprungort beweist das
nicht - die Kamera behaelt den Sprung-Cut, bis eine Zone ueberschritten wird; deshalb nur der Gehlauf als Beleg.)
Kein sichtbarer Fehler einer Nutzer-Maske im natuerlichen Spiel gefunden.

## RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, info/Re1.5/PSX.EXE)
Die NB1-Regel (tiefe_zelle_r41.py Kopf, Dossier) zitiert: bucket = (1023*vz)>>16 @0x8002565c, ZSF3 = 341 @0x80066c70, Masken-OT =
depth @0x80039650; Schluessel (depth+1)*65536/1023 = include/re15_pri.h (Altbestand 2026-09-19).
1. **@0x8002565c** - `80025640 .word 0x4b58002d` (cop2 0x158002D = AVSZ3) / `80025648 .word 0xe9870000` (swc2 $7 = OTZ, 0(t4);
   t4 = sp) / `8002564c lw v1,0(sp)` / `80025654 sra v0,v1,6` + `80025658 beq v0,zero` (otz < 64 verworfen) / **`8002565c sra v1,v1,4`**
   / `80025660 sll v1,v1,2` / `80025664 addu v1,s0,v1` = OT-Wort otz>>4. **stimmt.**
2. **@0x80066c70** - **`80066c70 addiu t0,zero,341`** / `80066c74 .word 0x48c8e800` (= ctc2 t0,$29 = ZSF3) / `80066c7c addiu t0,zero,256`
   / `80066c80 0x48c8f000` (ctc2 $30 = ZSF4). Mit AVSZ3: otz = 341*(SZ1+SZ2+SZ3)>>12 = (1023*vz)>>12, OT-Wort (1023*vz)>>16. **stimmt.**
3. **@0x80039650** - **`80039650 lh a0,2(s3)`** / `80039658 sll a0,a0,2` / `8003965c addu a0,a0,s6` / `80039660 jal 0x8006b538` (AddPrim)
   = Masken-Prim in OT-Wort depth; Aufruf des Masken-Zeichners `8001ce54 jal 0x80039590` (zuletzt im Bild). **stimmt.**
4. Byte-Belege der neuen auswahl.json-Werte (NB1 Mangel 4), eigener SCA-Leser (RDT+0x20, 12 B je Zelle):
   ROOM2040 Zelle 4 @RDT+0x008F8 `80 57 c2 01 b6 b8 9c e6 01 ff 00 33` = x -18250 w 22400 z -6500 d 450, Typ 1, u0 0xFF, Band 3
   (= "zelle" [-18250,-6500,22400,450]); ROOM2030 Zelle 21 @+0x00994 `da 2f f4 01 32 e7 d6 1f 01 ff 00 33` = x -6350 w 12250 z 8150
   d 500 (Nordgelaender), Zelle 19 @+0x0097C `f4 01 f2 44 18 15 d8 dc 01 ff 00 33` = x 5400 w 500 z -9000 d 17650 (Ostgelaender),
   Zelle 33 @+0x00A24 = x 5400..7750 z 12800..13300 (Pult 07_02). **Werte stimmen** (in auswahl.json ohne Byte-Offset zitiert, nur
   mit Koordinaten - nachpruefbar, kleiner Doku-Hinweis). Hoehen 1875/1800 = IoU-Messung gegen die Nutzer-Freistellung
   (raum.py "zelle"-Verfahren; das Original hat fuer diese Cuts NULL-Sektionen) - als Messwert der Port-Masken gekennzeichnet.
- Diff (re15_port + auswahl.json, merge-base..HEAD) auf "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|geschaetzt|vorerst":
  **0 Treffer**; kein neuer getenv; einziger Engine-Inhalt = erzeugte Tabelle gen/masken36_ausschluss_r37.inc (Riegel
  unit_r37_masken36_ausschluss rechnet sie aus den Dateien nach: Passed).
- Erklaert der Fix den Befund? Mangel 1: ja - die Rechtecke fielen weg, weil die Generator-Tiefe (Bodenkontakt, DEPTH_SAFETY 0.90) eine
  Figur VOR dem Objekt uebermalt haette; mit der Zellen-Tiefe (Eintritt des Sehstrahls in den Quader, max je Rechteck) verdecken sie
  nur noch Figurpunkte hinter dem Eintritt (eigene 3D-Pruefung 1a: 0 Faelle ohne Zelle davor). Mangel 4: ja - die Profil-Tiefe des
  Handlaufs ohne Bodenkontakt (391/392 in 2040 C7) lag hinter Leon; jetzt Zellen-Tiefe 38..41 / 170..178 (gemessen oben).

## Vertrag (VERTRAG.md §1.3/§2)
- `git diff master...HEAD --name-only` (597 Dateien): **keine** Aenderung unter release/, platform/android/, pri/, synchro/,
  shared_assets/PSX ausser MASKS/, keine CMakeLists (Test-Registrierung nur in tests/unit/probes/r41_masken36.cmake, per GLOB).
  NB1 (ee7149ed..HEAD) ausserhalb M_bilder/MASKS: Dossier, AUFTRAG/VERTRAG (Orchestrator-Kopie f1309cba), auswahl.json,
  gen/masken36_ausschluss_r37.inc, 6 Testdateien der Spur, test_r37_masken36.c (Werkzeug-Modus "plaetze", 16 Zeilen), 4 Werkzeuge.
- Keine Bank-9-Bits, Nachrichten-Ids, Slots oder Ereignisse belegt (M hat keine Zuteilung). Nutzer-PNG unter pri/ unveraendert
  (2030 07_01 nur ueber "minus" geteilt).
- Riegel je Mangel vorhanden und messend: Mangel 1 `integration_r41_masken36_zelle` (8 HINTER H >= Haelfte/H > 0, 8 DAVOR H == 0),
  Mangel 3 `v2030_15`/`v2031_15` (G= Spinne weg, H == 0) + Gegenprobe `m2030_15` (H > 0), Mangel 4 `m2030_07`/`m2040_07` (H > 0),
  Messwert `unit_r41_masken36_loecher` (<= 30 %). Parallel-fest: eigenes exe-Praefix je Riegel (r41m2/r41m3) und Laufordner.

## Urteile je Punkt (gegen den Wortlaut)
| Punkt | Urteil | Kern |
|---|---|---|
| 1 "Masken fuer STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" | **teilweise** | NB1 bestaetigt und fehlerfrei gemessen: 367 Rechtecke mit Zellen-Tiefe wieder drin (nur Tiefenfelder, 0 neu aus), 780 626 -> 889 591 gezeichnete Punkte, Kerben 18,4 %; eigene 3D-Pruefung aller 43 Cuts: 0 Verdeckungen ohne Gegenstand davor, eigene exe-Stellen DAVOR 0 / HINTER gedeckt. ABER weiter offen: 1186 ausgeschlossene Rechtecke (341 352 Punkte), 47 Objekte gar nicht und 99 teilweise gezeichnet, vier Cuts ganz ohne Maske (3040 C1, 3050 C6, 30E0 C6/C7), darunter echte Gegenstaende (3050 C6 Treppengelaender, 5020 C6 X-Kiste, 6000 C5 Lok/Traeger); der Bau-Agent fuehrt den Punkt selbst als TEILWEISE |
| 2 "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung) | **erfuellt** | Mangel 2 durch Nutzer-Entscheid erledigt (Nachtrag 11); Mangel 3 widerlegt und von mir nachgemessen (Spinne, Leon/Elza 0 uebermalt); Mangel 4 bestaetigt (keine VOR-Stelle, 3D-Pruefung) und der dabei gefundene Handlauf-Fehler 2030/2040 C7 behoben (Leon/Elza hinter dem Gelaender gedeckt, nur Cut 7 geaendert); Kurbel 2020 C3 aus Abnahme 0 weiter gruen; Stichprobe OFFEN 8 ohne Fehler im natuerlichen Spiel |

Gates: RE-Gate haelt (3 Adressen selbst disassembliert, Zellenwerte byte-belegt, keine Rate-Woerter), Vertrag haelt (Pfade,
Ressourcen, Riegel je Mangel, keine CMakeLists), gezielte Tests gruen (s. Tests).

## Maengel (nummeriert, nachpruefbar)
1. **Punkt 1 weiter nicht geschlossen** (Wortlaut "Masken schliessen"): nach NB1 sind 530 alte + 656 Saum-Rechtecke (Leon,
   Elza gleich) ausgeschlossen, 341 352 Atlaspunkte nur in ausgeschlossenen Rechtecken (`stat_nb1.py`), 11 494 Kerben offen
   (`unit_r41_masken36_loecher -V`); in 3040 C1, 3050 C6, 30E0 C6, 30E0 C7 zeichnet das Spiel gar keine Maske
   (gen/masken36_ausschluss_r37.inc: 19/19, 19/19, 35/35, 53/53). Echte Gegenstaende darunter (Bilder
   `M_bilder/NB1_rest/NB1_rest_ROOM3050.png` C6 Treppengelaender rot, `..._ROOM5020.png` C6 X-Kiste rechts rot - im neuen
   Uebersichtsbild `M_bilder/uebersicht/STAGE5/5020/06_port.png` fehlt sie, `..._ROOM6000.png` C5 Lok + Gittertraeger rot).
   Weg (nicht geraten): (a) die 135 Rechtecke, die trotz Zellen-Tiefe nur das Grundriss-Netz der Spur P nimmt, mit einer
   formgenauen 3D-Pruefung (Rechteck/Raute/Kreis/Dreieck wie hier 1c, aber mit der je Objekt gemessenen Hoehe statt pauschal
   2400) statt des Grundriss-Urteils entscheiden; eigene Vorpruefung (`rest135.py`, Zellen-Tiefen des Bau-Agenten aus
   seinem Lauf tz4b): von den 135 verdecken 36 an den Stichproben-Plaetzen nur Figurpunkte, vor denen eine Zellenform liegt (u.a.
   6000 C5 7, 5050 C1 9, 3040 C3 9 - mit pauschaler Hoehe 2400 und Zellen aller Baender, also nur ein Hinweis), 2 nicht
   (4040 C5 R9: 5 Plaetze, 5090 C13 R44: 6 Plaetze), 97 ohne Wirkung an den Stichproben-Plaetzen (davon 70 = 3050 C15, Original-
   NULL-Sektion ohne Stehplatz, zu Recht aus); (b) fuer die
   uebrigen echten Gegenstaende die Restliste dem Nutzer vorlegen (Freistellung je Kamera) - der Orchestrator entscheidet, ob
   Punkt 1 mit dieser Nutzerfrage als abgeschlossen gilt.

## Hinweise (keine Maengel der Spur)
- OFFEN 6 des Bau-Agenten (ROOM5100 C7 (-12030,-5300), Leons Hand unter dem ALTEN Rechteck 0) - selbst gemessen: 31 Leon-Punkte
  uebermalt (21 innen), alle unter Rechteck 0 (Tiefe 64, Schluessel 4164), Leon-Dreiecke dort z 4405..4467. Formgenauer Sehstrahl
  (`pixelstrahl.py`) durch (37,235)/(35,231)/(40,238): erster Eintritt in die Typ-1-Zelle Band 0 x -14000..-12600 z -23000..-3100
  bei vz 4160..4280 (Welt x -12600, y -190..-410), also VOR der Hand - die Hand ragt (Leon x -12030, Radius 450) in den Maschinen-
  block. Die Verdeckung ist geometrisch richtig, kein Fehler (`bilder/h5100_07_hand.png`).
- Orchestrator/Integration: `pri/STAGE3-6/<raum>/<cc>_port.png` erneut aus `M_bilder/uebersicht/` ersetzen - NB1 hat 53
  Uebersichtsbilder geaendert (`git diff --stat ee7149ed..HEAD -- analysis/befunde_runde41/M_bilder/uebersicht`), dazu die 10
  durchsichtigen (3040/3041 01, 3050/3051 06+15, 30E0/30E1 06+07; selbst geprueft: 320x240, Alpha 0, die Cuts zeichnen im Spiel 0
  Masken). Paket: MASKS/ROOM2030/2040 + _PRI07 neu (NB1), 30 Stage-3..6-MSK (nur Tiefen).
- OFFEN 7 des Bau-Agenten (Figurkasten des R37-Ausschlusses am Rand zu schmal, Altbestand, durch NB1 nicht veraendert) - selbst
  gemessen ROOM5050 C1 (-18100,-22950): 33 Leon-Punkte uebermalt (23 innen) = 1-2 Spalten an der Aussenkante des rechten Arms
  (Masken 35/36/64/72, Tiefe 68, Schluessel 4420 <= Leon z 4657..4679), `bilder/d5050_01_zoom.png`. Der formgenaue Strahl kreuzt die
  Zelle x -20825..-13325 z -26025..-23450 erst an der Oberkante des pauschalen 2400-Quaders (y -2392) - ob der Gegenstand dort so
  hoch ist, ist nicht belegt; sichtbar ist ein leicht angeschnittener Arm. Kein NB1-Fehler, aber ein offener Rest der STAGE3-6-
  Masken (Uebermalung statt Loch) - zur Abwaegung durch den Orchestrator.
- auswahl.json "zelle"-Werte (2030 C7, 2040 C7) tragen Koordinaten, aber keinen RDT-Offset - belegt hier: ROOM2040 Zelle 4
  @RDT+0x008F8, ROOM2030 Zelle 21 @+0x00994 / Zelle 19 @+0x0097C (s. RE-Gate 4).
- Beobachtung OFFEN 2 (Altbestand, nicht NB1): ROOM6000 C5 (FORCE_CUT, Annette-Text laeuft) - das 3D-Objekt oid 1
  (20916,0,-18540) wird von ALTEN Rechtecken 5/9/10/42.. (Tiefe 155..177) auf 755 Punkten uebermalt (`bilder/r6000_05.png`);
  ob es hinter der Lok steht, ist nicht gemessen.
- Alle Szenen (Bau-Agent und Abnahme) laufen mit RE15_FORCE_CUT; die HINTER-Stelle 20A0 C5 (20200,-12100) Band 2 zeigt das Spiel
  im Gehen mit Cut 0 (s. Punkt 2). Fuer kuenftige Schienen: Stelle nur werten, wenn der Cut dort im Gehen aktiv wird.
