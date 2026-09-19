# Komische Spieler-Animation direkt nach dem Raumeintritt (ab ROOM1040)

**Datum:** 2026-09-19 (Runde 16) · **Nutzer:** „Ab ROOM 1040, sobald man die Tueren durchlaeuft macht
Leon immer am Start im Raum eine ganz komische Animation" (Marken befund_1040_F5_marke7,
befund_1040_F8_marke9) · **Phase:** Messung + Original-RE + Fix-Plan, KEIN Engine-Code geaendert.
Sonde: `re15_port/tests/unit/probe_r16_tuer1040.c` (Registrierung `tests/unit/probes/r16_tuer-animation-1040.cmake`).

## 0. Kurzfassung

1. **Defekt (gemessen, echte exe + Sonde):** Nach JEDEM Raumwechsel (Tuer, Self-Tuer, Debug-JUMP) steht der Spieler
   mit `motion=0` da; der Renderer loest `0` ohne Sentinel als **Clip 0 der def-Bank** auf. In Raeumen mit
   RDT-Animationsblock @0x5C ist die def-Bank die **Raum-Cinematic-Bank** — in ROOM1040 ist deren Clip 0 (59 Bilder)
   ein Cutscene-Keyframe mit beidseitig ausgestreckten Armen. Der wird F0–F5 unter der Einblendung gehalten und
   danach (F6–F12) per 7-Bild-Crossfade in die Idle-Pose gezogen: rechter Unterarm wandert 431 Einheiten.
2. **Warum „ab 1040":** ROOM1000/1010/1020/1030 haben KEINEN Animationsblock → def=PL00, dessen Clip 0 Bild 0 nur
   41 Einheiten von der Idle-Pose entfernt liegt (unsichtbar). ROOM1040 ist der ERSTE Raum der Kette mit Block
   (dann 1050, 1090, 10D0, … 47 Raeume gesamt).
3. **Original:** Die Transitions-FSM nullt das Kommando (`sb zero,0x800aca58` @0x8001cbdc); der cmd-0-Handler
   @0x800318f8 endet mit `+0x94 := 1` @0x80031c10, `+0x95 := 0` @0x80031c18, `+0x8f := 0` @0x80031c20 und
   `FUN_8001f314(PLW-EMR 0x800acbc4, PLW-EDD 0x800acbc8, a2=0, a3=0x200)` @0x80031bfc–c28 — also **W-Bank
   (Waffenbank) Clip 1, Bild 0, hart gesetzt**. W01/W03 Clip 1 Bild 0 = Keyframe 22 = derselbe Keyframe wie der
   Idle-Clip 3 (`PL00W01.EDD` @0x90 `16 00 00 00`, @0x1a0 `16 00 00 00`). Der Idle-Eintritt (case 0 @0x80032074:
   `+0x94:=3` @0x80032088, `+0x8f:=7` @0x8003209c) blendet danach kf22→kf22 = **keinerlei sichtbare Bewegung**.
4. **Port muss:** an der Stelle des cmd-0-Endes (room_common.c:279–281 bzw. Self-Reenter game_step_common.c
   vor scd_room_reenter) statt `motion=0` den **W-Bank-Clip-1-Sentinel (210 = RE15_MOTION_IDLE_SETTLE → anim_select
   W-Bank Clip 1) mit anim_frame=0, anim_frac=0** setzen. Dann posiert der Port F0–F5 kf22 und der F6-Crossfade
   ist bewegungslos — wie das Original.
5. Hypothesen (a) Waffe, (b) Verletzung, (c) 1040-SCD-Plc_motion, (d) stehengebliebener Einlauf-Clip,
   (e) frame_count-Reset: alle **gemessen widerlegt** (§1.4).

## 1. Reproduktion/Messung

### 1.1 Echte exe, echter Tuer-Weg (re15_port/build/platform/pc/re15_pc.exe, Stand 2026-09-14 18:19)

Lauf `RE15_DEBUG_JUMP=1040@30 RE15_INPUT_SCRIPT="U1.3,A0.5,W2,L0.72,U0.6,A0.5,W6" RE15_INPUT_SCRIPT_START=600`
(+ `RE15_MOTRACE=1 RE15_POSE_DUMP RE15_ANIM_TRACE RE15_FRAMEDUMP=0-30/1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1`):
debug.log zeigt `[aot] DOOR FIRE slot=1 … spawn=(-26214,0,-3861)` (1040→1030, F586) und
`[aot] DOOR FIRE slot=2 rect=(-26900,-3650…) spawn=(-20083,0,-3559)` (1030→**1040**, F115 in 1030) —
beides ECHTE Tueren ueber aot_scan → apply_pending → transition_present.

