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
#include <stdio.h>    /* RE15_RE2_TRACE */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_damage.h"     /* re15_ai_arc_test — the RE1.5 twin of RE2 FUN_80015614 */
#include "re15_enemy_ai.h"   /* re15_ai_set_state_word / live_init / victim FSM / is_grabbed */
#include "re15_enemy.h"      /* re15_enemy_find (RE2 bank from the Welle-A loader) */
#include "re15_esp.h"        /* re15_esp_fx_spawn_ex (RE1.5 hit-FX stand-in, documented) */

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
/* WELLE C: the dog draws from the SAME generator (RE2 has ONE state word 0x800CE318 for every
 * overlay — 69 jal 0x80015FE8 in the dog module alone). Exported for enemy_ai_re2_dog.c. */
uint32_t re15_re2_rand(void) { return re2z_rand(); }
void re15_re2z_onesave_reset(void);                           /* Welle B (below): FUN_800401d4 latch */
void re15_re2z_rng_reset(void) { s_re2_rng = 0xD2706CA4u;     /* room load — keeps runs deterministic */
                                 re15_re2z_onesave_reset();
                                 re15_re2dog_room_reset(); }  /* WELLE C: 0x800CFBF4-Analog (der
                                                               * einzige EXE-Clear FUN_80052f3c ist
                                                               * ein Room-Init) */

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

/* FUN_80015758(a0=ptrA, a1=ptrB, a2=ang, a3=half) — the SECTOR test block G uses twice.
 * Self-disassembled 2026-07-29:
 *   80015778: lh a0,0(v0)    ; A.x  (the callers pass self+0x38 / PL+0x38, so X is at +0, Z at +8)
 *   8001577c: lh a1,8(v0)    ; A.z
 *   80015780: lh a2,0(v1)    ; B.x
 *   80015784: lh a3,8(v1)    ; B.z
 *   80015788: jal 0x800154ac ; bearing(A -> B)
 *   8001579c: subu v0,v0,s1  ; bearing - ang
 *   800157a8: addu v0,v0,s0  ;         + half
 *   800157ac: andi v0,v0,0xfff
 *   800157b0: sll  s0,s0,1   ; 2*half
 *   800157b4: sltu v0,v0,s0
 *   800157b8: xori v0,v0,0x1 ; RETURN !(t < 2*half)  ->  0 == INSIDE the sector
 * Same family as FUN_80015614/FUN_8001A9CC, but the cone is centred on an ARBITRARY angle instead
 * of the actor's own yaw — which is exactly why block G can test two half-sectors either side. */
static int re2z_sector(const re15_actor_t *a, const re15_actor_t *b, int ang, int half)
{
    /* the port's mesh-yaw convention carries a +0x400 offset (re15_damage.c re15_ai_arc_test) */
    int bearing = ((int)re15_atan2_q12(b->z - a->z, b->x - a->x) - 0x400) & 0xfff;
    unsigned t  = (unsigned)((bearing - ang + half) & 0xfff);
    return !(t < (unsigned)(half << 1));          /* 0 == inside */
}

/* Fill the ladder's gate struct from PORT state. Every field is one of:
 *   MAPPED   — a port quantity that provably is the same thing
 *   ZERO     — provably zero for a zombie the port actually runs (cited)
 *   OPEN     — no proven producer in the port yet; left 0, and the block it gates cannot fire.
 * WELLE B closed +0x23E (re2z_cd23e), +0x21A (re2z_flags21a) and +0x110 (contact). */
void re15_re2z_fill_gates(const re15_actor_t *e, const re15_actor_t *pl,
                          int player_claimed, re15_re2z_gates_t *g)
{
    for (unsigned i = 0; i < sizeof(*g); i++) ((unsigned char *)g)[i] = 0;
    if (!e || !pl) return;

    /* MAPPED ------------------------------------------------------------------------------- */
    g->dist    = e->ai_dist;                                    /* +0x1F0        @0x80101744 */
    g->arc1024 = re15_ai_arc_test(e, pl->x, pl->z, 1024);       /* a3 = 1024     @0x8010174c */
    g->arc512  = re15_ai_arc_test(e, pl->x, pl->z,  512);       /* a3 = 512      @0x80101754 */
    /* the third arc (half 1300, jal @0x80101788) is NOT computed: its v0 is destroyed by
     * `lbu v0,574(s0)` @0x80101790 before any reader, and the whole call chain
     * FUN_80015614 -> FUN_800154AC -> catan is store-free, so omitting it is observable nowhere. */
    g->self_106 = e->floor;                                     /* +0x106        @0x80101808 */
    g->pl_106   = pl->floor;                                    /*               @0x8010180c */
    /* WELLE B producers (closed by the Lane-Z RE + own re-disasm 2026-08-10):
     *  +0x23E = the snap-bite cooldown: ONLY writer sb 60 @0x80104E2C (EXEC[14] end), root
     *           decrement @0x80100470-80 — both now live in the port (re2z_cd23e).
     *  +0x21A = the flag word: INIT clear @0x8010087C; side latches 0x20/0x40 from grab-escape
     *           P5 (@0x80102A34-40); 0x4 lying orientation; 0x10 crawl marker (@0x8010358C);
     *           0x4000 kill-counted (@0x80108294) — re2z_flags21a.
     *  +0x110 bit 0 = the collision-query result (`sw v0,272(s0)` @0x800356D8, query
     *           FUN_8004C1BC, frame stamp +0x114 @0x800356E8). PORT MAPPING: the body-push
     *           contact bits +0x1C2 (contact_flags, holding LAST frame's result at AI time —
     *           same phase relationship as the original's frame stamp). Block C stays inert
     *           anyway because +0x1D4 never carries 0xC000 in RE1.5 rooms (see below). */
    g->self_23e = e->re2z_cd23e;                                /* +0x23E        @0x80101790 */
    g->self_21a = e->re2z_flags21a;                             /* +0x21A        @0x80101928 */
    g->self_110 = (e->contact_flags != 0) ? 1u : 0u;            /* +0x110 bit0   @0x80101844 */
    /* PL+0x1D3 bit 0x80 = the "this actor is claimed" latch. Mechanism fully proven: the zombie
     * SETS it in nine places (e.g. @0x80101968 / @0x801019B0) and never clears it; the CLEAR lives
     * on the player side (`andi 0x7f` + sb, @0x8003E844 / @0x800630E0 and the overlay grab-aborts
     * @0x80104FAC HURT / @0x801082F4 DEATH). The port's equivalent latch is s_player_grabbed.
     * The LOW SEVEN BITS are a player-side countdown (~40 EXE writers, decrement @0x8003BFF4-C008)
     * with no port producer — left 0 (OPEN): B/J stay gated by their OTHER conditions, which in
     * RE1.5 rooms keep them silent (B needs floors to DIFFER, J needs PL+0x8 == 15). */
    g->pl_156  = pl->hp;                                        /* +0x156        @0x801019e8 */
    g->pl_1d3  = player_claimed ? 0x80u : 0u;                   /*               @0x80101908 */
    g->g1_sector_hit = (re2z_sector(e, pl, ((int)e->rot_y + 256) & 0xfff, 256) == 0); /* @0x80101948 */
    g->g2_sector_hit = (re2z_sector(e, pl, ((int)e->rot_y - 256) & 0xfff, 256) == 0); /* @0x8010198c */

    /* ---- 0x800CFBF6: the global that gates blocks D and E ------------------------------------
     * RESOLVED (self-RE'd 2026-07-29). It is a PER-FRAME "what is the player doing right now"
     * bitfield, not a persistent flag:
     *   - It has exactly FOUR writers in the whole RE2 EXE. @0x8003BFF0 CLEARS bits 0..4
     *     (`andi 0xffe0`) — gated on 0x800CFBDC >= 0 @0x8003BFC0, so "cleared every frame" is
     *     wrong. The other three only ever OR a bit in.
     *   - Bits 0x1 and 0x10 are NEVER set anywhere, so mask 0x15 (block D) reduces to bit 0x4 and
     *     mask 0x17 (block E) to (0x2 | 0x4).
     *   - The three setters are player SUB-STATE handlers, reached through the player's action
     *     table: dispatcher @0x8003C5D4 indexes base 0x800A4084 by player+0x5, so
     *         sub 1 = 0x8003CBDC -> `ori 0x2` @0x8003CC80
     *         sub 2 = 0x8003D0E8 -> `ori 0x4` @0x8003D18C
     *         sub 3 = 0x8003D5F4 -> `ori 0x2` @0x8003D6B4
     *   - Which sub-state is which comes from the pad-driven selector @0x8003C650-C6C8:
     *         virtual bit 0x1   (forward)  -> sub 1   word 0x101 @0x801017.. see @0x8003C6B0/B4
     *         virtual bit 0x200 (run/cross)-> sub 2   word 0x201 @0x8003C6C0/C4
     *         virtual bit 0x4   (backward) -> sub 3   word 0x301 @0x8003C6A0/A4
     *         virtual bits 0xa  (turn L/R) -> sub 4   word 0x401 @0x8003C690/94  (sets NOTHING)
     *     Bit meanings from the port's own virtual-pad table (pad_common.c:27-35, RE1.5
     *     @0x80073dbc): bit0<-UP, bit2<-DOWN, bits1|3<-RIGHT|LEFT, bit9<-CROSS.
     * => bit 0x2 means "the player is WALKING" (forward or backward) and bit 0x4 means "the player
     *    is RUNNING". So block D (running only, from 3500, 25%) and block E (any movement, from
     *    2500, 50%) are movement-reactive: standing still, neither can fire. That is the RE2
     *    behaviour the folklore mistook for eyesight.
     *
     * ⚠️ PORT MAPPING, not a byte-true port of the field: the RE2 producer is RE2's player state
     * machine, which the port does not implement. The port's equivalent movement state is the
     * player motion sentinel (player_common.c:65-67: RUN = 100, WALK = 105, BACK = WALK + reverse),
     * recomputed every tick exactly like the original's per-frame bits. */
    if      (pl->motion == 100) g->global_cfbf6 = 0x04u;        /* RUN  -> @0x8003D18C */
    else if (pl->motion == 105) g->global_cfbf6 = 0x02u;        /* WALK -> @0x8003CC80 / @0x8003D6B4 */
    else                        g->global_cfbf6 = 0x00u;        /* idle/turn set NOTHING */

    /* ZERO / dead branches, with the proof ------------------------------------------------------
     * self+0x1F4/+0x1F8 (block A): ZERO writers anywhere in EMZ0.BIN — the producer is
     *   FUN_80065518 in the em/NPC family, which only ticks entity types 64..91. Zombies are
     *   clamped to 16..31 by the overlay loader (`addiu v0,t1,-16; sltiu v0,v0,0x10 -> t1=16`
     *   @0x8001B738-48), so block A cannot fire in a zombie-only room. DEAD, documented inert.
     * self+0x1D4 (block C): no writer in the zombie overlay; all EXE writers are script-/spawn-
     *   parameter driven (`lhu v0,2(a1)` @0x800570F4, setter @0x80055D90, switch @0x80056C60-D74,
     *   clears @0x800573E8/@0x80057A7C). RE1.5 room data never carries RE2's 0xC000 bits ->
     *   block C (EXEC[10] "hammering", reads +0x1D4 @0x80104220) stays INERT, documented.
     * pl_156 == -32768 (block K): the only -32768 -> +0x156 store in the game (@0x8010B730/38)
     *   writes an ENEMY's HP; how the PLAYER HP ever becomes -32768 is OPEN (FUN_800401d4 clamps
     *   death at "HP<-14"). The port's player HP never is -32768 -> K stays INERT, documented.
     * PL+0x8 (the G/J fork): player routine id 15 — semantics OPEN; 0 keeps the G branch. */
}

/* Run the RE2 walk decision for this tick and report the state word it commits, if any. */
int re15_re2z_walk_decide(const re15_actor_t *e, const re15_actor_t *pl,
                          int player_claimed, re15_re2z_decision_t *out)
{
    re15_re2z_gates_t g;
    re15_re2z_fill_gates(e, pl, player_claimed, &g);
    return re15_re2z_decide_walk(&g, out);
}

/* ============================================================================================
 * W2 — THE ATTACK-DECISION LADDER, DECISION[1] = 0x80101714
 *
 * CALL CONTEXT @0x801011A8-EC: 0x8010118C does `lbu +5` -> DECISION[0x8010C88C] -> `lbu +5` AGAIN
 * -> EXECUTOR[0x8010C8CC]. A store here therefore runs its executor THE SAME TICK.
 *
 * "LAST WRITER WINS" — self-verified, not assumed: in 0x80101714..0x80101A34 there are EXACTLY 9
 * `sw ...,4(s0)` (0x801017E4, 82C, 858, 8A0, 8E4, 958, 99C, 9E4, A10) and EXACTLY 2 jumps —
 * `j 0x80101a1c` @0x801017E0 (block A into the epilogue, the ONLY early exit) and `j 0x801019e8`
 * @0x801019AC (G skips J). No instruction in the function READS +0x4, so sequential C is exact.
 * If no block fires, +0x4 is left untouched — hence `wrote`.
 *
 * RNG DRAW COUNT IS ITSELF BEHAVIOUR: 0..2 draws per tick (D @0x80101888, E @0x801018D0), and only
 * after their first three gates pass. Right order + wrong draw count = the whole sequence desyncs.
 *
 * Word -> bytes (LE): 0x0E01 -> sub 14; 0x0A01 -> 10; 0x0C01 -> 12; 0x0301 -> 3;
 * 0x00060801 -> sub 8 with phase +0x6 = 6. Every `sw` ZEROES +0x6/+0x7 — load-bearing, because the
 * executors dispatch on +0x6.
 * ============================================================================================ */
