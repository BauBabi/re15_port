# Runde 38 Spur I — heliport1171 (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_heliport1171` (Zweig `r38/heliport1171`, Basis e2cc7673).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Elza stuck in ihrer Cutscene im ROOM 1170.

Lesart (AUFTRAG.md "Lesarten"): Elza-Raum = ROOM1171 (Elza-RDT = Leon-RDT + 1, PSX.EXE @0x800397e4/@0x800397ec).

## Stand

- 2026-10-16: Dossier angelegt, Pflichtlektuere (AUFTRAG/VERTRAG/UEBERGABE Runde 38) gelesen.
- 2026-10-16: Haenger REPRODUZIERT (beide KI-Modi), Ursache am Original belegt (Abschnitt 1/2). Keine Regression:
  der Mechanismus ist seit 74121bb8 (2026-07-06, FLIGHT-2-Port) unveraendert in v0.8.23/24/25.

## 1. Messung vorher

Gebauter Stand e2cc7673 (eigener Baum, `local_build.sh configure/build`), Messkopie `re15_pc_i38.exe`.
Lade-Weg: Elza-Spielstand (probe_r36_elza_karte) in ROOM1171 mit (3,0xC2)=1 (im Original gesetzt von ROOM1141
sub02 @0x00C20 `22 03 c2 01`), CONTINUE -> sub00 Switch(work_vars[0x0A]) Fall 0 -> sub07 @0x016C0 ->
Ck(3,0xC2)==1 -> Ereigniszweig (John 0x49 + ZWEI Kraehen 0x21 beh 0x40 @0x016DC/@0x016F0) -> Evt_exec sub02.

| Lauf | KI | Ergebnis |
|---|---|---|
| v1_re2 (RE15_SCD_TRACE) | Standard (RE2) | sub02 laeuft bis @0x013F4 `While Ck(5,0x1C)==0` (erreicht F88) und dreht dort bis F1500 (Laufende) |
| v2_re2 (STATE_LOG) | Standard (RE2) | beide Ereignis-Kraehen (g=40) im RE2-Hirn Zustand 1/Sub 0, Lage 900 Bilder unveraendert (-9970,-5680)/(-8644,-7294), Abstand 17132/17616 — sie kommen nie; Elza pm=2, pad=0000, steht |
| v2_re15 | RE15_AI_FLAVOR=re15 | Kraehen im RE1.5-FLIGHT-2 (Zustand 4): ASCEND -> HOVER -> Kontakt -> ORIENT (Kraehe 2 ab F95), Elza hp 100->92, gr=1 — trotzdem KEIN Fortgang |
| v3_re15_force | re15 + RE15_SET_FLAG_AT=5:28@150 (nur Diagnose) | Szene laeuft durch: msg 0/1/3, sub03/sub04 @F902, `Aot_on 01` -> DOOR FIRE Slot 1 -> ROOM10B1; aber Kraehen bleiben in ORIENT (kein SPIN, kein Tod), Elza ohne Opfer-Animation |
| v4_weg_vorher (ECHTER WEG, build_bisect = e2cc7673 ohne Tests) | Standard (RE2) | Elza-Spielstand ROOM1131 vor der Flurtuer Slot 3 (@0x008EE), Aktionstaste -> ROOM1171 Cut 8 (Szenario 8, kein Ereignis) -> Selbst-Tuer Slot 5 (@0x012F0, Cut 0) -> sub02 F61 -> "RE2 EM021 loaded" -> bis F1500 keine Zeile mehr (kein Text, keine Tuer) |

=> Der Haenger ist allein die Warteschleife @0x013F4 auf (5,0x1C). Spieler-Eingabe ist waehrend der Szene zu
(pm=2 durch (2,0x07)=1 @0x013A8 und (1,0x1B)=1 @0x013AC) — Elza steht fest, das ist der Nutzer-Befund.

