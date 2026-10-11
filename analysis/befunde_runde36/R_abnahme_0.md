# Runde 36 — Spur R "dachkiste" — Abnahme 0 (unabhaengig, 2026-10-11)

Baum `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste` @ f487909d (Basis 99398dc7).
Gebaut mit `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)` (ninja: no work to do — die exe
16:25:06 ist juenger als `dachkiste_r36.c` 16:24:54 und `aot_common.c` 13:34; sie enthaelt die Zeichenketten
`TREFFER-ZELLE`/`SCHUB-%s` des finalen Hakens). exe sha256 `13ffe6a5cce82c1d…`. Messkopie `re15_pc_r36abn.exe`
neben der exe, eigene Arbeitsordner im Scratchpad (`abn/<lauf>/`). Der Bericht wird fortlaufend geschrieben.

Wortlaut des Auftrags (Orchestrator, aus H_ziel1190.md 2.2/2.4/OFFEN + Spur-A-Vorgabe "Das macht ein Fortschritt im
Spiel unmoeglich"): (1) an der exe messen, ob Leon/Elza die Dach-Kiste obj 1 in ROOM1090/1091 bis zum Fall-Ausloeser
schieben koennen (echter Weg aus 10F0, jede Richtung); (2) am Original messen, ob es die Kiste bis zur Absprungzelle
laesst oder vorher blockt; (3) blockt das Original -> Port blockt an derselben Stelle mit demselben Mechanismus
(Beleg, Pin), sonst byte-true + Grenze benennen + Pin; (4) dieselbe Pruefung fuer ROOM3050/3051 obj 1 und
ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste?

## Code-Diff (git diff 99398dc7 -- re15_port, wirklich gelesen)

* `engine/src/aot_common.c` re15_prop_push_tick: 3 Zeilen — das Ergebnis von re15_collision_box_blocked (=
  FUN_8003b558(obj,2) @0x8002bfb0) wird in `gesperrt` gehalten und an `re15_dachkiste_schub_log(p,nx,nz,gesperrt)`
  gegeben; danach unveraendert `if (gesperrt) { count = 10; continue; }`. KEIN Verhalten geaendert (gleicher Ausdruck,
  gleiche Verzweigung). Extern-Deklaration im Block statt Header-Include (Stil, kein Defekt).
* NEU `engine/src/dachkiste_r36.c` + `include/re15_dachkiste_r36.h`: reiner Messschrieb `RE15_PUSH_LOG=<datei>`;
  ohne Env-Variable fruehes `return` (einmaliges getenv). Die Zellensuche (Quadrant FUN_8003b068, Band
  @0x8003b634-644, u0 & Maske @0x8003b64c-660, halboffenes Rechteck r = hx @0x8003b738-794) ist eine Kopie der
  Suche aus re15_collision_box_blocked mit Ausgabe — sie SCHREIBT nichts in den Spielzustand.
* NEU `tests/unit/probe_r36_dachkiste.c` (Zensus MIT Spieler, Modell), `tests/unit/probes/r36_dachkiste.cmake`
  (registriert `unit_r36_dachkiste_zensus` + `integration_r36_dachkiste`), NEU
  `tests/integration/test_r36_dachkiste.cmake` (echte exe, drei Laeufe leon/elza/west).
* Guess-Tells im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO): KEINE Treffer in re15_port/.
  Verhaltens-Konstanten: keine neuen (der Messschrieb rechnet mit den Werten der Kette; die Zensus-Sonde traegt je
  Konstante die Adresse, RAST 450 ist als "Feinheit, kein Verhaltenswert" gekennzeichnet).

## Gates

### Suite (gezielt, kein voller Lauf im Spurbaum)
`ctest --test-dir re15_port/build -R "unit_r36_dachkiste_zensus|unit_r36_objfall|unit_r36_klettern|unit_1090_|prop_sca_collision|prop_point_in_poly|prop_zone_dispatch|unit_r22_10f0_figur|unit_re2_room1090_ab|unit_plc_back_yaw_1090|probe_1090_|unit_map_1090|unit_r35_cut10f0|unit_r36_szenenleon_abgang10f0" --timeout 900`:
```
100% tests passed, 0 tests failed out of 31   (Total Test time 17.64 s; unit_r36_dachkiste_zensus 8.25 s)
```
`ctest -R integration_r36_dachkiste --timeout 1200 --output-on-failure`: (laeuft, Ergebnis unten).

