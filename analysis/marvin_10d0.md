# ROOM10D0 — Marvin-Cutscene: RE-Dossier (2026-08-03)

**Nutzer-Report:** "Die Cutscene mit Marvin ist falsch: Leons Kopf ist komisch verdreht,
Marvins Animationen sind falsch."

**Ergebnis in einem Satz:** Der Port verfehlt VIER Original-Mechanismen, die alle GLOBAL
(nicht 10D0-spezifisch) sind: (1) `Plc_neck` zielt im Port immer auf den Player statt auf
die Work-Entity, (2) die RBJ-Record→Entity-Bindung ist im Original DATENGETRIEBEN über eine
Marker-Bitmaske (FUN_8001b3f8) — der Port hat stattdessen eine handgepflegte Raum-Tabelle,
in der 10D0 fehlt, (3) `Plc_dest` Mode 6 ist im Original der Executor-Sub 6 (EVENT-REACH),
kein Walk — der Port lässt beide Akteure ewig Richtung Weltursprung (0,0) "laufen" und
friert damit Marvins gesamten Gesprächs-Animationslauf ein, (4) `Plc_motion(2,…)`
(Sub-State 2) ist im Original ein Play-once+Hold-FSM, im Port ein Idle-Loop.

Dynamisch reproduziert mit `re15_port/tests/unit/probe_marvin_10d0.c` (Diagnose-Probe,
kein ctest; Log-Auszüge unten).

---

## 1. MESSEN — ROOM10D0.RDT seziert

Datei: `re15_port/shared_assets/PSX/STAGE1/ROOM10D0.RDT` (286320 B). Header:
nSprite=0, nCut=15, nOmodel=2. Relevante Sektionen (Datei-Offsets):

| Sektion | Offset | Größe |
|---|---|---|
| main_scd | 0x001020 | 0x1ec |
| sub_scd | 0x00120c | 0xc3c (23 Subs) |
| animation (RBJ) | 0x002560 | 0xc458 |

### 1.1 Spawns (sub_scd sub00, Sektions-Offset +0x2e; Datei +0x123a ff.)

Erste Raum-Betretung (`Ck(4,247)==0`):

- `+0x004a` (Datei 0x1256): `44 00 40 40 00 00 00 ff 8f16 0000 495c 0000 7604 0000`
  = **Sce_em_set slot=0 type=0x40 (MARVIN) behavior=0x40 pos=(5775,0,23625) yaw=1142**,
  kill-flag 0xff (kein Persist-Gate). behavior-Bit 0x40 = stationär → INIT schickt ihn in
  den Shared-Executor Sub 6 (Event-Reach), vgl. Port `enemy_ai_common.c` @0x8011c860.
- `+0x005e` (Datei 0x126a): `44 01 10 0e …` = Zombie type=0x10 slot=1 behavior=0x0e
  pos=(5878,0,25694) — Spawn-Pose-Decoder sel 0x0E → Clip 0x2A (liegend).
- `+0x0036`: `2c 12 03 31 0000 5c12 c84b 2003 2003 ff00 18 15 0000` = **AOT slot 0x12**,
  Rechteck x=4700 z=19400 w=800 d=800, Event-Payload `ff 18 15` → **Evt_exec sub 0x15
  (= sub21) = die Marvin-Cutscene**, getriggert beim Betreten der Zone.

Wiederbetreten (`Ck(4,247)==1`, von sub21 @+0x07e0 gesetzt): stattdessen
`44 00 10 02` = ein stehender Zombie type 0x10 an Marvins Stelle (Marvin ist dann weg —
"zombifiziert"-Ersatz), plus Sca_id/Sca_floor-Umbau.

### 1.2 Die Cutscene (sub_scd **sub21**, Sektions-Offset +0x07d2)

Choreographie (Auszug; WS = Work_set: kind 2=NPC-Slot, 1=Player; alle Offsets
Sektions-relativ zu 0x120c):

