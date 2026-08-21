/*
 * RE1.5 Rebuilt — Actors (Phase 4.4.8, 2026-05-19).
 *
 * Per-actor state for NPCs / enemies (and eventually the player when
 * we unify game_state into the actor pool). RE2 has a 0xAC-byte actor
 * struct with 44 properties indexed by ID via PTR_LAB_800a4030 — see
 * RE2_Quellcode/FUN_80055cb0.c (set), FUN_80055f50.c (get).
 *
 * We port a subset enough for SCD `Member_set` / `Member_cmp` /
 * `Sce_em_set` to drive the spawn-and-position behavior used by most
 * RE1.5 room scripts. The remaining ~36 properties can be added
 * incrementally as scripts reference them.
 *
 * Member-id semantics match RE2 (see actor_common.c property table).
 */

#ifndef RE15_ACTOR_H
#define RE15_ACTOR_H

#include <stdint.h>

#define RE15_ACTOR_MAX        16
#define RE15_ACTOR_SLOT_PLAYER 0   /* Slot 0 is always the player (RE2-pure) */

/* T-round (2026-05-25): RE2 actor flag bits at +0x1c4 — control anim FSM. */
#define RE15_ANIM_FLAG_LOOP          0x04   /* loop on clip end (else freeze) */
#define RE15_ANIM_FLAG_SECOND_PASS   0x08   /* enable second pass */
#define RE15_ANIM_FLAG_CLEAR_INTERP  0x40   /* clear interp counter */
#define RE15_ANIM_FLAG_REVERSE       0x80   /* reverse playback */