### RE-Gate (selbst disassembliert, `re15_disasm.py dis`, info/Re1.5/PSX.EXE)
* Schubkette FUN_8002bd44: @0x8002bf14 `lbu v1,8(s2)`/`bne v1,v0` (Typ 4), @0x8002bf24-30 `lw v0,-32(s6)`/`andi 1`/
  `beq` (UP), @0x8002bf38-44 `lbu v1,846(s6)`/`lbu v0,130(s2)`/`bne` (Band == obj+0x82), @0x8002bf50 `jal 0x8002d1e8`
  a2=0 (Reichweite), @0x8002bf60-74 Zaehler++ / `sb zero,140(s2)`, @0x8002bf8c-94 `ori v0,zero,0x9`/`bne` (nur bei
  Zaehler == 9), @0x8002bfa4 `jal 0x8002cabc` `ori a2,zero,0x1`, @0x8002bfb0 `jal 0x8003b558` `ori a1,zero,0x2`,
  @0x8002bfb8 `bne v0,zero,0x8002c0b0`, @0x8002bfc0 `sb v0(=8),140(s2)`, @0x8002bfe0-e4 Substate 8: stimmt.
  @0x8002c0b0 `sb s7,140(s2)` mit s7 = 0xa (`ori s7,zero,0xa` @0x8002bd84), @0x8002c0bc-d4 Ruecksetzung s4/s5 ->
  obj+0x34/+0x3C (+ erneut `jal 0x8003b558` a1=2 @0x8002c0d0), @0x8002c0d8-e8 `jal 0x8002cabc` a2=0 (Spieler aus der
  Kiste): stimmt.
* FUN_8003b558: @0x8003b634-644 `lhu v0,8(s0)` (Zelle+10) / `sll 16` / `sra 28` = Bandnibble, `bne v1(obj+0x82)`;
  @0x8003b64c-660 `lhu v0,6(s0)` (Zelle+8) / `sll 16` / `sra 24` = u0, `and v0,s6,v0` (s6 = a1 & 0xff = 2), `beq`;
  @0x8003b684-690 `lhu v1,6(a3)`/`lhu v0,10(a3)`/`lhu t0,6(a3)`/`beq v1,v0` (hx == hz -> r = hx);
  @0x8003b738-768 `(u32)(cx - (x0 - r)) < w + 2r` (`sltu v1,v1,v0` @0x8003b764), @0x8003b770-794 dito z,
  @0x8003b79c `jal 0x8003da78` + `ori s5,s5,0x1`: stimmt. FUN_8003da78 @0x8003dab0 `addiu t0,v0,18`: stimmt.
* FUN_8002d1e8: @0x8002d204 `ori v0,zero,0x233` (563 voraus), @0x8002d224 `ori a3,zero,0x384` (900; 700 nur bei
  a2 != 0 @0x8002d280), Rueckgabe @0x8002d284-a0 `(u32)(d + 900) <= 1800` je Achse: stimmt (|d| <= 900).
* Objekt-Pass @0x8002c0fc-14c: Typ 4, `lbu v0,4(s2)` (faellt schon), `addiu a0,s2,52`/`lbu a2,130(s2)`/
  `jal 0x8003b7f0`/`ori a1,zero,0x1c2`, `andi v0,v0,0x2`, `jal 0x8002d3d8`, `sb 1,4(s2)`/`sb zero,5(s2)`: stimmt.
* FUN_8002cabc: @0x8002cce8-d10 |d| je Achse, `slt`/`beq` (groessere Achse), @0x8002cd30-44 / @0x8002cd98-ac
  obj = Spieler +- t5/t4 (Summe der Halbbreiten, `addu t5,a0,v0` @0x8002cb48 = 450 + 900 = 1350): stimmt.
Alle vom Dossier zitierten Adressen enthalten das Behauptete.

### Vertrag
* Bank-9-Bits: keine; Nachrichten-IDs: keine; Ereignisse: keine. Gelesen/gesetzt wird nur (9,71) im Riegel
  (RE15_SET_FLAG, vorhandenes Flag, Messhaken) — erlaubt.
* Pfade (git diff 99398dc7 --name-only): nur analysis/befunde_runde36/R_*, engine/src/aot_common.c (Haken 3 Zeilen),
  neue dachkiste_r36.c/.h, tests/unit/probe_r36_dachkiste.c, tests/unit/probes/r36_dachkiste.cmake,
  tests/integration/test_r36_dachkiste.cmake. Keine release/, platform/android/, shared_assets/PSX/, keine
  Test-CMakeLists: eingehalten.
* Riegel in der eigenen probes-Datei registriert; Messkopie mit eigenem Namen (`re15_pc_r36r_haken_<id>.exe`),
  kein taskkill; RE15_WINDOW_SCALE=1: eingehalten.

