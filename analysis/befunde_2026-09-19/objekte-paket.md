# Fehlende Objekte in der ausgelieferten Version (ROOM11F0 Schalter, ROOM1050 Leiche, ROOM1090 Kiste)

Runde 16, 2026-09-19. Nutzer: "In der paketierten Version fehlen diverse Dinge in ROOM 11F0
mindestens die Schalter vom Raetsel, in ROOM 1050 die zerteilte Leiche, in ROOM 1090 die Kiste usw."

## 0. Kurzfassung

Alle drei gemeldeten Objekte sind RDT-Props (`Obj_model_set`, 0x2D): die zwei Leichen-Haelften
in ROOM1050 sind obj 0x00/0x01, die Kisten in ROOM1090 obj 0x00/0x01, die zehn Schalter in
ROOM11F0 obj 0x02..0x0B. Das Paket v0.8.4 zeichnet sie alle, solange der Raum FRISCH betreten
wird — gemessen mit der Paket-exe selbst, Release- und Debug-Build sind identisch; es ist KEIN
Release-/Compiler-Defekt. Sie verschwinden, sobald vorher irgendein Raum durchlaufen wurde, dessen
`Item_aot_set` ein bereits genommenes Item traegt: der Port merkt sich "Prop-Index N ist genommen"
in einer PROZESSWEITEN Maske (`s_prop_taken_hidden`, scd_vm.c:414), die nur beim Boot genullt wird
(scd_vm_init, main.c:3472) — nicht beim Raumwechsel (scd_room_reenter memsettet nur g_scd,
scd_room_setup.c:211). Danach legt JEDER Raum seine Props mit obj_id N unsichtbar an (scd_vm.c:3803).
Das Original haelt diesen Zustand im raumlokalen Objekt-Pool @0x800b3f98 (Item_aot_set schreibt
`pool[tk_prop].flags = 0x80000000` @0x80040718), und der Raumlader FUN_800396fc nullt den Pool bei
jedem Laden (FUN_8003ea7c @0x8003eab0-cc, 32 Eintraege, Schritt 148). Der Port muss die Maske in
scd_room_reenter zuruecksetzen (und das Verstecken per obj_id statt Slot-Index adressieren).

## 1. Reproduktion/Messung

### 1.1 Was die Objekte sind (RDT selbst gelesen, `scratchpad/scdwalk.py`)

| Raum | nOmodel | Record (Datei-Offset) | obj | Typ | Position | Bedeutung |
|---|---|---|---|---|---|---|
| ROOM1050 | 2 | main00 @0x0BB0 | 0x01 | 0 | (14775,0,-10125) rot_y 1199 | Leichen-Haelfte / Rolltor-Szene (Cut 3,4,5) |
| ROOM1050 | 2 | sub00 @0x0C36 | 0x00 | 0 | (15432,0,-10424) rot_y 2048 | Leichen-Haelfte (Cut 3,4,5) |
| ROOM1090 | 4 | main00 @0x216E | 0x00 | 4 | (-8136,-1800,4753) | schiebbare Kiste, Band 1 (Cut 0,3,8,11) |
| ROOM1090 | 4 | main00 @0x2190 | 0x01 | 4 | (-5960,-9000,-15482) | schiebbare Kiste, Band 5 (Cut 1,2,9,10) |
| ROOM11F0 | 12 | sub00 @0x0E54 | 0x00 | 4 | (-19554,0,22684) | Cursor/Maschine (Cut 10) |
| ROOM11F0 | 12 | sub00 @0x0E76..0x0FA8 (+0x22) | 0x02..0x0B | 0 | x=-25975/-18775, z=26000..17800 | die 10 Schalter (Cut 10) |
| ROOM11F0 | 12 | main00 @0x0D10 | 0x01 | 0 | (11040,-16200,-15084) | (Cut 2,3,4,9,11) |

