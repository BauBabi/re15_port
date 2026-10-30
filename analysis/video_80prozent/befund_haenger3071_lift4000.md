# Bug 1 (Elza, STAGE3 Birkin-Szene haengt) + Bug 2 (ROOM4000/4001 Lift-NPCs)

Exe: scratchpad/build/platform/pc/re15_pc (master-Stand, nicht neu gebaut). Laeufe unter
scratchpad/agent_c/r1 (Bug 1), r3/r4 (Bug 2). Quelle der SCD-Dumps: `re15_port/tools/scd_dump_room.py`.

---------------------------------------------------------------------------------------------------
## BUG 1 — ROOM3071 (Elza, "307 WAREHOUSE"), Birkin-Szene sub02: SCD wartet ewig auf z5:31

### Reproduktion (gemessen)
```
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 \
RE15_DEBUG_JUMP=3071@gp RE15_FIRE_AOT='2@60#3071' RE15_SCD_TRACE=1 RE15_BIRKIN_DBG=1 RE15_STATE_LOG=state.txt
```
(`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`; AOT-Slot 2 = der echte Ausloeser der Szene,
gesetzt von sub00 @0x0344C `2c 02 03 31 ...` Rechteck (-26770,-9270) 1230x1270 -> Ereignis sub02.)

* `[fire-aot] slot=2 at F60 (Raum 3071)` -> Thread slot 10 = sub02 startet.
* sub02 laeuft bis zur Warteschleife **@0x0364D `0f 04 08 00` While / @0x03651 `21 05 1f 00` Ck(5,31)==0 /
  @0x03655 Evt_next / @0x03657 Ewhile** und haengt dort von **F631 bis Laufende F2025** (letzte Zeile
  `[scd] F2025 slot=10 @0x364D op=0x0F`), ~1400 Bilder, kein Fortschritt.
* state.txt letzte Zeile: `pm=2` (Spieler skriptgefuehrt, Pad gesperrt — sub02 hat @0x03570 Set(2,7)=1 und
  @0x03574 Set(1,27)=1 gesetzt), Elza steht bei (-25210,-8670) Motion 24; Birkin `t=30 st=1 ss1=1` (HUB) laeuft
  auf der Stelle. -> Fuer den Nutzer = "Spiel haengt".
* birkin_dbg.log: Tick 0 `st=1 sub=10 ... grid=0x01`, 4 Proben (60 Ticks) sub 10, dann `sub=3` (LUNGE)
  `grid=0x00`, danach nur noch `sub=1` (HUB). Kein z5:31, kein z5:30 je gesetzt.

### Was die Szene erwartet (ROOM3071.RDT sub02 @0x0356C..0x036B7)
```
0x03641 2e 02 00        Work_set(2,0)      = Birkin (Sce_em_set sub00 @0x03434 `44 00 30 21` Typ 0x30)
0x03645 34 0c 01 00     Member_set(0x0C,1) = +0x09 grid := 1   (Port re15_actor_set_member case 12 -> grid_id)
0x03649 09 0a 46 00     Sleep 70
0x0364D 0f 04 08 00 / 0x03651 21 05 1f 00   While Ck(5,31)==0 { Evt_next }     <-- HAENGT HIER
0x03659 Evt_exec sub7 / 0x0365D Evt_exec sub6 / Sleep 100
0x03665 0f 04 08 00 / 0x03669 21 05 1e 00   While Ck(5,30)==0 { Evt_next }     <-- naechste Wartestelle
0x03671 Evt_exec sub8 ... Cut_chg, Plc_dest Elza, Plc_ret, Set(2,7)=0, Set(1,27)=0
```
Leon (ROOM3070) hat diese Wartestellen nicht (dort laeuft die Szene ueber Member_set 0x0C=0x13 @0x0358E),
deshalb betrifft der Haenger nur Elzas Szenario.

