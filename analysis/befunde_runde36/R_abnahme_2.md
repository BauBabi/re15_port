# Runde 36 — Spur R "dachkiste" — Abnahme 2 (unabhaengig, 2026-10-11, Nachbesserung 2)

Baum `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste` @ 2083f1d8 (Basis 99398dc7; Abnahme 1 = 47384115 auf
01248da5). Gebaut mit `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (configure)` / `=== LOCAL-BUILD-OK
(build)` (ninja: no work to do — die exe 20:12:48 ist der Bau des Bau-Agenten aus demselben Quellstand, HEAD 20:54:34
aendert nur Dossier/Belege; sha256 `9cdba9aac0d2c69f…`, != Abnahme 1 `13ffe6a5…`, weil der Wegplan-Treiber neu
eingebaut ist). Messkopie `re15_pc_r36abn2.exe` neben der exe, Arbeitsordner im Scratchpad (`abn2/<lauf>/`),
Startskript = Kopie von `R_belege/lauf_nb2.sh` (Grundumgebung unveraendert). Der Bericht wird fortlaufend geschrieben.

Wortlaut (Orchestrator): (1) an der exe messen, ob Leon/Elza die Dach-Kiste obj 1 in ROOM1090/1091 bis zum Fall-Ausloeser
schieben koennen (echter Weg, jede Richtung); (2) am Original messen; (3) blockt das Original -> Port blockt an derselben
Stelle mit demselben Mechanismus (Beleg, Pin), sonst byte-true + Grenze benennen + Pin; (4) **dieselbe Pruefung** fuer
ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste? Abnahme 1 hatte Punkt 4 mit den
Maengeln N1 (11A0-Zugang zirkulaer), N2 (Ersatzweg 2020 im Port gesperrt), N3 (2050-Fall weder exe noch Original
gemessen), N4 (zensus_diag.txt veraltet), N5 (3050 am Original offen) als "teilweise" gewertet; die Nachbesserung 2
(93c37036 … 2083f1d8) behauptet, alle fuenf behoben zu haben.

## Code-Diff (git diff 99398dc7 -- re15_port, wirklich gelesen)

Gegenueber Abnahme 1 NEU: `engine/src/dachkiste_r36.c` +~310 Zeilen Wegplan-Treiber `RE15_WEGPLAN` (Befehle OBJ/RAUM/
WARTE/GEGNERWEG/ZELLE/GEHE/BLICK/SCHIEB/AKTION/AOTS/ENDE; liest je Bild Spielerlage/Blick/Objektlage und erzeugt Pad-
Bits; ohne Env `s_wp_n = -1; return 0` beim ersten Aufruf), `platform/pc/src/input_pc.c` 2-Zeilen-Haken hinter der
RE15_INPUT_SCRIPT-Ersetzung (`(void)re15_dachkiste_wegplan_pad(&bits)` — ueberschreibt `bits` nur, wenn ein Wegplan
aktiv ist), `include/re15_dachkiste_r36.h` (Prototyp). Unveraendert: `aot_common.c` 3-Zeilen-Haken (`gesperrt` halten +
Log-Aufruf, gleiche Verzweigung @0x8002bfb8/@0x8002c0b0). Tests: `probe_r36_dachkiste.c` (2050/2051 `soll = 1` + Rechteck-
Pin r*[49] +-450), `test_r36_dachkiste.cmake` (Laeufe `fall2050`, `weg2020` mit Wegplan-Dateien), `probes/r36_dachkiste.
cmake` (Kommentare). GEGNERWEG ruft `re15_actor_free` fuer alle Aktoren `type != 0` — nur im Wegplan, nur Messung.
Env-Schalter im Diff: `RE15_PUSH_LOG`, `RE15_WEGPLAN`, `RE15_WEGPLAN_LOG` (alle Messschrieb/Mess-Steuerung, kein Spiel-
Code liest sie). Guess-Tells (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/HACK/sieht richtig) im
re15_port-Diff: KEINE Treffer. **Kein Verhaltens-Code geaendert — bestaetigt.**

## Gates

### Suite (gezielt, kein voller Lauf im Spurbaum)
`ctest --test-dir re15_port/build -R "unit_r36_dachkiste_zensus|unit_r36_objfall|unit_r36_klettern|unit_1090_|prop_sca_collision|prop_point_in_poly|prop_zone_dispatch|unit_r22_10f0_figur|unit_re2_room1090_ab|unit_plc_back_yaw_1090|probe_1090_|unit_map_1090|unit_r35_cut10f0|unit_r36_szenenleon_abgang10f0" --timeout 900`:
```
100% tests passed, 0 tests failed out of 31   (Total Test time (real) = 17.65 sec; unit_r36_dachkiste_zensus 8.83 s)
```
Waehrend der Laeufe lief eine fremde exe einer Parallel-Spur (`re15_pc_r36p_leon_*`) — Last, nicht meine.
`integration_r36_dachkiste` (acht Laeufe): eigener Lauf s. u. (Abschnitt "Riegel").

### RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, info/Re1.5/PSX.EXE)
Die Kette FUN_8002bd44 / FUN_8003b558 / FUN_8003b7f0 / Band-Wildcard / Treppen-count war in Abnahme 0/1 disassembliert
und stimmt. Neu zitierte Stellen der Nachbesserung 2, alle selbst nachgelesen:
* **Sca_id_set-Handler LAB_8004175c** (`dis 0x8004175c 22`): `8004175c lw a2,28(a0)` (SCD-PC), `80041768 lbu v1,1(a2)`
  (Region), `8004176c lw v0,32(v0)` (Tabelle hinter 0x800ac778), `80041774 addiu v1,v1,1` / `80041778 sll v1,v1,2` (Index
  Region+1), `80041770 lbu a1,2(a2)` + `80041780 sll v0,a1,1` / `80041784 addu v0,v0,a1` / `80041788 sll v0,v0,2` (= Index x
  12), `8004178c lw v1,0(v1)` (Zellenfeld), `80041790 lbu a1,3(a2)` (Wert), **`8004179c sb a1,9(v0)`** = Byte +9 der
  12-Byte-Zelle = u0 — stimmt mit dem Dossier (N2) und mit `op_sca_id_set` (scd_vm.c:4700 `e->u0 = t->pc[3]`).
* **Schub angenommen/gesperrt** (`dis 0x8002bfa4 20`, `dis 0x8002c0a0 16`): `8002bfb0 jal 0x8003b558` / `8002bfb4 ori a1,2`,
  `8002bfb8 bne v0,zero,0x8002c0b0`, **`8002bfc0 sb v0(=8),140(s2)`** (= Haltepunkt SCHUB-OK), **`8002c0b0 sb s7,140(s2)`**
  (= SCHUB-GESPERRT), Ruecksetzung `8002c0c4 sw v0,52(s2)` / `8002c0d4 sw v0,60(s2)` aus s4/s5 — die Haltepunkte des
  Original-Skripts liegen auf den richtigen Instruktionen.
* **Fall-Ausloeser** (`dis 0x8002c0fc 22`): `8002c128 jal 0x8003b7f0` mit `8002c12c ori a1,zero,0x1c2` (450), `8002c130
  andi v0,v0,0x2`, `8002c134 beq`, **`8002c13c jal 0x8002d3d8`** (= Haltepunkt FALL-AUSLOESER), `8002c148 sb 1,4(s2)`: stimmt.