```
+07d2 Message_on(0x0b, 0xff80)            ; Intro-Text (Bit 0x2000 = Modal-Freeze)
+07e0 Set(4,247)=1                        ; "Szene gesehen"
+0816 Cut_chg(13);  +0818 Evt_exec(sub 0x16=sub22: Prop-0-Rolltor-Fahrt)
+081c WS(2,0)=MARVIN
+0820   Plc_neck(1, 5875,0,19825, 100)    ; Marvin schaut zu Leons Ankunftspunkt
+082a   Plc_motion(2, 6, 0)               ; Sub-State 2, Clip 6 (Winken/Rufen)
+083a WS(1,0)=LEON
+083e   Plc_dest(0, 9,0x20, 5775,23625)   ; Mode 9 = TURN zu Marvin
+0846   Plc_neck(1, 5775,0,23625, 100)    ; Leon schaut zu Marvin
+0858   Plc_motion(0,0,0)
+0866 WS(2,0): Plc_motion(2,6,0); Plc_flg(OR 0x80)   ; Clip 6 rückwärts
+0876 WS(1,0): Plc_motion(0,0,0); Plc_flg(OR 0x80)
+088a WS(2,0): Plc_motion(0,0,0) …
+08a2   Plc_dest(0, 4,0x21, 7675,20625)   ; MARVIN geht zu Leon (Mode 4 WALK)
+08ae Cut_chg(14)
+08b0   Pos_set(7675,0,20625); Dir_set(0,1770,0)     ; Marvin-Schnitt-Snap
+08c0   Plc_neck(1, 5975,0,19825, 100)    ; ⚠ Ziel = LEONS Position (Marvin→Leon-Blick)
+08ca   Plc_dest(0, 6,0x3f, 0,0)          ; ⚠ Mode 6 = EVENT-REACH (dest-Felder egal)
+08d2   Plc_motion(0,8,0)
+08d6 WS(1,0): Pos_set(5975,0,19825); Dir_set(0,3826,0)
+08ea   Plc_neck(1, 7675,0,20625, 100)    ; Leon schaut zu Marvin
+08f4   Plc_dest(0, 6,0x3f, 0,0)          ; ⚠ auch Leon: Mode 6
… Dialog-Schleife: abwechselnd WS(1/2) + Message_on + Plc_motion-Gesten
  LEON-Clips:   0,1,4,5,8   MARVIN-Clips: 0,1,3,4,5,6,8
  Muster: Plc_motion(0,X); Sleep; Plc_motion(0,X); Plc_flg(OR 0x80); Sleep  ; vor+zurück
+09a8 WS(1): Plc_neck(2, 0,0,300, 0x0A00) ; ⚠ Mode 2 = RELATIV (Pitch 300 = Nicken)
+09b6         Plc_neck(4, 3,0,0, 100)     ; ⚠ Mode 4 = Auto-Release (Countdown 3)
+09e2/+09f0 dieselben zwei Modes auf MARVIN
+0b46 WS(2): Plc_motion(0,8,0)+Flg(0x80); Plc_motion(0,6,0)
+0b5e   Plc_dest(0, 5,0x21, 7675,10700)   ; Marvin RENNT raus (Mode 5)
+0b66   Plc_neck(0, …)                    ; Release
+0b78 WS(1): Plc_dest(0, 9,0x20, 7675,10700); Plc_neck(0,…)  ; Leon dreht zur Tür
+0b92 WS(2): Pos_set(22300,0,-4300)       ; Marvin außer Sicht geparkt
+0b9a Cut_chg(5); +0baa Plc_ret
```

### 1.3 Der RBJ-Anhang (animation-Sektion, Datei 0x2560)

Header: `total_length=0xab38, record_count=2`. Record-Trailer @0x2560+0xab38 (= Datei
0xd098): 2 Paare `(EMR_prefix, EDD_off)` = REC0 (0x8, 0x5104), REC1 (0x55a0, 0xa69c).