typedef struct {
    uint8_t  active;       /* 0 = slot free, 1 = active */
    uint8_t  type;         /* enemy / NPC type (slot 0 = player, type=0) */
    /* RE2 base fields (per FUN_80055cb0.c Member_set dispatch) ===========
     * Member ID → field mapping verified F1 agent 2026-05-21. RE1.5 SCD
     * scripts use the same Member IDs as RE2 because both engines share
     * lineage.                                                           */
    /* RE1.5 SCD Member fields — byte-true id→offset from the engine's own member
     * cores FUN_8004116c (set) / FUN_80041358 (get); full table in actor_common.c.
     * (The earlier "RE2 id" labels were the RE2-translation that wrote Member id12
     * → Leon.y = "player under the floor"; corrected to the direct RE1.5 ids [#11].) */
    uint8_t  state;        /* RE1.5 Member id 8  → +0x04 */
    uint8_t  flags;        /* RE1.5 Member id 6  → +0x00 (subset: low byte of the word) */
    uint8_t  sub_state_1;  /* RE1.5 Member id 9  → +0x05 */
    uint8_t  sub_state_2;  /* RE1.5 Member id 10 → +0x06 */
    uint8_t  sub_state_3;  /* RE1.5 Member id 11 → +0x07 */
    uint8_t  sub_state_4;  /* RE1.5 Member id 13 → +0x08 */
    int32_t  member_0c;    /* RE1.5 Member id 7  → +0x0c (word) — stored, no consumer yet */
    uint8_t  grid_id;      /* RE1.5 Member id 12 → +0x09 — grid/cell id, the MOST frequent Member_set (117 sites) */
    uint8_t  member_0a;    /* RE1.5 Member id 14 → +0x0a — pro Frame auf 0xFF gewischt
                            * (sh 0xFFFF @0x8003ec4c Spieler / @0x8003ec68 aktive Gegner,
                            * zusammen mit member_0b im Stempel-Pass); Writer existiert
                            * (Gorilla-INIT +0x1b9=0x19, enemy_ai_common.c) */
    uint8_t  member_0b;    /* RE1.5 Member id 15 -> entity+0x0b: der 0-basierte Slot-Index der
                            * zuletzt getroffenen AOT-Zone (`addiu v0,s2,255` im Delay-Slot
                            * @0x80042f44 -> `sb v0,11(s1)` @0x80042f5c ACTION / @0x80042fc4 AUTO,
                            * LAST-WINS auf dem AUTO-Pfad), Leerwert 0xFF aus dem Frame-Wisch
                            * FUN_8003ec28 (`ori v0,zero,0xffff` @0x8003ec44 + `sh v0,0(v1)`
                            * @0x8003ec4c auf 0x800aca5e = Spieler+0x0A/0x0B).
                            * ⚠ Historie: hier lag frueher der LOS-Latch des Adult Spider (der
                            * einzige Typ ohne eigene Feldgruppe). Der ist nach `aspider_los`
                            * umgezogen — sonst haette der AOT-Stempel seine Sichtlinie zerstoert.
                            * Der Stempel (Glied 1) IST implementiert: re15_aot_stamp_entities
                            * (aot_common.c), seit 81bf395c; laeuft als letzter Zustands-Tick
                            * des Frames. Details: analysis/room1030_crawl_mechanism.md */
    uint8_t  floor;        /* RE1.5 Member id 18 → +0x82 (floor band) */
    int32_t  x;            /* RE1.5 Member id 0 → +0x34 (world X)          */
    int32_t  y;            /* RE1.5 Member id 1 → +0x38 (world Y)          */
    int32_t  z;            /* RE1.5 Member id 2 → +0x3c (world Z)          */
    int16_t  rot_x;        /* RE1.5 Member id 3 → +0x68 (rot_x)           */
    int16_t  rot_y;        /* RE1.5 Member id 4 → +0x6a (rot_y, 4096=360°)*/
    int16_t  rot_z;        /* RE1.5 Member id 5 → +0x6c (rot_z)           */
    int16_t  hp;            /* RE1.5 Member id 19 → +0x1ba (hp; lh signed)  */
    /* Combat hit-reaction state — byte-true player branch of the damage
     * resolver FUN_80012d60 (ghidra1_V2.txt:77656-77714). hit_react (RE2 +0x93):
     * bit0x1 = "already hit this attack" guard — SET @80012eec, CHECKED @80012e30
     * → exactly one damage application per attack window (cleared between attacks
     * by the deferred enemy-attack FSM, see re15_player_clear_hit_guard). bit0x2 /
     * bit0x80 are the enemy-side collision flags (unused on the player). status_flags
     * (RE2 +0x98, DAT_800acaec, u16): bit0x2 = bleed/poison, rolled on a type<2 hit
     * via 2×RNG&1 @80012ea4. See re15_damage.c. */
    uint8_t  hit_react;    /* RE2 +0x93 — per-attack hit guard + enemy collision bits */
    uint8_t  em_flag_id;   /* +0x1C6 — the Sce_em_set (0x44) pc[7] kill-flag index, latched at spawn.
                            * On death-commit the enemy sets the em-status flag[em_flag_id] so it does
                            * not respawn on room re-entry (the spawn gate GETs it). 0xFF = never persist. */
    uint16_t status_flags; /* RE2/RE1.5 +0x98 (Member id 17, lhu) — bit0x2 = bleed/poison */
    int16_t  water_y;      /* +0x88 — the sce=8 water-level stamp (LAB_8004330c re-stamps it every
                            * frame the entity is inside a water zone; wade/ripple FX consumer is
                            * presentation-side). 0 = no water. [wf_f536e1ee step 4] */
    /* Attack-hitbox dimensions — FUN_8002b5d0 reads these via target+0x78 (the
     * dim struct: radius_min@+6, height@+8, radius_max@+10) and target+0x7c (the
     * local centre offset). radius_min==radius_max → circular hitbox; else angular
     * sector (DEFERRED). hit_height = Y half-extent. SOURCE (RE'd 2026-06-29):
     * enemies = a per-type table (FUN_80019700 @80019894 sets +0x78 = type_table+0x8,
     * +0x7c @800198b0), overlay-patched → values read byte-true from room savestates;
     * player = a fixed EXE struct @0x80073e94 (radius 450/height 1530, VERIFIED in
     * PSX.EXE @0x64694). Wired by re15_player_apply_hitbox / re15_enemy_apply_hitbox
     * (re15_damage.c); 0 = no hitbox. All known instances are circular (radius_min==max). */
    uint16_t hit_radius_min, hit_radius_max, hit_height;
    int16_t  hit_offset_x, hit_offset_y, hit_offset_z;
    /* Forward attack-point (attacker +0x28/+0x2a/+0x2c) — the lunge's reach point at
     * which the attack trigger FUN_80017fa4 resolves a radius-500 hitbox. Precomputed
     * by the (deferred) lunge-movement AI; read by re15_enemy_attack. 0 until wired. */
    int16_t  atk_pt_x, atk_pt_y, atk_pt_z;
    /* Lunge attack-action active-frame counter (the work-struct +0xe; LAB_80017eb0 sets
     * it to 0x20 = 32). While > 0 the lunge action fires the hitbox each frame (re15_enemy_
     * lunge_tick -> re15_enemy_attack). 0 = not lunging. See re15_damage.c. */
    uint8_t  lunge_frames;
    /* ===== Enemy-AI state (byte-true, STAGE1 zombie handler FUN_8011d6d4) ==========
     * The AI is a nested FSM dispatched off three entity bytes the port maps onto the
     * existing actor fields (no new bytes for these — they ARE state/grid_id/sub_state_1):
     *   main state  entity+0x4  = actor.state       -> PTR_FUN_801217a0[state]
     *   sub index   entity+0x9  = actor.grid_id      -> PTR_FUN_801217b4[grid_id & 0xf]
     *               (for enemies grid_id ALSO packs flags: bit0x20 = skip-tick this frame,
     *                bit0x40 = stationary spawn — set by the room1140 briefing zombies)
     *   anim phase  entity+0x5  = actor.sub_state_1  -> the +0x5 logic/anim leaves
     * The transitions write a 32-bit word at +0x4 (e.g. 0x701 = state 1 / sub_state_1 7).
     * The fields below are the AI work-area the decision handlers read/write that DON'T
     * already exist above; offsets cite the RE1.5 entity struct (see re15_enemy_ai.c).   */
    uint32_t ai_dist;       /* +0x1d0: cached player distance SquareRoot0(ΔX²+ΔZ²) (tick) */
    int16_t  ai_timer;      /* +0x9c : search decision countdown (FUN_80101b64; 0 -> 0x101) */
    int16_t  grab_kill_ctr; /* +0x9e : the grab KILL counter (FUN_80102548 [2]=100; [3] -- per tick;
                             * was-0 OR player-hp<0 -> the DEVOUR handoff word ((+0x5)+2)<<8|1) */
    int32_t  anchor_x;      /* +0xa0 : the clip root-motion ANCHOR (FUN_8001ac38: pos - rotate(off));
                             * +0xa2   func_0x8001ad68 places pos = anchor + rotate(off[kf], yaw).
                             *         The grab [0] COPIES the zombie anchor onto the player -> the
                             *         pair interlocks in the AUTHORED formation (no clipping). */
    int32_t  anchor_z;
    uint16_t ai_flags;      /* +0x1d8: bit0x10 = "approach permitted" gate (decision block);
                             * bit0x1 = "actively grabbing" (set grab-[0] FUN_80102548.c Z.28,
                             * cleared throw-off-[4] Z.75; read by the domino-shove gate Z.87) */
    uint8_t  aim_band;      /* word0-Bits 29-31 (Kraehen-ACTIVE-Tail @0x80112560-c8): Elevation-
                             * Band fuer den Schuss-Resolver. 4 = UP (0x80000000 via 0x80012a0c
                             * (0x1770), vert>=4001 && dist<6000), 1 = DOWN (0x20000000 via
                             * 0x80012974(0x1770), vert<800), 2 = LEVEL (0x40000000), 0 = kein
                             * Band (unschiessbar). Nur der Kraehen-Tail stempelt es in STAGE1
                             * (crow_shot_attack.md F5). */
    uint8_t  grab_choreo;   /* word0 (+0x0) bit 0x1000 — grab-choreography latch: set in grab-[0]
                             * (FUN_80102548.c Z.21-22; the original also sets the PLAYER's word0 —
                             * no modeled consumer, not mirrored), cleared ONLY in grab-exit [8]
                             * (Z.115 &0xffffefff; a shot-aborted grab leaves it set = byte-true
                             * leak). Consumer: windup-timeout death gate FUN_80101224.c Z.12
                             * `(+0x1d8&0x100) && !(word0&0x1000)`. Dossier analysis/zombie_hit_1140.md D5/D6. */
    uint8_t  ai_contact;    /* +0x90 : WALL-contact byte (writer = the SCA resolver FUN_8003b0a4:
                             * heading-nibble<<4 | 8 | cell-attr&3; low nibble cleared per pass).
                             * Port writer deferred (ROOM1140 has 0 attr cells). */
    uint8_t  contact_flags; /* +0x1c2: BODY-push contact bits (FUN_8002aec4: |=1 player pushed me,
                             * |=2 an enemy did); cleared per tick (FUN_8002b498). */
    int8_t   contact_slot;  /* +0x1ac: the contacting ENEMY's actor slot (ptr in the original) —
                             * the grab [5] domino-shove target. */
    /* AI params FUN_8011d84c writes (PSX: into the model pool entity[0x62]+0x5fx). The
     * port has no model pool, so they live here as named actor fields (faithful-line:
     * map the data representation onto the port, don't rebuild the GTE/model-pool). The
     * attack cone 0x2c8 / wide cone 0x5f4 the decision handlers use are LITERALS in the
     * disasm, so these are init-faithful state rather than read by the decision path.   */
    int16_t  ai_arc;        /* +0x5fc = 0x2c8 (attack front arc) */
    int16_t  ai_p5f8;       /* +0x5f8 = 0x40                     */
    int16_t  ai_p5fa;       /* +0x5fa = 0x30                     */
    int16_t  ai_p5fe;       /* +0x5fe = 0x138                    */
    /* AI target point (+0x1dc/+0x1de) — the XZ the search leaf (FUN_8004f3a4, +0x5=1)
     * arc-tests against (a "where I'm heading / last seen the player" point). Written by
     * a movement leaf (deferred, model-pool); read here as an input. 0 = origin. */
    int16_t  ai_target_x;   /* +0x1dc */
    int16_t  ai_target_z;   /* +0x1de */
    /* STEER TARGET (+0x1bc/+0x1be) — the player XZ, REFRESHED EVERY TICK (RAM-arbitrated: in all
     * 16 live tl_run2 samples every zombie's +0x1bc/+0x1be == the player pos; the init store
     * @0x8010071c/734 is only the FIRST write; the per-tick writer evades an offset-literal scan —
     * exact address = open RE item). BOTH live walks steer toward it with the SIGNED gait slew
     * (FUN_801021f8:57/FUN_801057bc:40: func_0x8001aac4(+0x1bc,+0x1be,±sVar7) — the -1 rows steer
     * AWAY = the authored weave). */
    int16_t  steer_x;       /* +0x1bc */
    int16_t  steer_z;       /* +0x1be */
    uint8_t  repath_timer;  /* +0x91 : nav repath cycle (low7: reload |7 at 0, -1/tick; the DFS
                             * runs only on the 0-tick). bit 0x80 = nav-disable latch (checked,
                             * never set in the shipped game). Spawn seed = spawn_index & 7
                             * (@0x80042244) — staggers the per-tick DFS across entities. */
    uint8_t  ai_wp_node;    /* +0x1d6: wander-roam waypoint node id (set by the state-1 roam
                             * entry = re15_nav_rand_zone; consumed with ai_flags bit3) */
    /* LIVE STAGE1 zombie (@0x8011f7b4 family, FUN_80100424/FUN_80101224) attack windup timer
     * (entity +0x1da, s16). The active handler FUN_80101224 counts it down while the attack-arm
     * bit (ai_flags & 0x100 = +0x1d8 & 0x100) is set; at == 0x12c (300) it fires the lunge, at
     * == 0 it transitions to the post-attack recovery state. See re15_enemy_ai_live_active. */
    int16_t  ai_attack_timer;   /* +0x1da */
    /* HURT-state (FUN_80105a8c) fields. The original time-shares +0x1dc with ai_target_x (search
     * target) and uses +0x1d4 for the per-spawn stagger clip; the port keeps DEDICATED fields:
     *  - hurt_clip (+0x1d4): the stagger clip, seeded once per spawn = {2,3,4,5}[rng&7] (init
     *    FUN_80100688 @0x8010079c, table @0x8011f7e4). Copied to motion (+0x94) on HURT entry
     *    (stagger handler 0x80105b7c phase 0 @0x80105c30/c38).
     *  - hit_stun: the s16 hit-stun countdown (the +0x1dc semantic in HURT state). HURT holds while
     *    hit_stun >= 0, decrements per frame by step[+0x5] (@0x8011fe30), recovers to ACTIVE at < 0
     *    (FUN_80105a8c exit gate @0x80105b18). */
    uint8_t  hurt_clip;         /* +0x1d4 — per-spawn stagger clip {2,3,4,5} */
    int16_t  hit_stun;          /* HURT-stun countdown (the +0x1dc-in-HURT semantic) */
    /* HURT torso-bend (the visible RECOIL of a hit). The stagger handler does NOT switch clips — it
     * bends one bone while the walk clip keeps playing: `lhu part+100; lhu ent+0x9c; addu; sh part+100`
     * (@0x80105d54-64 bend-down / @0x80105df8-08 bend-up), where part = model_pool + 1204 = part index
     * 7 (stride 172, @0x80105ba4). +0x9c ramps -0x80/tick for 3 ticks, then +0x80/tick for 3
     * (@0x80105d7c-84 / @0x80105e20). The offset lands on the bone's vz euler — the SAME slot the
     * Plc_neck pitch uses (+0x64), see skeleton_common.c. bend_bone < 0 = inactive.
     * NOTE: only the INDEX 7 is proven (1204/172); the anatomical bone identity is not parsed. */
    /* RE2-AI option (inactive while re15_ai_flavor()==RE15). RE2 struct offsets in the comments. */
    uint8_t  re2z_gaitrow;      /* +0x16B RE2 gait row 0..31 into tbl @0x8010C924 */
    uint8_t  re2z_gaitinit;     /* port-only: has the gait machine been seeded for this walk? */
    uint16_t re2z_gaittmr;      /* +0x158 RE2 gait segment timer (frames left in this row)     */
    /* ---- RE2-Flavor WELLE B (enemy_ai_re2_zombie.c re15_re2z_tick): die RE2-Arbeitsbytes.
     * Jedes Feld traegt seinen RE2-Actor-Offset; Producer/Consumer-Adressen stehen am Code. */
    uint8_t  re2z_cd239;        /* +0x239 Moan-Cooldown (Set 150 @0x801027C0 u.a.; Root-Dec @0x8010045C-6C) */
    uint8_t  re2z_cd23e;        /* +0x23E Biss-Cooldown (Set 60 @0x80104E2C; Root-Dec @0x80100470-80)       */
    uint8_t  re2z_self1d3;      /* self+0x1D3 (Set 15 @0x8010276C-70; low-7-Dec im Root @0x80100484-98)     */
    uint8_t  re2z_flag222;      /* +0x222 "schon getroffen"-Marke (Flinch-Arbitrierung @0x80105080-9C)      */
    int8_t   re2z_res223;       /* +0x223 Flinch-Resistenz, signed (Seed 16+(rand&15) @0x80100888-9C)       */
    uint8_t  re2z_hits1d2;      /* +0x1D2 = zone + 3*bracket, PRO TREFFER GESETZT
                                 * (`sb v1,466` @0x80041A9C bzw. @0x80047330).
                                 * BRACKET (2026-08-18 zu Ende disassembliert): im Kontakt-Applier
                                 * FUN_800410CC der INDEX der getroffenen Angriffs-TEILBOX
                                 * (`addiu s4,v0,1|2|4` @0x800414A4/@0x800415AC/@0x800416B4 ->
                                 * sp+72 @0x80041768 -> `andi 0x2`/`0x4` @0x80041834-48); im
                                 * Projektil-Applier FUN_800470C0 `hitcode>>28` @0x80047114.
                                 * Bracket 0 = innerste Teilbox = direkter Treffer = der einzige
                                 * Bracket, bei dem Zeile 1 ueberhaupt Schaden macht
                                 * (0x800A412C w0 = 0x00000003). Der Port hat den Overlap-Test
                                 * FUN_80041CE4 und die Volumen-Tabelle 0x800A68E8 nicht — 1/2
                                 * sind daher nicht erzeugbar. Volle Kette in re15_damage.c.
                                 * Konsumenten: Zombie-Blut %3==0 (=Zone 0), Hunde-Gore /3 (=Bracket)  */
    uint8_t  re2z_walkclip;     /* +0x218 Walk-Clip aus dem Param-Block (@0x80100860-8C; Werte 0/2)         */
    uint8_t  re2z_dir16a;       /* +0x16A Fall-/Varianten-Byte (Knockdown-Seite, @0x8010328C-98)            */
    int16_t  re2z_t158;         /* +0x158 Budget/Timer (Grab-Wehr-Budget 148 @0x80102828-2C u.a.)           */
    int16_t  re2z_t15a;         /* +0x15A Idle-Moan-Timer (Seed rand+300 @0x8010148C-90)                    */
    uint16_t re2z_flags21a;     /* +0x21A Flag-Wort (INIT-Clear @0x8010087C; 0x20/0x40 Seiten-Latches,
                                 * 0x4 Liege-Orientierung, 0x10 Kriech-Marker, 0x8 Aufsteh-Latch
                                 * (@0x80103F88-90 set, @0x80102D60-64 clear), 0x4000 Kill-gezaehlt)        */
    uint16_t re2z_f10e;         /* +0x10E Spawn-/Zustands-Wort: 0x4000 Limpet-Latch (Spawns schreiben
                                 * 0x4002/0x4004 @0x80100A34-38/@0x80100A88-8C; einziger Overlay-Clear
                                 * @0x80104F0C EXEC[15]), 0x2000 "gefallen" (@0x80102DB8-C0/@0x80103308-20/
                                 * @0x80104468-70), **Bit 0 = KRIECHER** (Zustand-1-Wurzel
                                 * @0x80101154-74 -> Tabelle @0x8010C854, ungerade = 0x80101210)             */
    int16_t  re2z_root144;      /* +0x144 Wurzel-Delta X, den FUN_80015E7C ablegt (`sh v1,324(t0)`
                                 * @0x80015FD8, UNROTIERT). Der Kriecher liest ihn als Steuer-Gate
                                 * (`lh v0,324` / `slti 21` @0x801030C0-CC)                                  */
    uint8_t  re2z_prev_sub;     /* Port-Feld: ACTIVE-Sub-Schnappschuss fuer HURTs +0x5==1-Test
                                 * (@0x80105090-98) — das geteilte take_damage ueberschreibt +0x5          */
    uint8_t  re2z_grabclip;     /* P0-gewaehlter Grab-Clip (param[0x0C+s5*2] @0x801026C4-CC)                */
    uint8_t  re2z_bitefr;       /* Biss-Frame  aus dem (frame,dmg)-Paar param[0x14+s5*2]   @0x801028A0-AC   */
    uint8_t  re2z_bitedmg;      /* Biss-Schaden aus demselben Paar (Anwendung @0x801028F4-FC)               */
    int16_t  re2z_prev_hp;      /* Port-Feld: HP-Schnappschuss fuer die HURT-Resistenz-Abschreibung        */
    /* ---- RE2-Trefferreaktion: BONE-MATRIX-INJEKTION (das Oberkoerper-Zucken) -----------------
     * Die RE2-Trefferhandler drehen NACH dem Anim-Advance (FUN_8002959C) ZWEI Part-Matrizen des
     * Modell-Pools (+0x198): Part 0 wird NACH-multipliziert, Part 1 VOR-multipliziert mit der
     * transponierten (= gegenlaeufigen) Matrix. Belege, alle selbst disassembliert:
     *   Haupt-Handler P1 @0x801057A4-E8 : vec = (0,0, -((+0x158 * (s8)+0x16B) << 3) * +0x15A)
     *                                     RotMatrix 0x8008E1F4 -> MulMatrix  0x8008D934(part0+24)
     *                     @0x801057EC-838: vec.z = +((+0x158 * (s8)+0x16B) * (+0x15A << 3))
     *                                     RotMatrix          -> MulMatrix2 0x8008DA44(part1+24)
     *   Haupt-Handler P2 @0x801058D4-960: dasselbe OHNE den <<3 (vec.z = -/+(+0x158*+0x16B)*+0x15A)
     *   Ragdoll 0x801066FC @0x80106A04-3C / @0x80106CA0-D4: vec = (+0x13C,+0x13E,+0x140), zweite
     *                                     Matrix explizit ueber TransposeMatrix 0x8008E1B4.
     * Rz(-t) == Rz(t)^T (RotMatrix negiert bei negativem Winkel nur den Sinus, @0x8008E204-2C),
     * die drei Fundstellen sind also EIN Mechanismus: bone0 *= R(lean); bone1 = R(lean)^T * bone1.
     * Verrechnung mit der Keyframe-Pose: der Port baut die LOKALE Bone-Matrix aus den Eulern
     * (mat3_from_euler == RotMatrix, M = Rx*Ry*Rz) und multipliziert die Injektion direkt darauf —
     * ABSOLUT pro Tick, nicht akkumulierend (das Original baut die Part-Matrizen im Advance
     * jeden Tick neu, sonst waere die P2-Rampe 16->0 sinnlos). */
    int16_t  re2_lean[3];       /* +0x13C/+0x13E/+0x140 — der SVECTOR fuer RotMatrix           */
    uint8_t  re2_lean_on;       /* 1 = Injektion aktiv (wird zu Beginn jedes AI-Ticks geloescht) */
    /* Wurzel-Bone-Blend des Rutsch-Handlers 0x8010703C: FUN_80028F48(Identitaet @0x8009DB44,
     * Kopie der Part-0-Matrix, Part 0, w=+0x158) = elementweise ((4096-w)*I + w*M) >> 12
     * (@0x801072F8 / @0x801073C0). 0 = aus. */
    int16_t  re2_bone0_wgt;
    int16_t  re2z_gy232;        /* +0x232 = Kopie von +0x1C2 (Boden-Y) beim Ragdoll-Start
                                 * (`lhu v0,450; sh v0,562` @0x80106994-A0)                     */
    uint8_t  re2z_rag231;       /* +0x231 Ragdoll-Untermaschinen-Wahl (@0x80106738/@0x801067B0/
                                 * @0x8010681C) — 1 -> FUN_80109610, 2 -> FUN_801092C4, beide OPEN */
    /* ---- RE2-GORE/ZERLEGER (enemy_ai_re2_zombie.c, Welle E) ---------------------------------
     * Die drei ZONEN-POOLS +0x151/+0x152/+0x153 sind KEINE Erfindung: der RE2-INIT setzt alle
     * drei auf 13 (`addiu v0,zero,13` @0x8010081C, `sb v0,337/338/339(s2)` @0x80100820/24/28,
     * Zwilling im Restyle @0x801049B4-C0), und der EXE-Applier zieht je Treffer die 3-Bit-
     * Kosten des Schadens-Records ab, mit Saettigung bei -1:
     *   Region 0 -> +0x153 : `lw t0,112(sp)` / `bne t0,zero` @0x80041900-08, Abzug @0x80041910-44
     *   Region 1 -> +0x152 : `bne t0,1`      @0x80041950,    Abzug @0x80041954-88
     *   Region 2 -> +0x151 : `bne t0,2`      @0x80041994,    Abzug @0x80041998-CC
     *   Kosten  = `(rec->w1 >> (Bracket*3)) & 7`  (`sll v0,s1,1`/`addu v0,v0,s1`/`srlv`/`andi 7`)
     *   Klemme  = `sll v0,v0,24 / bgez / addiu v0,zero,-1 / sb` (@0x8004197C-88)
     * Die REGION ist exakt die Zone von +0x1D2: `+0x1D2 = 3*Bracket + Region` @0x80041A88-9C
     * (Zwilling @0x80047310-30) — der Port stempelt Zone 1, trifft also +0x152, und genau
     * `(s8)+0x152 < 0` ist das Gate des Zerleger-Zweigs @0x80105294-9C. */
    int8_t   re2z_pool151;      /* +0x151 hohe Zone   (Region 2) */
    int8_t   re2z_pool152;      /* +0x152 mittlere Zone (Region 1) — das Gore-Gate @0x8010529C */
    int8_t   re2z_pool153;      /* +0x153 tiefe Zone  (Region 0) */
    uint8_t  re2z_burn23a;      /* +0x23A Flammenwerfer-Trefferzaehler: INIT 0 @0x801008B4,
                                 * Inkrement @0x80105284/@0x80105578, Schwelle >= 9 @0x80105250 */
    uint16_t re2z_hitdir1d0;    /* +0x1D0 TREFFERRICHTUNG. Low-Byte wird pro Treffer neu gebaut
                                 * (`andi 0xff00` @0x80041384 / @0x80047178) aus
                                 * d = FUN_800154AC(Angreifer,Ziel) - Ziel+0x76 (@0x800419D8-A08):
                                 *   ((d+1024)&0xFFF) < 2048 -> |= 0x20 (Ruecken)  @0x80041A0C-2C
                                 *   ((d+1536)&0xFFF) < 1024 -> |= 0x40 (Seite A)  @0x80041A30-58
                                 *   ((d- 512)&0xFFF) < 1024 -> |= 0x80 (Seite B)  @0x80041A5C-84
                                 * (Zwilling im Schuss-Applier @0x80047360-D8.) */
                                /* Der Port stempelt +0x1D0 in der HURT-Wurzel, wenn +0x6 == 0 ist
                                 * — genau die Flanke, die re15_damage.c je Treffer erzeugt. */
    /* Der MODELLBLOCK +0x198 ist im Original ein Array aus 172-Byte-Part-Records (Stride 0xAC,
     * belegt durch die Offsetleiter 112/284/456/.../2520 = Part n*172+112 in FUN_80106128 und
     * durch `sw zero,2580` = Part 15 @0x801010DC). Der Port hat keinen solchen Block; diese zwei
     * Arrays bilden die zwei Felder ab, die der Zerleger-Zweig LIEST und SCHREIBT:
     *   [i][+0x00] Flag-Wort — Bit 0 = "Part vorhanden" (die Rauch-Emitter gaten darauf,
     *              `lw v0,516(a2)`/`andi 1` @0x801061C0-CC), 0x4A = abgerissener Arm
     *              (@0x80107544/@0x801075B4/@0x80107630), 0x1062 = abgesprengtes Bein
     *              (@0x8010537C), 0x10 = weggeaetzt (@0x80105EC4)
     *   [i][+0x70] Farbwort (r,g,b; LOW BYTE = R, Byte 3 = GPU-Code, `sw`@0x80027C08 +
     *              `sb`@0x80027C18) — neutral 0x00808080 (FUN_80028368.c:55), Verkohlung
     *              0x00404040.. (@0x8010627C-F4), Saeure 0x00304040.. (@0x8010633C-84),
     *              blutiger Stumpf 0x0010104F = r 0x4F/g 0x10/b 0x10 (@0x80107568)
     * ANZEIGE-BRUECKE (jetzt vorhanden): re15_re2z_gore_resolve() bildet den Part-Draw-Walk
     * FUN_80027160 @0x80027160 nach — Bit-0-Sichtbarkeitstest @0x8002737C/@0x800273C4 (flach)
     * plus die Eltern-Kaskade `(Eltern & 0x21) == 0x20` @0x80027480-94; die Tinte MULTIPLIZIERT
     * das Beleuchtungsergebnis (GTE `ldrgb`@0x80027C2C + `NCCT`@0x80027D10). Vollstaendige
     * Belegkette im Kopfkommentar der Bruecke in engine/src/enemy_ai_re2_zombie.c. */
    uint16_t re2z_part_flags[16];
    uint32_t re2z_part_tint[16];
    /* [i] = welcher MD1-Objektindex die GEOMETRIE dieses Parts liefert. Im Original sind das
     * die vier Wörter [i][+0x08/+0x0C/+0x10/+0x14] (Geometrie- und Paketzeiger, gelesen vom
     * Zeichner FUN_80027434 @0x80027AD4-B04); der Zerleger TAUSCHT sie:
     *   80105324: lw v0,2588(v1) / 8010532c: sw v0,8(s0)
     *   80105330: lw v0,2596(v1) / 80105338: sw v0,16(s0)
     *   8010533c: lw v0,2592(v1) / 80105344: sw v0,12(s0)
     *   80105348: lw v0,2600(v1) / 80105350: sw v0,20(s0)
     * 2588/2592/2596/2600 = 15*172 + 8/12/16/20 = die vier Mesh-Wörter des RESERVE-Parts 15.
     * Der Port hat statt Zeigern Objektindizes (Part i == Bone i == MD1-Mesh i, gepinnt in
     * test_re2_gore_render), also ist der Zwilling `re2z_part_mesh[thigh] = 15`. Seed = i. */
    uint8_t  re2z_part_mesh[16];
    /* ---- DAS FREIFLIEGENDE TEIL (Welle G) -----------------------------------------------------
     * Traegt Bit 0x40, dann UEBERSPRINGT der Zeichner die Eltern-Verkettung
     * (`andi v0,s3,0x40` @0x80027498 / `bne v0,zero,0x800275E4` @0x8002749C in FUN_80027434)
     * und benutzt die Matrix, die im
     * Part-Record selbst steht: rec+0x48 ist eine PSX-MATRIX (m[3][3] shorts @+0x48..+0x59,
     * t[3] longs @+0x5C/+0x60/+0x64). Beleg, dass param_4 == rec+0x48 ist: der Draw-Walk
     * uebergibt sie so (`_addiu a3,s2,0x48` @0x80027390 bzw. `addiu s0,s2,72` @0x80027370/B0
     * mit `addiu s0,s0,172` @0x800273F8 im Gleichtakt zu `addiu s2,s2,172`), und die Physik
     * liest/schreibt sie ueber BEIDE Wege deckungsgleich (FUN_80028AD8 ueber param_2+20/24/28,
     * FUN_80028DAC ueber param_1[0x17]/[0x18]/[0x19] = rec+0x5C/+0x60/+0x64).
     * Die restlichen Felder sind die Wurf-/Flug-Zustaende, die die beiden Physiken benutzen:
     *   +0x38/+0x3A/+0x3C  Wurf-Parameter (yaw, vy, Vortrieb), nach dem INIT die Geschwindigkeit
     *                      (`ApplyMatrixSV(..., s1+56)` @0x80028B3C-44)
     *   +0x3E/+0x40/+0x42  Ziel-Rotation der Setz-Blende (`lhu 62/64/66` @0x80028D34-4C)
     *   +0x79  Gravitation je Frame (`lbu 121` @0x80028B90)
     *   +0x7A  Blend-Zaehler 15..0 (`sb 15,122` @0x80028B54, `lb 122` @0x80028D24)
     *   +0x86  Aufschlag-Budget 3..1 (`sh 3,134` @0x80028B4C, `--` @0x80028BEC-F0)
     *   +0x98/+0x9A/+0x9C/+0x9E/+0xA0/+0xA4  die Felder der ZWEITEN Physik FUN_80028DAC
     *          (Kurs, vy, Vortrieb, vy-Zuwachs, Lebensdauer, Vortriebs-Zuwachs)
     * re2z_part_seeded ist ein PORT-Feld: das Original hat die Matrix immer live im Record,
     * der Port friert sie beim ersten Frame mit Bit 0x40 aus der Skelett-Pose ein. */
    int16_t  re2z_part_v[16][3];    /* +0x38/+0x3A/+0x3C */
    int16_t  re2z_part_rot[16][3];  /* +0x3E/+0x40/+0x42 */
    int16_t  re2z_part_m[16][9];    /* +0x48 MATRIX m[3][3], Q12                                  */
    int32_t  re2z_part_t[16][3];    /* +0x5C MATRIX t[3], Weltkoordinaten                         */
    int8_t   re2z_part_grav[16];    /* +0x79 */
    int8_t   re2z_part_blend[16];   /* +0x7A */
    int16_t  re2z_part_st86[16];    /* +0x86 */
    int16_t  re2z_part_yaw98[16];   /* +0x98 */
    int16_t  re2z_part_w9a[16];     /* +0x9A */
    int16_t  re2z_part_w9c[16];     /* +0x9C */
    int16_t  re2z_part_w9e[16];     /* +0x9E */
    uint16_t re2z_part_life[16];    /* +0xA0 */
    int16_t  re2z_part_wa4[16];     /* +0xA4 */
    uint16_t re2z_part_seeded;      /* PORT: Bit i = Matrix von Part i ist eingefroren            */
    uint16_t re2z_part_stepped;     /* PORT: Bit i = Part i hat in re2z_part_frame schon geschritten */
    uint32_t re2z_part_frame;       /* PORT: Frame des letzten Physik-Schritts (Ein-Schritt-Sperre,
                                     * im Original haengt der Schritt am ZEICHNEN:
                                     * `andi v0,s3,0x20` @0x80027694 -> `jal 0x80028AD8` @0x800276A0,
                                     * `jal 0x80028DAC` @0x80027B98)                              */
    /* ---- RE2-Flavor WELLE C (enemy_ai_re2_dog.c re15_re2dog_tick): die Cerberus-Arbeitsbytes
     * aus EMD0G_MOD0.BIN (ModB @0x80100000). Geteilt mit dem Zombie werden speed_h(+0x144),
     * re2z_t158(+0x158), re2z_t15a(+0x15A), re2z_dir16a(+0x16A Aggro/Mash, signed gelesen),
     * re2z_flags21a(+0x21A Fatigue u16), re2z_hits1d2(+0x1D2), re2z_self1d3(+0x1D3),
     * re2z_prev_sub/prev_hp (Port-Schnappschuesse) und dog_floor_y(+0x1C2 Boden-Y). */
    int16_t  re2d_vy146;        /* +0x146 vy (Sprung -280 @0x801013A4, Gravity +40 @0x801014C0 u.a.) */
    int16_t  re2d_turn224;      /* +0x224 RUN-Drehrate (96 @0x80100BCC/BE4, Tabelle @0x80100C4C)      */
    int16_t  re2d_offx228;      /* +0x228 STALK-Zieloffset X (tbl @0x801054A8, @0x801008AC-C4)        */
    int16_t  re2d_offz22a;      /* +0x22A STALK-Zieloffset Z (@0x801008C0-E0)                          */
    uint8_t  re2d_route218;     /* +0x218 Wegpunkt-/Routen-Byte (0x8004AA50-Rueckgabe; Port: MAPPING)  */
    uint8_t  re2d_air219;       /* +0x219 Luft-Flag (Set 1 @0x8010132C, Clear bei Landung @0x801015F0) */
    uint8_t  re2d_abort21c;     /* +0x21C Abdreh-Flag (Set @0x80100DD0-D4/-4-Zweig, Consume @0x80100E14-28) */
    uint8_t  re2d_pause21d;     /* +0x21D Chase-Pause (Seed tbl@0x80105420 @0x801017DC-E4, Dec @0x80100D3C) */
    uint8_t  re2d_bite21e;      /* +0x21E Kontakt-Resultat: 1 Boden-Biss/2 LATCH (@0x80104F24/@0x80104F84) */
    uint8_t  re2d_budget21f;    /* +0x21F Blut-/FX-Budget (Gate+Dec @0x80105090-98/@0x8010518C-98)     */
    uint8_t  re2d_rel220;       /* +0x220 Release-Einmal-Latch des Latch (@0x80102024-48)              */
    uint8_t  re2d_launch222;    /* +0x222 "im Sprung"-Flag (Set @0x80101330, Clear @0x80101460)        */
    uint8_t  re2d_dbl223;       /* +0x223 Doppel-Treffer-Marker (Set 1 @0x80103488, |0x80 @0x80103400) */
    uint8_t  re2d_nolatch22c;   /* +0x22C Kein-Latch-Marke (Kreis/Fenster; Gate @0x80105000-08)        */
    uint8_t  re2d_wound22d;     /* +0x22D verwundeter Spawn (Set @0x80100438; Gates @0x801033F0 u.a.)  */
    uint8_t  re2d_circle22e;    /* +0x22E Kreis-Modus (Spawn 9 @0x80100480; RUN-P0 @0x80100C58-7C)     */
    uint8_t  re2d_atkcd22f;     /* +0x22F Angriffs-Cooldown P3 (tbl@0x80105430+30 @0x801011B8-C4)      */
    uint8_t  re2d_stuck230;     /* +0x230 Stuck-Zaehler (Root @0x801000C8-D8; Unstick-Gates >=16)      */
    uint8_t  re2d_se231;        /* +0x231 Todesschrei-Latch (Gate @0x801041B8-C4, Set @0x801046E8)     */
    uint8_t  re2d_cd232;        /* +0x232 Root-Countdown (Dec @0x80100040-50)                          */
    /* ---- RE2-Flavor WELLE D (enemy_ai_re2_crow.c re15_re2crow_tick): die Kraehen-Arbeitsbytes
     * aus EMOVL21_S0.BIN (Slot 0 @0x80100000). GETEILT mit Zombie/Hund werden die gleich-
     * adressierten Felder: re2d_route218(+0x218 next-Phase/Accel-Step/Spin s8-gelesen),
     * re2d_air219(+0x219 Timer/Grab-Timeout), re2z_flags21a(+0x21A Flap-Reload), re2d_abort21c
     * (+0x21C Angriffs-Cooldown), re2d_pause21d(+0x21D Wand-Streak), re2d_bite21e(+0x21E
     * Kontakt-Streak), re2d_budget21f(+0x21F Routen-Byte 0x8004AA50), re2d_rel220(+0x220
     * Nav-Zaehler rand&0x7f), re2d_turn224(+0x224 Zielhoehe/Grav-Akku/Snapshot-X),
     * re2d_offx228(+0x228 Spiral-Spin ±200), re2z_t158(+0x158), re2z_t15a(+0x15A Lache-Ticks),
     * re2z_self1d3(+0x1D3), dog_floor_y(+0x1C2 Boden-Y), ai_dist(+0x1F0),
     * crow_shadow_w/h (Schatten-Record [+0x16C]+4/+6), crow_hide (GIB-Mesh-Wipe). */
    uint8_t  re2c_grab21b;      /* +0x21B GRAB-Phasen-Zaehler (Set 4 @0x801026B0-B4, Dec @0x80102700-10,
                                 * Reload (rand&7)+2 @0x80102718-2C, Lift-Ende @0x80102818-24)          */
    int8_t   re2c_pac221;       /* +0x221 Flock-Pacifier, s8 (Set 120 im Post-Pass @0x8010451C-20;
                                 * Dec im ACTIVE-Prolog @0x80100558-80; Gates lb/bgtz @0x801006D8-E0)   */
    int16_t  re2c_snap226;      /* +0x226 Idle-Hop-Snapshot-Z (sh @0x80100930) / HURT-Knock-Richtung
                                 * (Bearing(PL->self)-Yaw, sh @0x801029D4)                              */
    uint16_t re2c_flags22a;     /* +0x22A Flag-Wort (lhu/sh 554): 1 Wake-Timer-Arm (Takeoff @0x80100D0C-20,
                                 * Sub-0-DEC @0x80100754-7C), 2 LOS (Root @0x801001C8-E8, ret==0 -> set),
                                 * 4 Mutex-Claim (@0x801042D0-DC), 8 Grab-Join (Broadcast @0x80102540-44,
                                 * Konsum 8+2 @0x8010420C-24), 0x10 Abort (@0x80104410-1C), 0x20
                                 * Kraechz-Sperre (Idle-P10 @0x80100A04-10, State-4 @0x801035B0-BC),
                                 * 0x40 Konvergenz (@0x80102548-4C, Nav-Gate @0x80100584-90), 0x80
                                 * Pacify-Broadcast (@0x80102680-84, Post-Pass @0x80104504-20)          */
    /* ---- RE2-Flavor WELLE E (enemy_ai_re2_spider.c re15_re2spider_tick): die Spinnen-
     * Arbeitsbytes aus EMS25.BIN (Adult 0x25) / EMS26.BIN (Baby 0x26), Slot 0 @0x80100000.
     * GETEILT mit Zombie/Hund/Kraehe werden die gleich-adressierten Felder: hp(+0x156),
     * re2z_t158(+0x158), re2z_t15a(+0x15A), speed_h(+0x144 Schritt-X), re2d_vy146(+0x146),
     * re2z_self1d3(+0x1D3), re2z_f10e(+0x10E Spawn-Deskriptor), ai_dist(+0x1F0).
     * Die folgenden Offsets kollidieren in der BREITE mit Hund/Kraehe (die lesen +0x218/+0x21A/
     * +0x21C als BYTE bzw. anderes Wort) — deshalb eigene Felder, keine Umdeutung fremder. */
    int16_t  re2s_z148;         /* +0x148 Schritt-Z (walk-P0 `sh zero,328` @0x8010093C, Attack-P0
                                 * @0x80100D5C) — dritte Komponente des 0x800152C8-Vektors        */
    int16_t  re2s_t218;         /* +0x218 Re-Decide-Sperre, lh/sh (Dec im ACTIVE-Tail
                                 * @0x80100600-14; Seed 15 @0x80100F58/@0x80101068)               */
    int16_t  re2s_yaw21a;       /* +0x21A Oberflaechen-Yaw-Offset (INIT 0 Boden / 2048 Decke
                                 * @0x80100488, 2048 Wand @0x801004C4) — a1 von FUN_800152C8     */
    int16_t  re2s_t21c;         /* +0x21C Anzeige-/Freeze-Zaehler (ACTIVE-Tail-Gate @0x8010062C) */
    uint16_t re2s_c21e;         /* +0x21E Frame-Zaehler, laeuft solange +0x21C != 0
                                 * (@0x8010063C-4C; Clear @0x8010065C/@0x80101384)                */
    uint8_t  re2s_mode222;      /* +0x222 OBERFLAECHEN-MODUS 0=Boden 1=Decke 2=Uebergang 3=Wand.
                                 * INIT-Sprungtabelle @0x80100004 nach (+0x10E & 0xF):
                                 * 0 -> 0 (@0x80100460), 2/3 -> 1 (@0x8010047C), 4..11 -> 3
                                 * (@0x801004B4). ACTIVE dispatcht darauf @0x801005D4.           */
    uint8_t  re2s_gs225;        /* +0x225 Boden-Kontakt-Automat (INIT 1 fuer Wand @0x80100588;
                                 * Tabelle @0x801000B4, Treiber FUN_80104DE8)                    */
    int32_t  re2s_y22c;         /* +0x22C Referenz-Y der Oberflaeche (Decke Y+1250 @0x801004AC,
                                 * Wand 1250-1800*((+0x10E&0xF0)>>4) @0x8010057C)                */
    uint8_t  re2s_legs220;      /* +0x220 BITMASKE der abgetrennten Beine (FUN_80105BF0
                                 * `+0x220 |= 1<<k` @0x80105C64; Test @0x80105C34 /
                                 * @0x8010376C / @0x801039F8)                                  */
    uint8_t  re2s_legn221;      /* +0x221 verbleibende Beine, INIT 8 @0x80100374/@0x801003B4;
                                 * Dec @0x80105C74/@0x80103A3C; Gate `< 3` @0x80105C20 /
                                 * @0x801039E4                                                 */
    uint8_t  re2s_fall223;      /* +0x223 "faellt gerade" (1 @0x801031EC / @0x80104764,
                                 * 0 @0x8010336C)                                              */
    uint8_t  re2s_sink23e;      /* +0x23E Wasser-Sink-/Schwimm-Zustand 0..3 (FUN_80104F18)     */
    uint8_t  re2s_sink23f;      /* +0x23F Sink-Freigabe (1 @0x80103E94/@0x80104134/@0x8010463C,
                                 * 0 @0x80104F48)                                              */
    int16_t  re2s_sink244;      /* +0x244 Sink-Akkumulator (@0x80104F94/@0x80104FA8)           */
    int16_t  re2s_water10c;     /* +0x10C WASSER-/BODEN-Y aus FUN_800527B4(X,Z) (HURT-Kopf
                                 * @0x80102CB0-D0, DEATH-Kopf @0x80103CB8-D8). 0 = "kein
                                 * Wasser-AOT getroffen". Der Port hat KEINEN Zwilling des
                                 * sce==7-AOT-Scans -> bleibt 0 (OPEN, aber mit dem
                                 * Original-Rueckgabewert des Nicht-Treffers).                 */
    uint16_t re2s_yaw226;       /* +0x226 ANKER-YAW der Oberflaeche. Clear @0x80101D9C
                                 * (FUN_80101D04), Setzer 2048 @0x8010244C. Gelesen von der
                                 * Decken-Ausrichtung @0x80101A08/@0x80101A38, dem Faden-Winkel
                                 * @0x80102360 und FUN_80102B10 @0x80102B14.                    */
    int32_t  re2s_p228;         /* +0x228 Anker-Parameter (a1 von FUN_801054D0 @0x80101688 /
                                 * @0x80102BFC). Im Modul EMS25.BIN gibt es KEINEN Schreiber
                                 * (eigener Store-Scan ueber alle sb/sh/sw mit Offset 552) —
                                 * der Wert kommt aus der EXE. Port: bleibt 0 (OPEN).           */
    uint8_t  re2s_next231;      /* +0x231 NAECHSTER Substate nach dem Abseilen (Setzer 2
                                 * @0x80101EC0, Clear @0x80102930/@0x80102AE4; gelesen
                                 * @0x80101FBC, @0x801022B4, @0x801022F8)                       */
    uint8_t  re2s_f236;         /* +0x236 Faden-/Seil-Sichtbarkeit: 1 @0x801016AC / @0x801022F4 /
                                 * @0x8010245C, 0 @0x80101B80 / @0x80102774. Reines Render-Bit
                                 * ohne Port-Konsument, wird aber byte-true mitgefuehrt.        */
    int16_t  re2s_partner240;   /* +0x240 PARTNER-Spinne. Im Modul EMS25.BIN gibt es GENAU EINEN
                                 * Schreiber, den INIT-Clear `sw zero,576(s2)` @0x8010042C —
                                 * gesetzt wird der Zeiger ausserhalb des Moduls (EXE). Der Port
                                 * hat dafuer keinen Erzeuger -> immer -1 (OPEN, kein Ersatz).
                                 * Port fuehrt einen SLOT-Index statt eines Zeigers.            */
    uint8_t  re2s_q230;         /* +0x230 Wand-Quadrant (+0x10E-4)>>1 @0x801004DC               */
    uint8_t  re2s_c232;         /* +0x232 Byte-Countdown (ACTIVE-Tail-Dec @0x80100618-28)        */
    uint8_t  re2s_snap233[3];   /* +0x233/+0x234/+0x235 = Schnappschuss von +0x5/+0x6/+0x7,
                                 * genommen wenn +0x21C == 0 (@0x80100650-68)                    */
    uint8_t  re2s_row23c;       /* +0x23C GELATCHTE Waffenzeile (nur wenn +0x6 == 0:
                                 * HURT @0x80102CAC, DEATH @0x80103CB4) — steuert das
                                 * Blut-FX-Gate (@0x80102DC0 / @0x80103DDC)                    */
    uint8_t  re2s_dead239;      /* +0x239 "eingeschmolzen": blockt die CORPSE-Wiederbelebung
                                 * HP=1 (@0x80103EA0-B8); Setzer @0x80104C28 (Zeile 11)        */
    uint8_t  re2s_done224;      /* +0x224 Leiche fertig (@0x80103EB8) — routet einen weiteren
                                 * Treffer auf den Zerstoerungspfad (@0x80103D38)              */
    uint8_t  re2s_tgt237;       /* +0x237 Ziel = Partner statt Spieler (FUN_80105F98 @0x80105FF8);
                                 * im Port IMMER 0 (RE1.5 hat keinen Partner-Slot, s. Datei)     */
    uint8_t  re2s_se238;        /* +0x238 Schritt-SE-Rotation mod 3 (FUN_80106004 @0x80106084)   */
    int8_t   re2s_c23a;         /* +0x23A signed (DEATH-SE-Wahl @0x80103FF4-4004)                */
    uint8_t  re2s_fx23b;        /* +0x23B FX-Emitter-Ticks (FUN_80106140 @0x80106158-68)         */
    uint8_t  re2s_c23d;         /* +0x23D Root-Countdown (@0x80100104-114)                       */
    uint8_t  re2s_f106;         /* +0x106 Etagen-Index = -Y / 1800 (Root @0x80100144-70)         */
    uint8_t  re2s_seeded;       /* PORT-Feld: INIT-HP schon gezogen? (RE1.5-Spawns setzen hp)    */
    uint8_t  re2_hp_stamped;    /* PORT-Feld (re15_damage.c re15_re2_hp_sync): RE2-INIT-HP schon
                                 * gestempelt? Wird beim Zustand 0 (INIT steht aus) und beim
                                 * Deaktivieren des Slots wieder geloescht. NUR im RE2-Flavor
                                 * gelesen — der RE1.5-Pfad fasst das Feld nie an.              */
    int16_t  hurt_bend_bone;    /* part index to bend, -1 = none */
    int16_t  hurt_bend_vz;      /* the PRE-update +0x9c applied this tick */
    /* Phase 4.5.13-RE2 F1: speed was at ID 27 (wrong) — correct ID is
     * 0x16 (+0x1CC in RE2). Renamed for clarity; opcode 0x35 Speed_set
     * uses an indexed velocity vector (ID 0x17..0x1A), not this scalar. */
    int16_t  speed_h;      /* ID 0x16 → speed_h    (RE2 +0x1CC)           */
    /* Velocity vector — written by Speed_set (0x35), integrated by
     * Add_speed (0x36) each tick. RE2 uses IDs 0x17/0x18/0x19/0x1A for
     * X/Y/Z/W. The W component is angular (rot_y) velocity. */
    int16_t  vel_x;        /* ID 0x17 → velocity X (RE2 +0x1D4)           */
    int16_t  vel_y;        /* ID 0x18 → velocity Y (RE2 +0x1D6)           */
    int16_t  vel_z;        /* ID 0x19 → velocity Z (RE2 +0x1D8)           */
    int16_t  vel_w;        /* ID 0x1A → angular Y  (RE2 +0x1DA)           */
    /* Target/lookat (IDs 0x20..0x25, RE2 +0x94..+0x9E). Used by Plc_neck
     * mode 4 ("look-at") and the F8 NCCT lighting target.               */
    int16_t  target_x;     /* RE2 +0x94                                   */
    int16_t  target_y;     /* RE2 +0x96                                   */
    int16_t  target_z;     /* RE2 +0x98                                   */
    int16_t  lookat_x;     /* head-look PITCH applied to bone 3 (set by neck FSM) */
    int16_t  lookat_y;     /* head-look YAW   applied to bone 3 (set by neck FSM) */
    int16_t  lookat_z;     /* RE2 +0x9E                                   */
    /* Plc_neck head-look FSM — byte-true FUN_80037358 (per-frame consumer) + opcode handler
     * @0x80041e98. GLOBAL (cutscene_headlook.md, 2026-08-03): the FSM runs for the PLAYER
     * (caller @0x80031d78) AND for every STAGE1 NPC root (jal 0x80037358 @0x8011c69c and
     * siblings) — the opcode targets the per-thread WORK entity (`lw v1,0x154(a0)`
     * @0x80041e9c), never implicitly the player. Flag bits (+0x1b8, after the 0x80 base):
     *   0x04 world point (+0x160/162/164) · 0x08 RELATIVE (+0x162 yaw offset on the body
     *   facing, +0x164 pitch target) · 0x40/0x20 yaw/pitch SWEEP (counter +0x160, mirror on
     *   snap-arrival, completion -> flags=0x12 @0x80037698/@0x80037858) · 0x10 pitch->0 ·
     *   0x02 target=keyframe (release) · 0x80 steps from the SCD speed bytes (+0x9e/0x9f)
     *   instead of the part defaults. Flags LOW BITS == 0 -> ENTITY TRACKING (look at the
     *   head part of neck_target_slot; NPC INIT default = the player @0x8011c738). */
    uint8_t  neck_flags;   /* +0x1b8 flag bits (see above) */
    int16_t  neck_tx, neck_ty, neck_tz;  /* +0x160/162/164: operands (world point / relative
                                          * offsets / sweep counter in tx, mirrored target in ty) */
    int16_t  neck_speed;   /* SCD speed s16: low byte = yaw step (+0x9e), high = pitch (+0x9f) */
    int16_t  neck_yaw;     /* part+0x94: current slewed head yaw   (signed) */
    int16_t  neck_pitch;   /* part+0x96: current slewed head pitch (signed) */
    uint8_t  neck_bone;    /* +0x1b9: head part index (player 8 @0x80031938, NPC 8 @0x8011c778);
                            * 0 = entity has no neck data -> FSM skipped (only player + NPC
                            * roots run FUN_80037358 in STAGE1) */
    uint8_t  neck_sweep;   /* port: sweep armed latch (first pass seeds target = full clamp) */
    int16_t  neck_step_yaw, neck_step_pitch;    /* part+0x98/+0x9a defaults: player 96/96
                                                 * @0x800319a4-ac, NPC 64/48 @0x8011c790-98 */
    int16_t  neck_clamp_yaw, neck_clamp_pitch;  /* part+0x9c/+0x9e clamps: player ±0x200/±0x138
                                                 * @0x800319b0-c4, NPC ±0x2c8/±0x138 @0x8011c7a0/b0 */
    int8_t   neck_target_slot;   /* +0x1a8 entity-tracking target (actor slot; -1 = none) */
    /* Cached head-part WORLD position from this actor's last posed frame (= the original's
     * part+0x54/58/5c, read by a TRACKING looker on the next frame). ok=0 until first pose. */
    int32_t  head_world[3];
    uint8_t  head_world_ok;
    /* Animation =========================================================*/
    int16_t  motion;       /* ID 8 (0x8) → motion_no/clip id (RE2 +0x0A)  */
    int32_t  anim_frame;   /* per-clip frame counter (engine-side, RE2
                            * stores this in a separate work-area)        */
    uint8_t  anim_freeze;  /* 2026-06-17: set on a scripted Plc_dest walk/run ARRIVAL
                            * (actor_locomotion.c) → re15_compute_actor_kf freezes the held
                            * walk clip at frame 0 (byte-true FUN_8001f3bc end-of-clip wrap),
                            * matching the original which holds the walk clip (no idle reset).
                            * Cleared by re15_actor_set_motion on the next motion change.    */
    /* T-round (2026-05-25): per-actor anim flag word (RE2 +0x1c4).
     * Bit 0x04 = LOOP (else FREEZE on clip end — original engine default).
     * Bit 0x08 = "second pass" enable.
     * Bit 0x40 = clear interp counter.
     * Bit 0x80 = REVERSE playback (Plc_flg(0,128,0) sets this).
     * Written by Plc_motion's pc[3] high byte + Plc_flg subop=OR/SET/XOR. */
    uint16_t anim_flags;   /* also RE1.5 Member id 16 → +0x1c4 (lhu) */
    /* BD-round 2026-05-28: per-actor Plc_motion FSM-init delay. PSX
     * Plc_motion writes state=4 to actor+0x4; on NEXT tick FSM transitions
     * state=4→state=1 and zeros anim_frame (per T8 disasm). Set to 1 on
     * motion change so first tick the animation is FROZEN (no anim_frame
     * progression), matching PSX's 1-tick init delay between Plc_motion
     * fire and visible animation start. */
    uint8_t  motion_init_delay;
    /* === Keyframe CROSSFADE (byte-true FUN_8001f3bc FRAC = entity+0x8f) =====
     * On a motion change the original blends sub-frames from the PREVIOUS
     * rendered pose into the new clip: root translation via GTE gpf12/gpl12 and
     * EVERY bone angle via FUN_80020510 (shortest-arc 12-bit lerp), with weight
     * 0x1000-0x200*frac. anim_frac counts down (one step per rendered frame); 0
     * = no blend (steady playback). prev_angles/prev_root hold the last RENDERED
     * pose, updated in re15_skel_compute_pose (which reads the actor via the
     * global g_anim_pose_actor = the original's DAT_800ac784 current-actor).
     * The [32] MUST match RE15_EMD_MAX_BONES (re15_emd.h, included below the
     * struct so the literal is used here). Seeded to 7 on every motion change. */
    uint8_t  anim_frac;
    uint8_t  anim_prev_valid;
    uint16_t anim_blend_rate;   /* the FUN_8001f314 4th arg = the crossfade WEIGHT STEP: prev weight
                                 * = anim_frac * rate (0x1000 = 100%). WALK-family zombie states pass
                                 * 0x100 (max 0xf00 = 94%, decaying -6%/frame — never a frozen
                                 * plateau); one-shots (stand-up, grab, devour, the player handlers)
                                 * pass 0x200. 0 -> treated as 0x200 (the player default). */
    /* Player animation-bank select. 1 = PL00.EDD (the original's COMMON/char-keyed bank, holds
     * the ROOM1150 kneel clip 11 = the real ~25-frame stand→kneel pelvis fold py -1810→-761);
     * 0 = the room RBJ cinematic overlay (ROOM1170 wave + the clip-11 HOLD/settle, gesture clips
     * absent from PL00.EDD). Set by op_plc_motion, honored in re15_actor_anim_select for the
     * player. RESULT-CORRECT selector, validated on all shipped data. Full RE (2026-06-18,
     * see memory anim_bank_selection_mechanism_2026_06_18): the Plc_motion entity operand (pc[1])
     * only lands in actor+0x05 — it does NOT pick the bank in the original — but it co-varies
     * faithfully with the bank everywhere checked. Incl. ROOM1150 sub08: clip 11 at entity 1 =
     * kneel DOWN/UP (the PL00 fold), at entity 0 = HOLD the kneel during dialog (the RBJ settle).
     * clip 11 is deliberately authored in BOTH banks (PL00.EDD=fold, RBJ=settle), so the entity
     * byte IS the intended fold-vs-hold switch — matches original_kneeing/ (fold-then-hold).
     * ⚠️ The original's COMMON-vs-RBJ pose selector itself is NOT fully RE'd (the RBJ-pose reader
     * is unconfirmed; DAT_800aca5a switches WEAPON↔COMMON, not COMMON↔RBJ). The entity rule is the
     * faithful, lower-risk expression; revisit only if a room breaks the entity↔bank co-variance. */
    uint8_t  anim_use_pl00;
    int16_t  prev_angles[32][3];
    int32_t  prev_root[3];
    /* Plc_dest walker state (Phase 4.5.13-RE2 F3 — port FUN_8001ed9c) ==
     * When SCD fires Plc_dest, we stash dest+mode here and the per-frame
     * integrator drains it: yaw lerp toward dest, position step, set
     * arrival flag when Manhattan/Euclidean distance < step_size.       */
    uint8_t  walk_active;     /* 1 = walking toward dest                  */
    uint8_t  walk_mode;       /* hi-byte from Plc_dest: 0x05 RUN, 0x09 TURN */
    uint8_t  walk_flag_bit;   /* arrival flag bit in zone 5 (lo byte)     */
    uint8_t  walk_pad;        /* (legacy, unused since BO walker FSM)      */
    uint8_t  walk_fsm;        /* BO Tier-3: PSX DAT_800aca5a 0=init 1=align 2=step */
    int16_t  walk_dest_x;
    int16_t  walk_dest_z;
    /* AO6-round 2026-05-26: target yaw captured ONCE when Plc_dest fires,
     * not recomputed each tick. Per [[walker_canonical_N4_2026_05_24]] the
     * PSX walker (FUN_800245d8) doesn't do atan2 — target is set ONCE by
     * the motion-mode handler via FUN_8001a6d4 atan2, then FUN_8001a8f8
     * slews actor.rot_y toward it. Per-tick atan2 in our previous code
     * caused overshoot oscillation: walker passes dest by step-size, new
     * atan2 flips 180°, walker spins back, overshoots, repeats forever.
     * User-visible: Leon spinning around Y-axis after spawn. */
    int16_t  walk_target_yaw;

    /* Root-motion integration cache (Phase 4.5.13-RE2 I-FINAL).
     * Stores the previous tick's keyframe pos so we can compute the
     * per-frame delta (current_kf_pos - prev_kf_pos) and apply it
     * (yaw-rotated) to actor.pos. RE2-faithful: clip keyframes carry
     * absolute root positions in clip-local coords; the engine moves
     * the actor by their per-frame differential. */
    int16_t  root_prev_kf;     /* last applied keyframe index (-1 = none) */
    int16_t  root_prev_motion; /* motion when prev_kf was recorded        */
    int16_t  root_prev_x;
    int16_t  root_prev_y;
    int16_t  root_prev_z;
    int16_t  root_pad;

    /* Crow (type 0x21) 3D-flight state — byte-true FUN_80112020 family (RE15_CROW_AI.md).
     * The crow shares this actor struct but flies in 3D: y (+0x38) IS the altitude
     * (smaller y = higher). Its flight fields live BEYOND the modeled zombie offsets
     * (+0x1e4/+0x1ea/+0x1ec/+0x1d4), so they get dedicated names — same pattern as the
     * dedicated hurt_clip/hit_stun fields above. */
    int16_t  crow_perch_h;   /* +0x1ea: target/perch height, seeded once at INIT (= spawn y) */
    int16_t  crow_vert_err;  /* +0x1ec: playerY - y, recomputed every ACTIVE tick            */
    int16_t  crow_vvel;      /* +0x1e4: vertical velocity, integrated into y each fly tick    */
    uint8_t  crow_mode;      /* +0x1d4: a FRESH RNG byte every root tick (@0x80112028 jal rng ->
                              * @0x8011204c sb v0,468 in the testbit-jal delay slot) — low6 = |vvel|,
                              * bit0x80 = climb/descend, %60/%3/&1/&0x32 timers/dirs (audit wf_827f186d crow #1) */
    uint16_t crow_dist;      /* +0x1dc: horizontal distance to the player (SquareRoot0). UNSIGNED:
                              * every original consumer is `lhu 476` + sltiu (@0x80115e30/@0x801126c0/
                              * @0x801126cc/@0x8011332c/@0x80113a94/@0x80114110/@0x8011352c/@0x8011468c;
                              * 2x sh, 8x lhu, ZERO lh in 0x80111a00-0x80116400) (audit wf_827f186d crow #14) */
    int16_t  crow_floor;     /* +0x1ba: floor-Y reference under the crow, refreshed EVERY root tick from
                              * room_coll FUN_8001c6e8(&pos, dim[3], a2=8, a3=0x400) @0x80112158-84
                              * (audit wf_827f186d crow #5) */
    uint8_t  crow_wall;      /* +0x1d1: SCA wall-pass hit flag, root post-pass @0x801121f8-218
                              * `0x8003b0a4(&+0x34, a1=dim[3]=200, a2=4)` -> sb 465 (audit wf_827f186d crow #B) */
    int16_t  crow_grav;      /* +0x1e8: vertical accel added to vvel each dive/fall tick       */
    int16_t  crow_speed;     /* +0x8c : horizontal move speed (per flight sub-state)           */
    uint8_t  crow_timer;     /* +0x1d5: sub-state countdown (climb-back / maneuver duration)   */
    uint8_t  crow_pturn;     /* +0x1d3: anim/timer completion flag (dive-decide pending-turn)  */
    uint8_t  crow_hs;        /* +0x1d8: flock handshake byte (dispatcher force-substate gate)  */
    uint8_t  crow_parity;    /* +0x1d2: per-tick LOS/parity toggle (wing-flap alternation)     */
    uint8_t  crow_atk_ctr;   /* +0x1d6: attack/peck attempt counter (gates re-commit, <3)      */
    uint8_t  crow_bank;      /* +0x1d7: proximity yaw-weave bank latch (0x80115e24)            */
    uint8_t  sca_mask;       /* +0x1d7 (Zombie-Vokabular): SCA-Kollisionsmaske — 4 aufrecht /
                              * 8 kriechend (Toggle-Writes @0x801050b4 sb 4 / @0x801050f4 sb 8;
                              * Kriech-Erstframe @0x8010374c sb 8). GENAU ZWEI Original-Leser:
                              * Zombie-Root @0x80100624 lbu 471(a0) + ZGirl-Root @0x8010aac8 —
                              * alle anderen Aufrufer von FUN_8003b0a4 uebergeben hart 4.
                              * Default 4 beim Spawn (@0x80100828). EIGENES Feld, NICHT
                              * crow_bank mitbenutzen (Dossier §4 Schritt 1): die Kraehe
                              * belegt +0x1d7 physisch anders und liest ihn nie als Maske. */
    uint8_t  xfer_dir;       /* +0x9F (Zombie-Vokabular): Richtungs-Latch des Kriechtor-TOGGLES
                              * (FUN_80104f80): Phase 0 setzt 0 (@0x8010502c sb zero,159(v0)),
                              * dann 1 NUR wenn (+0x09 & 0x80) CLEAR (@0x80105044 andi 0x80 /
                              * @0x80105048 bne / @0x80105050 sb a0,159(v1)) = HINWEG (stehender
                              * Zombie legt sich hin). Phase 1 reicht ihn als f314-Richtung a2
                              * durch (@0x8010506c, (s8)+0x9F); Phase 2 verzweigt darauf
                              * (@0x801050cc beq: 0 = Aufstehen fertig, 1 = Kriech-Commit 0x81).
                              * EIGENES Feld (Dossier §2): +0x9F ist sonst dog_aux9f/neck_speed-
                              * High — und NICHT ueber anim_flags Bit 0x80 routen (Port-Bit 0x80
                              * auf +0x1C4 ist port-erfunden, im Original nie getestet). */
    uint8_t  crow_diveflag;  /* +0x1da: dive-committed flag                                    */
    uint8_t  crow_armed;     /* +0x1db: attack-armed flag (LOS-gated)                          */
    int16_t  crow_yawrate;   /* +0x1de: per-state yaw-slew rate                                */
    int16_t  crow_accel;     /* +0x1e6: per-tick speed accel (dive ramp)                       */
    uint8_t  crow_contact;   /* +0x1d0: player-contact flag (strike/grab connect)             */
    int16_t  crow_struggle;  /* +0x9c : grab-hold struggle meter (drains, <0 = release)        */
    uint16_t crow_shadow_w;  /* +0xbc: Schatten-Halbbreite — ACTIVE-Tail ((y-floor)>>4)+400 min 100
                              * (@0x80115fa0-e4); Corpse-Pool-Grower +10/Tick (@0x8011589c-a0);
                              * GIB-Wipe = 1 (@0x80115938). (crow_death_pool.md §1.3/1.4) */
    uint16_t crow_shadow_h;  /* +0xbe: Schatten-Halbtiefe — dieselben Writer (@0x801158a8-ac etc.) */
    uint8_t  crow_tint;      /* Grauwert der Prim-Farbwoerter +0xc4/+0xec: ((y-floor)>>5)+128 min 32,
                              * v|v<<8|v<<16 Top-Byte erhalten (@0x80115fe8-6058); Wipe -> 0 (schwarz,
                              * &=0xff000000 @0x80115940-54) */
    uint8_t  crow_pool;      /* Farb-Zustand +0xc4/+0xec: 1 = (alt&0xff000000)|0x00ffff38 dunkelrote
                              * Lache (@0x80115884-c8), 0 = Grau-Tint/Wipe */
    uint8_t  crow_hide;      /* GIB-Scatter: Original zerlegt den Koerper in die 13 spawn-allozierten
                              * Bone-Parts (+0x188, Armierung @0x80114a50-aa4) und toetet sie nach 50
                              * Ticks (`sw zero,0(part)` @0x80114b78) -> Koerper weg; der Port
                              * verbirgt das Mesh ab GIB-step-0 (Part-Scatter-Mover nicht RE'd,
                              * crow_death_pool.md §4.2) */

    /* Dog (Cerberus, type 0x20) AI state — byte-true 0x8010d7f8 family (RE15_DOG_AI.md).
     * A ground chase/lunge/bite enemy; shares the zombie steering/collision + take_damage.
     * State 1 (ACTIVE) is a dual-dispatch brain on sub_state_1 (decision @0x80120f94 + act
     * @0x80120fd4): sub 0 idle / 1 turn / 2 chase / 3 attack-range / 4 lunge / 8 bite. */
    int16_t  dog_dist;       /* +0x1d4: cached dist-to-player (lh signed, SquareRoot0)          */
    uint16_t dog_flags;      /* +0x1d0: bit0 = sticky has-LOS                                   */
    uint8_t  dog_atk_cd;     /* +0x1d6: attack-cooldown timer (menace)                          */
    uint8_t  dog_pounce_cd;  /* +0x1e6: pounce-cooldown timer                                   */
    int16_t  dog_yawrate;    /* +0x1e2: per-approach yaw-slew rate (+0x7 micro-step = sub_state_3) */
    int16_t  dog_grab_armed; /* +0x1e4: grab-armed flag (set=1 by sub 5 windup @0x8010ed54; a bite
                              * then escalates to the eaten GRAB even when non-lethal). Cleared on
                              * grab entry @0x8010f4b4 / recover. */
    int16_t  dog_blocked_ctr;/* +0x1dc: frames-blocked counter — root FUN_8010dbcc tail
                              * @0x8010dd80-dda4: `+0x1da==0 -> +0x1dc=0 else +0x1dc++` (audit
                              * wf_827f186d dog #15). Feeds the chase wake gates (@0x8010e130/e170),
                              * the chase steer-vs-wander split (@0x8010e3bc slti 31) and the
                              * menace LOS-loss exit (@0x8010e92c slti 31). */
    uint16_t dog_reroute_dir;/* +0x1e8 = +0x90 & 1, latched at the reroute commit (@0x8010e2bc-c0) */
    uint16_t dog_reroute_sca;/* +0x1ea = *(u16*)(*(+0x1b4)+10) — the standing SCA cell's attr
                              * halfword, latched at the reroute commit (@0x8010e294-2a4); bits 2-3
                              * = the reroute-14 drop-level count (@0x80110764). */
    int8_t   dog_aux9f;      /* +0x9f byte: chase wander heading latch (sb @0x8010e3f4, read back
                              * sign-extended lb @0x8010e404) / menace no-LOS counter (@0x8010e9xx)
                              * / reroute-14 wall-byte latch (@0x80110658). */
    int16_t  dog_floor_y;    /* +0x1ba: the dog's floor Y (ground level). The original's +0x1ba is
                              * maintained by the engine floor probe; the port seeds it at INIT from
                              * the spawn Y and moves it with the reroute level hops (+-0x708). */

    /* ---- Maggot (type 0x27, EM027) — byte-true 0x80116db8 family work bytes (audit wf_827f186d).
     * All four are INIT-cleared/-seeded by FUN_80116f50 (@0x8011707c/8c/98-9c/ac). */
    uint8_t  mag_airborne;   /* +0x1e0: airborne latch — set 1 at leap LAUNCH (@0x80118ab0) / finisher
                              * entry (@0x80118e84); gates the state-2/3 ballistic settle (@0x8011af6c,
                              * @0x8011b70c); cleared on land (@0x80118c94) */
    uint8_t  mag_pin_cd;     /* +0x1e1: rear-up/pin re-attempt lockout — 0xff on pin connect
                              * (@0x8011adf4), 0x5a on miss (@0x8011af10); -1/brain tick (@0x8011741c-38);
                              * ==0 gates the sub-15 upgrade (@0x80117b28-30) */
    uint8_t  mag_boost;      /* +0x1e2 = 4 (INIT @0x80117098-9c, no other writer): leap impulse
                              * += *10 (@0x80118a64-80), vert impulse 600+*30 (@0x80118c00-18),
                              * collision-box index @0x80121368[.] (INIT @0x801171a0-c0) */
    uint8_t  mag_1e3;        /* +0x1e3: hurt-exit variant — !=0 routes the flinch exit to sub 9
                              * instead of the sub-7 leap (@0x8011b1c8-d8). ONLY writer in STAGE1.BIN
                              * is the INIT clear (@0x801170ac) -> always 0 in shipped play. */

    /* ---- Spider-Baby (type 0x26, EM026) — a STATIONARY web-spitter/ambush (RE15_SPIDER_AI.md).
     * Emerges vertically from its spawn point (spider_phase < 13 = intangible), then solid + a -2
     * contact stagger. STATE[1] arms collision hit-codes + telegraph "web" fx; killable (hurt/death). */
    uint8_t  spider_phase;   /* +0x1d0: emerge/attack budget (0..12 emerging-intangible; strike init 40/44) */
    int16_t  spider_timer;   /* +0x1d4: inter-strike wind-up timer (rng&0x3f + 16 = [16,79]) */
    int16_t  spider_home_x;  /* +0x1d8: cached spawn X (u16) */
    int16_t  spider_home_y;  /* +0x1d6: cached spawn Y (u16) — the vertical emerge anchor */
    int16_t  spider_home_z;  /* +0x1da: cached spawn Z (u16) */

    /* ---- G-Birkin BOSS (type 0x30/0x36, EM030/EM036) — byte-true 0x80116230 family (STAGE3.BIN).
     * The shared boss root snapshots/decrements these each root tick; the ACT/HURT/DEATH sub-machines
     * read them. Dedicated port fields for the PSX offsets that overlap the zombie/dog work bytes above
     * (same pattern as the dog_ / crow_ / mag_ / spider_ groups). hit_stun already carries +0x1dc. */
    uint8_t  birkin_flags;   /* +0x1dd: bit0(0x1)=run-off active (root steers to override pt), bit2(0x4)=
                              * heave toggle (wounded, gates DECIDE mutate), bit3(0x8)=mutating (revive-guard) */
    uint8_t  birkin_hurt_cd; /* +0x1de: HURT lockout countdown (set 9 on flinch @0x8011a0c4; at 0 clears +0x93&1) */
    uint8_t  birkin_atk_cd;  /* +0x1df: lunge/tackle cooldown (sub-3 done 0x3c/abort 0x1e, sub-7 done 0xa) */
    uint8_t  birkin_saved_state, birkin_saved_sub, birkin_saved_ph2, birkin_saved_ph3; /* +0x1d8 word snapshot
                              * of +0x4 each root tick (@0x80116698); HURT/revive resume +0x4=*(+0x1d8) */
    int16_t  birkin_runoff_x, birkin_runoff_z; /* +0x1d4/+0x1d6: death-morph run-off point (-22000,-12000) */
    uint8_t  birkin_pause;   /* +0x9e: ACT[1] two-phase close-in pause countdown (0x5a) */
    uint8_t  birkin_grab;    /* PORT-LOCAL persistent latch for the GLOBAL grab channel DAT_800aca58==5
                              * (aca58 is engine-global, and run_all clears s_player_grabbed each frame): set on
                              * a claw connect, re-asserts s_player_grabbed each tick so the HUB DECIDE grab-tail
                              * (which gates on aca58==5) is reachable; cleared on throw/HURT/death. */

    /* ---- Cockroach (type 0x29, EM029) — byte-true 0x80110b00 family (STAGE3.BIN; audit wf_efd92a2c
     * cockroach). Dedicated fields for the PSX offsets that overlap the shared work bytes above. The
     * roach reuses dog_atk_cd(+0x1dc attack cooldown), crow_speed(+0x8c scurry speed), ai_timer(+0x9c idle
     * timer), sub_state_2(+0x6 phase byte) and sub_state_3(+0x7 sighted/blind flag). */
    uint8_t  roach_los;      /* +0x1d0 bit0: LATCHED LOS — only a 0/1 probe result updates it (@0x8011105c) */
    uint8_t  roach_air;      /* +0x1e0: airborne latch (leap/flight subs 7/9) (@0x8011250c) */
    uint8_t  roach_beh2;     /* +0x1e2: behavior&2 -> HURT-exit sub 5 (INIT clear @0x80110e2x; set by beh&2) */
    uint8_t  roach_esc;      /* +0x1e3: default 1 (cleared by beh&4) -> HURT-exit sub 9 fly-away (@0x80110e18) */
    int16_t  roach_fade;     /* +0x9e: corpse fade counter, seeded 0x5a (@0x80115a98) */

    /* ---- Adult Spider (type 0x25, STAGE2) — bisher der EINZIGE Typ OHNE eigene Feldgruppe.
     * Sein LOS-Latch lag deshalb auf `member_0b`, das im ORIGINAL aber den AOT-Slot-Stempel
     * traegt (entity+0x0b, `sb v0,11(s1)` @0x80042f5c/@0x80042fc4). Beides auf einem Feld heisst:
     * sobald der AOT-Stempel portiert wird, zerstoert er die Sichtlinie des Spiders. Deshalb hier
     * ein eigenes Feld — gleiches Muster wie dog_/crow_/mag_/spider_/roach_. Reiner Speicher-
     * Umzug, die Latch-Semantik (`nur ein 0/1-Sondenergebnis aktualisiert`) bleibt unveraendert. */
    uint8_t  aspider_los;    /* +0x1d0 bit0: LATCHED LOS (Verdikt-Tick @0x80110e70-bc,
                              * HURT-Recover @0x80113f04) */
} re15_actor_t;