int re15_re2z_decide_walk(const re15_re2z_gates_t *g, re15_re2z_decision_t *out)
{
    if (!g || !out) return 0;
    out->wrote = 0; out->word = 0; out->early_out = 0;
    out->claim_player = 0; out->set_10e_4000 = 0; out->rng_draws = 0;

    if (g->self_23e == 0) {                                   /* lbu 574(s0) @0x80101790, bne @0x8010179c */
        /* --- A: the ONLY early return. j 0x80101a1c @0x801017E0, store in its delay slot ------ */
        if ((g->self_1f4 & 0xC0000000u)                       /* @0x801017a0/ac/b0                 */
            && (g->self_1f4 & 0x3FFFFFFFu) < 2000u            /* sltiu 0x7d0 @0x801017c0           */
            && g->a_sector_hit) {                             /* FUN_80015714(...)==0 @0x801017d0  */
            out->wrote = 1; out->word = 0x00000E01u; out->early_out = 1;
            return 1;                                         /* sw @0x801017e4 (delay slot)       */
        }
        /* --- B: same word, but NO return — falls through into C ------------------------------ */
        if (g->arc512 == 0                                    /* sll/bne @0x801017f4/f8            */
            && g->dist < 2000u                                /* sltiu @0x801017fc (UNSIGNED)      */
            && g->self_106 != g->pl_106                       /* beq-away @0x80101814              */
            && g->pl_1d3 == 0) {                              /* bne @0x80101824 (the WHOLE byte)  */
            out->wrote = 1; out->word = 0x00000E01u;          /* sw @0x8010182c                    */
        }
    }
    /* --- C ------------------------------------------------------------------------------------ */
    if ((g->self_1d4 & (int16_t)0xC000) != 0                  /* lh @0x80101830 / andi @0x80101838 */
        && (g->self_110 & 1u) != 0) {                         /* lw @0x80101844 / andi @0x8010184c */
        out->wrote = 1; out->word = 0x00000A01u;              /* sw @0x80101858                    */
    }
    /* --- D ------------------------------------------------------------------------------------ */
    if (g->dist < 3500u                                       /* sltiu 0xdac @0x8010185c           */
        && g->arc1024 != 0                                    /* beq-away @0x80101868 -> OUTSIDE   */
        && (g->global_cfbf6 & 0x15u) != 0) {                  /* lhu @0x80101874 / andi @0x8010187c */
        out->rng_draws++;                                     /* jal 0x80015FE8 @0x80101888        */
        if ((re2z_rand() & 3u) == 0u) {                       /* andi @0x80101890 / bne @0x80101894 */
            out->wrote = 1; out->word = 0x00000C01u;          /* sw @0x801018a0                    */
        }
    }
    /* --- E: SEQUENTIAL with D, not a tier. @0x80101860 sends a FAILED D straight to E's own test
     *        @0x801018a4, so below 2500 BOTH run and BOTH draw. ------------------------------- */
    if (g->dist < 2500u                                       /* sltiu 0x9c4 @0x801018a4           */
        && g->arc1024 != 0                                    /* beq-away @0x801018b0              */
        && (g->global_cfbf6 & 0x17u) != 0) {                  /* andi @0x801018c4                  */
        out->rng_draws++;                                     /* jal @0x801018d0                   */
        if ((re2z_rand() & 1u) == 0u) {                       /* andi @0x801018d8 / bne @0x801018dc */
            out->wrote = 1; out->word = 0x00000C01u;          /* sw @0x801018e4                    */
        }
    }
    /* --- the G / J fork ----------------------------------------------------------------------- */
    if (g->pl_8 != 15) {                                      /* lbu @0x801018e8 / beq @0x801018f0 */
        /* --- G: the SIDE GRAB. Two INDEPENDENT halves; G1 falls THROUGH into G2. ------------- */
        if (g->dist < 1200u                                   /* sltiu 0x4b0 @0x801018f4           */
            && !(g->pl_1d3 & 0x80u)                           /* andi @0x80101908 / bne @0x8010190c */
            && g->self_106 == g->pl_106) {                    /* bne-away @0x80101920              */
            if (!(g->self_21a & 0x20u) && g->g1_sector_hit) { /* andi @0x80101930; jal @0x80101948 */
                out->wrote = 1; out->word = 0x00000301u;      /* sw @0x80101958                    */
                out->claim_player = 1;                        /* PL[0x1D3] |= 0x80 @0x80101968     */
            }
            if (!(g->self_21a & 0x40u) && g->g2_sector_hit) { /* andi @0x80101974; jal @0x8010198c */
                out->wrote = 1; out->word = 0x00000301u;      /* sw @0x8010199c                    */
                out->claim_player = 1;                        /* PL[0x1D3] |= 0x80 @0x801019b0     */
            }
        }
        /* j 0x801019e8 @0x801019AC — the G branch ALWAYS skips J */
    } else {
        /* --- J: block B minus the +0x106 test ------------------------------------------------ */
        if (g->self_23e == 0                                  /* lbu @0x801019b4 / bne @0x801019bc */
            && g->arc512 == 0                                 /* sll/bne @0x801019c0/c4            */
            && g->dist < 2000u                                /* sltiu @0x801019c8                 */
            && g->pl_1d3 == 0) {                              /* bne @0x801019dc                   */
            out->wrote = 1; out->word = 0x00000E01u;          /* sw @0x801019e4                    */
        }
    }
    /* --- K: the LAST writer, so it beats every block above ------------------------------------ */
    if (g->pl_156 == (int16_t)-32768                          /* lh 342(s1) @0x801019e8            */
        && g->arc512 == 0                                     /* sll/bne @0x801019f4/f8            */
        && g->dist < 1000u) {                                 /* sltiu 0x3e8 @0x801019fc           */
        out->wrote = 1; out->word = 0x00060801u;              /* lui 0x6/ori 0x801/sw @0x80101a10  */
        out->set_10e_4000 = 1;                                /* +0x10E |= 0x4000 @0x80101a08/14/18 */
    }
    return out->wrote;
}

/* ============================================================================================
 * WELLE B — THE FULL RE2 ZOMBIE BRAIN (behavior + presentation).
 *
 * re15_re2z_tick REPLACES the RE1.5 state dispatch for an owned zombie (hook in
 * re15_enemy_ai_live_tick / re15_zgirl_ai_tick). Structure mirrors the overlay:
 *   root prolog (cooldown bank) @0x8010045C-98  ->  state table @0x8010C830:
 *   [0] INIT 0x8010065C  [1] ACTIVE 0x8010114C (DECISION @0x8010C88C then EXECUTOR @0x8010C8CC
 *   on the SAME tick — 0x8010118C re-reads +0x5 between the two, @0x801011A8-EC)
 *   [2] HURT 0x80104F40  [3] DEATH 0x80108250  [7] CORPSE 0x8010A440  [8] 0x80109CFC.
 *
 * PRESENTATION: clip indices are RE2-NATIVE (the Welle-A re2_ems loader fills the actor's bank
 * with the real EM01x EMD, 31-clip action bank); SEs go through the ENEMSE hook below.
 * Every constant carries its EMZ0.BIN address (all re-disassembled 2026-08-10, see the
 * per-line cites). OPEN items are marked OPEN, mapped stand-ins are marked MAPPING.
 * ==========================================================================================*/

/* ---- the byte-verified model parameter block @0x80100000 (EMZ0.BIN offsets 0x00..0xDF) ----
 * dump 2026-08-10: 05 00 00 00 | 00 02 00 02 00 02 02 00 | 0b 0b 0e 0e | 0b 0b 0e 0e |
 *                  10 14 01 05 10 1e 01 0a | <10 grab-phase ptrs == 0x801026C0..0x80102EB4> |
 *                  @0x44: 01 02 17 16 08 09 | @0x4C: 5a 00 1e 00 ... | @0xD8: 03 03 04 0d */
static const uint8_t re2z_param_walk[8]  = { 0, 2, 0, 2, 0, 2, 2, 0 };       /* @0x80100004 */
static const uint8_t re2z_param_grab[8]  = { 0x0B,0x0B,0x0E,0x0E,0x0B,0x0B,0x0E,0x0E }; /* @0x8010000C
                                              * P0 reads [s5*2] (`sll v0,s5,1; lbu 16(v0)`
                                              * @0x801026C4-CC) -> upright 0x0B, crawler 0x0E */
static const uint8_t re2z_param_bite[8]  = { 16,20, 1,5, 16,30, 1,10 };      /* @0x80100014
                                              * (bite-frame, damage) pairs, index s5*2:
                                              * upright (16,20), crawler (1,5), kind 0x11/0x17
                                              * upright (16,30) / crawler (1,10) @0x801028A0-FC */
static const uint8_t re2z_param_clips[6] = { 0x01,0x02,0x17,0x16,0x08,0x09 };/* @0x80100044
                                              * [0..1] knockdown-fall L/R (EXEC[5] P0 @0x801032C8),
                                              * [2..3] corpse-lie 23/22 (@0x8010A490-BC),
                                              * [4..5] ground-lie idle 8/9 (@0x80103574-A8) */
static const uint8_t re2z_param_getup[4] = { 3, 3, 4, 13 };                  /* @0x801000D8
                                              * EXEC[9] get-up clip list (copied to sp+16
                                              * @0x80103E6C-84) */
static const uint8_t re2z_param_feed[8]  = { 0x12,0x13,0x14,0x12,0x13,0x14,0x12,0x13 };
                                             /* @0x801000A8 (bytes gedumpt) — der Fress-Loop
                                              * rotiert per rand&7 (@0x80103BF4-C10 P0,
                                              * @0x80103CB0-D4 P2 Re-Draw) */
static const uint8_t re2z_param_walk13[8] = { 0, 2, 0, 2, 0, 2, 0, 2 };       /* @0x801000F8
                                              * (bytes gedumpt) — EXEC[13]s EIGENE Stil-Tabelle
                                              * (kopiert @0x80104958-74), EIN Draw + andi 7
                                              * (@0x801049C4-EC) — NICHT die INIT-Tabelle */
static const uint16_t re2z_hp13_tbl[16] = { 70,84,118,65,50,85,48,65,40,73,69,56,70,55,72,55 };
                                             /* @0x8010C600 (selbst gelesen) — EXEC[13]s
                                              * HP-Re-Roll `sh v1,342(s1)` @0x801049C8 */

/* ---- ENEMSE audio hook (PC registers; the engine stays link-clean for PSX) ---------------- */
static void (*s_re2z_se_fn)(int se_id, int flag2000) = 0;
static void (*s_re2z_se_bank_fn)(int bank) = 0;

/* Zombie ENEMSE bank. The room-pair table @0x800a7400 (RE2 PSX.EXE file 0x97C00, 73 rows x 2,
 * byte-read 2026-08-10: row 11 = {0x10,0x00}) could not be resolved to a kind base (values fit
 * kind-0x10 for the PAIR rows 32+ but leave no pure-cop row). EMPIRICAL probe over the decoded
 * EDT maps (all 73 banks): bank 0 is the only SINGLE-kind bank whose live map covers EVERY SE
 * id the zombie overlay triggers ({3,4,5,8,9,10,11,12} — live ids 0..13); bank 11 lacks 11/12,
 * banks 28/65 lack 4/5 in their zombie half, bank 72 covers them only in the +0x10 half.
 * -> default bank 0, flag2000=0; RE15_RE2_SE_BANK overrides for A/B listening. OPEN: the exact
 * FUN_80052b38 row semantics. */
#define RE2Z_ENEMSE_BANK 0

void re15_re2z_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int))
{
    s_re2z_se_fn      = se_fn;
    s_re2z_se_bank_fn = bank_fn;
    if (s_re2z_se_bank_fn) {
        const char *ov = getenv("RE15_RE2_SE_BANK");
        s_re2z_se_bank_fn(ov ? atoi(ov) : RE2Z_ENEMSE_BANK);
    }
}
static void re2z_se(int id) { if (s_re2z_se_fn) s_re2z_se_fn(id, 0); }
void re15_re2z_se_play(int se_id) { re2z_se(se_id); }   /* Frame-Flag-SFX 0x801016c8-Pfad */

/* ---- FUN_800401d4 — the grab-bite player damage (decompile-read; port of the mechanism) ----
 * HP store 0x800CFD4E (= PL+0x156). Death: HP < -14 OR the one-save latch (0x800CFB74|0x1000)
 * already set; otherwise the first lethal bite is "one save": HP = 0 + latch. Return bits used
 * by the grab P3 (@0x80102900-24): bit0 -> throw-off, bit1 -> player dead (self word 0x601 +
 * PL cmd (dir<<8)|6 @0x80102924-50).
 * NOT ported (documented): the x1.5 / x5 / x2 damage scalings — they hang off RE2-only globals
 * (flag 0x40 @0x800CFB74, HP>120 on RE2's 200-HP scale, difficulty @0x800D482A) that have no
 * RE1.5-port equivalent. Raw byte-cited damages apply 1:1. */
static int s_re2z_onesave = 0;   /* cleared with the PRNG on room load (re15_re2z_rng_reset) */
static int re2z_player_damage(re15_actor_t *pl, int dmg)
{
    pl->hp = (int16_t)(pl->hp - dmg);
    if (pl->hp < 0) {
        if (pl->hp < -14 || s_re2z_onesave) return 2;    /* death (bit1) */
        pl->hp = 0; s_re2z_onesave = 1; return 1;        /* one save (bit0) -> throw-off */
    }
    return 0;
}
void re15_re2z_onesave_reset(void) { s_re2z_onesave = 0; }