Die "sichtbar aus Cut" Angaben stammen aus dem RVD-Anker-Quad je Cut (`scratchpad/zones.py`,
Region-Test identisch zu `re15_prop_culled`/FUN_80014368). ROOM11F0s Cut 10 ist der Panel-Blick
(Region x[-32700,-6500] z[12500,32700] @RDT 0x520). Sce_em_set der drei Raeume sind Zombies
(11F0: 0x10/0x11) bzw. Ada 0x42 (1050 @0x0C8E, 1090 geparkt @0x22CC) — keine Leiche, keine Kiste.
ESP-Effekte: keine der drei Sachen.

### 1.2 Paket-exe frisch betreten: ALLES da (Paket, lokales Release, Debug identisch)

Paket `release/re15_port_v0.8.4_win64.zip(+.z01)` entpackt (SHA256 der exe == release/win_out
== release/wxbuild, gebaut mit GCC 10 cross, `-O3 -DNDEBUG`; RDTs im Paket byte-gleich zum Repo
und zu info/Re1.5/PSX). Lauf: `RE15_NO_INTRO=1 RE15_INPUT_SCRIPT="W3,A0.2,W8,A0.2,W3,A0.2,W3,A0.2,W40"
RE15_DEBUG_JUMP=<raum>@30 RE15_FORCE_CUT=<cut> RE15_SOFTWARE_RENDER=1 RE15_FRAMEDUMP=...`
(debug.log = stderr, unbuffered; `[prop-render]` wird pro Prop-Slot EINMAL je Prozess geloggt).

| Lauf | Paket-exe | lokales Release (GCC 16 -O3) | Debug (re15_port/build) |
|---|---|---|---|
| 11F0 Cut 10 | 11 `[prop-render]` (obj 0, 2..B) | 11 | 11 |
| 1050 Cut 3 | 2 (obj 1, 0) | 2 | 2 |
| 1090 Cut 0 | obj 0 + obj 3 | gleich | gleich |
| 1090 Cut 1 | obj 0, 3 (+1) | gleich | gleich |

Bild `scratchpad/fd_pkg_11F0_c10/fd_000250.png`: alle 10 Schalter + Cursor. Bild
`fd_pkg_1050_c3/fd_000250.png`: Rolltor unten, zerteilte Leiche darunter. Kandidaten aus der
Aufgabenstellung (UB unter -O2/-O3, Prop-Cull, Textur-Slot, Asset-Root, Release-Pfade) sind damit
ausgeschlossen: gleiche Zaehler, gleiche Bilder in allen drei Binaries.

### 1.3 Paket-exe nach einem Raum mit genommenem Item: Props WEG

Simulation "Item schon genommen": `RE15_SET_FLAG=9:166,9:167` (= Flag-Zone 9 Bits der beiden
Items in ROOM1000 sub00 @0x0C3A/@0x0C50, tk_prop 0/1), `RE15_DEBUG_JUMP=1000@5`, dann
`RE15_GOTO_ROOM=1050 RE15_FORCE_CUT=3`:

| Lauf (Paket-exe) | ROOM1000 | Folgeraum |
|---|---|---|
| Kontrolle ohne Flags: 1000 -> 1050 | obj 0 gezeichnet | 1050: Leiche gezeichnet (`mask_pkg_ctrl_1050/fd_250.png`) |
| Flags 9:166/167: 1000 -> 1050 | obj 0/1 versteckt (korrekt) | 1050: **0 `[prop-render]`, Leiche und Rolltor fehlen** (`mask_pkg_1050/fd_000250.png`) |
| Flags 9:166/167: 1000 -> 1090 Cut 0 | versteckt | 1090: nur obj 3 gezeichnet, **Kisten obj 0/1 fehlen** |
| Flags 9:166/167: 1000 -> 1090 Cut 3 (Kisten-Blick) | versteckt | 1090: 0 `[prop-render]`, **Kiste weg, Leon steht auf ihrem Platz** (`kiste_pkg_maske/fd_250.png`; frisch: 1 `[prop-render]`, Kiste vor der Tuer, `kiste_pkg_frisch/fd_250.png`) |
| Flag 9:142 (ROOM1010 Item tk_prop 2): 1010 -> 11F0 Cut 10 | obj 2 versteckt | 11F0: **Schalter obj 2 fehlt** (leerer Sockel oben links, `mask_pkg_11F0/fd_000250.png`), obj 3..B da |