| Record | Marker (u32 @prefix) | Bones | KF-Größe | Keyframes | Clips (Frames) |
|---|---|---|---|---|---|
| 0 | **0x1** (Datei 0x2568) | 15 | 80 B | 259 | 10: 20,30,30,20,30,25,35,50,24,20 |
| 1 | **0x2** (Datei 0x7b00) | 15 | 80 B | 259 | 10: identische EDD-Tabelle, EIGENER KF-Pool |

Vergleich (Marker-Schema game-weit konsistent):
- ROOM1150: REC0 marker=1 (15 Clips), REC1 marker=2 (7 Clips → Irons em45)
- ROOM1170: REC0 marker=1 (26 Clips), REC1 marker=2 (25 Clips → **Elliot**)
- ROOM10D0: REC0 marker=1 → Leon, REC1 marker=2 → **Marvin (Enemy-Entity 0)**

Nach dem Trailer folgen noch 0x1910 Bytes (`6c020000 00000000 02000000 38 18 f8 0f 170…`)
— Struktur NICHT identifiziert (OFFEN, siehe §6).

---

## 2. ORIGINAL — die steuernden Mechanismen (Disasm-Belege)

### 2.1 RBJ-Binder FUN_8001b3f8 — Marker-Bitmaske → Entity (DATENGETRIEBEN)

Aufrufer: Raum-Setup `FUN_800396fc` @0x80039a08 (`jal FUN_8001b3f8`), NACH `FUN_8003ef6c`
(SCD-Init/Spawns) — d.h. der Binder ÜBERSCHREIBT die von Sce_em_set gesetzten Anim-Kanäle.

```
8001b404 lw  a2,0x5c(v0)          ; RDT+0x5C = animation/RBJ-Sektion
8001b41c lw  v0,0x0(a2)           ; total_length → Trailer
8001b420 lbu t2,0x4(a2)           ; record_count
8001b438 lw  a0,0x0(v1)           ; MARKER-Wort des Records
8001b440 andi v0,a0,0x1           ; Bit 0 = PLAYER
8001b450 sw  v0,0x0(t3)  => DAT_800acbd4   ; Player+0x170 = EMR (Prefix+4)
8001b460 sw  v0,0x4(t3)  => DAT_800acbd8   ; Player+0x174 = EDD
8001b470 andi v0,a0,0x1 / srl a0,1         ; weitere Bits: Bit (1+i) = Enemy-Entity i
8001b480-90 v1 = ((i<<5 - i)<<2 + i)<<2    ; = i * 0x1F4 (500) = Enemy-Stride
8001b4a0 sw  t1,(DAT_800acdac + i*0x1F4)   ; enemy[i]+0x170 = Record-EMR
8001b4bc sw  v0,(DAT_800acdb0 + i*0x1F4)   ; enemy[i]+0x174 = Record-EDD
```

→ In 10D0: REC0 (marker 1) → Leon; REC1 (marker 2) → enemy[0] = **Marvin**. Es gibt im
Original KEINE Raum-Tabelle — die Bindung steht in den RDT-Daten selbst.

### 2.2 Plc_motion (0x3F) @0x80041b90 — Ziel = Work-Entity, pc[1] = Executor-Sub

```
80041b98 lbu a2,0x1(pc)     ; pc[1]
80041b9c lhu a1,0x2(pc)     ; u16 LE pc[2..3]
80041ba0 lw  v0,0x154(a0)   ; WORK-ENTITY (thread+0x154)
80041ba8 sb  a1,0x94(v0)    ; +0x94 = Clip (Low-Byte)
80041bb0 sb  4,0x4(v0)      ; +0x4  = State 4 (Executor)
80041bb4/b8 sb 0,0x6/0x7    ; Phasen-Reset
80041bbc/c0 sh 0,0x1c8/0x1ca
80041bc4 sb  a2,0x5(v0)     ; +0x5  = pc[1] = EXECUTOR-SUB  ← Plc_motion(2,6,0) → Sub 2
80041bc8 sh  a1,0x1c4(v0)   ; +0x1c4 = pc[3] (anim_flags)
```