/* WELLE D: der volle FUN_800401d4-MODE-Parameter (Decompile RE2_Quellcode_V2/FUN_800401d4.c,
 * selbst gelesen 2026-08-16). Der Kraehen-GRAB-Peck ruft (5, aliveflag) @0x8010265C-64
 * (`lbu a1,536(s0); jal 0x800401d4; addiu a0,zero,5` — a1 = +0x218 = HP>0?1:0, Prolog
 * @0x801024C4-D8):
 *   mode 1 (Spieler lebt): HP-=dmg; HP>=0 -> 0; HP<0 -> KEIN Todespfad, faellt in den
 *          One-Save-Schwanz (HP=0 + Latch, ret 1) — der Peck allein toetet NIE direkt.
 *   mode 0 (HP<=0):        wie gehabt (HP<-14 ODER Latch -> ret 2 Tod; sonst One-Save).
 *   mode >1:               ret 1 ohne One-Save-Schreiber (`if (param_2 != 0) return 1`).
 * Die x1.5/x5/x2-Skalierungen bleiben wie in Welle B unportiert (RE2-only-Globals). */
int re15_re2_player_damage_mode(re15_actor_t *pl, int dmg, int mode)
{
    if (mode == 0) return re2z_player_damage(pl, dmg);
    pl->hp = (int16_t)(pl->hp - dmg);
    if (mode != 1) return 1;                             /* `if (param_2 != 0) return 1` */
    if (pl->hp >= 0) return 0;                           /* `-1 < param_1*0x10000 -> 0` */
    pl->hp = 0; s_re2z_onesave = 1; return 1;            /* One-Save-Schwanz */
}
/* WELLE C: the dog's flight bite runs through the SAME FUN_800401d4 (jal @0x80104EBC, a0=20,
 * a1=0) and shares the ONE-SAVE latch DAT_800cfd4c bit 0x1000 with the zombie bite — one
 * exported entry keeps that latch single. Return: 0 survived / 1 one-save / 2 death. */
int re15_re2_player_damage(re15_actor_t *pl, int dmg) { return re2z_player_damage(pl, dmg); }

/* 0x800CFBF6 movement bits from PORT state (the mapping the ladder already used; see the
 * fill_gates comment for the producer chain @0x8003CC80/@0x8003D18C/@0x8003D6B4). */
static uint16_t re2z_cfbf6(const re15_actor_t *pl)
{
    if (pl->motion == 100) return 0x04u;     /* RUN  @0x8003D18C */
    if (pl->motion == 105) return 0x02u;     /* WALK @0x8003CC80 / @0x8003D6B4 */
    return 0;
}

/* Clip-word write `sw (rate<<16)|(frame<<8)|clip, 332(s)` -> port anim fields. The +0x14E rate
 * half maps onto the port's crossfade seed (same 0xF/7 family as RE1.5's +0x8f), the advance
 * helper 0x8002959c's a3 (256/512) onto anim_blend_rate (0x100/0x200). */
static void re2z_clip(re15_actor_t *e, int clip, int frame, int frac, int blend, int loop)
{
    e->motion     = (int16_t)clip;
    e->anim_frame = (uint16_t)frame;
    e->anim_frac  = (uint8_t)frac;
    e->anim_blend_rate = (uint16_t)blend;
    e->anim_freeze = 0;
    e->anim_flags &= (uint16_t)~(0x80u | 0x04u);
    if (loop) e->anim_flags |= 0x04u;
}

/* current loop slot of the playing clip (original +0x14D frame byte; the port counter is
 * monotonic, the renderer takes slot = frame % frame_count). No bank -> raw frame. */
static int re2z_frame_slot(const re15_actor_t *e)
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return (int)e->anim_frame;
    return (int)(e->anim_frame % (uint32_t)fc);
}
static int re2z_clip_done(const re15_actor_t *e)
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return 1;                                /* no bank -> never stall the FSM */
    return ((int)e->anim_frame >= fc - 1) ? 1 : 0;
}

/* the walk moan block @0x80101C44-88: gate +0x239==0; (rand&0x1F)==0 -> SE 10, else a SECOND
 * draw (rand&0x1F)==0 -> SE 11; on either hit +0x239 = 150. The draw COUNT is behaviour. */
static void re2z_walk_moan(re15_actor_t *e)
{
    if (e->re2z_cd239 != 0) return;                       /* @0x80101C44-4C */
    if ((re2z_rand() & 0x1fu) == 0u) {                    /* @0x80101C54-60 -> SE 10 */
        re2z_se(10); e->re2z_cd239 = 150;                 /* @0x80101C7C-88 */
    } else if ((re2z_rand() & 0x1fu) == 0u) {             /* @0x80101C68-74 -> SE 11 */
        re2z_se(11); e->re2z_cd239 = 150;
    }
}

/* RE1.5-mapped hit-FX stand-in (DOCUMENTED): RE2's FX system (FUN_8001bf10, packed group ids
 * like 0x0A001000 @0x80104DE0-F4) is not ported (Lane-I §3: row-format compatibility unproven).
 * The port spawns the RE1.5 room-bank blood the same way the RE1.5 bite/gore code does. */
static void re2z_blood_fx(re15_actor_t *e)
{
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500, e->x, e->y, e->z, (int16_t)e->rot_y);
}

/* FUN_800152C8(self, yaw_off) — applies the +0x144 vector yaw-rotated (self-disasm'd:
 *   lhu +0x76; += a1; RotY 0x8008e1f4; apply 0x8008dba4 on &self+0x144 @0x80015304;
 *   +0x38 += out.x @0x80015314-20, +0x40 += out.z @0x80015324-34).
 * The vector is REFILLED each tick by 0x80015e7c with the clip root DELTA (`sh v1,324(t0)`
 * @0x80015FD8-E4) — the e7c+152c8 pair is ONE delta application in the port
 * (re15_re2z_move_root). This helper only carries the EXTRA pushes computed into the vector
 * BETWEEN e7c and 152c8 (grab P8: delta.x -= 30 @0x80102CA0-AC). Rotation is linear, so the
 * split application is bit-equivalent to the in-vector addition. The bare `sh 11,324` seeds
 * (@0x80104824-28 / @0x80104DC8-CC / @0x80103614-1C) are DEAD STORES: the next 0x80015e7c
 * overwrites the vector before any 152c8 reads it — not modeled, documented. */
static void re2z_thrust(re15_actor_t *e, int spd)
{
    if (!spd) return;
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * spd) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * spd) >> 12);
}

/* ============================== ACTIVE: DECISIONS ========================================== */

/* DECISION[0] @0x80101294 (stand) — self-disasm'd 2026-08-10:
 *   a1024 = arc(PL,1024) @0x801012C4-C8; a800 = arc(PL,800) @0x801012E4;
 *   dist<0x1388 && a1024==0 && (+0x154&0x800)                  -> 0x101  @0x80101308-1C
 *   dist<0xBB8 && a800!=0 && (cfbf6&0x15) && (+0x154&0x800) && (rand&1)==0 -> 0xC01 @0x80101374-78
 *   dist<0x7D0 && a1024!=0 && (cfbf6&0x17) && (+0x154&0x800) && (rand&1)==0 -> 0xC01 @0x801013CC-D0
 * +0x154 & 0x800: an actor flag word with no identified port producer -> MAPPED 1 (OPEN). */
static void re2z_decide_stand(re15_actor_t *e, re15_actor_t *pl)
{
    uint32_t dist = e->ai_dist;
    int a1024 = re15_ai_arc_test(e, pl->x, pl->z, 1024);
    int a800  = re15_ai_arc_test(e, pl->x, pl->z,  800);
    uint16_t mv = re2z_cfbf6(pl);
    if (dist < 0x1388u && a1024 == 0)
        re15_ai_set_state_word(e, 0x101);                          /* @0x80101318-1C */
    if (dist < 0xbb8u && a800 != 0 && (mv & 0x15u) && (re2z_rand() & 1u) == 0u)
        re15_ai_set_state_word(e, 0xC01);                          /* @0x80101374-78 */
    if (dist < 0x7d0u && a1024 != 0 && (mv & 0x17u) && (re2z_rand() & 1u) == 0u)
        re15_ai_set_state_word(e, 0xC01);                          /* @0x801013CC-D0 */
}

/* DECISION[1] = the W2 ladder (@0x80101714) — now applied for real. */
static void re2z_decide_walk_apply(re15_actor_t *e, re15_actor_t *pl)
{
    re15_re2z_gates_t g; re15_re2z_decision_t d;
    re15_re2z_fill_gates(e, pl, re15_player_is_grabbed(), &g);
    int committed = re15_re2z_decide_walk(&g, &d);
    if (getenv("RE15_RE2_TRACE") && (committed || e->ai_dist < 1500u))
        fprintf(stderr, "[re2z] gates d=%u flo=%u/%u claimed=%u 21a=%04x cd23e=%u g1=%d g2=%d -> %s 0x%08x\n",
                g.dist, g.self_106, g.pl_106, g.pl_1d3, g.self_21a, g.self_23e,
                g.g1_sector_hit, g.g2_sector_hit, committed ? "COMMIT" : "none", d.word);
    if (committed)
        re15_ai_set_state_word(e, d.word);   /* every word now has a live executor below */
    /* d.claim_player (@0x80101968/@0x801019B0): the port claim = the grab pin; EXEC[3] P0 runs
     * on this SAME tick (decision-then-executor) and performs the full latch. */
}

/* ============================== ACTIVE: EXECUTORS ========================================== */

/* EXEC[0] @0x801013F4 (stand): P0 @0x80101458 clip 0 rate 0xF at a RANDOM start frame
 * (((rand&0x1F)<<8)|0xF0000|0), +0x158 = rand+150 @0x80101480-88, +0x15A = rand+300
 * @0x8010148C-90. P1 @0x80101494: advance(256); +0x15A expiry -> 1/2 moan SE 11 @0x801014CC-EC
 * (cooldown 150), reseed rand+300 @0x801014F0-FC. */
static void re2z_exec_stand(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        uint32_t r = re2z_rand();
        re2z_clip(e, 0, (int)(r & 0x1fu), 0xF, 0x100, 1);          /* @0x80101458-70 */
        e->sub_state_2 = 1;                                        /* @0x80101474-7C */
        e->re2z_t158 = (int16_t)(re2z_rand() + 150);               /* @0x80101478-88 */
        e->re2z_t15a = (int16_t)(re2z_rand() + 300);               /* @0x80101484-90 */
        return;
    }
    if (e->re2z_t15a != 0) { e->re2z_t15a--; return; }             /* @0x801014A8-B8 */
    if (e->re2z_cd239 == 0 && (re2z_rand() & 1u) != 0u) {          /* @0x801014BC-D8 */
        re2z_se(11); e->re2z_cd239 = 150;                          /* @0x801014E0-EC */
    }
    e->re2z_t15a = (int16_t)(re2z_rand() + 300);                   /* @0x801014F0-FC */
}

/* EXEC[1] @0x80101A40 (walk): P0 @0x80101A74 clip word = +0x218 | 0xF0000 (@0x80101A7C-8C) +
 * the gait seeding (the W1 machine). Per tick: moan block, the W1 turn gate, clip-driven
 * movement. Steer point +0x1C4/+0x1C6 = the RE2 navigator output; the PORT equivalent is its
 * own nav steer (re15_nav_update_steer fills e->steer_x/z each tick BEFORE this brain runs —
 * cross-zone it pathfinds exactly like FUN_8004A808's first-hop would). */
static void re2z_exec_walk(int slot, re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 1);          /* @0x80101A74-8C */
        e->sub_state_2 = 1;
        /* gait rows/timer seed themselves through re2z_gait_tick's entry tag */
    }
    re2z_walk_moan(e);                                             /* @0x80101C44-88 */
    re15_re2z_walk_turn(e, e->steer_x, e->steer_z, e->ai_dist);    /* @0x80101BAC-CAC */
    /* WALK movement = the e7c+152c8 pair (@0x80101CBC + @0x80101D60): 0x80015e7c computes the
     * PAIR-1 clip root delta into +0x144, 0x800152C8 applies it rotated -> ONE delta
     * application in the port. NOT the RE1.5 foot-lock: pair-1 clips 0/2 carry the forward
     * translation in the kf root fields (byte-read: clip 0 sx 2/47/121/209...). */
    re15_re2z_move_root(e);                                        /* e7c @0x80101CBC + 152c8 @0x80101D60 */
    (void)slot;
    /* WALK edge-fall death commits 0xA03/0xB03 (@0x80101E64/6C) — no reachable cliff geometry
     * in the RE1.5 rooms the port ships; OPEN, documented. */
}

/* EXEC[2] @0x80102260 (bump/shove contact): clip 1 @0x801022AC, SE 10 @0x801022DC; ends ->
 * 0x101. ENTRY OPEN — no port producer commits 2 yet; executor implemented for completeness. */
static void re2z_exec_bump(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                                     /* P0 (Review #11) */
        re2z_clip(e, 4 + e->re2z_walkclip, 0, 0xF, 0x100, 0);      /* Wort = 0xF0004 + walkclip ->
                                                                    * Pair-1-Clip 4/6 (`lui 0xf; ori
                                                                    * 4; lbu 536; addu; sw`
                                                                    * @0x80102290-AC) */
        e->re2z_t158 = (int16_t)((re2z_rand() & 0x3fu) + 180);     /* @0x801022A8-C0 (Draw IMMER) */
        if (e->re2z_cd239 == 0) {                                  /* @0x801022B4-BC */
            re2z_se((re2z_rand() & 1u) == 0u ? 11 : 10);           /* @0x801022C4-DC */
            e->re2z_cd239 = 150;                                   /* @0x801022E4-E8 */
        }
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x8010229C-A0 */
        return;
    }
    if (re2z_clip_done(e)) re15_ai_set_state_word(e, 0x101);       /* Exit MAPPED (P1+ nicht RE'd;
                                                                    * Entry OPEN, toter Code) */
}

/* EXEC[3] @0x801025EC — THE GRAB. Phase table @0x8010001C (10 ptrs, byte-verified ==
 * 0x801026C0/27D8/2814/2838/2968/29A4/2BE8/2C30/2C60/2EB4). s5 = crawl-bit + 2*(kind 0x11/0x17)
 * @0x8010266C-90 (crawler OPEN -> bit 0). Player side = the port's victim/cmd infra. */
