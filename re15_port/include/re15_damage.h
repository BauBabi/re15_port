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

/* Seed the bleed/poison RNG (deterministic tests + reproducible future runs). The PSX
 * source (FUN_8001af20 over a leftover register) is non-reproducible by construction;
 * see re15_damage.c for how the EXACT hash + EXACT 1/4 probability are preserved. */
void re15_damage_seed_rng(uint32_t seed);

#endif /* RE15_DAMAGE_H */
