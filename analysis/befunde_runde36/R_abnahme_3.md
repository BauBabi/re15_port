# Runde 36 — Spur R "dachkiste" — Abnahme 3 (unabhaengig, 2026-10-12, Nachbesserung 3)

Baum `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste` @ 70f2cd7e (Basis 99398dc7; Abnahme 2 = 11f75e5a).
Bericht fortlaufend geschrieben (Sitzungslimit). Stand: ABGESCHLOSSEN — Punkte 1-3 erfuellt, Punkt 4 teilweise
(ROOM3050: Fortschritt Super Redhawk im Port ohne Kiste aufnehmbar, Lift-Band gebrochen; am Original gegengemessen) ->
bestanden = false (Maengel 1-3, Abschnitt 9).

## 0. Bau

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
* exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-11 23:02:50, sha256 `09d8fba15161…1b342f11`.
  Gegenprobe Quellstand: `git diff 50edb2be..HEAD --stat -- re15_port` aendert NUR
  `tests/integration/test_r36_dachkiste.cmake` und `tests/unit/probes/r36_dachkiste.cmake`; letzte Engine-Quelle
  `zellschub_r36.c` 23:02:06 < exe 23:02:50 -> die exe IST der Engine-Stand von HEAD (Tests werden per cmake -P
  gelesen, kein Neubau noetig). Arbeitsbaum sauber (`git status`).

## 1. Code-Diff (git diff 99398dc7 -- re15_port, gelesen)

Gegenueber Abnahme 2 NEU und VERHALTENSRELEVANT (Nachbesserung 3):
* `engine/src/zellschub_r36.c` (+229) / `include/re15_zellschub_r36.h` (+80): `re15_zellschub_8003b558` (FUN_8003b558 mit
  Nicht-Quadrat-Zweig und Ausschiebung je Treffer), `re15_zellschub_8003da78` (FUN_8003da78, Spiegel explizit),
  `re15_zellschub_ruhe` (LAB_8002c0b8 fuer ein Objekt), `re15_zellschub_spiegel` (+0x40/+0x44), `re15_zellschub_nach_pass`
  (Bilder ohne Objekt-Pass), `re15_zellschub_install` (Spiegel = Lage beim Raumaufbau, als PORT-WAHL gekennzeichnet).
* `engine/src/aot_common.c` `re15_prop_push_tick`: Stand-Objekt (DAT_800ac788) -> nur Spiegel; Szenen-Schub 0x200 (Spur J)
  und jeder Nicht-Schub-Zweig -> Ruhe-Zweig; Schubtest ueber `re15_zellschub_8003b558` auf eine KOPIE der Ziel-Lage;
  `flags & 0x80` -> Ruhe-Zweig (@0x8002bf78-84); Spiegel am Ende jedes Eintrags.
* `engine/src/re15_collision.c`: `y_ueberlappt_8002cb84` (Y-Test) in `push_out_of` (Spieler UND NPC-Familie 0x40..0x4d) und
  `re15_collision_push_prop` (Schiebe-Modus a2 = 1); Aktor-Box als `RE15_AKTORBOX_CY/HY` (-1530/1530) mit Beleg.
  Signaturaenderung `re15_collision_objects_actor(x, z, actor_y)` -> Aufrufer `enemy_ai_common.c` (1 Zeile) und der
  bestehende Pin `test_1090_npc_prop_push.c` (1 Zeile).
* Haken: `game_step_common.c` (+3: Include + `re15_zellschub_nach_pass` vor `re15_objfall_nach_pass`; der Objekt-Pass
  `re15_prop_push_tick` steht vorher (Zeile 1681), der Merker `s_pass_lief` verhindert einen Doppellauf — Reihenfolge
  geprueft), `scd_room_setup.c` (+2: Include + Installer-Zeile am Blockende mit "Runde 36 Spur R").
* Diagnose/Messung (kein Verhalten): `dachkiste_r36.c` (RE15_PUSH_LOG inkl. RUHE-AUSSCHIEBUNG, RE15_WEGPLAN mit neuem
  Befehl FLAG), `input_pc.c` 2-Zeilen-Haken (unveraendert seit Abnahme 2).
* Guess-Tells (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/HACK/approx/vermutlich) in den
  `+`-Zeilen des re15_port-Diffs: KEINE. `getenv` im Engine-Diff nur RE15_PUSH_LOG / RE15_WEGPLAN / RE15_WEGPLAN_LOG
  (Messschrieb/Mess-Steuerung; kein Spiel-Code liest sie, kein Env-Schalter als Abschluss).

## 2. RE-Gate (selbst disassembliert: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, info/Re1.5/PSX.EXE)

Stichproben (mehr als drei, weil Nachbesserung 3 Verhalten aendert):
1. **Ruhe-Zweig LAB_8002c0b8** (`dis 0x8002bf74 16`, `dis 0x8002c0a0 30`, `dis 0x8002beb0 40`): `8002bf78 lw v0,0(s2) /
   8002bf80 andi v0,v0,0x80 / 8002bf84 bne v0,zero,0x8002c0b8`, `8002bf8c lbu v1,140(s2) / 8002bf90 ori v0,zero,0x9 /
   8002bf94 bne v1,v0,0x8002c0b8`, `8002c0b0 sb s7,140(s2)` (gesperrt), **`8002c0b8 ori a1,zero,0x2` / `8002c0bc-d4` obj.x/z
   = (s16)s4/s5 / `8002c0d0 jal 0x8003b558` / danach `8002c0d8-e4` ohne Lesen von v0 `jal 0x8002cabc` (a2 = 0)** — die
   Rueckgabe wird verworfen, die Ausschiebung bleibt. Szenen-Schub `8002bec0 andi v0,v0,0x200` -> `8002bed4 jal 0x8002cabc`
   (a2 = 1) -> `8002bedc/bee0 lhu s4,52 / lhu s5,60` -> `8002bee4 j 0x8002c0b8`; Stand-Objekt `8002bef0 lw v0,0(s6)` /
   `8002bef8 bne s2,v0,0x8002bf14` / `8002bf04 jal 0x8002cfd4` / `8002bf0c j 0x8002c0ec` (kein FUN_8003b558); Spiegel
   `8002c0ec lhu v1,56 / c0f0 lhu v0,52 / c0f4 lhu a0,60 / c0f8 sh v1,66 / c100 sh v0,64 / c10c sh a0,68`. Port = dasselbe.
2. **FUN_8003b558 Nicht-Quadrat-Zweig** (`dis 0x8003b628 112`): `8003b684 lhu v1,6(a3)` hx, `8003b688 lhu v0,10(a3)` hz,
   `8003b68c lhu t0,6(a3)` r = hx, `8003b690 beq v1,v0,0x8003b738`; x-Code `8003b6b4-6cc` = ((cxw-x1)&0x80000000 |
   ((cxw-x0)&0x80000000)>>1)>>30, z-Code `8003b6e4-704`; `8003b6fc bne a0,v0(=2),0x8003b738`, `8003b708 beq v1,a0,0x8003b738`,
   `8003b70c sltiu v0,v1,0x3 / 8003b710 beq v0,zero,0x8003b728 / 8003b718 beq v1,zero,0x8003b734 / 8003b720 j 0x8003b738 /
   8003b728 ori v0,zero,0x3 / 8003b72c bne v1,v0,0x8003b738 / 8003b734 lhu t0,10(a3)` => r = hz genau bei x-Code 2 und
   z-Code 0 oder 3 = `if (xcode == 2u && zcode != 2u && (zcode == 3u || zcode == 0u)) r = hz;` (zellschub_r36.c). Band
   `8003b634-644` ((s16)(Zelle+10)>>12), Klasse `8003b64c-660` ((s8)u0 & Maske), obj+0 & 8 `8003b668-674`, Rechteck
   `8003b738-794` mit obj.x/z JE ZELLE neu gelesen (`8003b73c lw v0,52(s1)`, `8003b774 lw v0,60(s1)`), Treffer
   `8003b79c jal 0x8003da78` (a0 = Zelle `8003b798`, a1 = obj `8003b7a0`, a2 = r `8003b74c`), `8003b7a4 ori s5,s5,0x1`. Stimmt.