static void re2z_exec_grab(re15_actor_t *e, re15_actor_t *pl)
{
    int s5 = ((e->type == 0x11) ? 2 : 0);                          /* @0x80102678-90 (0x17 not in
                                                                    * the RE1.5 family) */
    if (e->sub_state_2 <= 3) re15_re2z_player_pin();               /* hold phases pin the player */
    switch (e->sub_state_2) {
    case 0: {                                                      /* P0 @0x801026C0 */
        e->re2z_grabclip = re2z_param_grab[s5 * 2];                /* lbu 16(sp+s5*2) @0x801026C4-CC */
        e->re2z_bitefr   = re2z_param_bite[s5 * 2];                /* sp+64 pair @0x801028A0-AC */
        e->re2z_bitedmg  = re2z_param_bite[s5 * 2 + 1];            /* @0x801028F4 */
        re2z_clip(e, e->re2z_grabclip, 0, 0xF, 0x100, 0);          /* sw (0xF<<16)|clip @0x801026D8-E0 */
        /* snap-turn onto the player: 0x80015558(PL.x,PL.z,rate 2048) @0x801026E4-F8 — with
         * clamp 0x800 the compare always snaps (same math as the RE1.5 grab [0] snap). */
        e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0x0fff);
        {   /* PL+0x4 = (FUN_80015910-dir << 8)|5 @0x80102714-28 -> the port victim variant;
             * MAPPING: the direction sub collapses onto the RE1.5 front/behind selector. */
            int behind = re15_ai_facing_aligned(e, pl);
            re15_re2z_victim_begin(e, pl, behind);                 /* PL+0x1B4=self @0x80102710,
                                                                    * claim @0x8010275C-60 */
        }
        re15_re2z_grab_anchor(e, pl, (int)e->re2z_grabclip);       /* pin helper 0x80015b94
                                                                    * @0x8010270C (one anchor) */
        e->re2z_self1d3 = 15;                                      /* sb 15,467(s1) @0x8010276C-70 */
        e->ai_flags |= 1u;                                         /* port grab latch (domino infra) */
        e->grab_choreo = 1;                                        /* word0|=0x1000 @0x80102768-88 */
        if (e->re2z_cd239 == 0) {                                  /* @0x8010278C-94 */
            re2z_se((re2z_rand() & 1u) ? 10 : 11);                 /* @0x8010279C-B8 */
            e->re2z_cd239 = 150;                                   /* @0x801027BC-C0 */
        }
        /* rumble 0x800395B8(20,0,250,0) @0x801027C4-D0 — no rumble subsystem, documented. */
        e->sub_state_2 = 1;                                        /* sb 1,6(s1) @0x801026D4 */
        break;
    }
    case 1:                                                        /* P1 @0x801027D8: latch anim */
        re15_re2z_grab_rootmotion(e);                              /* pose 0x80015cb8 + advance */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* @0x80102800-10 */
        break;
    case 2:                                                        /* P2 @0x80102814 */
        re2z_clip(e, e->re2z_grabclip + 1, 0, 0, 0x100, 1);        /* bite = grab+1, PLAIN store
                                                                    * `addiu v1,1; sw` @0x80102830-34
                                                                    * -> Rate-Halbwort 0 = harter
                                                                    * Schnitt (Review #7) */
        e->re2z_t158 = 148;                                        /* struggle budget @0x80102828-2C */
        e->sub_state_2 = 3;                                        /* @0x80102820-24 */
        break;
    case 3: {                                                      /* P3 @0x80102838: bite loop */
        re15_re2z_grab_rootmotion(e);
        int mash = re15_re2z_mash();                               /* FUN_8001598C @0x80102860 */
        e->re2z_t158 = (int16_t)(e->re2z_t158 - (2 + 5 * mash));   /* -=2+5*mash @0x80102868-7C */
        if (e->re2z_t158 < 0)                                      /* bgez @0x80102884 ueberspringt
                                                                    * NUR die Phase-4-Writes — der
                                                                    * Ablauf-Tick faellt in den
                                                                    * Biss-Check DURCH (Review #6) */
            e->sub_state_2 = 4;                                    /* both sides @0x8010288C-94 */
        if (re2z_frame_slot(e) == (int)e->re2z_bitefr) {           /* +0x14D == pair[0] @0x801028A0-AC */
            re2z_se(3);                                            /* bite SE @0x801028E8-F0 */
            int r = re2z_player_damage(pl, (int)e->re2z_bitedmg);  /* FUN_800401d4 @0x801028F4-FC */
            if (r & 1) e->sub_state_2 = 4;                         /* one-save -> throw @0x80102904-1C */
            if (r & 2) {                                           /* player dead @0x80102920-50 */
                pl->state = 7;                                     /* grabbed death (port death FSM) */
                re15_re2z_victim_devour(e, re15_ai_facing_aligned(e, pl));
                                                                   /* Kill-Tick zieht FUN_80015910
                                                                    * NEU: PL-Cmd = (dir<<8)|6
                                                                    * @0x8010293C-50 — Variante
                                                                    * explizit (Review #17) */
                re15_ai_set_state_word(e, 0x601);                  /* sw 0x601 @0x80102924-38 */
            }
        }
        break;
    }
    case 4:                                                        /* P4 @0x80102968: throw-off */
        re2z_clip(e, e->re2z_grabclip + 2, 0, 7, 0x200, 0);        /* clip+2 rate 7 @0x8010297C-A0 */
        e->re2z_t158 = 0;                                          /* sh zero,344 @0x80102990 */
        e->re2z_dir16a = 0;                                        /* sb zero,362 @0x80102994 */
        e->ai_flags &= (uint16_t)~1u;                              /* port latch off (RE1.5 [4] twin) */
        re15_player_victim_throwoff();                             /* PL+0x6=4 shake-off (P3 wrote it
                                                                    * on BOTH sides @0x8010288C-94) */
        e->sub_state_2 = 5;                                        /* sb 5,6(s1) @0x8010298C */
        break;
    case 5: {                                                      /* P5 @0x801029A4: escape latch */
        if (!(e->re2z_flags21a & 0x60u) && re2z_frame_slot(e) == 3 /* @0x801029A4-C0 */
            && (s5 & 1) == 0) {                                    /* upright only @0x801029C8-CC */
            uint32_t r1 = re2z_rand(), r2 = re2z_rand();           /* @0x801029D4/DC */
            if (((r1 >> (r2 & 3u)) & 0xfu) == 0u) {                /* srav+andi @0x801029E4-F0 (1/16) */
                int dir = re15_ai_facing_aligned(e, pl) ? 1 : 0;   /* FUN_80015910 @0x80102A00 (MAPPING) */
                uint8_t d23d = (uint8_t)((unsigned)dir | (re2z_rand() & 2u)); /* @0x80102A08-1C */
                e->re2z_flags21a |= 0x40u;                         /* delay-slot sh @0x80102A28/34 */
                if (d23d & 2u)
                    e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x40u) | 0x20u); /* @0x80102A38-40 */
            }
        }
        re15_re2z_grab_rootmotion(e);                              /* pose+advance(512) @0x80102A50-68 */
        if (re2z_frame_slot(e) == 7) { e->sub_state_2 = 7; break; }/* upright cut @0x80102BD0-E4 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* @0x80102A6C-80 */
        if (e->sub_state_2 == 6) e->sub_state_2 = 7;               /* P6 @0x80102BE8 is the CRAWLER
                                                                    * exit (state 7 + HP -1); crawler
                                                                    * OPEN -> upright path only */
        break;
    }
    case 7:                                                        /* P7 @0x80102C30: double root */
        re15_re2z_move_root(e);                                    /* 2x 0x80015e7c @0x80102C3C/50
                                                                    * (delta form: one application
                                                                    * per rendered frame step) */
        e->sub_state_2 = 8;                                        /* sb 8,6(s1) @0x80102C58-5C */
        break;
    case 8: {                                                      /* P8 @0x80102C60: recover/fall */
        re15_re2z_move_root(e);                                    /* e7c @0x80102C6C fills +0x144 */
        re2z_thrust(e, -30);                                       /* +0x144.x -= 30 between e7c
                                                                    * and 152c8 (@0x80102CA0-AC):
                                                                    * throw recoil = clip delta
                                                                    * minus 30 backward per tick */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* advance @0x80102C94-A4 */
        int fell = 0;
        if (re2z_frame_slot(e) == 22) {                            /* @0x80102CB0-B8 */
            uint32_t r1 = re2z_rand(), r2 = re2z_rand();           /* @0x80102CD8/E0 */
            int fall = ((((r1 >> (r2 & 3u)) & 0xfu) ^ 2u) != 0u);  /* xori 2/sltu @0x80102CE8-D04
                                                                    * (15/16 -> falls backward; the
                                                                    * difficulty==3 branch @0x80102CC8
                                                                    * has no RE1.5 equivalent) */
            if (fall) {
                fell = 1;
                re15_ai_set_state_word(e, 0x501);                  /* sw 0x501 @0x80102D24-2C */
                e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu)); /* @0x80102D40-48 */
                e->re2z_dir16a = 1;                                /* sb 1,362 @0x80102D4C-50 */
                e->sub_state_2 = 1;                                /* sb 1,6 @0x80102D54 (skip P0 roll) */
                e->re2z_flag222 = 0;                               /* sb zero,546 @0x80102D90 */
                e->re2z_flags21a = (uint16_t)(((e->re2z_flags21a & ~0x8u) | 0x4u) | 0x202u);
                                                                   /* &=~0x8 @0x80102D60-64, |=0x4
                                                                    * @0x80102DA8-B0, |=0x202 @0x80102DBC */
                e->re2z_self1d3 |= 0x80u;                          /* ori 0x80 @0x80102D8C/DA0-A4 */
                e->re2z_f10e |= 0x2000u;                           /* +0x10E|=0x2000 @0x80102DA8/DB8-C0 */
                e->grid_id |= 0x80u;                               /* PORT-MAPPING (Review #18): der
                                                                    * flavor-blinde Damage-Resolver
                                                                    * klassifiziert ueber grid&0x80
                                                                    * (RE1.5-Zwilling @0x801022b8) */
                re2z_clip(e, 2, 0x14, 0xA, 0x200, 0);              /* word 0xA1402 @0x80102D68-7C:
                                                                    * clip 2 from frame 20, rate 10 */
                re2z_se((re2z_rand() & 1u) ? 12 : 13);             /* rand&1==0 -> 13, sonst 12
                                                                    * (@0x80102DC4-DC, Review #7) */
                e->re2z_t158 = 1;                                  /* sh 1,344 @0x80102DE4-EC */
            }
        }
        if (!fell && re2z_frame_slot(e) >= 7 && re2z_frame_slot(e) <= 24) {
            /* PARTNER-DOMINO (@0x80102DF0-EB0, selbst disassembliert — Review #1): waehrend der
             * Frames 7..24 (`addiu -7; sltiu 0x12` @0x80102DF8-FC) weckt der taumelnde Zombie
             * seinen PARTNER: Original via +0xD-Index in die Entity-Tabelle @0x800CFE14
             * (@0x80102E08-20; +0xD-Produzent nicht RE'd) -> PORT-MAPPING: der Body-Push-Kontakt
             * (contact_slot, +0x1AC-Analog). Gates byte-true: kind&0x10 (@0x80102E28-34),
             * HP>=0 (@0x80102E3C-44), !(+0x10E&1) Kriecher (@0x80102E4C-58), +0x1D3==0 ganzes
             * Byte (@0x80102E60-68), !(+0x21A&0x8) Aufsteh-Latch (@0x80102E70-7C),
             * !(+0x10E&0x2000) schon-gefallen (@0x80102E80-84). Aktion: Partner+0x4 = 0x901
             * (@0x80102E90-98), Partner+0x16B = FUN_80015910-Richtung (@0x80102E94-A8),
             * SE 4 (@0x80102E9C-A4 — HIER lebt der SE 4, nicht in P9). */
            int ps = (int)e->contact_slot;
            if (ps >= 1 && ps < RE15_ACTOR_MAX) {
                re15_actor_t *pz = &g_actors[ps];
                if (pz->active && re15_re2z_owns_type(pz->type)
                    && pz->hp >= 0
                    && !(pz->re2z_f10e & 0x1u)
                    && pz->re2z_self1d3 == 0
                    && !(pz->re2z_flags21a & 0x8u)
                    && !(pz->re2z_f10e & 0x2000u)) {
                    re15_ai_set_state_word(pz, 0x901);             /* sw 0x901 @0x80102E90-98 */
                    pz->re2z_gaitrow = (uint8_t)(re15_ai_facing_aligned(e, pz) ? 1 : 0);
                                                                   /* +0x16B = 15910-dir (MAPPING:
                                                                    * front/behind-Kollaps)
                                                                    * @0x80102E94-A8 */
                    re2z_se(4);                                    /* jal SE(4) @0x80102E9C-A4 */
                }
            }
        }
        break;
    }
    default:                                                       /* P9 @0x80102EB4-B8: exit */
        re15_ai_set_state_word(e, 0x1);                            /* `addiu v0,1; sw v0,4` ->
                                                                    * word 0x1 = STAND, KEIN SE
                                                                    * (Review #1) */
        break;
    }
}