extern re15_actor_t g_actors[RE15_ACTOR_MAX];
extern uint8_t      g_actor_count;   /* highest active slot + 1 — for HUD */

/* Phase 4.5.13 O5 (2026-05-23): Plc_motion handler @ PSX.EXE 0x80041b90
 * does NOT reset anim_frame (+0x95) — it only writes motion_no (+0x94)
 * and clears state sub-flags (+0x06/+0x07/+0x1c8/+0x1ca). The anim_frame
 * wraps via the keyframe processor's `if (frame_count <= bVar3) +0x95=0`
 * at clip end. Phase-locked transitions are intentional. */
static inline void re15_actor_set_motion(re15_actor_t *a, int16_t m)
{
    /* T-round: on every Plc_motion, original engine clears
     * actor+0x06/0x07/0x95 (state + frame_index). Reset anim_frame to 0
     * so the new clip starts from frame 0 (not at whatever residual count
     * from the prior clip). Required for FREEZE-default renderer to
     * correctly distinguish "currently playing" (cur < frame_count)
     * vs "ended, freeze on last" (cur >= frame_count). */
    if (a->motion != m) {
        a->anim_freeze = 0;
        a->anim_frame  = 0;
        a->anim_flags  = 0;       /* clear sticky flags from prior clip */
        a->motion_init_delay = 1; /* BD-round: PSX state=4→state=1 1-tick FSM */
        a->anim_frac   = 7;       /* seed FRAC crossfade (FUN_8001f3bc +0x8f) — blend
                                   * from the prior rendered pose into the new clip. NEEDED:
                                   * cutscene clips (e.g. room1150 clip 10) have frame-0 =
                                   * full kneel, so the kneel-DOWN is produced ENTIRELY by
                                   * this blend (no in-clip bend). */
    }
    a->motion = m;
}

