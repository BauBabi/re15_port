/*
 * RE1.5 Rebuilt — RESIDENT EVIL 2 zombie brain (PORT OPTION, selectable in OPTIONS).
 *
 * Everything here is RE'd from the RE2 (Leon, retail) enemy overlay:
 *     info/re2leon/COMMON/BIN/EMZ0.BIN, loaded RAW @0x80100000
 *     disassemble:  .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMZ0.BIN
 * RE2 state table @0x8010C830 (self-verified):
 *     [0] 0x8010065C INIT   [1] 0x8010114C ACTIVE  [2] 0x80104F40 HURT   [3] 0x80108250 DEATH
 *     [4] 0x80065C88 (EXE)  [5][6] NULL            [7] 0x8010A440 CORPSE (12 substates)
 *     [8] 0x80109CFC        (RE2-EXCLUSIVE, 11 substates — RE1.5's table has no [8])
 *
 * ⚠️ WHAT "RE2 AI" REALLY IS (the overnight RE refuted the folklore, so do not re-add it):
 *   - There is NO line-of-sight / raycast for zombies. A full jal-scan over all 13267 overlay words
 *     finds ZERO calls to 0x80050858 / 0x80065518 / 0x80065890 — those belong to the em/NPC family.
 *   - There is NO crowd/swarm intelligence in the zombie: ACTIVE dispatches ONCE through
 *     (+0x10E & 0x3F) into 0x8010C854, whose 14 entries alternate on bit0 only -> exactly two
 *     variants (0x8010118C upright / 0x80101210 crawling), not eleven "modes".
 *   - The neighbour word +0x1F4 is READ by the zombie (@0x801017A4) but never WRITTEN by it; the
 *     producer is FUN_80065518 in the NPC family -> a dead branch in zombie-only rooms.
 * The real difference is LOCOMOTION, attack arbitration and hit reaction. This file starts with the
 * locomotion gate, which is the one that actually reads as "it stops staggering and comes at you".
 */

#include <stdint.h>
#include <stdlib.h>   /* getenv (headless flavor override) */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

/* ---- the flavor switch itself ------------------------------------------------------------- */

static re15_ai_flavor_t s_flavor = RE15_AI_FLAVOR_RE15;   /* byte-true default */
static int s_flavor_env_read = 0;

re15_ai_flavor_t re15_ai_flavor(void)
{
    /* RE15_AI_FLAVOR=re2 lets the headless harness select the brain without the menu. */
    if (!s_flavor_env_read) {
        const char *v = getenv("RE15_AI_FLAVOR");
        s_flavor_env_read = 1;
        if (v && (v[0] == '2' || v[0] == 'r' || v[0] == 'R')) s_flavor = RE15_AI_FLAVOR_RE2;
    }
    return s_flavor;
}
void             re15_ai_flavor_set(re15_ai_flavor_t f) { s_flavor = (f == RE15_AI_FLAVOR_RE2)
                                                                     ? RE15_AI_FLAVOR_RE2
                                                                     : RE15_AI_FLAVOR_RE15; }

/* The RE1.5 zombie family. RE2 folds its whole 0x10..0x1F kind range onto one group (@0x8001B738),
 * so this is the port's equivalent set — and ONLY these ever leave the RE1.5 brain. */
int re15_re2z_owns_type(unsigned type)
{
    return type == 0x10 || type == 0x11 || type == 0x12
        || type == 0x13 || type == 0x16 || type == 0x18;
}

/* ---- W1: the RE2 WALK TURN GATE ------------------------------------------------------------
 *
 * Byte-true from the walk substate (self-disassembled 2026-07-29):
 *   80101bac: lw    v0,496(s1)        ; +0x1F0 = distance to the player (the RE2 EXE fills this in
 *                                     ;          before the dispatch, @0x800265A4-E0)
 *   80101bb4: sltiu v0,v0,0x1389      ; dist < 5001 ?
 *   80101bb8: bne   v0,zero,0x80101bec
 *   80101bbc: addiu v1,zero,8         ;   YES -> rate = +8, MONOTONE toward the steer point
 *   80101bc0: lb    v0,363(s1)        ;   NO  -> gait row +0x16B
 *   80101bd4: lhu   v0,-14044(at)     ;          tbl16[row] @0x8010C924 (32 u16, self-dumped)
 *   80101bdc: srl   v0,v0,15          ;          bit15 of the row
 *   80101be0: sll   v0,v0,4           ;          *16
 *   80101be8: subu  v1,v1,v0          ;          rate = 8 - (bit15 ? 16 : 0)  ->  +8 or -8  = WEAVE
 *   80101bf8: lh    a1,452(s1)        ; a1 = steer point X (+0x1C4)
 *   80101bfc: lh    a2,454(s1)        ; a2 = steer point Z (+0x1C6)
 *   80101c00: jal   0x80015558        ; turn toward it at rate v1
 * and a SECOND, additive turn when close:
 *   80101c94: sltiu v0,v0,0xbb8       ; dist < 3000 ?
 *   80101ca8: jal   0x80015558
 *   80101cac: addiu a3,zero,16        ;   -> an EXTRA rate-16 turn toward the same point
 *
 * So: far away the RE2 zombie WEAVES (+8/-8 flipping with the gait row's bit15), but inside 5001 it
 * turns monotonically onto you, and inside 3000 it turns at 8+16 per frame. RE1.5 instead flips the
 * sign at every gait segment boundary regardless of distance — which is exactly the "drunk" walk.
 *
 * ⚠️ DEFERRED, NOT byte-true yet: the steer point (+0x1C4/+0x1C6) is produced by the RE2 navigator
 * FUN_8004A808, called from the root BEFORE the dispatch (@0x80100354). That navigator is not ported,
 * so this uses the PLAYER position as the steer target. For open-floor pursuit the two coincide; they
 * differ around corners/obstacles. Marked here so nobody mistakes it for a finished port. */