3. **FUN_8003da78** (`dis 0x8003da78 152`): `8003dab0 addiu t0,v0,18` ((x1+r)-cxw+18), `8003dab8 addiu a0,a0,-18`,
   `8003dabc-dacc` kleinerer Betrag (`slt -a0,t0` -> t0 = a0), z `8003daf0 addiu a3,v0,18` / `8003dafc addiu a0,a0,-18`;
   Lagecode `8003db14 lh v0,64(a1) / db18 lh v1,68(a1) / db1c-db38` (Bit0 sign(t0^dx), Bit1 sign(a3^dz)); Code 2
   `8003dba0-dbc4` (`addiu v0,v0,400 / andi 0xffff / sltiu v0,v0,0x321` -> `sw 60(a1)`), Code 1 `8003db74-db9c` dito x,
   Code 3 und Ueberlaeufe `8003dbc8-dc28` (kleinerer Betrag), Code 0 `8003dc2c-dc94` (Lage == Spiegel -> kleinerer
   Betrag) bzw. `8003dc98-dcb4` (Lage != Spiegel -> obj.x/z = Spiegel, `addu v0,zero,zero`). Jede Port-Zeile in
   `re15_zellschub_8003da78` entspricht einer dieser Instruktionen (18 / 400 / 0x321 / Rueckgabe 0 im bewegten Code 0).
4. **Y-Test FUN_8002cabc** (`dis 0x8002cabc 72`): `8002cb78 ori v0,zero,0x2 / 8002cb7c beq t9,v0,0x8002cde4` (a2 = 2 ohne
   Y-Test), `8002cb84 lh v1,2(t1)` objbox.cy, `8002cb88 lw a1,56(t0)` obj.y, `8002cb8c lh v0,2(t2)` Aktor-Box(+0x78).cy,
   `8002cb90 lw a0,56(a3)` Aktor.y, `8002cb9c lhu a1,8(t2)` / `8002cba0 lhu a0,8(t1)` hy, `8002cba4 subu t6,v1,v0`,
   `8002cba8 addu a1,a1,a0`, `8002cbac addu t3,t6,a1`, `8002cbb0 sll v0,a1,1`, `8002cbb4 sltu v0,v0,t3`, `8002cbb8 xori
   t3,v0,0x1`, `8002cbbc bne t3,zero,0x8002cbcc`, `8002cbc4 j 0x8002cde4` + `8002cbc8 addu v0,zero,zero`. Port
   `!((u32)(hySum<<1) < (u32)(dy+hySum))` = dasselbe. Aktor-Box: `xxd -s 0x64694 -l 12 PSX.EXE` = `0000 06fa 0000 c201
   fa05 c201` = {0,-1530,0,450,1530,450}; STAGE1.BIN Offset 0x21658 dieselben 12 Bytes. Einziger statischer Schreiber
   des Spieler-Box-Zeigers DAT_800acacc (= 0x800aca54 + 0x78): `80031664 sw v1,-0x3534(at)` mit v1 = 0x80073e94 aus
   PTR_DAT_80073ea0 (FUN_800314b0, Spieler-Init; ghidra1_V2.txt Z. 126342/126350) -> die feste Konstante ist belegt.
5. **Wiederbetreten** (Kern der neuen Softlock-Antwort): `dis 0x80039990 8` -> `800399a0 jal 0x8003ea7c`; `dis 0x8003ea7c
   30` -> `8003eab0 lui at,0x800b / 8003eab4 addiu at,at,16280` (0x800b3f98) / `8003eabc sw zero,0(at)` / `8003eac0 addiu
   s0,s0,1 / 8003eac4 sltiu v0,s0,0x20 / 8003eacc addiu v1,v1,148` = Wort 0 aller 32 Pool-Eintraege genullt. Installer
   `dis 0x80040914 64`: `80040978 lw v0,0(a1) / 80040980 beq v0,zero,0x80040990` (Wort 0 == 0 -> `80040990-800409a4`
   Wort 0 = Record-Flags | 1; sonst `8004098c sw zero,0(a1)`), danach UNBEDINGT `800409bc sw v0,52(a1)` / `800409c8 sw
   v0,56(a1)` / `800409d4 sw v0,60(a1)` (x/y/z aus dem Record +10/+12/+14); KEIN Store nach +64/+68 (Spiegel). SCD
   (`scd_dump_room.py ROOM2050.RDT`): main00 `0x01D0E Obj_model_set 2d 06 04 00 03 00 00 01 00 00 c0 f9 e8 ea 7c db ...
   obj=0x06 type=4 band=3 pos=(-1600,-5400,-9348)` steht VOR dem einzigen Ifel @0x01D30 (`Ck (5,7)=1`, gilt obj 8) =
   unbedingt. => Nach jedem Raumladen steht obj 6 aktiv auf dem Record, egal wo es vorher lag. Belegt.
6. **Bild 13 des Falls** (`dis 0x8002c640 40`): `8002c660 lbu v1,6(s1) / 8002c668 ori v0,zero,0xd / 8002c66c bne
   v1,v0,0x8002c6c8`, im Block `8002c6ac sh a0,64(s1) / 8002c6b0 sh a1,68(s1)` (a0/a1 = `lhu 52/60`) = Spiegel = Lage. Stimmt
   mit `o.zaehler == 0xd` in zellschub_r36.c.

**RE-Gate: haelt.** Jede verhaltensrelevante Konstante der Nachbesserung (18, 400, 0x321, Maske 2, Bit 0x8, Bit 0x80,
Zaehler 9/10/8, 0xd, -1530/1530) traegt ihre Adresse und stimmt mit der zitierten Instruktion; der Spiegel-Startwert ist als
PORT-WAHL gekennzeichnet und begruendet (Installer schreibt +64/+68 nicht — oben selbst gesehen). Bestehende, NICHT in
diesem Diff entstandene Abweichung der Schubkette (vom Bau-Agenten unter OFFEN benannt): Zaehler obj+0x8C haelt im Port
bei 255 (`aot_common.c` `if (s_prop_push_count[p] < 255)`), im Original laeuft das Byte ueber (`8002bf60-70`).

### 2.1 Eigene Nachpruefung der PORT-WAHL "Spiegel-Startwert" (Dossier OFFEN) — Hinweis H1