`[mot]`-Spur nach der Tuer 1030→1040 (identisch beim Debug-JUMP nach 1040):

| Bild | mo | af | frac | pmode | Render (ANIM_TRACE: clip / fc / slot) |
|---|---|---|---|---|---|
| F1–F5 | 0 | 0 | 0 | 0 | def-Bank (RDT@5C) Clip 0, fc 59, slot 0 |
| F6 | 200 | 0 | 7 | 0 | W01 Clip 3, kf 22, Crossfade 7/8 alt |
| F7…F12 | 200 | 1…6 | 6…1 | 0 | Crossfade laeuft ab |
| F13+ | 200 | 7+ | 0 | 0 | Idle neutral |

`RE15_POSE_DUMP` (Render-Ebene, Bone 13 = rechter Unterarm, Bone 9 = Kopf), drei Raumeintritte desselben Laufs:

| Eintritt | F0–F5 b13 | F6 | F8 | F10 | F13 (Idle) | Δ b13 F5→F13 |
|---|---|---|---|---|---|---|
| 1 JUMP → 1040 | (309,-2273,434) | (274,-2222,431) | (84,-2095,417) | (-82,-2078,407) | (-122,-2083,406) | **431 / 190 / 28** |
| 2 Tuer → 1030 (kein Block) | (-81,-2082,415) | (-86,-2082,414) | (-105,-2084,409) | (-119,-2084,406) | (-122,-2083,406) | 41 / 1 / 9 (unsichtbar) |
| 3 Tuer → 1040 | (309,-2273,434) | (274,-2222,431) | (84,-2095,417) | (-82,-2078,407) | (-122,-2083,406) | **431 / 190 / 28** |

Bilder (komponierter Frame, `analysis/befunde_2026-09-19/`): `tuer1040_door_F06.png` (beide Arme seitlich
ausgestreckt = Cinematic-Clip-0-Keyframe 0), `tuer1040_door_F08.png` (Arme sinken, Mitte des Crossfades),
`tuer1040_door_F13.png` (Idle). F01/F05 sind wegen der Einblendung (Stufe 0xFF…0x0F) schwarz — die Marke F5 des
Nutzers liegt im letzten Blendbild, F8 mitten im Crossfade; beides passt zur Spur.

### 1.2 Sonde `probe_r16_tuer1040` (Engine-only, echter Tuer-Pfad ROOM1030 slot 2 → ROOM1040)

`re15_port/build_r16_tuer1040/tests/unit/probe_r16_tuer1040.exe <A|B|C|D|E> [n]`. Sie laedt PL00/W01/W03 wie
main.c, baut die def-Bank exakt wie main.c (`re15_apply_room_cinematic` auf den RDT-Block @0x5C, sonst PL00),
tickt `re15_fade_tick + re15_room_transition_tick + scd_vm_tick + re15_game_step` und protokolliert
`re15_actor_anim_select`/`re15_compute_actor_kf`. Kernzeilen (Variante A, Messer, 40 Bilder Gehen vor der Tuer):

```
[bank] ROOM1040 hat RDT-Animationsblock @0x5C (9608 Bytes) -> def=RDT@5C clips=5 kf=100
PREAP  mo=105 af=144 frac=0 … RENDER bank=W01 clip=5            (Gehen, vor apply)
ENTRY  mo=0 af=0 frac=0 st=1 pause=00000007 trans=1 | RENDER bank=RDT@5C clip=0 fc=59 slot->kf=0
DEST t=0..4  mo=0 … pause=ff000007 trans=1        | RENDER bank=RDT@5C clip=0 kf=0   (Freeze State 4/5)
DEST t=5     mo=200 af=0 frac=7 pause=00000007 trans=0 | RENDER bank=W01 clip=3 kf=22 (Crossfade-Start)
DEST t=12    mo=200 af=7 frac=0
```

Varianten: **B** (Handgun, Item 3): identisch, nur `bank=W03` ab t=5. **C** (Stehen statt Gehen): identisch.
**D** (hp=40): identisch. **E** (Laufen, CROSS): identisch (`PREAP mo=100 … W03 clip 0`, ENTRY mo=0).