## Punkt 4 — RDT-Belege selbst nachgelesen (re15_port/tools/scd_dump_room.py, eigener Lauf)

* ROOM3050/3051: Door slot 0 @0x01E86 floor 0 -> ROOM3040; slot 1 @0x01EA6 floor 4 -> ROOM30E0; Item slot 6 @0x01EC6
  `50 06 09 31 01 00 20 9f 6c a3 08 07 20 03 07 00 06 00 5c 00 02 00` = floor 1, Item 0x07 x6 (Super Redhawk) —
  nur auf der Oberkante der GEFALLENEN Kiste (Band 0 + 1). KEIN AOT auf floor 5 (Oberkante der stehenden Kiste): stimmt
  mit dem Dossier (4.2). Der Fall ist der vorgesehene Weg, nichts haengt an der oberen Lage.
* ROOM2050/2051: Door slot 2 @0x01C8A floor 4 (Kistenoberkante) -> ROOM2040, Ankunft Band 3, ohne Schluessel-Bytes.
  Tueren nach ROOM2040 (eigener Zensus): ROOM2060/2061 slot 1 @0x00DBA floor 1 -> 2040 (Ankunft Band 3),
  **ROOM2070/2071 main00 slot 5 @0x00B1C `3b 05 02 31 00 00 … 00 00 01 04 0c 05` floor 0 -> ROOM2040 cut 12, Ankunft
  Band 5 — UNBEDINGT (steht nach dem Ifel/Else-Block @0x00ABA-0B1A, vor dem Switch @0x00B3C); Gegenstueck ROOM2040
  slot 4 @0x0131C floor 5 -> ROOM2070 cut 9.** ROOM2070 ist aus ROOM2000/2001 slot 3 @0x0167E (floor 1) erreichbar.
  Folge: ROOM2040 ist ohne die Kiste in 2050 erreichbar — die Schlussfolgerung des Dossiers haelt, aber der im
  Dossier (4.3 "NACHGEPRUEFT") genannte Weg 2030 -> 2060 -> 2040 ist nicht belegt (2030 slot 3 @0x01B4C liegt auf
  floor 9, die Ankunft aus 2070 ist floor 3; 2060 slot 1 liegt auf floor 1, die Ankunft aus 2030 ist floor 0 — die
  Etagenwechsel IN den Raeumen zeigt das Dossier nicht), und die Aussage "ROOM2070/2071 hat KEINE Tuer nach 2040;
  der Vorgaenger-Eintrag '2070 slot 5' ist falsch" ist selbst falsch (s.o.). `R_belege/stage2_tueren_2040.txt`
  fehlen die Records 2070 @0x00AD6/@0x00AFA (slot 4 -> ROOM11A0?) und @0x00B1C (slot 5 -> 2040).
* ROOM1090/1091: Door slot 1 @0x0213A floor 6 -> ROOM1100 (cut 5, Spawn (-20900,0,-10336)) = einziger AOT auf
  floor 6; slot 0 @0x0211A floor 1 -> ROOM1050: stimmt mit H 2.2.

## Punkt 2 — Original (Pruefung der Belege des Bau-Agenten; eigene Wiederholung s. unten)

`R_belege/orig_lauf5/log.txt` und `orig_lauf6/log.txt` (DuckStation-GDB, Spielstand `stage_saves/room1090_orig.sav`):
* Vorbedingungen im Log: `EXE-Stub @0x80026e4c: 0800e003 (Auslieferung)` = unveraenderte EXE (CLAUDE.md-Pruefbyte);
  Spieler (-5282,-9000,-1514) Band 5; obj1 (-5960,-9000,-15482) Band 5 = Record @0x02190
  (`2d 01 04 00 05 00 00 01 00 00 b8 e8 d8 dc 86 c3 … 00 00 7c fc 00 00 84 03 84 03 84 03`, selbst gedumpt: Band 5,
  w0 0x0100, Box c(0,-900,0) h(900,900,900)); Haltepunkte `Z0` auf @0x8002bfc0 / @0x8002c0b0 / @0x8002c13c /
  @0x8003da78 = genau die Stellen, die ich oben disassembliert habe (angenommen / gesperrt / Fall-Ausloeser /
  Zellentreffer); Spieler-Box `+0x78 = (0,-1530,0,450,1530,450)` = `read 0x80073e94 6 --w 2 --signed` ->
  `[0, -1530, 0, 450, 1530, 450]` (selbst gelesen): stimmt.
