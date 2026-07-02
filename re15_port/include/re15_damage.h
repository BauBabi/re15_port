/*
 * RE1.5 Rebuilt — Player damage resolution (#13, 2026-06-28).
 *
 * Byte-true port of the PLAYER branch of FUN_80012d60 (ghidra1_V2.txt:77607-77814,
 * decompile RE_15_Quellcode_V2/FUN_80012d60.c) — the unified hit/damage resolver.
 * The original tests one attack hitbox against every active enemy (DAT_800acc2c
 * array, stride 0x1f4) AND the player block (DAT_800aca54), applying dmg_table
 * damage to whatever it overlaps. This module ports the PLAYER half: applying a
 * resolved hit to the player's HP + hurt/death state + bleed/poison status.
 *
 * The in-game TRIGGER — the enemy attack-action FSM (dispatch table @0x80071da4,
 * e.g. FUN_80017fa4 = the zombie lunge that calls FUN_80012d60(0x1f4,hitbox,0))
 * and the actor-vs-actor hitbox test FUN_8002b5d0 — is NOT ported yet (the port
 * has no enemy AI / actor hitbox), so nothing calls this in-game today. It is the
 * byte-true, unit-tested core that the enemy-AI pass will wire in. See HANDOVER.md.
 */
#ifndef RE15_DAMAGE_H
#define RE15_DAMAGE_H

#include <stdint.h>
#include "re15_actor.h"

/* Attack-type → damage (s16). Byte-true DAT_8006f418 (ghidra1_V2.txt:223455-223478,
 * 11×s16 LE; read @80012e54 / @80012ff4). Indexed by attack_type & 0xff.
 *   [0]=10  (zombie bite/grab, the FUN_80017fa4 lunge)   [6]=100
 *   [1]=20                                               [7]=200
 *   [2..4]=1000 (instakill class)                        [8]=300
 *   [5]=50                                               [9]=1000  [10]=0          */
extern const int16_t re15_damage_table[11];

/* Attack-type → hit-reaction code. Byte-true DAT_8006f430 (ghidra1_V2.txt:223480-
 * 223490; read @80012fe8). Written to the TARGET's reaction sub-state in the ENEMY
 * branch (enemy +0x5); the player branch uses the front/back selector instead.
 * Exported for the (deferred) enemy branch + documentation. */
extern const uint8_t re15_react_table[11];

/* Apply a resolved hit to the player = the player branch of FUN_80012d60
 * (@80012e18..80012f04). HP -= re15_damage_table[attack_type]; sets the hurt state
 * (+0x4=2), the front/back hurt sub-state (+0x5), clears +0x6, sets the hit-once
 * guard (+0x93 bit0x1); on a type<2 hit rolls the bleed/poison status (+0x98 bit0x2);
 * a signed HP < 0 promotes to the death state (+0x4=3). (src_x,src_z) = the attacking
 * hitbox origin, used by the front/back selector (FUN_8001a7a8).
 * Returns 1 if the hit landed (guard was clear), 0 if the player was already hit
 * this attack window (guard set → no-op, byte-true @80012e30). */
int re15_player_take_damage(re15_actor_t *p, uint8_t attack_type,
                            int32_t src_x, int32_t src_z);

/* Apply a resolved hit to an ENEMY actor = the enemy branch of FUN_80012d60
 * (@80012f08-80013034). Same damage table; reaction sub-state = re15_react_table
 * (the hit clip), no bleed roll (player-only). The resolver loop owns the per-frame
 * collision-confirm / self-exclusion / state-mask bits around it. Returns 1 if the
 * hit landed, 0 if the enemy was already hit this attack window. */
int re15_enemy_take_damage(re15_actor_t *e, uint8_t attack_type);

/* Per-frame zombie gore effect (FUN_80106a44 @0x80106a98): if +0x93 & 2 (the 2nd-hit bit set
 * by re15_enemy_take_damage) is set, spawn the gore effect at the zombie and clear the bit.
 * Called per live zombie in re15_enemy_ai_run_all. */
void re15_enemy_gore_tick(re15_actor_t *e);

/* Zombie gore-SETUP behaviour (FUN_80106edc @0x80106edc, dispatch index sub_state_1==0x58): on
 * entry (sub_state_3==0) a 50% RNG spawns the gore burst (2x effect-id 0 + 1x effect-id 5 = the
 * VISIBLE ROOM1140 gore) at the zombie + sets sub_state_3=1. Called per live zombie in run_all. */
void re15_enemy_gore_setup(re15_actor_t *e);

/* Zombie HURT hit-effect (master hurt dispatch FUN_80105a8c -> FUN_80105b7c phase 0): on the hurt
 * entry (state==2, sub_state_3==0) spawn effect-id 0 (the universal hit/blood fx from the global
 * CORE00.ESP bank) at the zombie + advance the phase. Called per live zombie in run_all. */
