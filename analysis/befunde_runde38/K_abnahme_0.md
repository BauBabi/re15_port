# Runde 38 — Spur K "itemorte": unabhaengige Abnahme 0

Pruefer: Abnahme-Agent 0 (2026-10-16, Beginn ~21:55). Baum `.claude/worktrees/r38_itemorte`, Zweig `r38/itemorte`,
HEAD 4fce8e62, Merge-Basis 1bcbd0ff. Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).

**Ergebnis: NICHT BESTANDEN.** Punkt 1 (Feuerloescher ROOM1030) ist an der exe gemessen erfuellt. Punkt 2 ist
teilweise erfuellt: beide Schluessel haengen an der Nutzer-Marke und sind aufnehmbar, der Parking Lot Key ist gut
sichtbar, der Prison Key (Elza) aber nur ein duenner dunkler Ring (14 Pixel bei Skala 1, 112 bei Skala 3 gegen 55/491
beim Parking Lot Key) — Mangel M1 (Abschnitt 5). RE-Gate und Vertrag halten; gezielter ctest-Lauf 117/117 gruen.

## 0. Werkzeug und Bau

* Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do`, `LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 21:23; keine Quelle unter
  engine/include/platform/pc neuer als die exe (`find -newer` leer), `git status` leer; die neuen Logzeilen
  (`[itemorte] ROOM%04X: Slot %d (msg %d) -> Slot %d ...`, `[schluessel] ... rot (%d,%d,%d), Zone Slot %d`) stehen in der exe.
* Messkopie `re15_pc_r38k_abn0.exe` (eigener Name neben der exe, Asset-Wurzel = exe-Verzeichnis; nie `taskkill /IM`;
  nach der Abnahme geloescht).
  Spielstand mit dem Kartenwerkzeug `probe_r37_feuerloescher_karte` (Quelle gelesen: Lage/Flags/Inventar/Cut/Elza),
  Start `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1`,
  Eingabe `RE15_INPUT_SCRIPT` (Basis Spielbild, Start 60), Bilder `RE15_FRAMEDUMP`, Kamera `RE15_CAM_TRACE=1`.
  Lauf-Skript `lauf.sh` und eigenes Differenz-Werkzeug `diff.py` (|dR|+|dG|+|dB| > 30, NICHT das Werkzeug des
  Bau-Agenten) im Scratchpad der Abnahme (`r38k_abn0/`). Belegbilder (vergroessert) unter
  `re15_port/build/abnahme_k0/` (git-ignoriert, nicht committet).

## 1. Punkt 1 — "fire extinguisher ... in room 1030 haengen an der stelle von fire_extinguisher.bmp, anstatt in room 1050"

Nutzerbild selbst ausgewertet: rote Marke = 132 Pixel, x 38..48 y 111..122 (reines Rot). Hintergrund = ROOM1030 Cut 0
(Lobby, Rolltor; Pfeiler links unter dem Notruf-Monitor) — Augenvergleich mit dem Spielbild (p1, unten) deckungsgleich.