* **Entity-Schleife FUN_8001a50c** (`dis 0x8001a50c 48`): `8001a528 lbu s1,-13746(s1)` = 0x800b0000-0x35B2 = **0x800ACA4E**
  (g_active_count), `8001a55c lw v0,0(v1)` / `8001a564 andi v0,v0,0x1` / `8001a568 beq v0,zero,0x8001a5a0` (Aktiv-Bit),
  `8001a570 lbu v0,8(v1)` (Typ) / `8001a580 lw v0,0(v0)` / `8001a588 jalr v0` (Handler UNGEPRUEFT — Eintrag 0 waere jalr 0),
  **`8001a58c addiu s0,s0,1` im Delay-Slot** (zaehlt nur aktive), `8001a5b0 andi v0,s0,0xff` / `8001a5b4 sltu v0,v0,s1` /
  `8001a5b8 bne -> 0x8001a550`: ohne Zaehlerkorrektur laeuft die Schleife endlos (s0 bleibt 0 < s1) — das erklaert den
  gemessenen Freeze (orig_2050_diag2_ent) und die Korrektur (ent2). Gegner-Neutralisierung am Original ist belegt.
* **FUN_80042bac** (`dis 0x80042bac 20`): `80042bd0 ori v0,zero,0x26c` (= 620) -> `80042bf0 sh v0,16(sp)`: "Punkt 620
  voraus" stimmt. `8003da78` = Zellen-Routine (a0 = Zelle), wie in Abnahme 0.

### RDT-/SCD-Belege (xxd + `re15_port/tools/scd_dump_room.py`, selbst)
* ROOM2020 sub00: @0x0954 `Ck (3,114)=0` -> @0x0958 `Aot_set slot 3 sce 1` (Text) + **@0x098E-099E `37 00..04 0f ff`**;
  Else -> @0x09AA `Ck (3,113)=0` -> **@0x09AE `Aot_set slot 3 sce 3 flags 0x31 floor 1 rect (2200,-4150,800,800)`** +
  **@0x0A06-0A16 `37 0x 0f ff`**; Else ((3,113)=1): obj 2 an der Stelle @0x0A1E, `Sca_floor_set 0x10` @0x0A40-0A50, KEIN
  Sca_id_set (u0 bleibt RDT 0x00). sub02 @0x0A5C: `Message_on 0` / `Evt_next` / `Message_on 1` / `Evt_next` / `Ck (12,31)=0`
  -> `Aot_reset 3`, `Set (3,113)=1` @0x0A7A, `Cut_chg 3` @0x0A7E, … `Cut_chg 4` @0x0ABC, … **@0x0ADE-0AEE `37 0x 0f 00`**,
  **@0x0AF2-0B02 `39 0x 0f 10`**. r0[15] @0x004F8 = `34 08 ee 1b d2 f6 f6 f0 01 00 00 13` = x -2350 w 2100, z -3850 d 7150,
  typ 1, u0 0x00, u1 0x00, fl 0x13. Alles wie im Dossier.
* ROOM2010 sub02 @0x008CA `22 03 72 01` = `Set (3,114)=1` hinter `Ck (12,31)=0` @0x008C6; Ereignis `Aot_set slot 4 sce 3`
  @0x0880 unter `Ck (3,114)=0` @0x087C. Eigener Scan aller STAGE1-3-RDTs auf `22 03 72 01`: NUR ROOM2010/2011. Stimmt.
* ROOM2050 sub00 **@0x01E94-1EA4 `37 0x 32 00`** (r*[50] u0 = 0), sub02 @0x02294 `37 0x 33 00` / @0x022A8 `37 0x 32 ff`.
  r0[49] @0x00B5C = `60 09 08 07 ce 18 82 c9 01 01 02 32` = x 6350 w 2400, z -13950 d 1800, u0 0x01 u1 0x02 fl 0x32
  (Wort 0x3202); r0[50] @0x00B68 = x 6650 w 1900, z -12150 d 500, u0 0xff (RDT) -> Laufzeit 0x00 durch sub00. Stimmt.
* **ROOM2051.RDT ist byteidentisch mit ROOM2050.RDT** (`cmp`): Elzas Raum traegt dieselben Records/SCA/SCD.

### N4 (zensus_diag.txt): eigener Lauf `probe_r36_dachkiste.exe diag` -> `diff` gegen `R_belege/zensus_diag.txt`:
**IDENTISCH** (1090/1091 12 Zellen / 0 Ausloeser; 3050/3051 13373 / 8; 2050/2051 3898 / 1). Behoben.

### Vertrag
* Bank-9-Bits/Nachrichten-IDs/Ereignisse: keine. Gelesen/gesetzt nur (9,71) und (3,114) im Riegel (Messhaken via
  RE15_SET_FLAG, vorhandene RDT-Flags).