### 1.4 Sonde (deterministisch, ohne Renderer)

`re15_port/tests/unit/probe_r16_objekte_paket.c` (Registrierung `tests/unit/probes/r16_objekte-paket.cmake`,
gebaut in `re15_port/build_r16_objekte-paket`), liest `g_scd.props[].active` nach
`scd_room_reenter` + 60 Ticks:

```
== A Kontrolle: Boot -> ROOM1050
  [1050 frisch] prop_count=2: obj01=AN obj00=AN                       ok
== B Boot + Flags(9,166),(9,167) -> ROOM1000 -> ROOM1050
  [1000 mit genommenen Items] prop_count=2: obj00=aus obj01=aus       ok (E: im selben Raum versteckt = Original)
  [1050 nach 1000] prop_count=2: obj01=aus obj00=aus                  FAIL (Original: AN)
== C Boot + Flags(9,140/141/142) -> ROOM1010 -> ROOM11F0
  [11F0 nach 1010] prop_count=12: obj01=aus obj00=aus obj02=aus obj03..0B=AN   FAIL x3
== D Boot + Flags(9,166/167) -> ROOM1000 -> ROOM1090
  [1090 nach 1000] prop_count=3: obj00=aus obj01=aus obj03=AN         FAIL
DEFEKT: 5 Abweichung(en) vom Original   (rc=1)
```

### 1.5 Warum es beim Nutzer "diverse Dinge" trifft — Zensus der Item-Prop-Indizes (STAGE1)

Echte `Item_aot_set` (sce=9) mit Taken-Bit und tk_prop < 32 in STAGE1 (`scratchpad/itemcensus.py`, 64 Records):
tk_prop 0: ROOM1000/1010/1011/1070/1110/11D0/1220; 1: 1000/1010/10F0/1110/11B0/11D0/11F0/1220;
2: 1010/10F0/1110/11B0/1220; 3: 10F0/1110; 4/5: ROOM1020; 6..14: ROOM1190 sub13/sub14 (die 8 Items
im Ausruestungsraum). Wer diese Raeume nach dem Aufsammeln nochmals betritt (Normalfall: 1000,
1010, 1020, 1190 sind Durchgangsraeume), traegt danach die Bits 0..14 — genau die obj_ids der
Leiche (0/1), der Kisten (0/1), des Cursors (0) und aller Schalter (2..11). Mein Debug-JUMP-Lauf
startet frisch und zeigt deshalb alles; der Nutzer spielt durch und sieht nichts.

### 1.6 Messfallen dieser Runde

- Die Paket-exe kommt nicht ueber den Titel, wenn das Input-Script nur einmal bestaetigt: der
  Titel-Fade dauert ~5,5 s echte Zeit (@0x80102cd4-d9c), danach wartet die Charakterwahl. Script
  mit mehreren A-Tasten (s.o.) noetig; `RE15_DEBUG_JUMP` zaehlt nur In-Game-Frames.
- `RE15_FRAMEDUMP` braucht unter Git-Bash `MSYS_NO_PATHCONV=1` und einen `C:/...`-Pfad, sonst
  wird das `/` im Step-Teil zerlegt und JEDES Bild geschrieben (1,4 GB pro Lauf).
- `[prop-render]` (main.c:8845 `s_prop_logged[pi]`) ist ein Prozess-Latch je Slot-Index — nach dem
  ersten Raum fehlen Zeilen fuer Slots, die schon einmal geloggt wurden. Zaehler nur im ersten Raum
  eines Prozesses vergleichen; sonst Bild oder Sonde.