### 2.3 State-4-Executor: Sub-Tabellen (Player @0x80073e30, NPC @0x80076ca0)

Beide aus PSX.EXE dekodiert (Datei-Offset = addr−0x80010000+0x800):

| Sub | Player @0x80073e30 | NPC @0x80076ca0 | Bedeutung |
|---|---|---|---|
| 0 | 0x80050cb8 | 0x80050cb8 | Motion-FSM play+hold/loop |
| 1 | 0x80050ddc | 0x80050ddc | Pose-FSM-Variante |
| 2 | 0x80050f00 | 0x80050f00 | Pose-FSM-Variante (s.u.) |
| 3 | 0x80051024 | 0x80051024 | Pose-FSM-Variante |
| 4 | 0x80030af0 (WALK) | 0x80051148 (NPC-WALK) | walk-to-dest |
| 5 | 0x80030d28 (RUN) | 0x80051484 | run-to-dest |
| 6 | **0x800517f0** | **0x800517f0** | **EVENT-REACH: Clip 1 einmal → Idle-Clip-2-Loop** |
| 7/8 | 0x80031080/0x800311f0 | 0x80051908/0x80051b00 | Walk-Varianten |
| 9 | 0x80031360 (TURN) | 0x80051cf8 | turn-in-place |

**Sub 2 @0x80050f00** (das, was `Plc_motion(2,6,0)` auf Marvin startet):
```
80050f34 sb 1,0x6(a0)       ; Phase 0→1
80050f44 sb 7,0x8f          ; Crossfade-Seed 7
80050f54 sb 0,0x95          ; Frame 0
80050f64-78 (+0x1c4&0x40) → +0x8f=0    ; No-Blend-Bit
80050f88 lw a0,0x170(v0)    ; ← ENTITY-EIGENER EMR-Kanal (in 10D0 = RBJ REC1!)
80050f90 lw a1,0x174(v0)    ; ← ENTITY-EIGENER EDD-Kanal
80050f94 jal FUN_8001f314   ; anim_set-Stepper, a2 = (+0x1c4>>7)&1 = REVERSE-Bit
80050fb8 (+0x1c4&0x8) → zweiter FUN_8001f314-Call    ; Double-Step-Bit
80050fec sb 2,0x6           ; Clip-Ende → Phase 2 = HOLD
80051004-10 (+0x1c4&0x4) → Phase 1     ; LOOP nur mit Bit 4
```
→ **Play once + HOLD** (anim_flags=0), Rückwärts über Plc_flg-OR-0x80. Kein Idle-Loop.

### 2.4 Plc_dest (0x40) @0x80041be4 — Mode = Executor-Sub, Mode 6 ≠ Walk

```
80041be4 lw a1,0x154(a0)          ; WORK-ENTITY
80041bf8-c0c Re-Init-Guard: (+0x1c4&4 && +0x5==mode) → State-Init überspringen
80041c14 sb 4,0x4(a1)             ; State 4
80041c18 sb a2,0x5(a1)            ; +0x5 = MODE  ← Mode 6 → Executor-Sub 6!
80041c24 sb v1,0x1c3(a1)          ; Arrival-Flag-Index (pc[3])
80041c38/58 sh dest → +0x1bc/+0x1be
80041c4c sh 0,0x1c4               ; anim_flags = 0
80041c50-54 sltiu type(+0x8),0x10 ; Player (<0x10) vs NPC (>=0x10) Clip-Preselect:
  caseD_4..9 @0x80041c8c-0x80041d3c: Player-Tabellen 0x80073ea5/0x80073f25/0x80073ec5/
    0x80073f05/0x80073ee5 → +0x1c8;  Mode 6 = KEIN Preselect (Join-Label)
  NPC-Pfad @0x80041d40: eigener Switch (Tabelle @0x80010de4 → 0x80041d70…)
```
→ `Plc_dest(0,6,0x3f,0,0)` heißt: **State 4 / Sub 6 = Event-Reach** (Clip 1 einmal,
dann Idle-Clip-2-Loop, @0x800517f0). Die dest-Koordinaten (0,0) werden von Sub 6 nie
gelesen. KEIN Walker, KEIN Ziel-Anlaufen.

