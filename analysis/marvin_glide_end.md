# ROOM10D0 — Marvin GLEITET am Cutscene-Ende (Nutzer-Report #4): RE-Dossier (2026-08-04)

**Nutzer-Report:** "Marvin scheint am Ende seiner Cutscene (sub21) zu schweben/gleiten
statt zu laufen" — vierter Report, NACH den Fixes 3f5310cc (Walker advanced Clip 5) und
2194c41f (Bank-1-Kanal fuer Executor-Subs {2,4,5,6,9}).

**Ergebnis in einem Satz:** Der Port laesst Marvins Abgang (Plc_dest **Mode 5 = RUN**,
sub21 +0x0b5e) vom generischen Locomotion-Walker fahren, der (a) waehrend der Align-Phase
die ALTE Gesten-Pose eingefroren haelt (und aus der falschen Bank posiert), (b) den
**WALK-Gait Clip 5** (30f) bei RUN-Speed 200/tick spielt — das Original schaltet beim
Align-Abschluss auf den **RUN-Gait Clip 0** (`sb zero,0x94` @0x8005157c, Bank 1, 22f) —
und (c) bei Ankunft **einfriert** (anim_freeze, Frame-0-Pose, 56 Ticks Statue), waehrend
das Original auf **Sub 6 Event-Reach** uebergibt (`+0x5=6` @0x80051794: Clip 1 einmal →
Clip-2-Idle-Loop) — Marvin kommt an, gestikuliert und atmet, bis Pos_set ihn parkt.

---

## 1. MESSEN — Port-Repro (probe_marvin_10d0, Banken wie live geladen)