## 2. Original-Mechanismus

### 2.1 Item_aot_set (LAB_80040644): genommenes Item versteckt SEINEN Pool-Eintrag

PSX.EXE, `re15_disasm.py dis 0x800406c0`:
```
800406d4: lui  a0,0x8007
800406d8: lw   a0,18056(a0)        ; 0x80074688 -> DAT_800b1078 = Flag-Zone 9 (Taken-Bits)
800406dc: jal  0x8004efe4          ; Flag-Test(Zone, Bit = Record+4)
800406e4: beq  v0,zero,0x8004071c  ; nicht genommen -> fertig
800406e8: lui  v1,0x8000           ; v1 = 0x80000000
800406ec: lw   v0,0(s0)
800406f4: sb   zero,0(v0)          ; Record.sce = 0 (AOT inert)
800406f8: sll  v0,s1,3             ; s1 = tk_prop
800406fc: addu v0,v0,s1            ; *9
80040700: sll  v0,v0,2             ; *36
80040704: addu v0,v0,s1            ; *37
80040708: sll  v0,v0,2             ; *148 = Pool-Schrittweite
8004070c: lui  at,0x800b
80040710: addiu at,at,16280        ; 0x800b3f98 = Objekt-Pool
80040714: addu at,at,v0
80040718: sw   v1,0(at)            ; pool[tk_prop].flags = 0x80000000  (Bit 0 = 0 -> nicht gezeichnet)
```
Der Zeichner FUN_8002c18c prueft `(pool[i].flags & 1) != 0` (Decompile RE_15_Quellcode_V2/FUN_8002c18c.c,
Schleife bis nOmodel `DAT_800ac778+2`). Live-Aufnahme (FUN_80021eb4 @0x80021fc8 `sw zero,0(at)`
mit demselben `*148`-Index aus Record+6) nullt das Wort ebenfalls.

### 2.2 Der Pool ist RAUMLOKAL: FUN_8003ea7c nullt ihn bei jedem Raumladen

```
8003eaa4: sw   zero,-13920(at)     ; 0x800ac9a0 = 0
8003eaac: sw   zero,-13916(at)     ; 0x800ac9a4 = 0
8003eab0: lui  at,0x800b
8003eab4: addiu at,at,16280        ; 0x800b3f98
8003eab8: addu at,at,v1
8003eabc: sw   zero,0(at)          ; pool[i].flags = 0
8003eac0: addiu s0,s0,1
8003eac4: sltiu v0,s0,0x20         ; 32 Eintraege
8003eac8: bne  v0,zero,0x8003eab0
8003eacc: addiu v1,v1,148
8003ead4: lw   v0,-14472(v0)       ; DAT_800ac778 = RDT
8003eadc: lbu  v0,2(v0)            ; nOmodel -> danach Modell-Bind je obj_id
```
Aufruf aus der Raumlade-Kette FUN_800396fc: `800399a0: jal FUN_8003ea7c` (ghidra1_V2.txt:137745;
Decompile FUN_800396fc.c:59, nach FUN_80043eac/FUN_80043fb0, vor FUN_80039c2c). Es gibt im
Original keinen raumuebergreifenden "genommen"-Speicher fuer Props: die Persistenz sitzt allein
im Taken-Bit (Zone 9), und jeder Raum wendet sie ueber sein EIGENES Item_aot_set auf seinen
EIGENEN Pool an.

### 2.3 Obj_model_set (LAB_80040914) auf frisch genulltem Pool