* Ergebnis (lauf6 NORD): `TREFFER-ZELLE a0=80123984 … x -7256..4624 z -18560..-16403 typ 1 u0 0xff u1 0x00 fl 0x53`,
  `SCHUB-GESPERRT obj1=(-5960,-9000,-15485,…,9) spieler=(-5887,-9000,-14208,…) s4/s5=(-5960,-15482)`; die
  Zellbytes sind byteidentisch mit r3[4] @0x01FD4 meines eigenen SCA-Dumps (`sca_dump.py ROOM1090.RDT 5`), die
  -15485 = Zellkante -16403 + 900 + 18 (FUN_8003da78 @0x8003dab0, s.o.), s4/s5 = die gesicherte Lage, die die
  Ruecksetzung @0x8002c0bc-d4 zurueckschreibt. lauf5: NORD/NORD-2/WESTSEITE gesperrt an r3[4], WEST gesperrt
  (Ziel x -6035 -> Westwand r3[2]), OST/SUED ohne Versuch; `Ereignisse gesamt: 4`, kein SCHUB-OK, kein
  FALL-AUSLOESER, Ende obj1 (-5960,-9000,-15482) Band 5; `settings.ini zurueckgeschrieben (EnableGDBServer =
  false: True)` — im Profil geprueft: `EnableGDBServer = false`, `ConfirmPowerOff = true` (Zeile 2/507).
* Warum das mechanisch zwingend ist (aus den disassemblierten Stellen, nicht aus dem Ergebnis): die Reichweite
  FUN_8002d1e8 verlangt den Punkt 563 VORAUS innerhalb 900 der Kistenmitte; bei einem Schub nach Norden (Kistenmitte
  z -15482, Kontaktlinie des Spielers z -14132) heisst das Spieler-z <= -14582 + 563*cos(phi) nach dem FSM-Schritt
  s = ~74*cos(phi), also 637*cos(phi) >= 450 -> cos(phi) >= 0.706 -> s >= 52 > 21 Einheiten Luft zur Bruestung
  (Mitte >= -15503, `sltu` @0x8003b764 halboffen). Der Zaehler obj+0x8C erreicht die 9 erst ~7 Bilder NACH dem
  Kontakt (Reichweite beginnt 113 Einheiten = 1,5 Bilder davor), der Spieler steht dann jedes Bild um den vollen
  FSM-Schritt in der Kiste (Ausschiebung @0x8002c0d8-e8 erst danach) — jeder Schubschritt nach Norden ist >= 52,
  nach Westen (18 Luft, dieselbe Rechnung) ebenso. Nach Sueden braeuchte es den Spieler bei z <= -16832 (Bruestung
  laesst z >= -15953 zu), nach Osten bei x <= -7310 (Westwand laesst x >= -6428 zu): keine Spielerlage.

## Punkt 4 — Urteil

* ROOM3050/3051 obj 1: erfuellt — Fall = vorgesehener Weg (Latch (3,93), Record der gefallenen Lage, Super Redhawk
  Item slot 6 floor 1 NUR auf der gefallenen Kiste; Tueren floor 0/4 brauchen die Kiste nicht; kein AOT auf floor 5).
  Von mir am RDT nachgelesen (oben).
* ROOM2050/2051 obj 6: Schlussfolgerung richtig (ROOM2040 haengt nicht an der Kiste), Beleg im Dossier teilweise
  falsch: s. Maengel M1/M2. Der Fall der Kiste obj 6 im Original (Absprung-Kaskade) wurde weder an der exe noch am
  Original gemessen (Zensus = Modell), unter OFFEN als "nicht Auftrag" abgelegt.

## Punkt 1/3 — eigene Messung an der exe (Messkopie `re15_pc_r36abn.exe`, Scratchpad `abn/<lauf>/`)