void re15_actor_init(void);

/* Allocate a slot for a new actor of `type`. Returns slot index, -1
 * if all slots busy. */
int  re15_actor_alloc(uint8_t type);

/* Reset slot. */
void re15_actor_free(int slot);

/* Member access — used by SCD Member_set / Member_cmp.
 * Returns the property as s32 (sign-extending smaller types). Writes
 * truncate to the property's native width. Unknown member_id → 0 / no-op. */
int32_t re15_actor_get_member(int slot, uint8_t member_id);
void    re15_actor_set_member(int slot, uint8_t member_id, int32_t value);

/* Phase 4.5.13-RE2 F3: per-frame walker integrator (port of RE2 FUN_8001ed9c).
 * `step_walk` advances ONE actor's locomotion; `step_all_walkers` drains
 * every active walker. Call once per SCD tick from main.c after scd_vm_tick. */
void re15_actor_step_walk(re15_actor_t *a);
void re15_actor_step_all_walkers(void);

/* Yaw (Q12, 0 = +Z) from a world XZ delta. `re15_atan2_q12(dz,dx) - 1024` gives
 * the mesh rot_y (0 = +X) that faces direction (dx,dz). */
int16_t re15_atan2_q12(int32_t dz, int32_t dx);

/* Per-frame Plc_neck head-look FSM (byte-true FUN_80024e40 damped look-at) is computed
 * inside re15_skel_compute_pose at the head bone (bone 8), where the root bone matrix is
 * available so the look angle is taken in the correct root-local frame. No standalone
 * call — the old body-relative re15_neck_update was retired to avoid double-slewing. */