| Lauf | Befehl (lauf.sh) | Ergebnis |
|---|---|---|
| p1_mit / p1_ohne | `1030 -11500 -19500 2048 cut:0` [+ `flag:9:0x86 flag:3:0x85`], Bild 100 | Log: `[itemorte] ROOM1030: Slot 14 (msg 14) -> Slot 20`, `[feuerloescher] ROOM1030: Loescher an der Wand obj 5 bei (-2813,-2122,-17904) rot_y 3072, Zone Slot 14 ...`, `[prop-render] pi=1 oid=0x05 ...`; cam-trace Cut 0. **Differenz 59 Pixel, 50 in der Marke, 59 im Fenster (+-2), 0 ausserhalb; Huelle x 40..45 y 110..123** — roter Loescher auf der hellen Platte unter dem Monitor (`abnahme_k0/p1_1030_cut0_mit_ohne.png`) |
| p1_tuer_mit / _ohne | ECHTER TUER-WEG: `1050 20900 12350 0 cut:0`, Skript `W1,A0.5,W8`, Ende `150#1030` | `[tuer] Sequenz Archiv 2 DOOR13 ...`, `[room] PC loaded room1030.rdt`, Ankunft (1172,-12558) **Cut 2**, Installer + `[prop-render] pi=1 oid=0x05` auch auf dem Tuer-Weg. Bild 140: **175 Pixel, Huelle x 271..280 y 87..110, 0 ausserhalb** (`p1_tuer_1030_cut2_mit_ohne.png`: Loescher haengt am Pfeiler) |
| p1_auf | Aufnahme aus EIGENER Lage `1030 -4250 -18200 0 cut:0` (Blick +x), `W1,A0.1,W3.5,A0.1,W4,A0.1,W6` | `[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 wie ROOM1000 sub01 @0x00D0C`; dritter Druck an derselben Stelle `[msgtext] room=1030 id=14 "Seems like even the emergency broadcast has gone offline..."`. Bilder F100 Item-Bild, F150 "Will you take the ...", F300 Pfeiler leer, F420 Monitor-Text (`p1_auf_ablauf.png`); Kamera bleibt Cut 0 |
| p1_1050_mit / _ohne | `1050 15500 -21000 0 cut:6` [+ Bit] (alter Ort, Cut 6) | kein `[feuerloescher]`, kein `oid=0x05`; **Differenz 0 Pixel im ganzen Bild** -> ROOM1050 leer |
| p1_1031_elza | `1031 -11500 -19500 2048 cut:0 elza flag:3:0xC1 flag:3:0x7D` (die zwei Flags halten die Original-Auto-Tueren 18/19 von ROOM1031 main00 @0x0204A/@0x0207A (Ck(3,0xC1)==0 / Ck(3,0x7D)==0) zu; ohne sie springt Elza sofort nach ROOM1241 — vorbestehend, nicht Spur K) | `[prop-render]` nur obj 0 und obj 15, kein `[feuerloescher]`/`[itemorte]`; gegen Leons 1030-ohne-Bild **0 Pixel in der Marke** (Rest nur die Spielfigur x 202..220) |

Alle Cuts von ROOM1030 (eigene Laeufe `RE15_FORCE_CUT=<n>`, mit/ohne (9,0x86), diff.py ueber das ganze Bild):

| Cut | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9* | 10* | 11* | 12* |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Pixel | 59 | 90 | 175 | 0 (Regel) | 0 | 0 | 0 | 0 | 0 | 58 | 0 (Regel, V1) | 0 | 0 |

(* mit (4,0x0F) = Zustand nach dem Rolltor, Cut_replace 0->9/3->10/4->11/6->12 @0x01FF4..; ohne das Flag ist Cut 9
ein anderes Anker-Viereck und zeigt 0 — das ist kein Spielzustand.) Gezeichnet wird er genau in den Kameras, die die
-x-Flaeche des Pfeilers zeigen (0/9, 1, 2); in Kamera 3/10 liegt die Flaeche hinter dem Pfeiler (Bild des Bau-Agenten
`k3_...png`: ohne Regel schiene er dort durch) — die Regel ist gerechtfertigt, kein Cut zeigt ihn faelschlich.

Urteil Punkt 1: **erfuellt** (Ort = Nutzer-Marke im Nutzer-Cut, auf dem CONTINUE- und dem Tuer-Weg gezeichnet, auch im
Rolltor-Zustand (Cut 9), Aufnahme funktioniert, Monitor-Satz bleibt erreichbar, ROOM1050 leer, Elza unveraendert ohne
Wand-Loescher).

## 2. Punkt 2 — Parking Lot Key / Prison Key "gut sichtbar" an der Stelle aus key_location.bmp

Nutzerbild selbst ausgewertet: rote Marke 99 Pixel, x 133..143 y 77..85 (11 x 9) auf der Pinnwand unten rechts;
Hintergrund = ROOM10F0 Cut 5 (deckungsgleich mit dem Spielbild p2_leon).