Frame-Eintraege der Waffenbank (`PL00W01.EDD`, byte-genau, ebenso `PL00W03.EDD`):
Clip 1 @0x90 `16 00 00 00 17 00 00 00 …` (16 Bilder, kf 22,23,…), Clip 3 @0x1a0 `16 00 00 00` (1 Bild, kf 22).
`PL00.EDD` Clip 0 @0x60 `00 00 00 00 01 00 00 00 …` (34 Bilder, kf 0…).

Raeume mit Animationsblock @0x5C (47): 1011 1021 1031 **1040** 1041 1050 1090 10B1 10D0 1141 1150 1151 1170 1171 11B0
11B1 11C0 1211 2000 2001 2020 2021 3000 3001 3020 3060 3061 3070 3071 3080 3091 30C1 30E0 30E1 4000 4001 4010 4031
5011 5021 5031 5090 5091 50B0 6000 6030 6031. ROOM1040-Block: 1 Record, 15 Bones, 100 Keyframes, 5 Clips
(59/56/70/102/102 Bilder) — Clip 0 ist ein Cutscene-Clip (Rolltor-Schalter-Sequenz), Bild 0 = Arme ausgestreckt.

### 1.3 Weg 1000→1010→…→1040

Die Autopilot-Laeufe durch die Lobby (1030 Spawn → Tuer slot 2) blieben an der Theke haengen (`[auto] fest bei …`);
der Weg wurde deshalb per Eingabeskript 1040→1030→1040 (echte Tueren, §1.1) gefahren. Fuer die Kette 1000…1030
genuegt die Datenlage: keiner dieser Raeume hat einen Animationsblock, ihr Eintritt ist der Fall „Eintritt 2" der
Tabelle (Δ 41 Einheiten, unsichtbar) — deshalb faellt es dem Nutzer erst ab 1040 auf.

### 1.4 Hypothesen-Test (alle gemessen)

| Hyp. | Ergebnis |
|---|---|
| (a) Waffe/Carry-Bank-Index | Nein: A (Messer) und B/E (Handgun) zeigen dieselbe Sequenz, nur W01↔W03 ab t=5 |
| (b) Verletzt-Zustand | Nein: D (hp=40) identisch |
| (c) ROOM1040-SCD setzt Plc_motion | Nein: `pmode=0`, motion bleibt 0 bis zur Freigabe; Clip 0 kommt aus dem def-Bank-Fallback von `motion=0`, nicht aus einem Plc_motion (1040-SCD nutzt nur `Plc_dest(0,9,…)` in sub08) |
| (d) Einlauf-Clip des Vorraums bleibt stehen | Nein: PREAP mo=105/100 → ENTRY mo=0 (room_common.c:279) |
| (e) `g_engine.frame_count = 0` main.c:6527 | Kein Einfluss: Pose/Clip haengen nicht am Bildzaehler (Spur identisch bei JUMP und Tuer) |

## 2. Original-Mechanismus

**Reihenfolge im Hauptloop (PSX.EXE):** Transitions-FSM `lbu v0,DAT_800b5359` @0x8001c994 dispatcht den State; der
State-3-Rumpf endet in LAB_8001cc98 und faellt in die Subsystem-Aufrufe desselben Bildes: SCD `jal FUN_8003f038`
@0x8001cdec … Spieler-Dispatcher `jal FUN_80031c44` @0x8001ce0c, AOT `jal FUN_800436a8` @0x8001ce1c.

**State 3 (Tuer/JUMP/Self-Reenter, LAB_8001cbbc):**
```
8001cbc4  ori   v0,zero,0x7
8001cbdc  sb    zero,-0x35a8(at)=>DAT_800aca58     ; Spieler +0x04 (Kommando) := 0
8001cbe4  sw    v0,-0x35c0(at)=>DAT_800aca40       ; Pause-Flags := 7 (kein Vorzeichenbit -> Dispatcher laeuft)
8001cc00  jal   FUN_800217b0  (a0=0x200,a1=-0x1800,a2=7,a3=0)   ; Blende konfigurieren
8001cc18  jal   FUN_800216ec  (0,0,0xffffff,0)                   ; Blende starten
8001cc28  sb    v0(=4),DAT_800b5359                              ; State := 4
```
**State 4** @0x8001cc34–6c: `lui v0,0xff00 / or a0,a0,v0 / sw a0,DAT_800aca40` → Pause |= 0xff000000 (Spieler-Freeze:
`bltz a0` @0x80031c78 ueberspringt den ganzen Dispatcher). **State 5** @0x8001cc70–94: `jal FUN_8002178c(0)`, bei
Blende-fertig `sb zero,DAT_800b5359` + Pause-Flags zurueck.