Grundumgebung je Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt
RE15_PUSH_LOG=push.txt RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1` (+ Lauf-Variablen). Alle Laeufe unter
Last (7 fremde `re15_pc_abn6f.exe` einer parallelen Abnahme) — die Zeitachse ist bildbasiert (EXIT_AT je Bild), die
Ergebnisse sind davon unabhaengig.

### Riegel `integration_r36_dachkiste` (eigener Lauf, `re15_port/build/r36_dachkiste_wd/lauf_bd57a280/`)
```
1/1 Test #572: integration_r36_dachkiste ..........   Passed  301.13 sec
100% tests passed, 0 tests failed out of 1
```
leon: `[setflag] flag(9,71)=1`, `loaded room10f0.rdt`, `[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500)
target_cut=0 spawn=(-13600,-9000,1300)`, `loaded room1090.rdt`; push.txt `F1611/F1651/F1711 room=1090 p=1 obj=1
SCHUB-GESPERRT spieler=(-6410|-6417,-14206) rot=5184 lage=(-5960,-15482) ziel=(-5960,-15556) band=5` + 3x
`TREFFER-ZELLE r3[4]: x -7256..4624 z -18560..-16403 typ 1 u0 0xff u1 0x00 fl 0x53`; objfall.txt 0x AUSLOESER,
letzte Lage `F1995 … p1 (-5960,-9000,-15482) b5`. elza: `Spieler-Familie PL04`, `loaded room10f1.rdt`, derselbe
DOOR FIRE, `loaded room1091.rdt`, push.txt F1611/F1651/F1711 mit identischen Zahlen, 3x r3[4], 0 Ausloeser, Record.
west: `F69/F144 room=1090 p=1 obj=1 SCHUB-GESPERRT spieler=(-4684,-15092|-14672) rot=2112 … ziel=(-6034,-15482)`
+ 2x `TREFFER-ZELLE r3[2]: x -9200..-6878 z -18524..265`, 0 Ausloeser, Record.

### Eigener Lauf `leon_echt` (Leon, ECHTER WEG: `RE15_SET_FLAG=9:71 RE15_DEBUG_JUMP=10F0@250 RE15_PLAYER_POS=-7125,4950,0
RE15_INPUT_SCRIPT=<Weg des Riegels> RE15_EXIT_AT=2000#1090`)
* debug.log: `[pl] Spieler-Familie PL00`, `[setflag] flag(9,71/0x47) = 1`, `loaded room10f0.rdt`, kein
  `Szene gestartet`, `[aot] DOOR FIRE slot=1 … spawn=(-13600,-9000,1300)` (Z. 1291), `loaded room1090.rdt`,
  `EXIT_AT: Bild 2000 in Raum 1090`. objfall.txt `F759 room=10F0 p=3 KERBE m0b=10 pos=(-4785,0,216)` (Tritt in der
  Kerbe), climb.txt `T1121 … pad=8000 pos=(-4899,0,-58) sub=9 kind=0x81 obj=3` (Klettern auf den Tritt).
* push.txt (ROOM1090): `F1611 p=1 obj=1 SCHUB-GESPERRT spieler=(-6410,-14206) rot=5184 lage=(-5960,-15482)
  ziel=(-5960,-15556) band=5`, `F1651` und `F1711` dito mit spieler x -6417; je Versuch `TREFFER-ZELLE r3[4]: x
  -7256..4624 z -18560..-16403 typ 1 u0 0xff u1 0x00 fl 0x53`. Kein `SCHUB-OK` fuer obj 1 (die vierte GESPERRT-
  Zeile ist der Tritt p=3 in 10F0 an der Kerbe, F829). objfall.txt: 0x `p=1 AUSLOESER`, letzte Lage `F1995 room=1090
  … p1 (-5960,-9000,-15482) b5`. Bildgleich mit dem Riegel-Lauf und mit `R_belege/p1_echt`.
* Vergleich Original (lauf6 NORD): Sperre an derselben Zelle (0x80123984 = r3[4], gleiche Bytes), Ziel-Lage Spieler-z
  - 1350 in beiden (-15556 gegen -15558; Spieler -14206 gegen -14208 = Anlauf/Schrittphase, nicht die Kette),
  Ruecksetzung auf den Record in beiden. Mechanismus identisch: FUN_8003b558(obj,2) trifft die Bruestungszelle.

### Eigener Lauf `elza_echt` (Elza, ECHTER WEG: wie leon_echt mit `RE15_PSELECT_AUTO_SWITCH=1`, `RE15_EXIT_AT=2000#1091`)
* debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `loaded room10f1.rdt`, `[aot] DOOR FIRE slot=1
  rect=(-4750,-1750,hw=600,hh=500) target_cut=0 spawn=(-13600,-9000,1300)`, `loaded room1091.rdt`, `EXIT_AT: Bild
  2000 in Raum 1091`.
* push.txt: `F1611/F1651/F1711 room=1091 p=1 obj=1 SCHUB-GESPERRT spieler=(-6410|-6417,-14206) rot=5184
  lage=(-5960,-15482) ziel=(-5960,-15556) band=5`, 3x `TREFFER-ZELLE r3[4]: x -7256..4624 z -18560..-16403 typ 1
  u0 0xff u1 0x00 fl 0x53`; kein SCHUB-OK fuer obj 1; objfall.txt 0x AUSLOESER, letzte Lage `F1995 room=1091 … p1
  (-5960,-9000,-15482) b5`. Zahlengleich mit Leon (die SCA von ROOM1091 ist auf Band 5 byteidentisch mit ROOM1090,
  eigener Diff der sca_dump-Ausgaben).