Lagecode-Rechnung selbst nachvollzogen (`dis 0x8003da78 152`, `8003db1c-db38`): Bit0 = Vorzeichen von (t0 XOR (obj.x -
Spiegel.x)), Bit1 = Vorzeichen von (a3 XOR (obj.z - Spiegel.z)). Fuer den 2060-Tritt (t0 = -18, a3 = -618) ist Code 0
"bewegt" (Ruecksetzung auf den Altspeicher @0x8003dc98-cb4) genau dann erreicht, wenn obj.x - Spiegel.x < 0 UND obj.z -
Spiegel.z < 0, also wenn der Altspeicher OESTLICH UND SUEDLICH (+x, +z) der Lage liegt (dx = 0 ergibt Bit0 = 1, nicht 0).
Das Dossier (P1 "Rechnung fuer den 2060-Tritt" und OFFEN) schreibt "westlich-noerdlich" und "gleichsinnig/0" — die
Himmelsrichtung ist vertauscht (Dokumentationsfehler, keine Code-Folge: der Port setzt den Spiegel = Lage).
Folge im Spiel selbst nachgeprueft (Original-Pool-Index = Obj-Nummer, Installer `8004093c lbu a3,1(a2)` x 0x94): die beiden
Tueren nach ROOM2060 kommen aus ROOM2030 slot 3 @0x01B4C und ROOM2040 slot 1 @0x012BC (`stage2_tueren.txt`). ROOM2030 obj 2
= `Obj_model_set` @0x01D9C/@0x01DEA (-10675, y, 4050) -> Spiegel westlich und suedlich -> Code 1 -> x += -18 = -3143.
ROOM2040 obj 2 = Tritt @0x0139C (22925,-5400,-5400); die 2040-Tuer nach 2060 (floor 4, x -13300..-12300 z 1800..2800)
verlangt, dass dieser Tritt vorher an die Tuer geschoben wurde (x ~ -12800 = westlich) -> Code 1 oder 3 -> -3143; selbst
ungeschoben (oestlich, noerdlich) -> Code 2 mit |a3| 618 + 400 >= 801 -> kleinerer Betrag -> -3143. Fuer die Objekte, die auf diesen
beiden echten Wegen den Pool-Platz 2 belegen, ergibt der Altspeicher des Originals also dieselbe Lage wie die PORT-WAHL.
Kein Mangel (nur die vertauschte Richtung im Dossier).

## 3. Vertrag

* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine belegt (Diff-Suche nach game_flag_set/scd_event_fire/
  re15_msg_install_text: keine Treffer im Spielcode). Gelesen/gesetzt nur ueber Messhaken: (9,71) (RE15_SET_FLAG, 10F0-Szene
  gesehen) und (3,114) (RE15_SET_FLAG im Lauf weg2020) — vorhandene Flags.