| Lauf | Befehl (lauf.sh) | Ergebnis |
|---|---|---|
| p2_leon_mit / _ohne | `10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94` [+ `flag:9:82`], Bild 100, Skala 1 | `[itemorte] ROOM10F0: Slot 8 (msg 4) -> Slot 15`, `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei (1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8`, `[prop-render] pi=4 oid=0x04`; **55 Pixel, 51 in der Marke, 0 ausserhalb, Huelle x 132..143 y 78..84**, mittlere Luminanz-Aenderung Betrag-dL 62,3 — helle Klinge, schwarzer Kopf, rotes Emblem, als Schluessel erkennbar (`p2_10f0_cut5_mit_ohne.png`) |
| p2_elza_mit / _ohne | `10F1 0 4000 1024 cut:5 elza` [+ `flag:9:83`], Skala 1 | `[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) ... rot (1024,3618,2048), Zone Slot 8`; **14 Pixel (Schwelle 30) bzw. 17 (Schwelle 10), 0 ausserhalb, Huelle x 134..143 y 78..85**; Mittel mit (71,67,67) gegen Kork (66,64,50) — ein unterbrochener Bogen aus Einzelpixeln (`p2_10f1_cut5_mit_ohne.png`) |
| p2_leon_s3 / p2_elza_s3 | dieselben Laeufe mit `RE15_WINDOW_SCALE=3` (960x720, 3D in Fensteraufloesung, render_pc.c `SDL_RenderSetLogicalSize` + `SDL_RenderGeometry`) | Parking Lot Key **491 Pixel** (Huelle x 397..432 y 233..255, Betrag-dL 58,8); Prison Key **112 Pixel** (Huelle x 402..431 y 234..257, Betrag-dL 28,5, Luminanz-Mittel mit 68,3 / ohne 65,6) = duenner 1-Pixel-Ring in Kork-Ton; Ausschnitt 1:1 `p2_s3_1zu1.png`, vergroessert `p2_10f1_s3_mit_ohne.png` |
| p2_tuer_leon | ECHTER TUER-WEG `10D0 1900 -7000 2048 cut:0 flag:3:50 flag:4:247 flag:9:71 flag:3:94`, `W1,A0.5,W12`, Ende `200#10F0` | `[tuer] Sequenz Archiv 2 DOOR13 ...`, `[room] PC loaded room10f0.rdt`, `[itemorte] ROOM10F0: Slot 8 (msg 4) -> Slot 15`, `[schluessel] ROOM10F0: PARKING LOT KEY ...` — Installer laeuft auch auf dem Tuer-Weg (Ankunft (8400,-350) Cut 0, dort liegt der Schluessel ausserhalb des Anker-Vierecks) |
| p2_geh3 (Leon) | ECHTER GEHWEG vom Gang zur Pinnwand: `10F0 0 4000 1024 cut:5 ...`, `W1,U3.2,W0.3,L0.36,W0.3,U0.45,W0.3,R0.36,W0.3,U1.6,W1,A0.1,W3.5` | State-Log: Gang bis (0,-432) (Regal), nach +x bis (1036,-390), nach -z bis (1036,-2932) vor der Pinnwand; Kamera durchgehend Cut 5; Aktion -> Modal "Will you take the Pa..." mit RE2-Bild (`p2_geh3.png`) |
| p2_geh_elza (Elza) | derselbe Gehweg in 10F1 + `A0.1,W4,A0.1,W4` | `[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)`, `Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)`, danach `[msgtext] room=10f1 id=4 "Several files from past cases.Nothing useful here."` (`p2_geh_elza.png`) |
| Annaeherung | Pinnwand-Ausschnitt x 100..180 y 55..105, 4x: Elza F100/F200/F260 gegen Leon F200 | Parking Lot Key deutlich; Prison Key nur als einzelne graue Pixel eines Rings — beim Hingehen in der Bildgroesse 2x nicht auszumachen (`p2_annaeherung_zoom.png`) |
| p2_elza_s4 | wie p2_elza mit `RE15_WINDOW_SCALE=4` (1280x960; Standard-Hoechstskala render_pc.c:45 `WINDOW_SCALE 4`, auf 1440p-Schirmen) | Prison Key **215 Pixel**, Huelle x 536..575 y 312..343, Betrag-dL 30,7, Luminanz-Mittel mit 68,5 / ohne 65,7 — weiterhin ein duenner dunkler Ring (`p2_10f1_s4_mit_ohne.png`) |
| p2_tuer_elza | ECHTER TUER-WEG Elza `10D1 1900 -7000 2048 cut:0 elza flag:3:50 flag:4:247` | `[room] PC loaded room10f1.rdt`, `[itemorte] ROOM10F1: Slot 8 (msg 4) -> Slot 15`, `[schluessel] ROOM10F1: PRISON KEY ...` — Installer auch auf Elzas Tuer-Weg |
| alle Cuts 10F0 | `RE15_FORCE_CUT=0..11` (nCut 12), mit/ohne (9,82) | Cut 5: 55, Cut 6: 108, Cut 7: 0 (Regel; ohne Regel laege er auf dem Buecherregal, Bild `k3_...png` des Bau-Agenten), Cut 0/1/2/3/4/8/9/10/11: 0 (Anker-Viereck) |
| V1..V5 (aus meinem ctest-Lauf, Bilder 22:20 nachgerechnet mit diff.py) | integration_r38_itemorte | 1030 Cut 10: 0; 10F0 Cut 7: 0; 10F0 Cut 6: 108 (x 81..98 y 67..76); 10F1 Cut 7: 0; 10F1 Cut 6: 29 (x 83..97 y 67..78); eigene Laeufe 1030 Cut 1: 90 (x 296..303), Cut 3: 0 |