### Eigene Sprung-Varianten (Leon, `RE15_DEBUG_JUMP=1090@250`, `RE15_PLAYER_POS=x,z,rot,5`, debug.log je Lauf
`[parity] JUMP-Spawn ersetzt durch RE15_PLAYER_POS (…) band=5`, `loaded room1090.rdt`, `EXIT_AT: Bild 450|500`)

| Lauf | Start (x,z,Blick) / Skript | Versuche (push.txt) | TREFFER-ZELLE | Ausloeser / Kiste am Ende |
|---|---|---|---|---|
| `nord` (Replik p1_nord / Original NORD) | (-5887,-12500,1024) `W2,U3,W0.5,U1.5,W0.5,U1.5,W1` | F89/F174/F234 GESPERRT spieler=(-5887,-14207) ziel=(-5960,-15557) | 3x r3[4] Bruestung | 0 / `F450 … p1 (-5960,-9000,-15482) b5` |
| `diag45` (45 Grad NW gegen die Suedflanke) | (-4700,-12800,1536) `W2,U4,W0.5,U1.5,W0.5,U1.5,W1` | KEIN Versuch (kein push.txt): Spieler endet an der SW-Ecke (-6410,-14132) rot 1536; Punkt 563 voraus = (-6808,-14530) -> dz 952 > 900, Reichweite FUN_8002d1e8 verfehlt, Zaehler bleibt 0 | — | 0 / `F495 … p1 (-5960,-9000,-15482) b5` |
| `rennen` (UP+CROSS = Laufen) | (-5887,-12500,1024) `W2,UX3,W0.5,UX1.5,W0.5,U1.5,W1` | F76/F174 GESPERRT spieler=(-5887,-14332) ziel=(-5960,-15682) (Laufschritt 200), F234 GESPERRT (-14207 -> -15557) | 3x r3[4] | 0 / Record |
| `nordtasche_west` (Nordtasche, Blick West) | (-3853,-15852,2048) `W2,U3,W0.5,U1.5,W0.5,U1.5,W1` | F77/F174/F234 GESPERRT spieler=(-4685,-15852) ziel=(-6035,-15482) | 3x r3[2] Westwand | 0 / Record |

Ergebnis an der exe (9 Laeufe: 3 Riegel + 6 eigene, Leon und Elza, echter Weg und Sprung, vier Himmelsrichtungen,
Gehen/Laufen/schraeg): **0 angenommene Schuebe an obj 1, 0 Fall-Ausloeser, Kiste in jedem Lauf am Ende exakt auf dem
Record (-5960,-9000,-15482) Band 5.** Die Sperre faellt Versuch fuer Versuch an derselben Zelle wie im Original
(r3[4] fuer Nord, r3[2] fuer West; `nordtasche_west` liefert mit (-4685 / -6035) exakt die Zahlen des Original-Plans
WEST). Der Port blockt also an derselben Stelle mit demselben Mechanismus (FUN_8003b558(obj,2) -> Zelle u0&2 im
Rechteck r = hx; Sperre -> Zaehler 10, keine Lagenaenderung) — Punkt 3 erfuellt, kein Verhaltens-Fix noetig.

## Punkt 2 — eigene Wiederholung am Original (DuckStation-GDB, Plan NORD)