Regression? Nein. `s_crow_gflags |= 0x8u` (privates Wort) steht seit 74121bb8 (2026-07-06) unveraendert in
v0.8.23 3ec5b1f8 (Z. 6585), v0.8.24 e8b3958f (Z. 6598), v0.8.25 2077bb8e (Z. 6603); die RE2-Weiche fuer 0x21
ebenso in allen drei. Die Szene konnte im Port nie durchlaufen.

## 2. RE-Belege

### 2.1 Wer setzt (5,0x1C)? — nur die Ereignis-Kraehe beim Rueckgriff

- SCD `Ck`/`Set` (Handler @0x8003fcf4 / @0x8003fdd0, Sprungtabelle @0x8007452c/@0x80074530) loesen die Bank ueber
  die Zeigertabelle @0x80074664 auf: `lui at,0x8007; addiu at,at,0x4664; addu at,at,a2; lw v0,0(at)`
  (@0x8003fd20-2c). Tabelle [5] = 0x800b1028 (@0x80074678). => (5,0x1C) = Wort 0x800b1028, Maske
  0x80000000>>28 = 0x8. Port: g_game.flags[5][0], Maske 0x8 (game_state.c, MSB-first).
- Bit-Helfer PSX.EXE: SET 0x8004ef90 (`srl v1,a1,5 .. or v0,v0,a0; jr ra; sw v0,0(v1)`), CLR 0x8004efb8,
  TEST 0x8004efe4.
- Kraehe FLIGHT-2 HOVER 0x80115130 (STAGE1.BIN): Kontakt `lbu v0,464(a0)` (+0x1d0) @0x80115254; wenn
  `lbu v1,0x800aca58 != 5` (@0x80115268-70): acbfc=Kraehe @0x8011527c, aca5a=0 @0x80115288,
  acbcc=Kraehe+0x178 @0x80115290, **aca58=5 @0x801152a0, aca59=1 @0x801152ac**, acbd0=Kraehe+0x17c @0x801152b4;
  dann (immer) `jal 0x80115d74(2)` @0x801152b8 (-> ORIENT) und
  **`addiu a0,s0,17824` @0x801152c0 (s0 = 0x800aca88 -> a0 = 0x800b1028), `jal 0x8004ef90` @0x801152c4,
  `ori a1,zero,0x1c` @0x801152c8** = SET (5,0x1C). KEIN HP-Write im ganzen Griff.
- Zensus STAGE1.BIN (alle `jal 0x8004ef90/efb8/efe4`): Bank-5-Setzer nur @0x801152c4 (Bit 0x1c) und
  @0x80116ac8 (Bit (grid&0x7f)+29, Feuer-Emitter 0x26, nur ROOM1090). PSX.EXE: 12 direkte Set/Clr-Aufrufe, keiner
  mit Bit 0x1c. SCD-Zensus aller RDTs: (5,0x1C..0x1F) in STAGE1 NUR ROOM1171 sub02 (9 Stellen), sonst nur
  3070/3071, 4030/4031, 5080/5081, 5090/5091, 50E0, 50F1.
- Kraehen-Zensus aller RDTs: Typ 0x21 nur 10C0/10C1, 1120/1121, 1170, 1171; beh&0x40 (Ereignis-Kraehe) NUR
  ROOM1171 sub07 @0x016DC und @0x016F0.

### 2.2 Was die Szene danach mit den Bits macht (Kraehen-Wurzel 0x80112020, FLIGHT-2 0x80114e54)

- Wurzel @0x80112030-4c: TEST(0x800b1028, 0x1f) -> wenn Zustand 4 und (+0x9 & 0x40): `sb 3 -> +0x4` @0x80112088,
  `jal 0x80115d74(0)` @0x8011208c, CLR(0x800b1028,0x1f) @0x80112098 = Tod EINER Ereignis-Kraehe je gesetztem Bit.
- Wurzel @0x801120a0-e8: TEST(0x800b1028,0x1d) && aca58==5 && aca5a<3 -> `sb 3 -> 0x800aca5a` @0x801120e8 =
  Opfer-Phase 3 (Wurf).
