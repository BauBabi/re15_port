# Kraehen-Wurf cmd-4 Mode 6 (Softlock-Risiko)

Alle Byte-Belege stehen. Ich stelle das Dossier-Rohmaterial zusammen.

---

# KRÄHEN-WURF (cmd 4 / Mode 6) — vollständiges RE, Port-Stand, Plan

## 1) MECHANISMUS (alles byte-verifiziert, Roh-Disasm dieser Session sofern nicht anders markiert)

### 1.0 Prämissen-Korrektur (wichtig fürs Dossier)
Der Wurf ist **NICHT der Sturz-Angriff (Dive)**. Der Dive schreibt cmd **2** (Standard-Flinch, `sb 2→0x800aca58` @0x80113b00, dir=facing+2 @0x80113b10-28). Der cmd-4-Write @0x80115d04 ist der Abschluss des **RÜCK-PECK-GRABS** — er liegt in der **spieler-seitigen Opfer-FSM (REAR)**, nicht in einem Krähen-Attack-Move. Zweitens: das Audit (`AUDIT_GESAMT_2026-08-23.md:91` „Divergenz ROOM10C0") ist **falsch verortet** — ROOM10C0 kann den Wurf nie erreichen (§1.6).

### 1.1 Auslöser-Kette (Krähe, STAGE1.BIN)
1. **Event-Krähe:** `Sce_em_set` behavior-Byte = grid_id (+0x9). INIT-Override @0x80112408: `grid & 0x40` → state 4 (FLIGHT-2). FLIGHT-2-Substate-Tabelle **@0x80121220** (Dump): [0]=0x80114fb8 ASCEND, **[1]=0x80115130 HOVER**, **[2]=0x801152e0 HOLD**, [3]=0x801153ac SPIN, [4]=0x8011553c DIVE-ARM, [5]/[6]=0x80115830/0x80115910.
2. **ASCEND→HOVER:** vert_err ≥ 2001 @0x80115118.
3. **HOVER = FUN_80115130:** Halbzyklus-Oszillation (`+0x8c=0xb4` @0x80115168-70, vvel = `+0x1d4 & 0x3f` @0x80115180-8c, Negation bei vert_err<2000 @0x801151a4-bc), Yaw-Track auf den Spieler `jal 0x8001a8f8` rate 0x32 @0x80115238, `jal 0x800245d8(0)` @0x80115240. **Treffer-Voraussetzung: Kontakt-Flag `+0x1d0 ≠ 0`** @0x80115254-5c — sonst nichts. **Kein hit_react-Gate, KEIN HP-Write** (voller Disasm 0x80115130–0x801152dc: keine einzige Instruktion auf 0x800acaee — anders als Front-Grab `hp−=8` @0x80113e34-3c!).
4. **REAR-CONNECT** (wenn `aca58 != 5` @0x80115264-70, sonst nur Sprung zu Schritt 5):
   ```
   8011527c: sw a0 -> 0x800acbfc      ; Grabber-Zeiger
   80115280/90: lw +0x178 -> sw 0x800acbcc   ; Paar C EMR
   801152b0/b4: lw +0x17c -> sw 0x800acbd0   ; Paar C EDD
   80115288: sh 0 -> 0x800aca5a       ; Phase 0
   801152a0: sb 5 -> 0x800aca58       ; cmd 5 GRAB
   801152ac: sb 1 -> 0x800aca59       ; dir 1 = REAR-Opfer-FSM
   801152b8: jal 0x80115d74 (a0=2)    ; Kraehe -> sub 2 HOLD
   801152c4: jal 0x8004ef90 (a1=0x1c) ; z5-Flag 28 setzen (Basis 0x800b1028)
   ```
5. **Krähen-HOLD 0x801152e0** (sub 2): Ph0 Clip 8 (`jal 0x80115d94(8)` @0x80115310-14) + Blut-FX `jal 0x80019700(0x1000, rot_y+0x6a, *(+0x188)+408, 0x8012110c)` @0x80115318-38, danach je Clip-Wrap erneut @0x80115374-98. **Kein Spieler-Write, kein HP, kein Mash-Budget** — der Rear-Hold endet NUR per Script (anders als Front-Grab-Mash `+0x9c<0 → aca5a++` @0x801140b4-c0).
6. **Script-Push z5:29** — ROOM1171.RDT sub02 (Byte-Dump dieser Session): @0x13f4 `0f 04 08 00 21 05 1c 00 02 00…` = While-Wait auf Ck z5:28; @0x1400 `09 0a 3c 00` = Sleep 60; **@0x1404 `22 05 1d 01` = Set z5:29**. Krähen-Root-Pre-Pass @0x801120a0-e8 (Roh-Disasm): `jal 0x8004efe4(0x800b1028, 0x1d)` && `aca58==5` (@0x801120bc-c4) && `aca5a<3` (@0x801120d0-dc) → **`sb 3 → 0x800aca5a`** @0x801120e4-e8 = Opfer-FSM-Phase 3 (Wurf-Clip). Parallel schaltet der FLIGHT-2-Dispatcher die Krähe bei Bit 0x1d auf sub 3 SPIN (@0x80114f34, Port-zitiert) = Wegflug.
7. **Der Wurf-Write** — REAR-Opfer-FSM Phase 4 (Tabelle **@0x80100304**, Dump: [0..4]=0x80115bb8/0x80115c0c/0x80115c74/0x80115cb4/**0x80115cdc**), erreicht via Hook-A-Dispatch (§1.2):
   ```
   80115ce4/ec: lw [0x800acbcc]/[0x800acbd0]   ; Paar C
   80115cf0: jal 0x8001f314 (a2=0, a3=0x200)   ; Wurf-Clip 2 spielen
   80115cf8: beq v0,zero,0x80115d1c            ; noch nicht fertig -> return
   80115d04: sb 4 -> 0x800aca58                ; cmd 4  (Plc-Klasse)
   80115d10: sb 6 -> 0x800aca59                ; Mode 6 (Halten)
   80115d18: sh 0 -> 0x800aca5a                ; Phase 0 (+ aca5b)
   ```
   Kein hit_react-/aca3c-Clear (die REAR-FSM hat beides nie gesetzt — Ph0 @0x80115bb8 schreibt nur acc0c:=1, Blend 7 @0x80115bd0, Clip 0 @0x80115bec, SE `0x80045024(0x03020001, player+0x34)` @0x80115c04; Asymmetrie zur Front-FSM, crow_victim_anim.md §1.3, Phasen-Sites re-verifiziert).

### 1.2 Spieler-Seite: die Dispatch-Kette
- Top-Level **PTR_LAB_80073f90** (Index `aca58`, Dump): [0]=0x800318f8 Idle-Init, [1]=0x80031de8 Gameplay, [2]=0x80035af0 Hit, [3]=0x800366bc Death, **[4]=0x80030660 Plc**, **[5]=0x80036834 Grab**, [6]=0x800368c0, [7]=0x8003694c. **Genau 8 Einträge** — ab 0x80073fb0 beginnt die NÄCHSTE Tabelle ([8]=0x80031f38, [9]=0x800322e8 … = cmd-1-Substate-Raum), sie ist am Wurf **unbeteiligt**.
- **cmd-5-Handler 0x80036834** (Roh-Disasm): erster Frame (`aca5a==0`): `acb04:=0` @0x80036860, `acc0e := −floor·1800` @0x80036854-80; dann jeden Frame `lbu type=+0x8([0x800acbfc])` @0x80036890, **`jalr [0x800AC758 + type·4]`** @0x8003689c-a8 (Basis: a0=0x800aca5a, Offset −770 = 0x800ac758). Typ 0x21 → 0x800ac7dc = **Hook A `LAB_8011597c`** (Registrierung `sw @0x8011ea20-28`), dieser `jalr @0x8012123c[aca59]` (Dump: [0]=0x801159bc FRONT, **[1]=0x80115b80 REAR**, [2]=0x80115d6c `jr ra`-Stub).
- **cmd-4-Handler 0x80030660** (voller Disasm): Half-Rate-Gate `0x800acc18` Bit 0x10+Parity 0x20 (@0x80030670-80, Toggle `xori 0x20` @0x800306c4-c8), Dispatch **`jalr [0x80073e30 + aca59·4]`** @0x80030694-ac ohne Bounds-Check, Footlock-Tails `acc18&1/&2 → 0x800369f8` @0x800306d8/f4.
- **0x80073e30[6] = 0x800517f0** (Dump @0x80073e48). Handler voll disassembliert, Entity = `[0x800ac784]`:
  - Ph0 @0x80051844: `+0x6:=1`, **Clip `+0x94:=1`** @0x80051854, Frame 0 @0x80051864, Blend `+0x8f:=7` @0x80051874 → fällt in Ph1;
  - Ph1 @0x80051878: `f314(+0x170, +0x174, a2=0, 0x200)` @0x8005188c, `+0x6 += ret` @0x800518a8-b0 (Clip-Wrap → Ph2);
  - Ph2 @0x800518b4: `+0x6:=3`, **Clip 2** @0x800518c8, Blend 7 @0x800518d8 → fällt in Ph3;
  - **Ph3 @0x800518dc: nur f314 — KEIN Phase-Advance, KEIN Pad-Read, KEIN Dest-Read = Endlos-Halt.**
  - Kanal beim Spieler: `+0x170/+0x174` = 0x800acbc4/0x800acbc8 = **PLW-Paar B** (FUN_80036b68); PL00W01-Clips 1/2 = 16/52 Frames, 0 Fußaufsetzer (Bank-Beleg @0x800312a4/ac, gemessen actor_locomotion.c:423-428).

### 1.3 DER EXIT (die Softlock-Frage) — Script-getrieben, byte-belegt, KEIN Engine-Timeout
0x800517f0-Ph3 hat keinen Selbst-Exit (§1.2). Die Auflösung:
1. **Regulärer Szenen-Exit:** ROOM1171 sub02 **@RDT 0x14de** (Hex-Dump dieser Session): `2e 01 00 00` = Work_set(1,0=Spieler), **`40 00 05 00 8c 00 ce f0`** = `Plc_dest mode 5 → (140, −3890)`. Opcode-Handler **0x80041be4** (Roh-Disasm): Guard @0x80041bf8-c0c = `lhu +0x1c4; andi 4; beq zero→INIT` und `lbu +0x5; beq ==mode→skip`; da `+0x5 == 6 ≠ 5`, läuft der **volle Re-Init**: `sb 4→+0x4` @0x80041c14, `sb mode→+0x5` @0x80041c18, `sb 0→+0x6` @0x80041c1c, `sb 0→+0x7` @0x80041c20, `sh 0→+0x1c4` @0x80041c4c → Mode-6-Halt ist beendet, Leon rennt. (Danach weitere Plc_dest-Ketten; Szenen-Ende stellt cmd 1 wieder her — `Plc_ret`-Pfad LAB_80041f88, Port-zitiert scd_vm.c:2306.)
2. **Weitere legitime Beender:** `Plc_motion` überschreibt `+0x5`; Raumlader `sw zero→0x800aca58` @0x80031518 (FUN_800314b0); Karten-Screen-Exit `sb zero` @0x8001cbdc.
3. **Konsequenz:** einen Timeout zu erfinden wäre ein Rate-Defekt. Softlock-Schutz = das Script selbst; der Port hat die drei Beender bereits (§2).

### 1.4 Mode-6-„EVENT-REACH"-Kollision — AUFGEKLÄRT: zwei Tabellen, EIN Handler
- **0x80073e30** = Spieler-Plc-Mode-Tabelle (via 0x80030660): [4]=0x80030af0 WALK, [5]=0x80030d28 RUN, **[6]=0x800517f0**, [7]/[8]/[9]=0x80031080/0x800311f0/0x80031360, [0-3]/[10]/[11] generische Executor.
- **0x80076ca0** = NPC-State-4-Tabelle (Dump): [4]=0x80051148, [5]=0x80051484 (NPC-eigene Walk-Subs), **[6]=0x800517f0 — identisch**. Ihr Dispatcher ist **FUN_80050be8**, ein exakter Zwilling von 0x80030660 (Roh-Disasm: Half-Rate-Gate auf `ent+0x1c4` @0x80050bf8-c0c, `lbu +0x5; sll 2; lui 0x8007; addiu at,27808 → 0x80076ca0; jalr` @0x80050c14-34; Ghidra hat den Xref wegen des lui/addiu-Paars nicht — deshalb fand ihn kein Text-Grep).
- Semantik: Mode 6 = „Clip 1 einmal, Clip 2 als Halte-Loop aus dem `+0x170/+0x174`-Kanal". Beim **NPC** ist der Kanal die Event-Bank (Bank 1 dir[4]/dir[3]) → „EVENT-REACH"-Pose; beim **Spieler** die PLW-Bank → Liege-Halt nach dem Wurf. **Gleiche Maschine, zwei Etiketten.** 0x800741a8 (cmd-2-Hit-Tabelle, Dump: 6 Einträge, [4]/[5]=Knockdown 0x800360e8/0x8003644c) und 0x80073fb0 (Beginn der Folgetabelle) haben mit Mode 6 nichts zu tun.

### 1.5 Anim-Inhalt des Wurfs
Wurf-Clip = **Paar C Clip 2** (EM021-Victim-Bank, CDEMD0.EMS Blob 8 @0x17b800, dir[5]-EDD @blob+0x49d4: Clips 0/1/2 = 14/36/20 Frames, dir[6]-Pool 15 Bones/80 B = PL00-kompatibel — crow_victim_anim.md §1.5). Danach **PLW Clip 1 (16 f) → Clip 2 (52 f) Loop**. Root-Motion gibt es in Mode 6 nicht (kein 245d8/ad68 im Handler-Body) — der „Wurf" ist rein animiert, Position bleibt.

### 1.6 Raum-Zensus (RDT-Byte-Belege dieser Session): Wurf existiert NUR in ROOM1171
`Sce_em_set`-Scan (op 0x44, pc[2]=0x21) über alle STAGE1-RDTs: Krähen-Spawns mit **behav&0x40 gibt es ausschließlich in ROOM1171.RDT @0x16dc (slot 1) und @0x16f0 (slot 2)**. ROOM10C0/10C1: 3× behav 0x00 (@0xf8c/0xfa0/0xfb4); ROOM1120/1121: 3× 0x00; ROOM1170/1171-Normalkrähen: 2×0x90/3×0x10/2×0x00. behav 0x00/0x10/0x90 → state 1 → nur FRONT-Grab (`sb 0→aca59` @0x80113e30, Exit cmd 1 @0x80115b40) → **der Wurf-Pfad (aca59=1) ist in 10C0 mechanisch unerreichbar**. ROOM10C0-Savestates existieren nicht in stage_saves/ (leer für 10c0/1171); für 10C0 ist auch keine Gegenprobe nötig, da der Auslöser dort statisch nicht existiert.

## 2) PORT-STAND (code-verifiziert, Datei:Zeile)

| Baustein | Ort | IST |
|---|---|---|
| Mode-6-Halt (0x800517f0) auf dem Spieler | `game_step_common.c:88-166` (`s_ev_reach`, `re15_player_event_reach_begin/end/clip/tick`), Render-Override `platform/pc/main.c:5752-5756`, Anim-Übergabe `player_common.c:548-556` | ✓ byte-true vorhanden (Clip 1 einmal → Clip-2-Loop, PLW-Längen aus EDD) |
| Mode-6-EINTRITT via Script | `scd_vm.c:2167-2168` (Plc_dest mode 6 → begin), `actor_locomotion.c:429-431` (Walk-Ankunft → begin, = `aca59:=6` @0x80030cf0) | ✓ |
| Mode-6-EXIT | `scd_vm.c:2178-2186` (Walk-Plc_dest → end + state 4), `:1953-1958` (Plc_motion → end), `:2301-2304` (Plc_ret → end), `game_step_common.c:135-150` (`re15_player_cmd_reset`: Raum-Load/Karten-Exit killt `s_ev_reach`) | ✓ **der Exit-Apparat existiert bereits vollständig** |
| Krähen-FLIGHT-2 (state 4) | `enemy_ai_common.c:5340-5412` (ASCEND/HOVER/ORIENT/SPIN/DIVE-ARM), Dispatcher-0x1d-Tick `:5753-5754`, INIT-Override grid&0x40 `:5662-5663` | ✓ Maschine da, ABER: |
| Rear-Connect | `:5366-5370`: `re15_crow_hit_player(e, player, 8)` | ✗ **DIVERGENZ: −8 HP (Original hat KEINEN HP-Write, §1.1.3); kein Victim-Latch, kein aca59=1, kein z5:28** |
| Opfer-FSM | `:688-760` Latch (Krähe hart variant=0, `:694-696`), `:1023-1253` Tick (Front/Struggle/Release komplett inkl. Krähen-Sonderfälle `:1049/:1074/:1226/:1239`) | Front ✓; **REAR (variant 1) fehlt komplett** |
| z5:29-Root-Push (@0x801120a0-e8) | `:5597-5600` | ✗ **explizit OPEN markiert** (und dort als „SPIN escalation" fehl-etikettiert — es ist der Opfer-FSM-Phase-3-Push) |
| z5-Flag-Speicher | `s_crow_gflags` (`:4656`, Bits 28-31 als Masken 0x8/0x4/0x2/0x1) vs. SCD `re15_game_flag(5,…)` (`scd_vm.c` op_ck/op_set:1768ff) | ✗ **ZWEI getrennte Speicher für dasselbe Original-Wort 0x800b1028** — die Spinne liest sogar schon `re15_game_flag_get(5,31)` (`:10617`) während die Krähe `s_crow_gflags&0x1` prüft (`:5593`). Die 1171-Szene (SCD Set z5:29 / Ck z5:28) kann die Krähe so nie erreichen |
| Wurf-Writer | `game_step_common.c:79` (Kommentar „bleibt OFFEN") | ✗ fehlt — Aufgabe dieses Tickets |

## 3) IMPLEMENTIERUNGS-PLAN (jede Konstante trägt ihre §1-Adresse)

1. **G1 — z5-Brücke (Voraussetzung für alles):** `s_crow_gflags`-Bits 28-31 auf `re15_game_flag(5, 28..31)` umstellen (Sites: enemy_ai_common.c 4663/4667/5367/5395/5593-5595/5753/7136). Maske↔Index: `0x80000000>>(idx&0x1f)` (FUN_8004ef90 @0x8004ef90-efb4) — 28→0x8, 29→0x4, 30→0x2, 31→0x1. Damit erreichen `Set z5:29` (RDT @0x1404) und `Ck z5:28` (@0x13f4) die Krähe, und der Krähen-Grab-Set (@0x801152c4) das Script. Konsum-Clear von Bit 31 (@0x80112098) mitziehen. Beseitigt zugleich die heutige Spinnen/Krähen-Inkonsistenz (:7136 vs. :10617).
2. **G2 — Rear-Connect byte-true** (enemy_ai_common.c FLIGHT-2 case 1, :5366-5370): `re15_crow_hit_player(...,8)` ERSETZEN durch: (a) wenn schon gegriffen (`g_player_victim==1`) → nur `re15_crow_sub(e,2)` (@0x80115270); (b) sonst Victim-Latch Typ 0x21 **variant 1** (@0x801152ac), Phase 0 (@0x80115288), **KEIN HP-Abzug** (§1.1.3), **KEIN hit_react/aca3c** (REAR-Ph0-Asymmetrie @0x80115bb8-c04), crow sub:=2 (@0x801152b8), `re15_game_flag_set(5,28,1)` (@0x801152c4). `re15_player_victim_latch` braucht dafür einen Variant-Parameter/Pfad (der :694-Hartcode 0 gilt weiter für den Front-Grab @0x80113e30) und muss für variant 1 die Front-Only-Writes (`hit_react|=1` :719, aca3c-Repräsentation) überspringen.
3. **G3 — REAR-Opfer-FSM** in `re15_player_victim_tick` (Verzweigung auf `type==0x21 && variant==1`): Ph0 Clip 0 + Blend 7 (@0x80115bd0/bec) + SE `0x03020001` (@0x80115c04); Ph1 Wrap→Clip 1 + SE (@0x80115c3c-54); Ph2 Halte-Loop, SE je Wrap (@0x80115ca4), **kein Selbst-Advance**; Ph3 = z5:29-Push aus dem Krähen-Root-Pre-Pass (:5597 OPEN schließen): `re15_game_flag_get(5,29) && Grab aktiv && Phase<3 → Phase 3` (@0x801120a0-e8) → Clip 2, Frame 0, **ohne** Blend-Seed (Port modelliert das schon :1049-1051); Ph4 Wrap → **DER WURF**: `re15_player_event_reach_begin()` (= cmd 4/dir 6/Phase 0 @0x80115d04/10/18), Victim-State räumen, Pin lösen, **ohne** hit_react-Clear und **ohne** den Front-Exit-Kram (:1206-1246 nicht durchlaufen). Alternativ-Route über den bestehenden `!s_player_grabbed`-Release-Zweig (:1043) ist NICHT äquivalent — der Original-Push ist z5:29 auf `aca5a`, nicht das Krähen-Sub-Ende.
4. **G4 — Kommentar-/Doku-Korrekturen:** game_step_common.c:79 (Exit ist belegt, §1.3), enemy_ai_common.c:5597-5600 (Fehletikett „SPIN escalation" → Opfer-FSM-Phase-3-Push), AUDIT-P1 („ROOM10C0" → ROOM1171, §1.6).
5. **Explizit NICHT bauen:** kein Timeout, kein Mash-Exit für Mode 6 (§1.3), kein HP-Abzug am Rear-Connect, kein Wurf-Root-Motion.
6. **Verifikation:** (a) Unit-Sonde (re15-room-probe-Stil) ROOM1171: behav-0x40-Krähe → state 4 (INIT :5662), sub 1 mit Kontakt forcieren → HP unverändert, `g_player_victim=1/variant=1`, Clips 0→1-Loop; `re15_game_flag_set(5,29,1)` → Clip 2 (20 f) → Wrap → `re15_player_event_reach_clip()==1` dann `==2`; dann `op_plc_dest(mode 5)` → `s_ev_reach==0`, Walker läuft (**kein Softlock**). (b) Szene end-to-end: `RE15_FORCE_EVENT`/z3:194-Zweig, sub02-Ablauf gegen §1.1.6 (Wait z5:28 → Sleep 60 → Wurf → 6× SE + z5:31-Abschuss → Plc_dest @0x14de). (c) Regression ROOM10C0/1170: Front-Grab-Ende weiterhin Kontrolle zurück (bestehende Probes). (d) gdigrab-Sichtprüfung (Skill re15-port-visual-verify) für die Liege-Pose.

## 4) OFFEN (ehrlich)

1. **SE `0x03020001`** (REAR-FSM, @0x80115c04): Format Bank 3/Record 2 (analog 0x04010001 = Bank 4/Rec 1, enemy_ai_common.c:1182-1184), aber die Bank-3-Tabelle ist im Port nicht gemappt. Nächster Schritt: `FUN_80045024`-Bank-Decoder gegen die residenten EDH-Tabellen (0x800ac778-Familie) laufen lassen.
2. **Visuelle Clip-Identitäten** PLW 1/2 (16/52 f) und EM021-Victim 0/1/2 (14/36/20 f): Sequenzlogik byte-belegt, Sichtprüfung (gdigrab/Render-Katalog) steht aus — Bestandteil der Verifikation (d).
3. **Wer den 1171-Szenen-Schluss `aca58:=1` schreibt** (nach den Plc_dest-Ketten): Plc_ret-Pfad LAB_80041f88 ist Port-zitiert (scd_vm.c:2306), von mir aber nicht neu roh-disassembliert; für den Wurf-Exit irrelevant (der Halt endet schon @0x14de), fürs Szenen-Dossier nachziehen.
4. **`0x800acc0c`-Semantik** (REAR-Ph0 setzt 1; Konsument FUN_80024c30) — weiter offen (= crow_victim_anim §7.6); im Port bislang ohne Gegenstück, Wirkung unbekannt → beim Szenen-E2E-Test auf Anim-Basis-Artefakte achten.
5. **Modes-4/5-Ankunfts-Bug-Interaktion** (unaligned Flag-Set 0x800B11DE, player_knockdown §5.4): sub05 der Szene wartet auf z5:0 — ob die Szene im Original über den Bug hinwegläuft, ist nur statisch kartiert; Live-Capture (re15-room-capture, z3:194) steht aus. Der Port implementiert die korrekte ([7]-Muster-)Codierung.
6. **FLIGHT-2-Subs [5]/[6]** (0x80115830/0x80115910) nur teil-RE'd (Fade/Spezial); für den Wurf nicht auf dem Pfad.
