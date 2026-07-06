# RE1.5 Hund (DOG / Cerberus, Typ 0x20) — byte-true RE (ENUMERATE)

Der Hund = **Enemy-Typ 0x20** (Modell **EM020.EMD**, BioModels-bestätigt `{ "EM020.EMD", DOG }`). Ein
klassischer boden-basierter Combat-Gegner (Patrol → Chase → Sprung-Lunge → Biss). Diese Datei = die
ENUMERATE-Grundlage (Skill `re15-enemy-ai-re`); der Port ist ein Mehr-Wellen-Aufwand wie Zombie/Crow.

## Dispatch (byte-verifiziert, STAGE1.BIN)

- **EXE-Dispatch @0x80072bac[0x20] = 0x8010d7f8** (STAGE1-Overlay-Registrierung @0x8011e8d4:
  `lui/addiu v0=0x8010d7f8; sw v0, 0x80072c2c`). **Root-Handler = 0x8010d7f8**.
- Der Root dispatcht den STATE (+0x4) via Tabelle **@0x80120f74**, DANN läuft der geteilte
  Body/Kollisions-Tail (0x8002b498 contact-clear, 0x8002aec4/0x8002b544 body-push, 0x8003b0a4
  SCA-Wall-Clamp) — **dieselbe geteilte Infra wie Zombie/Crow** (~70%-Shared-These bestätigt;
  boden-basiert, teilt Steering/Kollision mit dem Zombie).
- **State-Tabelle @0x80120f74 (12 Einträge, byte-verifiziert):**

| +0x4 | Handler | Rolle (klassifiziert — via Workflow) |
|---|---|---|
| 0 | 0x8010d93c | INIT |
| 1 | 0x8010dbcc | ACTIVE (Haupt-Brain: idle/patrol/chase/lunge-decide) |
| 2 | 0x801108f0 | (Angriff? Lunge/Biss) |
| 3 | 0x80110dc0 | (Angriff?) |
| 4,5,6 | 0x80111350 | (shared — HURT/Stagger?) |
| 7 | 0x80111774 | (?) |
| 8 | 0x8010ddb8 | (?) |
| 9 | 0x8010df94 | (?) |
| 10 | 0x8010e0c4 | (DEATH?) |
| 11 | 0x8010e568 | (CORPSE?) |

## Modell (verifiziert)