- FLIGHT-2 @0x80114f18-6c: `addiu a0,s0,18596` (s0 = 0x800ac784 -> 0x800b1028), TEST(...,0x1d) @0x80114f3c ->
  wenn Sub != 3: `jal 0x80115d74(3)` (SPIN); Sub-Tabelle @0x80121220 = {0x80114fb8 ASCEND, 0x80115130 HOVER,
  0x801152e0 ORIENT, 0x801153ac SPIN, 0x8011553c DIVE-ARM, 0x80115830, 0x80115910}.
- SCD ROOM1171 sub02: Set (5,0x1E)=1 @0x013B2, Warten @0x013F4, Sleep 60, Set (5,0x1D)=1 @0x01404, 6 Schuesse +
  2x Set (5,0x1F)=1 @0x01464/@0x01498 (je eine Kraehe stirbt), Aufraeumen @0x014A0-AC, Plc_dest mode 5 @0x014E2
  (Ende des Liegens), ..., Message_on 3 @0x01586, `Aot_on 01` @0x0158E (Slot 1 -> ROOM10B1).

### 2.3 Opfer-Seite: Hook A [0x21], RUECK-FSM (aca59=1) 0x80115b80

Tabelle @0x8012123c = {0x801159bc vorn, **0x80115b80 hinten**, 0x80115d6c Stub}; Phasen @0x80100304 =
{0x80115bb8, 0x80115c0c, 0x80115c74, 0x80115cb4, 0x80115cdc}.
- P0 @0x80115bb8: acc0c=1 @0x80115bc4, +0x8f=7 @0x80115bd0, Clip 0 @0x80115bec, Bild 0 @0x80115bf4, aca5a++
  @0x80115c00, Se_on(`lui a0,0x302` / `ori a0,a0,0x1` = 0x03020001) @0x80115c04 -> faellt in P1.
  KEIN hit_react, KEIN aca3c (anders als vorn @0x80115a3c-4c).
- P1 @0x80115c0c: f314(acbcc/acbd0 = Opferbank der Kraehe) @0x80115c20; Umlauf -> Se_on 0x03020001 @0x80115c3c,
  Clip 1 @0x80115c54, Bild 0 @0x80115c5c, aca5a++ @0x80115c68.
- P2 @0x80115c74: f314 @0x80115c88 (Halte-Schleife Clip 1), Umlauf -> Se_on 0x03020001 @0x80115ca4.
- P3 @0x80115cb4: Clip 2 @0x80115cc4, Bild 0 @0x80115ccc, aca5a++ @0x80115cd8 -> faellt in P4.
- P4 @0x80115cdc: f314 @0x80115cf0; Umlauf -> aca58=4 @0x80115d04, aca59=6 @0x80115d10, `sh zero` aca5a
  @0x80115d18 = Kommando 4 / Modus 6.
- Modus 6 = 0x800517f0 (Tabelle 0x80073e30[6]): P0 @0x80051844-74 +0x6=1, Clip(+0x94)=1, Bild 0, +0x8f=7,
  faellt in P1 (f314 auf +0x170/+0x174 = PLW @0x80051884-8c); P2 @0x800518b4 Clip 2 @0x800518c8, P3 Schleife.
  Kein Selbst-Ausgang; Ende = naechster Plc-Befehl (Plc_dest-Re-Init @0x80041c14). Im Port vorhanden:
  re15_player_event_reach_begin/_tick/_end (game_step_common.c), der Plc_dest-Gang ruft _end (scd_vm.c).
- Se_on 0x03020001: FUN_80045024 Bank = `srl v1,a0,24` @0x80045028 = 3, Satz = `srl v0,a0,16; andi 0xff`
  @0x80045078-7c = 2; Bank-Tabelle @0x80010e70[3] = 0x800450f8: `sltiu v0,s4,0x19`, EDT = `lw a0,20(v0)` von
  [0x800ac778] @0x80045104-10 = die Raum-snd1-EDT, die im Port re15_audio_room_se() spielt (audio_pc.c).