* Pfade (`git diff 99398dc7 --name-only`): `analysis/befunde_runde36/R_*` (+ `R_belege/`, darunter drei Spielstaende
  room2050/2060/3050_orig.sav je ~1,5 MB), `engine/src/{aot_common,dachkiste_r36,enemy_ai_common,game_step_common,
  re15_collision,scd_room_setup,zellschub_r36}.c`, `include/{re15_collision,re15_dachkiste_r36,re15_zellschub_r36}.h`,
  `platform/pc/src/input_pc.c`, `tests/integration/test_r36_dachkiste.cmake`, `tests/unit/{probe_r36_dachkiste,
  test_1090_npc_prop_push,test_r36_dachkiste_zellschub}.c`, `tests/unit/probes/r36_dachkiste.cmake`. KEINE release/,
  platform/android/, shared_assets/PSX/, tests/*/CMakeLists.txt, RELEASE_NOTES (grep leer).
* Gemeinsame Dateien: scd_room_setup.c +2 (Installer-Zeile am Blockende "Runde 36 Spur R"), game_step_common.c +3,
  enemy_ai_common.c 1 Zeile, input_pc.c 2 Zeilen — kleine Haken. `aot_common.c` (+44/-) und `re15_collision.c` (+48/-)
  sind groesser, stehen aber nicht in der Liste der gemeinsamen Dateien (VERTRAG 1.4) und sind die Original-Funktionen
  selbst (Verzweigungen von FUN_8002bd44 bzw. FUN_8002cabc), die neue Logik liegt in `zellschub_r36.c`. Hinweis H2 fuer
  die Integration: Konfliktflaeche mit anderen Spuren, die re15_prop_push_tick/push_out_of beruehren.
* Messkopien unter eigenem Namen (`re15_pc_r36r_haken_<id>.exe` im Riegel), kein taskkill /IM, RE15_WINDOW_SCALE=1.
* Tests je Punkt vorhanden und messend: `unit_r36_dachkiste_zensus` (Punkte 1-4, Sonde), `unit_r36_dachkiste_zellschub`
  (P1/P2/P4 der Abnahme 2), `integration_r36_dachkiste` (elf Laeufe echte exe; leon/elza/west = Punkte 1/3, fall3050/
  fall2050/echt2050/elza2051/weg2020/weg2040/treppe2040/tritt2060 = Punkt 4).
* Eigener Zensus `test_r36_dachkiste_zellschub.exe diag` (Scratchpad `abn3/zellschub_diag.txt`): **`ZENSUS: 206 Raeume, 513
  aktive Objekte, 2 vom Ruhe-Zweig im ersten Bild bewegt`** = `BEWEGT ROOM2060 p=2 obj=2 ... (-3125,2075) -> (-3143,2075)` und
  `BEWEGT ROOM2061 ...` — reproduziert die Zahl des Bau-Agenten. (34 RDTs ueberspringt die Sonde mit `SKIP: RDT-Parse`,
  u. a. ROOM20D1/20E0/20F0/30F0/40C0-40F1/5150-5171/6050-6071 — dieselben wie im Lauf des Bau-Agenten.)

## 4. Eigene exe-Laeufe (Messkopie `re15_pc_r36abn3.exe`, Startskript `abn3/lauf_abn3.sh` = Grundumgebung des Riegels,
Arbeitsordner im Scratchpad `abn3/laeufe/<lauf>/`, Wegplan-Treiber RE15_WEGPLAN)

### M1 — NEU: Elza, ROOM2051, Fall der Kiste obj 6 und WIEDERBETRETEN (vom Bau-Agenten nur mit Leon gemessen)
Befehl: `bash lauf_abn3.sh m1_elza2051_wiederkehr wp_elza2051_wiederkehr.txt RE15_PSELECT_AUTO_SWITCH=1
RE15_DEBUG_JUMP=2000@250 RE15_PLAYER_POS=-26100,8500,3072,1 RE15_EXIT_AT=5400`; Wegplan = Riegel-Plan echt2050 mit den
Raeumen 2001/2051 + Rueckkehr (2051 slot 0 -> 2001 -> slot 4 -> 2051) + DANACH ein Schub nach Sueden an der
zurueckgesetzten Kiste (Funktionsprobe: die Stufe ist nicht nur sichtbar, sondern wieder schiebbar).
* debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `loaded room2001.rdt` -> `[aot] DOOR FIRE slot=4
  rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)` -> `loaded room2051.rdt` -> `[aot] DOOR FIRE slot=0
  rect=(9100,5700,hw=1000,hh=1000) target_cut=10 spawn=(-26450,-1800,10250)` -> `loaded room2001.rdt` -> `DOOR FIRE slot=4 ...`
  -> `loaded room2051.rdt`.
* wegplan.txt (erster Besuch): `ZELLE r0[50] LAUFZEIT: ... u0 0x00 ... fl 0x33`, `F487 ... spieler=(17,-6335) ... Befehl 17
  beendet: angekommen` (Y-Test: durch die Band-0-Kiste, keine Klemme bei 3402), Sued `obj.z = -7004`, Ost `obj.x = 7556`,
  `F1912 ... SCHIEB: Objekt-y -5400 -> -5392 (Fall)`; objfall.txt `F1910 room=2051 p=0 AUSLOESER wort=0x3202 band=3
  pos=(7556,-5400,-11716)`, `F1911 zust 0->1`, `F1923 BODEN FUN_8001c6e8=-3600 band 3 -> 2`, `F1927 LANDUNG y=-3600 band=2
  pos=(7556,-13250)`; push.txt `F1924 ... RUHE-AUSSCHIEBUNG lage=(7556,-13250) -> (7556,-13068) band=2` (Ruhe-Zweig wie im
  Original orig_2050_fall); Ruhelage vor dem Verlassen `obj0=(7556,-3600,-13068) b2` (F2302..F2520).
* **Wiederbetreten: `F126 room=2051 spieler=(9100,4100) ... obj0=(-1600,-5400,-9348) b3 | RAUM 2051 geladen`** = Record
  main00 @0x01D0E (ROOM2051 byteidentisch); danach derselbe Weg zur Kiste (wieder `(17,-6335)` angekommen) und **`F759 ...
  SCHIEB: Bedingung erfuellt (obj.z = -7996)`** — die zurueckgesetzte Kiste nimmt Schuebe an (push.txt gesamt 931x `room=2051
  p=0 obj=6 SCHUB-OK`, 0x GESPERRT). objfall.txt am Ende `p0 (-1600,-5400,-7996) b3`.
* Ergebnis: **fuer Elza gilt dasselbe wie fuer Leon — nach dem Fall der 2051-Kiste genuegt es, den Raum zu verlassen und
  wieder zu betreten; die Stufe zur Tuer slot 2 steht wieder auf ihrem Platz und ist schiebbar.** Die Dossier-Luecke
  "Wiederbetreten nur mit Leon gemessen" ist damit an der exe geschlossen.

### M2 — NEU: ROOM3050 "Stufe 2" an der exe — der Weg, an dem laut Dossier der Fortschritt (Super Redhawk) haengt
Dossier 4.2 / "Fuer den Nutzer": "nur vom Dach der gefallenen Kiste erreicht man die Super Redhawk (Item slot 6, Floor 1)
... Port = Original"; OFFEN: "ROOM3050 Stufe 2 (Kerbe slot 7 @0x01F06) nur Record-Beleg ... der Abstieg auf Band 0
(Ereignis-Leiter sub00 Aot slot 2..5 @0x02044-2080 unter (3,82)) ist hier nicht gefahren". Gefahren:
Befehl `bash lauf_abn3.sh m2_3050_stufe2 wp_3050_stufe2.txt RE15_DEBUG_JUMP=3050@250 RE15_PLAYER_POS=-3596,1523,1024,4
RE15_MSG_LOG=1 RE15_EXIT_AT=6000` (Wegplan: West-Schub wie fall3050, Nordseite, Sued-Schub bis zum Fall; zurueck zum Lift
(Aot slot 4 sce 3 floor 4, Rechteck x -3300..-500 z -9900..-6400 -> sub04), AKTION; unten nach Westen/Norden zur
gefallenen Kiste, Schub nach Norden in die Kerbe slot 7, Aufstieg, Item).
* Fall wie im Riegel: objfall.txt `F1231 room=3050 p=1 AUSLOESER wort=0x4002 band=4 pos=(-24058,-7200,-18488)`, `F1244 BODEN
  FUN_8001c6e8=0 band 4 -> 0`, `F1253 LANDUNG y=0 band=0 pos=(-24058,-15631)`, Ruhelage `obj1=(-24058,0,-15591) b0`; 432x
  `room=3050 ... obj=1 SCHUB-OK`.
* Lift: `F2200 ... pm=1` (sub04 laeuft), `F2601 FLAG (3,82) = 1` (sub04 @0x023EC `Set (3,82)=1` erreicht). Der Spieler
  steht danach auf y 0 — **aber im Kollisionsband 4**: climb.txt `T2591 room=3050 ... pos=(-2038,0,-8179) ... band=4`
  (bis zum Laufende T5193 `pos=(-17842,0,-22782) ... band=4`). Unten nach Westen: `F2825 GEHE: nach 8 Runden Rest 344` bei
  x -7657 = -8125 + 450 + 18 = Ostkante der BAND-4-Wand x -10350..-8125 (`sca_dump.py ROOM3050.RDT 4`), dann `F3228 KLEMME
  beim Gehen nach (-20000,-14200)` — der Spieler laeuft auf y 0 gegen die Waende der OBEREN Ebene und erreicht die Kiste
  auf Band 0 nicht; ein Schub waere ohnehin am Band-Test @0x8002bf38 (Spielerband 4 != Kistenband 0) gescheitert.
  Original (selbst disassembliert): sub04 @0x02390 `Work_set 2e 01 00` (Spieler), @0x023D0 `Member_set 34 12 00 00` ->
  FUN_8004116c (`dis 0x8004116c 24`, Tabelle 0x80010c8c[18] -> 0x80041224) **`80041228 sb a2,130(a0)`** = Spieler+0x82 =
  0x800aca54+0x82 = **DAT_800acad6**, genau das Byte, das Schubkette (@0x8002bf38 `lbu v1,846(s6)`) und AOT-Scan
  (@0x80042cc0 `lbu v1,130(s1)`) als Spielerband lesen. Port: `re15_actor_set_member` case 18 schreibt nur
  `g_actors[..].floor` (actor_common.c), das Kollisionsband `s_coll_band` (re15_collision.c, `re15_collision_debug_band`)
  bleibt stehen — kein Aufrufer von `re15_collision_set_band` reagiert auf Member_set. **Port != Original: der Lift von
  ROOM3050 bringt den Spieler nicht auf Band 0; der vorgesehene Weg zur Super Redhawk (Fall -> Lift -> Kerbe -> Aufstieg)
  ist im Port nicht gangbar.** (Nicht im Diff dieser Spur entstanden; gefunden, weil Punkt 4 genau diesen Weg betrifft.)
* Am Ende des Laufs (Spieler auf y 0 bei (-24070,-22782), Blick 1037 = Nord, AKTION x3): **`F5418 FLAG (9,92) = 1`** —
  das Taken-Bit der Super Redhawk (Item_aot_set slot 6 @0x01EC6, pc[18] = 0x5c = 92, Bank 9) ist gesetzt, obwohl die Kiste
  NICHT in der Kerbe liegt (`F5419 FLAG (3,93) = 0`, Kiste weiter `(-24058,0,-15591) b0`) und der Spieler auf keiner
  Kiste steht. Ursache s. M3.

### M3 — NEU: ROOM3050 Super Redhawk OHNE Kiste von der OBEREN Ebene (Band 4) aufnehmbar
Befehl `bash lauf_abn3.sh m3_3050_oben_item wp_3050_oben_item.txt RE15_DEBUG_JUMP=3050@250
RE15_PLAYER_POS=-3596,1523,1024,4 RE15_EXIT_AT=1500` (Wegplan: Nordgang der oberen Ebene bis (-24070,-22782), Blick 1024,
AKTION x3; die Kiste wird NICHT beruehrt).
* wegplan.txt: `F61 FLAG (9,92) = 0`, `F776 ... spieler=(-24026,-22770) ... obj1=(-18000,-7200,-20800) b4 | ... angekommen`,
  climb.txt `T1058 room=3050 pm=0 pad=8000 pos=(-24026,-7200,-22770) rot=1072 band=4` (Spieler auf y -7200, Band 4, Square),
  **`F1036 FLAG (9,92) = 1`**, `F1037 FLAG (3,93) = 0`, Kiste unberuehrt auf dem Record `(-18000,-7200,-20800) b4`.
* Original (selbst disassembliert, `dis 0x80042bac 80`): der AOT-Scan FUN_80042bac prueft fuer JEDEN Eintrag vor der
  Geometrie `80042cac lbu v0,2(s0)` (Band-Byte des Records = Item_aot_set pc[4] = floor 1) / `80042cb4 andi v0,v0,0x80` /
  `80042cb8 bne v0,zero,0x80042cd4` / **`80042cc0 lbu v1,130(s1)` (Spielerband +0x82) / `80042cc4 lbu v0,2(s0)` / `80042ccc
  bne v1,v0,0x8004301c`** (naechster Eintrag) — der Typ-Dispatch (Item sce 9 usw.) kommt erst danach. Ein Spieler auf Band 4
  (oder 0) kann ein Floor-1-Item nicht aufnehmen; die vorgesehene Band-1-Flaeche an dieser Stelle ist das Dach der Kiste in der
  Kerbe (Record @0x01F40 (-24000,0,-22200) unter (3,93)=1). Port (aot_common.c, AOT-Scan): das Band-Gate
  (`if (pb >= 0 && !(a->band & 0x80) && (int)a->band != pb)`) gilt nur fuer GENERIC/AUTO_EVENT/MESSAGE/EXAMINE_WORKVAR/
  FLAG_CHG, Tueren haben ihr eigenes; **ITEM ist nicht band-gegatet** (`fire = gen_reach && g_aot_action_pressed && ...`).
  **Port != Original: im Port haengt die Super Redhawk NICHT an der Kiste** — sie liegt fuer jeden Spieler auf der oberen
  Ebene direkt greifbar. Die Dossier-Antwort zu Punkt 4 ("nur vom Dach der gefallenen Kiste erreicht man die Super Redhawk
  ... Port = Original") stimmt fuer das Original, nicht fuer den Port.

## 5. Gezielte Regression + Riegel (selbst gefahren, Spurbaum, keine volle Suite)

Befehl: `ctest --test-dir re15_port/build -R "r36_dachkiste|r36_objfall|r36_klettern|klettern|objfall|1090|10f0|2050|2060|3050|
sca_collision|collision|push|prop_|tritt|kiste|kerbe|unit_r36_|r35_|r36_szenenleon|unit_npc_|unit_stair_|r16_objekte"
--timeout 900 -j 4 --output-on-failure` (= Regex des Bau-Agenten + die Gruppen, die der Diff ZUSAETZLICH beruehrt:
`integration_r36_szenenleon` (Szenen-Schub 0x200 von Spur J laeuft jetzt in den Ruhe-Zweig), `unit_npc_*` (Y-Test in
`re15_collision_objects_actor`), `unit_stair_*`, `probe_r16_objekte_paket`; 234 Tests, davon 30 Integrationslaeufe).
Protokoll `abn3/regression.log` (Scratchpad). Waehrend der Suite liefen meine eigenen exe-Laeufe M1-M3 nacheinander
(Last); kein Fenster-Haken wurde rot.

Riegel `integration_r36_dachkiste` (elf Laeufe, Laufordner `re15_port/build/r36_dachkiste_wd/lauf_dd108a11/`, jede Datei
selbst gelesen):
* `leon` (PL00): `loaded room10f0.rdt` -> 367x SCHUB-OK am 10F0-Tritt, `F759 room=10F0 p=3 KERBE m0b=10 pos=(-4785,0,216)`,
  `T1121 ... sub=9 kind=0x81 obj=3` (Aufstieg) -> `[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500) target_cut=0
  spawn=(-13600,-9000,1300)` -> `loaded room1090.rdt`; **`F1611/F1651/F1711 room=1090 p=1 obj=1 SCHUB-GESPERRT
  spieler=(-6410|-6417,-14206) ... lage=(-5960,-15482) ziel=(-5960,-15556) band=5`**, je `TREFFER-ZELLE r3[4]: x -7256..4624
  z -18560..-16403 typ 1 u0 0xff ... r=900`; 0x SCHUB-OK an obj 1, 0x RUHE-AUSSCHIEBUNG an obj 1, 0x AUSLOESER, letzte Lage
  `p1 (-5960,-9000,-15482) b5`.
* `elza` (`Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `loaded room10f1.rdt` -> `room1091.rdt`): bildgleich F1611/F1651/
  F1711 GESPERRT an r3[4], 0 OK, 0 RUHE, 0 AUSLOESER, `p1 (-5960,-9000,-15482) b5`.
* `west`: `F69/F144 ... SCHUB-GESPERRT ... ziel=(-6034,-15482)` + `TREFFER-ZELLE r3[2]: x -9200..-6878 z -18524..265`, 0 OK,
  0 AUSLOESER, `p1 (-5960,-9000,-15482) b5`.
* `fall3050`: 425x SCHUB-OK, 1x GESPERRT (Westwand), 0 RUHE, `F1383 ... AUSLOESER wort=0x4002 band=4 pos=(-24058,-7200,-18488)`,
  `F1396 BODEN ... band 4 -> 0`, `F1405 LANDUNG y=0 band=0`, Ende `p1 (-24058,0,-15561) b0`.
* `weg2040`: `loaded room2070.rdt` -> `DOOR FIRE slot=5 rect=(5500,-19200,hw=500,hh=1000) target_cut=12 spawn=(-19750,-9000,-19850)`
  -> `loaded room2040.rdt`. `treppe2040`: `[stair] START dir=DOWN cur_band=5 target_band=3`, `T951 room=2040 ...
  pos=(-1359,-5400,-10068) ... band=3`.
* `fall2050`: `ZELLE r0[50] LAUFZEIT: ... u0 0x00 ... fl 0x33`, Sued `obj.z = -7037`, Ost `obj.x = 7553`, 797x SCHUB-OK, 0x
  GESPERRT, `F1289 ... AUSLOESER wort=0x3202 band=3 pos=(7553,-5400,-11706)`, `F1302 BODEN ... band 3 -> 2`, `F1306 LANDUNG
  y=-3600 band=2 pos=(7553,-13250)`, Ende `p0 (7553,-3600,-13068) b2` (= Original-Ruhelage z -13068, orig_2050_fall).
* `weg2020`: `ZELLE r0[15] ... u0 0xff ... fl 0x13` -> `[msg] room=2020 id=0`, `id=1` -> `Cut_chg(3)` F422 / `(4)` F550 / `(3)` F620
  -> `ZELLE r0[15] ... u0 0x00 u1 0x00 fl 0x10` -> `[msg] room=2020 id=2` -> `DOOR FIRE slot=1 rect=(-14200,3300,hw=1000,hh=500)
  target_cut=13 spawn=(-450,-16200,-9700)` -> `loaded room2030.rdt`.
* `tritt2060`: **`F1 room=2060 p=2 obj=2 RUHE-AUSSCHIEBUNG lage=(-3125,2075) -> (-3143,2075) band=0 y=0` + `ZELLE [9]: x -2400..3150
  z 2150..5350 r=675`** (Original room2060_orig.sav Pool 2 (-3143,0,2075)), `FLAG (3,172) = 0`, Nord `obj.z = -7402`, Ost
  `obj.x = 337`, `FLAG (3,172) = 1`, 657x SCHUB-OK, 0x GESPERRT, `DOOR FIRE slot=1 rect=(450,-8350,hw=650,hh=500) target_cut=15
  spawn=(-12850,-5400,1600)` -> `loaded room2040.rdt`.
* `echt2050`: `loaded room2000.rdt` -> `DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)` ->
  `loaded room2050.rdt`; `F487 ... spieler=(17,-6335) ... Befehl 17 beendet: angekommen` (Y-Test), Sued `-7036`, Ost `7553`,
  812x SCHUB-OK, 0x GESPERRT, `F1874 ... AUSLOESER wort=0x3202 band=3 pos=(7553,-5400,-11705)`, `F1891 LANDUNG y=-3600 band=2`;
  zurueck `DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000) target_cut=10 spawn=(-26450,-1800,10250)` -> `loaded room2000.rdt`
  -> `DOOR FIRE slot=4 ...` -> `loaded room2050.rdt` (2x geladen), `F126 ... obj0=(-1600,-5400,-9348) b3 | RAUM 2050 geladen`,
  letzte Lage **`F5400 room=2050 ... | p0 (-1600,-5400,-9348) b3`**.
* `elza2051`: `Spieler-Familie PL04`, `loaded room2001.rdt` -> slot 4 -> `loaded room2051.rdt`, `(17,-6335)` angekommen, Sued `-7004`,
  Ost `7556`, 851x SCHUB-OK, 0x GESPERRT, `F1910 ... AUSLOESER wort=0x3202 band=3 pos=(7556,-5400,-11716)`, `F1927 LANDUNG
  y=-3600 band=2 pos=(7556,-13250)`.

**Ergebnis:** `234/234 Test #573: integration_r36_dachkiste ................   Passed  1437.34 sec` /
**`100% tests passed, 0 tests failed out of 234`** / `Total Test time (real) = 2098.80 sec`; darin u. a.
`integration_r36_klettern` Passed 534.11 s, `integration_r36_szenenleon` Passed 162.24 s, `integration_r35_cut1150` Passed
515.63 s, `integration_r35_cut10f0` Passed 241.53 s, alle `unit_r36_*`, `unit_npc_*`, `unit_stair_*`, `unit_1090_npc_prop_push`
gruen. Keine Regression in den beruehrten Modulen; kein Fenster-Haken musste nachgefahren werden.

## 6. Eigene Original-Messungen (DuckStation-GDB, MZD-Disc, Auslieferungs-EXE; Helfer Gdb/ini_set/pad_bind_alle/pids aus
`R_belege/orig_1090_push.py`, Ablauf eigen; Skripte und Logs im Scratchpad `abn3/orig/`; jeweils kein exe-Lauf parallel)

### O1 — ROOM2050: Wiederbetreten mit per GDB in die Fall-Ruhelage gesetzter Kiste (unabhaengig vom Schubweg)
`python orig_abn3_wiederkehr.py o2050_wiederkehr_b` (Spielstand `R_belege/room2050_orig.sav`, JUMP 205):
`EXE-Stub @0x80026e4c: 0800e003 (Auslieferung 0800e003)`; `START Pool 6: w0 00000101 pos (-1600,-5400,-9348) ... band=3`;
Spinnen ueber Aktiv-Bit + g_active_count (2 -> 0) abgeschaltet; **`GESETZT (GDB, Fall-Ruhelage) Pool 6: ... pos
(7569,-3600,-13068) spiegel (7569,-13068) ... band=2`**, `NACH 2 s RUHE Pool 6: ... pos (7569,-3600,-13068) ... band=2` (der
Ruhe-Zweig haelt sie zwischen den Zellen [22]/[24] bei -13068, wie orig_2050_fall); Blick 3072 + Square -> `TUER 2050 slot 0:
TUER AUSGELOEST - RDT-Kopf 0010090000000000 -> 001003000000002c, Spieler (-26450, -1800, 10250, 0, 1, 0)`; in ROOM2000 Blick
2048 + Square -> `TUER 2000 slot 4: TUER AUSGELOEST - ... Spieler (9100, -5400, 4100, 1024, 3, 0)`; dann je Sekunde:
**`t= 1.0 s RDT +8 = 000097c0 | Pool 6: w0 00000000 pos (7569,-3600,-13068) band=2`** (Raumlader hat Wort 0 genullt, Lage alt)
... **`t= 4.1 s RDT +8 = 80122af0 | Pool 6: w0 00000101 pos (-1600,-5400,-9348) band=3`** -> `ERGEBNIS: Pool 6 nach dem
Wiederbetreten (-1600,-5400,-9348) band=3 - Record main00 @0x01D0E ... -> AUF DEM RECORD`; `settings.ini zurueckgeschrieben
(EnableGDBServer = false: True)`. (Lauf a `o2050_wiederkehr`: in ROOM2000 traf meine Gegner-Abschaltung g_active_count = 0
waehrend des Raumaufbaus, Leon drehte danach nicht mehr — dasselbe Werkzeug-Phaenomen wie orig_2050_diag2_ent des Bau-Agenten;
verworfen, in Lauf b ohne Abschaltung in ROOM2000.) **Bestaetigt unabhaengig: das Original stellt die gefallene 2050-Kiste beim
Wiederbetreten auf den Record zurueck — Port (Riegel echt2050, M1 Elza) = Original.**

### O2 — ROOM3050: Super Redhawk nur mit Spielerband 1 (A/B) und Lift setzt Band 0 (C)
`python orig_abn3_3050_item.py o3050_item` (Spielstand `R_belege/room3050_orig.sav`, JUMP 305; Spieler per GDB auf
(-24026,·,-22770) Blick 1072 = dieselbe Stellung wie im Port-Lauf M3; Taken-Bit = FUN_8004efe4(*(0x80074688) = 0x800b1078,
92) = Wort +8 Maske 0x8 (`dis 0x8004efe4`: `srl v1,a1,5 / sll 2 / lw / lui 0x8000 / srlv`)):
* `EXE-Stub @0x80026e4c: 0800e003`, `Zone-9-Basis *(0x80074688) = 800b1078, Wort +8 = 00000000, Taken-Bit 92 = 0`.
* **A (y 0, Band 0, unter dem Item): 3x Square -> `Taken-Bit 92 = 0`.**
* **A2 (y -7200, Band 4, obere Ebene = Port-Lauf M3): 3x Square -> `Taken-Bit 92 = 0`** (Spieler bleibt `band 4`, Lage stabil).
* **B (y 0, Band 1 = Dach-Hoehe der Kiste): nach Druck 3 `Taken-Bit 92 = 1` (`Wort +8 = 00000008`)** — dieselbe x/z/Blick-
  Stellung, nur das Spielerband (+0x82) anders: das Band-Gate @0x80042cac-0x80042ccc entscheidet.
* **C (Lift): Spieler auf den Lift (-1900,-7200,-8150) Band 4, Blick 0, Square -> y -7200 / -6300 / -4800 / -3300 / -1800 /
  -400 / 0 im Sekundentakt, `t=6 s: ... 'y': 0, ... 'band': 0 ..., (3,82) = 1`** (vorher `(3,82) = 0`; Zone 3 Basis 0x800b0ff8
  Wort +8 Maske 0x2000). Das Original setzt beim Abstieg das Spielerband auf 0 (sub04 @0x023D0 Member_set 0x12 ->
  `80041228 sb a2,130(a0)`).
* `settings.ini zurueckgeschrieben (EnableGDBServer = false: True)`; danach im Hauptbaum geprueft: `EnableGDBServer = false`,
  `ConfirmPowerOff = true`.
**Port gegen Original fuer ROOM3050:** Original — Super Redhawk nur von Band 1 (Kistendach), Lift bringt Band 0; Port —
Super Redhawk von Band 4 ohne Kiste (M3: `F1036 FLAG (9,92) = 1`) und von y 0 mit Band 4 (M2), Lift laesst Band 4 stehen (M2:
`T2591 ... pos=(-2038,0,-8179) ... band=4`). Zwei Port-Abweichungen genau an dem Fortschritt, der laut Dossier "an der Kiste
haengt".

### Hinweis H3 an den Orchestrator (ausserhalb des Wortlauts dieser Spur, beim Lesen von Punkt 4 / ROOM3050 gefunden)

Das Taken-Bit der Original-Items liegt in BANK 9: `op_item_aot_set` (scd_vm.c, "TAKEN-BIT persistence (byte-true
@0x800406d4-0x80040718 ... FUN_8004efe4(DAT_800b1078 = flag zone 9, bit = payload[2] = pc[18]/pc[26])") und
`re15_game_flag_get(9, tk_bit)`. Eigener Scan aller 164 `Item_aot_set` der ausgelieferten RDTs (`scd_dump_room.py`,
Scratchpad `abn3/item_aots.txt`, pc[18] dekodiert): benutzte Taken-Bits u. a. **85 (ROOM3040/3041 Item 54), 86
(ROOM4040/4041 Item 65), 90 (ROOM2090/2091 Item 63), 91 (ROOM2040/2041 Item 50), 92 (ROOM3050/3051 Item 7 = SUPER
REDHAWK)**; die Original-SCD prueft sie auch selbst (`Ck (9,85)` ROOM3040, `Ck (9,86)` ROOM4040, `Ck (9,92)=0` ROOM3050
@0x0223C). VERTRAG.md 1.1 vergibt genau diese Bits: 85 = J-Reserve, 86 = L-Reserve, **90/91 = Spur N "Karte B2/B1
genommen"**, 92 = P-Reserve (die Zensus-Grundlage dort war nur `re15_port/include/*.h`). Folge, falls N die Bits so
belegt: das Aufnehmen der Karte B2/B1 laesst Item 63 in ROOM2090 bzw. Item 50 in ROOM2040 als "genommen" verschwinden
(und umgekehrt). Fuer diese Spur (Punkt 4) ist (9,92) das Taken-Bit der Super Redhawk auf der gefallenen 3050-Kiste —
keine integrierte Spur setzt es (Suche in include/ und engine/src: kein Treffer). Bitte vor der Integration von N pruefen.

Herkunft der beiden 3050-Abweichungen: `git diff 99398dc7 --stat -- re15_port/engine/src/actor_common.c
re15_port/engine/src/scd_vm.c` = leer; `git diff 99398dc7 -- re15_port/engine/src/aot_common.c` beruehrt nur die Includes und
`re15_prop_push_tick` (Hunks @@ -33 / @@ -1723 / @@ -1741). Beide Abweichungen sind also NICHT durch Nachbesserung 3 entstanden
(keine Regression dieser Spur) — sie liegen aber genau auf dem Fortschritt, nach dem Punkt 4 fragt, und widerlegen die
Port-Aussage des Dossiers fuer ROOM3050.

## 7. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 — an der exe messen, ob Leon (und Elza in ROOM1091) die Dach-Kiste bis zum Fall-Ausloeser schieben kann (echter Weg, jede Richtung) | **erfuellt** | Riegel `leon`/`elza` (echter Weg 10F0/10F1 -> Tritt -> Klettern -> Tuer slot 1 -> 1090/1091 -> Korridor) und `west` in MEINEM Lauf `lauf_dd108a11`: F1611/F1651/F1711 bzw. F69/F144 `SCHUB-GESPERRT` an r3[4]/r3[2], 0x SCHUB-OK, 0x RUHE-AUSSCHIEBUNG, 0x AUSLOESER, Kiste `p1 (-5960,-9000,-15482) b5`. Der neue Ruhe-Zweig erreicht die Dach-Kiste nicht (21 bzw. 18 Einheiten Luft, halboffener Test selbst nachgerechnet: 3978 >= 3957, 4140 >= 4122). Dazu die Wegplan-Laeufe der Abnahme 2 (1090nord/1091nord). |
| 2 — am Original messen: bis zur Absprungzelle schiebbar oder vorher geblockt? | **erfuellt** | unveraendert seit Abnahme 0/1/2: orig_lauf5/6 (Haltepunkte @0x8002bfc0/@0x8002c0b0/@0x8002c13c, Treffer 0x80123984 = r3[4]) + unabhaengige Wiederholung NORD in Abnahme 0; Nachbesserung 3 aendert daran nichts. |
| 3 — blockt das Original, blockt der Port an derselben Stelle mit demselben Mechanismus (Beleg @0x, Pin) | **erfuellt** | Kette @0x8002bf14-c0e8 / FUN_8003b558 / LAB_8002c0b8 / FUN_8003da78 selbst disassembliert (Abschnitt 2), Port-Code Zeile fuer Zeile verglichen; dieselbe Zelle, dieselbe Ziel-Lage (-15556 vs. -15558 = Spieler-Schritt), dieselbe Ruecksetzung; Pins `unit_r36_dachkiste_zensus`, `unit_r36_dachkiste_zellschub`, `integration_r36_dachkiste` gruen (234/234). |
| 4 — dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste? | **teilweise** | **2050/2051 erfuellt:** Fall Port = Original (Riegel fall2050/echt2050/elza2051, Ruhelage -13068), Softlock nach dem Fall NEIN — Wiederbetreten setzt die Kiste zurueck (Leon Riegel echt2050 `F5400 ... p0 (-1600,-5400,-9348) b3`; **Elza neu M1** inkl. Schub danach; Original unabhaengig O1), Ersatzweg 2020 -> 2030 -> 2060 (Ruhe-Zweig, Riegel tritt2060) gangbar, Y-Test auf dem echten Weg (`(17,-6335)`). **3050/3051 NICHT erfuellt:** der Fall selbst ist Port = Original (Riegel fall3050, Abnahme 2 O2), aber der Fortschritt, der laut Dossier an der Kiste haengt (Super Redhawk, Floor 1, nur vom Kistendach in der Kerbe), verhaelt sich im Port anders: aufnehmbar OHNE Kiste von der oberen Ebene (M3) und vom Boden (M2) — Original nur mit Band 1 (O2 A/A2/B); der vorgesehene Weg (Lift -> Band 0 -> Kerbe) ist im Port nicht gangbar (M2, Lift laesst Band 4 stehen; Original setzt Band 0, O2 C). Die Dossier-Antwort "nur vom Dach der gefallenen Kiste erreicht man die Super Redhawk ... Port = Original" ist fuer den Port falsch; "Stufe 2" stand nur als Record-Beleg unter OFFEN. |

## 8. Gates

* **Bau:** LOCAL-BUILD-OK (configure + build, ninja: no work to do; exe = Engine-Stand HEAD). ✔
* **RE-Gate:** haelt (Abschnitt 2; sechs Stellen selbst disassembliert, alle Konstanten der Nachbesserung mit Adresse;
  Spiegel-Startwert als PORT-WAHL gekennzeichnet und begruendet; keine Guess-Tells, kein Env-Schalter als Abschluss). ✔
* **Fix erklaert den Befund:** P1 (2060-Tritt unbeweglich) <- fehlender Ruhe-Zweig: Port-Tritt nach Bild 1 bei -3143 wie
  room2060_orig.sav, danach Schub frei (Riegel tritt2060); P2 (Klemme bei x 3402) <- fehlender Y-Test: Spieler kommt bei
  (17,-6335) an (Riegel echt2050, M1). ✔
* **Vertrag:** eingehalten (Abschnitt 3; Hinweis H2 zur Konfliktflaeche). ✔
* **Tests:** vorhanden, messend, gruen (234/234 gezielt, Riegel 1437 s). ✔

## 9. Maengel (nummeriert, nachpruefbar)

1. **Punkt 4 / ROOM3050 — die Super Redhawk haengt im Port NICHT an der Kiste.** Port: `abn3/laeufe/m3_3050_oben_item`
   (Leon Band 4 auf y -7200 bei (-24026,-22770), Blick 1072, Square; Kiste unberuehrt (-18000,-7200,-20800) b4) ->
   `F1036 FLAG (9,92) = 1`. Original (O2): an derselben Stelle Band 4 -> `Taken-Bit 92 = 0`, Band 0 -> 0, nur Band 1 -> 1.
   Mechanismus Original: AOT-Scan FUN_80042bac `80042cac lbu v0,2(s0)` / `80042cb4 andi 0x80` / `80042cc0 lbu v1,130(s1)` /
   `80042ccc bne v1,v0,0x8004301c` fuer JEDEN Eintrag vor dem Typ-Dispatch. Port: aot_common.c (AOT-Scan) wendet das Band-Gate
   nur auf GENERIC/AUTO_EVENT/MESSAGE/EXAMINE_WORKVAR/FLAG_CHG an, ITEM feuert mit `gen_reach && g_aot_action_pressed && ...`
   ohne Band. Nachbesserung: (a) Dossier 4.2 / "Fuer den Nutzer" / Riegel-Text berichtigen (Port != Original fuer den
   3050-Fortschritt, mit Messung); (b) Band-Gate fuer ITEM nach @0x80042cac-ccc bauen — VORHER Zensus der 12 Item_aot_set mit
   floor != 0 (ROOM2030/2031 floor 1 und 6, 2040/2041 floor 3, 2090/2091 floor 1, 3010/3011 floor 1, 3050/3051 floor 1;
   `abn3/item_aots.txt`), damit kein Item im Port unerreichbar wird; Pin: 3050 Band 4 -> keine Aufnahme, Band 1 auf der Kiste ->
   Aufnahme. **(b) nur zusammen mit Mangel 2** — sonst waere die Super Redhawk im Port gar nicht mehr erreichbar (vom Port
   erzeugte Sperre).
2. **Punkt 4 / ROOM3050 — der vorgesehene Weg (Lift -> Band 0 -> Kerbe -> Aufstieg) ist im Port nicht gangbar.** Port:
   `abn3/laeufe/m2_3050_stufe2` — nach sub04 (`F2601 FLAG (3,82) = 1`) climb.txt `T2591 ... pos=(-2038,0,-8179) ... band=4`,
   `F2825 GEHE: nach 8 Runden Rest 344` bei x -7657 (= Band-4-Wand x -10350..-8125 + 450 + 18), `F3228 KLEMME`; die gefallene
   Kiste (Band 0) ist nicht erreichbar/schiebbar. Original (O2 C): `t=6 s: ... 'y': 0, ... 'band': 0 ..., (3,82) = 1`.
   Mechanismus: sub04 @0x02390 `Work_set 2e 01 00` (Spieler) + @0x023D0 `Member_set 34 12 00 00` -> FUN_8004116c Tabelle
   0x80010c8c[18] -> `80041228 sb a2,130(a0)` = Spieler+0x82 = DAT_800acad6 (das Band der Schubkette @0x8002bf38 und des
   AOT-Scans @0x80042cc0). Port: `re15_actor_set_member` case 18 schreibt nur `g_actors[].floor`, `s_coll_band`
   (`re15_collision_debug_band`) bleibt. Nachbesserung: Member-Id 18 am Spieler auf das Kollisionsband wirken lassen
   (`re15_collision_set_band`), Pin (3050-Lift -> Band 0), danach "Stufe 2" Ende-zu-Ende an der exe UND am Original messen
   (Fall -> Lift -> Schub in Kerbe slot 7 @0x01F06 -> `(3,93)=1` (sub01 @0x02184-2196) -> Aufstieg -> Aufnahme (9,92)).
3. **Dossier:** Punkt 4 / "Fuer den Nutzer" / Riegel-Kommentar (`fall3050`: "Super Redhawk haengt am FALL") behaupten fuer
   ROOM3050 "Port = Original" fuer einen Weg, der nur als Record-Beleg vorlag und im Port nachweislich anders verlaeuft
   (Maengel 1/2). Ausserdem P1/OFFEN "Spiegel-Startwert": Himmelsrichtung vertauscht (Code 0 "bewegt" = Altspeicher
   OESTLICH-SUEDLICH, nicht westlich-noerdlich; dx = 0 ergibt Bit0 = 1) — s. Abschnitt 2.1 (ohne Code-Folge).

## 10. Hinweise (keine Maengel dieser Spur)

* **H1** Spiegel-Startwert-PORT-WAHL: auf den beiden echten Wegen nach ROOM2060 (aus 2030 slot 3, aus 2040 slot 1) liefert
  auch der Altspeicher des Originals -3143 (Abschnitt 2.1) — die PORT-WAHL hat dort keine Folge.
* **H2** aot_common.c `re15_prop_push_tick` (+44/-) und re15_collision.c `push_out_of`/`re15_collision_push_prop` (+48/-) sind
  Konfliktflaeche fuer andere Spuren, die diese Funktionen beruehren (Integration).
* **H3 an den Orchestrator — Bank-9-Vertrag:** Original-Items benutzen Bank 9 als Taken-Bits (s. Abschnitt "Hinweis H3 an den Orchestrator" vor Abschnitt 7,
  `abn3/item_aots.txt`): 85 (ROOM3040/3041), 86 (ROOM4040/4041), **90 (ROOM2090/2091), 91 (ROOM2040/2041)**, 92 (ROOM3050/3051,
  Super Redhawk). VERTRAG 1.1 vergibt 90/91 an Spur N ("Karte B2/B1 genommen"), 85/86/92 als Reserven. Vor der Integration
  von N pruefen.
* **H4** Der fehlende Band-Gate fuer ITEM betrifft alle 12 Items mit floor != 0 (Liste in Mangel 1) — im Port vermutlich
  ueberall vom falschen Stockwerk aufnehmbar; nur 3050 gemessen. Fuer NACHLAUF, falls nicht in dieser Spur behoben.
* **H5** (vom Bau-Agenten benannt, bestaetigt) Zaehler obj+0x8C: Original Byte mit Ueberlauf (`8002bf60 lbu / bf68 addiu 1 /
  bf70 sb`), Port haelt bei 255 — fuer die 1090-Kiste ohne Folge (ein zweiter Versuch nach dem Ueberlauf trifft dieselbe Zelle).

## 11. Gesamt

**Softlock-Frage ROOM1090/1091 (Kern): NEIN, Port = Original** — an der exe (Riegel leon/elza/west, eigener Lauf) erneut
bestaetigt; Punkte 1-3 erfuellt. **ROOM2050/2051: Softlock nach dem Fall NEIN** — Wiederbetreten setzt die Kiste zurueck (Leon
Riegel, Elza M1 NEU, Original O1 NEU und unabhaengig), Ersatzweg ueber 2060 gangbar; Nachbesserung 3 (Ruhe-Zweig, Y-Test,
r = hz) ist RE-belegt, erklaert die Befunde der Abnahme 2 und ist ohne Regression (234/234). **ROOM3050/3051:** der Fall ist
Port = Original, aber der Fortschritt, der an der Kiste haengt (Super Redhawk), ist im Port ohne Kiste von oben aufnehmbar und
sein vorgesehener Weg (Lift auf Band 0) im Port gebrochen — beides am Original gegengemessen (O2). Die Dossier-Antwort zu
Punkt 4 ist fuer 3050 falsch -> **Punkt 4 teilweise -> bestanden = false** (Maengel 1-3).

Messpfade: Riegel `re15_port/build/r36_dachkiste_wd/lauf_dd108a11/{leon,elza,west,fall3050,weg2040,treppe2040,fall2050,
weg2020,tritt2060,echt2050,elza2051}/`; eigene exe-Laeufe Scratchpad `abn3/laeufe/{m1_elza2051_wiederkehr,m2_3050_stufe2,
m3_3050_oben_item}/` (debug.log, climb.txt, objfall.txt, push.txt, wegplan.txt, Wegplaene `abn3/wp_*.txt`); Original
`abn3/orig/{o2050_wiederkehr_b,o3050_item}/log.txt` (Skripte `abn3/orig/orig_abn3_wiederkehr.py`, `orig_abn3_3050_item.py`);
Regression `abn3/regression.log`; Zensus `abn3/zellschub_diag.txt`, `abn3/item_aots.txt`. DuckStation `settings.ini` nach
jedem Lauf aus der Sicherung zurueck (EnableGDBServer = false, ConfirmPowerOff = true; geprueft).