- **EM020.EMD** liegt in **CDEMD0.EMS @Index 7** (Offset 0x152800, 166912 B), valides EMD, **28 Clips**.
- **Clip-Frame-Counts (verbatim, für's byte-true Einbetten wie beim Crow):**
  `{49,18,30,14,14,14,18,15,30,35,35,14,42,14,44,14,27,14,50,29,17,13,20,29,18,40,30,99}`.
- Lädt generisch via `pc_enemy_load(0x20)` (Typ 0x20 in `s_ems_order` Index 7).

## Port-Infra (Wave-1-bereit, analog Crow)

- Spawn via `op_sce_em_set` (Typ 0x20 aus der RDT-Enemy-Liste); Render generisch (NPC-Loop);
  AI-Tick-Einfügepunkt: `run_all`-Gate [enemy_ai_common.c:2457] (`else if (t==0x20)`, eigener Branch;
  der Hund ist boden-basiert → nutzt anders als der Crow die geteilte Wall-Clamp/Body-Push-Reihenfolge).

## Architektur (via CLUSTER-Workflow wf_ccc60f69, adversarial verifiziert)

**Wichtige Korrektur:** Die State-Tabelle @0x80120f74 (per +0x4) und die ACTIVE-Sub-Tabellen aliasen
(B-decision @0x80120f94 = state-tab+0x20). Der Root dispatcht +0x4; **state 1 (ACTIVE) ist der Brain**
= Dual-Dispatch auf +0x5 (decision B @0x80120f94 + act C @0x80120fd4). Und: **states [2]/[3]/[7] sind
die EMPFANGENDE Seite** (HURT/DEATH/CORPSE, spiegeln den Zombie exakt — der Dog ist KILLBAR via die
geteilte take_damage). Der offensive Teil (chase/lunge/bite) ist im ACTIVE-Brain.

- **ACTIVE-Sub-States (+0x5):** 0 IDLE (dec 0x8010ddb8: dist<4000 & LOS / alarm → sub 1), 1 TURN (clip 2
  → sub 2), 2 CHASE (dec 0x8010e0c4: arc 2500/192 → sub 3; **POUNCE-Gate** cd==0 & player.hp≥81 & dist≥7001
  & LOS & ¼-Chance → sub 4 LUNGE; act: walk clip 0, yaw-slew, footstep Se(6), speed 8), 3 ATTACK-RANGE
  (dec 0x8010e568: arc 3000/384 → sub 8 BITE; act: growl clip 3), 4 LUNGE (clip 0xb), 8 **BITE** (clip 0x14),
  13/14 OBSTACLE-REROUTE.
- **states 4/5/6 = 0x80111350** (grid-0x40-Dogs): LUNGE/pounce-land + **Player-Kill-„gefressen"-Cutscenes**
  (Maschine A @0x80111984 / B @0x80111cf0, player-cmd-FSM 0x800aca58/59/5a — wie der Zombie-Grab).
- **HURT [2]:** flinch clip 6 → hp≥0 recover state 1 / hp<0 state 3. **DEATH [3]:** clip 0x0e (grounded) /
  clip 7 (airborne + Forward-Slide) + Se(7) yelp, 150f-Timer → state 7. **CORPSE [7]:** 90f Color-Fade → inert.

## Wellen-Status

1. ✅ **CLUSTER RE** (wf_ccc60f69): alle 12 States byte-true + adversarial verifiziert.
2. ✅ **Wave 1 PORTIERT** (Commit f8874c99, test_dog_ai, 38/38): INIT + ACTIVE (idle/turn/chase/attack-range)
   + HURT/DEATH/CORPSE (killbar). Hund spawnt, jagt den Spieler, ist tötbar. EM020-Clips embedded.
3. ✅ **Wave 2 PORTIERT (Commit 9f7857c6, test_dog_ai, 38/38): der Biss-Angriff.**
   - **BITE (sub 8, 0x8010f15c, clip 0x14):** im Connect-Fenster (anim_frame≥13, @0x8010f254) Vorwärts-
     Lunge (+0x8c+=6) und — wenn Spieler nicht reagiert (+0x93==0) & im Cone 2000/384 (arc @0x8010f2a8) —
     **`player.hp -= 10`** (@0x8010f2d0) + Se(5). → zurück zu CHASE. **Der Biss war „unresolved" im Cluster;
     per Direkt-Disasm der inner-jt @0x801001ec gefunden = −10 HP (kein Grab; die Grab-Kill ist state 4/5/6).**
   - **POUNCE-Gate (CHASE @0x8010e194):** cd==0 & player.hp≥81 & dist≥7001 & LOS & ¼ → sub 4 (byte-true).
   - **LUNGE (sub 4, 0x8010ea44, clip 0xb):** funktionaler Sprung → Land → attack-range. *Faithful-line:
     die exakte inner-jt-Sprung-Physik (@0x801001ac) = Wave 3.*
4. ✅ **Wave 3 (teil, Commit e93c9013): Lunge-Windup byte-true.** inner-jt @0x801001ac RE'd: step0 clip 0xb
   + timer 0x3c + Aim, step2 Bark Se(2) @0x8010eb40, step4 schreibt **+0x4=5 → state 5** (0x80111350). **KERN:
   state 5s Pounce-Land ist grid-gated (+0x9==0x43 @0x801113e8)** — der Pounce-Pin + die grid-0x40-Kill-
   Cutscenes sind **Spezial-Grid-Verhalten** (0x40/0x42/0x43), NICHT der normale Combat-Dog. Windup portiert
   (clip 0xb, Bark); der normale Dog-Sprung löst zu attack-range auf.
5. ✅ **Wave 3 KOMPLETT (Commit 19d4a12e): POUNCE-LAND (state 5) + Kill-Cutscene-Entry.**
   - Lunge (sub 4) → byte-true auf Dog-HP (@0x8010eb88): hp≥0 → state 5 (Pounce-Land), hp<0 → state 3 (Death).
   - **re15_dog_state456** POUNCE-LAND (sub 0, 0x80111398): clip 0x14, Sprung-Bogen (+0x38−=20 Rise @0x801114dc),
     Land clip 0x15, → +0x4=0x201 state 1/sub 2 CHASE. **Grid-gated (+0x9==0x43/0x42)** — normaler Dog routet
     SICHER zu chase (kein Freeze). Test(5): grid-0x43 Sprung Peak y=−80 → Land → CHASE.
   - **Kill-Cutscene-Entry** (sub 4/5 Maschine A / 10/11 B): Dog pinnt den Spieler via s_player_grabbed
     (game_step pinnt, parallel zum Zombie/Crow-Grab). *Faithful-line: die exakte aca5a-player-cmd-Sub-FSM-
     Kadenz (die Drag/Eat-Frames) ist deferred.*
6. **Dynamik-Verify:** Savestate aus einem Hunde-Raum + `re15_enemy_state.py`, neue Dog-Label-Map @0x80120f74.

**Der Hund (Typ 0x20) ist byte-true portiert — die KOMPLETTE State-Machine** (0 INIT / 1 ACTIVE [idle/turn/
chase/attack-range/lunge/bite] / 2 HURT / 3 DEATH / 4-5-6 POUNCE-LAND+Kill / 7 CORPSE): spawnt, jagt, beißt
(−10 HP), springt (Pounce-Leap+Land), pinnt den Spieler (grid-0x40-Kill), und ist tötbar.

## ✅ FINALE Welle (Commit dog-100%): die zwei deferrten Punkte byte-true portiert — ZERO faithful-line

RE via Workflow wf_7cc285c0 (5 RE-Agenten + 5 Refuter, adversarial) + 2 Solo-Agenten, alle disasm-zitiert:

1. **Eaten-GRAB (der Kill, war „exakte Drag/Eat-Kadenz deferred")** — der Tod liegt in **state 1, sub 9/10**
   (NICHT 4/5/6; Agenten-Korrektur). **Eintritt @0x8010f458:** ein Biss (sub 8, −10 HP) eskaliert zum
   Fressen `+0x5 = facing_aligned + 9` **gdw. (A) `+0x1e4≠0` (armed) ODER (B) hp−10 < 0 (lethal)** — lethaler
   Biss frisst immer, non-lethal nur wenn der Dog bei hp<50 vor-armed (sub 5 windup @0x8010ed54). **Grab-Hold
   7-Step @0x8010020c/@0x8010022c** (`re15_dog_grabhold`): Latch clip 0x17/0x1a → Eat-Loop clip 0x18 (Struggle-
   Drain +0x9c=100 −(1+100·mash), Feed-Timer +0x9e=50) → Release clip 0x19; Exits **sub 0xb** (gefressen,
   aca58=6 → devour → Player state 7) / **sub 0xc** (befreit → clip 8 → chase). Spieler-Seite = die geteilte
   Victim-FSM (`re15_player_victim_latch/devour`, aca58=5, Leon aus EM020 bank2 — wie der Zombie-Grab).
   **Pounce-Kill (state 4/5/6, machine A @0x80111984 9-Phasen / machine B @0x80111cf0 5-Phasen):** beide
   animieren den SPIELER via s_victim_phase + Blut/Gore-fx (0x1500/0x2000) + Kill (aca58=7 @0x80111ea0);
   Port = Victim-FSM + `re15_esp_fx_spawn` + devour. (Cam-Pan acb10/acb12 + Cut-Freeze aca3c|=0xc0 = port-
   weite Cutscene-Kamera-Lücke, geteilt mit dem Zombie-Grab-Tod.)
2. **Obstacle-Reroute (sub 13 leap-over @0x801102dc / sub 14 turn-around @0x801105bc)** —
   `re15_dog_reroute13/14`. **Eintritt aus der CHASE-Decision:** `+0x5 = 0xe − (+0x90 & 1)` (@0x8010e2b4/d4) →
   sub 13 bei Wall-Kontakt, sub 14 sonst. Beide lesen +0x90 (ai_contact) — Escape-Heading = `(+0x90&0xf0)<<4`
   (16 Richtungen), drehen dahin, laufen clip 9/0x0a drüber, zurück zu CHASE (+0x5=2). Port: +0x90 wird
   byte-true-äquivalent aus dem Kollisions-Clamp-Displacement (dieselbe Wall-Normale, die der SCA-Resolver
   0x8003b0a4 @0x8003b1dc kodiert) in `re15_enemy_ai_run_all` gesetzt.

Test `test_dog_ai` Teile (6) EATEN-GRAB (Biss→Grab→pinned→gefressen state 7), (7) ARM-GRAB (hp<50→sub 5/7),
(8) REROUTE (sub 13→dreht→läuft→CHASE). **38/38 ctest grün.** Einzige verbleibende faithful-line = die
port-weite Cutscene-Kamera/Fade-Infra (nicht Dog-spezifisch, geteilt mit dem Zombie).
5. **Dynamik-Verify:** Savestate aus einem Hunde-Raum + `re15_enemy_state.py` mit NEUER Dog-Label-Map
   (@0x80120f74; die Zombie-Map @0x8011f7b4 passt NICHT).