### Wer setzt z5:31 / z5:30 im Original — STAGE3.BIN, Birkin-Familie (Root 0x80116230)
Einzige zwei Produzenten im ganzen STAGE3-Overlay (Scan aller `jal 0x8004ef90` mit a0=0x800b1028):
```
BRAIN ACT-Tabelle @0x8011eef0[10] = 0x80119524  (DECIDE @0x8011eeb8[10] = 0x8011951c = jr ra)
Phasen-Sprungtabelle @0x80100434 (lui at,0x8010 / addiu at,1076 @0x80119564-68) auf +0x6:
 ph0 0x80119580: +0x6=1, +0x94=1 (Clip 1), +0x95=0, +0x8f=0xf, +0x9c = 0x78 (120) @0x801195c8-cc
 ph1 0x801195d0: anim_set; wenn +0x9c >= 71: 0x8011b5c4 + (+0x8c += 10) + pos_advance  (@0x80119600-30)
                 80119640 lh v1,156(v0) / 80119644 ori v0,zero,0x46 / 80119648 bne v1,v0,...
                 80119650 lui a0,0x800b / 80119654 addiu a0,a0,4136 (0x800b1028)
                 80119658 jal 0x8004ef90 / 8011965c ori a1,zero,0x1f        ==> z5:31 bei +0x9c == 70
                 +0x9c-- ; bei 0 -> ph=2                                     (@0x8011966c-90)
 ph2 0x80119694: ph=3, +0x94=0xf, +0x8f=0xf, +0x9c=0x96 (150)
 ph3 0x801196e4: anim_set, ph += fertig; bei +0x95 == 0x5b: jal 0x800453d0(6) (SE 6), Kopien
                 +0x188->[0xac8/0xacc/0xad0/0xad4] -> [0xa1c/0xa20/0xa24/0xa28],
                 801197a4 addiu a0,a0,4136 / 801197b0 ori a1,zero,0x1e / 801197b4 jal 0x8004ef90  ==> z5:30
 ph4 0x801197c4 ... ph5..ph13 (Clips 1/0xd/0x13, Timer 0x46/0x28/2/0x1e, Steer 0x8001aac4 auf den
                 Spieler @0x80119a5c) ... ph13 0x80119ae0: +0x5=3, +0x6=0, grid &= 0xfe, +0x0 &= ~8.
```
(FUN_8004ef90 = `bank[bit>>5] |= 0x80000000 >> (bit&31)`, also exakt das Bit, das Ck(5,31)/Ck(5,30) pruefen.)
Damit: z5:31 faellt im Original 50 Bilder nach dem Eintritt in Sub 10 (Timer 120 -> 70), z5:30 bei Bild 0x5b
von Clip 0xf in Phase 3.

### Port-Ursache
`re15_port/engine/src/enemy_ai_common.c` ~Z.12124, Birkin BRAIN `case 10`:
```c
case 10: {  /* CHARGE-COMBO 0x80119524: 14-phase grid-1 SPAWN ENTRANCE (effects 0x1f/0x1e, ...).
             * The full spawn choreography is a cinematic gap (OPEN); ... Faithful-compact: charge -> sub 3. */
    ... re15_birkin_clip(e,1); steer; re15_dog_advance(e,0x14);
    if (re15_birkin_anim(e) || dist < 0x9c4) { e->sub_state_1 = 3; ... grid &= ~1; birkin_rohr = 1; }
```
Der Port hat die 14-Phasen-Maschine 0x80119524 durch eine "verdichtete" Ersatzlogik ersetzt, die die beiden
Fortschritts-Flags **z5:31 (@0x80119658) und z5:30 (@0x801197b4) nie setzt** (die Kommentarzeile nennt sie
sogar "effects 0x1f/0x1e" — es sind Spiel-Flags, keine Effekte; gleicher Fehlertyp wie frueher z5:0x1C im
Morph-Tail @0x8011a6b8, s. Kommentar ~Z.12230). ROOM3071 sub02 wartet daher bei @0x0364D fuer immer.
Fix-Richtung: case 10 byte-true nach 0x80119524 (Tabelle @0x80100434, 14 Phasen) portieren, mindestens
`re15_game_flag_set(5,31,1)` bei Timer==0x46 in Ph1 und `re15_game_flag_set(5,30,1)` bei Bild 0x5b in Ph3;
nicht nur die zwei Flags reinkleben — die Szene (sub06/07/08 Objekt-Wuerfe, Cut-Folge) haengt am Timing.