/* Phase 4.5.13-RE2 I5: keyframe-driven actor translation. Mirrors RE1.5
 * engine's FUN_8001F3BC integrator that reads root delta from bytes 0..5
 * of each keyframe and adds (yaw-rotated) to actor world pos. */
#include "re15_emd.h"
void re15_actor_apply_root_motion(re15_actor_t *a,
                                  const re15_emd_skeleton_t *skel,
                                  const re15_emd_animation_t *anim);

/* ---- RE2-Gore: die ANZEIGE-BRUECKE des Modellblocks +0x198 (PORT-OPTION, nur RE2-Flavor) ----
 * Zwilling des Part-Draw-Walks FUN_80027160 @0x80027160. Vollstaendige Belegkette im
 * Kopfkommentar in engine/src/enemy_ai_re2_zombie.c.
 *   re15_re2z_gore_active()  1, wenn RE2-Flavor + RE2-Zombie-Typ + INIT-Seed. Im RE1.5-Pfad
 *                            IMMER 0 — der Renderer darf ohne diesen Gate nichts aendern.
 *   re15_re2z_gore_resolve() fuellt out_draw[0..n-1] (0 = Part nicht zeichnen, Bit-0-Test
 *                            @0x8002737C/@0x800273C4 + Eltern-Kaskade @0x80027480-94) und
 *                            out_tint[0..n-1] (Farbwort +0x70, LOW BYTE = R, neutral
 *                            0x00808080) und out_mesh[0..n-1] (MD1-Objektindex des Parts,
 *                            normal == i, nach dem Stumpf-Tausch @0x8010531C-50 == 15).
 *                            Rueckgabe 0 = Gore inaktiv, out_* unberuehrt. */