### 2.4 RE2 hat dafuer nichts

RE2-Flag-Helfer (RE2 PSX.EXE) per Muster gefunden: SET 0x8007730c (`srl a2,a1,5 .. or v1,v1,v0; jr ra;
sw v1,0(a2)`), CLR 0x80077334, TEST 0x80077360. Das RE2-Kraehenmodul EMOVL21_S0.BIN (344 jal auf 42 Ziele)
ruft KEINEN davon. Das RE2-Kraehenhirn kann das RE1.5-Ereignis also weder ausloesen noch lesen (gemessen v2_re2:
Zustand 1/Sub 0, keine Bewegung). Die Szene ist in RE1.5 fertig (kein Beta-Loch) -> RE1.5 ist hier das Ziel.

### 2.5 Elzas Kraehenbank (Opfer-Clips) und der Peck-Effekt der Kraehe

- Modell-Lader FUN_80022300: Elza-Bit `bltz` @0x80022334 -> Datei 0x27 = EMD/CDEMD1.EMS (@0x800223ac). Die
  EM021-Opferbank (dir[5]) hat dort 14 / 31 / 30 Bilder (Leons CDEMD0: 14 / 36 / 20) — gemessen vom Riegel
  `unit_r38_heliport1171_banken` (beide Archive geparst). Die Rueck-FSM laeuft also mit 14/31/30.
- ORIENT 0x801152e0: Schritt 0 `jal 0x80115d94(8)` @0x80115310 (Clip 8), dann `ori a0,zero,0x1000` @0x80115318,
  a3 = 0x8012110c (16 Nullbytes, `bytes 0x8012110c`), `lw a2,392(v0)` (+0x188 Modellinstanz) / `lh a1,106(v0)`
  (+0x6a), `jal 0x80019700` @0x80115334 mit `addiu a2,a2,408` = Teil 2 (Teil-Stride 172, Matrix +64: 408 =
  172*2+64, dieselbe Rechnung wie Zombie @0x80105cd8), Schritt++ @0x80115354 und faellt in Schritt 1: f314
  @0x8011536c, bei Umlauf dieselbe FX @0x80115394. FUN_80019700 = ESP-Spawner (Effekt = a0>>24 = 0
  `srl t8,a0,24` @0x80019728, Sub = (a0>>16)&0xff = 0 @0x8001970c/30, Skala = a0&0xffff = 0x1000 `andi s1,a0,0xffff`
  @0x80019714 -> `sh s1,114(t0)` @0x800197dc; Port-Zwilling re15_esp_fx_spawn_ex, vgl. Granate 0x040D1000 in
  re15_esp.h). Der Port-Kommentar "no distinct effect" war falsch: es ist Raum-ESP-Effekt 0 (Skala 0x1000, halb so
  gross wie der Zombie-Treffer 0x2000) am Kraehenteil 2, bei Eintritt und bei jedem Clip-Umlauf.
- Ton der Ereignis-Kraehen (STAGE1.BIN `scan`): ASCEND 0x80114fb8 Schritt 0 `jal 0x800453d0` @0x80115004 mit
  `addu a0,zero,zero` @0x80115008 = Raum-SE 0, dann Clip 4 @0x8011500c; SPIN 0x801153ac Schritt 0 `jal 0x800453d0`
  @0x801153f8 / a0 = 0 @0x801153fc, Clip 4 @0x80115410, +0x8c = 0xb4, +0x1e4 = -30, +0x1d5 = 0x20. HOVER, ORIENT,
  DIVE-ARM 0x8011553c und die Subs 0x80115830/0x80115910 rufen keinen SE. Der Port spielte in ASCEND und SPIN keinen
  Ton (SPIN wird waehrend (5,0x1D) gesetzt ist immer wieder betreten -> der Kraehenruf wiederholt sich).

## 3. Umsetzung