---------------------------------------------------------------------------------------------------
### Antwort auf die Koordinator-Hinweise (Nibble 4 / ROOM3080)
* Der Haenger wartet auf **z5:31 (0x1f)** (@0x03651 `21 05 1f 00`), danach auf **z5:30 (0x1e)** (@0x03669 `21 05 1e 00`).
  Elzas Szene laeuft NICHT ueber Nibble 4, sondern ueber **Nibble 1**: Member_set(0x0C,1) @0x03645 setzt grid=1; INIT
  0x801166e0 @0x801168a0-b8 `lbu v0,9(a0)/andi 0xf/bne v0,1/ori v0,zero,0xa/sb v0,5(a0)` -> +0x5 = 10 (+ flags|=8
  @0x801168d4) -> ACT[10] 0x80119524 -> die Setzer 0x1f @0x80119658 / 0x1e @0x801197b4. Die Nibble-4-Setzer
  (@0x8011919c/0x8011921c/0x801192f4) sind fuer diesen Haenger nicht der Weg (gehoeren zu den grid-x4-Szenen, z.B.
  ROOM3080 Typ 0x36 grid 0x24 — Leon-Raum).
* ROOM3080 ist Leon (Ada 0x42 + Marvin 0x40 + 0x36 grid 0x24 @0x007D4). ROOM3081 (Elza) spawnt nur 0x4B + 0x49
  (@0x007A0/@0x007B4), keinen Birkin -> Elzas STAGE3-Birkin-Szene ist ausschliesslich ROOM3071 sub02.
  (3081 laeuft laut H_abnahme_1.md 4.6 in 285 Bildern bis room4001 durch.)

---------------------------------------------------------------------------------------------------
## BUG 2 — ROOM4000 (Leon: Ada 0x42 + Marvin 0x40) / ROOM4001 (Elza: 0x49 + 0x4B): falsche NPC-Animation
## waehrend der Lift-Abfahrt

### Messung (RE15_DEBUG_JUMP=4000@gp, RE15_STATE_LOG + RE15_ANIM_TRACE, Lauf r3/r4)
SCD: sub00 @0x012A2/@0x012B6 `44 00 42 40 ...` / `44 01 40 40 ...` (beide grid 0x40, gleiche Lage), Evt_exec sub5.
sub05 startet sub6/7/8 und haelt Ada per Work_set(2,0) @0x0156E, sub08 haelt Marvin per Work_set(2,1) @0x01720; beide
fahren per Speed_set/Add_speed 60 Bilder abwaerts (@0x01582..@0x0159C bzw. @0x01734..@0x0174E). Das alles laeuft
schon im Init-Lauf des Raums (debug.log: `thread start slot=10/11/12/13` in F1768 vor dem ersten Spielbild).

Port, Bild 1..65 (Abfahrt):
```
[1 t=42 st=0 ss1=0 ... g=40 mo=2 af=5 / 25 / 49]   Ada:    State 0 (INIT nie gelaufen), Clip 2 der Bank 0 (50 Bilder) im Kreis
[2 t=40 st=0 ss1=0 ... g=40 mo=2 af=0 / 0 / 0]     Marvin: State 0, Clip 2 Bank 0 Bild 0 EINGEFROREN
anim.txt: idx1 type66 motion2 clip2 fc=50 loco=0 ; idx2 type64 motion2 clip2 fc=50  (bis F66 Marvin Plc_motion(1,23), F75 Ada Walk 5)
```
Ursache im Port: `re15_npc_ai_tick` (enemy_ai_common.c ~Z.10639-10645)
```c
int in_motion_pose = (e->state == 4 && e->sub_state_1 <= 9) || ...;
if (e->walk_active || (re15_scd_slot_event_controlled(slot) && !in_motion_pose)) { e->hp = -1; return; }
```
Der SCD-Faden haelt den Slot (Work_set) BEVOR der erste KI-Takt die INIT (Fall 0) erreicht; State 0 ist kein
"motion pose" -> der Tick kehrt jedes Bild vor dem `switch` zurueck, INIT laeuft nie. Ada wird dann vom globalen
Advancer (player_common.c re15_actors_anim_advance, gilt fuer State 0) auf der Container-Bank weitergetaktet, Marvin
(0x40 ist self-advancing -> uebersprungen) bleibt auf Bild 0 stehen.