#define RE2Z_DIST_LOCKON   0x1389u   /* 5001 @0x80101bb4 */
#define RE2Z_DIST_CLOSE    0x0bb8u   /* 3000 @0x80101c94 */
#define RE2Z_TURN_BASE     8         /*      @0x80101bbc */
#define RE2Z_TURN_EXTRA    16        /*      @0x80101cac */

/* Gait table @0x8010C924 — 32 u16, byte-verified against EMZ0.BIN. EVERY entry packs BOTH:
 *     bits 0..14 = the segment DURATION in frames   (andi 0x7fff @0x80101B84)
 *     bit  15    = the TURN-AWAY flag               (srl 15     @0x80101BDC)
 * so the sequence reads 190/50, 150/64, 110/64, 180/52, ... = a long segment turning TOWARD the
 * steer point alternating with a short one turning AWAY. That alternation IS the RE2 walk. */
const uint16_t re15_re2z_gait_tbl[32] = {
      190, 32818,   150, 32832,   110, 32832,   180, 32820,
      140, 32820,   120, 32826,   150, 32846,   140, 32818,
      150, 32830,   150, 32840,   110, 32838,   120, 32842,
      180, 32802,   110, 32820,   200, 32834,   180, 32834,
};

/* The turn helper itself is ALREADY in the port, byte-true, and is literally the same engine
 * function in both games — verified by disassembling both and diffing:
 *     RE1.5  FUN_8001aac4 (a0=tx a1=tz a2=slew, operates on the global current entity,
 *                          pos +0x34/+0x3c, yaw +0x6a, bearing via FUN_8001a6d4)
 *     RE2    FUN_80015558 (a0=actor* a1=tx a2=tz a3=rate,
 *                          pos +0x38/+0x40, yaw +0x76, bearing via FUN_800154ac)
 * Different signature and struct offsets, IDENTICAL math after the bearing:
 *     bgez rate      -> rate<0: rate=-rate AND bearing+=0x800 (steer AWAY, not "clamp the other way")
 *     delta = (rate + bearing - yaw) & 0xfff
 *     delta < 2*rate -> SNAP yaw = bearing            (@0x800155cc / RE1.5 same slt)
 *     delta < 0x801  -> yaw += rate  else  yaw -= rate (@0x800155d4 sltiu 0x801)
 * The bearing helpers are byte-identical too: same 0x400/0xc00 base, same catan((dz<<12)/dx), and
 * RE2's `(4096-at)&0xfff` == RE1.5's `(0-at)&0xfff`. So the RE2 walk turn reuses the port's
 * re15_enemy_steer_point verbatim — no second implementation, nothing re-derived by hand. */
extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);

/* ---- the RE2 PRNG (@0x80015FE8) ------------------------------------------------------------
 *   80015ff0: lw   v0,0(a0)      ; a0 = 0x800CE318, the 16-bit state
 *   80015ff8: srl  v1,v0,7
 *   80015ffc: andi v1,v1,0xff    ; h = (s >> 7) & 0xff
 *   80016000: addu v0,v1,v0
 *   80016004: andi v0,v0,0xff    ; lo = (h + s) & 0xff
 *   80016008: sll  v1,v1,8
 *   8001600c: or   v0,v0,v1      ; v = lo | (h << 8)
 *   80016014: andi v0,v0,0xff    ; RETURN v & 0xff
 *   80016018: sw   v1,0(a0)      ; STATE  v & 0xffff
 * Seeded to 0xD2706CA4 (lui 0xd270 / ori 0x6ca4) at both writers of 0x800CE318: @0x8002B908-1C and
 * @0x8003BCB0-C4. Seeding matters — from state 0 this generator is a fixed point (0 -> 0 forever).
 * Kept separate from the port's RE1.5 RNG on purpose: the RE1.5 generator hashes the CALLER's
 * argument (its state store @0x800AC774 is a dead store), this one feeds back on itself. */
static uint32_t s_re2_rng = 0xD2706CA4u;
static uint32_t re2z_rand(void)
{
    uint32_t s = s_re2_rng;
    uint32_t h = (s >> 7) & 0xffu;
    uint32_t v = ((h + s) & 0xffu) | (h << 8);
    s_re2_rng  = v & 0xffffu;
    return v & 0xffu;
}
void re15_re2z_rng_reset(void) { s_re2_rng = 0xD2706CA4u; }   /* room load — keeps runs deterministic */