### 2.5 Plc_neck (0x41) @0x80041e98 — Ziel = Work-Entity, 5 Modi

```
80041e9c lw v1,0x154(a0)     ; ← WORK-ENTITY (nicht der Player!)
80041ea8 sb 0x80,0x1b8(v1)   ; Neck-Flags Basis
  Mode 0 → |0x12 (Release/Auto-Reset)   Mode 1 → |0x04 (WELT-PUNKT)
  Mode 2 → |0x08 (RELATIV-Winkel)       Mode 3 → |0x2a   Mode 4 → |0x58 (Auto-Release)
80041f28/3c/50 sh pc[2/4/6] → +0x160/+0x162/+0x164
80041f68/6c sb pc[8]/pc[9] → +0x9e/+0x9f (Yaw-/Pitch-Step)
```

Konsument = Head-Look **FUN_80037358** (Caller FUN_80031c44 @0x80031d78), pro Frame:
- Default-Ziel (ohne Bit 0x04): Kopf-Bone der Entity in `+0x1a8` (Entity-Tracking).
- Bit 0x04: Welt-Punkt aus +0x160/162/164 (→ FUN_8003790c atan2+SquareRoot0).
- Bit 0x08: RELATIVE Winkel: Yaw-Offset +0x162, Pitch-Ziel +0x164 (Mode 2: Pitch 300 =
  Kopfnicken, Yaw-Step 0 / Pitch-Step 10 aus 0x0A00).
- Bit 0x40/0x20 (Mode 4): Countdown in +0x160 (hier 3); bei Ablauf `+0x1b8 = 0x12`
  (Auto-Release). Bit 0x10: Pitch→0. Bit 0x80: Steps aus +0x9e/+0x9f.
- Ausgabe: Bone-Yaw/Pitch (+0x62/+0x64 des Kopf-Bones, Index aus Entity+0x1b9) mit
  Slew-Clamp; RotMatrix; danach Restore (transienter Pose-Add).

---

## 3. PORT-IST — vier Divergenzen (statisch + dynamisch belegt)

Dynamik: `re15_port/tests/unit/probe_marvin_10d0.c` — lädt ROOM10D0.RDT, `scd_room_reenter`,
startet sub21 direkt (deterministisch statt AOT-Zone), tickt `scd_vm_tick` +
`re15_actor_step_all_walkers` + `re15_actors_anim_advance` + `re15_enemy_ai_run_all(0)`,
CROSS-Edge (0x8000) pro Tick als Dialog-Confirm. Spawn-Check: Marvin slot1 type=0x40
grid=0x40 (5775,0,23625) rot=1142 ✓, Zombie slot2 type=0x10 mo=42 (Clip 0x2A) ✓.

### D1 — `op_plc_neck` zielt IMMER auf den Player (scd_vm.c:1963)

`re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];` — das Original liest die
Work-Entity (`lw v1,0x154(a0)` @0x80041e9c). Alle 4 Marvin-Plc_necks landen auf Leon:

```
tick   31  LEON neck=0x84 tgt=(5875,0,19825)   ← Marvins +0820 (soll: Marvin→Leon-Punkt)
tick  573  LEON neck=0x84 tgt=(5850,0,25100)   ← Marvins +0918 — 252 Ticks lang schaut
                                                 Leon zur Zombie-Leiche statt zu Marvin
tick 1137  LEON neck=0x88 tgt=(0,0,300)        ← Marvins Mode-2 (Nicken) fehlgeleitet
tick 1167  LEON neck=0xd8 tgt=(3,0,0)          ← Marvins Mode-4; bleibt bis tick 2198
                                                 stehen und LÖSCHT Leons eigenes
                                                 Look-at-Marvin (0x84/(7675,20625))
                                                 für die GESAMTE zweite Szenenhälfte
```
Bei +08c0 ist Marvins Neck-Ziel (5975,0,19825) exakt Leons Position — auf Leon fehlgeroutet
ist das ein atan2 über ~0-Distanz (Selbst-Blick); im selben Tick überschreibt Leons eigenes
+08ea das Ziel wieder (Log zeigt das Netto-Resultat), die 0x88/0xd8-Fenster oben bleiben
aber STEHEN → **"Leons Kopf ist komisch verdreht"**.