Original: es gibt KEINE SCD-Besitz-Sperre. STAGE4 Marvin-Wurzel 0x80116be4 (Registrierung `sw v0,0x2cac(at)`
@0x80118418 = dispatch[0x40]): einzige Tore `lw aca40 / lui 0x2000 / and / bne` @0x80116bf8-c04 und
`lbu +0x9 / andi 0x20 / bne` @0x80116c18-24, dann State-Tabelle @0x8011a2d8[+0x4] (@0x80116c70-80). INIT
0x80116d20: `lbu v0,9(v1); andi v0,v0,0x40; beq ...; ori v0,zero,4; sb v0,4(v1)` @0x80116e90-a4, +0x5=6 @0x80116eb0
(Ada 0x42 @0x801171b4 analog; STAGE1 @0x8011ce1c-40). Executor-Sub 6 = 0x80076ca0[6] = 0x800517f0:
Ph0 @0x80051844-74: +0x6=1, +0x94=1, +0x95=0, +0x8f=7; anim_set mit `lw a0,368 / lw a1,372` (+0x170/+0x174 =
EIGENE Bank 1) @0x80051884-8c; fertig -> Ph2 @0x800518b4: +0x94=2, Ph3 @0x800518dc-f0 Loop Bank 1.
=> Im Original spielen beide waehrend der Fahrt Bank-1-Clip 1 einmal (Ada/Marvin 16 Bilder) und dann den
Bank-1-Idle Clip 2 (52 Bilder) — der Port zeigt Ada im falschen Clip (Bank 0 Clip 2, 50 Bilder) und Marvin starr.
ROOM4001 hat dasselbe Muster (sub00 @0x01194/@0x011A8 `44 00 49 40`/`44 01 4b 40`, Lift-Faeden sub10 Work_set(2,0)
@0x01880, sub13 Work_set(2,1) @0x01BE6) -> gleicher Mechanismus (dort nicht separat gemessen).

### Regression? (gemessen gegen alte Releases, Builds in scratchpad/agent_c/b828, b824)
* v0.8.28 (f632a17b) und v0.8.24 (e8b3958f) zeigen in ROOM4000 BITGLEICH dasselbe (r6/r7/r8: st=0, Ada af 0..49,
  Marvin af 0; anim/state 420 Bilder v0.8.28 == master, 0 Diff-Zeilen). Kein Rueckschritt zwischen 0.8.24 und master.
* Die Sperre selbst stammt aus 76f20c1e (2026-07-21, Elliot-Arm-Loop) / 4cf6beade (2026-07-24 "GLOBAL SCD-owns-animation
  yield") / 3ed7b2f8 (2026-07-26, in_motion_pose-Ausnahme). Der Fehler ist also seit Juli strukturell drin.
* Bekannt war er schon: Runde 37 (1f712129/0e6117b7, ROOM11C0) — Kommentar in irons_tod_1150.c ~Z.544: "der Port laesst
  einen NPC, den ein SCD-Faden per Work_set haelt, vor seinem INIT nicht ticken ... Ada blieb bis Bild 17 ohne INIT in
  einer haengenden Greif-Pose" — dort nur lokal umgangen (OP_WORK(1,0) im Port-Programm k_p_11c0), nicht global
  behoben. Der verwandte frühere Fix fuer Sub 6 (eigene Bank 1, marvin_spawn_anim.md, ROOM10D0) greift in 4000/4001
  nie, weil Sub 6 dort nie erreicht wird. Einen frueheren Fix speziell fuer 4000/4001 habe ich in Commit-Historie
  (6760 Commits, --grep 4000/4001/Lift/Aufzug/Ada/Marvin) und analysis/ NICHT gefunden.
* Fix-Richtung: State 0 (INIT) von der SCD-Yield-Sperre ausnehmen (das Original hat keine), d.h. INIT laufen lassen,
  danach greift in_motion_pose (State 4 Sub 6) ohnehin; die 11C0-Umgehung danach pruefen.