int re15_re2z_gore_active(const re15_actor_t *e);
int re15_re2z_gore_resolve(const re15_actor_t *e, const int8_t *bone_parent, int n,
                           uint8_t *out_draw, uint32_t *out_tint, uint8_t *out_mesh);

/* ---- DAS FREIFLIEGENDE TEIL (Welle G) -------------------------------------------------------
 * re15_re2z_gore_part_matrix() ist der Zwilling des Zweigs, den FUN_80027434 fuer ein Teil mit
 * Bit 0x40 nimmt:
 *   80027498: andi v0,s3,0x40
 *   8002749c: bne  v0,zero,0x800275e4      ; Eltern-Verkettung UEBERSPRINGEN, Matrix steht schon
 *   80027694: andi v0,s3,0x20
 *   800276a0: jal  0x80028ad8              ; Wurf-Physik mit Aufschlag (Bein 0x1062)
 *   80027b98: jal  0x80028dac              ; Drift-Physik mit Lebensdauer (Arm 0x4A)
 * Rueckgabe 1 = das Teil hat eine EIGENE Matrix; rot[9] (Q12, zeilenweise) und trans[3] werden
 * dann UEBERSCHRIEBEN. Rueckgabe 0 = normale Skelett-Pose, rot/trans bleiben unberuehrt.
 * `frame` ist der Frame-Zaehler: der Physikschritt laeuft genau EINMAL je Frame und Aktor,
 * egal wie oft der Renderer die Funktion aufruft (Schatten-Pass, zweiter Zeichen-Pass).
 * Im RE1.5-Flavor liefert die Funktion IMMER 0 (dasselbe Dreifach-Gate wie _gore_active). */