Zusatz: die Port-Neck-FSM (skeleton_common.c:328) läuft NUR für den Player
(`bact == &g_actors[RE15_ACTOR_SLOT_PLAYER]`) — NPC-Head-Look existiert nicht, Marvin
schaut nie zu Leon. Und `active = 0x80 && 0x04` behandelt Mode 2 (relativ, Bit 0x08)
und Mode 4 (0x58) als Release — Leons Kopfnicken (+09a8, Pitch 300) entfällt.

### D2 — RBJ REC1 wird nie an Marvin gebunden (enemy_common.c:89)

`s_room_rbj_enemy[] = { { 0x1150, 0x45, 1 } }` — handgepflegt, 10D0 fehlt. Der Port
spielt Marvins Gesten aus der EM040-eigenen Bank; der NPC-Executor nimmt Cliplängen aus
`s_irons_clip_len` = {34,32,50,26,20,20,50,1,1,…} (enemy_ai_common.c:6162) statt aus dem
gebundenen EDD-Kanal (RBJ REC1: {20,30,30,20,30,25,35,50,24,20}) — z.B. Clip 6: 50 statt
35 Frames, Clip 8: **1 statt 24** Frames. Im Original ist die Länge IMMER die des
Entity-eigenen +0x174-Kanals (FUN_8001f314-Call @0x80050f94 mit +0x170/+0x174).
Das Original ist datengetrieben (Marker-Bitmaske, §2.1) — jede Raum-Tabelle ist
konzeptionell falsch. (Gleiches Schema erklärt ROOM1150 REC1→Irons und ROOM1170
REC1→**Elliot** — der Port legt dort derzeit REC0 auf Elliot; siehe OFFEN §6.)

### D3 — `Plc_dest` Mode 6 als Walk nach (0,0): Marvin friert ein + dreht sich weg

op_plc_dest (scd_vm.c:1861) setzt für JEDEN Mode `walk_active=1` und lässt
`re15_actor_step_walk` das Ziel anlaufen. Mode 6 hat `mode_to_speed()=0` (actor_locomotion.c:163)
→ Distanz zu (0,0) fällt nie unter die Schwelle → **walk_active hängt für immer**, und der
Walker slewt die Facing Richtung Weltursprung. Messung:

```
tick  452  MARVIN walk=1/6  rot 1674→1255 (statt Dir_set 1770; atan2 nach (0,0))
tick  452..2148: JEDE Marvin-Geste (mo 8,1,4,3,0,6 …) bleibt bei fr=0 —
  re15_npc_ai_tick yieldet auf walk_active (enemy_ai_common.c:6406) und der
  State-4-Sub-VM (einziger Frame-Advancer) läuft nie → MARVIN IST 1700 TICKS
  LANG EINE EINGEFRORENE PUPPE = "Marvins Animationen sind falsch".
tick  452  LEON ebenfalls walk=1/6 bis 2198 (sein +08f4).
```
Original: Mode 6 = State 4/Sub 6 EVENT-REACH @0x800517f0 (Clip 1 einmal → Idle-Clip-2-Loop,
dest wird nicht gelesen; §2.4) — beide Akteure "setzen sich" in den Gesprächs-Idle.

Nebenbefund: der Port-Walker stempelt bei Mode-4/5-NPC-Walks die PLAYER-Sentinels 105/100
auf Marvin (`tick 434 mo=105`, `tick 2198 mo=100`) — das Original preselektiert den
NPC-Clip über den Typ-Switch @0x80041d40 (+0x1c8) und Sub-Walk-INIT setzt Clip 5
(@0x80051198).