void re15_enemy_hurt_fx(re15_actor_t *e);

/* Zombie DEATH gore (death sub-FSM FUN_80107cb0): effect-id 0 blood burst at death-start and again
 * at anim_frame 35. Called from re15_enemy_ai_live_death at both points. */
void re15_enemy_death_fx(re15_actor_t *e);

/* Player DEATH state (Phase 8.10) — the byte-true core of the player death FSM. The player is dead
 * when HP < 0 (FUN_80012d60 @0x80012ee8; the grab drains HP to the same HP<0). The original then runs
 * the death-sequence command handler (player command 0x800aca58 dispatched @0x80073f90[state]: [3] =
 * generic death @0x800366bc, [7] = the GRABBED death @0x8003694c — the combat-death save shows 7),
 * which freezes input and runs a fade + death camera on a 120-frame timer (@0x8003694c INIT:
 * DAT_800acaf2 = 0x78), then game over. game_step reads re15_player_is_dead() to FREEZE the player
 * (skip the tick) while dead and advances re15_player_death_tick(). The fade/camera + the eaten-anim
 * FSM (@0x8010a28c) + the game-over screen are the DEFERRED presentation (no port fade/game-over). */
int  re15_player_is_dead(void);      /* 1 iff the player HP < 0 (death). */
void re15_player_death_reset(void);  /* clear the death-sequence timer (new game / continue / test). */
int  re15_player_death_tick(void);   /* advance the death sequence one frame; returns frames left
                                      * (120..0; 0 = sequence done -> game over deferred; -1 = alive). */

/* Player WEAPON SHOT (Phase 8.10, two-sided combat) — the byte-true core of FUN_80011f50 (a SEPARATE
 * resolver from the enemy FUN_80012d60). Auto-aims the nearest live zombie in front of the player
 * within the per-weapon reach (byte-true table @0x8006e5a0), applies the per-weapon byte-true damage
 * (table @0x8006e0d0, the zombie row; a DISTINCT table from re15_damage_table) -> the enemy enters
 * state 2 (HURT) / 3 (DEATH) + reaction clip (+0x5=weapon_id), with the crit/headshot rule (weapon 7,
 * or weapon 8 within 3000 -> instant kill). The damaged zombie then runs re15_enemy_ai_live_hurt/death.
 * weapon_id 0..21 (DAT_800aca5d). Returns the hit enemy slot+1 (0 = no target in cone/reach). The exact
 * per-weapon line-vs-box cone tester + the aim/fire input FSM (@0x80035810) + the hit-direction clip
 * are the deferred refinements. */
int  re15_player_weapon_fire(int weapon_id);

/* Clear the per-attack hit-once guard (+0x93 bit0x1). The original clears it when the
 * attacker's hitbox deactivates so the NEXT attack can land; that clear-trigger lives
 * in the deferred enemy-attack FSM. Exposed so the future wiring (and the unit test)
 * can re-arm the player for a fresh hit. */
void re15_player_clear_hit_guard(re15_actor_t *p);

/* Attack-hitbox vs actor collision test = the CIRCULAR path of FUN_8002b5d0
 * (ghidra1_V2.txt:118005-118130) — the per-target geometry test the resolver loop
 * runs. 1 if an attack hitbox (origin atk_x/y/z, radius atk_radius) overlaps a
 * circular hitbox of `radius`/`height` centred at (cx,cy,cz) = target world pos +
 * the local offset. Byte-true to the symmetric-hitbox branch @8002b6fc..end; the
 * angular-sector branch (radius_min!=radius_max, needs the BIOS ratan2) is DEFERRED.
 * Exposed for the resolver wrapper + the unit test. */
int re15_hitbox_overlap(int32_t cx, int32_t cy, int32_t cz,
                        int32_t radius, int32_t height,
                        int32_t atk_x, int32_t atk_y, int32_t atk_z,
                        int32_t atk_radius);

/* Attack hitbox descriptor = FUN_80012d60 param_2 (int[3] world X/Y/Z origin)
 * + param_1 (the attack radius). The zombie lunge (FUN_80017fa4) builds it from the
 * attacker's pos (+0x28/+0x2a/+0x2c via FUN_8001c6e8) with radius 0x1f4 (500). */
typedef struct {
    int32_t  x, y, z;     /* attack hitbox origin (world) */
    uint16_t radius;      /* attack radius */
} re15_attack_box_t;

/* FUN_8002b5d0 wrapper: test `atk` against `target`'s hitbox (its hit_radius_min/
 * max, hit_height and hit_offset fields), centred at the target's world pos + the
 * local offset. 1 on overlap. Circular path byte-true; the sector case
 * (radius_min != radius_max) is DEFERRED. */