Neue Dateien (Logik):
- `re15_port/include/re15_heliport1171_r38.h` — Belege, Konstanten (Typ 0x21 / beh 0x40 @0x016DC, Bank 5 @0x80074664[5],
  Bits 0x1C @0x801152c4 / 0x1D @0x801120a8, SE-Satz 2 @0x80045078-7c, Blend 7 @0x80115bd0, Clips 0/1/2
  @0x80115bec/@0x80115c54/@0x80115cc4, Phase 3 @0x801120e8, Clip-Laengen CDEMD1 14/31/30).
- `re15_port/engine/src/heliport1171_r38.c` — `re15_r38i_ereigniskraehe`, `re15_r38i_rueckgriff` (HOVER-Kontakt:
  aca58=5/aca59=1 nur ohne laufenden Griff @0x80115264-70, Kommando ersetzt Ziel-Aktion/Plc-Gang/Modus 6),
  `re15_r38i_wurfstoss` (Wurzel @0x801120a0-e8), `re15_r38i_rueckgriff_tick` (Rueck-FSM P0..P4 0x80115b80, Pose ueber
  re15_player_victim_force = Kraehen-Opferbank, Ende -> re15_player_event_reach_begin = Modus 6 0x800517f0),
  `re15_r38i_install` (Kommandowort weg beim Raumaufbau, FUN_800314b0 @0x80031518).

Haken in gemeinsamen Dateien (je 1-3 Zeilen, Kommentar "Runde 38 Spur I"):
- `engine/src/enemy_ai_common.c`: `#define s_crow_gflags (g_game.flags[5][0])` (statt privatem Wort) und kein
  Ruecksetzen im Flock-Reset (Raum-Init @0x8003ed74 loescht Bank 5); RE2-Weiche der Kraehe `&& !re15_r38i_ereigniskraehe(e)`;
  HOVER-Griff `re15_r38i_rueckgriff(e, player)` statt `re15_crow_hit_player(e, player, 8)` (die -8 HP waren erfunden);
  `re15_r38i_wurfstoss()` an der Stelle @0x801120a0-e8 der Wurzel; ORIENT ruft `re15_r38i_orient_blut(e)` bei Eintritt
  (@0x80115334) und Clip-Umlauf (@0x80115394); ASCEND/SPIN-Eintritt `re15_audio_room_se(0)` (@0x80115004/@0x801153f8);
  veraltete Kommentare ("in STAGE1 nie gesetzt / unerreichbar") berichtigt; Include.
- `engine/src/game_step_common.c`: `re15_r38i_rueckgriff_tick()` direkt nach `re15_player_victim_tick()` (Spieler-Takt
  nach dem Gegner-Durchlauf, wie Hook A im Spieler-Task nach FUN_8001a50c).
- `engine/src/scd_room_setup.c`: Installer-Zeile am Ende des Blocks.
- `platform/pc/main.c`: beide Bank-Ladestellen `&& !re15_r38i_ereigniskraehe(...)` -> RE1.5-Bank (CDEMD1) fuer die
  Ereignis-Kraehen in jedem KI-Modus (Muster "0x26 nach HERKUNFT").

PORT-WAHL (Form belegt): Ereignis-Kraehen laufen in jedem KI-Modus auf RE1.5-Hirn + RE1.5-Bank. Begruendung: das
Ereignis existiert nur in RE1.5 (RE2-Modul ruft keinen Flag-Helfer, 2.4), der Mechanismus ist dort vollstaendig; die
Weiche greift game-weit nur in ROOM1171 sub07 (Zensus 2.1, Riegel `zensus`). Schaden/HP-Tabellen der Kraehe bleiben
typ-gebunden: in der Szene kann Elza nicht schiessen (pm=2 von @0x013A8/@0x013AC bis zum Raumwechsel).

Uebernommene Konvention: Modus 6 ist die vorhandene Port-Maschine (re15_player_event_reach_*), deren erster Takt im
selben Bild wie der Start vorrueckt (wie beim Plc_dest-Modus-6 der Szenen 10D0/1090) — Original: Modus 6 beginnt im
Bild NACH dem Umlauf @0x80115cf0 und zeigt dort Bild 0; Port: Bild 1 im Umlaufbild. Ein Bild Versatz, nicht neu.