```
80040978: lw   v0,0(a1)            ; pool[obj_id].flags
80040980: beq  v0,zero,0x80040990  ; 0 (frisch) -> flags = pc[6..7] | 1  (@0x80040990-a4)
8004098c: sw   zero,0(a1)          ; sonst (Zweitinstallation) -> 0 = AUS
```
Ein Eintrag mit 0x80000000 aus 2.1 wuerde hier ebenfalls auf AUS laufen — deshalb ist die
Nullung in 2.2 die Voraussetzung dafuer, dass ein neuer Raum seine Props ueberhaupt sieht.

## 3. Port-Ist

- `re15_port/engine/src/scd_vm.c:414` `static uint32_t s_prop_taken_hidden = 0;` — Datei-static,
  ausserhalb von `g_scd`.
- `scd_vm.c:419` (in `scd_vm_init`): einzige Nullung. `scd_vm_init` wird nur beim Boot gerufen
  (`platform/pc/main.c:3472`).
- `scd_vm.c:3690-3696` (`op_item_aot_set`): `if (tk_bit && re15_game_flag_get(9, tk_bit))` ->
  `s_prop_taken_hidden |= 1u << tk_prop` und `g_scd.props[tk_prop].active = 0` (SLOT-Index!).
- `scd_vm.c:3803-3804` (`op_obj_model_set`): `active = (reinstall || (s_prop_taken_hidden &
  (1u << obj_id))) ? 0 : 1` — wendet die Maske auf JEDEN spaeteren Raum an.
- `re15_port/engine/src/scd_room_setup.c:211` (`scd_room_reenter`, alle drei Ladewege des Ports):
  `memset(&g_scd, 0, ...)` — die Maske bleibt stehen. Ergebnis wie in 1.3/1.4: nach ROOM1000 mit
  genommenen Items sind in ROOM1050/1090 obj 0/1 unsichtbar, nach ROOM1010 in ROOM11F0 obj 2 usw.
- Zweiter, kleinerer Fehler an derselben Stelle: `g_scd.props[tk_prop].active = 0` adressiert den
  Port-Slot, nicht die obj_id (das Original indiziert den Pool mit tk_prop = obj_id, 2.1). Faellt
  zusammen, solange ein Raum seine Props in obj_id-Reihenfolge ab 0 installiert — ROOM1190 sub14
  (obj 7..16 auf Slots 0..9) tut das nicht.
- Der Zeichenpfad (`main.c:8791-8815`, `re15_prop_culled`, `RE15_TIM_SLOT_PROP`, `pc_load_room_prop_set`)
  ist in Ordnung: alle Props aller drei Raeume werden geladen (`s_room_prop_ok`) und in Paket/Release/
  Debug identisch gezeichnet (1.2).

## 4. Fix-Plan

1. `re15_port/engine/src/scd_vm.c`: neue Funktion `void scd_prop_taken_mask_reset(void)`
   (`s_prop_taken_hidden = 0;`), Kommentar: byte-true FUN_8003ea7c @0x8003eab0-cc (Pool-Nullung,
   32 Eintraege, Schritt 148), gerufen aus FUN_800396fc @0x800399a0 bei JEDEM Raumladen.
   `scd_vm_init` ruft sie ebenfalls (statt der Direktzuweisung in Zeile 419).
2. `re15_port/engine/src/scd_room_setup.c:211` (`scd_room_reenter`), direkt nach dem
   `memset(&g_scd, ...)`: `scd_prop_taken_mask_reset();`. Damit gilt die Maske genau wie der
   Original-Pool nur fuer den gerade geladenen Raum; das Item_aot_set des Raums setzt sie beim
   Laden neu, wenn das Item genommen ist (Sonde Teil E bleibt gruen).
   Deklaration in `re15_port/include/re15_scd.h` neben `scd_vm_init`.
3. `scd_vm.c:3695-3696`: das direkte Verstecken per obj_id statt Slot: Schleife ueber
   `g_scd.props[k].obj_id == tk_prop` -> `active = 0` (byte-true Pool-Index @0x800406f8-718 =
   obj_id). Reihenfolge egal, weil die Maske den Fall "Item_aot_set vor Obj_model_set" abdeckt.