/* EXEC[5] @0x80103188 — KNOCKDOWN-FALL (the "flinch" 0x501 commit falls the zombie). Own phase
 * table at param +0x6C (9 ptrs: 0x8010328C..0x801036F4). Phases the port models:
 *   P0 @0x8010328C: side = rand&1 -> +0x16A + orientation bit 0x4 (@0x8010329C-B4), fall clip
 *      param[0x44+side] (1/2) @0x801032C8-CC, +0x21A|=0x202, +0x222=0 @0x801032F4.
 *   P1..P5 (0x80103370..0x8010352C): fall plays out -> P6 (internals: FX/SE, unmodeled OPEN).
 *   P6 @0x80103568: phase=7 @0x80103580, +0x21A|=0x10 CRAWL MARKER @0x8010358C, ground-lie clip
 *      param[0x44+4+side] (8/9) rate 0xF @0x80103574-A8, 1/4 moan SE 11 cd 150 @0x801035F0-610.
 *   P7 @0x80103628: ground hold (shadow grower = render, skip). Duration: param u16 @0x8010004C
 *      = 90 (MAPPED as the lie time — P7's exact advance is not RE'd, OPEN).
 *   P8 @0x801036F4: commit 0x101 @0x801036F4-F8. */
static void re2z_exec_knockdown(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0: {
        int side = (int)(re2z_rand() & 1u);                        /* @0x8010328C-98 */
        e->re2z_dir16a = (uint8_t)side;
        e->re2z_flags21a = (uint16_t)(((e->re2z_flags21a & ~0x4u) | (side ? 0x4u : 0u)) | 0x202u);
        e->re2z_flag222 = 0;                                       /* @0x801032F4 */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80103304/330C */
        e->re2z_f10e |= 0x2000u;                                   /* +0x10E|=0x2000 @0x80103308/3320 */
        re2z_clip(e, re2z_param_clips[side], side * 5 + 10, 0xF, 0x200, 0);
                                                                   /* fall clip 1/2, STARTFRAME
                                                                    * side*5+10 = 10/15 (`sll 2 +
                                                                    * addu + addiu 10 + sll 8`
                                                                    * @0x80103310-1C, sw @0x80103338
                                                                    * — Review #4) */
        if (e->re2z_cd239 == 0) {                                  /* @0x8010332C-34 (Review #5) */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* ==0 -> 13, !=0 -> 12
                                                                    * @0x8010333C-58 */
            e->re2z_cd239 = 150;                                   /* @0x8010335C-60 */
        }
        e->re2z_t158 = 0;                                          /* sh zero,344 @0x80103368 */
        e->grid_id |= 0x80u;                                       /* PORT-MAPPING (Review #18):
                                                                    * Downed-Band fuer den flavor-
                                                                    * blinden Damage-Resolver
                                                                    * (RE1.5-Zwilling-Set beim Fall) */
        e->sub_state_2 = 1;                                        /* @0x801032D0-D4 */
        break;
    }
    case 1:                                                        /* fall plays out (P1..P5
                                                                    * @0x80103370-52C: FX/SE-Interna
                                                                    * OPEN) */
        if (re2z_clip_done(e)) e->sub_state_2 = 6;
        break;
    case 6:                                                        /* P6 @0x80103568 */
        e->re2z_flags21a |= 0x10u;                                 /* CRAWL MARKER @0x8010358C */
        re2z_clip(e, re2z_param_clips[4 + (e->re2z_dir16a & 1)], 0, 0xF, 0x100, 0); /* 8/9 rate 0xF
                                                                    * @0x80103574-A8; play-once —
                                                                    * P7 advanct DENSELBEN Clip zu
                                                                    * Ende (kein Loop-Flag) */
        if (e->re2z_cd239 == 0 && (re2z_rand() & 3u) == 0u) {      /* 1/4 moan @0x801035F0-FC */
            re2z_se(11); e->re2z_cd239 = 150;                      /* @0x80103604-10 */
        }
        /* +0x144 = 11 (@0x80103614-1C): dead store, see the re2z_thrust header */
        e->sub_state_2 = 7;                                        /* @0x8010357C-80 */
        break;
    case 7:                                                        /* P7 @0x80103628: Liege-Clip
                                                                    * laeuft aus (advance(256)
                                                                    * @0x801036A0-A4) — KEIN Timer
                                                                    * (der alte 90er war gemappt,
                                                                    * Review #6/[4]) */
        if (re2z_clip_done(e)) {
            e->re2z_flags21a &= (uint16_t)~0x10u;                  /* Kriech-Marker WEG @0x801036B8-BC */
            e->re2z_flags21a &= (uint16_t)~0x2u;                   /* @0x801036C8-CC */
            e->sub_state_2 = 8;                                    /* sb 8,6 @0x801036C4 */
        }
        break;
    default:                                                       /* P8 @0x801036F4 */
        /* ⛔ OPEN (Batch B1, Folge-RE — VISUELL, nicht im State-Log sichtbar): P8 committet
         * `addiu v0,zero,257` @0x801036F4 / `sw v0,4(s2)` @0x801036F8 = 0x101 DIREKT in den WALK —
         * der Zombie schnappt aus der Liege in den Gang, OHNE EXEC[9] Get-up (gemessen s1 5->1,
         * nie 9). Selbst nachdisassembliert und byte-true so uebernommen. Zu klaeren bleibt, ob der
         * Original-WALK-/STAND-Decide das Liege-Latch `+0x21A & 0x200` (gesetzt in P0
         * @0x801032F4) konsumiert und dann doch 0x901 einschiebt — dafuer decide[1] @0x80101714 auf
         * `lhu 538(s0)`-Reads scannen. Bis dahin KEIN Port-Eingriff (waere geraten). */
        re15_ai_set_state_word(e, 0x101);                          /* sw 0x101 @0x801036F4-F8 */
        e->grid_id &= (uint8_t)0x7Fu;                              /* PORT-MAPPING (Review #18):
                                                                    * Downed-Band-Clear beim
                                                                    * Aufstehen (RE1.5-Zwilling) */
        break;
    }
}

/* EXEC[6] @0x80103954 — post-kill / secondary reaction (the grab's player-death commit 0x601
 * lands here @0x80102924-38): SE 10 @0x801039F0, then back to 0x101 @0x80103B14. */
static void re2z_exec_six(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                                     /* P0 @0x801039B0 (Review #10) */
        re2z_clip(e, 0x18, 0, 0xF, 0x200, 0);                      /* sw 0xF0018,332 @0x801039B0/C8 */
        /* Anker 0x80015b94(PL, banks) @0x801039D0: richtet den Fresser am toten Spieler aus —
         * die Spieler-Seite haelt der Port ueber die Victim-FSM; OPEN, dokumentiert. */
        re2z_se((re2z_rand() & 1u) ? 10 : 11);                     /* rand&1==0 -> 11, sonst 10
                                                                    * (@0x801039D8-F0, KEIN cd-Gate) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x801039F8-A04 */
        e->sub_state_2 = 1;                                        /* @0x801039CC-D4 */
        return;
    }
    if (re2z_clip_done(e)) re15_ai_set_state_word(e, 0x101);       /* @0x80103B14 */
}

/* EXEC[7] @0x80103780 — LYING SLEEPER (spawn pose; INIT commits 0x701). Phase table param+0x94.
 *   P0 @0x801037CC: clip 23, or 22 when +0x21A&0x4 (@0x801037D0-E4); +0x1C0|=1; phase 1;
 *      !(+0x21A&0x4000) -> phase 2 @0x80103824-34.
 *   P2 @0x80103838: ground idle clip 8 (9 when bit 0x4 @0x80103864-8C), rate 0xF, random frame.
 *   WAKE (phases 3/4 @0x801038D8/0x80103900): conditions not RE'd -> MAPPED to the RE1.5 lying
 *   wake (dist < 0xBB8 && player alive, the RE1.5 sleeping decide's own gate); target = the
 *   get-up commit 0x901 (EXEC[9] — the RE2 ground-rise executor). */
static void re2z_exec_lying(re15_actor_t *e, const re15_actor_t *pl)
{
    /* WAKE-Produzent (PORT-MAPPING, dokumentiert): das Original haelt den Liege-Spawn ueber das
     * Limpet-Latch +0x10E&0x4000 in P1 (@0x8010381C-28); der EINZIGE Overlay-Clear sitzt in
     * EXEC[15] @0x80104F0C (Bank-B-Kette, skript-/EXE-seitig angestossen — Produzent nicht RE'd).
     * Der Port ersetzt den fehlenden Skript-Wecker durch das RE1.5-Naehe-Gate und laesst danach
     * die byte-true Executor-Kette laufen. */
    if ((e->re2z_f10e & 0x4000u) && e->ai_dist < 0xbb8u && pl->hp >= 0)
        e->re2z_f10e &= (uint16_t)~0x4000u;                        /* andi 0xbfff @0x80104F0C (MAPPED
                                                                    * hierher verlegt) */
    switch (e->sub_state_2) {
    case 0:                                                        /* P0 @0x801037CC */
        re2z_clip(e, (e->re2z_flags21a & 0x4u) ? 0x16 : 0x17, 0, 0, 0x100, 0);
                                                                   /* PLAIN store (`addiu 23/22; sw`
                                                                    * @0x801037CC-E4) -> Rate 0
                                                                    * (Review #7) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80103804-14 */
        e->sub_state_2 = 1;                                        /* @0x801037FC-800 */
        break;
    case 1:                                                        /* P1 @0x8010381C: Limpet-Halt */
        if (!(e->re2z_f10e & 0x4000u))                             /* lhu 270; andi 0x4000; bne
                                                                    * @0x8010381C-28 (+0x10E, NICHT
                                                                    * +0x21A — Review #15) */
            e->sub_state_2 = 2;                                    /* @0x8010382C-34 */
        break;
    case 2: {                                                      /* P2 @0x80103838 */
        int back = (int)((e->re2z_flags21a >> 2) & 1u);
        int fr = (int)(re2z_rand() & 0xfu);                        /* @0x80103838-4C */
        if (back) fr = (int)(re2z_rand() & 0xfu);                  /* ZWEITER Draw fuer Clip 9
                                                                    * (@0x80103864-78) */
        re2z_clip(e, re2z_param_clips[4 + back], fr, 0xF, 0x100, 0); /* 0xF0008/0xF0009 @0x80103840-80 */
        e->sub_state_2 = 3;                                        /* sb 3,6 @0x80103888-90 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80103884-8C */
            if ((re2z_rand() & 1u) != 0u) re2z_se(12);             /* @0x8010389C-AC */
            else if ((re2z_rand() & 1u) == 0u) re2z_se(10);        /* @0x801038B0-C0 */
            else re2z_se(11);                                      /* @0x801038C4-C8 */
            e->re2z_cd239 = 150;                                   /* @0x801038D0-D4 */
        }
        break;
    }
    case 3:                                                        /* P3 @0x801038D8: advance */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* @0x801038E4-FC */
        break;
    default:                                                       /* P4 @0x80103900 */
        re15_ai_set_state_word(e, 0x101);                          /* addiu 257; sw @0x80103900-0C */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f @0x80103914-18 */
        e->grid_id = 0;                                            /* PORT-MAPPING (Review #16): das
                                                                    * Liege-Nibble 0x88 muss beim
                                                                    * Aufstehen weg (RE1.5-Zwilling
                                                                    * enemy_ai_common.c:102), sonst
                                                                    * klassifiziert der flavor-blinde
                                                                    * Damage-Resolver den Stehenden
                                                                    * als DOWN = LEVEL-untreffbar */
        break;
    }
}

/* EXEC[8] @0x80103B74 — FEEDING (INIT spawns with 0x801 + clip 18 @0x80100AD4/DC). Feeding loop
 * clip rate 7 @0x80103C10; exit -> 0x101 exists @0x80103D94 (conditions not RE'd -> wake gate
 * MAPPED to the RE1.5 feeder wake dist < 0xFA0). Block K would enter phase 6 (eat the dead
 * player) — unreachable, see the ladder. */
static void re2z_exec_feeding(re15_actor_t *e, const re15_actor_t *pl)
{
    /* WAKE-Produzent: wie beim Lyer PORT-MAPPING (Limpet-Clear @0x80104F0C, Skript-Wecker fehlt)
     * mit dem RE1.5-Feeder-Gate; danach die byte-true Kette P1->P3->P4->P5. */
    if ((e->re2z_f10e & 0x4000u) && e->ai_dist < 0xfa0u && pl->hp >= 0)
        e->re2z_f10e &= (uint16_t)~0x4000u;
    switch (e->sub_state_2) {
    case 0:                                                        /* P0 @0x80103BE8 */
        re2z_clip(e, re2z_param_feed[re2z_rand() & 7u], 0, 7, 0x200, 0);
                                                                   /* Clip = rand&7 aus @0x801000A8
                                                                    * + 0x70000 (@0x80103BF4-C10 —
                                                                    * ROTATION 18/19/20, Review #8) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80103C04-14 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80103BE8-F0 */
        break;
    case 1:                                                        /* P1 @0x80103C18 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* advance(512)
                                                                    * @0x80103C24-3C */
        /* Fress-Tropf-FX (word0&0x10000, FX 0x13D0 @0x80103C50-90): RE2-FX-System, OPEN. */
        if (!(e->re2z_f10e & 0x4000u))                             /* lhu 270; andi 0x4000
                                                                    * @0x80103C94-A0 */
            e->sub_state_2 = 3;                                    /* sb 3,6 @0x80103CA4-AC */
        break;
    case 2:                                                        /* P2 @0x80103CB0: Re-Draw */
        re2z_clip(e, re2z_param_feed[re2z_rand() & 7u], 3, 0, 0x100, 0);
                                                                   /* Wort = clip + 0x300 (Frame 3,
                                                                    * Rate 0) @0x80103CB8-D4 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80103CC4-C8 */
        break;
    case 3:                                                        /* P3 @0x80103CD8: Aufstehen */
        re2z_clip(e, 0x15, 0, 0xF, 0x200, 0);                      /* sw 0xF0015 @0x80103CD8-EC */
        e->sub_state_2 = 4;                                        /* sb 4,6 @0x80103CDC-E0 */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f @0x80103CE4-FC */
        e->re2z_flags21a |= 0x10u;                                 /* ori 0x10 @0x80103D00-10 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80103D04-0C */
            if ((re2z_rand() & 1u) == 0u) {                        /* @0x80103D14-20 */
                if ((re2z_rand() & 1u) != 0u) re2z_se(11);         /* @0x80103D28-38 */
                else if ((re2z_rand() & 1u) != 0u) re2z_se(10);    /* @0x80103D3C-50 */
            }
            e->re2z_cd239 = 150;                                   /* @0x80103D58-5C */
        }
        break;
    case 4:                                                        /* P4 @0x80103D60 */
        if (re2z_clip_done(e)) {                                   /* advance(256) @0x80103D6C-74 */
            e->re2z_flags21a &= (uint16_t)~0x10u;                  /* andi 0xffef @0x80103D84-8C */
            e->sub_state_2 = 5;                                    /* sb 5,6 @0x80103D80 */
        }
        break;
    default:                                                       /* P5 @0x80103D90 */
        re15_ai_set_state_word(e, 0x101);                          /* addiu 257; sw @0x80103D90-94 */
        e->re2z_self1d3 &= 0x7Fu;                                  /* @0x80103D98.. */
        e->grid_id = 0;                                            /* PORT-MAPPING (Review #16) */
        break;
    }
    (void)pl;
}