## 4. Messung nachher

Belege in `analysis/befunde_runde38/I_belege/` (Logauszuege, Zeitlinie, Bilder).

- Lade-Weg ROOM1171 (8366bfb5, Messkopie, beide KI-Modi, `n1_re2`/`n1_re15`): Griff F95 (gr=1, Opfer-Clip 0),
  Warten @0x013F4 vorbei F96, Clip 1 ab F109 (= 14 Bilder), Wurf F156 (Clip 2, = Griff + 1 + Sleep 60), Modus 6
  F186 (PLW-Clip 1, = Wurf + 30), Clip 2 ab F201; Kraehen: ORIENT -> SPIN F156 -> Tod F363 / F410 (Zustand 3) ->
  Leiche F388 / F437 (Zustand 7); msg 0 / 1 / 3; sub03/sub04 F847; `Aot_on 01` -> DOOR FIRE Slot 1 -> ROOM10B1
  (F1077 sub02 von 10B1). HP durchgehend 100. Identische Bildzahlen in RE2- und RE15-Modus (dasselbe RE1.5-Hirn).
  Zeitlinie: `I_belege/nachher_n1_re2_zeitlinie.txt`.
- ECHTER WEG (Elza-Spielstand ROOM1131 vor der Flurtuer, (3,0xC2)=1): Aktionstaste -> DOOR FIRE Slot 3 -> ROOM1171
  (Cut 8, kein Ereignis) -> Selbst-Tuer Slot 5 (RE15_FIRE_AOT, sonst derselbe Tuerpfad) -> Szenario 0 -> sub02 F61 ->
  msg 0/1/3 -> ROOM10B1. VORHER (build_bisect = e2cc7673): nach sub02 F61 nur noch "RE2 EM021 loaded", bis F1500
  keine Zeile mehr. `I_belege/weg_vorher_nachher_debug.txt`.
- Bild (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, Lade-Weg, `I_belege/nachher_szene_framedump.png`): F95-F130 Kraehe
  sitzt Elza im Nacken (Rueckgriff, Opfer-Pose der Kraehenbank), F160/F175 Wurf-Clip (Elza nach vorn gekruemmt),
  F190/F205 Modus 6 (PLW-Clips), F450 "John: Elza, hurry up!", F540/F580 zwei Kraehenleichen auf dem Helipad, Elza
  laeuft zu John.

## 5. Tests

Registriert NUR in `re15_port/tests/unit/probes/r38_heliport1171.cmake`:
- `test_r38_heliport1171.c` (Unit, echte RDTs + echte Baenke, Bilder in main.c-Reihenfolge), Teile:
  `banken` (EM021-Opferbank beider Archive: CDEMD0 14/36/20, CDEMD1 14/31/30 = Header), `zensus` (opcode-genauer Walk
  aller 206 RDTs: Ereignis-Kraehen nur ROOM1171 @0x016DC/@0x016F0, kein Skript setzt (5,0x1C)=1, Warte-Bytes
  @0x013F4), `flagwort` (HOVER-Kontakt setzt Bank-5-Wort-Maske 0x8, kein HP-Abzug, ORIENT-Effekt bei Eintritt und
  Clip-8-Umlauf, (5,0x1D) -> SPIN + Opfer-Phase 4, (5,0x1F) -> Zustand 3 + Bit geloescht), `szene_re2` / `szene_re15`
  (ganze Szene: Griff, Warten endet im Folgebild, Clip 0 = 14 Bilder, SE 2 im Griff-Bild und am Clip-Ende, Wurf =
  Griff+61, Modus 6 = Wurf+30, HP 100, beide Kraehen tot + Leiche, msg 0/1/3, Tuer -> ROOM10B1, (4,0xE9)=1),
  `danach` (Wiedereintritt Cut 3: 7 normale Kraehen, Abstieg Slot 2 -> ROOM10B1 installiert), `leon1170` (Leon: 7 Kraehen,
  keine Ereignis-Kraehe, keine im FLIGHT-2, Bank-5-Bits unberuehrt).