**Spieler-Dispatcher FUN_80031c44** (laeuft noch im State-3-Bild, Pause=7):
```
80031c8c  lbu  v1,DAT_800aca58        ; Kommando (jetzt 0)
80031cac  lw   v0,PTR_LAB_80073f90[v1] ; = LAB_800318f8
80031cb4  jalr v0
```
**cmd-0-Handler LAB_800318f8** (Tabelle @0x80073f90 Eintrag 0):
```
80031924  sb   zero,DAT_800acae8      ; +0x94 := 0      (ERSTER Store — das ist die Zeile, die der Port zitiert)
8003192c  sw   v0(=1),DAT_800aca58    ; +0x04..+0x07 := 1/0/0/0  (Kommando 1, Sub 0 = Idle, Phase 0)
80031954  sb   zero,DAT_800acae9      ; +0x95 := 0
8003197c  sh   zero,DAT_800acc18      ; +0x1c4 := 0
...       (Licht/Matrix-Setup FUN_8001af5c, RotMatrix, Feld-Resets)
80031bfc  lw   a0,DAT_800acbc4        ; PLW-Paar der ausgeruesteten Waffe (EMR) — einziger Schreiber FUN_80036b68
80031c04  lw   a1,DAT_800acbc8        ;   (EDD)                              @0x80036be4/@0x80036c04
80031c08  ori  v0,zero,0x1
80031c10  sb   v0,DAT_800acae8        ; +0x94 := 1   <-- LETZTER Store: Clip 1 der W-BANK
80031c18  sb   zero,DAT_800acae9      ; +0x95 := 0   (Bild 0)
80031c20  sb   zero,DAT_800acae3      ; +0x8f := 0   (KEIN Crossfade: f3bc-Zweig uVar5==0 setzt die Pose hart)
80031c24  jal  FUN_8001f314           ; anim_set(W-EMR, W-EDD, a2=0 (vorwaerts), a3=0x200)
80031c2c  jal  FUN_8003703c (0x64)    ; Entity-Naehe-Scan, ohne Pose-Wirkung
```
FUN_8001f314 (RE_15_Quellcode_V2/FUN_8001f314.c): `frame = *(EDD + motion*4)`, Slot = +0x95 (a2==0), Zeiger nach
+0x168, dann FUN_8001f3bc: bei `+0x8f == 0` werden Wurzel/Bones DIREKT in den Pose-Puffer (+0x188) geschrieben
(kein GTE-Blend), sonst `gte_ldIR0(0x1000 - 0x200*+0x8f)` = Crossfade mit dem vorigen Puffer.

**Idle-Eintritt nach der Freigabe** (cmd 1, Sub-UPDATE-Tabelle @0x80073ff0[0] = LAB_80032038, `switchD_8003206c` auf
+0x06 = DAT_800aca5a):
```
80032074  ori v0,zero,0x1 / 8003207c sb v0,DAT_800aca5a     ; Phase := 1
80032080  ori v0,zero,0x3 / 80032088 sb v0,DAT_800acae8     ; +0x94 := 3 (W-Bank Clip 3 = neutral)
80032094  sb  zero,DAT_800acae9                             ; +0x95 := 0
8003208c  ori v0,zero,0x7 / 8003209c sb v0,DAT_800acae3     ; +0x8f := 7 (7-Bild-Crossfade)
800320a8  jal FUN_8001af20 ; 800320b0 andi 0x1f / 800320b4 addiu 0x5a -> Timer 90..121 @DAT_800acaf0
800320e8  lw a0,DAT_800acbc4 / 800320f0 lw a1,DAT_800acbc8 / 800320f4 jal FUN_8001f314 (a2=0,a3=0x200)
```
**Folge im Original:** Bild des State 3: Pose = W-Bank Clip 1 Bild 0 = **Keyframe 22** (EDD-Bytes §1.2), hart.
State 4/5 (Blende, ~6 Bilder): eingefroren auf kf 22. Freigabe: Crossfade 7 Bilder von kf 22 (Puffer) nach Clip 3
Bild 0 = kf 22 → **Pose bleibt unveraendert**. Der Spieler steht vom ersten Bild an in der neutralen Idle-Pose.