/* EXEC[9] @0x80103E48 — GET-UP from the ground (target of the 0x901 commits, incl. state 8's
 * @0x8010AE9C). Clip from the param list @0x801000D8 (copied @0x80103E6C-84): belly 3 / back 4
 * by the orientation bit; SE 12 @0x80103FA8/@0x80104104; done -> 0x101 (@0x80103EE4/@0x80104144). */
static void re2z_exec_getup(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        int back = (e->re2z_flags21a >> 2) & 1;
        re2z_clip(e, re2z_param_getup[back ? 2 : 0], 0, 0xF, 0x200, 0);
        e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x10u) | 0x8u);
                                                                   /* andi 0xffef + ori 0x8
                                                                    * @0x80103F7C/88/90 — das
                                                                    * Aufsteh-Latch (Partner-
                                                                    * Domino-Gate 7) */
        if (e->re2z_cd239 == 0 && (re2z_rand() & 1u) != 0u) {      /* cd-Gate @0x80103F84-8C +
                                                                    * rand&1 @0x80103F94-A0
                                                                    * (Review #13) */
            re2z_se(12);                                           /* @0x80103FA8 */
            e->re2z_cd239 = 150;                                   /* @0x80103FB0-B4 */
        }
        e->sub_state_2 = 1;
        return;
    }
    if (re2z_clip_done(e)) {
        re15_ai_set_state_word(e, 0x101);                          /* v1=257 @0x80103EE4 */
        e->grid_id = 0;                                            /* PORT-MAPPING (Review #16/#18):
                                                                    * Liege-Nibble + Downed-Bit weg
                                                                    * beim Aufstehen */
    }
}

/* EXEC[11] @0x8010439C: clip 2, +0x21A|=0x2 @0x80104448, SE 10 @0x8010448C.
 * ENTRY OPEN (no port producer); executor implemented for completeness. */
static void re2z_exec_eleven(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                                     /* P0 (Review #12) */
        re2z_clip(e, 0x0A, 0, 0xF, 0x200, 0);                      /* Wort 0xF000A: lui 0xf im
                                                                    * P0-Delay-Slot @0x801043D8,
                                                                    * ori 0xa @0x801043F8, sw
                                                                    * @0x8010440C */
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 128);    /* 0x80015558(+0x1C4/6, 128)
                                                                    * @0x80104400-14 */
        e->re2z_t158 = 0;                                          /* sh zero,344 @0x8010442C */
        e->re2z_flag222 = 0;                                       /* sb zero,546 @0x80104430 */
        e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x4u) | 0x2u);
                                                                   /* andi 0xfffb + ori 0x2
                                                                    * @0x80104438-48 */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x8010444C-5C */
        e->re2z_f10e |= 0x2000u;                                   /* @0x80104460-70 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80104464-6C */
            re2z_se((re2z_rand() & 1u) == 0u ? 11 : 10);           /* @0x80104474-90 */
            e->re2z_cd239 = 150;                                   /* @0x80104494-98 */
        }
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80104410-18 */
        return;
    }
    if (re2z_clip_done(e)) re15_ai_set_state_word(e, 0x101);       /* Exit MAPPED; Entry OPEN */
}

/* EXEC[12] @0x80104748 — LUNGE BITE (0x0C01, blocks D/E). Fully self-disasm'd:
 *   P0 @0x801047B8: clip 0x19 rate 7 (sw 0x70019 @0x801047B8-C0); steer(PL, 32) @0x801047DC-E0;
 *      +0x158 = arc(PL,190) @0x801047F8-80C (signed +-190 or 0); SE 10 (cd 150) @0x80104810-20.
 *   P1 @0x8010482C: arc(PL,320)==0 -> +0x158=0, phase 2 @0x80104840-50; else yaw += +0x158 each
 *      tick @0x8010485C-70 + advance(512) -> done -> phase 2 @0x80104874-80.
 *   P2 @0x80104884: clip 0x1B rate 7 (sw 0x7001B) -> phase 3 @0x80104890-94.
 *   P3 @0x8010489C: steer(PL,32); root motion 0x80015e7c @0x801048B8; done OR frame 25 ->
 *      0x101 (@0x801048D8-DC / @0x801048EC-FC). NO direct damage — the only FUN_800401d4
 *      caller in the overlay is the grab; the lunge is a distance-closer. */
static void re2z_exec_lunge(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:
        re2z_clip(e, 0x19, 0, 7, 0x200, 0);                        /* @0x801047B8-C0 */
        re15_enemy_steer_point(e, pl->x, pl->z, 32);               /* @0x801047DC-E0 */
        e->re2z_t158 = (int16_t)re15_ai_arc_test(e, pl->x, pl->z, 190); /* @0x801047F8-80C */
        if (e->re2z_cd239 == 0) { re2z_se(10); e->re2z_cd239 = 150; }   /* @0x80104810-20 */
        /* +0x144 = 11 (@0x80104824-28): dead store, see the re2z_thrust header */
        e->sub_state_2 = 1;                                        /* @0x801047C4-C8 */
        break;
    case 1:
        if (re15_ai_arc_test(e, pl->x, pl->z, 320) == 0) {         /* @0x8010482C-40 */
            e->re2z_t158 = 0; e->sub_state_2 = 2;                  /* @0x80104848-50 */
        } else {
            e->rot_y = (int16_t)(((int)e->rot_y + e->re2z_t158) & 0x0fff); /* @0x8010485C-70 */
            if (re2z_clip_done(e)) e->sub_state_2 = 2;             /* advance(512) @0x80104874-80 */
        }
        break;
    case 2:
        re2z_clip(e, 0x1B, 0, 7, 0x200, 0);                        /* @0x80104884-8C */
        e->sub_state_2 = 3;                                        /* @0x80104890-94 */
        /* fallthrough into P3's steer, like @0x80104898 falls into 0x8010489C */
        /* FALLTHRU */
    case 3:
        re15_enemy_steer_point(e, pl->x, pl->z, 32);               /* @0x8010489C-A8 */
        re15_re2z_move_root(e);                                    /* e7c @0x801048B8 + 152c8
                                                                    * @0x801048E4: clip 0x1B root
                                                                    * sx 82/141/212 (byte-read)
                                                                    * = the strike dash */
        if (re2z_clip_done(e) || re2z_frame_slot(e) == 25)         /* @0x801048D4/@0x801048F0 */
            re15_ai_set_state_word(e, 0x101);                      /* sw 0x101 @0x801048DC/FC */
        break;
    }
}

/* EXEC[13] @0x80104928 — walk-style re-roll: +0x218 re-picked from the param block
 * (@0x801049E8-EC, same two-draw shift pick as INIT), then commit 1 (@0x80104988). */
static void re2z_exec_restyle(re15_actor_t *e)
{
    /* EXEC[13] @0x80104928 ist ein VOLLER Re-Init (Review #9); heute toter Code (kein
     * 0xD01-Produzent im Port), byte-true nach eigenem Disasm: */
    re15_ai_set_state_word(e, 0x1);                                /* sw 1,4 @0x80104988 */
    e->re2z_self1d3 = 0;                                           /* sb zero,467 @0x8010499C */
    e->hp = (int16_t)re2z_hp13_tbl[re2z_rand() & 0xfu];            /* HP-Re-Roll @0x8010C600[rand&0xf]
                                                                    * (`sh v1,342` @0x801049C8) */
    e->re2z_walkclip = re2z_param_walk13[re2z_rand() & 7u];        /* EIN Draw + andi 7 aus
                                                                    * @0x801000F8 (@0x801049C4-EC) —
                                                                    * NICHT der INIT-Zwei-Draw */
    e->re2z_flags21a = 0;                                          /* sh zero,538 @0x801049DC */
    e->re2z_flag222 = 0;                                           /* sb zero,546 @0x801049E4 */
    e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));          /* @0x801049E8/A14-18 */
    /* nicht modelliert (dokumentiert): Re-Bind jal 0x80028794 @0x80104984, +0x151..153=13
     * @0x801049B4-C0, +0x219-Clear @0x801049E0, Schatten-Reset 500/-1500 @0x801049FC-0x80104A28 */
}

/* EXEC[14] @0x80104D74 — SNAP BITE (0x0E01, blocks A/B/J):
 *   P0 @0x80104DB4: clip 0x11 rate 0xF (sw 0xF0011 @0x80104DB4-BC).
 *   P1 @0x80104DD0: at frame 10 (@0x80104DD0-D8): FX packed 0x0A001000 @0x80104DE0-F4 (port:
 *      RE1.5 blood stand-in, documented) + SE 5 @0x80104DFC-04; advance(256) done ->
 *      +0x4 = 1 (sw 1 @0x80104E24 -> ACTIVE sub 0) + +0x23E = 60 (sb @0x80104E28-2C). */
static void re2z_exec_snapbite(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        re2z_clip(e, 0x11, 0, 0xF, 0x100, 0);                      /* @0x80104DB4-BC */
        /* +0x144 = 11 (@0x80104DC8-CC): dead store, see the re2z_thrust header */
        e->sub_state_2 = 1;                                        /* @0x80104DC0-C4 */
        return;
    }
    if (e->sub_state_2 == 1 && re2z_frame_slot(e) >= 10) {         /* frame 10 @0x80104DD0-D8 */
        re2z_blood_fx(e);                                          /* FX 0x0A001000 @0x80104DE0-F4 */
        re2z_se(5);                                                /* @0x80104DFC-04 */
        e->sub_state_2 = 2;                                        /* (port: one-shot marker; the
                                                                    * original fires on the exact
                                                                    * +0x14D==10 frame byte) */
    }
    if (re2z_clip_done(e)) {
        re15_ai_set_state_word(e, 0x1);                            /* sw 1 @0x80104E24 */
        e->re2z_cd23e = 60;                                        /* sb 60,574 @0x80104E28-2C */
    }
}

/* ---- ACTIVE dispatcher: decision-then-executor on the SAME tick (@0x801011A8-EC) ----------- */
static void re2z_active(int slot, re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {                                      /* DECISION @0x8010C88C */
    case 0:  re2z_decide_stand(e, pl); break;                      /* 0x80101294 */
    case 1:  re2z_decide_walk_apply(e, pl); break;                 /* 0x80101714 (the ladder) */
    default: break;                                                /* stubs / not RE'd */
    }
    switch (e->sub_state_1) {                                      /* EXECUTOR @0x8010C8CC */
    case 0:  re2z_exec_stand(e); break;                            /* 0x801013F4 */
    case 1:  re2z_exec_walk(slot, e); break;                       /* 0x80101A40 */
    case 2:  re2z_exec_bump(e); break;                             /* 0x80102260 */
    case 3:  re2z_exec_grab(e, pl); break;                         /* 0x801025EC */
    case 4:  break;                                                /* 0x80103178 = jr ra (verified) */
    case 5:  re2z_exec_knockdown(e); break;                        /* 0x80103188 */
    case 6:  re2z_exec_six(e); break;                              /* 0x80103954 */
    case 7:  re2z_exec_lying(e, pl); break;                        /* 0x80103780 */
    case 8:  re2z_exec_feeding(e, pl); break;                      /* 0x80103B74 */
    case 9:  re2z_exec_getup(e); break;                            /* 0x80103E48 */
    case 10: re15_ai_set_state_word(e, 0x101); break;              /* 0x8010417C block-C hammering —
                                                                    * INERT (see fill_gates), bail */
    case 11: re2z_exec_eleven(e); break;                           /* 0x8010439C */
    case 12: re2z_exec_lunge(e, pl); break;                        /* 0x80104748 */
    case 13: re2z_exec_restyle(e); break;                          /* 0x80104928 */
    case 14: re2z_exec_snapbite(e); break;                         /* 0x80104D74 */
    case 15: re15_ai_set_state_word(e, 0x101); break;              /* 0x80104E54 bank-B chain — no
                                                                    * reachable producer, OPEN */
    default: re15_ai_set_state_word(e, 0x101); break;
    }
}

/* ---- HURT @0x80104F40 — the router ---------------------------------------------------------
 * The RE2 damage writer (EXE side) puts +0x4=2; the port reaches state 2 through the shared
 * re15_enemy_take_damage (its RE1.5 sub-state writes are ignored here — RE2 re-derives).
 * Flinch-resistance model: +0x223 seeded 16+(rand&15) at INIT (@0x80100888-9C) and on every
 * flinch (@0x801050C0-C8); the DECREMENT lives in the un-disassembled RE2-EXE damage writer ->
 * PORT MAPPING: res223 -= applied damage (tracked via the HP delta), no invented constant. */