* Pfade (git diff 99398dc7 --name-only): analysis/befunde_runde36/R_* (+ R_belege/*, darunter room2050_orig.sav 1,6 MB),
  engine/src/aot_common.c, engine/src/dachkiste_r36.c, include/re15_dachkiste_r36.h, platform/pc/src/input_pc.c,
  tests/unit/probe_r36_dachkiste.c, tests/unit/probes/r36_dachkiste.cmake, tests/integration/test_r36_dachkiste.cmake.
  Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists: eingehalten. input_pc.c = gemeinsame
  Datei, 2-Zeilen-Haken mit Kommentar "Runde 36 Spur R": erlaubt. Messkopie eigener Name, kein taskkill,
  RE15_WINDOW_SCALE=1 im Riegel.


## Riegel `integration_r36_dachkiste` — eigener Lauf (acht Laeufe, Laufordner `re15_port/build/r36_dachkiste_wd/lauf_1dc98da6/`)
`ctest --test-dir re15_port/build -R integration_r36_dachkiste --timeout 1800 --output-on-failure`:
```
1/1 Test #572: integration_r36_dachkiste ........   Passed  805.32 sec
100% tests passed, 0 tests failed out of 1
```
Inhalt der acht Laeufe (jede Datei selbst gelesen, nicht nur der ctest-Status):
* `leon` (PL00, ECHTER WEG): `loaded room10f0.rdt` -> `[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500) target_cut=0
  spawn=(-13600,-9000,1300)` -> `loaded room1090.rdt`; push.txt: **3x `room=1090 p=1 obj=1 SCHUB-GESPERRT … lage=(-5960,-15482)
  ziel=(-5960,-15556) band=5`** je mit `TREFFER-ZELLE r3[4]: x -7256..4624 z -18560..-16403 … u0 0xff … fl 0x53`, **0x
  `room=1090 p=1 obj=1 SCHUB-OK`**, objfall.txt 0x `room=1090 p=1 AUSLOESER`, letzte Lage `p1 (-5960,-9000,-15482) b5`. (Die
  366 SCHUB-OK der Datei gehoeren dem Tritt in ROOM10F0, dazu 1x `TREFFER-ZELLE r0[5]` in 10F0.)
* `elza` (PL04, `Spieler-Familie PL04 (character=4, Elza-Bit=1)`, ROOM10F1 -> ROOM1091): dieselben Zahlen — 3x GESPERRT an
  r3[4] bei ziel z -15556, 0 SCHUB-OK, 0 AUSLOESER, `p1 (-5960,-9000,-15482) b5`.
* `west` (Tasche (-4610,-15155) Blick 2112): 2x `SCHUB-GESPERRT … ziel=(-6034,-15482)` mit `TREFFER-ZELLE r3[2]: x -9200..-6878
  z -18524..265`, 0 OK, 0 AUSLOESER, `p1 (-5960,-9000,-15482) b5`.
* `fall3050`: 425x `room=3050 p=1 obj=1 SCHUB-OK`, 1x GESPERRT (`ziel=(-24108,-20800)`, `TREFFER-ZELLE r3[31]: x -27000..-25000`
  = Westwand), `F1383 … AUSLOESER wort=0x4002 band=4 pos=(-24058,-7200,-18488)`, `BODEN FUN_8001c6e8=0 band 4 -> 0`,
  `LANDUNG y=0 band=0`, Ende `p1 (-24058,0,-15561) b0 r(-1029,0,0)`.
* `weg2040`: `loaded room2070.rdt` -> `DOOR FIRE slot=5 rect=(5500,-19200,hw=500,hh=1000) target_cut=12 spawn=(-19750,-9000,
  -19850)` -> `loaded room2040.rdt`. `treppe2040`: `[STAIRREG] slot=5 type=13 … count=2`, climb.txt `T951 room=2040 …
  pos=(-1359,-5400,-10068) … band=3`.
* **`fall2050` (NEU):** wegplan_log `Gegner Slot 1/2 Typ 0x25 … freigegeben`, `ZELLE r0[50] LAUFZEIT: x 6650..8550 z -12150..
  -11650 typ 1 u0 0x00 u1 0x00 fl 0x33`, `ZELLE r0[49] … u0 0x01 u1 0x02 fl 0x32`; `SCHIEB: Bedingung erfuellt (obj.z = -7037)`
  (Sued, F195), `… (obj.x = 7571)` (Ost, F1224), `Objekt-y -5400 -> -5392 (Fall)` (Nord, F1625); push.txt **1120x `room=2050
  p=0 obj=6 SCHUB-OK`, 0x GESPERRT**; objfall.txt `F1623 room=2050 p=0 AUSLOESER wort=0x3202 band=3 pos=(7571,-5400,-11706)
  spieler=(7380,-10356) richt=(0,-1)`, `F1624 … zust 0->1`, `F1636 BODEN FUN_8001c6e8=-3600 band 3 -> 2`, `F1640 LANDUNG
  y=-3600 band=2`, Ende `p0 (7571,-3600,-13681) b2 r(-1038,0,2048)`. Bildgleich mit dem Beleg des Bau-Agenten
  (`R_belege/nb2_2050fall_c`: F1624/F1625, dieselben Lagen) — der Lauf ist deterministisch.
* **`weg2020` (NEU):** `ZELLE r0[15] LAUFZEIT: … u0 0xff u1 0x00 fl 0x13` (F61, Raumstart) -> `[msg] room=2020 id=0`, `id=1`,
  `[scd F422] Cut_chg(3)`, `F550 Cut_chg(4)`, `F620 Cut_chg(3)` -> `ZELLE r0[15] LAUFZEIT: … u0 0x00 u1 0x00 fl 0x10` (F1020)
  -> `[msg] room=2020 id=2` -> `[aot] DOOR FIRE slot=1 rect=(-14200,3300,hw=1000,hh=500) target_cut=13 spawn=(-450,-16200,
  -9700)` -> `loaded room2030.rdt`.
Der Beleg des Bau-Agenten `R_belege/nb2_2020durchgang_f` wurde ebenfalls selbst gelesen (wegplan.txt: u0 0xff -> Szene ->
u0 0x00/fl 0x10, Spieler DURCH die Luecke (-1291,2290) -> (-14222,2832); debug.log: msg 0/1/1, Cut_chg 3/4/3, msg 2, DOOR
FIRE slot=1, `loaded room2030.rdt`, `EXIT_AT: Bild 300 in Raum 2030`).

## Eigene exe-Laeufe (Messkopie `re15_pc_r36abn2.exe`, Wegplan-Treiber, Scratchpad `abn2/<lauf>/`)

### A1 — ROOM2050 ueber den ECHTEN WEG (Leon: ROOM2000 Band 1 -> Tuer slot 4 @0x0169E -> ROOM2050) — Lauf `echt2050`
Umgebung wie `R_belege/lauf_nb2.sh`, `RE15_DEBUG_JUMP=2000@250 RE15_PLAYER_POS=-26100,8500,3072,1 RE15_EXIT_AT=4200`,
Wegplan: RAUM 2000, GEGNERWEG, GEHE (-26250,10300), BLICK 2048, AKTION x2, RAUM 2050, GEGNERWEG, ZELLE 50/49, dann der
Weg des ORIGINAL-Skripts `orig_2050_push.py` (GEHE (9100,-6300) -> (-100,-6300) -> (-100,-8400) -> (-100,-11200) ->
(-1000,-10800)) und die drei Schuebe wie im Riegel. Ergebnis:
* debug.log: `[parity] JUMP-Spawn ersetzt durch RE15_PLAYER_POS (-26100,8500) rot=3072 band=1`, `loaded room2000.rdt`,
  **`[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)`**, `loaded
  room2050.rdt` — die echte Tuer 2000 -> 2050 (Punkt 620 voraus, FUN_80042bac @0x80042bd0) zuendet, Ankunft (9100,-5400,
  4100) Band 3 wie im Original-Spielstand. wegplan.txt: `Gegner Slot 1/2 Typ 0x25 … freigegeben` (Spinnen).
* GEHE (9100,-6300): angekommen (9100,-6175) (Ostkorridor, wie das Original (9017,-6393)). **GEHE (-100,-6300): der
  Spieler bleibt bei x 3402 stehen** — climb.txt `T861 room=2050 … pos=(3402,-5400,-6634) rot=2032 band=3 … pad=0010`,
  danach bis T4200 x = 3402 bei z -6717..-5975 (UP gehalten, seitliches Rutschen in +z), kein Ankommen, objfall.txt F4200
  `spieler=(3402,-5400,-6178)`. **3402 = 2952 + 450** = Ostkante der Box der Band-0-Kiste `p2 (2052,0,-5552) b0`
  (ROOM2050 sub00 @0x01DB4 `Obj_model_set obj=1 type=4 band=0 pos=(2052,0,-5552) box c(0,-900,0) h(900,900,900)` ->
  x 1152..2952, z -6452..-4652) plus Spieler-Radius 450 (PR, FUN_8003b0a4). Der Spieler steht auf Band 3 (y -5400),
  die Kiste auf Band 0 (y 0, Oberkante -1800) — **der Port schiebt den Band-3-Spieler aus einer Band-0-Kiste heraus.**
* Dasselbe mit Elza (Lauf `elza2051`, `RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=2051@250 RE15_PLAYER_POS=9100,4100,1024,3`):
  `Spieler-Familie PL04`, `loaded room2051.rdt`, GEHE (9100,-6300) angekommen (9100,-6175), dann Klemme bei
  `pos=(3402,-5400,-6717)` bis F3600, `p2 (2052,0,-5552) b0` — identisch.
* **Original (R_belege/orig_2050_d/log.txt, Bau-Agent; Auslieferungs-EXE):** `GEHE (9100,-6300): angekommen bei
  (9017,-6393)`, **`GEHE (-100,-6300): angekommen bei (-166,-6289), Schub-Ereignisse unterwegs 0`** — Leon laeuft auf
  Band 3 bei z -6300..-6393 durch die x-Spanne 2952..1152, also DURCH die Grundflaeche der Band-0-Kiste (z -6452..-4652).
  Im Original steht die Band-0-Kiste dort ebenfalls (sub00 @0x01DB4 unbedingt unter (3,109)=0; eigener Pool-Dump am
  Original s. Abschnitt Original). **Port != Original auf dem echten Weg.**
* **Mechanismus (selbst disassembliert, `dis 0x8002cabc 60`, `dis 0x8002cba8 40`):** die Ausschiebung FUN_8002cabc
  (Aufruf @0x8002c0e4 mit a2 = 0 fuer den Spieler, @0x8002be40 fuer Aktoren) prueft nach der x/z-Broadphase
  (@0x8002cb50-cb70) fuer a2 != 2 (@0x8002cb78-cb7c) die HOEHE: `8002cb84 lh v1,2(t1)` (objbox.cy) / `8002cb88 lw
  a1,56(t0)` (obj.y) / `8002cb8c lh v0,2(t2)` (actorbox.cy) / `8002cb90 lw a0,56(a3)` (actor.y) / `8002cb94-98 addu`
  (Mitten) / `8002cb9c lhu a1,8(t2)` (actorbox.hy) / `8002cba0 lhu a0,8(t1)` (objbox.hy) / `8002cba4 subu t6,v1,v0`
  (dy) / `8002cba8 addu a1,a1,a0` (hy-Summe) / `8002cbac addu t3,t6,a1` / `8002cbb0 sll v0,a1,1` / `8002cbb4 sltu
  v0,v0,t3` / `8002cbb8 xori t3,v0,0x1` / `8002cbbc bne t3,zero,0x8002cbcc` (ueberlappt -> weiter) / **`8002cbc4 j
  0x8002cde4` mit `addu v0,zero,zero`** (keine Y-Ueberlappung -> Rueckgabe 0, KEINE Ausschiebung). Der Port
  (`re15_collision.c:946 push_out_of`, aufgerufen aus `re15_collision_objects` :974) rechnet nur x/z (`adx > sumX`,
  `adz > sumZ`) und hat **keinen Y-Test** — er behandelt jede aktive Kiste band-blind. Spieler y -5400 (Band 3) gegen
  Kiste y 0 / cy -900 / hy 900: |dy| >= 3600 > 900 + actorbox.hy — im Original kein Treffer.
* Folge fuer diese Spur: die Kiste obj 6 bleibt im Port erreichbar (Korridor z -8050..-6452 noerdlich der Band-0-Kiste
  ist frei; Lauf `echt2050b` mit Umweg z -7250 s. u.), die Schubkette selbst ist davon unberuehrt — aber der im
  Original gemessene Weg ist im Port NICHT gangbar, und die exe-Messung des Bau-Agenten (Spieler per RE15_PLAYER_POS
  hinter die Kiste gesetzt) hat den echten Weg nicht gefahren. Kein Code dieser Spur (re15_collision.c), aber eine
  Port-Abweichung, die das "Port = Original" fuer ROOM2050 auf den Schub beschraenkt (s. Maengel).

### A2 — ROOM1090/1091 Dach-Kiste mit dem Wegplan-Treiber von Sueden (Punkte 1-3, zusaetzlich zum Riegel)
Lauf `1090nord` (`RE15_DEBUG_JUMP=1090@250 RE15_PLAYER_POS=-5960,-12800,1024,5`, Wegplan GEHE (-5960,-12800) -> SCHIEB 1024
Z <= -16000, zweimal): push.txt **2x `room=1090 p=1 obj=1 SCHUB-GESPERRT … lage=(-5960,-15482) ziel=(-5960,-15557)` bzw.
`ziel=(-5960,-15556)`** je mit `TREFFER-ZELLE r3[4]: x -7256..4624 z -18560..-16403 … u0 0xff`, 0 SCHUB-OK, 0 AUSLOESER,
Ende `p1 (-5960,-9000,-15482) b5`; der Treiber meldet `SCHIEB: Spieler steht 40 Bilder (Klemme)` bei (-5960,-14132) —
nach dem gesperrten Schub steht der Spieler an der Kiste (Ausschiebung @0x8002c0d8), genau wie im Original (orig_lauf6
NORD: Ziel z -15558, Treffer 0x80123984 = r3[4]). Elza (`1091nord`, `RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=1091@250`, `Spieler-Familie PL04`): bildgleich —
F88 `room=1091 p=1 obj=1 SCHUB-GESPERRT … ziel=(-5960,-15557)`, F288 `… ziel=(-5960,-15556)`, beide `TREFFER-ZELLE r3[4]`,
0 SCHUB-OK, 0 AUSLOESER, Ende `p1 (-5960,-9000,-15482) b5`. Punkte 1-3 gelten fuer beide Szenarien unveraendert.

### A3 — ROOM2010 Hinweis-Ereignis (Voraussetzung des 2020-Durchgangs, (3,114)) — Lauf `2010hinweis`
`RE15_DEBUG_JUMP=2010@250 RE15_PLAYER_POS=4200,-1050,2048,0 RE15_MSG_LOG=1 RE15_SCD_TRACE=1`, Wegplan AOTS / BLICK 2048 /
AKTION x3 / AOTS. Vorher `AOT slot 4 typ 0 ev 2 flags 0x31 band 0 mitte (3200,-1050) halb (500,500)` (= Aot_set slot 4
sce 3 @0x0880, Rechteck 2700..3700 / -1550..-550). Nach dem ersten Druck `[msg] room=2010 id=0`, `id=0`, `id=1`; SCD-Trace
des Ereignis-Threads: `F86 slot=10 @0x08BC op=0x2B` (Message_on), `F180 @0x08C0 op=0x02` (Evt_next), `F181 @0x08C2 op=0x06`
(Ifel), **`@0x08C6 op=0x21` (Ck (12,31)=0), `@0x08CA op=0x22` (Set (3,114)=1), `@0x08CE op=0x46` (Aot_reset 4)**, `F182
@0x08E4 op=0x2B`; nachher `AOT slot 4 typ 11 ev 2 flags 0x00` (zurueckgesetzt). Der Hinweis, den das 2020-Raetsel
voraussetzt, ist im Port an der exe erreichbar und setzt (3,114) genau an den im Dossier zitierten Offsets.

### A4 — ROOM2050 ECHTER WEG mit Umweg noerdlich der Band-0-Kiste — Lauf `echt2050b` (Leon)
Wie A1, nur GEHE (9100,-6300) -> **(7000,-7250) -> (-100,-7250)** -> (-100,-8400) -> … (Korridor z -8050..-6452 noerdlich der
Band-0-Kiste ist frei). Ergebnis: `DOOR FIRE slot=4 … spawn=(9100,-5400,4100)`, `loaded room2050.rdt`, alle GEHE
angekommen; SCHIEB Sued `Bedingung erfuellt (obj.z = -7036)` (F775), Ost `… (obj.x = 7558)` (F1872, im Fenster [7550,7650)),
Nord ab F2024; push.txt **1176x `room=2050 p=0 obj=6 SCHUB-OK`, 0x GESPERRT**; objfall.txt **`F2257 room=2050 p=0 AUSLOESER
wort=0x3202 band=3 pos=(7558,-5400,-11705) spieler=(7440,-10355) richt=(0,-1)`**, `F2258 zust 0->1`, `F2270 BODEN
FUN_8001c6e8=-3600 band 3 -> 2`, `F2274 LANDUNG y=-3600 band=2 pos=(7558,-13640)`, Ende `obj0=(7558,-3600,-13680) b2`.
**Ueber die echte Tuer aus ROOM2000 erreicht Leon die Kiste und schiebt sie ueber die Kante** — derselbe Ausloeser
(Absprungzelle r*[49] +450, @0x8002c12c), dieselbe Landehoehe wie im Riegel (`fall2050`) und wie im Original
(`orig_2050_d`: Ausloeser (7562,-5400,-11732), Landung (7562,-3600,-13088)). Fuer Punkt 4 gilt damit an der exe: der
Fall ist ueber den echten Weg erreichbar (mit dem in A1 beschriebenen Umweg).

**Nachtrag zu A1 (Zahlen aus dem Original-Spielstand `R_belege/room2050_orig.sav`, selbst gelesen mit re15_ss.py):**
Spieler (9100,-5400,4100) Band 3, Box-Zeiger +0x78 = 0x80073e94 -> `{0,-1530,0,450,1530,450}` (cx,cy,cz,hx,hy,hz); Pool 1 =
`typ 4 pos (2052,0,-5552) band 0 box {0,-900,0,900,900,900}` (= sub00 @0x01DB4, im Original-Spielstand vorhanden), Pool 6 =
obj 6 (-1600,-5400,-9348) Band 3. Y-Test FUN_8002cabc: actorCY = -5400 + (-1530) = -6930, objCY = 0 + (-900) = -900, dy =
objCY - actorCY = 6030, hy-Summe = 1530 + 900 = 2430, dy + hy-Summe = 8460, 2 x hy-Summe = 4860: `sltu 4860 < 8460` = 1 ->
`xori` = 0 -> `j 0x8002cde4`, v0 = 0 — **im Original keine Ausschiebung** (Leon lief gemessen bei (9017,-6393) -> (-166,
-6289) hindurch). Im Port (`push_out_of`, nur adx/adz) Klemme bei x 3402 = 2952 + 450. Nebenwirkung derselben Luecke:
`re15_collision_objects_actor` (NPC/Gegner) hat ebenfalls keinen Y-Test (Original @0x8002be40 ruft dieselbe Funktion
mit a2 = 0, der Y-Test gilt dort genauso).

### A5 — ROOM2051 (Elza) von der Tuer-Ankunft mit Umweg — Lauf `elza2051b`
`RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=2051@250 RE15_PLAYER_POS=9100,4100,1024,3` (= Ankunft aus ROOM2000 slot 4),
Wegplan wie A4 ab RAUM 2051: `Spieler-Familie PL04`, `loaded room2051.rdt`; SCHIEB Sued `Bedingung erfuellt (obj.z =
-7004)` (F682), Ost `… (obj.x = 7575)` (F1748), Nord ab F1899; push.txt **1167x `room=2051 p=0 obj=6 SCHUB-OK`, 0x
GESPERRT**; objfall.txt **`F2149 room=2051 p=0 AUSLOESER wort=0x3202 band=3 pos=(7575,-5400,-11716) spieler=(7463,-10366)
richt=(0,-1)`**, `zust 0->1`, `BODEN … band 3 -> 2`, `F2166 LANDUNG y=-3600 band=2 pos=(7575,-13645)`. ROOM2051 ist
byteidentisch mit ROOM2050 (cmp), und Elza erreicht dieselbe Absprungzelle und denselben Fall — Punkt 4 gilt fuer beide
Szenarien (die Sonde pinnt 2050 und 2051, der Riegel faehrt nur Leon).

## Original (DuckStation-GDB, Auslieferungs-EXE, eigene Laeufe; Skripte im Scratchpad = Kopien von R_belege/orig_2050_push.py)

### O1 — ROOM2050 obj 6, eigene Wiederholung (`orig_2050_abn2`, Spielstand `R_belege/room2050_orig.sav` JUMP 205)
`EXE-Stub @0x80026e4c: 0800e003 (Auslieferung)`; Pool-Dump am Start: `Pool 1: pos (2052,0,-5552) … band=0` (die Band-0-Kiste
aus A1 steht im Original an derselben Stelle), `Pool 6: (-1600,-5400,-9348) band=3`; Spieler (9100,-5400,4100) Band 3;
Spinnen `Entity 0/1 … typ 0x25 … Aktiv-Bit 0 geloescht`, `g_active_count 2 -> 0`. **`GEHE (9100,-6300): angekommen bei
(9002,-6417)`, `GEHE (-100,-6300): angekommen bei (-125,-6291)`** — Leon laeuft auf Band 3 bei z -6291..-6417 durch die
Grundflaeche der Band-0-Kiste (x 1152..2952, z -6452..-4652) hindurch, keine Ausschiebung (Gegenstueck zur Port-Klemme in
A1). SUED: 124 SCHUB-OK, 0 gesperrt (Kistenmitte -9348 -> -7040); OST: 425 SCHUB-OK, 0 gesperrt (x -1600 -> 7580, im Fenster
[7550,7650)); NORD: 221 SCHUB-OK, 0 gesperrt, **`FALL-AUSLOESER obj6=(7580,-5400,-11708) spieler=(7283,-5400,-10358)`**
(Halt @0x8002c13c), danach `faellt=1`, Band 3 -> 2, Endlage `obj6=(7580,-3600,-13088) faellt=0 band=2`; `Ereignisse gesamt:
771 Fall-Ausloeser: 1 HP: 100`; `settings.ini zurueckgeschrieben (EnableGDBServer = false: True)`. Reproduziert den Beleg
des Bau-Agenten (`orig_2050_d`: 117/432/230, Ausloeser (7562,-11732), Landung (7562,-3600,-13088)) unabhaengig: **Port =
Original fuer den Schub und den Fall der 2050-Kiste; Landung z -13088 (Original) gegen -13680/-13681 (Port) bleibt die
in OFFEN notierte Abweichung der Fallbahn (Spur A).**

### A6 — ROOM2060, letztes Glied des "Ersatzwegs" (Tritt obj 2 aus der Startlage in die Kerbe -> Tuer slot 1 -> 2040) — Laeufe `2060echt`, `2060echt2`
Das Dossier (4.3, "Fuer den Nutzer", OFFEN) stuetzt das 2060-Glied allein auf Spur A; Spur A hat dort aber NUR den Aufstieg
vom bereits gerasteten Tritt ((3,172)=1, Record @0x00ECC (475,0,-7375)) in einer Sonde gemessen (A_klettern.md Z. 231) — nicht
den Schub aus der Startlage @0x00EA6 (-3125,0,2075) in die Kerbe slot 2 @0x00E92 (x -550..1500 z -8300..-6250), der (3,172)
ueber sub01 @0x0113E-1156 (`Ck (3,172)=0 / Work_set 3,2 / Member_cmp 15 == 2 / Set (3,172)=1`, derselbe Latch wie 3050) setzt,
und nicht die Tuer. Eigene Laeufe (`RE15_DEBUG_JUMP=2060@250 RE15_PLAYER_POS=7600,3950,2048,0` = Ankunft aus 2030 slot 3,
Spinnen per GEGNERWEG weg):
* `2060echt`: Weg Ost -> Nordgasse -> Westgasse; der Spieler lief bei x -3100 von NORDEN in den Tritt (Westgasse x -5600..
  -2100, Tritt-Box x -3800..-2450 laesst oestlich nur 300 frei) und schob ihn nach SUEDEN: push.txt `F534 room=2060 p=2 obj=2
  SCHUB-GESPERRT … lage=(-3125,2075) ziel=(-3125,2149)` + `TREFFER-ZELLE r0[9]: x -2400..3150 z 2150..5350` (Planfehler, kein
  Befund). Die exe lief dabei ueber die Stelle (-2328,-7336) des Haengers aus R_abnahme_1.md ohne Einfrieren (T4051 Bilder,
  `EXIT_AT: Bild 3800`).
* `2060echt2` (westlich am Tritt vorbei, von Sueden bei (-3147,3298) Blick 1024 = Nord): **jeder Schub nach Norden ist
  gesperrt** — push.txt `F669 … SCHUB-GESPERRT spieler=(-3102,3051) rot=5072 lage=(-3125,2075) ziel=(-3125,2001) band=0` /
  `F1613 … ziel=(-3125,2002)`, beide `TREFFER-ZELLE r0[9]: x -2400..3150 z 2150..5350 typ 1 u0 0xff`; 0 SCHUB-OK; der Tritt
  bleibt (-3125,0,2075); danach Ost-Schub von Westen (x -4712,-7493 — Planziel, der Tritt stand nicht mehr dort) ohne
  Ereignis, Aufstieg (`AKTION` bei (542,-6234)) ohne Tritt wirkungslos, keine `DOOR FIRE`. Rechnung: Tritt-Box c(50,-900,0)
  h(675,900,600) -> Mitte x -3075, Ostkante -2400 = Westkante von r0[9]; FUN_8003b558 Rechtecktest @0x8003b738-768
  `(u32)(cx - (x0 - r)) < w + 2r` mit r = hx = 675: -3075 - (-2400 - 675) = **0 < 6900 -> Treffer** (halboffen, die exakte
  Kantenberuehrung zaehlt), z @0x8003b770-794: 2001 - (2150 - 675) = 526 < 4550 -> Treffer. Der Nicht-Quadrat-Zweig des
  Originals (`dis 0x8003b684 …`: `lhu v1,6(a3)` hx / `lhu v0,10(a3)` hz / `beq v1,v0 -> 0x8003b738`; sonst @0x8003b698-734:
  Vorzeichen-Code der Objektmitte gegen x0/x1 (`and … s4`, `srl 30`), **nur wenn die Mitte INNERHALB der x-Spanne der Zelle
  liegt und ausserhalb der z-Spanne, wird r = hz (`lhu t0,10(a3)` @0x8003b734)**, sonst r = hx) aendert hier nichts: die
  Tritt-Mitte (-3075) liegt WESTLICH von x0 (-2400) -> Code 3 != 2 -> r = hx. Nach dem Disassemblat muesste also auch das
  Original den Tritt an dieser Stelle sperren — ob die Beta den Tritt ueberhaupt in die Kerbe bringt (Drehung der Box bei
  rot_y 1024? anderer Weg?), ist damit OFFEN und wird am Original gemessen (s. O3). Der Port hat den Nicht-Quadrat-Zweig
  nicht (`re15_collision_box_blocked` nimmt immer `box_hx`, aot_common.c @0x8002bfb0-Haken) — fuer diesen Fall ohne
  Folge, fuer Objekte mit hx != hz und Mitte innerhalb der x-Spanne einer Zelle aber eine zweite Luecke (Spur A, nicht R).
**Folge:** Das Glied "2060 Tritt -> slot 1 -> 2040" ist an der exe NICHT gangbar (Tritt unbeweglich aus der Startlage); die
Dossier-Aussage "ROOM2040 bleibt ohne die Kiste erreichbar ueber 2020 -> 2030 -> 2060 (Tritt) -> Halle" ist damit fuer den
Port nicht gedeckt. Ob sie fuer das Original gilt, entscheidet O3.

### O2 — ROOM3050 obj 1 am Original (N5, vom Bau-Agenten offen gelassen; hier selbst gemessen, Lauf `orig_3050b_abn2`)
Spielstand selbst erzeugt (`re15_quickload.py`-Kopie mit 0,04-s-Tippern, `--triangle 2 --right 5 --postload 20` = JUMP 305,
Pad-Bindung SDL-0..2, 1,48 MB). Der Debug-Sprung setzt Leon auf **Band 0 bei (-23996,0,1523)** (nicht an die 30E0-Ankunft);
ein erster Lauf (`orig_3050_abn2`) kam von dort nicht zur Band-4-Kiste (Bandpruefung @0x8002bf38, Waende) und wurde
abgebrochen. Lauf b setzt Leon per GDB-RAM auf die 30E0-Ankunft (-3596,-7200,1523) Band 4 (+0x34/+0x38/+0x3C, +0x82) —
das Gegenstueck zu `RE15_PLAYER_POS=-3596,1523,1024,4` der exe-Messung `nb1_3050fall_d`/Riegel `fall3050`. Pool: `Pool 1:
(-18000,-7200,-20800) band=4` = obj 1 @0x01F1A; keine Gegner (kein Sce_em_set im main00). Ergebnis (Auslieferungs-EXE
0800e003): Weg (-6449,1526) -> (-6440,-20728) -> (-11880,-20498) -> (-16650,-20315) (= Port-Weg x -6482 nach Norden, z
-20300 nach Westen); **WEST: 307 SCHUB-OK, 0 gesperrt**, Kiste x -18000 -> -24070 (Port: 425 Schuebe bis -24058, dann 1x
Westwand r3[31] gesperrt — der Original-Lauf endete vorher ueber die Bedingung x <= -24050); Umweg auf die Nordseite
(-22720,-22540) -> (-24058,-22150); **SUED: 117 SCHUB-OK, 0 gesperrt, `FALL-AUSLOESER obj=(-24070,-7200,-18492)
spieler=(-23927,-7200,-19842)`** (Halt @0x8002c13c; Port: AUSLOESER pos=(-24058,-7200,-18488)); danach `faellt=1`, Band 4 ->
0, Endlage **`obj=(-24070,0,-15631) faellt=0 band=0`** (Port: `p1 (-24058,0,-15561) b0`). `Ereignisse gesamt: 433,
Fall-Ausloeser: 1, HP: 100`. **ROOM3050: Port = Original — derselbe Schubpfad, dieselbe Absprungzelle r*[46] (+450), Landung
auf Band 0; der vorgesehene Fall (Super Redhawk slot 6 @0x01EC6 floor 1 nur auf der gefallenen Kiste) laeuft in beiden.**
(Werkzeug-Hinweis: zwei Instanzen meiner Kette 3 liefen versehentlich gleichzeitig an EINEM DuckStation/GDB — die
Logzeilen sind doppelt, die Ereignisse identisch (433/434); die settings.ini wurde deshalb am Ende aus der sauberen
Sicherung des 2050-Laufs zurueckgeschrieben, s. Schluss.)

### A7 — Zweite Port-Abweichung: Objekte werden im Port NICHT je Bild aus Zellkanten geschoben (Original FUN_8003da78, +18)
* **Messung am Original:** im selbst erzeugten ROOM2060-Spielstand (`quickload --triangle 1 --right 6` = JUMP 206, 20 s
  Ruhe, keine Eingabe) steht der Tritt obj 2 bei **`Pool 2: w0 00000001 pos (-3143,0,2075) band=0 box [50,-900,0,675,900,600]`**
  — 18 Einheiten westlich des Records (-3125,0,2075) @0x00EA6; die Box ist unrotiert (hx 675, hz 600). Im Port steht er
  dauerhaft auf dem Record (objfall.txt `p2 (-3125,0,2075)`, Laeufe 2060echt/2060echt2) und beruehrt r0[9] exakt an der
  Kante (A6).
* **Mechanismus (selbst disassembliert):** FUN_8002bd44 laeuft je Bild ueber alle aktiven Typ-4-Objekte; jeder Nicht-Schub-
  Zweig (`bne … 0x8002bf74` @0x8002bf1c/bf30/bf44/bf58, dann `8002bf78-84 obj w0 & 0x80 -> 0x8002c0b8`, `8002bf8c-94
  obj+0x8C != 9 -> 0x8002c0b8`) fuehrt nach 0x8002c0b8: `ori a1,2` / s4,s5 -> obj.x/obj.z (= aktuelle Lage, @0x8002c0bc-d4) /
  **`8002c0d0 jal 0x8003b558`** (Zellentest Klasse 2 auf die AKTUELLE Lage) — bei Treffer `8003b79c jal 0x8003da78`:
  `8003da98-dab8` t0 = (x0 + w + r) - cx + 18 bzw. a0 = (x0 - r) - cx - 18 (kleinerer Betrag, @0x8003dac0-cc), dito z
  @0x8003dad0-db10, Richtungswahl ueber obj+0x40/+0x44 @0x8003db14-40 -> das Objekt wird um Eindringtiefe + 18 aus der
  Zelle geschoben (die "+18"-Ausschiebung aus R_dachkiste.md 2.1). Die Ghidra-Xrefs (ghidra1_V2.txt) zeigen genau zwei
  Aufrufer von FUN_8003b558 (@0x8002bfb0 Schub, @0x8002c0d0 Ruhe-Zweig) und einen von FUN_8003da78 (@0x8003b79c). Beim
  Raumstart liegt der 2060-Tritt mit 0 Einheiten Abstand an r0[9] (halboffener Test = Treffer), also schiebt ihn das
  Original im ersten Bild um 18 nach Westen (-3143) — danach ist der Nord-Schub frei (x-Test: -3093 - (-3075) = -18 -> kein
  Treffer).
* **Port (`aot_common.c:1717 re15_prop_push_tick`):** `if (s_prop_push_count[p] != 9) continue; /* @0x8002bf8c */` — in
  Ruhebildern wird weder FUN_8003b558 noch die Ausschiebung gerechnet; der gesperrte Schub schreibt "nichts"
  (Kommentar "Ruecksetzung = nichts schreiben") statt wie das Original nach der Ruecksetzung erneut zu testen und
  auszuschieben. Folge: ein Objekt, das den Zellkantentest trifft, bleibt im Port fuer immer an der Kante haengen — der
  2060-Tritt ist im Port unbeweglich, im Original (s. O3) nicht. Kein Code dieser Spur (Schubkette, Runde 35/A), aber die
  Ursache dafuer, dass der "Ersatzweg" ueber 2060 im Port nicht geht.

### O3 — ROOM2060 Tritt obj 2 am Original (Lauf `orig_2060b_abn2`, Spielstand `room2060_orig.sav` JUMP 206, Auslieferungs-EXE)
Leon an der 2030-Ankunft (7600,0,3950) Band 0 (= JUMP-206-Spawn = Tuer slot 0 Ankunft), Spinnen ueber Aktiv-Bit +
g_active_count abgeschaltet, Weg wie `2060echt2` (Ostseite nach Norden, Nordgasse nach Westen, Westgasse nach Sueden
westlich am Tritt vorbei, vor die Suedseite (-3055,3338)), Blick 1024 (Nord), UP: **`SCHUB-OK obj=(-3143,0,2000,…)
spieler=(-3093,0,3050,1056,0,1) s4/s5=(-3143,2075)`** (Halt @0x8002bfc0) … **`NORD: Schub angenommen 19 x, gesperrt 0 x`**,
Tritt-Mitte z 2075 -> 1852 (Bedingung z <= 1900). **Das Original schiebt den 2060-Tritt aus der Startlage nach Norden; der
Port sperrt denselben Schub (A6: `ziel=(-3125,2001)` Treffer r0[9]).** Ursache = A7: das Original hat den Tritt beim
Raumstart auf x -3143 ausgeschoben (18 Einheiten Abstand zu r0[9]), der Port laesst ihn bei -3125 an der Kante.
Damit ist belegt: der "Ersatzweg" 2050 -> 2020 -> 2030 -> 2060 (Tritt) -> 2040 ist im ORIGINAL an diesem Glied gangbar
(der Rest des Glieds — Schub nach Osten in die Kerbe, Latch (3,172), Aufstieg, Tuer slot 1 — wurde am Original nicht weiter
gefahren), im PORT dagegen nicht. **Port != Original.**

## Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 -- an der exe messen, ob Leon/Elza die Dach-Kiste bis zum Fall-Ausloeser schieben koennen (echter Weg, jede Richtung) | **erfuellt** | Riegel `leon`/`elza` (echter Weg 10F0/10F1 -> Tritt -> Tuer -> 1090/1091) + `west` in meinem Lauf `lauf_1dc98da6`: je 3x/2x SCHUB-GESPERRT an r3[4]/r3[2], 0 SCHUB-OK, 0 AUSLOESER, Kiste auf dem Record; eigene Treiber-Laeufe `1090nord`/`1091nord` (A2) bildgleich; dazu die 9 Laeufe der Abnahme 0 |
| 2 -- am Original messen: bis zur Absprungzelle oder vorher geblockt? | **erfuellt** | unveraendert: orig_lauf5/6 (Bau-Agent) + eigene Wiederholung NORD in Abnahme 0 -- erster Schubschritt trifft 0x80123984 = r3[4], Ruecksetzung, 0 Fall-Ausloeser |
| 3 -- Port blockt an derselben Stelle mit demselben Mechanismus (Beleg @0x, Pin) | **erfuellt** | Kette @0x8002bf14-c0e8 / FUN_8003b558 @0x8003b634-794 (Abnahme 0/1 disassembliert, hier @0x8002bfb0-c0d4 erneut gelesen), gleiche Zelle, gleiche Ziel-Lage (-15556/-15557 vs. -15558), gleiche Ruecksetzung; Pins `unit_r36_dachkiste_zensus` + `integration_r36_dachkiste` gruen und diskriminierend |
| 4 -- dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste? | **teilweise** | **3050/3051:** exe (Riegel `fall3050`), Original (O2, hier selbst gemessen: Port = Original, Landung Band 0), Pin, Datenlage (Super Redhawk nur auf der gefallenen Kiste) -> erfuellt. **2050/2051:** Schub und Fall an der exe (Riegel `fall2050`, eigene Laeufe ueber die echte Tuer `echt2050b`, Elza `elza2051b`) und am Original (orig_2050_d + eigene Wiederholung O1) gemessen, byte-true belassen, Pin, Grenze benannt -> dieser Teil erfuellt. ABER: die Antwort "Fortschritt haengt an der Kiste (Tuer slot 2), ROOM2040 bleibt ueber 2020 -> 2030 -> 2060 (Tritt) -> Halle erreichbar" ist im Port NICHT gedeckt: das 2060-Glied ist an der exe nicht gangbar (A6: Tritt obj 2 unbeweglich, jeder Schub gesperrt), waehrend das Original den Tritt schiebt (O3) -- Ursache ist die fehlende Ruhe-Bild-Ausschiebung FUN_8003da78 (A7). Damit erzeugt der Port nach dem (byte-true) Fall der 2050-Kiste einen Zustand, in dem ROOM2040 nur noch ueber den zirkulaeren 11A0-Weg erreichbar waere -- ein vom Port erzeugter Softlock, den der Nutzer-Wortlaut ausschliesst. Zudem war der echte Weg zur 2050-Kiste an der exe vom Bau-Agenten nicht gefahren; auf ihm klemmt der Port an einer Band-0-Kiste (A1; Original laeuft durch, O1) |

## Maengel (nummeriert, nachpruefbar)

1. **P1 -- Ersatzweg-Glied ROOM2060 im Port nicht gangbar, Original schiebt den Tritt (A6/A7/O3).** Port: `2060echt2` push.txt
   `F669 room=2060 p=2 obj=2 SCHUB-GESPERRT ... lage=(-3125,2075) ziel=(-3125,2001)` + `TREFFER-ZELLE r0[9]: x -2400..3150`,
   0 SCHUB-OK, Tritt bleibt auf dem Record (-3125). Original (`orig_2060b_abn2`): Tritt beim Raumstart auf (-3143,0,2075)
   ausgeschoben, Nord-Schub `SCHUB-OK ... 19 x, gesperrt 0 x`, Tritt z 2075 -> 1852. Mechanismus: FUN_8002bd44 Ruhe-Zweig
   @0x8002bf78-94 -> 0x8002c0b8 (`jal 0x8003b558` @0x8002c0d0 auf die aktuelle Lage, Treffer -> `jal 0x8003da78` @0x8003b79c,
   Ausschiebung um Eindringtiefe + 18 @0x8003dab0/@0x8003daf0) fehlt im Port (`aot_common.c:1717 re15_prop_push_tick`:
   `if (s_prop_push_count[p] != 9) continue;`, gesperrter Schub = "nichts schreiben"). Folge: nach dem Fall der 2050-Kiste
   (im Port wie im Original moeglich) ist ROOM2040 im Port nicht mehr erreichbar (2070-Nordgang nur zirkulaer, N1) -- ein
   vom Port erzeugter Softlock. Nachbesserung: Ruhe-Zweig byte-true nachbilden (FUN_8003b558 je Bild + FUN_8003da78) ODER
   die Dossier-Aussage "ROOM2040 bleibt erreichbar" streichen und die Grenze als Port-Softlock benennen; Pin: ROOM2060
   Raumstart -> obj 2 bei x -3143 (18 aus r0[9]), Nord-Schub von (-3055,3338) SCHUB-OK; Riegel-Glied bis `DOOR FIRE slot=1`
   -> ROOM2040. Zustaendigkeit (Schubkette, Spur A/Integration) im Dossier benennen, wenn nicht in R behoben.
2. **P2 -- Band-blinde Ausschiebung Spieler <-> Objekt (A1/O1).** Port `re15_collision.c:946 push_out_of` (und
   `re15_collision_objects_actor`) ohne Y-Test; Original FUN_8002cabc @0x8002cb84-cbc8 vergleicht `objCY - actorCY` gegen
   die hy-Summe (`sltu`/`xori`, `j 0x8002cde4` mit v0 = 0 bei Nicht-Ueberlappung). Messung: Leon/Elza (Band 3, y -5400)
   klemmen bei x 3402 = 2952 + 450 an der Band-0-Kiste obj 1 (2052,0,-5552) (`echt2050`, `elza2051`), das Original laeuft
   bei (9002,-6417) -> (-125,-6291) hindurch (O1; Zahlen: dy 6030 > 2 x 2430). Nachbesserung: Y-Ueberlappung byte-true in
   push_out_of (a2 != 2) nachbilden; Pin: echter Weg 2000 slot 4 -> 2050, GEHE (9100,-6300) -> (-100,-6300) ohne Klemme.
   (Nicht Code dieser Spur; aber die 2050-Messung "echter Weg" des Dossiers haengt daran.)
3. **P3 -- Dossier-Luecken.** (a) Die exe-Messung der 2050-Kiste (nb2_2050fall_c, Riegel `fall2050`) startet per
   RE15_PLAYER_POS hinter der Kiste; der echte Weg von der Tuer wurde nicht gefahren (auf ihm P2). (b) 4.3 letzter Absatz
   ("In der Sonde bleibt 2050/2051 deshalb DIAGNOSE (soll = -1 ...)") widerspricht dem Stand der Nachbesserung 2 (soll = 1,
   Tests-Abschnitt). (c) N5 (3050 am Original) steht weiter unter OFFEN -- hier gemessen (O2, Port = Original), ins
   Dossier uebernehmen (Messweg-Korrektur: JUMP 305 spawnt auf Band 0 bei (-23996,0,1523); Spieler per GDB-RAM auf die
   30E0-Ankunft Band 4 setzen). (d) "Fuer den Nutzer": die Aussage zum Ersatzweg ueber 2060 ist nach P1 nicht haltbar.
4. **P4 (klein) -- Nicht-Quadrat-Zweig von FUN_8003b558 fehlt im Port.** @0x8003b684-690 `beq hx,hz -> 0x8003b738`, sonst
   @0x8003b698-734: liegt die Objektmitte innerhalb der x-Spanne der Zelle und ausserhalb der z-Spanne, ist r = hz
   (`lhu t0,10(a3)` @0x8003b734); `re15_collision_box_blocked` nimmt immer `box_hx`. Fuer die hier gemessenen Faelle ohne
   Folge (2060: Mitte westlich von x0 -> r = hx in beiden), fuer Tritte mit hx != hz an Zellkanten aber eine weitere
   Abweichung -- im Dossier/OFFEN nennen (Spur A).

Keine Maengel an der Code-Form (reiner Messschrieb/Treiber, kein Verhalten, kein Guess-Tell), keine Vertragsverletzung,
keine Regression in den beruehrten Modulen (Unit-Gruppe 31/31, Riegel 1/1 mit acht Laeufen). N1, N2 (Mechanismus belegt
und an der exe gefahren), N3 (exe + Original, reproduziert) und N4 sind behoben; N5 wurde nicht vom Bau-Agenten, sondern
hier gemessen (Ergebnis: Port = Original).

## Gesamt

**Softlock-Frage ROOM1090/1091 (Kern des Auftrags): NEIN -- Port = Original, in dieser Abnahme erneut bestaetigt
(Riegel + eigene Treiber-Laeufe, beide Szenarien).** Punkte 1-3 erfuellt. ROOM3050: Port = Original (hier am Original
gemessen). ROOM2050/2051: Schub und Fall Port = Original (exe ueber die echte Tuer, Original zweimal) -- aber die Folge
des Falls ist im Port anders als im Original: der einzige Ersatzweg nach ROOM2040 (ueber den 2060-Tritt) funktioniert im
Original (O3) und NICHT im Port (A6/A7), und der echte Weg zur 2050-Kiste klemmt im Port an einer Band-0-Kiste (A1/O1).
Beide Ursachen liegen ausserhalb des Spur-Codes (Schubkette aot_common.c, Kollision re15_collision.c), aber die
Dossier-Antwort "kein Port-Softlock, ROOM2040 bleibt erreichbar" ist fuer den gebauten Stand falsch -> **bestanden =
false** (Punkt 4 teilweise; Maengel P1-P4).

Messpfade: Riegel `re15_port/build/r36_dachkiste_wd/lauf_1dc98da6/{leon,elza,west,fall3050,weg2040,treppe2040,fall2050,
weg2020}/`; eigene exe-Laeufe Scratchpad `abn2/{echt2050,elza2051,1090nord,1091nord,2060echt,2010hinweis,echt2050b,
elza2051b,2060echt2}/` (debug.log, climb.txt, objfall.txt, push.txt, wegplan.txt); Original Scratchpad `abn2/
{orig_2050_abn2,orig_3050_abn2,orig_3050b_abn2,orig_2060_abn2,orig_2060b_abn2}/log.txt` mit den Spielstaenden
`abn2/room3050_orig.sav`, `abn2/room2060_orig.sav` und den Skripten `orig_2050_abn2.py`, `orig_3050b_push.py`,
`orig_2060b_push.py`, `quickload_3050.py`, `quickload_2060.py` (unversioniert; Inhalte oben zitiert). DuckStation
`settings.ini` am Ende aus der sauberen Sicherung zurueckgeschrieben (EnableGDBServer = false, ConfirmPowerOff = true,
keine SDL-1/SDL-2-Bindungen; geprueft).