int re15_hitbox_test(const re15_actor_t *target, const re15_attack_box_t *atk);

/* The FUN_80012d60 resolver LOOP. Tests `atk` against the player block (g_actors[0])
 * and every active enemy (g_actors[1..]); applies re15_damage_table[attack_type]
 * damage once per attack window — player via re15_player_take_damage, enemies via
 * re15_enemy_take_damage with the per-attack collision bits. attacker_slot is
 * self-excluded (GATE A @80012f40); pass -1 if none. Returns the number of targets
 * the hitbox engaged this call (player overlap + each gated-in enemy), matching the
 * original's cVar9 (engagement count, not only applied damage). */
int re15_resolve_attack(const re15_attack_box_t *atk, uint8_t attack_type,
                        int attacker_slot);

/* FUN_80017fa4 (decompile RE_15_Quellcode_V2/FUN_80017fa4.c) — the enemy attack-action's
 * DAMAGE trigger. Authoritative: it resolves FUN_80012d60(500, &point, 0) — radius 500
 * (0x1f4), attack_type 0 (= 10 dmg) — at the lunge's forward attack-point. That point is
 * the work-struct's GTE world position [0x14/0x15/0x16] (computed by the action driver
 * FUN_80019e20 via RotMatrix/ApplyMatrix from the model-instance pose); the port maps it
 * to atk_pt_* = the attack-bone world pos (faithful-line skeleton mapping). attacker_slot
 * is self-excluded. Returns the engagement count (>0 = the bite connected).
 * DEFERRED (model-pool / other subsystems): the FUN_8001c6e8 room-collision OR-branch, the
 * on-hit SE (FUN_800199d4/FUN_80045024), and the GTE attack-point computation itself. The
 * on-hit work-struct reset (+0x6e=0xd) is modelled by the lunge driver below. */
int re15_enemy_attack(int attacker_slot);

/* Enemy lunge attack-action driver = LAB_80017eb0 (setup) + LAB_80017f50 (per-frame tick),
 * which the action driver FUN_80019e20 runs for the lunging enemy's model instance. The
 * lunge has a 0x20-frame ACTIVE window (work-struct +0xe, set to 0x20 by LAB_80017eb0);
 * each frame LAB_80017f50 decrements it and fires the hitbox FUN_80017fa4 (re15_enemy_attack)
 * — so the bite can connect on any of the ~32 active frames, and on a connect FUN_80017fa4
 * resets the action (+0x6e=0xd) so it bites at most once per lunge.
 *   re15_enemy_lunge_begin  — start the active window (lunge_frames = 0x20).
 *   re15_enemy_lunge_tick   — one frame: fire the hitbox while the window is open; returns
 *                             the engagement count this frame (0 when not lunging). Ends the
 *                             window on a connect (the +0x6e=0xd reset) or when it runs out.
 * The lunge MOVEMENT (the work-struct velocity/pos advance + the GTE attack-point) is the
 * port's skeleton job (faithful-line); the exact end-of-window keyframe transition
 * (FUN_800174e4) is deferred. The byte-true core here is the active-frame window + the
 * per-frame fire + the bite-once-per-lunge reset.
 *
 * The lunge is action 0x16/0x17/0x18 in the action table PTR_LAB_80071d40 ([0x16]=LAB_
 * 80017eb0 setup, [0x17]=LAB_80017ed8 advance/movement, [0x18]=LAB_80017f50 fire). The
 * action driver FUN_80019e20 runs the model-instance's +0x0 slot (movement 0x16->0x17) and
 * its +0x2 slot (fire 0x18) each frame. The lunge BEGIN (instance +0x0 := 0x16) is set by
 * an anim-keyframe action event (FUN_80019e20's +0x37/+0x3c keyframe stepping) — i.e. it
 * fires when the approach/attack animation reaches its lunge frame. That begin-trigger is
 * part of the deferred model-pool/anim layer; in the port the AI/anim layer calls
 * re15_enemy_lunge_begin at the equivalent point. */
void re15_enemy_lunge_begin(int attacker_slot);
int  re15_enemy_lunge_tick(int attacker_slot);