- `tests/integration/test_r38_heliport1171.cmake` (echte exe, Messkopie): `weg_re2`, `weg_re15` (ROOM1131 -> Flurtuer ->
  ROOM1171 -> Selbst-Tuer Slot 5 -> Szene -> ROOM10B1), `laden` (Lade-Weg); prueft Texte 0/1/3 in Folge, Tuer Slot 1 ->
  ROOM10B1, keine RE2-EM021-Bank, HP 100, Griff -> Wurf-Clip -> Modus 6, danach Elza steuerbar (pm=0 in ROOM10B1).
  Gegenprobe gegen e2cc7673 (build_bisect): ROT mit "Texte 0/1/3 nicht in Folge (-1/-1/-1) - die Szene haengt".
  Kuerzung (ehrlich): die Selbst-Tuer Slot 5 feuert der Haken RE15_FIRE_AOT (re15_aot_fire_slot = derselbe Tuerpfad wie
  die Aktionstaste); der Treppenweg Boden -> Ebene 2 -> Ebene 4 (Slots 6/8) wird nicht gelaufen.

(Ergebnisse unten.)

## 6. OFFEN

- `0x800acc0c := 1` in P0 der Rueck-FSM (@0x80115bc4) hat im Port kein Modell (wie in der Vorn-FSM, crow_victim_anim.md
  §7.6) — unveraendert offen, ohne Wirkung auf den Ablauf.
- Se_on 0x03020001 ist positional (a0&0xff = 1 -> 0x80045a64 mit Spieler+0x34, @0x80115c04); der Port spielt alle
  Raum-SEs (re15_audio_room_se) ohne Positionsmischung — bestehende Port-Eigenschaft, nicht neu.
- Modus-6-Takt: ein Bild frueher als das Original (Abschnitt 3, uebernommene Konvention der vorhandenen Maschine).
- INTEGRATION (Hinweis an den Orchestrator): Spur B ("kein Angriff in Szenen") darf den Rueckgriff der Ereignis-Kraehe
  NICHT sperren — er ist hier der Ausloeser der Szene (einziger Setzer von (5,0x1C)); eine Sperre liesse sub02 wieder
  ewig @0x013F4 warten. Die Riegel `unit_r38_heliport1171_szene_re2/_re15` und `integration_r38_heliport1171_*`
  fallen dann rot. Spur F (1141 -> 1171): unabhaengig — der Ereigniszweig haengt nur an (3,0xC2)==1 und an einem
  Eintritt mit Szenario 0/3 (sub00 Switch @0x01374/@0x01384).

## 7. Fuer den Nutzer

- Neue Assets: keine. Sprachdateien: keine neuen. Die Nutzer-Aufnahmen synchro/STAGE1/room1171/main00.wav und
  main01.wav gehoeren zu msg 0 "Elza, hurry up!" (@0x014BA) und msg 1 "We need to get this thing off of here! Help me
  out!" (@0x0151E) und spielen jetzt, weil die Szene durchlaeuft (Regel synchro/README.md: Message-Id). HINWEIS:
  main02.wav gehoert zu msg 2 "C'mon, give me a hand!" — diese Zeile zeigt das Original-Skript NIE (Zensus aller
  Message_on in ROOM1171: nur 0, 1, 3); fuer msg 3 "Alright, let's get down from here!" (@0x01586) fehlt eine
  Aufnahme (main03.wav).
- Bedienung: keine Aenderung. Elza wird auf dem Helipad von hinten von einer Kraehe gepackt (wie im Original, kein
  Schaden), nach dem Wurf erschiesst John beide Kraehen, die Szene endet mit "Alright, let's get down from here!"
  und dem Abstieg nach ROOM10B1.
- Entscheidungsfrage: keine.