Kopie von `R_belege/orig_1090_push.py` (unveraendert) im Scratchpad (`abn/r36R/orig/`), `python orig_1090_push.py
<ordner> nord` (ohne `-I`: das Skript ist Repo-Werkzeug, `vgamepad` liegt in den site-packages), Spielstand
`C:/workspace/git/reAi_v2/stage_saves/room1090_orig.sav` nur gelesen, MZD-Disc, kein anderer exe-/DuckStation-
Prozess waehrend des Laufs (tasklist geprueft: 0). `lauf_nord/log.txt`:
```
EXE-Stub @0x80026e4c: 0800e003 (Auslieferung 0800e003)
Spieler: (-5282, -9000, -1514, 672, 5, 0) obj0: (-9836, -1800, 4753, …) obj1: (-5960, -9000, -15482, 0, 0, 5, 0)
Z0 8002bfc0 / 8002c0b0 / 8002c13c / 8003da78: OK
Spieler-Box +0x78: (0, -1530, 0, 450, 1530, 450)
NORD VOR spieler=(-5761,-9000,-13365) blick=-15456 band=5 sub=3 | obj1=(-5960,-9000,-15482) faellt=0 z=0 band=5 n=0
  TREFFER-ZELLE a0=80123984 obj=800b402c: x -7256..4624 z -18560..-16403 typ 1 u0 0xff u1 0x00 fl 0x53 | obj1=(-5960, -9000, -15562, 0, 0, 5, 9)
  SCHUB-GESPERRT obj1=(-5960, -9000, -15485, 0, 0, 5, 9) spieler=(-5574, -9000, -14212, -15456, 5, 1) s4/s5=(-5960,-15482)
NORD: Schub angenommen 0 x, gesperrt 1 x, Fall-Ausloeser 0 x
ENDE … obj1=(-5960,-9000,-15482) faellt=0 z=0 band=5 n=0
Ereignisse gesamt: 1
settings.ini zurueckgeschrieben (EnableGDBServer = false: True)
```
* Der Versuch fiel bei Spieler-z -14212 (Blick 928 = 8 Grad oestlich von Nord, Regelung innerhalb der Toleranz):
  Ziel-z -15562 = -14212 - 1350 (FUN_8002cabc), Treffer an der Bruestungszelle 0x80123984 (Bytes = r3[4] @0x01FD4
  meines SCA-Dumps), Zaehler 9, Ruecksetzung auf s4/s5 = Record, obj+0x04 = 0 (kein Fall). Danach glitt der Spieler
  an der Suedflanke nach Osten in die Tasche (NORD-2 ohne Versuch, da ausserhalb der Reichweite). Gleicher Mechanismus,
  gleiche Zelle wie in den Laeufen des Bau-Agenten (lauf5/lauf6) und wie im Port (Ziel = Spieler-z - 1350, r3[4]).
* `settings.ini` nach dem Lauf: sha256 `e83ad78f2bd3351f` = Ausgangszustand vor dem Lauf (gleicher Hash),
  `EnableGDBServer = false`, `ConfirmPowerOff = true`; kein DuckStation-Prozess uebrig.

## Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 — an der exe messen, ob Leon/Elza die Dach-Kiste bis zum Fall-Ausloeser schieben koennen (echter Weg, jede Richtung) | **erfuellt** | Bau-Agent: p1_nord/west/westseite/sued/ost, e1_nord/west, p1_echt/e1_echt; von mir wiederholt: Riegel (leon/elza echter Weg + west) und 6 eigene Laeufe (leon_echt, elza_echt, nord, diag45, rennen, nordtasche_west) — 0 angenommene Schuebe, 0 Ausloeser, Kiste auf dem Record; Sperrzellen r3[4]/r3[2] |
| 2 — am Original messen: bis zur Absprungzelle oder vorher geblockt? | **erfuellt** | orig_lauf5/6 des Bau-Agenten (EXE-Stub Auslieferung, Haltepunkte an den disassemblierten Stellen, Zellbytes = RDT) + eigene Wiederholung NORD: der ERSTE Schubschritt trifft die Bruestungszelle 0x80123984 (r3[4]), Ruecksetzung auf den Record, 0 Fall-Ausloeser — das Original blockt lange vor jeder Absprungzelle (alle Band-5-Absprungzellen liegen bei z >= -3600, Kiste bei -15482) |
| 3 — Port blockt an derselben Stelle mit demselben Mechanismus (Beleg @0x, Pin) | **erfuellt** | Kette re15_prop_push_tick = FUN_8002bd44 @0x8002bf14-c0e8 (Adressen von mir disassembliert, s. RE-Gate); gleiche Treffer-Zelle, gleiche Ziel-Lage (Spieler +- 1350), gleiche Ruecksetzung; kein Verhaltens-Fix noetig und keiner gemacht; Pins `unit_r36_dachkiste_zensus` (Modell) + `integration_r36_dachkiste` (echte exe, Leon und Elza ueber den echten Weg, diskriminierend: SCHUB-OK/AUSLOESER/Lage != Record = rot), beide von mir gruen gefahren |
| 4 — dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt ein Fortschritt an der Kiste? | **teilweise** | 3050/3051: belegt und von mir am RDT nachgelesen (Fall = vorgesehener Weg zur Super Redhawk, Tueren floor 0/4, nichts auf floor 5). 2050/2051: Schlussfolgerung richtig, aber der Beleg im Dossier ist in einem Punkt falsch (2070 slot 5 -> 2040 existiert, Maengel M1), der genannte Ersatzweg 2030 -> 2060 -> 2040 ist nicht belegt (M2), kein Pin fuer 3050/2050 (M3) |