4. Keine Konstante neu; keine Aenderung an Zeichner, Cull oder Textur-Slots.
5. Absicherung: `tests/unit/probes/r16_objekte-paket.cmake` um
   `add_test(NAME probe_r16_objekte_paket COMMAND probe_r16_objekte_paket)` ergaenzen — die Sonde
   prueft A (frisch), B/C/D (Folgeraum sichtbar) und E (gleicher Raum versteckt) und schlaegt
   heute mit rc=1 fehl. Bestehende Pins, die beruehrt sein koennten: `probe_item_model_1020`
   (Maske "Item_aot_set VOR Obj_model_set" in ROOM1020 — bleibt erhalten, weil die Maske im
   selben Raum weiterhin gesetzt wird) und `probe_gen_11f0`.
6. Verifikation danach mit der Paket-exe wie in 1.3 (`RE15_SET_FLAG=9:166,9:167 RE15_DEBUG_JUMP=1000@5
   RE15_GOTO_ROOM=1050 RE15_FORCE_CUT=3` -> Leiche/Rolltor sichtbar; `9:142 1010 -> 11F0 Cut 10`
   -> alle 10 Schalter) und dann Release-Paket neu bauen (docker_win_build.sh, nicht lokal).
   Risiko: gering — die Maske war eine reine Port-Konstruktion (2026-08-25) fuer den
   Reihenfolge-Fall in ROOM1020; ihre Lebensdauer wird nur auf den Raum begrenzt.

## 5. Offen / nicht belegt

- Nicht per Savestate im Original gegengemessen (DuckStation), dass ROOM1050/1090/11F0 nach
  Aufsammeln in 1000/1010 ihre Props zeigen — der Beleg ist statisch (2.1/2.2: Pool-Nullung bei
  jedem Laden, kein zweiter Speicherort). Ein DuckStation-Lauf 1000 (Item nehmen) -> 1050 waere
  die letzte Bestaetigung; `stage_saves/room1090_orig.sav` liegt bereit, ist aber ein
  Einzelzustand ohne Vorgeschichte.
- Welche Items der Nutzer tatsaechlich genommen hat, ist nicht bekannt (sein Save liegt nicht im
  Repo); die Zuordnung in 1.5 ist die Menge aller Kandidaten. "usw." im Report deckt jeden Prop
  mit obj_id <= 14 in jedem Raum ab.
- Die Zweitinstallations-Regel (2.3, `reinstall -> AUS`) wurde hier nicht neu geprueft; sie ist
  fuer die drei Raeume irrelevant (keine doppelte obj_id).

## 6. Umsetzung (Phase 2, 2026-09-19, Branch `worktree-wf_074e2f88-24e-1`, Thema objekte-und-tuer)

Umgesetzt wie Abschnitt 4 plus die Korrekturen der Gegenpruefung (`objekte-paket.skeptiker.md` §3):

1. `re15_port/engine/src/scd_vm.c`: `scd_prop_taken_mask_reset()` (= Pool-Nullung FUN_8003ea7c
   @0x8003eab0-cc, 32 Eintraege, Schritt 148; Aufruf aus FUN_800396fc @0x800399a0) und
   `scd_prop_hide_by_obj_id()` (Pool-Index = tk_prop = obj_id, @0x800406f8-0x80040718; die
   Live-Aufnahme nutzt denselben Index @0x80021fa0-fc8). `scd_vm_init` ruft die Reset-Funktion.
   `op_item_aot_set` versteckt ueber die obj_id statt ueber den Slot-Index.
2. `re15_port/engine/src/scd_room_setup.c` (`scd_room_reenter`, direkt nach dem `memset(&g_scd)`):
   `scd_prop_taken_mask_reset()`. Gilt damit fuer alle drei Ladewege des Ports, auch den
   Same-Room-Reenter (game_step_common.c) — konsistent mit dem einzigen Ladeweg des Originals
   (0x8001ca54 -> FUN_8001d600 -> FUN_800396fc -> FUN_8003ea7c), im Kommentar so begruendet.