/* Faithful-line skeleton mapping of the lunge ATTACK-POINT (Phase 8.1). The original's damage
 * origin is NOT a single bone/offset: it is the live GTE WORLD position of the attacker's posed
 * body-part frames, recomputed every tick by the EXE action driver FUN_80019e20 into the work
 * record's +0x28/+0x2a/+0x2c (read by FUN_80017fa4 as work[0x14/0x15/0x16]). FUN_80019e20 builds
 * it as ApplyMatrix(M1, vecA) + (+0x60) + ApplyMatrix(M_pose, vecB), where M_pose is refreshed
 * each tick from the per-body-part frame ptr +0x74 = model_base(+0x188) + bodypart*0xac + 0x40 —
 * so the point tracks that body-part's posed position. The lunge actually drives EIGHT body-part
 * hitboxes (model-pool LUT DAT_8011f7a4 = {00,07,08,0e,02,04,09,0d}; the head/upper-body parts
 * 0x0e/0x08 dominate), each firing FUN_80017fa4(500,&pt,0). (The overlay FUN_80104178 drives the
 * lunge MOTION + gore on the entity block, NOT the damage point — RE15_FUN_CATALOG 73/77
 * conflated this; RE'd byte-true 2026-06-29, FUN_80019e20.c/FUN_80019d50.c/FUN_8010b274.c.)
 *
 * That GTE/model-pool path is DEFERRED by design. This function is the faithful-line stand-in:
 * it poses the enemy's skeleton at `keyframe` (as a QUERY — the crossfade blend state is
 * preserved), takes `attack_bone`'s world position via re15_skel_bone_to_world (the EXACT
 * transform the render loop applies), and stores it into atk_pt_* (int16, matching the original's
 * short store). The MECHANISM (pose -> world) is exact; choosing ONE representative attack bone
 * (vs the original's 8 body-part frames) is a documented faithful-line SIMPLIFICATION, so the
 * bone is a parameter, not a byte-true constant hardcoded here. The model-pool-index -> EMD-bone
 * resolution (which em10/16/47 bone == head/jaw) is the open integration-step item (needs the
 * model-pool build order or a mid-lunge room1140 savestate). Inert until a lunge fires (atk_pt
 * is only read by re15_enemy_attack while lunge_frames>0). No-op on bad slot/skeleton/bone. */
void re15_enemy_update_attack_point(int slot, const re15_emd_skeleton_t *skel,
                                    int keyframe, int attack_bone);

/* Wire the byte-true hitbox dims (+0x78 struct radius_min/height/radius_max + the
 * +0x7c local offset) onto an actor. Player = the fixed EXE-static struct @0x80073e94
 * (VERIFIED in PSX.EXE @0x64694: radius 450 / height 1530, circular). Enemies = the
 * per-type values read from room savestates (type 0x47 STAGE1 zombie = 450/1530;
 * 0x16 = 400/1440; 0x29 = 1100/1080; all circular, offset (0,-height,0)); unknown
 * types get no hitbox. Call at player init / enemy spawn. */
void re15_player_apply_hitbox(re15_actor_t *p);
void re15_enemy_apply_hitbox(re15_actor_t *a, uint8_t type);

/* STAGE1 zombie AI byte-true primitives (handler FUN_8011d6d4). re15_enemy_player_dist
 * = the cached distance SquareRoot0(DX²+DZ²) (16-bit-wrapped deltas). re15_ai_arc_test
 * = FUN_8001a9cc: 0 if the player is inside the ±`cone` front arc of the zombie, else
 * ±cone. The full attack FSM (which state is the lunge, the GTE attack-point) is NOT
 * yet mapped — these are the verified inputs, see re15_damage.c / the memory. */
int32_t re15_enemy_player_dist(const re15_actor_t *e, const re15_actor_t *player);
int     re15_ai_arc_test(const re15_actor_t *e, int32_t px, int32_t pz, int cone);

/* The zombie's byte-true attack-commit condition (→ entity state 0x701), confirmed
 * across FUN_80101b64/FUN_80101de4/FUN_80102058: dist<2000 && re15_ai_arc_test(0x2c8)!=0.
 * Returns 1 when the zombie should commit the attack. The 0x701→FUN_80017fa4 lunge
 * chain + the arc convention are still open (see re15_damage.c). */
int     re15_enemy_should_attack(const re15_actor_t *e, const re15_actor_t *player);

/* Seed the bleed/poison RNG (deterministic tests + reproducible future runs). The PSX
 * source (FUN_8001af20 over a leftover register) is non-reproducible by construction;
 * see re15_damage.c for how the EXACT hash + EXACT 1/4 probability are preserved. */
void re15_damage_seed_rng(uint32_t seed);

/* The engine's shared pseudo-random draw = FUN_8001af20 (the (x + (x>>7)) & 0xff hash).
 * The original is ONE global RNG used everywhere — the bleed roll here AND the enemy-AI
 * decision draws (FUN_80101b64 `&7`, FUN_80101de4 `&3`). Exposed so the AI module draws
 * from the SAME sequence (faithful: one RNG), seeded by re15_damage_seed_rng. Returns
 * the low 8 bits of the hash, matching the original's byte result. */
uint8_t re15_engine_rand8(void);

#endif /* RE15_DAMAGE_H */