## Maengel (nummeriert, nachpruefbar)

1. **M1 — Dossier 4.3 "NACHGEPRUEFT" ist falsch, Zensusdatei unvollstaendig.** `R_dachkiste.md` 4.3 behauptet:
   "ROOM2070/2071 hat KEINE Tuer nach 2040 (slots 0..3 …); der Vorgaenger-Eintrag '2070 slot 5' ist falsch".
   Eigener Dump (`python re15_port/tools/scd_dump_room.py info/Re1.5/PSX/STAGE2/ROOM2070.RDT`): main00 @0x00B1C
   `Door_aot_set 3b 05 02 31 00 00 88 13 18 b1 e8 03 d0 07 da b2 d8 dc 76 b2 00 00 01 04 0c 05 00 …` = slot 5,
   floor 0, Stage-Index 1, Ziel ROOM2040, cut 12, Ankunft Band 5 — UNBEDINGT (nach dem Ifel/Else-Block
   @0x00ABA-0B1A, vor dem Switch @0x00B3C); ROOM2071 identisch @0x00B1C. Gegenstueck ROOM2040 slot 4 @0x0131C
   (floor 5 -> ROOM2070 cut 9), das die Zensusdatei selbst listet. In `R_belege/stage2_tueren_2040.txt` fehlen die
   Records 2070/2071 @0x00AD6, @0x00AFA (slot 4) und @0x00B1C (slot 5). Der Vorgaenger-Eintrag war richtig.
   Nachbesserung: 4.3 und die Zensusdatei berichtigen; der Weg ohne Kiste ist ROOM2000/2001 slot 3 @0x0167E ->
   ROOM2070 -> slot 5 @0x00B1C -> ROOM2040 (Band 5).
2. **M2 — der im Dossier genannte Ersatzweg 2030 -> 2060 -> 2040 ist nicht belegt.** ROOM2030 slot 3 @0x01B4C (->
   2060) liegt auf floor 9, die Ankuenfte in 2030 aus 2070 (slots 1/2 @0x00A5A/@0x00A7A) sind Band 3 und aus 2060
   (slot 0 @0x00D9A) Band 9; ROOM2060 slot 1 @0x00DBA (-> 2040) liegt auf floor 1, die Ankunft aus 2030 ist Band 0.
   Die Etagenwechsel innerhalb von 2030/2060 (Band 3 -> 9 bzw. 0 -> 1) sind im Dossier nicht gezeigt. Entweder
   belegen (Treppen-/Leiter-AOTs, Sprung-Lauf) oder den Satz durch den Weg aus M1 ersetzen.
3. **M3 — kein Pin fuer Punkt 4.** `probe_r36_dachkiste.c` fuehrt 3050/3051 und 2050/2051 mit `soll = -1`
   (nur Diagnose); der Zensus-Befund "Ausloeser mit Spieler erreichbar" (3050: 7 Uebergaenge, erste Lage
   (-23875,-18500); 2050: 1, (7550,-12000)) wird von keinem Test gehalten. Der Fall der Kiste obj 6 in 2050/2051
   wurde weder an der exe noch am Original gemessen (unter OFFEN als "nicht Auftrag" abgelegt). Nachbesserung:
   mindestens 3050/3051 als `soll = 1` pinnen (Fall = vorgesehener Weg), 2050/2051 ebenso oder mit Begruendung
   als Diagnose kennzeichnen.

Keine Maengel am Code (reiner Messschrieb + Riegel, kein Verhalten), keine Vertragsverletzung, keine Regression in
den beruehrten Modulen (31/31 Unit-Gruppe, Riegel 1/1).

## Gesamt

**Softlock-Frage (Kern des Auftrags): NEIN — Port = Original, von mir an der exe (9 Laeufe) und am Original (1 Lauf)
bestaetigt.** Punkte 1-3 erfuellt; Punkt 4 teilweise (Dossier-Fehler M1, unbelegter Ersatzweg M2, fehlender Pin M3)
-> **bestanden = false** wegen Punkt 4; die Nachbesserung ist Dossier/Test, kein Verhaltens-Code.

Messpfade: Riegel `re15_port/build/r36_dachkiste_wd/lauf_bd57a280/{leon,elza,west}/{debug.log,push.txt,objfall.txt}`;
eigene Laeufe Scratchpad `abn/{leon_echt,elza_echt,nord,diag45,rennen,nordtasche_west}/`, Original
`abn/r36R/orig/lauf_nord/log.txt` (unversioniert, Inhalte oben zitiert).