### D4 — `Plc_motion(2,6,0)`: Sub 2 wird als Idle-LOOP dispatcht

op_plc_motion routet korrekt auf die Work-Entity und setzt `sub_state_1=2` (byte-true),
aber `re15_npc_sub_dispatch` (enemy_ai_common.c:6314) schickt Sub 1–3 in
`re15_npc_sub_idle` = Endlos-Loop des Clips. Original-Sub-2 @0x80050f00 = play once +
HOLD (Loop NUR mit +0x1c4&0x4; Reverse-Bit 0x80; Double-Step-Bit 0x8; §2.3).
Messung: tick 161 ff. — Marvin loopt Clip 6 mit Wrap bei 50 (falsche Länge, s. D2)
statt einmal 35 Frames zu spielen und zu halten.

---

## 4. FIX-PLAN (alle vier Fixes GLOBAL; kein Engine-Code in dieser Diagnose geändert)

1. **[GLOBAL] op_plc_neck → Work-Entity** (`lw v1,0x154(a0)` @0x80041e9c): Neck-Felder
   auf `g_actors[work_slot]` schreiben (Fallback Player wie Plc_motion/Plc_dest). Die
   Head-Look-FSM (skeleton_common.c) pro Akteur mit gesetzten Neck-Flags ausführen
   (NPC-Skelette der 15-Bone-Humanoiden: Kopf-Bone-Index aus Entity+0x1b9 — Quelle der
   Initialisierung noch offen, §6; bis dahin Bone 8 für 15-Bone-Skelette, wie Player).
   Modi byte-true: Bit 0x04 Welt-Punkt (vorhanden), Bit 0x08 RELATIV (+0x162 Yaw-Offset,
   +0x164 Pitch-Ziel), Bits 0x40/0x20/0x10 Auto-Release-Countdown (+0x160), FUN_80037358.
2. **[GLOBAL] RBJ-Marker-Binder statt Raum-Tabelle** (FUN_8001b3f8, §2.1): beim
   Cinematic-Apply für jeden Record das Marker-Wort lesen; Bit 0 → Player-Bank,
   Bit (1+i) → `g_actors[1+i]`-Bank (Skel-Keyframes+EDD ersetzen, Mesh/Bind behalten).
   `s_room_rbj_enemy` entfällt; ROOM1150 (REC1 marker=2 → enemy0=em45) bleibt identisch.
   ROOM1170 (REC1 marker=2 → Elliot) vor Umstellung gegen Savestate messen (§6!).
3. **[GLOBAL] Plc_dest = State-4-Sub-Dispatch** (@0x80041c14-18): Mode → `state=4,
   sub_state_1=mode` mit Re-Init-Guard (@0x80041bf8-c0c); Sub 6 = Event-Reach (vorhanden:
   `re15_npc_sub_event_reach`), KEIN walk_active; NPC-Walk-Clips aus dem Typ-Preselect
   (+0x1c8, Tabelle @0x80010de4-Pfad) statt Player-Sentinels; Player-Mode-6 = derselbe
   Sub @0x800517f0 auf der Player-Bank.
4. **[GLOBAL] Executor-Subs 1–3 = Pose-FSMs** (0x80050ddc/0x80050f00/0x80051024): wie
   Sub 0 play+hold/loop (inkl. Reverse 0x80 und Double-Step 0x8 @0x80050fb8), statt
   `re15_npc_sub_idle`-Loop. Cliplänge IMMER aus dem gebundenen Anim-Kanal der Entity
   (ersetzt `s_irons_clip_len` UND den Elliot-Sonderfall `s_npc_elliot_anim`).

