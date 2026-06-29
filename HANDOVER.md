# RE1.5 Port — Session-Handover (Stand 2026-06-30, Phase 8.10: Combat-Loop komplett + zwei-seitig begonnen)

Kanonisches „lies-mich-zuerst" für die nächste Session. **Die STAGE1-Zombie-Combat-Logik ist byte-true in-game:
spawn → wake (dist<4000) → engage → turn-to-face → GRAB (−10/−5 HP) → Spieler GEPINNT → HP<0 → TOD (death-FSM-Kern,
state 7, 120-Frame-Timer; Fade/Game-Over-Screen deferred).** Der Combat-Loop ist damit end-to-end geschlossen.
Forward-Walk AUFGELÖST: das m0-Live-Brain (das der Port hat) setzt NIE +0x5=6 → der Briefing-Combat (wake→engage→turn→grab→
pin→drain) ist byte-true KOMPLETT für ROOM1140. Der +0x5=6-Walk im Save ist ein Dead-Player-„walk-to-corpse"-Artefakt
(player hp=-1) + ein m1-Varianten-Verhalten (andere Räume). KEINE Live-Lücke; nichts zu portieren für ROOM1140 (§8.10).
**ZWEI-SEITIGER Combat begonnen (Nutzer-gewählt): die Zombie-hurt/death-States ([2]/[3]→corpse) sind portiert (die
empfangende Seite); der Player-Fire-Trigger (Aim/Fire→`re15_resolve_attack`→hurt/death) ist der nächste Chunk — der
Background-Workflow `player-fire-re` REt die Player-Fire-Pipeline.** Ergänzt die Auto-Memory (v.a. `reai-v2-foundation-combat` = die laufende AI-RE, `disasm-verify-decompiles`,
`reai-v2-duckstation-dynamic-re`).

## TL;DR — Wo stehe ich

- **Git:** master, sauber (nur `.idea/` untracked). Phase 8.10 = **6 Commits:** `fb4a06e7` Player-grabbed-Lock ·
  `a7ae354a` Forward-Walk-RE + Root-Motion-Doku-Fix · `8038cc5f` Forward-Walk AUFGELÖST · `05af1a76` Player-DEATH-FSM-Kern ·
  (+ dieser Commit) Zombie-hurt/death-States (zwei-seitiger Combat begonnen). Der Combat-Loop ROOM1140 ist end-to-end
  byte-true geschlossen (spawn→wake→engage→turn→grab→pin→drain→TOD); Fade/Game-Over-Screen = deferred Präsentation.