Bewertung "gut sichtbar":
* **Parking Lot Key (Leon): erfuellt** — in der Marke, fuellt sie, hebt sich in Farbe und Helligkeit klar vom Kork ab
  (55 Pixel bei Skala 1, 491 bei Skala 3), auf dem Gehweg sichtbar, Aufnahme funktioniert.
* **Prison Key (Elza): Ort erfuellt, "gut sichtbar" NICHT** — gemessen: bei Skala 1 aendern sich 14 Pixel (ein Viertel
  des Parking Lot Key), Kontrastsumme Sum|dL| ~ 500 gegen ~ 3430 beim Parking Lot Key (Faktor ~7); bei Skala 3 ein
  1-Pixel-Ring (112 Pixel, Faktor ~4,4 weniger, mittlere Luminanz nur +2,7 gegen den Kork). Das Modell (RE2 Cabin
  Key, duenner dunkler Eisenring, Runde-36-Wahl) ist die Ursache; der Bau-Agent hat das selbst als offene Grenze
  benannt (Dossier Abschnitt 6/8) und nicht geloest. Der Wortlaut verlangt "gut sichtbar" fuer BEIDE Schluessel
  ("den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar"); die Modellwahl ist frei
  (Runde-36-Wortlaut, analysis/befunde_runde36/AUFTRAG.md Z. 49: "Auch hier - suche die ein gutes Model aus Resident
  Evil 2 aus für einen prison key.").

Urteil Punkt 2: **teilweise** (Ort fuer beide erfuellt, gut sichtbar nur fuer den Parking Lot Key) -> Mangel M1.

## 3. RE-Gate (Stichprobe, selbst disassembliert)

| Zitat im Diff | selbst geprueft (Werkzeug, Binaerdatei) | Befund |
|---|---|---|
| Aktions-Scan PSX.EXE @0x80042bd0 `ori v0,zero,0x26c`, @0x80042c50 `lw s0,0(s4)`, @0x80042f50 `beq v0,zero,0x8004301c`, @0x80042f8c `jalr v0`, @0x80042f94 `j 0x80043028` | `re15_disasm.py dis` info/Re1.5/PSX.EXE | alle Instruktionen wortgleich. Zusaetzlich gelesen: s6 = a2 (@0x80042bbc), @0x80042ca0 `andi v0,sat,0x10; bne v0,s6` -> im Aktionslauf (s6 = 0x10) kehrt die Funktion nach dem ERSTEN Treffer zurueck (@0x80042fbc `j 0x80043028` auch im Zweig ohne sat 0x80) — "kleinster Slot gewinnt" belegt; @0x80042f48 `lbu v0,0(s0)` = sce, 0 -> weiter (genommene Zone uebersprungen) |
| RE2 Obj_model_set = Tabelle 0x800a74c8 [0x2D] -> 0x80055260; @0x800552d0 `lhu v0,10(s2)` \|1 -> Pool+0; @0x8005530c `lh a1,12(s2)` -> @0x80055320 `sw a1,16(s1)` | `re2_disasm.py table/dis` info/re2leon/PSX.EXE | Eintrag [45] = 0x80055260, [46] = 0x80055904, [55] = 0x8005655c; Pool = 0x800cc1e8 + 16700 + id*504; Attribut-Wort pc[12..13] -> Pool+0x10 — wie zitiert |
| RE2 ROOM6150.RDT @0x02C98 / ROOM1050.RDT @0x01274 Attribut 0x0010 | `xxd` info/re2leon/PL0/RDT | `2d08 0000 0000 0000 0000 0000 1000` / `2d00 0000 0000 0000 0000 0a00 1000` — Attribut 0x0010. Zuordnung selbst geprueft: der folgende Item_aot_set ROOM6150 @0x2CBE Item 0x5F (Power Room Key) md1 = 8, ROOM1050 @0x1208 Item 0x58 (Cabin Key) md1 = 0 -> die zitierten Objekte SIND die Schluessel |
| Flach-Zweig RE2 @0x800365e8 `andi v0,v0,0x10` -> @0x80036610 `jal 0x8002d0e8`; RE1.5 @0x8002c2f4 `andi v0,v0,0x10` -> @0x8002c320 `jal 0x80022f0c`, Anker-Test @0x8002c2dc `jal 0x80014368` | `re2_disasm.py` / `re15_disasm.py dis` | wortgleich |
| ROOM1030 main00 @0x01DA6 / ROOM10F0+10F1 main00 @0x00FEA; Cut_replace @0x01FF7/@0x02785 | `xxd` shared_assets/PSX/STAGE1 | `2c0e 0131 0000 a2ef d8b5 3305 fa06 0e00` = Slot 14 sce 1 sat 0x31 (-4190,-18984) 1331x1786 msg 14; `2c08 0131 0000 bc02 28f1 4006 2003 0400` = Slot 8 (700,-3800) 1600x800 msg 4 (beide Varianten gleich); `4b 03 0a` an beiden Stellen. `scd_dump_room.py`: kein Sub-Skript von 1030/10F0/10F1 setzt die Slots 8/14/15/20 neu |

* Suche im Code-Diff nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/ungefaehr/geschaetzt: **0 Treffer**;
  kein neues `getenv` im Diff (kein Env-Schalter als Abschluss).
* Konstanten ohne Adresse sind gekennzeichnet: Lagen/Massstab/Drehung/Zonen = NUTZER-VORGABE bzw. PORT-WAHL mit
  belegter Form (Kopf re15_itemorte_r38.h, Werkzeug-Kopf, Dossier 2.8); Ausweich-Slots 20/15 = Muster Runde 36 N;
  Sichtbarkeitsregel Cut 3/10 bzw. 7 = PORT-ZUSATZ mit Messung.
* Erzeugte Dateien nachgeprueft: `itemorte_export.py` mit umgelenktem Ziel neu laufen lassen -> `itemorte_r38_lage.inc`
  und `itemorte_r38_md1.inc` **bitgleich** (`cmp` ohne Ausgabe) — nicht von Hand geaendert.
* Erklaert der Fix den Befund? Ja: vorher hing in 1030 Cut 0 nichts an der Marke und in 10F0 Cut 5 aenderte sich an der
  Marke 0 von 99 Pixeln (Dossier 1, Stand 1bcbd0ff); jetzt Prop + Zone genau dort (Messungen oben).

Gate: **haelt**.

## 4. Vertrag / Pfade / Tests

* **Pfade** (`git diff master...HEAD --name-only`, Merge-Basis 1bcbd0ff — `git diff master` ohne Punkte zeigt zusaetzlich
  die seit der Basis auf master gewachsenen Orchestrator-Dateien als "Loeschung", das ist NICHT die Spur): neu
  `engine/src/itemorte_r38.c`, `engine/src/gen/itemorte_r38_{lage,md1}.inc`, `include/re15_itemorte_r38.h`,
  `tests/unit/test_r38_itemorte.c`, `tests/unit/probes/r38_itemorte.cmake`, `tests/integration/test_r38_itemorte.cmake`,
  `tools/r38_itemorte/itemorte_export.py`, Dossier + K_belege; geaendert die Spur-K-Dateien `feuerloescher_r37.c`,
  `schluessel_r36.c` (+ ihre Koepfe), die gemeinsame `platform/pc/main.c` (+4 Zeilen: Include + 3-Zeilen-Haken nach
  `re15_prop_culled`), `kistenregel_r37.c` (nur 1 Kommentarzeile), angepasste Riegel `test_r36_schluessel.*`,
  `test_r37_feuerloescher.*` (nur die Orte; keine Pruefung entfallen, Ersatzpruefungen am neuen Ort). KEIN `release/`,
  `platform/android/`, `shared_assets/PSX/`, keine Test-CMakeLists. Die aus `re15_schluessel_r36.h` entfernten
  Runde-36-Lage-Konstanten (RE15_R36Q_SLOT/X/Y/Z/ROT_Y/RECT_*) nutzt weder HEAD noch master sonst (`git grep`).
* **Ressourcen**: keine neuen Bank-9-Bits (Loescher (9,0x86)/(3,0x85), Schluessel (9,82)/(9,83)/(4,0xEC) aus Runde 36/37),
  keine Nachrichten-Ids, keine Assets. Belegte AOT-Slots: ROOM1030 14 (Zone) + 20 (Monitor-Satz), ROOM10F0/10F1 8 (Zone)
  + 15 (Pinnwand-Satz); kein Original-Skript (main/sub) setzt diese Slots neu (scd_dump_room.py). Kein anderer Installer
  in diesem Baum beruehrt 10F0/10F1-Slots (`grep` engine/src: nur schluessel_r36.c/itemorte_r38.c).
* **Riegel je Punkt vorhanden und messend**: `unit_r38_itemorte_{lage,raum,aufnahme,sicht}` (direkt gestartet:
  **50 Pruefungen, 0 Fehler**), `integration_r38_itemorte` (B1..B4, A, K1, K2, V1..V5, E, O an der echten exe).
* **Gezielter ctest-Lauf am gebauten Stand** (eigener Lauf, Regex in `r38k_abn0/regex.txt` = die 112er-Auswahl des
  Bau-Agenten + `unit_r36_kartenbits|unit_1090_fire_pin|unit_1090_flame_out_pin|unit_1090_room_pin|unit_re2_room1090_ab`;
  sauberer PATH wie local_build.sh, `-j 3 --timeout 600 --output-on-failure`, 21:57-22:28):
  **`100% tests passed, 0 tests failed out of 117`** (Total Test time (real) = 1879.81 sec), darin
  integration_r38_itemorte 609.73 s, integration_r37_feuerloescher 962.65 s, integration_r36_schluessel 150.00 s,
  integration_r36_schluessel_befreiung 76.75 s, integration_r37_kistenregel 476.81 s, integration_r36_story_leon/elza,
  integration_r35_cut10f0, integration_r34n_e_dokumente_bild, integration_flag_census/item_name_census.
* **Nebenwirkungen beide Szenarien** (gemessen): Leon 1050 leer (p1_1050), Elza 1031 ohne Loescher (p1_1031_elza),
  Elza 1091 leerer Loescher unveraendert (E2 meines Laufs: `ROOM1091: Loescher liegt leer am Boden obj 5 bei
  (2500,-1800,-1300)`), Leon 1090 Loeschen mit dem in 1030 aufgenommenen Loescher (integration_r37_feuerloescher A
  gruen), Parkplatz-Tuer 11B0 + Zelle 1211 (integration_r36_schluessel(+_befreiung) gruen), Lageplan 1F in 1030
  (`[kartenitems] ROOM1030: ... Zone Slot 8 (Muenztelefon -> Slot 39)` neben dem Loescher im selben Lauf).

Vertrag: **haelt**.

## 5. Maengel (nummeriert, nachpruefbar)

**M1 — Punkt 2, Elza: der Prison Key ist an der Marke, aber NICHT "gut sichtbar".**
* Wortlaut: "den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location platzierst".
* Gemessen im Nutzer-Cut ROOM10F1 Cut 5 (Elza), Differenz mit/ohne (9,83), |dR|+|dG|+|dB| > 30:
  Skala 1: **14 Pixel** (Parking Lot Key am selben Ort 55), Kontrastsumme Sum|dL| ~ 500 (Parking Lot Key ~ 3430);
  Skala 3: **112 Pixel** (Parking Lot Key 491); Skala 4: 215 Pixel. Es ist ein 1-Pixel-Ring in dunklem Eisengrau auf
  dunklem Kork (Luminanz-Mittel der geaenderten Pixel 68,3 gegen 65,6 ohne Schluessel, Skala 3). Beim Hingehen (Gehweg
  p2_geh_elza, Bilder F100..F260) ist er in der Bildgroesse 2x nicht auszumachen.
* Ursache: das Modell (RE2 "Cabin Key", Runde-36-Wahl) — der Bau-Agent benennt das selbst als offene Grenze (Dossier
  Abschnitt 6 und Schlusssatz Abschnitt 8) und hat es nicht geloest. Die Modellwahl ist NICHT vom Nutzer festgelegt
  (Runde 36, AUFTRAG.md Z. 49: "suche die ein gutes Model aus Resident Evil 2 aus für einen prison key").
* Nachpruefen: `lauf.sh p2_elza_mit 120 "W3" 10F1 0 4000 1024 cut:5 elza -- RE15_FRAMEDUMP=100:fd_100.ppm` und dasselbe
  mit `flag:9:83`, dann `diff.py ... 133 77 143 85 30` (und mit `RE15_WINDOW_SCALE=3`, Fenster 399 231 431 257);
  Riegel B3 (>= 10) / B4 (>= 90) schreiben den schwachen Stand fest.
* Erwartet: ein Prison Key, der in Elzas Cut 5 an der Marke so deutlich zu erkennen ist wie der Parking Lot Key
  (gleiche Groessenordnung an geaenderten Pixeln und Kontrast bei Skala 1 und 3) — z. B. ein anderes, kraeftigeres
  RE2-Schluesselmodell (Wahl im Dossier begruenden) — und Riegel B3/B4 auf den neuen Stand.

## 6. Hinweise (kein Mangel)

* H1: Die Logzeile `[schluessel] ROOM10F0/10F1: ... Zone Slot 8` wird auch geschrieben, wenn die Zone NICHT angelegt
  werden konnte (Slot 15 belegt -> `re15_itemorte_r38_platz_frei` tut nichts -> Slot 8 bleibt der Satz msg 4). Heute
  greift das nicht (Riegel unit_r38_itemorte_raum), bei der Integration aber muessen ROOM1030 Slot 20 und ROOM10F0/
  10F1 Slot 15 frei bleiben (sonst: sichtbarer, nicht aufnehmbarer Schluessel bzw. Loescher) — vom Bau-Agenten benannt.
* H2: Veralteter Kommentar `scd_room_setup.c:466` ("Feuerloescher 1050/1090/1091") — absichtlich nicht angefasst
  (gemeinsame Datei); bei der Integration nachziehen.
* H3: `kistenregel_r37.c` steht nicht in der K-Liste von VERTRAG 1.4; die Aenderung ist eine einzige Kommentarzeile.
* H4: Neue Quelldatei `engine/src/itemorte_r38.c` -> Android-Bau neu konfigurieren (GLOB-Cache).
* H5: Der Parking Lot Key ist auf das 6,46-Fache der RE2-Groesse skaliert (sichtbar 735 x 468 Einheiten), damit er die
  11x9-Marke fuellt — PORT-WAHL auf "gut sichtbar", im Bild (Skala 3) liest er sich als Schluessel an der Pinnwand.

## 7. Ergebnis

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Feuerloescher ROOM1030 an der Stelle aus fire_extinguisher.bmp statt 1050 | **erfuellt** | p1 (59 Pixel, 50 in der Marke, 0 aussen), Tuer-Weg 1050->1030 (Cut 2, 175 Pixel), Aufnahme + Monitor-Satz, 1050 leer (0 Pixel), Elza 1031 ohne, alle 13 Cuts |
| 2 Parking Lot Key / Prison Key gut sichtbar an der Stelle aus key_location.bmp | **teilweise** | Ort beide erfuellt (Huelle in der Marke, Tuer- und Gehweg, Aufnahme Leon + Elza); gut sichtbar nur Parking Lot Key (55/491 Pixel), Prison Key 14/112 Pixel duenner dunkler Ring -> M1 |
| RE-Gate | haelt | 5 Stellen selbst disassembliert/gelesen (Abschnitt 3), keine Rate-Woerter, .inc bitgleich reproduziert |
| Vertrag/Tests | haelt | Abschnitt 4, gezielt 117/117 gruen |

**Bestanden: NEIN** — Mangel M1 (Prison Key nicht gut sichtbar).
