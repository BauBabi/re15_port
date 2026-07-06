# RE1.5 NPCs (Irons/Sherry/Annette/Katherine) — byte-true RE

Die STAGE1-NPCs sind **invulnerable Cutscene/Dialog-Actors** (HP = −1): **0x40/0x42 = Chief Irons (EM040)**,
**0x45 = Sherry (EM045)**, **0x47/0x49 = Annette Birkin (EM047/EM049)**, **0x4b = Katherine (EM04B)**
(BioModels-bestätigt). Sie idle-posen, laufen (Nav-Steer), schauen den Spieler an, und (nur Katherine)
triggern Dialog. RE via Workflow **wf_69c86050** (4 CLUSTER + 4 Refuter, adversarial verifiziert).

**Schlüssel-Fund: alle NPCs teilen eine gemeinsame EXE-Verhaltensbibliothek** (states [4] + [6-11] @
0x80050be8 / 0x8004f100–fd3c) — byte-belegt bei Irons/0x40, Ben/0x42, Katherine/0x4b (identische Tabellen-
Einträge). RE'e ich die Lib einmal → gilt für alle 6 NPCs. Nur die overlay-States [0-3,5] sind per-NPC.

## Dispatch (byte-verifiziert)

- **EXE-Dispatch @0x80072bac[0x40] = 0x8011c5a0** (STAGE1-Reg @0x8011e914). Root 0x8011c5a0: Pause-Gates
  (0x800aca40&0x20000000, +0x9&0x20) → **Look-Helfer 0x8001bd60(−10, 0x14)** (Head-Turn) → **Nav-Steer
  0x80039e7c** (Pathfind zu +0x1dc/+0x1de) → State-Dispatch (+0x4) via **@0x80121598** (12 Einträge) →
  Body-Push (0x8002aec4/b544) + **SCA-Wall-Clamp** (radius 450) + Box-Rebuild.
- State-Tabelle @0x80121598: **[0] INIT 0x8011c6dc / [1] 0x8011c884 / [2] 0x8011ca48 / [3] 0x8011ca90 /
  [5] 0x8011c950** (overlay, Irons-spezifisch) — **[4] 0x80050be8 / [6-11] 0x8004f100/f3a4/f5e8/f7dc/fb3c/
  fd3c** (EXE-SHARED, alle NPCs).

## STATE [0] INIT 0x8011c6dc (byte-true CONFIRMED)

+0x4=1 (default → overlay state 1) ODER +0x4=4 & +0x5=6 (wenn +0x9&0x40 → shared executor). **+0x9a = −1
(KEIN HP / invulnerable)**, +0x9e=120, +0x1b9=8, **+0x94=2 (idle-Clip)**, anim_set, Shadow-Blob (600×700),
+0x1a8 = Dialog-Block-Ptr (0x800aca54), Region-Box {64,48,712,0,0,312}. Kein Se/Dialog-Trigger.

## SHARED EXE-Lib (byte-true CONFIRMED — der NPC-Verhaltens-VM)

- **STATE [4] 0x80050be8 = EXECUTOR**: nested Sub-Dispatcher auf +0x5 via **@0x80076ca0** (10 Sub-Verhalten)
  + Root-Motion-Apply (0x800369f8 → +0x34/+0x3c). Half-Rate-Gate (+0x1c4 & 0x10/0x20).
  - Sub-Lib @0x80076ca0: **sub 0-3 = idle-pose** (2-Layer anim_set, kein move); **sub 4/5/7/8 = walk-to-
    target** (Clip aus per-Typ-Tabelle @0x80076c00, Ziel = +0x1bc/+0x1be steer, yaw-face 0x8001aac4 +
    pos_advance, Arrival 0x8002d7d8 → +0x5=18/22); **sub 9 = turn/look-at in place** (kein pos_advance).
- **STATE [6] 0x8004f100 = WATCHER (player-relativ)**: 4 sequentielle if (last-match-wins) setzen +0x5:
  Timer +0x1d0≥1501 → +0x5=1; Cone-out (arc 0x8001ab9c, ±0x4b0) → +0x5=2; **Katherine-Dialog** (g_flag52
  bit0 & type==0x4b) → +0x5=6; player.hit_react≠0 → +0x5=6.
- **STATE [7] 0x8004f3a4 = WATCHER (nav-relativ)**: Timer/Cone(±0x400 zu +0x1dc/+0x1de)/Katherine-Dialog/
  hit_react → +0x5=3/5/6.
- **Helpers:** 0x800369f8 Root-Motion (Walk-Displacement → +0x34/+0x3c), 0x8001ab9c Cone-Test (0 = aligned),
  0x8001aac4 yaw-face, 0x8002d7d8 Arrival-Predicate. **Dialog nur Katherine** (g_flag52 + type 0x4b).

## EM040 (Irons) Clip-Frame-Counts (byte-true, dir[1], 24 Clips)

`{34,32,50,26,20,20,50,1,1,1,1,25,1,1,1,1,1,10,25,1,1,1,30,30}` — idle 0x2=50. (1-Frame-Clips = Platzhalter.)

## Wellen-Status

1. ✅ **ENUMERATE + CLUSTER RE** (wf_69c86050, adversarial): Irons-Root/INIT + die shared EXE-Lib (executor
   + watchers + sub-library) byte-true, jede Konstante disasm-zitiert. Die shared Lib gilt für alle 6 NPCs.
2. ✅ **Wave 1 PORTIERT** (test_npc_ai, 41/41): re15_npc_ai_tick (run_all `else if t==0x40`) — Irons INIT
   (**HP=−1 invulnerable**, idle-Clip 2, → state 1) + idle-pose-Hold (steht + animiert). EM040-Clips embedded.
   Invulnerable via kein Hitbox-Case (nicht zielbar). Grundlage für alle NPCs gelegt.
3. **Wave 2 (deferred, NPC-aktiv-Savestate nötig):** der volle Verhaltens-VM (walk-to-target sub 4/5/7/8 +
   look-at sub 9 + die watcher-States [6]/[7] + der Root-Motion-Apply), die per-NPC overlay-States
   (Irons [1-3,5], Sherry/Annette/Katherine INITs), die anderen 5 NPC-Typen (0x42/45/47/49/4b), der
   Katherine-Dialog-Trigger. Nav-Steer (0x80039e7c) + Look (0x8001bd60) sind EXE-seitig schon portiert.
4. **Dynamik-Verify:** Savestate aus einem NPC-aktiven Raum + re15_enemy_state.py mit NPC-Label-Map
   @0x80121598; walk/look/dialogue live beobachten.