/* ---- the GAIT MACHINE (@0x80101A7C-AC init, @0x80101B2C-90 per tick) ------------------------
 * INIT (walk entered):
 *   80101a90: andi v0,v0,0xf     ; row = (rand & 0xf)
 *   80101a94: sll  v0,v0,1       ;     * 2  -> the start row is ALWAYS EVEN
 *   80101a9c: sb   v0,363(s1)    ; +0x16B
 *   80101ac0: addu v1,v1,v0      ; timer = (tbl[row] & 0x7fff) + rand   <-- FULL byte here
 *   80101ac8: sh   v1,344(s1)    ; +0x158
 * PER TICK:
 *   80101b2c: addiu v0,v1,-1
 *   80101b30: bne  v1,zero,turn  ; timer != 0 -> just decrement and go turn
 *   80101b34: sh   v0,344(s1)
 *   80101b40: addiu v0,v0,1      ; else row += 1
 *   80101b50: slti v0,v0,32      ;      wrap at 32
 *   80101b5c: sb   zero,363(s1)
 *   80101b80: andi v0,v0,0x1f    ; timer = (tbl[row] & 0x7fff) + (rand & 0x1f)  <-- MASKED here
 *   80101b90: sh   v1,344(s1)
 * The init/expiry asymmetry (full byte vs & 0x1f) is in the bytes, not a typo on my part.
 *
 * NOT ported (read, but their consumers are unidentified — flagged rather than invented):
 *   +0x16A = (rand & 0x1f) + 30   @0x80101AD4-E8
 *   +0x14D = (rand & 0x1f)        @0x80101B04-0C
 */
void re15_re2z_gait_init(re15_actor_t *e)
{
    if (!e) return;
    uint32_t r1 = re2z_rand();
    e->re2z_gaitrow = (uint8_t)((r1 & 0x0fu) << 1);                      /* @0x80101a90/94/9c */
    uint32_t r2 = re2z_rand();
    e->re2z_gaittmr = (uint16_t)((re15_re2z_gait_tbl[e->re2z_gaitrow] & 0x7fffu) + r2);
    e->re2z_gaitinit = (uint8_t)(0x80u | (e->sub_state_1 & 0x7fu));
}

static void re2z_gait_tick(re15_actor_t *e)
{
    /* The original runs the init block on the tick the WALK SUBSTATE IS ENTERED (it sits in the
     * substate prologue @0x80101A7C, ahead of the per-tick timer code at 0x80101B2C). The port
     * detects that same entry by the substate byte changing since the last gait tick — so a zombie
     * that gets knocked down and walks again re-rolls its gait exactly like the original, instead
     * of resuming a stale row. `re2z_gaitinit` therefore stores 0x80 | sub_state_1, never a bool. */
    uint8_t tag = (uint8_t)(0x80u | (e->sub_state_1 & 0x7fu));
    if (e->re2z_gaitinit != tag) { re15_re2z_gait_init(e); e->re2z_gaitinit = tag; return; }
    if (e->re2z_gaittmr != 0) { e->re2z_gaittmr--; return; }             /* @0x80101b2c-34 */
    unsigned row = (unsigned)e->re2z_gaitrow + 1u;                       /* @0x80101b40 */
    if (row >= 32u) row = 0u;                                            /* @0x80101b50/5c */
    e->re2z_gaitrow = (uint8_t)row;
    uint32_t r = re2z_rand() & 0x1fu;                                    /* @0x80101b80 */
    e->re2z_gaittmr = (uint16_t)((re15_re2z_gait_tbl[row] & 0x7fffu) + r);
}

/* Apply the RE2 walk turn for one tick. `dist` = distance to the player (the port's ai_dist, which is
 * the same quantity RE2 keeps in +0x1F0). Returns 1 if it handled the turn. */
int re15_re2z_walk_turn(re15_actor_t *e, int32_t px, int32_t pz, uint32_t dist)
{
    if (!e) return 0;
    re2z_gait_tick(e);
    int rate;
    if (dist < RE2Z_DIST_LOCKON) {
        rate = RE2Z_TURN_BASE;                                   /* @0x80101bec lock-on */
    } else {
        int row   = e->re2z_gaitrow & 31;                        /* +0x16B, bounded by slti 32 */
        int bit15 = (re15_re2z_gait_tbl[row] >> 15) & 1;         /* @0x80101bdc/e0 */
        rate = RE2Z_TURN_BASE - (bit15 ? 16 : 0);                /* @0x80101be8 -> +8 or -8 */
    }
    re15_enemy_steer_point(e, px, pz, rate);                     /* @0x80101c00 jal 0x80015558 */
    if (dist < RE2Z_DIST_CLOSE)
        re15_enemy_steer_point(e, px, pz, RE2Z_TURN_EXTRA);      /* @0x80101ca8/cac, a3 = 16 */
    return 1;
}
