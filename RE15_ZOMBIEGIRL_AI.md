# RE1.5 Zombie Girl (type 0x13, EM013 = ZOMBIE_GIRL) — byte-true RE

Die Zombie-Girl = **Enemy-Typ 0x13** (Modell **EM013**, BioModels `EM013.EMD = ZOMBIE_GIRL`, EMS-idx 3, 42
Clips). Eine **nav-pfadende weibliche Zombie-Variante**, die die **STANDARD-Zombie-Combat-Maschinerie
wiederverwendet**. RE via Workflow **wf_21e29175** (4 CLUSTER + 4 Refuter, adversarial verifiziert).

## Dispatch (byte-verifiziert)

- **EXE-Dispatch @0x80072bac[0x13] = 0x8010a8c8** (STAGE1-Reg @0x8011e874). **KORREKTUR:** @0x80120208 war
  in einem alten Memo als „dormanter Zombie-Lunge-Dispatch" fehlnotiert — es ist die Zombie-Girl-State-Tab.
- Root 0x8010a8c8: Pause-Gates → Dist→+0x1d0 → +0x1d5-Reservation-Countdown (releast Flock-Bit @0x800aca50)
  → Look 0x8001bd60 → **NAV-STEER 0x80039e7c zur LIVE-Spieler-Pos** → State-Dispatch (+0x4) via **@0x80120208**
  → attack_point 0x80104178 → Body-Push + SCA-Wall-Clamp + Box-Rebuild.
- State-Tab @0x80120208: [0] INIT 0x8010ab2c, [1] ACTIVE-Brain 0x8010b274, [2] hurt 0x8010bf80, [3] attack
  0x8010c014, [4] 0x8010919c, **[7] 0x80109554 = die SHARED Zombie-Corpse-Settle**, [10]/[11] attack-subs.

## STATE [0] INIT 0x8010ab2c (byte-true CONFIRMED)

→ state 1. **HP = (rng&0x1f)+50 = 50..81** (+0x9a @0x8010ac1c). +0x9c=20, Steer-Target=Spieler (+0x1bc/+0x1be),
SCA-Descriptor +0x78=0x8011f778, **Variant +0x1d4 = {2,3,4,5}[rng&7]** (@0x8011f7e4), Hit-Box-Seed +0x188.

## STATE [1] ACTIVE-BRAIN 0x8010b274 (byte-true CONFIRMED)

**Der Kern-Fund: sie nutzt die STANDARD-Zombie-Phase-Handler.** Lunge-Arm-Timer (+0x1da, gate +0x1d8&0x100)
→ @300 8-Part-Lunge-Pose → @0 **commit to attack STATE 3** (+0x5=0x15, clip 0xb/0x1f). Perception (LOS
0x8001bc08, broadcast 0x80012aa4 radius 3000). **MODE-Dispatch auf +0x9&0xf via @0x80120230** (16 Modes) →
je Mode zwei +0x5-Dispatches (decide @0x80120264 / animate @0x801202a8, 11 Phasen) = **dieselben Handler wie
der Standard-Zombie**: engage 0x80102058, **GRAB 0x80102548** (cmd5 DAT_800aca58 + **player.hp −= 10**
@0x80102774), corpse 0x80109554. **Nav-Walk** (0x8010be50): clip 0x1a=26, steer 0x8001aac4 rate 0x10,
root-motion 0x8010c088, −1800 forward-hop. **Kein direkter player.hp/aca5x-Write in state[1]** — aller Schaden
fließt durch die geteilten 0x80102548/0x80102058.

**Engage 0x80102058 Thresholds:** Wall-Contact-Turn (+0x90&3); dist<2000 & cone → +0x4=0x0701; hit_react==0
& dist<1200 & aligned → grab (+0x5=facing+3); dist<1500 & **player.hp<0** → +0x4=0x0c01.

## Wellen-Status

1. ✅ **ENUMERATE + CLUSTER RE** (wf_21e29175, adversarial): Root/INIT + Active-Brain (Mode/Phase-Dispatch,
   engage, grab) + Attack/Hurt/Death byte-true, jede Konstante disasm-zitiert. **Kern: reuse der Zombie-Combat.**
2. ✅ **Wave 1 PORTIERT** (test_zgirl_ai, 42/42): re15_zgirl_ai_tick (run_all `else if t==0x13`, Boden→wall-
   clamp). INIT (HP 50-81, nav-walk clip 26, →state 1) + ACTIVE nav-chase (clip 26, steer + advance zum
   Spieler) → **GRAB** (state 3, pinnt den Spieler via die geteilte Victim-FSM, **−10**) + killbar (take_damage
   hp<0 → state 7 CORPSE; state 2 non-lethal → flinch). EM013-Clips embedded (42), Hitbox 0x13=400/1440.
3. **Wave 2 (deferred, Savestate nötig):** der exakte Mode-Dispatch (+0x9&0xf @0x80120230, 16 Modes), die
   Lunge-Arm-Timer-Choreografie (+0x1da, 8-Part-Pose), die wiederholten Grab-Bites (−5), die per-Mode/Phase-
   Handler-Feinheiten. Da sie die Zombie-Phase-Handler teilt, ist viel davon schon in der Zombie-Foundation.
4. **Dynamik-Verify:** Savestate aus einem Zombie-Girl-Raum + re15_enemy_state.py mit der @0x80120208-Map.