- **Build/Test:** `taskkill //F //IM re15_pc.exe 2>/dev/null; true; export PATH="/c/msys64/mingw64/bin:$PATH";
  cmake --build re15_port/build; ctest --test-dir re15_port/build --timeout 30` → **31/31 grün** (mingw64 GCC + Ninja).
  (Das `taskkill` ist nötig, falls die Exe noch läuft + die Datei sperrt — sonst Link-„Permission denied", kein Code-Fehler.)
- **⚠️ GROSSE KORREKTUR diese Session:** die in Phase 2–7 portierte AI (`FUN_8011d6d4` → `@0x801217a0`) ist die Typ-**0x47**-
  Familie — NICHT die Live-Zombies. Savestate-bewiesen laufen die Live-Zombies (Typ 0x10/0x11/0x16) über `@0x8011f7b4`
  (`FUN_80100424`/`FUN_80101224`). Ursache: das Decompilat `STAGE1/FUN_80100424.c` ist FALSCH (→ Memory
  `disasm-verify-decompiles` + Katalog „HIGH-VALUE CORRECTIONS"). Phase 8.3 (darauf gebaut) → revertiert; die Live-AI
  auf der korrekten Familie re-rooted (8.5a–d).
- **Die Live-Zombie-AI ist jetzt FUNKTIONAL KOMPLETT** (alles auf `@0x8011f7b4`, byte-true): die volle Attack-Kette
  läuft aus einer ENTSCHEIDUNG — **Tick → INIT → ACTIVE → Decision-Brain committet (+0x5=7) → Arm → Windup →
  Lunge@300 → geteilte Hitbox → HP fällt.**
- **PHASE 8.6 (game_step-Wiring) ERLEDIGT:** `re15_enemy_ai_run_all(g_scd.combat_active)` hängt jetzt am Ende von
  `re15_game_step` (die TYP-gegatete Slice von `FUN_8001a50c` — tickt NUR 0x10/0x11/0x16 → Elliot/Krähen/Player
  unberührt). **1170-sicher** verifiziert: ROOM1240-Boot + ROOM1170 spawnen keine Live-Zombies → No-Op (Headless-Boot
  unverändert). combat_active = neues `g_scd.combat_active` = byte-true `DAT_800aca3c & 1` (gehaltener Latch, beim
  Raum-Load gelöscht). Verifiziert port-seitig via neuem ctest **`test_room1140_combat`** (echte ROOM1140 laden →
  sub00 spawnt 5 Zombies → run_all tickt sie INIT→ACTIVE + dist, sie feeden [Wake-up deferred], armed-Lunge durch
  run_all → HP fällt, 0x47 typ-gegated unberührt) + Live-Headless-Boot ROOM1140 (Zombies ticken, kein Crash). 31/31 ctest.
- **PHASE 8.7 ERLEDIGT (savestate-Ground-Truth + 2 Befunde + Wake-up portiert):**
  - **8.7a Hitbox-Dims (Commit 0283bd2c):** 0x10/0x11 = **400/1440** byte-true aus dem Live-Combat-Save gewired (fielen
    vorher durch zu „keine Hitbox").
  - **⚠️ GROSSER BEFUND — der Lunge-Arm ist DORMANT, die Zombies greifen per GRAB an.** Über ALLE 9 Savestates (inkl. dem
    echten ROOM1140-Combat) ist `DAT_800aca3c & 1` **nie** gesetzt; ROOM1140-SCD macht nur `Set bank1 bit27` (≠ bit31 = das
    Combat-Bit). Der Spieler stirbt mit state 7 = *grabbed*. Der echte In-Game-Kill = der **GRAB** (engage FUN_80102058 →
    +0x5=3/4 → FUN_80102548: Player-Register 0x800aca58 + player+0x93|=1 + **−5 HP/Biss** direkt auf player+0x9a, umgeht
    FUN_80012d60, KEIN DAT_800aca3c-Gate). Mein 8.5d-Lunge-Arm bleibt byte-true als Mechanismus, ist aber dormant.
  - **8.7b Wake-up portiert (dieser Commit):** der feeding-Sub-Mode `@0x8011f80c[6]=0x801018f8` (dist-Gate 0x80103980 +
    +0x6-State-Machine 0x80103a58) → bei `dist<4000` wacht der Zombie zu combat auf (+0x9=0, +0x4=0x201=state1/+0x5=2
    engage). Lying `[8]=0x80101974` = passiv (leeres `jr ra`). In `re15_enemy_ai_live_active` eingehängt; alles disasm-
    selbst-verifiziert. test_room1140_combat Part (5): feeding-Zombie nah → wacht zu engage auf, ferner bleibt schlafend.
- **PHASE 8.8 ERLEDIGT (dieser Commit): die GRAB-Execution portiert = der eigentliche In-Game-Angriff.** Volle 9-Sub-Step-
  Machine `FUN_80102548` (@0x8011f890[3/4]) byte-true RE't (Agent + Selbst-Disasm-Verify); f840[3/4]-LOGIC = No-Op
  (`jr ra`), alles in der f890-ANIMATE-Hälfte. Portiert (`re15_enemy_ai_live_grab`, in den Combat-Dispatch eingehängt):
  die byte-true Damage **[2] IMPACT −10 HP** (0x801026f0) + **[3] BITE −5 HP** (0x80102788, loopt im Original via +0x9c) +
  Sub-Step-Struktur + Exit **[8] +0x4=0x201** (zurück zum engage, 0x80102b90). HP<0 → Player-Death (state 3). Faithful-line:
  Anim-gegatete Sub-Step-Advances + der Bite-LOOP-Count sind Stand-ins (Port = 1 Biss/Grab-Zyklus, der engage re-committet
  während der Spieler in Range bleibt → Damage wiederholt sich). Test Part (6): Zombie im Grab-State → Player −15 HP →
  Exit zu engage. 31/31 ctest, ROOM1140-Headless sauber.
- **PHASE 8.9 ERLEDIGT (dieser Commit): die TURN-to-face portiert + eine Modell-Korrektur.** Der aktive Zombie DREHT
  sich jetzt zum Spieler: `+0x5=7` ist die **TURN**-State (decide [7] 0x80102d20 = Grab-Commit wenn nah+facing(0x200)+
  floor; animate [7] 0x80102dc8 = `rot_y += arc_test(player,0x80)` ±0x80/Frame). `re15_enemy_ai_live_turn` +
  dispatch_decision case 7, in die Combat-Animate-Hälfte eingehängt. **KORREKTUR:** `+0x5=7` ist NICHT der Lunge-Arm
  (meine 8.5d-Kopplung war eine Approximation) — der Arm (FUN_8010ab2c) ist ein SEPARATER, dormanter Dispatch
  (@0x80120208[+0x4=6]); die falsche Kopplung entfernt, `test_live_decision_arm` re-pointed aufs Arm-Primitiv direkt.
  Test Part (7): Zombie bei +0x5=7 90° abgewandt nah am Spieler → dreht sich (rot_y 1024→1664) → committet Grab →
  Player-HP fällt. **OFFEN-Befund:** der Vorwärts-Walk [5/6] ist Anim-Root-Motion-gekoppelt (FUN_8001ad68, Displacement
  aus dem Walk-Anim-Stream) → deferred wie der Player-Walk. 31/31 ctest, ROOM1140-Headless sauber.
- **PHASE 8.10 TEIL 2 ERLEDIGT (dieser Commit): der PLAYER-GRABBED-LOCK — der Spieler wird beim Grab gepinnt.** Byte-true
  RE't (Selbst-Disasm) + portiert: der Grab latcht `0x800aca58 = cmd 5` (FUN_80102548 Sub-Step 0 @0x80102640); der
  Player-Command-FSM dispatcht cmd 5 → `LAB_80036834` (@0x80073f90[5]) = der grabbed-Handler, der den Spieler pinnt
  (`DAT_800acc0e = -floor*1800`) + die per-Typ grabbed-Pose fährt und **NIE das Pad liest** → der Spieler kann nicht
  steuern/weglaufen während des Grabs. Portiert als `re15_player_is_grabbed()` (enemy_ai_common.c) — gesetzt in
  `re15_enemy_ai_live_grab` (jeder Frame im Grab-State), je Frame in `re15_enemy_ai_run_all` neu abgeleitet (= faithful-line
  Release für den deferred Player-grabbed-FSM, kein Soft-Lock). game_step pinnt den Spieler (skippt `re15_player_tick` +
  Collision, wie der Stair-Branch) wenn grabbed. **1170-sicher** (Branch nur erreichbar wenn ein Live-Zombie grabt; ROOM1170/
  1240 = nie). Savestate-verankert: 0x800aca58-Sweep über 9 Saves zeigt cmd 1/4/**7** live (cmd 7 = death im Combat-Death-Save
  = der Grab→Death-Pfad belegt; cmd 5 = der transiente Mid-Grab). test_room1140_combat Part(8): free→grabbed→free. 31/31 ctest.
- **8.10 TEIL 1 (FORWARD-WALK) byte-true RE'd, TRANSLATION deferred:** decide f840[5/6] @0x80102bd0 = `jr ra` No-Op; animate
  f890[5/6] @0x80102bd8 = Setup bei +0x6==0 (motion +0x94 = `+0x5+4` = Walk-Clip 9/10, frame=0, anim_frac=7, `0x800aca58 =
  cmd 6`, Grab-Link 0x800acbcc/d0, SE 0x800453d0(4)) dann per-Frame `FUN_8001ad68` + `anim_set(…,0x200)`. **FUN_8001ad68
  selbst-disasm-verifiziert:** `FUN_8001ae38(entity[0x170],[0x174],&disp)` liest dispVec.x/z aus der **Anim-Daten-
  Displacement-Tabelle** (indiziert über `+0x94` motion / `+0x95` frame — KEINE EMR-Rotations-Keyframes, eine separate
  Displacement-Struktur), dann `M = RotMatrix(rot_y +0x6a)`, `disp = ApplyMatrix(M, disp)`, `x(+0x34)=+0xa0+disp.x`,
  `z(+0x3c)=+0xa2+disp.z`. → Die Vorwärts-Translation ist DATEN-getrieben aus der Enemy-Walk-Anim, die der Port nicht in
  dieser Form lädt → byte-true unmöglich ohne die Anim-Subsystem-Integration (raten verboten). **DEFERRED:** die Enemy-Walk-
  Anim-Displacement-Tabelle (entity[0x170]/[0x174]) in den Step-Pfad wiren. Wo +0x5=5/6 gesetzt wird: in den höheren
  Sub-Mode-Handlern (z.B. @0x801036c0 schreibt 0x601), NICHT in den portierten search/engage/turn-Decides.
- **8.10 TEIL 3 (mid-Combat-Savestate) OFFEN:** der vorhandene Combat-Save ist post-death (cmd 7); einen mid-Grab ziehen
  (cmd 5 + Player-Pin live) → die Sub-Steps + Bite-Count + den Pin gegen den Port prüfen (`re15-room-capture`).
- **Neues Tooling:** Skill **`re15-room-probe`** (echten Raum laden + SCD/AI ticken + State lesen, kein DuckStation —
  genau für die 8.6-Verifikation) + `re15-psx-disasm` um „Decompile-Misstrauen" + Tabellen-Familien-Decode erweitert.
- **Disziplin:** jede Konstante zitiert eine Disasm-Adresse/Datei-Offset; Overlay-`.c` vor dem Portieren disasm-
  verifizieren (eins war diese Session falsch); additiv + getestet; kein `game_step`-Eingriff bis 8.6.

## Die LIVE STAGE1-Zombie-AI (`@0x8011f7b4`-Familie) — Architektur-Karte (byte-true, KORREKT)

Per-Frame-Loop `FUN_8001a50c` (@0x8001ce04 im Main-Update) iteriert `DAT_800acc2c` (stride 0x1f4) und ruft
`@0x80072bac[entity+0x8 type]`. Für die Live-Zombies (Typ **0x10/0x11/0x16**):
```
FUN_80100424  per-frame Tick: pause/skip-Gates + dist@+0x1d0 + dispatch @0x8011f7b4[entity+0x4]
  [0] FUN_80100688  INIT    → state→ACTIVE(1), ai_timer(+0x9c)=0x14
  [1] FUN_80101224  ACTIVE  → IF +0x1d8&0x100 (armed): +0x1da-- ; @0x12c(300) Lunge ; @0 Recovery(0x1503)
                            → ELSE (unarmed): @0x8011f80c[+0x9&0xf] → (sub0) FUN_8010168c → @0x8011f840[+0x5]
                              = das Decision-Brain (FUN_80101b64/de4/2058 — und @0x8011f840 == &@0x8011f80c[13])
  [2/3/4] 0x80105a8c/06ba4/0919c   hurt/death/idle — DEFERRED
ARM = FUN_8010ab2c (Attack-Commit, vom Brain bei +0x5=7): Arc +0x5fc=0x390 ; gated DAT_800aca3c&1 →
      +0x1d8|=0x100 + Seed +0x1da = (rng&0xff)+(rng&0xff)+600  (600..1110 → zählt auf 300 = Lunge)
```
Die Lunge-EXECUTION ist GETEILT + typ-agnostisch (EXE): `FUN_80017fa4` (Biss) liest den GTE-Attack-Point
work `+0x28/+0x2a/+0x2c` (im Port = atk_pt-Skeleton-Bone, 8.1), `FUN_80012d60(500,&pt,0)` = Damage-Resolver.

**Portierter Code** (`enemy_ai_common.c`/`re15_enemy_ai.h`, alles additiv + getestet — `test_enemy_ai`,
`test_damage`, `test_room1140_spawn`):
- **Live-Familie:** `re15_enemy_ai_live_init/active/tick/step`, `re15_enemy_ai_live_arm`,
  `re15_enemy_ai_set_combat_active`. Neues Actor-Feld `ai_attack_timer` (+0x1da). Wiederverwendet:
  `re15_ai_dispatch_decision` (das Decision-Brain, jetzt als LIVE bestätigt — NICHT „System B parallel"),
  `re15_enemy_lunge_begin/tick` + `re15_enemy_attack` + Damage/Hitbox (`re15_damage.c`, geteilt),
  `re15_enemy_update_attack_point` + `re15_skel_bone_to_world` (atk_pt, 8.1), `op_sce_em_set` +
  `re15_enemy_spawn_action` (room1140-Spawn, 8.2 — verifiziert via `test_room1140_spawn`).
- **0x47-Familie (Phase 2–7)** = `re15_enemy_ai_init/tick/active` + `re15_ai_exe_assess/search/turn`
  (`@0x801217a0`/`FUN_8011d6d4`): echte byte-true RE eines PARALLELEN Gegner-Typs — behalten, klar als 0x47
  gelabelt, **NICHT der Live-Pfad**. Leicht zu verwechseln (gleiche Struktur, andere Tabelle).

## PHASE 8 — wo es weiter geht (Integration + dynamische Verifikation)

### 8.1 — atk_pt-Skeleton-Mapping (ERLEDIGT, Commit nach c278b08e)
Geliefert (additiv, getestet, **kein** `game_step`-Eingriff):
- `re15_skel_bone_to_world` (skeleton_common.c) = die **byte-true** Modell→Welt-Bone-Transformation
  (`Ryaw(rot_y)·trans + actor.pos`), herausgezogen aus NPC-Render-Loop + Stair-Fuß-Probe.
- `re15_enemy_update_attack_point` (re15_damage.c) = faithful-line Stand-in: posed das Skelett (QUERY)
  und schreibt die Attack-Bone-Weltpos in `atk_pt_*` (int16). **Mechanismus exakt; Bone = Parameter.**
- Tests: `test_attack_point_mapping` + `test_ai_step_chain` treibt `atk_pt` jetzt durch die reale
  Abbildung. **29/29 ctest grün.**

**Byte-true Lunge-Kette (RE'd 2026-06-29, 2 Agenten + Decompile, trianguliert):**
- **Damage-Punkt** = die live GTE-Weltpos der **8 posed Body-Part-Frames**, jeden Frame von
  `FUN_80019e20` (EXE-Action-Driver) in work `+0x28/+0x2a/+0x2c` geschrieben = `ApplyMatrix(M1,vecA)
  + (+0x60) + ApplyMatrix(M_pose,vecB)`, `M_pose` aus `+0x74 = model_base(+0x188)+bodypart*0xac+0x40`.
  `FUN_80017fa4` liest ihn → `FUN_80012d60(500,&pt,0)`. Body-Part-LUT `DAT_8011f7a4 =
  {00,07,08,0e,02,04,09,0d}` (0x0e/0x08 = Kopf/Oberkörper dominieren). **`FUN_80104178` = Lunge-MOTION
  +Gore (Entity-Block), NICHT der Damage-Punkt** (Katalog 73/77 hatten das konflatiert — korrigiert).
- **Lunge-BEGIN-Trigger** (= das „Anim-Keyframe-Event", das im Port `re15_enemy_lunge_begin` ersetzt):
  Attack-Timer `entity+0x1da` zählt auf **300** → `FUN_8010b274` feuert 8× `FUN_80019d50(8,3,0x16,
  body_part_ptr)` → schreibt Action `0x16` (`sh a2,0(v1)` @0x80019d98) in die Body-Part-Records →
  Action-Selbstlauf `0x16(setup +0xe=0x20)→0x17→0x18(jal FUN_80017fa4)→0x19(Biss)`. Dispatch verifiziert
  `0x80071da4→FUN_80017fa4`, `0x80071d98→LAB_80017eb0`.
- **OFFEN (Integration):** Model-Pool-Index → EMD-Bone (welcher em10/16/47-Bone == Kopf/Kiefer) braucht
  die Model-Pool-Build-Order ODER ein mid-lunge room1140-Savestate (combat_death.sav war post-bite,
  Pool leer). + Live-Wiring von `re15_enemy_update_attack_point` in den Render/Step-Pfad (deferred,
  da Konsument noch nicht in `game_step`).

### 8.2 — room1140-Spawn (ERLEDIGT + byte-true verifiziert, Commit nach Phase 8.1)
**PRÄMISSE KORRIGIERT:** Die Briefing-Zombies kommen DOCH via `Sce_em_set` — aus dem RDT-SCD
**sub00** des Raums (`sub_scd[0]`), NICHT aus einer Overlay-Entity-Liste. RE'd byte-true 2026-06-29
(ROOM1140.RDT sub00 + EXE-Handler `LAB_800420a0` ghidra1:152496-152869, eigener Agent). sub00 hat
10 `Sce_em_set` (5 IF / 5 ELSE) gegated über `Ck zone3 flag 0xd2`; Flag clear (scd_vm_init nullt
Flags) = Erstbesuch = 5 Zombies: **0x16/0x10/0x10/0x11/0x11** an festen Positionen, Posen 0x13
liegend / 0x27 fressend. Da der Port `op_sce_em_set` hat + sub00 läuft, spawnt er sie **schon**.
- **Verifiziert:** neuer Test `tests/unit/test_room1140_spawn.c` lädt die ECHTE ROOM1140.RDT, tickt
  main00+sub00, prüft das 5-Zombie-Roster byte-true (Typ/Pos/rot_y/Pose). **30/30 ctest grün.**
- **Byte-true Fix:** `op_sce_em_set` (scd_vm.c) schrieb `behavior` (pc[3]) nach **state** (+0x4)
  statt **grid_id** (+0x9) — Pre-AI-RE-Mismap; korrigiert (state→0=INIT, grid_id→behavior). Nötig,
  weil die AI auf +0x4 (Main-State) + +0x9 (Sub-Mode/Pose-Sel) dispatcht.
- **OFFEN (cited):** Hitbox-Dims für Typ 0x10/0x11 fehlen in `re15_enemy_apply_hitbox` (nur 0x16/0x47
  da; on-disc-Tabelle ist overlay-gepatcht → braucht room1140-Savestate, `re15-room-capture`). Die
  pc[18]!=0→state=4-Variante deferred (Roster hat pc[18]=0).

### 8.3 — REVERTET + GRAVIERENDE ARCHITEKTUR-KORREKTUR (Commit c3e577e7)
**⚠️ Mein portierter AI-Root ist die FALSCHE Typ-Tabelle.** Phase 8.3 (Spawn-Konstruktor
`re15_enemy_spawn_init`) war aus `RE_15_Quellcode_Overlays/STAGE1/FUN_80100424.c` portiert — das
Decompilat ist **FALSCH/fehlanalysiert** (savestate + raw-STAGE1.BIN-PROVEN). Reverted. Der echte
**`FUN_80100424` ist der PER-FRAME-AI-TICK der LIVE-Zombies (0x10/0x11/0x16)**: dist-cache @+0x1d0 +
Dispatch **`@0x8011f7b4[+0x4]`** (active root **`FUN_80101224`**). `@0x80072bac` ist die Per-Frame-
Tick-Tabelle (einziger Caller = die Per-Frame-Loop **`FUN_8001a50c`** @0x8001ce04, stride 0x1f4); KEIN
Spawn-Konstruktor — Sce_em_set schreibt Felder inline (state=0). Briefing-Zombies-Spawn-State byte-true
= state 0 / grid=behavior (Phase 8.2, Test re-asserted). **Details: RE15_FUN_CATALOG „HIGH-VALUE CORRECTIONS".**
- **Mein Phase-2-7-Port = die Typ-0x47-Tabelle `@0x801217a0` (FUN_8011d6d4→FUN_8011d9f4→da48).** Die
  Live-Zombies nutzen die PARALLELE `@0x8011f7b4`-Familie (FUN_80100424→FUN_80101224). Phase-4s
  „Live-Pfad = FUN_8011d9f4" war eine falsche Inferenz (jetzt savestate-widerlegt).
- **GERETTET (geteilt + korrekt):** die Execution-/Lunge-/Hitbox-/Damage-/Attack-Point-Schicht
  (Phase 6-7). `FUN_80101224` ENTHÄLT den byte-true Lunge-BEGIN (`+0x1da==0x12c(300) → FUN_80019d50
  (8,3,0x16)` über die Body-Part-LUT `@0x8011f7a4`); FUN_80017fa4/80012d60/80019e20 sind typ-agnostische
  EXE. Auch das atk_pt-Skeleton-Mapping (8.1) + der room1140-Spawn (8.2) bleiben gültig.

### 8.5a — Live-AI re-root BEGONNEN (Commit bcada19a)
Portiert byte-true aus STAGE1.BIN (`re15-psx-disasm`), additiv + getestet (`test_live_active_lunge`,
30/30 grün), kein game_step-Eingriff:
- `re15_enemy_ai_live_init` (`FUN_80100688` @[0]): state→ACTIVE(1), `ai_timer`(+0x9c)=0x14.
- `re15_enemy_ai_live_active` (`FUN_80101224` @[1]) — die **Attack-Windup-Hälfte**: wenn `ai_flags &
  0x100` (attack-armed) → `ai_attack_timer`(+0x1da, neues Feld)-- ; bei `==0x12c(300)` feuert die Lunge
  (= das 8× action-0x16-Inject im Original) via die geteilte `re15_enemy_lunge_begin`; bei `==0` →
  Recovery-State (`+0x4`=0x1503=state3/+0x5=0x15, motion 0xb bzw. 0x1f wenn grid&0x80). **Der byte-true
  Lunge-Begin kommt jetzt aus dem RICHTIGEN Handler** (ersetzt den Platzhalter).

### 8.5b — Live-Tick + Integrations-Entry (ERLEDIGT, Commit a8880747)
`re15_enemy_ai_live_tick` (`FUN_80100424`: pause/skip-Gates + dist@+0x1d0 + Dispatch
`@0x8011f7b4[+0x4]` → 0=live_init/1=live_active/[2-4] deferred) + `re15_enemy_ai_live_step`
(Tick + geteilte Lunge-Slice = der game_step-Entry). Full-Chain-Test `test_live_step_chain`
(live spawn→tick INIT→ACTIVE→arm→Lunge@300→geteilte Hitbox→HP 100→90). 30/30 grün.

### 8.5c — Decision-Brain an den Live-Active gekoppelt + Fehllabel korrigiert (Commit e6df8260)
Der un-armed Tail von `FUN_80101224` (@0x80101560+) dispatcht `@0x8011f80c[+0x9 & 0xf]` (Sub-Mode-
Tabelle). **SCHLÜSSEL-BEFUND:** `@0x8011f840 == &@0x8011f80c[13]` — d.h. das Phase-3-„Decision-Brain"
(`FUN_80101b64/de4/2058` @0x8011f840) ist ein SUB-VIEW dieser Live-Tabelle, erreicht via
`@0x8011f80c[0]=FUN_8010168c → @0x8011f840[+0x5]`. Die Combat-Zombies (`+0x9=0`, sub 0) routen dahin.
→ **Phase-3s Brain IST der Live-Decision-Graph, KEIN paralleles System** (Fehllabel „System B =
parallel" korrigiert). Portiert: `re15_enemy_ai_live_active` un-armed (sub 0) → `re15_ai_dispatch_
decision` (committet 0x701→+0x5=7 bei Spieler-in-Range). Test angepasst.

### 8.5d — ATTACK-ARM portiert (ERLEDIGT, Commit b0b973f9) → die Live-AI-Kette ist FUNKTIONAL KOMPLETT
**`FUN_8010ab2c` disasm-VERIFIZIERT** (das `.c` ist KORREKT — das falsche `FUN_80100424.c` hatte
DESSEN Pose-Body geborgt): der Attack-Commit/Arm. AI-Params (Arc `+0x5fc=0x390` live, ≠0x2c8;
+0x5f8=0x60 etc.); gated `DAT_800aca3c & 1` → `+0x1d8|=0x100` (Arm-Bit) + `(rng&1)<<9` + Seed
`+0x1da = rng&0xff + rng&0xff + 600` (600..1110, zählt auf 300=Lunge). `re15_enemy_ai_live_arm` +
`re15_enemy_ai_set_combat_active`, eingehängt: Brain committet (+0x5=7) → arm (once) → Windup →
Lunge@300. **Die volle byte-true Live-Attack-Kette läuft jetzt aus einer ENTSCHEIDUNG** (Tick→INIT→
ACTIVE→Brain→arm→Windup→Lunge@300→geteilte Hitbox→HP fällt), alles auf der korrekten @0x8011f7b4-
Familie. Tests `test_live_decision_arm` + `test_live_step_chain`. 30/30 grün.

### 8.6 — game_step-INTEGRATION (ERLEDIGT, Commit folgt) → die Live-AI läuft jetzt IM Spiel
**`game_step`-Wiring gemacht + verifiziert.** Neue geteilte Funktion `re15_enemy_ai_run_all(combat_active)`
(enemy_ai_common.c) = die TYP-gegatete Slice des Original-Per-Frame-Entity-Loops `FUN_8001a50c` (@0x8001ce04:
iteriert die Entity-Liste, dispatcht `@0x80072bac[type]`): sie tickt NUR die Live-Zombie-Typen 0x10/0x11/0x16
durch `re15_enemy_ai_live_step` (FUN_80100424-Tick + geteilte Lunge-Slice); Elliot 0x47 / Krähen 0x21 / Player /
Props bleiben auf ihren bestehenden Pfaden. Eingehängt am ENDE von `re15_game_step` (nach Player-Move/Collision +
AOT-Scan = Player-Pos settled; die Lunge-EXECUTION landet nach dem AOT-Scan = byte-true zu `FUN_80019e20`
@0x8001ce2c). **1170-sicher:** der ROOM1240-Boot + ROOM1170 spawnen keine Live-Zombies → Typ-Gate macht run_all
dort zum No-Op (Headless-Boot unverändert, kein Crash; ROOM1140-Headless tickt die 5 Zombies sauber durch).
- **combat_active byte-true geklärt (3 Agenten trianguliert):** `DAT_800aca3c & 1` (der Gate, den der Arm
  `FUN_8010ab2c` @0x8010a4f0 `andi v0,v0,0x1` liest) = Flag-Bank 1 (`PTR_DAT_80074664[1]`), gesetzt via SCD
  `Set`(0x22) auf Bank 1 / Bit 31 (runtime-Maske `0x80000000>>31` → kein literaler `ori …,0x1` existiert), gehaltener
  Latch, beim Raum-Load gelöscht (`FUN_800396fc &= 0xffff0000`). Im Port modelliert als neues **`g_scd.combat_active`**
  (dediziertes Bool wie die Schwester-Bit 0x100 = `cut_auto_enabled`; der g_scd-memset beim Raum-Reenter/Init gibt
  den byte-true Raum-Load-Clear gratis). **OFFEN (Savestate):** ob der Prototyp das Bit je SETZT — wenn nie, ist das
  Arm-Gate dormant und die Briefing-Zombies sind ein Feeding-Set-Piece. Default 0 ist byte-true in beiden Fällen.
- **Port-seitig verifiziert:** neuer ctest **`test_room1140_combat`** (Skill `re15-room-probe`): lädt die echte
  ROOM1140, spawnt via sub00 die 5 Zombies, treibt `re15_enemy_ai_run_all` und prüft (1) alle 5 ticken INIT→ACTIVE +
  dist gecacht, (2) sie greifen als gespawnt NICHT an (feeding/lying `grid_id & 0xf` = 6/8 ≠ 0 → Combat-Brain nicht
  betreten, auch mit combat_active=1), (3) ein manuell armed Zombie feuert durch run_all die Lunge → Player −10 HP +
  hurt(2), (4) ein 0x47-Actor bleibt typ-gegated unberührt. 31/31 ctest grün.

### 8.7 — Savestate-Ground-Truth + Wake-up portiert (ERLEDIGT) → das Attack-Modell ist KORRIGIERT
**Aus dem Live-Combat-Savestate (`stage_saves/mzd_stage1_combat_death.sav`, via Skill `re15-savestate-ghidra`
`re15_enemy_state.py`) + 9-Save-Sweep + RE (Agent + Selbst-Disasm-Verify) drei Befunde:**
- **8.7a Hitbox-Dims (Commit 0283bd2c):** 0x10/0x11/0x16 = **400/1440** (Live-RAM-Read `*(entity+0x78)`), byte-true in
  `re15_enemy_apply_hitbox` gewired (0x10/0x11 fielen vorher durch zu „keine Hitbox"). test_room1140_combat assertet es.
- **⚠️ DER LUNGE-ARM IST DORMANT — die Zombies greifen per GRAB an.** `DAT_800aca3c & 1` (das Arm-Gate `FUN_8010ab2c`
  @0x8010a4f0 `andi v0,v0,0x1`) ist über ALLE 9 Savestates **nie** gesetzt; ROOM1140-SCD macht nur `Set bank1 bit27`
  (Maske 0x10 = bit4), **nie bit31** (= das Combat-Bit, Maske 0x80000000>>31 = 0x1). Der Spieler stirbt mit state 7 =
  *grabbed*. → Der echte In-Game-Kill = der **GRAB**: engage `FUN_80102058` (+0x5=2, schon portiert als
  `re15_ai_decide_engage`) committet bei dist<0x4b0 && arc(0x200) && same-floor && facing den Grab (+0x5=3/4) →
  `FUN_80102548` (@0x8011f890[3/4]): setzt Player-Register `0x800aca58 = (+0x5−3)<<8|5` + `player+0x93|=1` (grabbed) +
  zieht **−5 HP/Biss** direkt auf `player+0x9a` (wenn das Bite-Anim-Frame landet, `anim_set` a3=0x200), umgeht FUN_80012d60.
  Alles byte-true selbst-disasm-verifiziert (0x80102620/0x80102640/0x801027cc-e4). **8.5d-Lunge-Arm bleibt byte-true als
  Mechanismus, ist aber dormant.**
- **8.7b Wake-up portiert (dieser Commit):** feeding `@0x8011f80c[6]=0x801018f8` (Stage-A 0x80103980: `dist<0xfa0(4000)` &&
  +0x6==0 → +0x6=1, +0x9c=rng&0xf; +0x6-Machine 0x80103a58: 1=Timer-Countdown → 2=Stand-up-Anim → 3=COMMIT 0x80103b3c:
  +0x9=0, +0x4=0x201=state1/+0x5=2 engage, +0x93&=~1). Lying `[8]=0x80101974` = passiv (leeres `jr ra`, kein Dist-Gate).
  `re15_enemy_ai_live_feeding` + Sub-Mode-Dispatch in `re15_enemy_ai_live_active`. Faithful-line: die Phase-2-Stand-up-Anim-
  Dauer ist die deferred Anim-Schicht (Port advanced 2→3 direkt); die Phase-0-Busy-Writes (+0x93|=1/+0x1b8=1) deferred
  (Port-Feld-Aliasing). test_room1140_combat Part (5): feeding-Zombie nah am Spieler → wacht zu engage auf, ferner schläft.

### 8.8 — die GRAB-Execution portiert (ERLEDIGT) → der eigentliche In-Game-Angriff landet
**`FUN_80102548` (@0x8011f890[3/4]) vollständig byte-true RE't** (Agent + Selbst-Disasm-Verify): eine 9-Sub-Step-Machine,
dispatched auf entity+0x6 (Jump-Table @0x80100024; das Grab-Commit-Word 0x301/0x401 resettet +0x6=0). Die f840[3/4]-LOGIC-
Hälfte `0x80102540` ist ein **No-Op** (`jr ra`) — die ganze Grab-Arbeit ist in dieser f890-ANIMATE-Hälfte. Sub-Steps:
[0] init/latch (Player-Register 0x800aca58=cmd5 + player+0x93|=1 + Motion +0x94) · [1] pull-in (anim-gated) · **[2] IMPACT
`player.hp -= 10`** (0x8010277c) · **[3] BITE `player.hp -= 5`** (0x801027dc, LOOPT via +0x9c=0x6e-Fenster, −5/Bite-Anim-
Frame) · [4] hold · [5] struggle (Alt-Exit 0xb01 wenn Spieler sich freikämpft) · [6] release (player+0x93&=~1) · [7] recover ·
**[8] EXIT `+0x4=0x201`** (zurück zum engage, +0x0&=~0x1000; 0x80102b9c). Player-Seite: 0x800aca58 cmd 5 → LAB_80036834
(pinnt+animiert den Spieler); HP<0 → Damage-Entry cmd 3 (death) → der äußere state 7.
- **Portiert (`re15_enemy_ai_live_grab`, in den Combat-Dispatch grid 0 als f890-Hälfte eingehängt):** die byte-true Damage
  (−10 IMPACT + −5 BITE) + die Sub-Step-Struktur + der Exit (+0x5=2) + HP<0→Player-Death(state 3). **Faithful-line:** die
  anim-gegateten Sub-Step-Advances ([1]/[3]/[5]/[7]) + der Bite-LOOP-Count sind Stand-ins → Port = **1 Biss/Grab-Zyklus**;
  der engage re-committet den Grab solange der Spieler in Range/facing bleibt → Damage wiederholt sich über Zyklen.
  **DEFERRED (cited):** der Player-grabbed-Pose/Lock-FSM (0x800aca58 cmd5 → LAB_80036834) + das player+0x93|=1-grabbed-Flag
  (Port-Feld-Aliasing zum Hit-Guard) + die Grab-Motion +0x94 + die Grab-Link-Globals 0x800acbcc/d0 — Player-Subsystem + Anim.
- **Test Part (6):** Zombie direkt im Grab-State (grid 0, +0x5=3, +0x6=0) nah am Spieler → run_all durch die Sub-Steps →
  Player HP 100→85 (−15 byte-true) → Exit zu engage (+0x5=2). 31/31 ctest, ROOM1140-Headless sauber.

### 8.9 — die TURN-to-face portiert (ERLEDIGT) + die +0x5=7-Modell-Korrektur
**Agent + Selbst-Disasm-Verify** der aktiven Zombie-Bewegung. Dispatch: FUN_8010168c ruft je Frame die DECIDE-Hälfte
@0x8011f840[+0x5] DANN die ANIMATE-Hälfte @0x8011f890[+0x5] (gepaart, +8 Bytes versetzt). Bewegung NUR in den Animate-
Hälften (anim_set selbst bewegt NICHT — nur Frames/Pose). State-Split byte-true:
- **TURN [7]** (decide 0x80102d20 / animate 0x80102dc8): keine Translation, nur Drehung. animate = `+0x6a += arc_test(
  playerX,0x80)` (±0x80 ≈ 7°/Frame zum Spieler, @0x80102ee0). decide = der Grab-Commit (`player+0x93==0 && dist<0x4b0 &&
  arc_test(playerX,0x200)==0 && floor==`) → +0x4=(facing+3)<<8|1 (= grab 0x301/0x401). Also: dreht bis facing → grabt.
- **FORWARD-WALK [5/6]** (0x80102bd8): nutzt `FUN_8001ad68` = **Anim-Root-Motion** (Displacement aus dem Walk-Anim-Stream
  via FUN_8001ae38, indexiert by +0x94/+0x95, GTE-rotiert by Heading + +0xa0/+0xa2). **KEIN konstanter Step** → deferred
  (braucht die Enemy-Walk-Anim + Root-Motion, wie der Player-Walk). Der konstante Walker FUN_800245d8 (Speed +0x8c) wird
  nur für Back-Step/Recover genutzt, nicht den Vorwärts-Walk.
- **search [1] / engage [2]:** stehen + tracken (Yaw-Slew FUN_8001aac4 auf das AI-Target +0x1bc/+0x1be), keine Translation.
- **Yaw-Slew FUN_8001a8f8/aac4:** slewt +0x6a zum Bearing (atan2) um ±param_2/Frame, Overshoot-Korrektur. Turn nutzt 0x201.
- **⚠️ MODELL-KORREKTUR:** `+0x5=7` ist die **TURN**-State, NICHT der Lunge-Arm (meine 8.5d-„+0x5=7→arm"-Kopplung war eine
  Approximation). Der Arm (FUN_8010ab2c) ist ein SEPARATER, dormanter Dispatch (@0x80120208[+0x4=6]; DAT_800aca3c&1 nie
  gesetzt, 8.7). Die falsche Kopplung entfernt; `test_live_decision_arm` re-pointed aufs Arm-Primitiv direkt.
- **Portiert:** `re15_enemy_ai_live_turn` (animate [7]) + `re15_ai_dispatch_decision` case 7 (decide [7] grab-commit), in
  die Combat-Animate-Hälfte von `re15_enemy_ai_live_active` eingehängt (decide dann animate; +0x5=3/4→grab, +0x5=7→turn).
  Reine rot_y-Mutation (byte-true ±0x80-Rate); die within-±0x80-Fein-Slew ist moot (der ±0x200-Grab-Commit feuert zuerst).
- **Test Part (7):** Zombie bei +0x5=7, 90° abgewandt, nah (dist 600) → dreht sich (rot_y 1024→1664) → Grab-Commit →
  Player-HP fällt. 31/31 ctest, ROOM1140-Headless sauber.

### 8.10 — Player-grabbed-Lock + Player-DEATH + Zombie-hurt/death ERLEDIGT; ZWEI-SEITIGER Combat begonnen

**RICHTUNG (Nutzer-gewählt): ZWEI-SEITIGER Combat (Spieler schießt → Zombie hurt/death).** Bottom-up gebaut: zuerst die
empfangende Seite (Zombie-hurt/death), dann der Trigger (Player-Fire). Ein Background-Workflow `player-fire-re` REt die
Player-Fire-Pipeline (Aim/Fire-FSM @0x80073f90 + Fire→Damage-Trigger + Port-Input-Seite) für den nächsten Chunk.

**TEIL 5 (Zombie-hurt/death-States) PORTIERT (dieser Commit, 31/31 ctest):** die @0x8011f7b4-AI-Dispatch ist jetzt KOMPLETT
([0]init/[1]active/[2]hurt/[3]death/[4]idle-deferred/7=corpse). `re15_enemy_take_damage` (existiert) setzt state 2 (HURT) bei
Treffer / 3 (DEATH) bei HP<0; der Tick dispatcht jetzt: **[2] HURT** (`re15_enemy_ai_live_hurt`, FUN_80105a8c) → zurück zu
ACTIVE (byte-true `+0x4=1` @0x80105b48; das 2D-Stagger-Anim-FSM @0x8011fb90 + die Hit-Stun-DAUER = Reaktions-Clip-Framecount
= deferred Anim); **[3] DEATH** (`re15_enemy_ai_live_death`, FUN_80106ba4→0x80107cb0) → state **7** = CORPSE (byte-true
`+0x4=7` @0x80107ec8; out-of-range → kein Dispatch = inerter Corpse, kann nicht mehr engagen/grabben; Death-Anim+SE+Gore
deferred). **1170-sicher** (nur dispatcht wenn ein Treffer state 2/3 setzt = der kommende Player-Schuss). test Part(10):
Zombie-Schuss → HURT(2)→ACTIVE; tödlich → DEATH(3)→CORPSE(7) inert (kein Player-Damage). **Nächster Chunk: der Player-Fire-
Trigger** (Aim/Fire-Input → Hitbox → `re15_resolve_attack` [existiert] → diese hurt/death-States), aus dem Workflow-Ergebnis.

**TEIL 4 (Player-DEATH-FSM-Kern) PORTIERT (31/31 ctest):** der Grab TÖTET jetzt den Spieler. Byte-true:
HP<0 = Tod (FUN_80012d60 @0x80012ee8); der Grab erreicht dasselbe HP<0 → setzt **grabbed-death state 7** (save-bestätigt:
combat_death.sav `0x800aca58 = 7`; generic death = state 3 @0x800366bc, grabbed-death = state 7 @0x8003694c). Portiert
(re15_damage.c): `re15_player_is_dead()` (=HP<0) + `re15_player_death_tick()` (der byte-true Death-Sequenz-Timer **0x78=120**,
@0x8003694c INIT `DAT_800acaf2=0x78`) + `re15_player_death_reset()` (in re15_actor_init). game_step: neuer Branch
`else if (rdt_ok && re15_player_is_dead())` VOR dem grabbed-Branch — friert den Spieler ein (skip `re15_player_tick`+Collision,
liest kein Pad = exakt der originale Death-FSM) + tickt den Timer; RVD-Cam läuft (Death-Kamera). **1170-sicher** (Branch nur
erreichbar wenn ein Hit HP<0 treibt = der Grab in ROOM1140; gesunder Raum unberührt). Death hat Vorrang vor dem Grab (der
Zombie dead-grabt dann die Leiche — die hp<0-dead-grab-Arm der engage). test_room1140_combat Part(9): Grab-Kill → HP -2, dead
→ state 7, Timer 0x78→0x77. **DEFERRED (zitiert, Port hat keine Fade/Game-Over-Infra):** der Color-Fade + Death-Kamera
(@0x8003694c), der „gefressen"-Anim-FSM (@0x8010a28c, verschachtelt), der Game-Over-Screen (sub-state 2).

**TEIL 2 (Player-grabbed-Lock) PORTIERT (dieser Commit, 31/31 ctest):** der Grab pinnt jetzt den Spieler. Byte-true
selbst-disasm-verifiziert: der Grab latcht `0x800aca58 = ((+0x5-3)<<8)|5` = **cmd 5** (FUN_80102548 Sub-Step 0 @0x80102640),
der Player-Command-FSM dispatcht `@0x80073f90[cmd]` → `[5]=LAB_80036834` (@0x80036834): pinnt den Spieler (Init bei
DAT_800aca5a==0: `DAT_800acc0e = -floor*1800` grabbed-Y; `DAT_800acb04=0`), holt die greifende Entity (`DAT_800acbfc`,
gesetzt vom Walk/Grab-Setup), dispatcht eine **per-Typ grabbed-Pose** (`@0x800ac758[enemy.type]`) — liest NIE das Pad. Der
Exit [8] @0x80102b90 clear-t cmd 5 NICHT (Latch; der Player-grabbed-FSM resettet ihn beim Anim-Ende/Struggle = deferred).
Portiert: `re15_player_is_grabbed()` (enemy_ai_common.c) — `s_player_grabbed` gesetzt in `re15_enemy_ai_live_grab` (jeder
Frame im Grab), in `re15_enemy_ai_run_all` je Frame genullt = faithful-line Release (grabbed ⟺ ein Live-Zombie grabt diesen
Frame → kein Soft-Lock; wenn der engage nicht mehr re-committet [Spieler aus dem ±0x4b0-Kegel], ist der Spieler frei).
game_step: neuer Branch `else if (rdt_ok && re15_player_is_grabbed())` pinnt den Spieler (skip `re15_player_tick` +
Collision + Door-Scan, RVD-Cam-Scan läuft weiter — exakt der Stair-„engine-driven, kein Steuern"-Pattern). **1170-sicher**
(Branch nur erreichbar wenn ein Live-Zombie grabt). test_room1140_combat Part(8): `free=1 before → grabbed=1 during → free=1
after`. **Savestate-verankert:** 0x800aca58-Sweep (re15_flag_sweep.py) über 9 Saves = cmd 1/4/**7** live (cmd 7 = death im
mzd_stage1_combat_death.sav = der Grab→Death-Pfad belegt; cmd 5/6 = transiente Mid-Grab/Walk-Werte).
**DEFERRED (zitiert):** die per-Typ grabbed-Pose/-Anim (`@0x800ac758[type]`), der exakte XZ/Y-Pin (`DAT_800acc0e`), der
Struggle-Escape (Sub-Step 5 @0x80102968 = anim-gegatet + bit-0x2-Check), `player+0x93|=1` (Feld-Aliasing zum Hit-Guard).

**TEIL 1 (FORWARD-WALK) — exhaustiv RE'd (Workflow + Savestate-Arbiter), NICHT portiert (zwei Blocker, raten verboten).**
Die Mechanik ist vollständig byte-true; der Port hat den Reader sogar schon — aber zwei Teile sind nicht byte-true auflösbar,
darum bewusst NICHT portiert. Drei Befunde:
- **TRANSLATION (geklärt, Reader EXISTIERT im Port):** animate f890[5/6] @0x80102bd8 (decide @0x80102bd0 = `jr ra` No-Op):
  Setup bei `+0x6==0` (motion `+0x94=+0x5+4` = Walk-Clip 9/10, `0x800aca58 = cmd 6`, SE) → per-Frame `FUN_8001ad68`.
  `FUN_8001ad68`/`FUN_8001ae38` (selbst-disasm + Workflow + Format-Ref **dreifach** verifiziert): lesen die **Root-SPEED**
  aus dem **EMR-Keyframe** (RE15_KNOWLEDGE.md §5.2 L461 „Per keyframe: 12B position+speed"): `dx = s16 @ keyframe+6`,
  `dz = s16 @ keyframe+10` (beide FUN_8001ae38-Pfade lesen konsistent +6/+10); dann `RotMatrix(rot_y +0x6a)` · `ApplyMatrix`
  · `x(+0x34)=base(+0xa0)+disp.x`, `z(+0x3c)=base(+0xa2)+disp.z`. **Der Port liest diese Bytes BEREITS:**
  `re15_emd_get_keyframe_speed` (emd_common.c) gibt +6/+8/+10 zurück (für die ROOM1170-Cinematic gebaut, aber ungenutzt).
  → **Die alte Port-L3-Doku „kein Root-Motion in Keyframes / bytes 6..11 UNREAD" ist FALSCH** (durch den eigenen Port-Code +
  §5.2 + die Disasm widerlegt); dieser Commit korrigiert die actor_locomotion.c-Kommentare. Player-Walk nutzt korrekt
  Scalar-Speed (FUN_800245d8, +0x8C); FUN_8001ad68 hat NULL EXE-Caller (Overlay/Enemy-only). Quelle: das Enemy-Modell-File
  (EMS/EMD) — der Loader FUN_80022300 löst die Sections auf (`entity[0x170]=B+dir[4]`=EMR, `entity[0x174]=B+dir[3]`=EDD).
- **✅ TRIGGER AUFGELÖST (dieser Turn): der Forward-Walk ist KEINE Live-ROOM1140-Lücke — der Briefing-Combat ist komplett.**
  Variant-Schicht: `entity[+0x9]&0xf` wählt 13 Dispatcher @0x8011f80c (m0=FUN_8010168c/DECIDE 0x8011f840 [Port-Wurzel],
  m1=FUN_80101708/DECIDE 0x8011f8e0). Der einzige immediate `ori 0x601`-Write im GANZEN Overlay ist @0x801036c0 = m1
  DECIDE[0]. **Schlüssel-Befund 1 (byte-true):** die feeding-WAKE-up committet `sb zero,9` → grid_id **0 = m0** (@0x80103b3c)
  — die Briefing-Zombies wachen zu m0 auf, sind **nie m1**. **Schlüssel-Befund 2 (exhaustiv getract):** das m0-LIVE-
  Entscheidungs-Brain (engage-decide @0x80102058 + search-decides 0x80101b64/de4 — ALLE im Port als `re15_ai_decide_*`)
  produziert +0x5 ∈ {7 turn, 3/4 grab, 9/10 contact, 12 dead-grab, 16} — **NIE 6**. Also forward-walkt ein LIVE (Spieler
  lebt) m0-Briefing-Zombie NIE; er macht wake→engage→turn→grab (genau was der Port tut). **Schlüssel-Befund 3 (Savestate):**
  das +0x5=6 in mzd_stage1_combat_death.sav ist ein **DEAD-PLAYER-Artefakt** — der Save ist post-death (player hp=-1, cmd 7);
  die Distanz-Progression slot 2 (dist 4492) engage(2) → slot 1 (dist 1476) dead-grab(+0x5=12, das die engage NUR bei
  `hp<0` setzt) → slot 0 (dist 357) walk(+0x5=6, motion=10) = ein „walk-to-corpse" nach Spielertod, KEIN Live-Combat-Pfad.
  → **Der Forward-Walk (+0x5=6) ist ein m1-Varianten-Verhalten (andere Räume/Gegner) + ein Dead-Player-Edge — KEINE Live-
  Lücke für ROOM1140. Der Port-Combat dort ist byte-true KOMPLETT.** Disziplin-Win: drei Turns lang das +0x5=6 eines
  **post-death** Saves verfolgt — die Lehre: ein post-death-Save ist atypisch; für „was tut der LIVE-Combat" MUSS der
  Referenz-Save den Spieler am Leben haben ([[disasm-verify-decompiles]]).
- **Translation-Reader EXISTIERT + Enemy-Anim WIRD geladen** (Blocker B war keiner): die Enemy-Bank (`re15_enemy_bank_t`,
  re15_enemy.h) trägt `skel` (EMR+Keyframes) + `anim` (EDD) — beim Zombie-Spawn geladen; `re15_emd_get_keyframe_speed`
  liest die Walk-Speed +6/+10. D.h. WENN der Forward-Walk je gebraucht wird (m1-Gegner in einem anderen Raum), ist die
  Translation in ~10 Zeilen wirbar: motion=walk-Clip → `re15_emd_get_keyframe_speed(&bank->skel, kf, …)` → RotMatrix(rot_y)
  · speed + pos. Aber für ROOM1140 ist NICHTS zu tun.
- **OPTIONALE finale Bestätigung (kein Blocker):** ein mid-**LIVE**-Combat-Save (`re15_mzd_load_room.py --provoke S` mit
  KURZEM S, Spieler lebt) → bestätigen, dass die Live-m0-Zombies nur turn/grab erreichen (nie +0x5=6). Braucht zuerst den
  ROOM1140-JUMP-Index (--triangle/--right, nicht dokumentiert → eine --menushot-Erkundung). Niedrige Priorität: die statische
  RE ist eindeutig (das Live-Brain hat keinen Walk-Pfad).

**TEIL 3 (mid-Combat-Savestate) OFFEN:** der vorhandene Save ist post-death (cmd 7); einen mid-Grab ziehen (cmd 5 +
Player-Pin live) → Sub-Steps/Bite-Count/Pin gegen den Port prüfen (`re15-room-capture`; provoke ist timing-sensitiv).
- **Verbleibende byte-true Details:** Sub-States [2]/[3]/[4] (0x80105a8c/06ba4/0919c = hurt/death/idle), FUN_8001bc08-
  Sensor/+0x1d8-Update, AI-Pause-Gate (DAT_800aca40 & 0x20000000 → `re15_enemy_ai_set_paused`, noch nicht in game_step).
- **WAS VOM 0x47-PORT BLEIBT:** der `@0x801217a0`-Code (Phase 2-7) ist echte byte-true RE eines
  PARALLELEN Typs (0x47) — nicht wegwerfen, klar als 0x47 gelabelt; der Live-Pfad ist `@0x8011f7b4`.

Werkzeuge: **`re15-psx-disasm`** (EXE/Overlay-Disasm), **`re15-savestate-ghidra`** (Live-RAM +
Tabellen-Patch-Check), **`re15-room-capture`** (Raum laden/provozieren). Memory `reai-v2-foundation-combat`
hat die volle Detail-Karte (alle Adressen, FSM-Stufen, deferred-Teile).

---
*Historischer Handover (2026-06-28, der 44-Fix-Byte-true-Audit) — unten als Referenz erhalten.*

## TL;DR (2026-06-28, historisch)

- **Git:** HEAD war `0aed072b`; Code-Work in `3b4be5b8 "Many things finished"`.
- **Build/Test:** damals 28/28 grün.
- **Audit-Arbeitsweise:** jeden Audit-Fix VOR Anwendung gegen die Disasm verifizieren
  (≈70 % der Audit-Werte waren ungenau), dann anwenden → build → ctest.

## Diese Session geliefert (alles gebaut + 28/28 ctest grün)

### Byte-true Fixes — Wave 2 (#28/#32/#34/#36)
- **#28** Rotor-SE (rotor_common.c): Pan `& 0x7f` statt clamp (andi @0x80045d28/d3c); Distanz mit
  Y-Term `cam.y-|cam.y-heli.y|` + verschachteltem SquareRoot0 (@0x80045b04/b34/b38).
- **#32a** RVD-Zonen-Cap 32→64 (re15_rdt.h; FUN_80014230 hat kein 0x20-Limit). **#32b** FLR
  width/depth signed `lh` (@0x800437fc/0x80043814). #32c (Terminator-nach-Advance) **deferred**
  (OOB-Risiko, nur Leer-FLR-Edge).
- **#34** SCD 0x5F–0x63 RE2-Handler entfernt + Größen 0x5F–0x8E→1 (Tabelle endet 0x5E @0x80074620).
- **#36a** Dialog-Dismiss `& 0xc000` Cross|Square Edge (@0x80028458/698). **#36e** Page-Resume
  `message_timer=message_scroll` (@0x80028538). #36b/c/d **deferred** (Glyph-Cap braucht
  Font-Atlas-Check; FF/Lookahead = Mechanismus-Umbau).

### Byte-true Fixes — Wave 3 (#6/#7)
- **#6** Cmp (0x23): var_idx `pc[1]→pc[2]` (andi 0xff @0x8003ff80) + Operator-Cases 2/3/4 →
  `>=`/`<`/`<=` (switchD_8003ffc0). RISIKO: PSX-Boot-Recheck wert (ROOM1170-Boot hat aber kein Cmp).
- **#7** Save/Copy/Calc/Calc2 (0x24–0x27): work_vars[256] existierte schon; ALU-Helfer
  `scd_work_alu` = 12-Op-Tabelle FUN_80040140 (inkl. SHR-logical 0xA / SHR-arith 0xB, Div-durch-0=kein Write).

### #8 — Loop-Modell-Umbau (KOMPLETT, byte-true)
- **Unified Modell** in `scd_thread_t`: `loop_back[4]`/`loop_exit[4]`/`loop_for_cnt[4]`/`loop_count`
  (PSX +0x20/+0x60/+0xA0/+0x08) ersetzt do_stack/for_*.
- **7 Handler** byte-true: For(0x0D)/Next(0x0E)/**While(0x0F)**/**Ewhile(0x10)**/Do(0x11)/Edwhile(0x12)/**Break(0x1A)**.
- **`scd_eval_pred_chain`** — AND/OR-Prädikat-Kette via echtem Dispatcher; Returns auf
  `==SCD_R_CONTINUE` normalisiert (Prädikate liefern 1/3, nicht 0/1).
- `op_if/else/endif` + `block_stack` BLEIBEN getrennt (eigenes Subsystem).
- **Neue byte-true Tests:** `test_do_edwhile_loop` + `test_break_exits_loop` (test_scd_opcodes.c).
- Regressionsbeweis: For/Next-Logik identisch (nur Feld-Rename), Do/Edwhile-Ck byte-identisch, Rotoren=Goto.

### DuckStation Dynamic-RE Skills (live verifiziert)
- **`re15-room-capture`** (`.claude/skills/`): vgamepad-Treiber lädt einen RE1.5-Raum übers
  Debug-Menü (`--right N --triangle M --postload S`, Cross-Stage braucht ≥16) → SaveStateOnExit
  → `re15_capture.py` macht Capture+Auto-Verify mit Schwarzbild-Erkennung.
- **`re15-savestate-ghidra`** (`.claude/skills/`): `re15_ss.py` (zstd→RAM-Base 0x31a62→Reads+VRAM-PNG),
  `re15_savestate_inspect.py` (Stage-Overlay-Fingerprint @0x80100000 + HP/State/Cut/Flags),
  `re15_diff.py` (unbekanntes Global finden), `re15_ghidra.py` (Headless-Decompile-Wrapper, gecacht).
- Demo-Savestates: `stage_saves/mzd_demo/` (Stage 1 fp 0x96290818, Stage 2 fp 0x1c0394d0).
- **Nutzen:** riskante Fixes (#6 PSX-Boot, #13 Damage, #14/15 AOT) jetzt laufzeit-verifizierbar,
  ohne dass der Nutzer selbst spielt.

### Repo-Hygiene
- ~15.000 Build-Artefakte aus dem Index entfernt (`re15_port/build`, das versehentliche
  `re15_port/cmake/build` mit 7888 Dateien, `psx_dev/.../build*`). `.gitignore` robust:
  `build/`, `.claude/worktrees/`, `debug.log`, `.idea/`. **NIE `build/`-Dateien committen.**

### #9 — Switch/Case/Default/Eswitch (KOMPLETT, byte-true)
- **op_switch (0x13, LAB_8003fa5c)** scannt jetzt die GANZE Case/Default-Tabelle inline und springt
  direkt auf matched-case-body / default-body / past-eswitch (der Audit-Hinweis „Case off-by-2" war
  zu klein gegriffen — der echte Mechanismus liegt im Switch-Handler). Pusht/poppt das unified
  Loop-Modell (#8): Break (0x1A) verlässt den Switch über `loop_exit`, Eswitch (0x16) balanciert
  `loop_count`. **op_case=+6 (reiner Fall-through), op_default=+2, op_end_switch=loop_count--/+2.**
- **Index-Konvention belegt** gegen op_do (LAB_8003f8bc) + op_break (LAB_8003fca8): Push-am-neuen-Index,
  Break-liest-aktuellen, Pop-dekrement (PSX 1-indexiert → Port konsistent 0-indexiert). `switch_val`-Feld
  entfernt (kein Konsument mehr). Verifikation: eigener Disasm-Trace + Agent (alle 5 Claims CONFIRMED).
- **Neuer Test:** `test_switch_case_break` (3 Pfade: Match-mid-table+Break, erstes-Case-Match, No-Match→Default;
  loop_count balanciert je auf 0). **26/26 ctest grün.**

### #10 — Evt_chain (0x03) + Evt_exec (0x04) (KOMPLETT für Normal-Modus, byte-true)
- **Evt_chain (0x03, LAB_8003f270 → FUN_8003edec)** war ein No-Op (chained Sub lief NIE). Jetzt
  **in-place Thread-Reinit**: `pc=sub_scd[pc[3]]`, `loop_count=0`, `block_sp=0`, KEIN pc+=4;
  call_depth/work/locals/sleep BLEIBEN (FUN_8003edec macht KEIN memset — der Audit-„Reset call_depth/sleep"
  war falsch).
- **Evt_exec (0x04, LAB_8003f2a0 → FUN_8003ee3c)**: `cond=pc[1]`, `sub_id=pc[3]`, pc+=4, dann Slot-Wahl.
  Byte-true Normal-Modus (DAT_800b3f7a==0, agent-belegt): **`cond<10` → Slot=cond DIREKT** (force-overwrite) —
  fixt den room1021 `Evt_exec(9,0)`-Drop. `cond>=10` → Auto-Scan.
- **#24-Kopplung:** Die exakte PSX-Auto-Scan-Range [2..9]/[10..13] + Mode-Flag hängt am Thread-Pool-Umbau
  (24→14, #24). Port-Auto-Scan bleibt [10..23] (Slot 2 dediziert) bis #24 — ROOM1170-Intro Evt_exec(255,11)
  unverändert → keine Regression.
- **Verifikation:** eigener Disasm-Trace + Agent (4/4 Claims CONFIRMED inkl. FUN_8003edec-Felder + 5 DAT_800b3f7a-Writer).
  Neue Tests `test_evt_chain_reinit` + `test_evt_exec_direct_slot` (RDT-Mock). **26/26 ctest grün.**

### #13 — Player-Damage (KOMPLETT für Player-Branch, byte-true)
- **Neu:** `engine/src/re15_damage.c` + `include/re15_damage.h`; actor-Felder `hit_react`(+0x93)/`status_flags`(+0x98).
  `re15_player_take_damage(p, attack_type, src_x, src_z)` = Player-Branch von **FUN_80012d60** (@80012e18-f04).
- **Mechanik (instruktionsgenau, `ghidra1_V2.txt:77607-77814`, Disasm == Decompile):** Hit-Once-Gate
  `+0x93 bit0` (@80012e30 → 1 Treffer/Attacke) → `HP -= re15_damage_table[type]` (HP=s16@+0x9a, @80012e44-64) →
  `type<2`: Bleed/Gift-Roll `+0x98 bit0x2` via 2×RNG&1 (@80012ea4) + SE#10 (deferred) → state=2(hurt) /
  sub_state_1=Front/Back+2 / sub_state_2=0 / `+0x93|=1` → **signed HP<0 → state=3(death)**, sub-states 0.
- **Tabellen byte-true:** `DAT_8006f418` (dmg) = **{10,20,1000,1000,1000,50,100,200,300,1000,0}** — **11** Einträge
  (`[10]=0`, ghidra:223455-223478; der Audit listete nur 10). `DAT_8006f430` (Reaktion) = {03,03,09,0A,0B,0E,0F,10,11,12,14}.
- **Caller:** `FUN_80017fa4` (Gegner-Lunge, Dispatch-Tab @0x80071da4) → `FUN_80012d60(0x1f4, hitbox, 0)`,
  `clear a2` @80018004 = **attack_type 0 = 10 dmg** (Zombie-Biss). Hitbox = Angreifer-Pos +0x28/+0x2a/+0x2c via FUN_8001c6e8.
- **DEFERRED (ehrlich):** Gegner-Attack-FSM (@0x80071da4) + Actor-Hitbox `FUN_8002b5d0` sind NICHT portiert (Port hat
  keine Gegner-AI/Hitbox) → nichts ruft den Resolver in-game; er ist der getestete byte-true Kern für die Gegner-AI-Phase.
  Hit-Once-Clear (`re15_player_clear_hit_guard`) + 2. Call-Site @800185b8 (Audit „a2=2 Instakill", ungeprüft) = selbe FSM.
  Front/Back-Anim faithful abgeleitet, unbeobachtbar bis Hurt-Clips. Bleed-RNG cycle-exakt unmöglich (leftover-Register),
  1/4-Wkt erhalten. Verwandt: **#22** (Idle-FSM auf state==Idle gaten), **#41** (Poison-Drain HP-=2 — Status wird jetzt gesetzt).
- **Verifikation:** eigener Disasm-Trace (LAB-für-LAB) + Damage-/Reaktions-Tabelle direkt aus den Daten-Bytes gelesen.
  Test `tests/unit/test_damage.c` (6 Tests: 10-dmg-Biss, Hit-Once-Sperre+Re-arm, HP<0→death, Instakill, Bleed-Gate
  507/2000≈1/4, Tabelle byte-true). **27/27 ctest grün; Headless-Boot ~10s ohne Crash.**

### #11 + #12 — Member_set/cmp id-Tabelle (KOMPLETT, byte-true)
- **Kern:** Der Port übersetzte RE1.5-Member-ids → RE2-ids (`re15_to_re2_member_id`) — eine **Fiktion**. RE1.5 nutzt
  die DIREKTE Tabelle `FUN_8004116c` (set, ghidra:151322-151380) / `FUN_80041358` (get, :151486-151575): id 0..0x13
  → Actor-Offset. Übersetzung GELÖSCHT; `re15_actor_get/set_member` (actor_common.c) nehmen jetzt die rohe RE1.5-id.
- **Tabelle:** 0→x 1→y 2→z 3→rot_x 4→rot_y 5→rot_z 6→flags 7→+0x0c 8→**state** 9→sub_state_1 10→sub_state_2
  11→sub_state_3 12→**+0x09 grid_id** 13→sub_state_4 14→+0x0a 15→+0x0b 16→anim_flags 17→status_flags 18→floor 19→hp.
- **Bugs behoben:** id12 (häufigster Member_set!) schrieb via RE2-0x0C → **Leon.y = Spieler unter Boden**; jetzt grid_id.
  id8 schrieb motion statt **state**; id6/9/10/11/13/16/17/18/19 falsch/gedroppt. 4 Orphan-Felder ergänzt → alle 20 ids
  round-trippen. **ids 0–5 (Koords/Rot) byte-identisch zu vorher → ROOM1170 unverändert (keine Regression).**
- **#12:** Operatoren `{0:== 1:> 2:>= 3:< 4:<= 5:!= 6:&}` + LE-Wert waren schon byte-true (frühere Welle); der
  Restfehler war NUR die geteilte kaputte id-Quelle → mit #11 mitbehoben (cmp liest jetzt das richtige Feld).
- **GET-Vorzeichen** exakt über Port-Feldtypen (lbu zero-extend, lh sign-extend, lhu, lw). **Verifikation:** eigener
  Disasm-Read aller 20 Case-Bodies + Sprungtabellen. Test `tests/unit/test_member.c` (Direkt-Tabelle + VM-cmp, Feld-
  Quelle/LE/Operatoren). **28/28 ctest grün; Headless-Boot ~9s ohne Crash.**

## Tote Themen (NICHT wieder aufmachen)
- **„Pixel-Verschiebung"**: Der Nutzer hat bestätigt — es gibt KEINE sichtbare. Render-Math
  **#1–3** (RotMatrix/Trig-LUT/Kamera-Integer) sind reine byte-true-Korrektheit OHNE sichtbaren
  Bug → **keine Priorität**. War aus CLAUDE.md entfernt; nicht erneut als Pixel-Shift-Fix framen.

## Arbeitsweise für die nächste Session

1. **Build-Recipe:** `export PATH="/c/msys64/mingw64/bin:$PATH"` →
   `cmake --build re15_port/build` → `ctest --test-dir re15_port/build --timeout 30`.
   (Test-Flags beim Configure: `-DRE15_BUILD_TESTS=ON -DRE15_BUILD_TOOLS=ON`.)
2. **Verifikations-Workflow (bewährt):** pro Fix einen `general-purpose`-Agenten ansetzen, der
   die Audit-Behauptung gegen `ghidra1_V2.txt` + `RE_15_Quellcode_V2/` belegt und einen
   apply-ready Edit (alter→neuer Schnipsel) + VERDICT (CONFIRMED/WRONG/UNSURE) liefert.
   Unabhängige Fixes parallel. **Audit-Werte NIE blind übernehmen.**
3. **Nach jedem Fix:** build + ctest grün halten; bei riskanten Verhaltensänderungen zusätzlich
   per Savestate-Pipeline (Skills) gegen das echte Spiel verifizieren.
4. **Memory pflegen:** angewandte Fixes in `reai-v2-byte-true-audit` nachtragen.

## Nächste Schritte „der Reihe nach"

### #13 Player-Damage (FUN_80012d60, komplett fehlend) — JETZT, CRITICAL
- **Audit #13:** Der komplette Player-Damage-Pfad fehlt im Port (FUN_80012d60 = Damage-Entry,
  Damage-Tabelle, Death/Hit-State, I-Frames). HP-Global `DAT_800acaee` jetzt per Savestate lesbar
  (Skill `re15-savestate-ghidra`) → ideal für Laufzeit-Verifikation. Braucht Enemy-Attack-Hitbox als
  Caller (wer ruft FUN_80012d60?). Vor RE: `RE15_FUN_CATALOG.md` + Skill `ghidra-mapping` prüfen.
- **Muster:** Agent gegen FUN_80012d60 (+ Caller-Xrefs, Damage-Tabelle, I-Frame-Timer) verifizieren →
  anwenden → build+ctest → zusätzlich per Savestate-Pipeline gegen echtes Spiel (HP-Wert) prüfen.

### Danach (Audit-Reihenfolge, je per Agent verifizieren)
- **#14/#15** AOT-Scan-Refactor (Edge→per-Frame + Multi-Entity + 9-Frame-Tür) — riskant, gegen
  Savestate verifizieren.
- **#17/#18** Goto/Gosub-Unwind + Do-Exit-PC (jetzt mit dem unified Loop-Modell tractabler).
- **#21** Letterbox-Parität, **#22** Player-State-FSM (verzahnt mit #13), **#25** Aot_on,
  **#26** Work_set kind=5 (offene EXE-Auflösung DAT_800b23f4), **#27** Message_on, **#29**
  Collision-Slopes (5 Handler), **#30** Stair-Finalize, **#31** Camera-Orbit, **#33** EMD-MD1.

### Deferred / nicht byte-true (dokumentiert in BYTE_TRUE_AUDIT.md + Memory)
- #32c (FLR-Terminator, OOB-Risiko), #36b/c/d (Glyph-Cap/FF/Lookahead), Render-Math #1–3 (keine
  Priorität), For-count==0-Skip in #8 (bewusst defensiv, als nicht-byte-true markiert).
- NOCH OFFEN aus #8: Break-Flag-Byte (thread+0x4-Konsument unbekannt), Loop-N pro call-depth statt flach.

## Schlüssel-Referenzen
- `BYTE_TRUE_AUDIT.md` — 44 priorisierte Fixes mit zitierten Adressen.
- `RE_15_Quellcode_Overlays/` (Raum-/Gegner-Logik, PRIMÄR) · `RE_15_Quellcode_V2/` + `ghidra1_V2.txt`
  (EXE-Engine, Fallback) · `RE15_KNOWLEDGE.md` (Formate) · `RE15_FUN_CATALOG.md` (verifizierte FUNs).
- Memory: `reai-v2-byte-true-audit`, `reai-v2-duckstation-dynamic-re`, `reai-v2-ghidra-pipeline`,
  `reai-v2-build-recipe`.