3. Skeptiker §3.4 (Live-Aufnahme im selben Raum): `engine/src/item_modal_common.c` versteckte den
   Prop ebenfalls ueber den Slot-Index (`g_scd.props[s_taken_prop]`), nicht ueber die obj_id —
   dieselbe Fehlerklasse wie 4.3 (ROOM1190 sub14: obj 7..16 auf Slots 0..9). Auf den Helfer
   `scd_prop_hide_by_obj_id` umgestellt (`sw zero,0(at)` @0x80021fc8, Index `lbu v0,6(s0)` @0x80021fa0).
4. Deklarationen in `include/re15_scd.h`; `tests/unit/probes/r16_objekte-paket.cmake` traegt jetzt
   `add_test(probe_r16_objekte_paket)` (TIMEOUT 60) — die Sonde ist der Pin.

Messwerte (Sonde `probe_r16_objekte_paket`, deterministisch, ohne Renderer):

| Fall | vorher (1.4) | nachher |
|---|---|---|
| A 1050 frisch | obj0/1 AN | obj0/1 AN |
| E 1000 mit Taken-Bits 166/167 (gleicher Raum) | obj0/1 aus | obj0/1 aus (erhalten) |
| B 1050 nach 1000 | obj0/1 **aus** (FAIL) | obj01=AN obj00=AN |
| C 11F0 nach 1010 (Bits 140/141/142) | obj0/1/2 **aus** (FAIL x3) | obj00..0B alle AN |
| D 1090 nach 1000 | obj0/1 **aus** (FAIL) | obj00=AN obj01=AN obj03=AN |
| Summe | `DEFEKT: 5 Abweichung(en)`, rc=1 | `OK: 0 Abweichung(en)`, rc=0 |

`probe_item_model_1020` (kein add_test, von Hand): Ausgabe vor/nach dem Fix byte-gleich
(Abschnitt C: nach Neuladen prop[0]/prop[1] active=1, AOT-Slots 3/4/5 aktiv) — die Maske
"Item_aot_set VOR Obj_model_set" im selben Raum bleibt wirksam. `unit_gen_11f0_switches` gruen.

Gegenmessung 4.6 mit der Worktree-exe (build_p2, Startweg RE15_TITLE_SHOT + RE15_DEBUG_JUMP +
RE15_GOTO_ROOM, Bilder/Logs unter `analysis/befunde_2026-09-19/phase2_objekte-und-tuer/`):

| Lauf | vorher (1.3, Paket-exe) | nachher (Worktree-exe) |
|---|---|---|
| Flags 9:166/167, 1000@5 -> 1050 Cut 3 | 0 `[prop-render]`, Leiche/Rolltor fehlen | `[prop-render] pi=0 oid=0x01`, `pi=1 oid=0x00` (`obj_1050.log`), Bild `obj_1050_F250.png` |
| Flag 9:142, 1010@5 -> 11F0 Cut 10 | Schalter obj 2 fehlt (leerer Sockel) | `pi=2 oid=0x02` … `pi=11 oid=0x0B` — alle 10 Schalter (`obj_11F0.log`, `obj_11F0_F250.png`) |

(In ROOM1000/1010 selbst bleiben die genommenen Items unsichtbar: dort erscheinen die Slots 0/1
bzw. 0/1/2 im ersten Raum nicht als `[prop-render]` — 1000: keine Zeile; 1010: nur die zwei
nicht genommenen Props, obj 2 fehlt korrekt.)

Offen: kein DuckStation-Gegenlauf (statischer Beweis geschlossen, s. Skeptiker Befund 2);
das Release-Paket ist nach dem Merge ueber docker_win_build.sh neu zu bauen (die Paket-exe
selbst wurde nicht neu gebaut).