int re15_re2z_gore_part_matrix(re15_actor_t *e, int part, uint32_t frame,
                               int32_t rot[9], int32_t trans[3]);

/* ---- PORT-OPTION 2026-08-20: der Zerleger AUCH im RE1.5-Modus (Nutzer-Auftrag) --------------
 * ⛔ Bewusste Abweichung vom RE1.5-Original, per Schalter re15_re15_re2z_import()
 * (re15_ai_flavor.h) — dort steht die vollstaendige Begruendung + der Hebel zurueck.
 * re15_re15_re2z_gore_hit() ist der Einstieg aus dem RE1.5-TREFFERPFAD (re15_damage.c:
 * re15_player_weapon_fire / re15_enemy_take_damage). Er fahrt GENAU die zwei Schritte, die im
 * RE2-Modus vor dem Reaktions-Dispatch stehen: den Applier-Stempel (+0x1D0-Richtung
 * @0x80041A0C-84 und die Zonen-Reserve +0x151/+0x152/+0x153 @0x80041900-9C) und den Zerleger
 * @0x80105288-3D8 — und schreibt dabei AUSSCHLIESSLICH re2z_*-Felder, nie ein RE1.5-Zustandsfeld.
 *   row_src == 0 -> `row_id` ist eine RE1.5-WAFFEN-Id, == 1 -> eine ATTACKEN-Id.
 *   `pl` ist der Treffer-URSPRUNG (Peilung des +0x1D0-Stempels).
 * No-op ausserhalb des RE1.5-Modus, ohne die Option, fuer Nicht-Zombies und bei toedlichem
 * Treffer (der Zerleger sitzt im HURT, das DEATH hat seinen eigenen Gore-Zweig @0x80108250). */
void re15_re15_re2z_gore_hit(re15_actor_t *e, const re15_actor_t *pl,
                             int row_src, unsigned row_id);

#endif /* RE15_ACTOR_H */
