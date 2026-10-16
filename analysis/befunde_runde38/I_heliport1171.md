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

## 3. Umsetzung

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