Verifikation danach: `probe_marvin_10d0.exe` — erwartet: (a) Marvins neck-Felder gesetzt,
Leons nur von Leons Kommandos; (b) ab dem Mode-6-Punkt `st=4 s1=6` statt walk=1/6, Frames
laufen weiter; (c) Marvin-Facing 1770 nach Dir_set; (d) Clip-6-Hold nach 35 Frames.
Danach Live-Verifikation per gdigrab (Skill re15-port-visual-verify) gegen DuckStation
(re15-room-capture ROOM10D0).

---

## 5. Warum genau die zwei Nutzer-Symptome

- **"Leons Kopf ist komisch verdreht":** D1 — Marvins Neck-Kommandos treffen Leons Kopf
  (Fenster t573–825 Blick zur Leiche; ab t1167 löscht Marvins Mode-4 Leons Look-at-Marvin
  für die halbe Szene; bei +08c0 transient Selbst-Blick-Ziel), plus fehlender Mode-2-Pfad.
- **"Marvins Animationen sind falsch":** D3 (ab der Mitte der Szene 1700 Ticks komplett
  eingefroren + weggedreht) über D2 (falsche Bank/Cliplängen: em40-eigene Clips statt
  RBJ-REC1-Gesten) und D4 (Loop statt Hold am Szenenbeginn).

## 6. OFFEN (ehrlich)

1. **Entity+0x1b9 (Kopf-Bone-Index) + Neck-Clamps +0x9a/+0x9c** — Initialisierung nicht
   RE'd (Modell-Neck-Daten). Für 15-Bone-Humanoide beobachtet Bone 8 (Player); für NPC
   byte-true noch zu belegen (Savestate: enemy[0]+0x1b9 in 10D0 lesen).
2. **Executor-Subs 1 und 3** (0x80050ddc/0x80051024) — Unterschiede zu Sub 0/2 (vermutl.
   Step-Raten/a3-Param) nicht einzeln disassembliert.
3. **ROOM1170/Elliot:** Marker sagt REC1→Elliot; der Port nutzt REC0 mit der Begründung
   "Clip 25 nur in REC0" (emd_common.c:377-387). Wenn sub02 Clip 25 auf Elliot ruft und
   das Original REC1 (25 Clips, 0..24) gebunden hat, liest der ungebremste EDD-Index
   (@0x8001f314, kein Clamp) hinter der Tabelle — was auf Hardware dabei rauskommt, ist
   NICHT gemessen. Vor Umstellung auf den Marker-Binder dort per Savestate messen.
4. **0x1910 Extra-Bytes nach dem RBJ-Trailer** (Datei 0xd0a8-0xe9b7, Struktur
   `6c020000 … 02000000 38/18/f8/0f/170…`) — Zweck unbekannt (evtl. Kamera-/Neck-Daten
   der Szene). Kein Konsument identifiziert (FUN_8001b3f8 liest nur Trailer+Records).
5. **Dialog-Freeze-Fenster:** ob der Original-Executor während `Message_on` mit
   Flag-Bit 0x2000 (nur Message 0x0b hier) pausiert, wurde nicht separat gemessen —
   der Port-Gate (DAT_800aca40&0x20000000) ist per früherem RE verdrahtet.
6. **Plc_motion pc[1] beim PLAYER:** landet original ebenfalls in +0x5 (Player-Sub-Tabelle
   0x80073e30 hat dieselben Pose-Subs 0–3) — der Port nutzt pc[1] zusätzlich als
   PL00-vs-RBJ-Bank-Selektor (`anim_use_pl00`, scd_vm.c:1770). In 10D0 sind alle
   Leon-Plc_motions (0,…) → keine Divergenz hier; game-weit ungeprüft.

## 7. Artefakte

- Probe: `re15_port/tests/unit/probe_marvin_10d0.c` (+ CMake-Eintrag; kein ctest).
- Voll-Log des Laufs: Scratchpad `probe_10d0_out.txt` (Session-lokal; Kernzeilen oben).
- SCD-Disasm-Tooling: Session-Skript (Opcode-Längen = byte-true `s_opcode_sizes`
  aus scd_vm.c, Jump-Table @0x800744a8).