static void re2z_grab_abort(re15_actor_t *e, re15_actor_t *pl)
{
    /* @0x80104F68-FDC (HURT) / @0x801082B0-328 (DEATH): PL-cmd==5 && PL+0x1B4==self ->
     * PL+0x4=1, claim clear (andi 0x7f @0x80104FAC / @0x801082F4), PL+0x1C0&=0xED, both
     * word0 &= ~0x1004. Port: stop the pin + release the victim FSM + drop the latches. */
    if (e->ai_flags & 1u) {
        e->ai_flags &= (uint16_t)~1u;
        e->grab_choreo = 0;                                        /* word0 &= ~0x1004 @0x80104FB8-C4 */
        if (re15_player_victim_state() == 1)                       /* mid-struggle -> free him */
            re15_player_victim_throwoff();
        pl->hit_react &= (uint8_t)~1u;                             /* PL grabbed-flag release */
    }
}

static void re2z_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    int dmg = (int)e->re2z_prev_hp - (int)e->hp;                   /* PORT: the applied damage */
    if (dmg < 0) dmg = 0;
    e->re2z_prev_hp = e->hp;

    re2z_grab_abort(e, pl);                                        /* @0x80104F68-FDC */

    /* crawler branches: +0x10E&1 -> own table 0x8010CBE8 (@0x80104FF4-500C); +0x21A&0x10 ->
     * crawler conversion 0x80107A78 (@0x8010502C). The crawler variant is NOT ported (W5) —
     * the marker stays set byte-true, the conversion is skipped (documented OPEN). */

    e->re2z_res223 = (int8_t)(e->re2z_res223 - dmg);               /* MAPPING (see header) */
    if (e->re2z_res223 > 0) {                                      /* slt/bne @0x8010506C-78 ->
                                                                    * resist, OHNE +0x222-Write
                                                                    * (der Sprung geht an 0x80105164
                                                                    * VORBEI — Review #2) */
        re15_ai_set_state_word(e, 0x101);                          /* MAPPING: the resist path's
                                                                    * per-sub reactions @0x80105250+
                                                                    * are not RE'd -> keep walking */
        return;
    }
    /* Flinch-Eligibility (@0x80105080-98, Review #2): 0x501 NUR wenn +0x222==1 ODER +0x5==1
     * (der ACTIVE-Sub, den der RE2-Damage-Writer stehen laesst — Port: re2z_prev_sub, weil das
     * geteilte take_damage +0x5 ueberschreibt). Sonst: markieren (@0x80105164) + Resist. */
    if (e->re2z_flag222 != 1 && e->re2z_prev_sub != 1) {
        e->re2z_flag222 = 1;                                       /* sb 1,546 @0x80105164 */
        re15_ai_set_state_word(e, 0x101);                          /* Resist-Pfad-MAPPING wie oben */
        return;
    }
    e->re2z_flag222 = 1;                                           /* sb 1,546 @0x801050A0 */
    re15_ai_set_state_word(e, 0x501);                              /* sw 0x501 @0x801050A4-AC */
    e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));          /* @0x801050C0-C8 */
    /* +0x1D2 ist KEIN Zaehler (fix_1d2_spec, Welle-D-Nachtrag): der EXE-Applier SETZT pro
     * Treffer `zone + 3*bracket` (Projektil @0x80047310-30/@0x80047564-80, Hitscan
     * @0x80041A8C-9C — selbst disassembliert). Port-Produzent: re15_re2_stamp_1d2
     * (re15_damage.c). Das %3-Gate hier extrahiert die ZONE: ==0 heisst ZONE 0 =
     * BEIN-Treffer blutet — nicht "jeder 3. Treffer" (das fruehere Inkrement war falsch). */
    if ((e->re2z_hits1d2 % 3u) == 0u)                              /* 0xAAAAAAAB trick @0x801050B0-E4 */
        re2z_blood_fx(e);                                          /* bone-blood args 0x1000/0x17D0
                                                                    * @0x801050E8-EC (RE1.5 stand-in) */
    /* KEIN SE auf dem Flinch-Pfad (Review #14): beide 0x501-Zweige enden via `j 0x80105418` im
     * Epilog. Der SE 9 @0x801052B4-B8 lebt im NICHT portierten Per-Sub-Reaktionsbereich hinter
     * +0x21A&0x60==0, +0x152<0 und +0x1D0&0xC0 (EXE-seitige Treffer-Quellbits ohne Port-
     * Produzent) -> OPEN, stumm. */
}

/* ---- DEATH @0x80108250 --------------------------------------------------------------------- */
static void re2z_death(re15_actor_t *e, re15_actor_t *pl)
{
    if (!(e->re2z_flags21a & 0x4000u)) {                           /* @0x80108260-6C */
        e->re2z_flags21a |= 0x4000u;                               /* kill latch @0x80108294-98 */
        /* the global kill counter 0x800D46C0++ (@0x8010827C-88) has no port equivalent */
        re2z_grab_abort(e, pl);                                    /* @0x801082B0-328, claim clear
                                                                    * @0x801082F4 */
        re2z_clip(e, 7, 0, 7, 0x200, 0);                           /* death clip 7 (@0x80108A88
                                                                    * clip family; variants OPEN) */
        e->sub_state_2 = 1;
        return;
    }
    if (re2z_clip_done(e))
        re15_ai_set_state_word(e, 0x907);                          /* sw 0x907 @0x801084DC ->
                                                                    * CORPSE (sub 9 wait handler) */
}

/* ---- CORPSE @0x8010A440 (12 subs @0x8010019C; the port runs the sub-0 init then holds — the
 * wait/gunshot-reaction subs 1..11 (@0x8010A5D8..0x8010A808) are presentation-only, no
 * revival commits exist in any of them). ------------------------------------------------------ */
static void re2z_corpse(re15_actor_t *e)
{
    if (e->hp != -1 || e->sub_state_1 != 1) {                      /* run the sub-0 init ONCE */
        re2z_clip(e, (e->re2z_flags21a & 0x4u) ? 0x16 : 0x17,      /* 22 if bit 0x4, else 23
                                                                    * @0x8010A490-BC */
                  0, 7, 0x200, 0);
        e->hp = -1;                                                /* sh -1,342 @0x8010A4D4 */
        e->sub_state_1 = 1;                                        /* sb 1,5 @0x8010A4E0 */
        e->re2z_t158 = 40;                                         /* @0x8010A514-18 */
        e->re2z_dir16a = 120;                                      /* @0x8010A508-10 */
        e->speed_h = 0;                                            /* sh zero,324 @0x8010A520-24 */
        /* corpse tint 0xBFBF10 into the model color words (@0x8010A4C0-508): render-side,
         * no port tint channel yet — OPEN, documented. */
    }
    /* hold the lying pose; anim freeze at clip end is the renderer's one-shot default */
}

/* ---- STATE 8 @0x80109CFC (11 subs @0x8010CF18) — the on-the-ground-alive family. ENTRY is
 * OPEN (the DEATH/HURT helper tails @0x80107A58/@0x80107EB8 that feed it are not RE'd); the
 * port implements the resolution so any future producer lands coherently: subs 5/6 commit the
 * get-up 0x901 (@0x8010AE9C) once the current clip finished. */
static void re2z_state8(re15_actor_t *e)
{
    if (re2z_clip_done(e))
        re15_ai_set_state_word(e, 0x901);                          /* sw 0x901 @0x8010AE9C */
}

