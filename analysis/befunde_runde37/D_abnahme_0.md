# Runde 37 — Spur D "itemband": UNABHAENGIGE ABNAHME 0 (2026-10-12)

Baum `.claude/worktrees/r37_itemband` (Zweig `r37/itemband`, HEAD 05117c03, Merge-Basis master f1e019ca).
Wortlaut (AUFTRAG.md #8): "Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super
Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden."
Vertrag (VERTRAG.md, Spur D): Items auf erhoehten Flaechen nur vom richtigen Band aufnehmbar (Band-Gate + Member 0x12),
alle betroffenen Items bleiben auf dem vorgesehenen Weg erreichbar.

(Bericht wird fortlaufend geschrieben — Stand siehe Abschnitte.)

## 0. Stand der Abnahme
- [x] Diff gelesen, Bau (configure + build, danach Neu-Kompilieren der 3 Haken-Dateien erzwungen) — exe 12:25 aus sauberem Baum
- [x] unit_r37_itemband gruen (0.45 s)
- [x] RE-Gate: Adressen selbst disassembliert
- [x] exe-Messungen (M1-M3, R1-R2, L1/L1e/L2, N1, Original L1/L2)
- [x] gezielte Regression (131 Unit + 25 Integration gruen)
- [x] Urteil: #8 teilweise, bestanden = false (Maengel 1-4)

## 1. Vorpruefung
* `git diff master...HEAD --name-only`: Spur-D-Dossier/Belege + 7 Port-Dateien (aot_common.c +3, actor_common.c +3/-1,
  neu itemband_r37.c, re15_itemband_r37.h, test_r37_itemband.c, probes/r37_itemband.cmake, integration/test_r37_itemband.cmake).
  `git diff master` (zwei Punkte) zeigt zusaetzlich geloeschte Orchestrator-/UEBERGABE-Dateien — das ist nur der seit f1e019ca
  weitergelaufene master (dort nur doc/orch-Commits, `git diff --stat f1e019ca master -- re15_port` leer), KEIN Inhalt des Zweigs.
* Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> "ninja: no work to do". Weil der Bau-Agent fuer seine Gegenprobe
  Haken aus-/einkommentiert hat, habe ich actor_common.c / aot_common.c / itemband_r37.c per `touch` (Inhalt unveraendert,
  `git status --porcelain re15_port` leer) neu kompilieren lassen: `LOCAL-BUILD-OK (build)`, re15_pc.exe 12:25.
  `strings re15_pc.exe | grep -c RE15_ITEMBAND_LOG` = 1 (Fix-exe), Hauptbaum-exe (09:02, master-Code = Basis) = 0 (Vorher-exe).
* `ctest -R "^unit_r37_itemband$"` -> `1/1 ... Passed 0.45 sec`. Riegel gelesen: Teil G fuehrt jeden Item-Satz ueber den
  echten Opcode-Handler + `re15_aot_scan` aus, Teil M die echten Lift-Subs — er misst, er bestaetigt sich nicht selbst.

## 2. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)
| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| @0x80042cac-ccc Band-Gate fuer JEDEN Satz | `80042cac lbu v0,2(s0)` / `80042cb4 andi v0,v0,0x80` / `80042cb8 bne v0,zero,0x80042cd4` / `80042cc0 lbu v1,130(s1)` / `80042cc4 lbu v0,2(s0)` / `80042ccc bne v1,v0,0x8004301c`; `8004301c bne v0,zero,0x80042c50` = naechster Satz; `80042bb4 addu s1,a0,zero` (s1 = Entity) | bestaetigt |
| sce wird erst NACH dem Gate gelesen, [9] = ITEM | zwischen `80042c50 lw s0,0(s4)` und `80042ccc` nur `lbu 1(s0)`/`lbu 2(s0)`; `80042f48 lbu v0,0(s0)`, `80042f74 lbu v0,0(s0)` + `sll 2` + Tabelle s5 = 0x8007469c (`80042c40/44`); Tabelle [9] = 0x80043328 | bestaetigt |
| Aufrufer @0x80031fdc a0 = 0x800ACA54 | `80031f40/44 s0 = 0x800ac768`, `80031fdc addiu a0,s0,748` (Delay-Slot), `80031fe0 ori a1,zero,0x1`, `80031fe4 jal 0x80042bac`, `80031fe8 ori a2,zero,0x10`; davor `80031fd0 jal 0x8002d474` (Kletter-Sonde) | bestaetigt |
| Item_aot_set rec = pc+2 @0x800406cc | `80040690 lbu v0,1(a2)` (Slot), `80040698 addiu v1,v1,-13904` (0x800ac9b0), `800406cc addiu v0,a2,2`, `800406d0 sw v0,0(s0)` | bestaetigt |
| Member_set 0x12 -> @0x80041228 | `800410cc lw a0,340(s0)`, `800410d0 lbu a1,1(v0)`, `800410d4 lh a2,2(v0)`, `800410d8 jal 0x8004116c`; `8004116c sltiu v0,a1,0x14`, Tabelle 0x80010c8c[18] = 0x80041224; `80041224 j 0x80041230` / `80041228 sb a2,130(a0)` | bestaetigt |
| Work_set kind 1 @0x80040da4 | `80040da4 lui v0,0x800b` / `80040da8 addiu v0,v0,-13740` (= 0x800ACA54) / `80040db0 sw v0,340(a0)` | bestaetigt |
| Daten ROOM3050 | `scd_dump_room.py ROOM3050.RDT`: `0x01EC6 Item_aot_set 50 06 09 31 01 00 20 9f 6c a3 08 07 20 03 07 00 06 00 5c 00 02 00` (slot 6, floor 1, Rechteck (-24800,-23700,1800,800), Item 7, tk 0x5c = 92); sub04 `0x023D0 Member_set 34 12 00 00`; sub02 `0x0232C Member_set 34 12 04 00` | bestaetigt |

Konstanten im Diff: `RE15_R37D_MEMBER_BAND 18` (@0x80010c8c[18] -> @0x80041228) und `RE15_R37D_BAND_EGAL 0x80u` (@0x80042cb4) —
beide belegt. Suche nach "deferred|tunable|interim|for now|faithful|plausibel|TODO" in den 4 Code-Dateien des Diffs: siehe Abschnitt 5.

## 3. Messungen an der exe (eigene Laeufe, Messkopie `re15_pc_r37d_abn0.exe`, Vorher-Kopie `re15_pc_vorher_abn0.exe`)
Laufordner Scratchpad `abn_d/laeufe/<lauf>` (Vorher: `abn_d/laeufe/vorher/<lauf>`), Skript `abn_d/lauf.sh` (Umgebung wie
D_belege/lauf_d.sh: RE15_NO_INTRO/NOAUDIO/WINDOW_SCALE=1/PSELECT_AUTO, RE15_WEGPLAN, RE15_ITEMBAND_LOG, RE15_CLIMB_TRACE).

### M1 — ROOM3050 obere Ebene (Band 4), ECHTE Tuer aus ROOM30E0, eigene Stellung (nicht die des Bau-Agenten)
Standplatz der Tuer per `probe_r33_tueren.exe standplatz 30E0 3050` -> `RE15_PLAYER_POS=4273,4861,1792,0`.
Befehl `bash lauf.sh m1_tuer_oben plaene/m1_tuer_oben.txt RE15_DEBUG_JUMP=30E0@250 RE15_PLAYER_POS=4273,4861,1792,0 RE15_EXIT_AT=2200#3050`
(Wegplan: RAUM 30E0, AKTION an der Tuer, RAUM 3050, Gang der oberen Ebene, Stellung (-23300,-22650) Blick Nord, AKTION x3).
* debug.log: `[aot] DOOR FIRE slot=6 rect=(3700,4625,hw=325,hh=975) target_cut=8 spawn=(-3596,-7200,1523)`, `loaded room3050.rdt`
* itemband.txt: `F791 / F854 / F917 room=3050 ITEM slot 6 floor 1 item 0x07 tk 92 spieler-band 4 spieler=(-23297,-7200,-22642) -> GESPERRT (@0x80042ccc bne Band)`
* wegplan.txt: `F63 FLAG (9,92) = 0` ... `F1020 FLAG (9,92) = 0` -> **nicht aufgenommen**.

### M2 — ROOM3050 untere Ebene (Band 0) OHNE Kiste, ECHTE Tuer aus ROOM3040 (der woertliche "ohne die Kiste"-Fall von unten)
ROOM3040 slot 1 @0x012A0 fuehrt nach 3050 (Ankunft (-23996,0,1523), pc[25] = 0). Standplatz `RE15_PLAYER_POS=-26305,-4975,2048,0`.
Befehl `bash lauf.sh m2_tuer_unten plaene/m2_tuer_unten.txt RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-26305,-4975,2048,0 RE15_EXIT_AT=1800#3050`
(Wegplan: RAUM 3040, GEGNERWEG, AKTION an der Tuer, RAUM 3050, nach Norden bis in die Kerbe (-24058,-22650), Blick Nord, AKTION x3;
die Kiste bleibt oben auf (-18000,-7200,-20800)).
* debug.log: `[aot] DOOR FIRE slot=1 rect=(-26925,-4975,hw=375,hh=925) target_cut=0 spawn=(-23996,0,1523)`, `loaded room3050.rdt`
* wegplan.txt: `F287 (-23996,-14152) angekommen`, `F422 (-24051,-22622) angekommen`, `F675 FLAG (9,92) = 0`, `F676 FLAG (3,93) = 0`
* itemband.txt: `F446 / F509 / F572 room=3050 ITEM slot 6 floor 1 item 0x07 tk 92 spieler-band 0 spieler=(-24051,0,-22622) -> GESPERRT (@0x80042ccc bne Band)`
  -> **nicht aufgenommen**. (`mo=211` danach = RE15_MOTION_IDLE_SWAY, player_common.c:98 — kein Klettern, keine Aufnahme.)

### M1/M2 GEGENPROBE mit der Vorher-exe (Hauptbaum re15_pc.exe 09:02 = master-Code = Basis f1e019ca, als `re15_pc_vorher_abn0.exe`)
Dieselben Befehle mit `ABN_VORHER=1` (Laufordner `abn_d/laeufe/vorher/`):
* M1 oben: climb.txt `T1104 room=3050 pad=8000 pos=(-23297,-7200,-22642) rot=1072 band=4`; wegplan.txt **`F1020 FLAG (9,92) = 1`**
* M2 unten ohne Kiste: climb.txt `T759 room=3050 pad=8000 pos=(-24051,0,-22622) rot=1024 band=0`; wegplan.txt **`F675 FLAG (9,92) = 1`**, `(3,93) = 0`
-> Meine beiden Szenarien reproduzieren den Nutzer-Befund am Vorher-Stand (Aufnahme von oben UND von unten ohne Kiste) und
sind mit dem Fix gesperrt — sie diskriminieren.

### M3 — ROOM3050 VORGESEHENER Weg ueber die ECHTE Tuer aus 30E0, Leon und Elza (je 191 s)
Wegplan = der des Riegels (weg3050) mit vorangestellter Tuer (RAUM 30E0/30E1, AKTION): Schub West, Schub Sued bis zum Fall,
Lift, unten zur Kiste, Schub Nord in die Kerbe, Klettern, Aufnahme. Befehle:
`bash lauf.sh m3_tuer_weg_leon plaene/m3_tuer_weg_leon.txt RE15_DEBUG_JUMP=30E0@250 RE15_PLAYER_POS=4273,4861,1792,0 RE15_EXIT_AT=4700#3050`,
Elza: `... m3_tuer_weg_elza plaene/m3_tuer_weg_elza.txt RE15_PSELECT_AUTO_SWITCH=1 ... RE15_EXIT_AT=4700#3051`.
| | Leon (ROOM3050) | Elza (ROOM3051) |
|---|---|---|
| Tuer | `DOOR FIRE slot=6 ... spawn=(-3596,-7200,1523)` | `loaded room30e1.rdt`, `DOOR FIRE slot=2 ... spawn=(-3596,-7200,1523)`, `loaded room3051.rdt` |
| Fall | `F1235 SCHIEB: Objekt-y -7200 -> -7192 (Fall)` | `F1241 SCHIEB: Objekt-y -7200 -> -7192 (Fall)` |
| Lift | itemband `F2201 MEMBER 0x12 Spieler: Kollisionsband 4 -> 0 (@0x80041228) y=0`, `F2603 FLAG (3,82) = 1` | `F2205 MEMBER 0x12 ... 4 -> 0 ... y=0`, `F2607 FLAG (3,82) = 1` |
| Kerbe | `F3453 SCHIEB: Bedingung erfuellt (obj.z = -22225)`, `F3514 FLAG (3,93) = 1` | `F3577 ... (obj.z = -22206)`, `F3638 FLAG (3,93) = 1` |
| Aufnahme | `F3807 ITEM slot 6 floor 1 ... spieler-band 1 spieler=(-24072,-1800,-22732) -> FREI`, **`F4036 FLAG (9,92) = 1`** | `F3926 ... spieler-band 1 spieler=(-24043,-1800,-22732) -> FREI`, **`F4155 FLAG (9,92) = 1`** |
-> Der vorgesehene Weg zur Super Redhawk ist mit dem Fix fuer BEIDE Figuren Ende-zu-Ende gangbar (ohne Sprung in 3050).

### R1/R2 — Regressions-Stichproben (Items, die FREI bleiben muessen)
* **R1 ROOM1090-Dach, Port-Item Shotgun Shells (inhalt_r35, floor 5 = RE15_R35I_SCHROT_FLOOR, tk 81):**
  `bash lauf.sh r1_1090_dach plaene/r1_1090_dach.txt RE15_DEBUG_JUMP=1090@250 RE15_PLAYER_POS=-4610,-15155,0,5 RE15_EXIT_AT=1100#1090`
  -> itemband `F159 room=1090 ITEM slot 4 floor 5 item 0x16 tk 81 spieler-band 5 spieler=(-2477,-9000,-14710) -> FREI`, `F388 FLAG (9,81) = 1`.
* **R2 ROOM3040 Erdgeschoss-Item 0x36 (sub00 @0x014B2, floor 0, tk 85; Raum mit Treppen Band 0/1):** Ankunftsstelle der
  3060-Tuer (-8900,0,-26300) Band 0 (pc[25] = 0): `bash lauf.sh r2_3040_erdg plaene/r2_3040_erdg.txt RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-8900,-26300,2048,0 RE15_EXIT_AT=1100#3040`
  -> `F159 room=3040 ITEM slot 9 floor 0 item 0x36 tk 85 spieler-band 0 spieler=(-11773,0,-24769) -> FREI`, `F388 FLAG (9,85) = 1`.

### Z — eigener Zensus-Gegencheck (Regressionsrisiko der 152 floor-0-Items)
`zensus_itemband.py --alle --tueren` reproduziert 206 RDTs / 40674 Opcodes / 164 Item_aot_set / 12 erhoeht / 22 Member-0x12-Schreiber.
Zusaetzlich selbst (Skript `abn_d/tuer_band.py`): fuer ALLE 653 Door_aot_set gilt pc[25] (Ziel-Band des Originals) ==
-(Ankunfts-y/1800) (Port `re15_collision_band_from_y`) — **0 Abweichungen**; das Ankunftsband des Ports ist also das des
Originals. Raeume mit floor-0-Items und Nicht-0-Ankunft: nur ROOM2040/2041 (Satz sub00 @0x014F8 slot 13 mit Rechteck 0,0,0,0).
Raeume mit floor-0-Items UND Treppen: ROOM3040/3041 (Item im Erdgeschoss, R2 gemessen FREI) und ROOM5020 (einzige Ankunft
aus 5000 auf Band 0). Die vier Port-Installer (dokumente_r34, inhalt_r35, irons_tisch_1150, schluessel_r36) setzen
`g_aot.slots[].band` ausdruecklich (0 bzw. 5 fuer das 1090-Dach) — `re15_aot_set` selbst laesst `band` unangetastet.

### L1 — NEBENWIRKUNG: ROOM2030 Lift A aufwaerts aus dem Kabineneingang -> Leon sitzt oben fest (Port mit Fix = Original)
Anlass: Beleg des Bau-Agenten `D_belege/n2030_spas_weg2` (Start (6594,10846)): nach der Fahrt `(6594,10182)`, dann
`KLEMME ... Rest 3518/5864/7902`, `FLAG (9,109) = 0`; im Dossier unter OFFEN als "Startstellung des Messlaufs, kein
Mechanismus-Unterschied ... im Spiel steht man in der Kabine" abgelegt.
* Geometrie (RDT, selbst gelesen): Ausloeser slot 5 (main00 @0x01CE6, floor 3, sat 0x31 = nur Punkt 620 voraus) Rechteck
  x 5850..7350 z 11350..12850 -> ausloesbar ab z >= 10730 (Blick Sued), also AUCH im Kabineneingang. Band-6-Zellen:
  r0[49] x 5000..8000 z 10650..11150 (Kabinen-Nordwand oben), r0[48] x 5000..5500 z 8550..13050, r0[45] x -4150..5500
  z 8550..9050, r0[42] x 8000..10000 z 10400..19900, r0[55]/[56] Kabinenwaende. Die Band-6-Hauptflaeche mit der SPAS-12
  ist von der Flaeche noerdlich r0[49] vollstaendig getrennt; der obere Ausloeser slot 4 (main00 @0x01D20, floor 6,
  dasselbe Rechteck) ist von z <= 10200 aus nicht erreichbar (Punkt 620 voraus <= 10820 < 11350).
* Port mit Fix, ECHTER Anlauf (kein Teleport auf die Stelle): `bash lauf.sh l1_2030_lift_tuer plaene/l1_2030_lift_tuer.txt
  RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,-100,3072,3 RE15_EXIT_AT=2600#2030`:
  `F230 (6605,10770) angekommen` (Band 3), AKTION, itemband `F345 MEMBER 0x12 Spieler: Kollisionsband 3 -> 6 ... y=-10800`,
  `F346 spieler=(6605,10182)`, `F764 KLEMME beim Gehen nach (6600,13700): Rest 3518`, `F801 KLEMME ... (2000,14500)`,
  `F820 KLEMME ... (-650,14500)`, `F1089 FLAG (9,109) = 0`, erneuter Aktionsdruck nach Sueden: `F1610 FLAG (3,222) = 1`
  (keine Rueckfahrt). -> **Softlock: SPAS-12 unerreichbar, Lift nicht mehr ausloesbar.**
* Vorher-exe (v0.8.24-Code) gleicher Lauf: `F346 spieler=(6605,10770)` — keine Ausschiebung (Band blieb 3 auf y -10800);
  oben `F793 KLEMME beim Gehen nach (6600,13700): Rest 1368` bei z 12332 (Band-3-Wand r0[33]), `F1110 FLAG (9,109) = 0`, aber
  `F1170 (6600,10347) angekommen` = Leon kann oben "schwebend" nach Norden zurueck (Band-3-Gang) und den Raum ueber die
  Band-3-Tuer slot 0 verlassen. v0.8.24 war hier also defekt (SPAS-12 oben nicht erreichbar, dafuer von unten greifbar),
  aber OHNE Festsitzen. Mit dem Fix ist der Weg aus der Kabine heraus richtig — und der Eingang wird zur Falle.
  Gefahren (Vorher-exe, `bash lauf.sh v1_2030_oben_raus plaene/v1_2030_oben_raus.txt` mit ABN_VORHER=1, RE15_SET_FLAG=3:126,
  RE15_DEBUG_JUMP=2030@250, RE15_PLAYER_POS=6550,-100,3072,3, RE15_EXIT_AT=150#2070): Fahrt aus (6605,10770), oben
  `F898 (6562,2128) angekommen`, `F953 (6552,-92) angekommen` (climb.txt `T1237 pos=(6552,-10800,-92) band=3`), Blick Ost,
  AKTION -> `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=0 spawn=(4800,0,-6300)`, `loaded room2070.rdt`.
* **ORIGINAL selbst gemessen** (DuckStation-GDB, Auslieferungs-EXE `EXE-Stub @0x80026e4c: 0800e003`, `Zellen im RAM ==
  STAGE2/ROOM2030.RDT: True`, (3,126) gesetzt; Werkzeug = Kopie von D_belege/orig_r37_itemband.py mit Plan "2030T",
  Log `scratchpad abn_d/orig/o2030T/log.txt`): Spieler (6605,-5400,10770) Blick 3072 Band 3, Quadrat ->
  `t=3 s: {'x': 6605, 'y': -10800, 'z': 10182, 'band': 6}`; Quadrat mit Blick Sued -> 6 s `z 10182, y -10800` (keine
  Rueckfahrt); 3 s Vorwaerts -> `z 10182` (Klemme). **Das Original sitzt an derselben Stelle fest, Bild fuer Bild gleich
  (10182).** Die Port-Ausschiebung (push_rect = FUN_8003bca8) trifft auch den Gleichstand des Bau-Agenten (z 10900 ->
  10900+718 = 11618 = Original-Messwert o2030L) — kein Mechanismus-Unterschied.
* Zell-Analyse aller Lift-Fahrten mit Member 0x12 (Skript `abn_d/liftfalle.py`: Ausloesestellen frei auf dem Startband,
  Ausschiebung push_rect auf dem Zielband, Flood-Fill-Erreichbarkeit im 50er-Raster): 2030 Lift A auf **114** Fallenstellen
  (x 6350..6800 z 10750..10875 -> z 10182), 2030 Lift A ab **323** Kandidaten (x 6350..6800 z 13050..13450 -> z 13768,
  siehe L2), 2030 Lift B auf/ab 0, 3050 ab/auf 0, 50D0 ab/auf 0.
-> Die Falle ist ORIGINAL-(Beta-)Verhalten, kommt aber erst mit diesem Fix in den Port (v0.8.24: defekt, aber kein Festsitzen). Das Dossier
   stellt sie falsch dar ("im Spiel steht man in der Kabine") und der Abschnitt "Fuer den Nutzer" verschweigt sie.

### L2 — NEBENWIRKUNG: ROOM2030 Lift A ABWAERTS von knapp suedlich der oberen Kabine -> Leon sitzt unten fest (Port = Original)
Oben ist die Kabine nach SUEDEN offen (Band 6: r0[49] Nord, r0[55]/[56] Seiten), unten nach NORDEN (Band 3: r0[33] x 5400..7750
z 12800..13300 = geschlossene Suedwand, r0[34]/[35] Seiten). Der obere Ausloeser (slot 4, Punkt 620 voraus, z 11350..12850) feuert
beim Rueckweg von der SPAS-12 (Annaeherung von Sueden, Blick Nord) schon ab z <= 13470 — also VOR der Kabine.
* Port mit Fix: `bash lauf.sh l2_2030_lift_ab_sued plaene/l2_2030_lift_ab_sued.txt RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250
  RE15_PLAYER_POS=6550,-100,3072,3 RE15_EXIT_AT=3200#2030` (hoch aus der Kabine (6605,11970), oben nach Sueden bis (6605,13245),
  Blick Nord, AKTION): itemband `F361 MEMBER 0x12 ... 3 -> 6 ... y=-10800`, `F801 (6605,13245) angekommen`,
  `F936 MEMBER 0x12 Spieler: Kollisionsband 6 -> 3 (@0x80041228) y=-5400`, `F937 spieler=(6605,13768)`, `F1338 FLAG (3,222) = 0`;
  danach "GEHE (6600,12000)" bis Bild 3200 ohne Fortschritt: climb.txt `T1650 pos=(6428,-5400,13768) ... T3451 pos=(6637,-5400,13768)
  band=3` (1900 Bilder UP gegen r0[33], z bleibt 13768). -> **Softlock unten.**
* **ORIGINAL selbst gemessen** (Plan "2030D", Log `abn_d/orig/o2030D/log.txt`, Stub 0800e003, Zellen == RDT): hoch aus
  (6600,-5400,12000) -> `t=3 s ... z 12000, band 6`; dann (6600,-10800,13300) Blick 1024 Band 6, Quadrat ->
  `t=4 s: {'x': 6600, 'y': -5400, 'z': 13768, 'band': 3}`; Quadrat erneut 6 s und 3 s Vorwaerts -> `z 13768` (fest). **Identisch.**
* Flood-Fill (Skript `abn_d/komponente.py`): die Flaeche um (6600,13768) Band 3 bzw. (6605,10182) Band 6 ist die grosse
  Aussenflaeche jenseits der Raumwaende; kleinster Abstand zu Tuer slot 0 (2070) 1850, Lift A unten 850, Lift B unten 850,
  Treppe slot 8 1390, Lift A oben 1150, SPAS-12 4550 — alles > 620 = kein Ausgang. ROOM2030 hat keine Tuer auf Band 6.
* Kollisionsbloecke Leon/Elza byte-gleich (md5 der SCA-Bloecke 2030 = 2031, 3050 = 3051, 50D0 = 50D1): **die Fallen gelten
  auch fuer Elza (ROOM2031)**.

## 4. Erklaert der Fix den Befund?
Ja. Vorher-exe: Item-Satz ohne Band-Gate -> Aufnahme von Band 4 (M1) und Band 0 (M2); Fix-exe: `re15_itemband_r37_gate`
sperrt genau dort (`spieler-band 4/0 ... -> GESPERRT (@0x80042ccc bne Band)`), und auf dem vorgesehenen Weg setzt der Lift
per Member 0x12 das Kollisionsband (`MEMBER 0x12 ... 4 -> 0`), so dass Kerbe/Klettern Band 1 erreichen (`-> FREI`). Beide
Haken entsprechen den selbst gelesenen Instruktionen (Abschnitt 2); die Original-Messungen des Bau-Agenten (3050/3010/2030)
passen dazu, und meine eigene Original-Messung (L1/L2) zeigt Bild-fuer-Bild dieselben Endlagen wie der Port (10182 / 13768).

## 5. Gates
* **RE-Gate:** erfuellt — beide Konstanten belegt, 7 zitierte Stellen selbst disassembliert/dekodiert (Abschnitt 2).
  Rate-Signale im Code-Diff: `git diff master...HEAD -- re15_port | grep -i "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx"`
  -> nur eine unveraenderte Kontextzeile (`#include "re15_item_modal.h" ... deferred grant`), kein neuer Treffer. Einziger
  `getenv` = `RE15_ITEMBAND_LOG` (reiner Messschrieb, kein Verhalten).
* **Vertrag:** erfuellt — keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots/Ereignisse belegt; Pfade nur
  engine/src/itemband_r37.c, include/re15_itemband_r37.h, Haken aot_common.c (+3, Kommentar "Runde 37 Spur D") und
  actor_common.c (+3/-1, dito) = die D zugeteilten gemeinsamen Dateien; Tests nur ueber tests/unit/probes/r37_itemband.cmake;
  test_r37_itemband.cmake mit `cmake_minimum_required(VERSION 3.16)`; test_r37_itemband.c ohne POSIX-Aufrufe; kein
  release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists.
* **Tests der Spur (selbst gefahren):** `ctest -R "^unit_r37_itemband$"` Passed 0.45 s; `integration_r37_itemband` in meinem
  Lauf Passed 826.54 s (12 Laeufe).
* **Gezielte Regression (selbst gefahren):** Unit-Gruppen aller beruehrten Module
  `ctest -R "^(unit|probe)_.*(aot|item|inventar|itembox|member|band|stair|treppe|klett|climb|dachkiste|objfall|push|fahrstuhl|elevator|1170|1090|10f0|3050|2050|2060|irons_tisch|irons_diary|dokument|inhalt|schluessel|kartenitems|kartenbits|r36_karte|scd_|granate|leiche|sicherung|r37_itemband|tueren)" -j 6`
  -> **`100% tests passed, 0 tests failed out of 131`**. Integrationsgruppen (25 Tests, -j 3): siehe Abschnitt 6.
* **Nebenwirkungen:** NICHT erfuellt — L1/L2 (Abschnitt 3): zwei erreichbare harte Softlocks an ROOM2030/2031 Lift A, neu
  im Port durch den Member-0x12-Haken (Original-treu, v0.8.24 ohne).

### N1 — Nebenbefund 50D0 AUFFAHRT (vom Bau-Agenten nur im Riegel belegt) jetzt an der exe
Start an der Ankunftsstelle der 50F0-Tuer (-2800,0,-14950) Band 0, Kabine unten ((3,83) = 1, Zustand nach einer Abfahrt), Lift
slot 6 mit Frage JA, oben der Weg des Bau-Agenten rueckwaerts bis vor die Band-3-Tuer slot 0 (-> 50C0), Blick Sued, AKTION:
`bash lauf.sh n1b_50d0_auffahrt plaene/n1b_50d0_auffahrt.txt RE15_SET_FLAG=3:83 RE15_DEBUG_JUMP=50D0@250 RE15_PLAYER_POS=-2800,-14950,2048,0 RE15_EXIT_AT=200#50C0`
* Fix-exe: itemband `F503 room=50D0 MEMBER 0x12 Spieler: Kollisionsband 0 -> 3 (@0x80041228) y=-5400`, `F903 FLAG (3,83) = 0`,
  `F1029 (-19029,-11140)`, `F1114 (-23324,-10989)`, `F1250 (-23427,-3367) angekommen`, debug.log
  `[aot] DOOR FIRE slot=0 rect=(-24160,-1959,hw=1600,hh=900) target_cut=6 spawn=(-24583,0,-26199)`, `loaded room50c0.rdt`.
* Vorher-exe (Lauf `n1_50d0_auffahrt`, gleicher Weg bis F953): nach der Auffahrt bleibt Band 0 auf y -5400, climb.txt
  `T1500 ... T2700 pos=(-19xxx,-5400,-13308) band=0` — Klemme an Band-0-Waenden, die obere Ebene ist nicht begehbar.
-> Die positive Nebenwirkung (Lift setzt das Kollisionsband, obere/untere 50D0-Tueren erreichbar) ist in BEIDEN Richtungen belegt.

### L1e — dieselbe Falle mit Elza (ROOM2031) an der exe
`bash lauf.sh l1e_2031_lift_tuer plaene/l1e_2031_lift_tuer.txt RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,-100,3072,3 RE15_EXIT_AT=1700#2031`
-> `Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `loaded room2031.rdt`, `F230 (6605,10770) angekommen`, itemband
`F345 MEMBER 0x12 ... 3 -> 6 ... y=-10800`, `F346 spieler=(6605,10182)`, `F764/F801/F820 KLEMME ...`, `F1089 FLAG (9,109) = 0`.

### H — Herkunft (Nutzer: "Das war schon in v0.8.23 so")
`git show v0.8.21|v0.8.23|v0.8.24:re15_port/engine/src/aot_common.c` -> Gate-Liste Z. 1311/1314/1318 =
GENERIC/AUTO_EVENT/MESSAGE/EXAMINE_WORKVAR/FLAG_CHG, **ohne ITEM**; `actor_common.c` Z. 159 in allen drei Staenden
`case 18: a->floor = (uint8_t)value; break;` (kein Kollisionsband). -> keine Regression, Befund seit jeher vorhanden.

## 6. Integrationsgruppen (selbst gefahren, Spurbaum, -j 3)
`ctest --test-dir re15_port/build -R "^integration_.*(aot|item|inventar|itembox|member|band|stair|treppe|klett|climb|dachkiste|objfall|push|fahrstuhl|elevator|1170|1090|10f0|3050|2050|2060|irons_tisch|dokument|inhalt|schluessel|kartenitems|kartenbits|r36_karte|granate|leiche|sicherung|r37_itemband|elza_vollstart|kleidung_spielweg|r16_objekte|tueren)" --timeout 3000 -j 3 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 25`** (`Total Test time (real) = 1758.85 sec`), u.a. integration_r37_itemband
826.54 s, integration_r36_dachkiste 1390.91 s, integration_r36_klettern 512.23 s, integration_r36_inventar, _schluessel,
_schluessel_befreiung, integration_r35_inventar1050, integration_r30_irons_tisch_laden/_bild/_licht, integration_elza_vollstart,
integration_r36_tueren, integration_r35_karte_fahrstuhl. Kein Fenster-Haken musste nachgefahren werden.
Hinweis: KEIN Riegel faehrt Lift A von ausserhalb der Kabine (spas2030 startet in (6600,12000)) — die Falle L1/L2 ist
deshalb trotz voller Gruenung unentdeckt.

## 7. Urteil je Punkt
| # | Wortlaut | Urteil | Beleg |
|---|---|---|---|
| 8 | Items auf erhoehten Flaechen vom falschen Stockwerk aufnehmbar, z.B. Super Redhawk ROOM3050 ohne die Kiste — muss gefixed werden | **teilweise** | Kern erfuellt und gemessen: Super Redhawk ueber echte Tueren von oben (Band 4, M1) und von unten ohne Kiste (Band 0, M2) GESPERRT, (9,92) = 0 — Vorher-exe nahm in beiden Faellen auf; vorgesehener Weg (Fall, Lift 4->0, Kerbe, Klettern) fuer Leon UND Elza FREI, (9,92) = 1 (M3); alle 12 erhoehten Saetze im eigenen Lauf integration_r37_itemband gruen; Regressionen R1/R2 FREI; RE-Gate bestaetigt. ABER: der mitgebaute Member-0x12-Haken bringt zwei erreichbare harte Softlocks an ROOM2030/2031 Lift A in den Port (L1/L1e/L2), die v0.8.24 nicht hatte; die SPAS-12 (ein betroffenes Item) ist damit auf dem vorgesehenen Weg NICHT sicher erreichbar — Vertrag "alle betroffenen Items bleiben auf dem vorgesehenen Weg erreichbar" verletzt; Dossier verschweigt/verharmlost es. |

## 8. Maengel
1. **Softlock ROOM2030/2031 Lift A AUFWAERTS (neu durch den Haken actor_common.c case 18 -> re15_itemband_r37_member_band).**
   Ausloesen aus dem Kabineneingang (Port-Anlauf von Norden endet bei (6605,10770); Fallenbereich laut Zellanalyse x 6350..6800
   z 10750..10875, Ausloeser slot 5 feuert ab z 10730) -> nach der Fahrt (x,-10800,10182) Band 6 noerdlich der Wand r0[49]:
   SPAS-12 unerreichbar, Rueckfahrt (slot 4) nicht ausloesbar, keine Band-6-Tuer — Spiel nicht fortsetzbar. Leon (L1) und
   Elza (L1e) an der exe gemessen; v0.8.24-Code: keine Ausschiebung. Original identisch (eigene DuckStation-Messung z 10770 ->
   10182). Nachpruefbar: `abn_d/laeufe/l1_2030_lift_tuer`, `l1e_2031_lift_tuer`, `abn_d/orig/o2030T/log.txt`.
2. **Softlock ROOM2030/2031 Lift A ABWAERTS (gleiche Ursache).** Ausloesen beim Rueckweg von der SPAS-12 knapp suedlich der
   oberen Kabine (Port (6605,13245) Blick Nord; Fallenbereich x 6350..6800 z 13050..13450, Ausloeser feuert bis z 13470) ->
   unten (x,-5400,13768) Band 3 suedlich der geschlossenen Kabinenwand r0[33], ohne Ausgang (Tuer slot 0 / Lift A unten /
   Lift B / Treppe > 620 entfernt). Port gemessen (L2, 1900 Bilder Klemme), Original identisch ((6600,13300) -> 13768,
   `abn_d/orig/o2030D/log.txt`).
   Zu 1+2: Beide Fallen sind Beta-Verhalten des Originals, kommen aber erst mit diesem Fix in den Port. Nach der Projektregel
   "Beta -> Retail" (wo RE1.5 unfertig ist, gilt RE2) und dem Softlock-Ausschluss der Runde 36 (R_dachkiste: "ein vom Port
   erzeugter Softlock ist ausgeschlossen") darf der Port hier nicht festsetzen. Nachbesserung: Mechanismus mit Beleg
   (RE2-Vorbild bzw. als NUTZER-VORGABE/PORT-WAHL gekennzeichnet), ohne die byte-true Band-Logik fuer Items/Lifte wieder
   aufzugeben; die uebrigen Lifte (3050, 2030 Lift B, 50D0) haben laut Zellanalyse keine Fallenstellen.
3. **Dossier falsch/unvollstaendig.** D_itemband.md §5.5 (`n2030_spas_weg2`) und §7 OFFEN legen die im EIGENEN Lauf beobachtete
   Klemme + `(9,109) = 0` als "Startstellung des Messlaufs ... im Spiel steht man in der Kabine" ab — der Ausloeser (Punkt 620
   voraus) feuert aber ausserhalb der Kabine, und die Folge ist ein Softlock. §8 "Fuer den Nutzer" behauptet "Alle bleiben auf
   ihrem vorgesehenen Weg erreichbar" und nennt die Falle nicht. Korrigieren (mit den Messungen L1/L2 und dem Original-Beleg).
4. **Kein Riegel fuer die Lift-Ausloesung ausserhalb der Kabine.** integration_r37_itemband (spas2030) startet die Fahrt in der
   Kabine (6600,12000) und ist deshalb gruen, obwohl die Falle besteht. Nach der Nachbesserung je Richtung einen Lauf aus der
   Eingangsstellung ergaenzen (aufwaerts aus (6605,10770) -> SPAS-12 aufnehmbar; abwaerts aus (6605,13245) Blick Nord -> Tuer
   slot 0 erreichbar), mit Gegenprobe gegen den jetzigen Stand (rot).

## 9. Hinweise (kein Mangel)
* Neue Engine-Datei engine/src/itemband_r37.c: beim Android-Paketbau den GLOB-Cache (app/.cxx) neu erzeugen (bekannte Falle).
* Die vom Bau-Agenten offen gelassene 50D0-AUFFAHRT ist jetzt an der exe belegt (N1); Treppen-Endlage 2030 slot 8 (x 100 /
  z 47 Abweichung, stair_common.c) habe ich nicht nachgemessen — ohne Folge fuer die Aufnahme (beide Band 1).
* Der relayte Nutzer-Nachtrag dieser Runde (Beretta M93R, Feuerstoss trifft nur einmal) betrifft Spur G, nicht Spur D.

## 10. Ergebnis
**bestanden = false.** Punkt #8 ist im Kern umgesetzt und byte-true belegt, aber die mitgebaute Member-0x12-Kollision fuehrt
in ROOM2030/2031 zu zwei im normalen Spiel erreichbaren harten Softlocks (Original-treu, neu im Port), die das Dossier
verschweigt und kein Riegel abdeckt (Maengel 1-4).