**Bank-Zuordnung:** `DAT_800acbc4/8` sind die PLW-Zeiger der ausgeruesteten Waffe (FUN_80036b68 laedt
`PL00W<item>.PLW`; Port-Beleg in main.c „DIE BANK KOMMT AUS DER WAFFEN-ID"). W00≡W01≡W02 (Messer) und W03≡W04
(Handfeuerwaffe) tragen in Clip 1/3 dieselben Eintraege (kf 22), gemessen an PL00W01.EDD/PL00W03.EDD.

## 3. Port-Ist

- `re15_port/engine/src/room_common.c:277–301` (`re15_room_apply_pending`, Schritt 4): setzt `p->motion = 0`
  (Kommentar zitiert `+0x94 @0x80031924` = den ERSTEN Store des cmd-0-Handlers) und `anim_frame = 0`,
  `anim_flags = 0`, `state = 1`; dann `re15_player_cmd_reset()` (game_step_common.c:388, Aim/Idle-Statics).
  **Der LETZTE Store des Handlers (`+0x94 := 1` @0x80031c10 mit W-Bank-Zeigern) fehlt.** `motion=0` ist im Port
  kein Sentinel → `re15_actor_anim_select` (anim_select_common.c:200–204, 397ff) laesst `clip_override=-1` und
  posiert `def_*` Clip 0. `def_*` ist in Raeumen mit Block @0x5C die Raum-Cinematic-Bank (main.c:6570–6595
  `re15_apply_room_cinematic(&skel,&anim)` nach jedem Raumwechsel), sonst PL00.
- `re15_room_transition_present()` (room_common.c:132–150) spielt State 3 ab (Pause=7, Blende, State 4), setzt
  `state=1`/Sub 0, laesst `motion` unangetastet.
- `re15_room_transition_tick()` (room_common.c:154–169) friert mit `0xff000000`; game_step_common.c:1119 ueberspringt
  den Spieler-Tick (kein Advance, kein FSM) — also bleibt `motion=0`/def-Clip-0 fuer die Blende stehen (t=0..4).
- Nach der Freigabe: `re15_player_tick` Idle-FSM (player_common.c:1017–1027 + 1046–1051): `s_idle_phase=-1 → 0`,
  `want_motion=200`, `motion != want` → `anim_frame=0, anim_frac=7` → Crossfade (skeleton_common.c:212–223,
  `wp = frac*0x200`) vom zuletzt GERENDERTEN Puffer (= Cinematic-Clip-0-Pose) nach W01 Clip 3 kf 22.
  Der Crossfade-Mechanismus selbst ist byte-true; **falsch ist nur die Quellpose**, weil das Eintritts-Set fehlt.
- Self-Reenter-Pfad (game_step_common.c:1824 `scd_room_reenter` + :1883 `re15_room_transition_present`) setzt
  motion GAR NICHT zurueck (der Pad-Walk-Clip 105 bliebe eingefroren) — gleiche Klasse, hier nicht gemeldet.
- `platform/pc/main.c:6527 g_engine.frame_count = 0` (Hyp. e): ohne Bezug zur Pose, keine Aenderung noetig.

## 4. Fix-Plan (Phase 2)

1. **player_common.c:** neue Funktion `void re15_player_room_entry_pose(void)` neben `re15_player_idle_reset`:
   ```c
   p->motion = RE15_MOTION_IDLE_SETTLE;   /* 210 -> anim_select W-Bank Clip 1: `+0x94 := 1` @0x80031c10 mit
                                           * PLW-Paar DAT_800acbc4/8 @0x80031bfc/@0x80031c04 (cmd-0 LAB_800318f8) */
   p->anim_frame = 0;                     /* +0x95 := 0 @0x80031c18 */
   p->anim_frac  = 0;                     /* +0x8f := 0 @0x80031c20 -> f3bc-Zweig uVar5==0: Pose HART, kein Blend */
   p->anim_flags &= ~RE15_ANIM_REVERSE;   /* a2 = 0 @0x80031bf4 (clear a2) -> vorwaerts */
   p->motion_init_delay = 0; p->anim_use_pl00 = 0;
   ```
   Sentinel 210 ist heute `#define` in player_common.c und wird in anim_select_common.c:397–405 auf W-Bank Clip 1
   abgebildet (`banks->w01_*` = in main.c die Bank der ausgeruesteten Waffe, wie DAT_800acbc4/8). Kein neuer Sentinel.
2. **room_common.c:279–281:** die drei Stores durch den Aufruf ersetzen (Kommentar: „erster Store @0x80031924 wird
   im selben Handler @0x80031c10 ueberschrieben; wirksam ist der Endzustand"). Position VOR `scd_room_reenter`
   beibehalten — im Original laeuft der SCD-Tick @0x8001cdec VOR dem Dispatcher @0x8001ce0c, ein `Plc_motion` des
   Raum-main00 (cmd := 4 @0x80041b90) gewinnt also gegen cmd 0; im Port gewinnt es ebenso, weil es NACH dem
   Eintritts-Set schreibt.
3. **game_step_common.c Self-Reenter (~1820, vor `scd_room_reenter`):** denselben Aufruf einfuegen (State 3
   @0x8001cbdc gilt fuer jede Transition, FUN_8001d600 verzweigt nur am Tuer-Record @0x8001d618).
4. **anim_select_common.c:453:** den PL00W01-losen Rueckfall (`m == 200 → clip 6`) auf 210 erweitern (PSX-Build ohne
   W-Bank wuerde sonst `210 % clip_count` posieren).
5. **Reihenfolge:** 1 → 2 → 3 → 4 → Sonde/Pin → Pakete. Keine Konstante ohne @0x; keine Aenderung an Crossfade,
   Idle-FSM oder Bank-Wahl.
6. **Absicherung:** `probe_r16_tuer1040` um `add_test` + Pruefungen erweitern (Varianten A und B):
   - ENTRY und t=0..4: `RENDER bank == W01|W03`, `clip == 1`, `slot->kf == 22`, `frac == 0`;
   - t=5: `mo == 200`, `frac == 7`, `kf == 22`;
   - Render-Pose (`re15_skel_compute_pose` mit Composite-Skelett wie main.c) Bone 13 bei t=4 und t=12 **identisch**
     (kf22→kf22-Blend); Kontrolle: Vorraum-Eintritt 1030 ebenfalls kf 22.
   - Regressionslauf: `probe_hitdoor_entry_anim` A/B/C (dort ist „mo=200 ab t=0" dokumentiert — nach dem Fix
     mo=210 fuer t=0..4, dann 200; Text/Erwartung anpassen), `test_text_freeze`, `probe_load_after_death`,
     `probe_1140_reentry`, `probe_cam_1030_reentry`, `probe_zreentry_10d0` (Pfad apply_pending).
   - Sichtpruefung: echter Lauf wie §1.1 (`RE15_FRAMEDUMP=0-30/1`), F06/F08 muessen die Idle-Pose zeigen
     (b13 ≈ (-122,-2083,406) ab F0).
7. **Risiken:** (i) Raeume, deren main00 den Spieler per Plc_motion posiert (1170-Intro, 1150 Irons): Reihenfolge wie
   Punkt 2 sichert das; gegenpruefen mit `probe_1090_cutscene`/1170-Sonde. (ii) `anim_prev_valid` bleibt vom Vorraum
   gesetzt — bei `frac=0` ohne Wirkung, der erste Render schreibt den Puffer neu (wie f3bc uVar5==0).
   (iii) Wenn die W-Bank fehlt (nur PSX ohne PLW), Rueckfall Punkt 4.

## 5. Offen / nicht belegt

- Die Nutzer-Marken (BMPs) liegen nicht vor; die Zuordnung „F5 = letztes Blendbild mit Cinematic-Pose, F8 = Mitte
  des Crossfades" folgt aus der eigenen Spur (Software-Renderer, Fenster 320x240). Beim beschleunigten Renderer des
  Nutzers kann die Blende anders sichtbar sein, die Pose-Sequenz ist renderer-unabhaengig (POSE_DUMP).
- Der genaue Blende-Ablauf State 4/5 (6 Bilder) wurde nicht gegen einen Savestate gemessen; er ist bereits in
  room_common.c belegt (@0x8001cc34–94) und fuer den Defekt unerheblich (Pose ist ab Bild 0 falsch).
- W-Bank-Clip-1-Bild-0 = kf 22 wurde fuer PL00W01/W03 (Messer/Handfeuerwaffe) byte-genau geprueft; die uebrigen
  19 PLW-Baenke (Schrot, MG, …) nicht — der Port haelt alle 21 Baenke, die Pin sollte Clip 1 Bild 0 fuer jede
  geladene Bank einmal ausgeben.
- Welche Cutscene ROOM1040-Clip 0 gehoert (vermutlich Rolltor-Schalter, sub08 `Plc_dest(0,9,…)`), ist fuer den Fix
  irrelevant und nicht weiter verfolgt.
- Der Autopilot kommt in ROOM1030/1040 nicht um Theke/Ecken herum (`[auto] fest bei …`); fuer Messlaeufe hier das
  Eingabeskript aus §1.1 verwenden.

## 6. Umsetzung (Phase 2, 2026-09-19, Branch `worktree-wf_074e2f88-24e-1`, Thema objekte-und-tuer)

Umgesetzt wie §4 mit den Korrekturen der Gegenpruefung (`tuer-animation-1040.skeptiker.md`):

1. `engine/src/player_common.c`: `re15_player_room_entry_pose()` = Endzustand des cmd-0-Handlers
   LAB_800318f8: `motion = 210` (+0x94 := 1 @0x80031c10 mit dem PLW-Paar @0x80031bfc/@0x80031c04),
   `anim_frame = 0` (+0x95 @0x80031c18), `anim_frac = 0` (+0x8f @0x80031c20, f3bc-Zweig uVar5==0 =
   hart), `anim_flags = 0` (+0x1C4 @0x8003197c; a2 = 0 @0x80031bf4 — damit ist auch das REVERSE-Bit
   0x80 weg, Skeptiker-Punkt 5 erledigt sich, kein Flag-Name ausserhalb von player_common.c noetig),
   `motion_init_delay = 0`, `anim_use_pl00 = 0`. Deklaration in `include/re15_player.h`.
2. `engine/src/room_common.c` (`re15_room_apply_pending`, Schritt 4): die drei Stores durch den Aufruf
   ersetzt, Position VOR `scd_room_reenter` beibehalten; `#include "re15_player.h"`.
3. `engine/src/game_step_common.c` (Same-Room-Reenter, vor `scd_room_reenter`): derselbe Aufruf.
4. `engine/src/anim_select_common.c`: der W-Bank-lose Rueckfall. **Gemessen** (Sonde Variante N, Renderer
   ohne W-Bank): der alte Rueckfall `m == 200 -> def-Bank Clip 6` lieferte KEINE Idle-Pose — bei
   def=PL00 ist Clip 6 der 50-Bild-Sturz (kf 184, L1 = 7409 zur W-Idle kf 22, alle Bones), in ROOM1040
   (def = Cinematic-Bank, 5 Clips) posierte 210 % 5 = Cutscene-Clip 0 (b13 (309,-2273,434) = die
   gemeldete Pose) und 6 % 5 = Cutscene-Clip 1. Skeptiker-Punkt 4 bestaetigt. Neuer Rueckfall fuer
   200/210/211/212 ohne W-Bank: COMMON-Bank PL00 Clip 22 = der Idle-Clip, den das Original selbst aus
   PL00 spielt (Idle-Fall 9/a: `ori v0,zero,0x16` @0x80032284, `sb v0,0x800acae8` @0x8003228c,
   PL00-Paar @0x800322c0/@0x800322c8, f314 @0x800322cc). PL00-Keyframe-Scan (712 kf): der W-Idle am
   naechsten liegen kf 183 und die Bild-0 der Clips 17/19/20/21 (L1 = 3) — alles Bewegungsclips
   (Aim-Raise, Stairs); ein statischer Idle-Keyframe existiert in PL00 nicht, Clip 22 (kf 652, 30
   Bilder, L1 2443) ist der einzige Clip, den das Original als Idle benutzt. Der Fall ist im Spiel
   nicht erreichbar (PC laedt alle 21 PLW-Baenke beim Boot, PSX `re15_w01_ok` ebenso; das Original
   haelt das PLW-Paar ab FUN_800314b0 @0x800316f0).

Messwerte (Sonde `probe_r16_tuer1040`, jetzt Pin; Render-Pose mit Composite-Skelett wie main.c,
Bone 13 = rechter Unterarm; Freeze-Bilder im Port t=0..4, Freigabe t=5 — an diese Bildnummern gepinnt):

| Bild | vorher (§1.2) | nachher A (W01) / B (W03) |
|---|---|---|
| ENTRY | mo=0 af=0 frac=0, RENDER RDT@5C Clip 0 kf 0, b13 (309,-2273,434) | mo=210 af=0 frac=0, RENDER W01 Clip 1 kf 22, b13 (-122,-2083,406) |
| t=0..4 (Freeze) | wie ENTRY | wie ENTRY, Pose == statisch Clip-1-Bild-0 (L1 = 0) |
| t=5 | mo=200 frac=7, W01 Clip 3 kf 22, Crossfade ab (274,-2222,431) | mo=200 frac=7, W01 Clip 3 kf 22, b13 (-122,-2083,406) |
| t=12 | frac=0, b13 (-122,-2083,406); L1 t4->t12 = 649 | frac=0, b13 (-122,-2083,406); **L1 t4->t12 = 0** |

Bankabhaengiger Pin (Skeptiker-Punkt 1): Erwartung t=12 = statische Pose von W-Clip 3 am aktuellen
Bild (bei W01/W03 Bild 0 = kf 22; bei W0F Clip 3 mit 50 Bildern Bild 7 = kf 79) — Variante F (Item
0x0F, Bank W0F, c1f0 kf 22 != c3f0 kf 72, statisch L1 1760): t=0..4 kf 22 hart, t=5 Clip 3 kf 72 mit
frac 7, t=12 kf 79, L1 t4->t12 = 3049 = der 7-Bild-Blend, den auch das Original zeigt. "t=4 == t=12"
wird nur verlangt, wenn c1f0 == c3f0 UND Clip 3 ein Bild hat. Kontrolle Tuer zurueck nach ROOM1030
(kein Block): identische Pins gruen. Variante N (ohne W-Bank): RENDER PL00common Clip 22 kf 652.
Pins: `probe_r16_tuer1040_A/_B/_F/_N` (tests/unit/probes/p2_objekte-und-tuer.cmake).

Regressionen (Binaries VOR dem Fix = re15_port/build vom 2026-09-14, NACH = build_p2):
- `probe_hitdoor_entry_anim A`: einziger Unterschied `ENTRY mo=0` -> `mo=210`; alle 120 DEST-Zeilen
  byte-gleich (t=0 `mo=200 frac=7` wie vorher — die Sonde spielt keine Blende ab, also kein Freeze;
  im Sondenkopf dokumentiert). Skeptiker-Punkt 7: Erwartungstext vor dem Fix gelesen (es gibt in der
  Sonde keine Zusicherung "mo=200 ab t=0", nur das Protokoll).
- `probe_elliot_1170_run` (Self-Reenter 1170 -> sub14, Plc_motion des main00) und
  `probe_1090_cutscene`: Ausgabe vor/nach byte-gleich (`PIN: OK`; Endstand (-77,-1997) yaw 111).
  Damit ist Skeptiker-Punkt 7/Dossier-Punkt 2 ("Plc_motion gewinnt") gemessen, nicht angenommen.
- Skeptiker-Punkt 2 (anim_prev_valid): nicht uebernommen; `re15_actor_init` nullt ihn (Sonde: ENTRY pv=0).
- `unit_text_freeze`, `unit_load_after_death`, `probe_1140_reentry`, `unit_cam_1030_reentry`,
  `unit_zombie_10d0_reentry_a/b`, `unit_elliot_1170_run`: gruen (ctest-Summe s. Rueckgabe).

Sichtpruefung (echte Worktree-exe build_p2, Lauf wie §1.1: JUMP 1040@30, Skript ab Tick 600
1040 -> 1030 -> 1040 ueber die echten Tueren slot 1 / slot 2, `RE15_FRAMEDUMP=0-30/1`, Software-Renderer
320x240; Startweg RE15_TITLE_SHOT wie die Integrationstests, weil das Skript sonst im Titel steht):
- `[mot]`-Spur nach der Tuer 1030 -> 1040 (`phase2_objekte-und-tuer/tuer1040_door_phase2_mot.log`):
  F1..F5 `mo=210 af=0 frac=0`, F6 `mo=200 af=0 frac=7`, F7..F12 frac 6..1, F13 frac 0 — vorher (§1.1)
  F1..F5 `mo=0`.
- Bilder `tuer1040_door_phase2_F06/F08/F13.png` (+F01/F05/F10/F20): F06, F08 und F13 sind pixelgleich —
  Leon steht ab dem ersten sichtbaren Bild in der Idle-Pose (Arme unten). Vorher (`tuer1040_door_F06.png`):
  beide Arme seitlich ausgestreckt, F08 mitten im Crossfade.

ctest (build_p2, 310 Tests): 309/310 im Gesamtlauf; der eine rote Test `integration_save_counter_pin`
ist der in `tests/integration/test_save_counter_pin.cmake` dokumentierte Startfehler (exit=1 nach
"[pad] kein Controller gefunden", debug.log 5 Zeilen, Lauf 3 nach einem Wiederholungsversuch) — allein
wiederholt: gruen. Kein Bezug zu den Aenderungen (die exe des Hauptbaums vom 2026-09-14 zeigt in
dieser Sitzung dieselbe Startklasse). Zweiter Gesamtlauf nach den Commits 630e87d2/9a7d1ab0:
`100% tests passed, 0 tests failed out of 310` (219,85 s).
