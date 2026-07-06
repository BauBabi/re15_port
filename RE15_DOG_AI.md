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
5. **Offen (grid-gated Spezial-Dog, wie der Zombie-Grab):** der Pounce-Land (state 5, 0x80111350 sub 0:
   Sprung-Bogen +0x38−=20, Land → state 0x201) für grid-0x43-Dogs + die Player-Kill-„gefressen"-Cutscenes
   (Maschine A @0x80111984 / B @0x80111cf0, player-cmd-FSM aca58/59/5a — parallel zum Zombie-Grab). Plus
   Obstacle-Reroute (sub 13/14, braucht +0x90-Wall-Contact vom vollen SCA-Resolver).
6. **Dynamik-Verify:** Savestate aus einem Hunde-Raum + `re15_enemy_state.py`, neue Dog-Label-Map @0x80120f74.

**Der NORMALE Combat-Dog ist funktional komplett** (Spawn → Chase → Biss −10 HP → Pounce-Leap → killbar);
offen ist nur das grid-0x40/43-Spezial-Verhalten (Pounce-Pin + „gefressen"-Kill).
5. **Dynamik-Verify:** Savestate aus einem Hunde-Raum + `re15_enemy_state.py` mit NEUER Dog-Label-Map
   (@0x80120f74; die Zombie-Map @0x8011f7b4 passt NICHT).