Probe erweitert: laedt jetzt wie die Live-Plattform (main.c pc_enemy_load:446-453) auch
loco/victim/**own**-Bank (`own_ok=1`, Bank 1 = {22,16,52,1,50,30,10,16,1,17,…}) — der
Vorlauf mass sonst Wraps der Container-Bank (20 statt 30). Log (Scratchpad
`probe_10d0_v2.txt`), finales Segment:

```
2190-2197  MARVIN mo=6 fr=34 st=4 s1=0        ; Gesten-Hold (RBJ-Clip 6, 35f) nach +0b52
2198       Plc_dest(slot=1 mode=0x05 dest=(7675,10700) flag=33) walk_active=1
2198-2202  mo=6 fr=34, rot 1770->1290         ; ALIGN: 5 Ticks EINGEFRORENE Gesten-Pose
                                              ;   pivotiert; Renderer posiert dabei
                                              ;   own-Bank Clip 6 (10f) mit cur=34
                                              ;   -> 34%10=4 = FALSCHE-BANK-Pose
2203       mo=5 fr=0                          ; Align fertig -> set_motion(5)
2203-2251  mo=5 +1/tick, Wrap bei 30          ; WALK-GAIT Clip 5 (Bank 1, 30f) bei
           pos 200/tick (7675,20625)->(7674,10829)   RUN-Speed 200/tick = GLEITEN
2252       walk=0, fr eingefroren (anim_freeze -> Frame-0-Pose)
2252-2307  56 Ticks Statue an der Tuer        ; SOLL: Ankunfts-Geste + Idle (s.u.)
2308       Pos_set(22300,0,-4300)             ; Marvin geparkt; Cut_chg(5)
```

Mittlerer Walk (Mode 4, +0x08a2): Align 2 Ticks (frozen mo=0 fr=19), dann Clip 5 bei
75/tick — Steady-State deckungsgleich mit dem Original (s. §2.2); der Walk wird nach
Sleep(20) von Pos_set/Cut_chg(14) abgeschnitten (Original identisch, +0x08aa-b0).

## 2. ORIGINAL

### 2.1 sub21-Endsequenz byte-genau (ROOM10D0.RDT, sub_scd-Sektion @Datei 0x120c)

Byte-true-Walk mit den `s_opcode_sizes` aus scd_vm.c (Offsets sektions-relativ):

```
+0b46  3f 00 08 00              Plc_motion(0,8)      ; Marvin Geste 8
+0b4a  43 00 80 00              Plc_flg(OR 0x80)     ; rueckwaerts
+0b4e  09 0a 14 00              Sleep(20)
+0b52  3f 00 06 00              Plc_motion(0,6)      ; Marvin Geste 6 (Hold bei fr34)
+0b56  09 0a 32 00              Sleep(50)
+0b5a  2e 02 00                 Work_set(2,0)=MARVIN
+0b5e  40 00 05 21 fb1d cc29    Plc_dest(0, 5, 0x21, 7675, 10700)   ; RUN raus
+0b66  41 00 …                  Plc_neck(0)          ; Release
+0b70  09 0a 0a 00              Sleep(10)
+0b74  2e 01 00                 Work_set(1,0)=LEON
+0b78  40 00 09 20 fb1d cc29    Plc_dest(0, 9, 0x20, 7675, 10700)   ; Leon TURN
+0b80  41 00 …                  Plc_neck(0)
+0b8a  09 0a 64 00              Sleep(100)           ; ← FIXER 100-Tick-Wait,
                                                     ;   KEIN Arrival-Poll!
+0b8e  2e 02 00                 Work_set(2,0)=MARVIN
+0b92  32 00 1c57 0000 34ef     Pos_set(22300,0,-4300)  ; Marvin parken
+0b9a  29 05                    Cut_chg(5)
+0b9c  22 02 07 00 / +0ba0 22 01 1b 00 / +0ba4 3c 01  ; (Op 0x22/0x3c, s. OFFEN)
+0ba6  2e 01 00                 Work_set(1,0)
+0baa  42                       Plc_ret
```

⇒ Marvins Run-Out dauert im Original exakt 110 Ticks (Sleep 10+100) und laeuft OHNE
Poll; Strecke 9925 units @200/tick ≈ 50 Ticks ⇒ **~55-60 Ticks lang steht Marvin
ANGEKOMMEN an der Tuer und spielt die Sub-6-Ankunfts-/Idle-Maschine**, bevor Pos_set
+ Cut_chg(5) ihn wegnehmen.

### 2.2 Die NPC-Walk-Subs (State-4-Tabelle @0x80076ca0, PSX.EXE raw-disasm)

Tabelle (24 Eintraege): [4]=0x80051148 [5]=0x80051484 [6]=0x800517f0 [7]=0x80051908
[8]=0x80051b00 [9]=0x80051cf8; [0x12]=[0x16]=0x80052508 (Blocked-Handler).

Alle Walk-Subs sind dieselbe 3-Phasen-FSM auf +0x6: 0 INIT → 1 TURN-TO-FACE → 2 WALK.
Param-Tabellen indexiert `byte[(type-0x40)*2]` (Paare pro Typ; Werte fuer 0x40):

| Sub | INIT: +0x8c Speed | INIT: +0x94 Clip | f314-Kanal | Phase-2-Sonderheit |
|---|---|---|---|---|
| 4 WALK | @0x80076c00 = **75** (@0x800511b4/bc) | **5** @0x800511dc | +0x170/+0x174 (@0x800512bc/c0, @0x80051358/5c) | Clip bleibt 5 |
| 5 RUN | @0x80076c80 = **200** (@0x800514f0/f8) | **5** @0x80051518 | +0x170/+0x174 (@0x80051628/2c, @0x80051714/18) | **Align→Phase 2 setzt +0x94=0 (`sb zero,148` @0x8005157c) = RUN-GAIT CLIP 0** (Bank 1, 22f), +0x95=0 @0x8005158c, +0x8f=7 @0x8005159c |
| 7 | @0x80076c20 = **70** (@0x8005193c/4c) | **1** @0x8005196c | **+0x84/+0x16c LOCO** (@0x80051a20/24) | Clip bleibt 1 |
| 8 | @0x80076c60 = **60** (@0x80051b34/44) | **1** @0x80051b64 | **+0x84/+0x16c LOCO** (@0x80051c18/1c) | Clip bleibt 1 |
| 9 TURN | — (kein Speed) | **5** @0x80051d3c | +0x170/+0x174 (@0x80051e9c/ea0) | kein pos_advance; aligned → +0x5=6 @0x80051dac |

Gemeinsame Mechanik (Sub 4 als Referenz; Sub 5/7/8 strukturgleich):

- **Phase 1 (TURN-TO-FACE)**: `arc_test(+0x1bc,+0x1be, 0x15e)` @0x80051214/0x80051550;
  aligned → +0x6=2 (@0x80051230/0x8005156c). NICHT aligned: Steer
  `FUN_8001aac4(dest, slew=@0x80076c41[(type-0x40)*2] = 96)` (@0x80051264/@0x800515d0)
  + Footstep-SE (Frame-Flag 0x4000 via +0x168 → FUN_80045630, @0x800512a8/@0x80051614)
  + **f314-Advance des INIT-Clips** (@0x800512c4/@0x80051630) — der NPC pivotiert
  ANIMIERT im Gait, KEINE Translation (kein pos_advance im Phase-1-Pfad).
- **Phase 2 (WALK)**: Steer mit slew=@0x80076c01[(type-0x40)*2] = **48** (@0x80051330-3c
  / @0x8005169c-a4 — auch der RUN slewt hier 48, nicht 72!) → **pos_advance
  FUN_800245d8(a0=0)** = +0x8c units entlang rot_y (@0x80051344/@0x800516b0) → Blocked-
  Probe FUN_8002d7d8 → +0x9f; !=0 → +0x5=0x12 (Walk @0x800513a4) bzw. 0x16 (Run
  @0x800516f4) = Blocked-Sub 0x80052508 → f314-Advance (@0x80051360/@0x8005171c) →
  **Arrival**: SquareRoot0(dx²+dz²) gegen +0x1bc/+0x1be **< 100** (Walk, slti
  @0x800513f8) / **< 300** (Run, @0x80051764) → SCD-Flag `FUN_8004ef90(0x800b1028,
  +0x1c3)` (@0x80051414/@0x80051780) → **`+0x5 = 6; +0x6 = 0`** (@0x80051428/38 bzw.
  @0x80051794/a4) = **Uebergabe an Sub 6 EVENT-REACH** (@0x800517f0: Clip 1 einmal →
  Clip-2-Idle-Loop, Bank 1) → Re-Arm nur mit `+0x1c4 & 4`: +0x5=Mode, +0x6=2
  (@0x8005145c/6c bzw. @0x800517c8/d8).

### 2.3 Param-Tabellen komplett (32 B je Tabelle, Typen 0x40-0x4f)

```
0x80076c00 (Sub4):  (75,48)x4 (70,48)x4 (75,48)x3 (50,48)x2 (70,48)x2 (50,48)
0x80076c20 (Sub7):  (70,48)x4 (65,48)x4 (70,48)x3 (35,48)x2 (65,48)x2 (35,48)
0x80076c40 (Turn):  (0,96)x11 (0,80)x2 (0,96)x2 (0,80)      ; +1 = Phase-1-Slew/Cone
0x80076c60 (Sub8):  (60,48)x4 (55,48)x4 (60,48)x3 (30,40)x2 (55,48)x2 (30,40)
0x80076c80 (Sub5):  (200,72)x4 (210,72)x4 (200,72)x3 (120,64)x2 (210,72)x2 (120,64)
```
(Die ODD-Bytes von 0x80076c80 (72/64) werden von Sub 5 Phase 1/2 NICHT gelesen —
Phase 2 liest 0x80076c01=48; Leser der 72er unbekannt, s. OFFEN.)

## 3. DIVERGENZ — wo der Port Position ohne passenden Gait aendert

Der Port faehrt NPC-Plc_dest ueber `re15_actor_step_walk` (actor_locomotion.c) statt
ueber die (bereits portierte!) State-4-Sub-VM: `op_plc_dest` (scd_vm.c:1911-1954) setzt
fuer Walk-Modes NUR `walk_active` — NICHT `state=4/+0x5=mode` (Original @0x80041c14-18)
— und `re15_npc_ai_tick` yieldet auf walk_active (enemy_ai_common.c:6602), d.h.
`re15_npc_sub_walk` laeuft fuer einen Plc_dest nie.

- **D1 (der GLEIT-Kern, Finale +0x0b5e):** Port spielt fuer Mode 5 den WALK-Gait
  **Clip 5** (Bank 1, 30f; walker setzt hart 5, actor_locomotion.c:232) bei RUN-Speed
  200/tick. Original: Phase 2 = **Clip 0** (Bank 1, **22f** RUN-Gait) via
  `sb zero,0x94` @0x8005157c. Beine im falschen (langsameren, kuerzeren) Zyklus bei
  9925 units Strecke = Gleiten. [Messung §1 Ticks 2203-2251]
- **D2 (Ankunfts-Statue):** Walker-Arrival friert den Clip ein (anim_freeze=1,
  actor_locomotion.c:332-338 → compute_actor_kf posiert Frame 0) und laesst +0x5
  stehen. Original: Arrival → **Sub 6 Event-Reach** (+0x5=6 @0x80051794; Clip 1 16f
  einmal → Clip 2 52f Idle). Im Finale ist das ein **56-Tick-Fenster** (Arrival 2252 →
  Pos_set 2308), in dem Marvin ORIGINAL ankommt-gestikuliert-atmet, im PORT als
  Frame-0-Statue steht. [Messung §1]
- **D3 (Align-Pose):** Walker-State-1 haelt die letzte Gesten-Pose gefroren (mo=6
  fr=34) und der walk_active-Render-Zweig (main.c:5661-5676) posiert dabei own-Bank
  Clip 6 (10f) mit cur=34 → `34%10=4` = falsche Bank+falscher Frame fuer 5 Ticks.
  Original: INIT setzt den Gait-Clip SOFORT (+0x94=5 @0x80051518) und Phase 1
  advanced ihn per f314 (@0x80051630) waehrend des Pivots.
- **D4 (Slew-Raten):** Port Mode-5-State-2-Slew = 0x48=72 (PLC_YAW_SLEW_RUN_S2,
  Player-Run-Wert); NPC-Original Phase 2 = **48** aus @0x80076c01 (@0x8005169c). Und
  Mode-8-Speed: Port 70 (Player-LAB_800311f0), NPC-Original **60** @0x80076c60.
  (Nicht 10D0-sichtbar: sub21 nutzt fuer Marvin nur Modes 4/5/6/9.)

Nicht divergent: Mode-4-Steady-State (Clip 5, 75/tick, +1/tick, Wrap 30) ✓;
Run-Speed 200 ✓ (@0x80076c80); Arrival-Radien 100/300 ✓; Leons Mode-9-Turn (Player-
Tabelle 0x80073e30[9]=0x80031360) ✓; die 110-Tick-Choreo (Sleeps) ✓.

## 4. FIX-PLAN (GLOBAL; kein Engine-Code in dieser Diagnose geaendert)

Strukturell byte-true (Option A, empfohlen): **NPC-Plc_dest auf die State-4-Sub-VM
routen** statt auf den Walker —
1. `op_plc_dest` (scd_vm.c): fuer NPC-Slots AUCH bei Walk-Modes `state=4,
   sub_state_1=mode, sub_state_2=0` (@0x80041c14-18), dest zusaetzlich in die
   Steer-Felder (+0x1bc/+0x1be) der Sub-VM spiegeln, `anim_flags=0` (@0x80041c4c),
   Re-Init-Guard (@0x80041bf8-c0c); **kein walk_active fuer NPC-Slots** (der Yield
   enemy_ai_common.c:6602 entfaellt dann fuer diese, die Sub-VM ist der Advancer —
   Walk-Subs 4/5/7/8/9 duerfen nicht yielden, analog zum in_motion_pose-Gate).
2. `re15_npc_sub_walk` (enemy_ai_common.c:6452) auf die volle Original-FSM heben:
   - Per-SUB-Speed-Tabellen `byte[(type-0x40)*2]`: Sub 4 @0x80076c00, Sub 5
     @0x80076c80, Sub 7 @0x80076c20, Sub 8 @0x80076c60 (Werte §2.3).
   - INIT-Clip je Sub: 4/5 → Clip 5 (@0x800511dc/@0x80051518); 7/8 → Clip 1
     (@0x8005196c/@0x80051b64, LOCO-Kanal — Kanal-Map + Render-Override fuer Subs
     7/8 existieren schon: enemy_ai_common.c:6331-6334, main.c:5646-5654).
   - Phase 1: arc 0x15e, Slew @0x80076c41=96, f314-Advance des Clips, KEINE
     Translation (@0x80051200-2cc).
   - **Sub-5-Align→Phase-2: Clip 0, Frame 0, frac 7** (@0x8005157c-9c) — der
     RUN-Gait.
   - Phase 2: Slew **48** @0x80076c01, `re15_dog_advance(e, speed)` (=pos_advance
     a0=0 @0x80051344/@0x800516b0), f314-Advance, Arrival SquareRoot0-Distanz
     **<100** (Sub 4/7/8 @0x800513f8) / **<300** (Sub 5 @0x80051764) →
     `re15_game_flag_set(5, +0x1c3, 1)` (FUN_8004ef90 @0x80051414/80) →
     **+0x5=6, +0x6=0** (@0x80051428/@0x80051794) → Re-Arm bei `anim_flags&4`
     (+0x5=mode, +0x6=2 @0x8005145c/@0x800517c8).
3. Renderer/Clock: fuer die Sub-VM-Walks greifen die vorhandenen State-4-Zweige
   (main.c:5663-5676 own-Bank fuer Subs {2,4,5,6,9}; re15_actor_clip_len:3019-3024) —
   die walk_active-Sonderfaelle in beiden koennen fuer NPC-Slots entfallen; ebenso der
   NPC-Advance-Hack im Walker (actor_locomotion.c:287-302) und der NPC-Zweig der
   Arrival-Freeze-Logik (:332-338).
4. Player-Plc_dest bleibt auf dem Walker (Player-Tabelle 0x80073e30: 4=0x80030af0,
   5=0x80030d28, 9=0x80031360 — separat, bereits byte-true portiert). Elliot (0x47,
   ROOM1170-Ausnahme, PLD-Bank) VOR der Umstellung gegen seinen Savestate messen —
   sein Sonderpfad (Sentinel 105/100) haengt am Walker (marvin_10d0.md O3-Familie).

Verifikation danach: `probe_marvin_10d0.exe` — erwartet im Finale: (a) ab +0b5e
`st=4 s1=5 s2=0→1→2`, Phase 1 advanced Clip 5, (b) Phase 2 `mo=0`, Wrap 22, pos
200/tick, (c) Arrival ~Tick 50: `s1=6`, Clip 1 (16f) einmal, dann Clip-2-Loop bis
Pos_set. Danach Live-gdigrab (Skill re15-port-visual-verify) gegen DuckStation
(re15-room-capture ROOM10D0).

## 5. Warum es nach Fix #2/#3 immer noch glitt

3f5310cc gab dem Walker einen Frame-Advancer (Clip 5) und 2194c41f die richtige Bank —
beides macht den **Mode-4-WALK** korrekt (Clip 5 IST der Walk-Gait). Der ABGANG ist
aber **Mode 5 = RUN**: dort ist Clip 5 nur der Phase-1-Pivot-Gait; der eigentliche
Lauf-Gait ist Clip 0 (22f), gesetzt exakt beim Align-Abschluss (@0x8005157c) — ein
Mechanismus, den nur das Sub-5-Disasm zeigt, kein Bank-/Advancer-Fix erreichen kann.
Dazu die zwei Statue-Fenster (Align-Hold vorn, anim_freeze hinten), die den
"schwebt"-Eindruck rahmen.

## 6. OFFEN (ehrlich)

1. **Blocked-Subs 0x12/0x16 = FUN_80052508** (Tabelle[18]/[22]) + Probe FUN_8002d7d8
   (ruft room_coll 0x8001c6e8): nicht RE'd/portiert — in 10D0 ist der Weg frei; fuer
   Raeume mit Hindernis im NPC-Pfad noetig.
2. **Odd-Bytes @0x80076c81 (72/64):** kein Leser in Subs 4/5/7/8/9 gefunden
   (Phase 2 liest 0x80076c01) — Konsument unbekannt.
3. **f314-Schrittrate a3=0x200 = +1 Frame/Tick** ist aus allen bisherigen Savestate-
   Verifikationen uebernommen, nicht in DIESER Probe gegen Hardware gemessen.
4. **Bank-1-Clip 0 = visuell ein Lauf-Zyklus** ist aus dem Mechanismus geschlossen
   (der RUN-Sub setzt ihn); Poseninhalt nicht gerendert verglichen — Live-gdigrab
   nach dem Fix.
5. **Ops 0x22 (`22 02 07 00`/`22 01 1b 00`) und 0x3c (`3c 01`)** am Szenenende nicht
   dekodiert (nach Cut_chg(5), vor Plc_ret).
6. Ob der ORIGINAL-Executor Marvins Sub-5 waehrend der 110 Ticks im Halbtakt gated:
   +0x1c4 wird von Plc_dest auf 0 gesetzt (@0x80041c4c) → Gate
   `!(0x10&&0x20)` disp. jeden Tick — Halbtakt traefe nur mit extern gesetztem 0x10.
7. DuckStation-Gegenmessung des Finales (Frame-genauer Vergleich) steht aus — die
   Choreo-Dauer (110 Ticks) ist aus dem SCD belegt, nicht live gemessen.

## 7. Artefakte

- Probe: `re15_port/tests/unit/probe_marvin_10d0.c` — erweitert um den Live-Bank-Load
  (loco/victim/own wie main.c:446-453); kein Engine-Code geaendert.
- Logs: Scratchpad `probe_10d0_full.txt` (Container-Bank-Lauf), `probe_10d0_v2.txt`
  (Banken wie live; Kernzeilen in §1).
- SCD-Walker: Scratchpad `scd_walk21.py` (byte-true `s_opcode_sizes`).
- Disasm: `re15_disasm.py` gegen PSX.EXE — Subs 4 (0x80051148-0x80051470),
  5 (0x80051484-0x800517ec), 7 (0x80051908…), 8 (0x80051b00…), 9 (0x80051cf8-0x80051eb4),
  Tabellen 0x80076c00/20/40/60/80, Sub-Tabelle @0x80076ca0 (24 Eintraege).