/* ---- INIT (state 0) @0x8010065C ------------------------------------------------------------ */
static void re2z_init(int slot, re15_actor_t *e)
{
    uint8_t beh = e->grid_id;                                      /* Sce_em_set pc[3] (+0x9) */
    uint8_t sel = (uint8_t)(beh & 0x1f);                           /* RE1.5 spawn behavior nibble */
    /* Liege-Familien des RE1.5-Posenpakets, GENAU wie dort auf Bit 0x80 gegated
     * (`lbu v1,9(v0); andi v0,v1,0x80; beq -> 0x80100e30` @0x80100ca4-b0).
     *
     * ⚠ ZITAT KORRIGIERT (2026-08-17, Review-Fund F5): hier stand frueher "GENAU ZWEI
     * Liege-Familien". Unter DEMSELBEN 0x80-Gate liegen in STAGE1.BIN DREI sel-Decoder
     * (selbst disassembliert, roh @0x80100000):
     *   @0x80100cb8-ce0  sel {4,7,9}   -> +0x94 = 0x0c
     *   @0x80100cfc-d24  sel {5,8,0xa} -> +0x94 = 0x0e
     *   @0x80100d3c-d68  sel {1,3}     -> +0x94 = 0x0c (`ori v0,zero,0xc` @0x80100d54 /
     *                                     `sb v0,148(a0)` @0x80100d58)
     *                                     UND +0x5 = 5 (`ori v0,zero,0x5` @0x80100d64 /
     *                                     `sb v0,5(v1)` @0x80100d68)
     *   danach EIN gemeinsames f314 @0x80100da0 (+0x95=0 @0x80100d78, +0x8f=0 @0x80100d88)
     *   @0x80100dc0-de8  FINAL sel {4,7,9}   -> +0x94 = 0x12
     *   @0x80100e04-e2c  FINAL sel {5,8,0xa} -> +0x94 = 0x13
     * Der DRITTE Zweig bekommt KEIN FINAL — er behaelt 0x0c und traegt zusaetzlich den
     * Sub-State 5 (die RE1.5-Lane bildet ihn in enemy_ai_common.c bereits so ab).
     * Ausgeliefert erreichbar: beh 0x81 in ROOM1010/1011/1220/1221/4050/4051/5060/5061,
     * beh 0x83 in ROOM3010/3011.
     * Folge fuer den RE2-Flavor: {1,3} gehoeren zu den GEGATETEN Liege-Spawns und werden wie
     * {4,5,7,8,9,0xa} auf den Liege-Executor EXEC[7] abgebildet (PORT-OPTION, dokumentiert) —
     * sonst faellt beh 0x81/0x83 in den finalen else-Zweig, `re15_ai_set_state_word(e, 0x1)`
     * ueberschreibt den vom RE1.5-Live-INIT geseedeten sub_state_1=5, und die Leiche steht auf. */
    int lying_family = (sel == 1 || sel == 3 || sel == 4 || sel == 5 || sel == 7 ||
                        sel == 8 || sel == 9 || sel == 0x0a);
    /* the RE1.5 live-init supplies the PORT spawn data (HP row @0x8011f034, steer seed) — RE2
     * room data does not exist in RE1.5 rooms, so this is the byte-true data source. Its RE1.5
     * sub-state/motion writes are overridden below with the RE2 seeds. */
    re15_enemy_ai_live_init(slot);
    if (e->type == 0x13)                                           /* girl HP: her RE1.5 INIT row
                                                                    * (rng&0x1f)+50 @0x8010ac0c-1c */
        e->hp = (int16_t)((re15_engine_rand8() & 0x1f) + 50);

    /* RE2 INIT seeds (@0x8010065C..): */
    e->re2z_self1d3  = 0;                                          /* sb zero,467 @0x801006C8 */
    e->re2z_flags21a = 0;                                          /* sh zero,538 @0x8010087C */
    e->re2z_flag222  = 0;                                          /* sb zero,546 @0x80100884 */
    e->re2z_cd239 = 0; e->re2z_cd23e = 0;
    e->re2z_hits1d2 = 0;
    e->re2z_dir16a = 0;
    e->re2z_prev_sub = 0;
    {   /* +0x218 walk clip = walkstyle[(r1 >> (r2&3)) & 7] (@0x80100860-8C, two-draw pick) */
        uint32_t r1 = re2z_rand(), r2 = re2z_rand();
        e->re2z_walkclip = re2z_param_walk[(r1 >> (r2 & 3u)) & 7u];
    }
    e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));          /* @0x80100888-9C */
    /* +0x21A Bit 0x8000 = "dieser Zombie laeuft mit Frame-Wort-SEs (und dem Extra-Turn)" —
     * eines der beiden Sub-Gates vor dem Drittel-Takt der WALK-/BUMP-Executoren
     * (@0x80101cf4-f8 / @0x80102414-18, siehe enemy_ai_common.c re15_enemy_anim_sfx).
     * INIT-Produzent, selbst disassembliert:
     *   80100894: lbu v1,8(s2)                 ; Entity-Typ
     *   801008a0: addiu v0,zero,17             ; 0x11
     *   801008bc: bne v1,v0,0x801008d8         ; nur Typ 0x11 faellt durch
     *   801008c4: lhu v0,538(s2)
     *   801008c8: addiu v1,zero,250
     *   801008cc: sh  v1,342(s2)               ; +0x156 = HP = 250  (NICHT portiert, s.u.)
     *   801008d0: ori v0,v0,0x8000
     *   801008d4: sh  v0,538(s2)               ; +0x21A |= 0x8000
     * WICHTIG ZUM ZEITPUNKT: das ist der INIT-HANDLER (Zustand 0, Tabelle @0x8010C830) — im
     * Original wie im Port laeuft er im ERSTEN KI-TICK der Entity, NICHT beim Sce_em_set-Spawn.
     * Wer das Bit direkt nach dem Spawn liest (ohne einen re15_enemy_ai_run_all-Tick), sieht
     * korrekterweise 0.
     *
     * OPEN (nicht erfunden): @0x801008D8-0x80100950 setzt dasselbe Bit fuer 1 von 3 zufaelligen
     * Zombies, aber nur wenn das RE2-Spielglobal DAT_800cfb74 Bit 0x40 traegt
     * (`lw v0,-1164(v0)` @0x801008DC, `andi v0,v0,0x40` @0x801008E4, `beq -> 0x80100954`
     * @0x801008E8; Rest-3-Test @0x80100900-28; `+0x223 = (rand&0xf)+32` @0x80100944-4C).
     * Der Port hat kein Gegenstueck zu diesem RE2-Global -> Pfad bleibt OPEN.
     *
     * ⚠ +0x156 = 250 (@0x801008C8-CC) BEWUSST NICHT PORTIERT — begruendet, nicht vergessen:
     * +0x156 ist das HP-Halbwort (dasselbe, das re2z_corpse als `sh -1,342` @0x8010A4D4 auf -1
     * setzt und das die Death-/Hurt-Pfade lesen). Die 250 sind EINE Zeile des KOMPLETTEN
     * RE2-HP-Modells, das dieser Port bewusst nicht verwendet: der RE2-INIT wuerfelt die HP
     * sonst aus drei Tabellen — `lhu v0,-14736(at)`/`sh v0,342(s2)` @0x80100708-10 (Tabelle
     * 0x8010C670), @0x80100750-58 (0x8010C690), @0x80100784-8C (0x8010C600), je ueber einen
     * (rand>>(rand&3))&0xf-Index. Der Port nimmt die HP stattdessen aus dem RE1.5-Live-INIT
     * (HP-Zeile @0x8011f034), weil es fuer RE1.5-Raeume keine RE2-Raumdaten gibt. NUR die 250
     * nachzuziehen wuerde ein halbes Modell aufpfropfen: gemessen bekommt der ROOM1140-Typ-0x11
     * damit 250 statt 71 HP (3.5x zaeher) — eine Kampf-Balance-Aenderung ohne Beleg dafuer, dass
     * sie zum uebrigen (RE1.5-)HP-Satz passt. Zitiert, offen gefuehrt, nicht erfunden.
     *
     * NULL-STORES DESSELBEN BLOCKS (@0x801008A4-C0) — Port-Abgleich, damit die Luecke benannt ist:
     *   `sh zero,538` @0x8010087C -> re2z_flags21a = 0            PORTIERT (oben)
     *   `sb zero,546` @0x80100884 -> re2z_flag222 = 0             PORTIERT (oben)
     *   `sb v0,536`   @0x8010088C -> re2z_walkclip                PORTIERT (oben)
     *   `sb v0,547`   @0x8010089C -> re2z_res223                  PORTIERT (oben)
     *   `sb zero,537` @0x80100880 (+0x219), `sb zero,560/561` @0x801008A4/A8 (+0x230/+0x231),
     *   `sh zero,566` @0x801008AC (+0x236), `sb zero,569/570/571/572` @0x801008B0/B4/B8/C0
     *   (+0x239/+0x23A/+0x23B/+0x23C): davon hat der Port NUR +0x239 (re2z_cd239, direkt darunter
     *   genullt) und +0x219 (re2d_air219, ein HUND-Feld derselben Union — der Zombie-INIT nullt es
     *   im Original mit, der Port haelt die Flavor-Felder getrennt). +0x230/+0x231/+0x236/+0x23A/
     *   +0x23B/+0x23C haben im Port GAR KEIN Feld und auch keinen Leser -> nichts zu nullen. */
    if (e->type == 0x11) e->re2z_flags21a |= 0x8000u;              /* @0x801008BC-D4 */
    e->re2z_prev_hp = e->hp;
    e->speed_h = 0;                                                /* +0x144 spawn-clean (kein Walk-
                                                                    * Writer; Attacken saeen 11) */
    e->root_prev_kf = -1;                                          /* move_root delta re-anchor */
    e->sca_mask = 4;                                               /* upright SCA row (RE1.5 twin) */

    /* spawn-mode remap: the RE1.5 behavior nibble -> the RE2 spawn states. Die RE2-Spawns
     * schreiben +0x10E = 0x4004 (Feeder @0x80100A88-8C) / 0x4002 (Lyer @0x80100A34-38) —
     * das Limpet-Latch 0x4000 haelt die Executor-Ketten, bis der (gemappte) Wecker es loescht. */
    e->re2z_f10e = 0;
    if (sel == 6) {                                                /* feeding -> ACTIVE sub 8 */
        e->re2z_f10e = 0x4004u;                                    /* sh 0x4004,270 @0x80100A88-8C */
        re15_ai_set_state_word(e, 0x801);                          /* @0x80100AD4 */
        re2z_clip(e, 0x12, 0, 0, 0x100, 0);                        /* INIT-Seed Clip 18 PLAIN
                                                                    * (`addiu 18; sw 332`
                                                                    * @0x80100AD0-DC) — EXEC[8] P0
                                                                    * zieht naechsten Tick neu */
    } else if (sel == 8 || sel == 0x0b || sel == 0x0e ||
               (lying_family && (beh & 0x80u))) {                   /* lying -> ACTIVE sub 7 */
        /* ⛔ D15.1 — Nutzer-Report ROOM1100 "Zombies nicht komplett korrekt positioniert":
         * sel 7 (= behavior 0x87, die Slots 1/3 des Evidence-Korridors) fiel in den else-Zweig
         * und STAND aufrecht zwischen den liegenden 0x88ern (2 von 5 "Leichen").
         * Beleg: das RE1.5-Liege-Posenpaket (STAGE1.BIN roh, selbst disassembliert) — DREI
         * sel-Decoder unter EINEM 0x80-Gate (Zitat korrigiert 2026-08-17, Review-Fund F5;
         * hier stand frueher faelschlich "GENAU ZWEI Liege-Familien"):
         *   Gate  `lbu v1,9(v0); andi v0,v1,0x80; beq -> 0x80100e30`   @0x80100ca4-b0
         *   sel {4,7,9}   -> +0x94 = 0x0c  @0x80100cb8-e0   (Pose VOR dem f314)
         *   sel {5,8,0xa} -> +0x94 = 0x0e  @0x80100cfc-d24
         *   sel {1,3}     -> +0x94 = 0x0c  @0x80100d3c-58   UND +0x5 = 5 @0x80100d64-68
         *   +0x95 = 0 @0x80100d78, +0x8f = 0 @0x80100d88
         *   EIN  f314(+0x170/+0x174, a2=0, a3=0x200)        @0x80100da0
         *   sel {4,7,9}   -> +0x94 = 0x12  @0x80100dc0-e8   ← FINAL (ueberschreibt 0x0c NACH f314)
         *   sel {5,8,0xa} -> +0x94 = 0x13  @0x80100e04-e2c  ← FINAL
         *   sel {1,3}     -> KEIN FINAL, behaelt 0x0c + Sub-State 5
         * beh 0x87 -> sel 7 -> Clip 0x12, beh 0x88 -> sel 8 -> 0x13, beh 0x81/0x83 -> Clip 0x0c.
         * Der RE2-Flavor hat keine RE1.5-Clip-Indizes, bildet aber ALLE DREI Familien auf
         * denselben Liege-Executor EXEC[7] ab. sel 8 bleibt (wie bisher) ohne 0x80-Gate, damit die
         * bestehenden ROOM1140/ROOM1100-Pins unveraendert greifen; 1/3/4/5/7/9/0xa sind exakt so
         * gegated wie @0x80100cac. 0x0b/0x0e stammen aus dem ZWEITEN Decoder-Block, der NICHT auf
         * 0x80 gegated ist (`andi v0,v0,0x1f` @0x80100e44/@0x80100ec8; scd_vm.c:3001/3003 =
         * Clip 3 / 0x2A).
         * ⚠ ENTFERNT (2026-08-17): die Zusatzbedingung `&& sel != 7` schloss ausgerechnet sel 7
         * wieder aus — sie hatte KEINEN Byte-Beleg und widersprach sowohl diesem Kommentar als
         * auch @0x80100cbc/@0x80100cd4 (sel 7 steht dort explizit in der ERSTEN Familie).
         * PORT-OPTION (RE2-Flavor ist kein RE1.5-Original-Verhalten). */
        e->re2z_f10e = 0x4002u;                                    /* sh 0x4002,270 @0x80100A34-38 */
        re15_ai_set_state_word(e, 0x701);                          /* @0x801009E8-0x80100A84 */
    } else if (sel == 0x0d) {                                      /* pre-engaged -> WALK */
        re15_ai_set_state_word(e, 0x101);
    } else {
        re15_ai_set_state_word(e, 0x1);                            /* sw 1,4(s2) @0x801006AC-B8 */
    }
}

/* ---- the root tick ------------------------------------------------------------------------- */
int re15_re2z_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e->active) return 0;

    /* root prolog = the cooldown bank, BEFORE the state dispatch (@0x801004E0 reads the table):
     * +0x239 @0x8010045C-6C, +0x23E @0x80100470-80, self+0x1D3 low-7 @0x80100484-98
     * (+0x230 @0x8010049C.. has no modeled consumer). */
    if (e->re2z_cd239) e->re2z_cd239--;
    if (e->re2z_cd23e) e->re2z_cd23e--;
    if (e->re2z_self1d3 & 0x7fu)
        e->re2z_self1d3 = (uint8_t)((e->re2z_self1d3 & 0x80u) | ((e->re2z_self1d3 & 0x7fu) - 1u));

    if (e->state != 2) e->re2z_prev_hp = e->hp;                    /* HP snapshot for the HURT
                                                                    * resistance write-off */

    /* ⛔ D15.2 GEFIXT — SKRIPT-WECKER fuer die Liegenden (Nutzer-Report ROOM1100: nach dem
     * "Leichen erwachen"-Event passiert nichts). Das Weck-Signal des Spiels ist ein RE1.5-
     * MECHANISMUS und existiert in der RE2-Overlay-KI nicht:
     *   ROOM1100 sub02 fuehrt pro Zombie `Work_set(2,n); Member_set(12, 0x89 bzw. 0x8A)` aus.
     *   Member 12 ist entity+0x9 — der Sprungtabellen-Fall endet mit `j 0x80041230` @0x800411f4
     *   und dem Delay-Slot-Store **`sb a2,9(a0)` @0x800411f8**. Der Bump hebt das Liege-Nibble
     *   7/8 auf 9/10.
     *   In RE1.5 haengt an 9/10 der Dispatcher @0x8011f80c[9]/[10] = 0x801019f0, dessen DECIDE-Zeile
     *   @0x8011f9dc[0] = 0x801039fc der SOFORT-WECKER ist (Gegenstueck: 7/8 -> @0x8011f9d8[0] =
     *   0x801039f4 = `jr ra`-Stub = kein Selbstwecken). Port-Zwilling: enemy_ai_common.c case 9/10.
     *   Der RE2-Brain liest den Nibble nach dem INIT nie -> die Liegenden blieben fuer immer in
     *   s1==7 (gemessen 400+ Ticks ohne Transition).
     * PORT-MAPPING (RE2-Flavor, dokumentiert): der Bump wird hier in die byte-true RE2-Kette
     * uebersetzt — Get-up-Commit 0x901 = EXEC[9] @0x80103E48 (dasselbe Ziel, das der RE2-Liege-
     * Executor selbst nach dem Limpet-Clear ansteuert), Limpet-Latch +0x10E &= ~0x4000 loesen
     * und das Liege-Nibble wie beim regulaeren Aufstehen fallen lassen (re2z_exec_lying P4 /
     * re2z_exec_getup, beide `grid_id = 0`). Damit uebernimmt die gestaffelte sub02-Kaskade
     * (Sleep(10,20) zwischen den Bumps) dieselbe Dramaturgie wie in RE1.5.
     * ⚠ MASKE KORRIGIERT (2026-08-17, Review-Fund F4): hier stand `&= ~0x4002`. Der EINZIGE
     * +0x10E-Clear des Overlays loescht nur Bit 0x4000 —
     *   80104f0c: andi v1,v1,0xbfff
     *   80104f10: sh   v1,270(s0)
     * (eigener Scan aller 84 +0x10E-Zugriffe in EMOVL10_S0.BIN: AND-Masken nur 0xbfff
     * @0x80104F0C, 0xdfff @0x80103744/sh @0x8010374C und `andi 0xffc0; ori 0x1` @0x80104590-98).
     * Bit 0x0002 ueberlebt im Original: die Low-6-Bits von +0x10E sind der State-1-Dispatch-
     * Selektor (`lhu v0,270(a0)` @0x80101154, `andi 0x3f` @0x8010115C, Tabelle @0x8010C854)
     * und werden nach dem INIT weitergelesen (`andi 0x3f; ==6` @0x80104A80-94). Der Spawn-Seed
     * `addiu v0,zero,16386` / `sh v0,270(s2)` @0x80100A34-38 ist ein WRITE, kein Beleg fuer die
     * Umkehrung. Gegenprobe in dieser Datei: dieselbe Instruktion ist an zwei weiteren Stellen
     * korrekt als `~0x4000u` mit demselben Zitat @0x80104F0C abgebildet. */
    {   uint8_t nib = (uint8_t)(e->grid_id & 0x0fu);
        if (nib >= 9 && nib <= 10 && e->state == 1 && e->sub_state_1 == 7) {
            re15_ai_set_state_word(e, 0x901);                      /* EXEC[9] Get-up @0x80103E48 */
            e->re2z_f10e &= (uint16_t)~0x4000u;                    /* andi 0xbfff @0x80104F0C */
            e->grid_id = 0;
        }
    }

    switch (e->state) {                                            /* table @0x8010C830 */
    case 0: re2z_init(slot, e); break;                             /* 0x8010065C */
    case 1: re2z_active(slot, e, pl);
            e->re2z_prev_sub = e->sub_state_1;                     /* +0x5-Schnappschuss fuer HURTs
                                                                    * sub==1-Gate (@0x80105090-98;
                                                                    * das geteilte take_damage
                                                                    * ueberschreibt +0x5) */
            break;                                                 /* 0x8010114C */
    case 2: re2z_hurt(e, pl); break;                               /* 0x80104F40 */
    case 3: re2z_death(e, pl); break;                              /* 0x80108250 */
    case 7: re2z_corpse(e); break;                                 /* 0x8010A440 */
    case 8: re2z_state8(e); break;                                 /* 0x80109CFC */
    default: re15_ai_set_state_word(e, 0x101); break;              /* [4] EXE-shared nav / [5][6]
                                                                    * NULL -> recover to walk */
    }
    return 1;
}
