# Spur M masken36 — Abnahme 2 (unabhaengig, 2026-10-24/25, nach Nachbesserung 2)

Baum `.claude/worktrees/r41_masken36` (Zweig r41/masken36, merge-base master 4c667c97, Kopf ff63ad75; Nachbesserung 2 =
6c9ca88f..ff63ad75, 34 Commits nach der Abnahme 1 c013e233). Pruefer: Abnahme 2. Massstab: Wortlaut AUFTRAG.md (Punkte 1/2,
Nachtrag 11), VERTRAG.md §1.3/§2, CLAUDE.md RE-Gate, Mangel 1 der Abnahme 1 (`M_abnahme_1.md`). Bericht wird fortlaufend
geschrieben (Sitzungslimit). Messordner (Scratchpad):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/m41abn2/`.

## Stand
- [x] Lektuere AUFTRAG (inkl. Nachtrag 11; im master-AUFTRAG bis Nachtrag 21/J-E1 kein neuer Entscheid zu Spur M), VERTRAG, Dossier (NB2-Abschnitte), Abnahme 1; `git log master..HEAD` (82), NB2-Diff `c013e233..HEAD` gelesen (Code: test_r37_masken36.c +311, zellen3d_r41.py neu, tiefe_zelle_r41.py +59, zuordnung_r41.json neu, gen_r41_masken36_zellen.inc neu, Engine-Tabelle, Riegel)
- [x] Bau: `local_build.sh configure` OK, `build` "ninja: no work to do" (exe 21:57:09 juenger als gen/masken36_ausschluss_r37.inc 21:56:39; `git status` sauber; Messkopie `exe/re15_pc_m41abn2.exe` md5 550c784e...)
- [x] Statische Gegenproben NB2 (eigener Leser `stat_nb2.py`, s.u.)
- [x] Gezielte Tests: unit 31/31, Integration 22/22 (s.u.)
- [x] Punkt 1 an der exe (11 eigene DAVOR-Stellen, 13 HINTER/Sonder-Stellen, 3 Elza, Gehlauf 30A0; Befund obere Trennwandfelder 30A0/30A1)
- [x] Punkt 2: NB2 hat keine STAGE1/2-Maske beruehrt; Riegel stage2 gruen
- [x] RE-Gate (s.u.)
- [x] Vertrag (s.u.)
- [x] Urteil: **nicht bestanden** (Punkt 1 teilweise, Punkt 2 erfuellt; RE-Gate, Vertrag und Tests halten)

## Messmittel (Scratchpad m41abn2)
- Werkzeuge der Abnahme 1 (`tri.py`, `ana1.py`, `maske.py`, `schluessel.py`, `run.sh`, `stat_nb1.py` -> `stat_nb2.py`) uebernommen,
  nur Pfade umgestellt; Messkopie der exe `exe/re15_pc_m41abn2.exe` (md5 550c784e2cc4f5537a9649603205d02c = Spurbaum-exe 21:57:09).
- Vorher-Stand NB2 = Abnahme-1-Commit c013e233: `vor_nb2/MASKS/*.MSK` und `vor_nb2/ausschluss_vor.inc` per `git show c013e233:...`.

## Statische Gegenprobe NB2 (eigener Leser `stat_nb2.py`, vor = c013e233, nach = ff63ad75)
- Ausschluss-Tabelle: 128 -> 126 Zeilen; **wieder aufgenommen 164 Rechtecke (Leon) + 164 (Elza), neu ausgeschlossen 0**;
  ausgeschlossene Leon-Rechtecke 1186 -> **1022**.
- MSK: nur ROOM3040/3041 (Cut 0), 30A0/30A1 (0, 1), 5020/5021 (0, 6), 5090/5091 (11, 13), 6000/6001 (5, 7, 8, 9) geaendert,
  **0 Bytes ausserhalb der Tiefenfelder**; Tiefen ferner 214 / naeher 114 (Leon+Elza = 107/57 je Datei, wie Dossier).
  Jedes der 164 wieder aufgenommenen Rechtecke hat eine neue Tiefe; kein Rechteck mit neuer Tiefe ist weiter ausgeschlossen, keines
  war vorher gezeichnet.
- Gezeichnete Generator-Punkte Leon Stage 3..6: **889 591 -> 956 982**, nur in ausgeschlossenen Rechtecken **341 352 -> 273 961** -
  Zahlen des Bau-Agenten bitgenau bestaetigt.

## RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)
Abnahme 1 hat @0x8002565c (`sra v1,v1,4`), @0x80066c70 (`addiu t0,zero,341` + ctc2 $29) und @0x80039650 (`lh a0,2(s3)` -> AddPrim)
schon geprueft; hier ANDERE zitierte Stellen der NB2-Begruendung:
1. **@0x8001ce54** (Dossier: "Masken zuletzt im Bild @0x8001ce54 (`jal 0x80039590`)"): `8001ce40 lw v0,-13768(v0)` / `8001ce48 and v0,v0,v1`
   (v1 = 0x100000) / `8001ce4c bne v0,zero,0x8001ce5c` / **`8001ce54 jal 0x80039590`**; Sprungziel `80039590 lui v0,0x800b / lw v0,-14472(v0)
   / addiu sp,sp,-72 ...` = Prolog des Masken-Zeichners. **stimmt.**
2. **@0x80039324/@0x80039338** (Dossier: "Stages 3..6 haben fuer diese Cuts KEINE Masken (NULL-Sektion)"): `8003931c lw t5,28(v0)`
   (v0 = Kamerasatz cut*32, `80039314 sll v0,v0,5`) / **`80039324 lw v1,0(t5)`** / `80039328 addiu v0,zero,-1` / `8003932c bne v1,v0,...`
   / `80039334 j 0x8003955c` / **`80039338 sb zero,0(a0)`** = erstes Wort der PRI-Sektion 0xFFFFFFFF -> 0 Masken. **stimmt.** Dazu die
   Daten selbst gelesen (RDT, Kamerasatz +28 -> PRI-Offset, erstes Wort): ROOM3040 C0 @0x00784, 30A0 C0 @0x003C0 / C1 @0x003C4,
   5020 C0 @0x0075C / C6 @0x00774, 5090 C11 @0x00808 / C13 @0x00810, 6000 C5 @0x006EC / C7 @0x006F4 / C8 @0x006F8 / C9 @0x006FC:
   **alle `ff ff ff ff`** - das Original zeichnet in allen 11 durch NB2 geaenderten Cuts keine Maske; die Generator-Masken sind
   PORT-WAHL (seit R37), Massstab "keine Figur VOR dem Gegenstand uebermalen, dahinter verdecken".
3. **DAT_80073e94 (+6) = Spielerradius 450** (Dossier "Spielerradius 450 (DAT_80073e94[6])"): `read 0x80073e94 8 --w 2 --signed` ->
   `[0, -1530, 0, 450, 1530, 450, ...]` = +6 radius 450, +8 Hoehe 1530, +10 radius 450 (= RE15_FUN_CATALOG Zeile 66). **stimmt.**
   FUN_8003b0a4 (Form der Zellen "wie der Resolver"): Prolog `8003b0a4 addiu sp,sp,-72 ...`, Katalog Zeile 97 "SCA push-out resolver",
   Typ-Dispatch DAT_800b2858; der Port-Riegel benutzt dessen Port-Formen (form_drin, Radius 0) - Altbestand R37, unveraendert.
- Diff NB2 (`c013e233..HEAD`, re15_port + Dossier) auf "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|geschaetzt|vorerst|
  ungefaehr|sieht richtig": **0 Treffer**; **0 neue getenv**. Engine-Inhalt nur die erzeugte Tabelle gen/masken36_ausschluss_r37.inc
  (Riegel unit_r37_masken36_ausschluss rechnet sie nach). Hoehen der Koerper = IoU-Messung (im Kopf von zellen3d_r41.py /
  test_r37_masken36.c als Messung, nicht als Spielkonstante gekennzeichnet); Zellbytes je Modellzeile mit RDT-Offset (Riegel
  unit_r41_masken36_zellen3d prueft sie byte-genau).

## Tests (gezielt, Spurbaum)
1. `ctest -R "unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_r38_itemorte|unit_r39_optik|unit_r34n_e_dokumente|unit_bss|prop_bss|unit_stage6_hintergrund" -j 3 --timeout 1500`
   -> **`100% tests passed, 0 tests failed out of 31`** (unit_r37_masken36_ausschluss 519 s, unit_r41_masken36_zellen3d 119 s; Protokoll
   m41abn2/ctest_unit.log).
   Werte selbst nachgelesen (Test-Binaries direkt, m41abn2/zellen3d_V.log, loecher_V.log): zellen3d "Beleg im RDT 530/530 | MSK-Tiefe ==
   Zeile (Leon+Elza) 1060/1060 | Engine-Tabelle zeichnet 1060/1060 | Cuts 46, davon Ausschluss == Tabelle 46 | nur durch das 3D-Urteil
   gezeichnet 79 (mit Modell-Zeile 79)" - die 79 sind 6000 C5 26 / C7 12 / C8 24 / C9 14 und 5090 C11 3; die uebrigen 85 NB2-Rechtecke
   (30A0 C0/C1, 5020 C0/C6, 5090 C13, 3040 C0) traegt schon das alte Grundriss-Netz mit der neuen Zellen-Tiefe. loecher: Kerben VORHER
   63 546 -> NACHHER 10 178 (16,0 %), Blit max Mittel 10,66.
2. `ctest -R "integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r35_zgirl_5090|integration_r38_affen3|integration_r35_granate|integration_r37_elzaems_elza5091" -j 2 --timeout 1800`
   -> **`100% tests passed, 0 tests failed out of 22`** (Protokoll m41abn2/ctest_integ.log): die Spur-Riegel zellen3d (417 s), zelle (483 s),
   stage2 (532 s), alle R37-Masken-Riegel (stage3..6, nb2), PRI-/Optik-/Itemorte-/Dokument-Riegel und die Verhaltens-Riegel der durch NB2
   geaenderten Raeume (3040: affen3 x3; 5090/5091: zgirl_5090, granate, elzaems_elza5091). Kein roter Fenster-Haken, keine Wiederholung.

## Punkt 1 - eigene Pruefung der 164 wieder aufgenommenen Rechtecke
### 1a Eigene 3D-Suche (`scan_nb2.py`, unabhaengig vom C-Code des Generators)
Figur GROSSZUEGIGER als der Generator (Zylinder Radius 450 = Spielerradius, Hoehe 50..1800, 12 Richtungen x 3 Radien + Achse statt
Kasten +-350 quer zur Kamera), Plaetze = `test_r37_masken36 plaetze` (jeder 2.), eigene Kamera/SCA (Abnahme-1-Leser), nur Figurpunkte, die
ein NB2-Rechteck als VORDERSTE gezeichnete Maske verdeckt (Schluessel <= vz). (a) = ohne den Huellkoerper des eigenen Zellen-Modells
davor, (b) = ohne irgendeine solide Zelle (alle Baender, Huellquader bis 2400) davor.
| Cut | NB2-Rechtecke | Plaetze im Bild | mit NB2-Verdeckung | (a) ohne eigenen Koerper | (b) ohne jede Zelle |
|---|---|---|---|---|---|
| 3040 C0 | 3 | 586 | 0 | 0 | 0 |
| 30A0 C0 | 22 | 834 | 65 | 26 (R0, Kopfzeile ueber der gemessenen Hoehe 1525) | **0** |
| 30A0 C1 | 6 | 681 | 44 | 0 | 0 |
| 5020 C0 | 24 | 1503 | 42 | 0 | 0 |
| 5020 C6 | 20 | 876 | 4 | 0 | 0 |
| 5090 C11 | 3 | 757 | 42 | 0 | 0 |
| 5090 C13 | 10 | 864 | 46 | 9 (R44) | **0** |
| 6000 C5 | 26 | 3948 | 1006 | 0 | 0 |
| 6000 C7 | 12 | 2856 | 532 | 39 (R28) | **0** |
| 6000 C8 | 24 | 3353 | 674 | 79 (R93, R15) | **0** |
| 6000 C9 | 14 | 5449 | 514 | 0 | 0 |
Keine NB2-Verdeckung ohne irgendeine Zelle davor. Die (a)-Faelle traegt das alte Grundriss-Netz (eine solide Zelle des Bands im
Grundriss dazwischen, ohne Hoehe) - an der exe nachgesehen (1b).

### 1b exe-Stellen (eigene Plaetze, nicht die Szenen des Bau-Agenten; Bild 120, A/B + RE15_TRILOG, Schluessel wie der Lader)
Laeufer `run.sh` (RE15_DEBUG_JUMP + RE15_PLAYER_POS + RE15_FORCE_CUT, Messkopie md5 550c784e), Auswertung `ana2.py`: VORHER = MSK/Tabelle
von c013e233 auf denselben Dreiecken, NACHHER = Spurbaum; "uebermalt" = |A-B| > 24 auf Leon (Slot 0, vorderstes Dreieck);
3D-Gegenprobe nur fuer uebermalte Punkte mit Schluessel <= Leon-z.
| Szene | Platz (Band) | Leon | verdeckt vorher -> nachher | uebermalt (innen / ohne Schluessel) | davon NB2 | NB2 ohne eigenen Koerper / ohne jede Zelle |
|---|---|---|---|---|---|---|
| d30A0_00 DAVOR | (-21943,13663) B0 | 2363 | 0 -> 0 | 1 (1 / 1 = Rasterkante) | 0 | - |
| d30A0_01 DAVOR | (-21343,13663) B0 | 6521 | 0 -> 0 | 4 (0 / 4 = Rasterkante) | 0 mit Schluessel | - |
| d5020_00 DAVOR | (-11933,-18925) B0 | 1929 | 0 -> 0 | 0 | 0 | - |
| d5020_06 DAVOR | (-1533,-14525) B0 | 3153 | 0 -> 0 | 0 | 0 | - |
| d5090_11 DAVOR | (3617,-25365) B0 | 5013 | 115 -> 115 | 110 (99 / 1) | **0** | - (Altbestand R37, s. Hinweise) |
| d5090_13 DAVOR | (20817,-25965) B0 | 1294 | 0 -> 0 | 1 (0 / 1) | 0 mit Schluessel | - |
| d6000_05 DAVOR | (12233,-9984) B2 | 2500 | 0 -> 0 | 0 | 0 | - |
| d6000_07 DAVOR | (18233,-21184) B2 | 1900 | 0 -> 0 | 0 | 0 | - |
| d6000_08 DAVOR | (12833,-18784) B2 | 1583 | 0 -> 0 | 0 | 0 | - |
| d6000_09 DAVOR | (8033,-18784) B2 | 755 | 0 -> 0 | 3 (0 / 3) | 0 mit Schluessel | - |
| d3040_00 DAVOR | (-24400,-4800) B0 | 7088 | 0 -> 0 | 0 | 0 | - |
| h30A0_00 HINTER | (-22343,16063) B0 | 1719 | 38 -> 674 | 555 | 535 (R0) | 11 / 0 |
| s30A0_00 HINTER | (-21743,16263) B0 | 1979 | 15 -> 733 | 644 | 601 (R0) | 56 / 0 |
| h30A0_01 HINTER | (-19143,15863) B0 | 1767 | 33 -> 744 | 601 | 575 (R0,1,3) | 0 / 0 |
| h5020_00 HINTER | (-13533,-21725) B0 | 868 | 0 -> 697 | 564 | 564 | 0 / (73 nur ueber 2400) |
| h5020_06 HINTER | (-4133,-15925) B0 | 1354 | 0 -> 17 | 17 | 17 (R44,46) | 0 / 0 |
| h5090_11 HINTER | (2417,-25765) B0 | 2241 | 0 -> 0 | 0 | 0 | - (andere Figur 1210) |
| h5090_13 HINTER | (24817,-25965) B0 | 659 | 0 -> 659 | 628 | 628 | 0 / (108 nur ueber 2400) |
| s5090_13 HINTER | (25017,-20365) B0 | 468 | 0 -> 67 | 62 | 62 (R44) | 5 / 0 |
| h6000_05 HINTER | (22233,-17184) B0 | 111 | 1 -> 111 | 106 | 106 | 0 / 0 |
| h6000_07 / s6000_07 | (16033,-5784) / (21833,-19984) B0 | 366 / 949 | 5 -> 264 / 360 -> 408 | 216 / 381 | 211 / 48 | 0 / 0 |
| h6000_08 / s6000_08 | (22233,-7784) / (19833,-14184) B0 | 186 / 266 | 20 -> 186 / 0 -> 34 | 166 / 31 | 150 / 31 | 0 / 0 ; 9 / 0 |
| h6000_09 HINTER | (22033,-14584) B0 | 37 | 0 -> 37 | 21 | 21 | 0 / 0 |
**DAVOR: an 11 eigenen Stellen 0 Leon-Punkte von einem NB2-Rechteck mit Schluessel uebermalt** (die 1..4 Punkte ohne Schluessel sind
Rasterkanten des eigenen Rasterers). Die "ohne eigenen Koerper"-Punkte (`warum_px.py`): der Sehstrahl kreuzt den Grundriss der
EIGENEN Zelle oberhalb der IoU-Hoehe (30A0 C0 R0: Zelle 6 bei 1930..1953 ueber dem Boden, Rahmenpfosten am Trennwandende; 5090 C13
R44: Zelle 27 bei 1637..1645, Kistenstapel zweilagig) - der Gegenstand steht dort vor Leon, die Verdeckung ist richtig.
6000: alle HINTER-Plaetze liegen auf Band 0 (Gleisbett, OFFEN 10 - im Spiel vermutlich nicht begehbar); im Spiel wirkt die Lok-Maske
auf das 3D-Tor (d6000_05: 2529 Tor-Punkte jetzt hinter der Lokfront, vorher davor; Bild `bilder/d6000_05.png`).

Elza (RE15_TITLE_SHOT_CHAR=1, Raum xxx1 im Log): e30A1_00 HINTER (-21743,16263) 0 -> 607 (R0), e5021_06 DAVOR (-1533,-14525) 0 / 0,
e5021_00 HINTER (-13533,-21725) 0 -> 465 - wie Leon (MSK bytegleich).

### 1c Natuerlicher Cut (Gehlauf statt FORCE_CUT) und der Befund ROOM30A0/30A1 obere Trennwandfelder
`walk.sh w30A0b`: RE15_DEBUG_JUMP=30A0, Start (-18800,16300) Gier 2048, RE15_INPUT_SCRIPT=U6, RE15_CAM_TRACE=1: Leon geht hinter der
Buero-Trennwand (Zelle 6 @0x0045C, x -26525..-19975 z 14125..15375) nach Westen, F1 (-18800,16300) -> F60 (-21050,16300) -> F120
(-25347,16499) -> F180 (-26046,16832), **`shown(s_last)=0` durchgehend** - Cut 0 ist dort der Spiel-Cut; die HINTER-Szenen
h30A0_00/s30A0_00 sind Spielstellen. Bildbogen `bilder/walk_w30A0b.png` (Bilder 40..90).
Zwei weitere Gehlaeufe: `w30A0c` Start (-19143,11500) Gier 3072, U5: (-19143,13750) -> (-19028,17757) an der Ostkante der Trennwand
vorbei, **Cut 0 durchgehend**; `w30A0d` Start (-24000,11000): Cut 0 bis (-21060,11501), Cut 1 erst VORN bei (-21000,11457) ->
(-20168,9697). Cut 1 zeigt die Stelle HINTER der Trennwand in meinen Laeufen nicht - die C1-Szene h30A0_01 ist erzwungen (FORCE_CUT),
der Spiel-Fall ist Cut 0.
**Befund:** hinter der Trennwand ist die Figur seit NB2 UNTEN verdeckt (unteres Feld R0, Zellen-Tiefe 110 der Zelle 6), OBEN aber ueber dem
oberen Feld der Trennwand gezeichnet - sichtbar als Halbfigur, abgeschnitten an der Mittelleiste (Bild `bilder/M2_30A0_oberes_feld.png`:
s30A0_00 Leon 644 Punkte unten verdeckt, **1072 Punkte ueber dem oberen Feld**; h30A0_01 (Cut 1) 601 / **988**; Elza e30A1_00 548 / **704**).
Ursache (`unter.py`, `scan_fern.py`, Verlauf `git show <c>:...ROOM30A0.MSK`): das obere Feld ist Rechteck 4, im master/R40 Tiefe 83 und
AUSGESCHLOSSEN, seit NB1 (c013e233) gezeichnet mit der Tiefe einer Zelle HINTER der Trennwand - C0 R4 Tiefe 200 = Zelle 8 @0x00474
(x -28575..-19375 z 18225..20675, Rueckwand-Schraenke), C1 R4 Tiefe 207 = Zelle 1 @0x00420 (x -29418..-15068 z 20303..21933, Nordwand);
Schluessel 12877/13325 > Leon-z 6766..7910, die Maske verdeckt also niemanden zwischen Trennwand und Wand. 62 % (C0) / 60 % (C1) der
Sehstrahlen dieser Rechteckpunkte treten VORHER in den Grundriss der Trennwand-Zelle 6 ein (Abstand 5917/5605 gegen 10557/11789). Das
obere Feld (Mattglas mit angehefteten Zetteln/Plakat, im Hintergrund deckend) gehoert zur Trennwand - dieselbe Zelle 6, die NB2 von Hand
fuer die unteren Felder gewaehlt hat. Gleiches Muster C0 R19 (Tiefe 188 = Zelle 0 @0x00414, 62 % der Strahlen vorher in Zelle 6).
Die Zaehlung "gezeichnete Punkte" und die Uebersichtsbilder zeigen diese Felder als geschlossen; im Spiel verdecken sie nichts.
Der Ausschluss-Riegel und die exe-Riegel pruefen nur Uebermalung (Maske VOR der Figur ohne Gegenstand); eine zu FERNE Tiefe (Maske
HINTER der Figur, obwohl der Gegenstand davor steht) prueft kein Riegel. Die NB2-Aussage "HINTER ist Leon verdeckt" (h30A0_00
42 -> 757 von 2010 Punkten) verschweigt die obere Haelfte.
Zwei weitere Spielstellen auf dem Gehweg (Cut 0): t30A0_00 (-24300,16400) unten verdeckt 269 (R0, R22), **ueber den oberen Feldern 858**
(R4 631, R19 200, R5 27) - Leon steht scheinbar VOR der mittleren Wand und verliert nur die Fuesse (`bilder/t30A0_00.png`); u30A0_00
(-23000,16300) 477 / **986**. Das Uebersichtsbild `M_bilder/uebersicht/STAGE3/30A0/00_port.png` zeigt die ganze Trennwand (oben und unten)
als geschlossen (`bilder/uebersicht_5020_06_30A0_00.png`) - das Bild, an dem der Nutzer "Loecher" beurteilt, verspricht hier mehr, als das
Spiel zeigt. (Positiv im selben Bogen: 5020 C6 zeigt die X-Kiste jetzt.)

### 1d Was weiter offen ist (Bau-Agent selbst, nachgesehen)
- 1022 Rechtecke (Leon, Elza gleich) aus, 273 961 Punkte nur in ausgeschlossenen Rechtecken (`stat_nb2.py` bitgleich); Kerben 10 178
  (16,0 %). Echte Gegenstaende ohne Maske laut Restliste (`M_bilder/NB2_rest/`): 30A0 C3 Spinde (~10 000 px), 6000 C5 unterer
  Gittertraeger / C7 Lok-Unterteil, 5050 C1/C2 Schreibtisch und Pulte, 5090 C13 oberer Kistenstapel, 3040 C0/C9, 50B0 C1/C3 Roboterarm,
  5020 C7/C12, Gelaender 3050 C6 / 30E0 C6..C9 / 3040 C3 (`NB2_vier_cuts_region.png` selbst angesehen: die Regionen der vier
  maskenlosen Cuts sind Boden bzw. Rueckwand, die Gelaender liegen nicht darin - Aussage stimmt). OFFEN 9: 105 Modell-Rechtecke aus,
  weil die Region ueber den Koerper hinausreicht; der benannte Weg (Atlaspunkte ausserhalb des Koerpers durchsichtig / Rechteck teilen)
  ist nicht gegangen. Ein Orchestrator-Entscheid, dass Punkt 1 mit der Restliste an den Nutzer als geschlossen gilt, liegt nicht vor
  (master-AUFTRAG bis J-E1 23:10 gelesen).

## Punkt 2 - die 53 Nutzer-Masken 11A0/STAGE2 (Sichtpruefung)
NB2 hat keine STAGE1/2-Maske beruehrt: `git diff --name-only c013e233..HEAD -- re15_port/shared_assets/PSX/MASKS/` = nur ROOM3040/3041,
30A0/30A1, 5020/5021, 5090/5091, 6000/6001 (0 Dateien ROOM1xxx/2xxx); pri/ unveraendert (`git diff --stat master...HEAD -- pri/` leer).
Der Stand der Abnahme 1 (Punkt 2 erfuellt: 2020 C3 Kurbel, 2030/2040 C7 Gelaender, 2030 C15 Spinne, 20A0 C12 nach Nachtrag 11) gilt
unveraendert; der Spur-Riegel `integration_r41_masken36_stage2` (13 Szenen Leon/Elza) ist in meinem Lauf gruen (532 s).

## Vertrag (VERTRAG.md §1.3/§2)
- `git diff --name-only master...HEAD` (652 Dateien): keine Aenderung unter release/, platform/android/, pri/, synchro/, keine
  CMakeLists/RELEASE_NOTES, shared_assets nur PSX/MASKS/; einziger Engine-Inhalt der ganzen Spur = gen/masken36_ausschluss_r37.inc.
- NB2 ausserhalb M_bilder/MASKS: Dossier, Engine-Tabelle, tests/unit/{test_r37_masken36.c, test_r41_masken36.c (Messwert),
  gen_r41_masken36_zellen.inc, probes/r41_masken36.cmake}, tests/integration/test_r41_masken36_zellen3d_szenen.cmake,
  tools/maske/{tiefe_zelle_r41.py, zellen3d_r41.py, zuordnung_r41.json} - alles Spur-Dateien; Test-Registrierung nur in der
  Probe-Datei der Spur. Keine Bank-9-Bits, Nachrichten-Ids, Slots, Ereignisse (M hat keine Zuteilung).
- Riegel je NB2-Aenderung vorhanden und messend: `unit_r41_masken36_zellen3d` (Beleg 530/530, MSK-Tiefe, Engine, Gegenprobe 79),
  `integration_r41_masken36_zellen3d` (14 Szenen DAVOR H == 0 / HINTER), `unit_r37_masken36_ausschluss` (Nachrechnung mit 3D-Urteil),
  `unit_r41_masken36_loecher` (Messwert 16,0 %). Parallel-fest: eigenes exe-Praefix r41m4, eigener Laufordner.
- Luecke der Riegel (gehoert zu Mangel 1): kein Riegel misst eine zu FERNE Tiefe (Maske hinter der Figur, obwohl der Gegenstand davor
  steht) - genau der Fall 30A0 C0/C1.

## Urteile je Punkt (gegen den Wortlaut)
| Punkt | Urteil | Kern |
|---|---|---|
| 1 "Masken fuer STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" | **teilweise** | NB2 bitgenau bestaetigt (164 Rechtecke wieder drin, 0 neu aus, nur Tiefenfelder, 889 591 -> 956 982 Punkte, Kerben 16,0 %); eigene 3D-Suche mit breiterer Figur: keine NB2-Verdeckung ohne Zelle davor; exe: 11 eigene DAVOR-Stellen 0 NB2-Uebermalung, HINTER neu verdeckt (30A0 C0 15 -> 733, 5020 C0 0 -> 697, 5090 C13 0 -> 659, 6000 C5 1 -> 111, Elza gleich); X-Kiste 5020 C6 und Lok 6000 gezeichnet. ABER: (a) 30A0/30A1 C0 (C1 gleiches Muster, nur erzwungen) - die oberen Trennwandfelder tragen seit NB1 die Tiefe der Wand HINTER der Trennwand; hinter der Trennwand (Spiel-Cut 0, im Gehlauf belegt) erscheint Leon/Elza als Halbfigur ueber dem oberen Feld (858..1072 Punkte), waehrend NB2 die Beine verdeckt; (b) weiter 1022 Rechtecke / 273 961 Punkte aus, echte Gegenstaende ohne Maske (Restliste), OFFEN 9 nicht gegangen; kein Entscheid, dass die Restliste den Punkt schliesst |
| 2 "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung) | **erfuellt** | unveraendert seit Abnahme 1 (NB2 beruehrt keine STAGE1/2-Maske), Riegel integration_r41_masken36_stage2 gruen |

## Maengel (nummeriert, nachpruefbar)
1. **ROOM30A0/30A1 Cut 0 (C1 gleiches Muster): obere Trennwandfelder verdecken nichts - Figur hinter der Trennwand als Halbfigur.** C0 R4 Tiefe 200
   (Modell = Zelle 8 @RDT+0x00474, Rueckwand-Schraenke x -28575..-19375 z 18225..20675), C0 R5 Tiefe 199 (dto.), C0 R19 Tiefe 188
   (Zelle 0 @0x00414, x -28804..-26514 z 7463..16733), C1 R4 Tiefe 207 (Zelle 1 @0x00420, Nordwand x -29418..-15068 z 20303..21933);
   im master waren R4 (Tiefe 83/71) ausgeschlossen, NB1 hat sie mit diesen fernen Zellen-Tiefen wieder aufgenommen. 60..62 % ihrer
   Sehstrahlen betreten vorher die Trennwand-Zelle 6 @0x0045C (die Zelle, die NB2 von Hand fuer die unteren Felder nimmt). Gemessen
   (Spielstellen, Cut 0 im Gehlauf `walk_w30A0b` durchgehend): s30A0_00 (-21743,16263) unten verdeckt 644 / ueber dem oberen Feld 1072,
   u30A0_00 (-23000,16300) 477 / 986, t30A0_00 (-24300,16400) 269 / 858, Elza e30A1_00 548 / 704 (dazu erzwungen Cut 1 h30A0_01
   (-19143,15863) 601 / 988 - im Gehlauf an dieser Stelle Cut 0) (Bilder `m41abn2/bilder/M2_30A0_oberes_feld.png`, `t30A0_00.png`, `walk_w30A0b.png`). Das Uebersichtsbild 30A0/00_port.png zeigt
   die Felder als geschlossen. Erwartet: oberes Feld = Gegenstand Trennwand (Zuordnung zu Zelle 6, Hoehe ueber oberes + unteres Feld
   gemessen; die Nahfeld-Sperre greift nicht: Kamera C0 (-16896,-3600,9440), C1 (-24744,-3300,9840), beide ausserhalb von Zelle 6), danach an denselben Stellen "ueber dem oberen Feld" == 0 (ausser Kopf ueber der
   Trennwandoberkante) und DAVOR (Leon vor der Trennwand, z. B. d30A0_00 (-21943,13663)) weiter 0; Riegel dazu, der die zu FERNE Tiefe
   misst (sichtbare Figurpunkte unter einer gezeichneten Maske, deren Sehstrahl vorher einen soliden Gegenstand kreuzt). Die NB2-Aussage
   "HINTER ist Leon verdeckt" (30A0) im Dossier richtigstellen - das eigene Bild des Bau-Agenten
   `M_bilder/NB2/NB2_k2_h30A0_01_g0_0.png` zeigt die Halbfigur bereits (Beine rot verdeckt, Oberkoerper ueber dem oberen Feld, der
   angeheftete gelbe Zettel liegt VOR Leons Huefte, das Feld dahinter nicht). Gleiches Muster systematisch pruefen: `m41abn2/scan_fern.py` meldet 127
   Modell-Rechtecke (9 NB2), deren Strahlen zu >= 50 % vorher eine andere solide Zelle betreten (Verdachtsliste
   `m41abn2/scan_fern_alle.txt`; nur 30A0 an der exe bestaetigt, viele Eintraege mit Abstand 0 = Kamera ueber der Zelle sind Fehlalarme).
2. **Punkt 1 weiter nicht geschlossen (Restbestand, vom Bau-Agenten selbst ausgewiesen):** 1022 ausgeschlossene Rechtecke (Leon, Elza
   gleich), 273 961 Atlaspunkte nur in ausgeschlossenen Rechtecken, echte Gegenstaende ohne Maske (Restliste `M_bilder/NB2_rest/`:
   30A0 C3 Spinde, 6000 C5/C7 Traeger/Lok-Unterteil, 5050 C1/C2, 5090 C13 oben, 3040 C0/C9, 50B0 C1/C3, 5020 C7/C12, Gelaender 3050 C6 /
   30E0 C6..C9 / 3040 C3); OFFEN 9 (105 Modell-Rechtecke, Weg "Atlaspunkte ausserhalb des Koerpers durchsichtig / Rechteck teilen"
   benannt, nicht gegangen). Ob die Restliste an den Nutzer den Punkt schliesst, entscheidet der Orchestrator - bis dahin teilweise.

## Hinweise (keine Maengel der NB2)
- d5090_11 (3617,-25365) Cut 11: 109 Leon-Punkte (Oberschenkel, Bild x 75..85 y 179..236) von ALTEN Generator-Rechtecken 2/3/4/6/83/93/104
  (Tiefe 58, Schluessel 3780 <= Leon-z 3795..4166) uebermalt; Leon steht im Huellrechteck der Typ-6-Dreieckszelle 30 @0x009E0
  (x -425..5300 z -27200..-25200) an deren Schraegkante - Randfall wie OFFEN 7 (Figurkasten +-350), vor und nach NB2 gleich
  (`bilder/d5090_11_zoom.png`).
- 5090 C11: Feuer-Sprites (Slot 50) und der NPC (Slot 11) werden von ALTEN Rechtecken (15, 2, 17, 3, ...) auf 468..623 Punkten
  uebermalt, mit den MSK vor NB2 identisch; ob das Feuer dort hinter der Kiste brennt, ist nicht geprueft.
- 6000: alle HINTER-Stellen (Band 0, Gleisbett) sind vermutlich keine Spielstellen (OFFEN 10); sichtbare Wirkung der Lok-Masken im Spiel =
  das 3D-Tor (Obj_model_set[1] (20916,0,-18540)) liegt jetzt hinter der Lokfront (d6000_05: 2529 Tor-Punkte), geometrisch richtig.
- Orchestrator/Integration: die 11 durch NB2 geaenderten Uebersichtsbilder nach pri/STAGE3-6 uebernehmen (Liste im Dossier OFFEN 5) -
  nach Behebung von Mangel 1 erneut fuer 30A0/30A1.
- Kleinigkeit Doku: `tests/unit/probes/r41_masken36.cmake` Zeile 7 nennt beim Riegel unit_r41_masken36_loecher noch "gemessen 18,4 %" (NB1);
  der Messwert nach NB2 ist 16,0 % (Testtext in test_r41_masken36.c ist nachgezogen).

## Gates
- RE-Gate: haelt (3 zitierte Stellen selbst disassembliert, NULL-Sektionen der 11 Cuts byte-gelesen, Zellbytes der Zuordnungen von Hand
  selbst gelesen: 5020 @0x00870/@0x007F8, 5090 @0x009B0/@0x009BC, 30A0 @0x0045C, 3040 @0x00870 = Eintraege 12/2/26/27/6/8, Bytes wie
  zuordnung_r41.json; keine Rate-Woerter, keine neuen Schalter).
- Vertrag: haelt.
- Tests: unit 31/31, Integration 22/22.
- Fix erklaert den Befund der Abnahme 1? Teilweise: Weg (a) ist gegangen (79 Rechtecke nur durch das 3D-Urteil, 85 ueber Zuordnung von
  Hand + Grundriss-Netz), die X-Kiste und die Lok sind gezeichnet; die Restliste (b) liegt vor. Dass "gezeichnet" nicht "verdeckt" heisst
  (zu ferne Tiefe, Mangel 1), misst weder die Zaehlung noch ein Riegel.

**Ergebnis: nicht bestanden** - Punkt 1 teilweise (Maengel 1 und 2), Punkt 2 erfuellt.
