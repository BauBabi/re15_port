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
#include <string.h>   /* memset (Part-Record-Reset) */
#include "re15_actor.h"
#include "re15_math.h"       /* re15_vector_normal — MatrixNormal_0-Zwilling */
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

/* WELLE G — Modellherkunft (orthogonal zum Brain, siehe re15_ai_flavor.h). Default RE2, damit
 * der bisherige RE2-Modus UND jedes Harness mit `RE15_AI_FLAVOR=re2` byte-identisch bleiben;
 * `RE15_AI_MODELS=re15` (oder 15/0) waehlt den Hybrid headless. */
static re15_ai_models_t s_models = RE15_AI_MODELS_RE2;
static int s_models_env_read = 0;

re15_ai_models_t re15_ai_models(void)
{
    if (!s_models_env_read) {
        const char *v = getenv("RE15_AI_MODELS");
        s_models_env_read = 1;
        if (v && (strcmp(v, "re15") == 0 || strcmp(v, "RE15") == 0 ||
                  strcmp(v, "15")   == 0 || strcmp(v, "0")    == 0))
            s_models = RE15_AI_MODELS_RE15;
    }
    return s_models;
}
void re15_ai_models_set(re15_ai_models_t m) { s_models = (m == RE15_AI_MODELS_RE15)
                                                          ? RE15_AI_MODELS_RE15
                                                          : RE15_AI_MODELS_RE2;
                                              s_models_env_read = 1; }

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
static uint32_t s_re2_rng_draws = 0;   /* Port-Diagnose: Wurf-Zaehler (nur Tests lesen ihn) */
static uint32_t re2z_rand(void)
{
    uint32_t s = s_re2_rng;
    uint32_t h = (s >> 7) & 0xffu;
    uint32_t v = ((h + s) & 0xffu) | (h << 8);
    s_re2_rng  = v & 0xffffu;
    s_re2_rng_draws++;
    return v & 0xffu;
}
/* Die WURFZAHL ist Verhalten (jeder `jal 0x80015FE8` zaehlt); die Gore-Tests pinnen sie
 * differenziell (gleicher Pfad mit/ohne Zerleger-Zeile). */
uint32_t re15_re2_rand_draws(void) { return s_re2_rng_draws; }
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

/* Zombie ENEMSE bank — EMPIRISCH (bleibt eine deklarierte PORT-NAEHERUNG, siehe unten).
 * Probe ueber alle 73 dekodierten EDT-Maps: Bank 0 ist eine der Baenke, deren Live-Map JEDE
 * SE-id abdeckt, die das Zombie-Overlay ausloest (eigener jal-0x8005bd6c-Scan ueber
 * EMOVL10_S0.BIN, 2026-08-17: ids {2,3,4,5,7,8,9,10,11,12,13} an 38 Stellen, id 12 allein
 * 12x). RE15_RE2_SE_BANK uebersteuert fuer A/B-Hoerproben.
 *
 * ⛔ NUTZER-REPORT 2026-08-17 "die Zombies haben den falschen Sound" — WAS DAZU BELEGT IST:
 * Der MECHANISMUS ist jetzt byte-true disassembliert (re2_ems.c re2_enemse_select_bank,
 * FUN_80052b38 @0x80052b40-c2c + Raum-kind-Paar-Aufbau im Enemy-Spawn @0x8005728c-b8 +
 * Bank-Lader FUN_8005a09c). Er liefert die Bank NICHT aus dem Gegner-kind, sondern aus einem
 * EIGENEN Byte der RE2-RAUMDATEN:
 *     Spawn-Record +3 -> KIND   (`jal 0x8001b710` @0x800571f0; die 0x10..0x1F-Klemme
 *                        @0x8001b738-40 beweist den kind-Wertebereich; Modell-Binder
 *                        FUN_8001aaa8 liest den kind aus entity+0x8 @0x8001aac8)
 *     Spawn-Record +7 -> SOUND-ID (`lbu v0,7(v0)` @0x80057274 -> `sb v0,0x1fa(s0)` @0x80057280
 *                        -> DAT_800d8cd0/cd1 @0x8005728c-b8 -> FUN_80052b38)
 * Das sind ZWEI VERSCHIEDENE Bytes. Die Paar-Tabelle @0x800a7400 fuehrt ausserdem 15 ids
 * < 0x10 (0x01..0x0F), die im CDEMD0-kind-Raum (Minimum 0x10) gar nicht existieren — die
 * Tabelle lebt also NICHT im kind-Raum. Ein byte-true kind->Bank-Mapping kann es damit nicht
 * geben; es braeuchte die RE2-RAUMDATEN (record+7), die in diesem Repo NICHT liegen
 * (info/re2leon enthaelt nur COMMON/BIN, PL0, ZMOVIE, PSX.EXE).
 * GEGENPROBE, warum hier NICHT auf "Zeile 11 = {0x10,0x00}" umgestellt wurde: deren Live-Map
 * endet bei id 10 — die im Overlay meistgenutzten ids 11/12/13 waeren stumm. Die id-Deckung
 * ist aber ebenfalls kein Beweis (viele Baenke decken die Menge), darum bleibt es beim
 * Bestandswert und der Punkt als OPEN dokumentiert statt eine Zahl gegen eine andere zu
 * tauschen. */
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
/* [PORT-MAPPING] FUN_8001BF10(a0 = gepackte Effekt-Id, a1 = WINKEL, a2 = &Anker-Matrix,
 * a3 = &Geschwindigkeit|0). Der Port hat keinen ankergebundenen RE2-Emitter; der Stand-in ist
 * ein RE1.5-Raumbank-Spawn. Der WINKEL a1 wird 1:1 durchgereicht (Parameterwort des ESP-Slots),
 * damit der Wert im Port derselbe ist wie im Original.
 *
 * ⛔ NUTZER-REPORT 2026-08-19: "Das Blut beim Treffen erscheint am Fuss."
 * GEMESSEN (probe_re2_stagger, ROOM1140, echter Weg, geladene RE2-Bank EM010):
 *   Aktor-Wurzel (-1800,   0,-19600)      <- hier spawnte der Port
 *   Bone0 Huefte (-1800,-1166,-19600)     <- hier spawnt das Original  (dy = -1166)
 *   Bone1 Brust  (-1800,-1166,-19600)
 * Der Stand-in war POSITIONSLOS und benutzte die Aktor-Wurzel — und die liegt in RE1.5/RE2 auf
 * dem BODEN (+0x38/+0x3C/+0x40, +0x3C == Bodenhoehe). Also: Blut an den Fuessen.
 *
 * ---- DER ANKER IST EINE KNOCHEN-MATRIX (selbst disassembliert, re2leon/PSX.EXE) -------------
 * FUN_8001BF10 kopiert aus a2 GENAU 32 Byte = eine ganze PSX-MATRIX in den Effekt-Slot und
 * merkt sich zusaetzlich den ZEIGER, d.h. der Effekt HAENGT am Knochen:
 *   8001c03c: lw v0,0(a2)   8001c040: lw v1,4(a2)   8001c044: lw a0,8(a2)   8001c048: lw a1,12(a2)
 *   8001c04c: sw v0,76(t0)  8001c050: sw v1,80(t0)  8001c054: sw a0,84(t0)  8001c058: sw a1,88(t0)
 *   8001c05c: lw v0,16(a2) …                        8001c06c: sw v0,92(t0) … 8001c078: sw a1,104(t0)
 *   8001c094: sw a2,108(t0)      ; der ANKER-Zeiger (den FUN_8001CEFC spaeter vergleicht)
 * a2 ist immer `+0x198 + n*172 + 72` — der Modellblock hat Part-Stride 172 und die Part-MATRIX
 * bei +0x48 = 72 (Part 0 -> +72, Part 1 -> +244, Part 3 -> +588, Part 8 -> +1448 …).
 * Port-Zwilling der Matrix-Translation ist re15_enemy_bone_world_pos() — dieselbe QUERY-Pose,
 * die der RE2-HUND (enemy_ai_re2_dog.c re2d_fx) schon benutzt; der Zombie war der Ausreisser. */
/* Port-Diagnose (nur Tests, die Engine liest das nicht): Anker-Part + Weltposition des zuletzt
 * gespawnten Treffer-Effekts. */
static int     s_re2z_last_fx_part = -1;
static int32_t s_re2z_last_fx_pos[3];
int  re15_re2z_last_fx_part(void) { return s_re2z_last_fx_part; }
void re15_re2z_last_fx_pos(int32_t out[3])
{
    out[0] = s_re2z_last_fx_pos[0]; out[1] = s_re2z_last_fx_pos[1]; out[2] = s_re2z_last_fx_pos[2];
}

static void re2z_blood_fx_at(re15_actor_t *e, int part, int16_t yaw)
{
    int32_t p[3];
    re15_enemy_bone_world_pos(e, part, p);   /* == die Translation der Part-Matrix +0x198+n*172+72 */
    s_re2z_last_fx_part = part;
    s_re2z_last_fx_pos[0] = p[0]; s_re2z_last_fx_pos[1] = p[1]; s_re2z_last_fx_pos[2] = p[2];
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500, p[0], p[1], p[2], yaw);
}

/* Die ANKER-WAHL der drei Treffer-Emitter (Wurzel-Flinch @0x801050B0-158, Haupt-Handler-P0
 * @0x80105650-704, Stagger-P0 @0x80105D14-DBC — alle drei wortgleich):
 *     801050b0: lbu a0,466(s1)          ; +0x1D2
 *     801050b4-dc: a0 % 3               ; magisches multu 0xAAAAAAAB -> ZONE
 *     801050e4: bne a0,zero,0x8010511c
 *     801050f4: lw a2,408(s1)           ; +0x198
 *     80105110: _addiu a2,a2,244        ; ZONE 0  -> Part 1 (1*172+72)
 *     8010514c: lw s0,408(s1)
 *     80105158: _addiu a2,s0,72         ; sonst   -> Part 0 (0*172+72) */
static int re2z_blood_anchor(const re15_actor_t *e)
{
    return (((unsigned)e->re2z_hits1d2 % 3u) == 0u) ? 1 : 0;
}

static void re2z_blood_fx_dir(re15_actor_t *e, int16_t yaw)
{
    re2z_blood_fx_at(e, re2z_blood_anchor(e), yaw);
}

static void re2z_blood_fx(re15_actor_t *e)
{
    re2z_blood_fx_dir(e, (int16_t)e->rot_y);
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

/* ⛔ RESOLVER-LATCH-FREIGABE (+0x93 Bit 0) — Nutzer-Blocker 2026-08-19
 * "Bei der RE2-AI kann ich immer noch keinen Zombie treffen. Weder mit Schusswaffe noch mit
 * Messer." GEMESSEN (probe_re2_hitpath, ROOM1140, echter Weg game_step + R1/SQUARE, Ziel = der
 * stehende Zombie Slot 2 Typ 0x10):
 *   RE1.5-Flavor, Pistole, 120 Frames Dauerfeuer -> 3 TREFFER, +0x93 endet 0x80
 *   RE2  -Flavor, Pistole, 120 Frames Dauerfeuer -> 1 TREFFER, +0x93 bleibt 0x01 (fuer immer)
 *   RE1.5-Flavor, Messer  -> Treffer, +0x93 endet 0x00
 *   RE2  -Flavor, Messer  -> 1 Treffer, +0x93 bleibt 0x01
 * Nach dem ERSTEN Treffer ist der Zombie im RE2-Modus dauerhaft UNTREFFBAR.
 *
 * MECHANISMUS (selbst disassembliert, RE1.5 PSX.EXE — der Port faehrt DIESEN Resolver
 * FUN_80011f50 in BEIDEN Flavors, es gibt keinen portierten RE2-Resolver):
 *   SETZEN     80012490+ ... 800124e8: lbu v0,147(s1)
 *                             800124f0: ori v0,v0,0x1        (danach sb) = Ein-Treffer-Latch
 *   AUSSCHLUSS 800120c0: lui s4,0x300          (Maske 0x03000000 = +0x93 Bits 0|1)
 *              800120f4: lw  v0,144(s0)        (+0x90-Wort, +0x93 ist dessen High-Byte)
 *              800120fc: and v0,v0,s4
 *              80012100: beq v0,s4,0x80012124  -> Kandidat UEBERSPRUNGEN
 *   ZWEITKONTAKT 800123fc: lbu v1,147(s1) / 80012404: andi v0,v1,0x1 / 80012408: beq
 *              8001240c: ori v0,v1,0x2 / 80012410: sb v0,147(s1) / 80012418: jal 0x80011f50
 *              -> Bit 1 dazu, Rekursion auf das NAECHSTE Opfer. Ein Aktor mit Bit0 gesetzt
 *                 kassiert also NIE wieder Schaden, bis Bit 0 geloescht wird.
 *   FREIGABE (RE1.5-Overlay STAGE1.BIN, Zwilling dieser Zeile):
 *              80105f9c: lbu v0,147(v1)
 *              80105fa4: andi v0,v0,0xfe
 *              80105fac: sb  v0,147(v1)        (Stagger-Ausgang -> ACTIVE 0x10201)
 *              gleiche Freigabe im Knockdown-Ausgang 80106b8c-90 / 80106a1c-20 und im
 *              Liege-Wecker 80103b5c-68.
 *   D.h. in RE1.5 gehoert die Freigabe dem TREFFER-REAKTIONS-HANDLER: Phase 0 setzt
 *   `+0x93 |= 1` (@0x80106af4-fc / @0x80106958-60), die Endphase loescht es beim Ruecksprung
 *   nach ACTIVE.
 *
 * WARUM ES IM RE2-BRAIN FEHLTE (nachgewiesen, nicht vermutet): das RE2-Overlay hat das Feld
 * gar nicht. Eigener Voll-Scan beider RE2-Binaries nach `sb/lbu rt,147(rs)`:
 *   info/re2leon/COMMON/BIN/EMOVL10_S0.BIN : 0 Treffer
 *   info/re2leon/PSX.EXE                   : 0 Treffer
 * (RE2 hat ein anderes Entity-Layout — z.B. HP als +0x156 `sh -1,342` @0x8010A4D4 statt
 * RE1.5 +0x9a.) Das +0x93-Protokoll ist also reines RE1.5-EXE-Resolver-Eigentum, und mit
 * der Uebernahme der Zustandsmaschine durch den RE2-Brain fiel der EINZIGE Freigeber weg.
 * PORT-VERTRAG, kein geratener Wert: dieselbe Maske 0xfe (@0x80105fa4) an derselben
 * STRUKTURELLEN Stelle — dem Endausgang jeder RE2-Treffer-Reaktion zurueck nach ACTIVE. */
static void re2z_hit_latch_release(re15_actor_t *e)
{
    e->hit_react &= (uint8_t)~1u;                                  /* andi 0xfe @0x80105fa4 */
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
        /* DER SPIELER-CLAIM FAELLT SCHON WAEHREND DES STURZES — Phase 2 (`sb 3,6(s2)`
         * @0x8010340C, also der Aufschlag-Frame) loescht ihn:
         *   80103470: lbu v1,467(s2)
         *   80103484: andi v1,v1,0x7f
         *   80103490: sb  v1,467(s2)
         * Das ist der Grund, warum man einen fallenden/liegenden RE2-Zombie weiter treffen kann
         * (Kandidatenfilter `lbu 467 / bne zero` @0x80047138-40). [PORT-MAPPING] fuer die
         * STELLE: der Port fasst P1..P5 zu dieser einen Phase zusammen, der Clear laeuft also
         * ab dem ersten Sturz-Tick statt ab dem Aufschlag-Tick. */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f @0x80103484-90 */
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
        /* P8 ist der EINZIGE Ausgang der Sturz-Kette und damit der Zwilling des RE1.5-Knockdown-
         * Ausgangs case 6, der DREI Dinge zusammen macht: `-> 0x201`, `grid_id &= 0x7f` (Downed-
         * Band weg) UND `hit_react &= ~1` (@0x80106b8c-90 / @0x80106a1c-20). Der Downed-Clear
         * stand hier schon (Review #18) — die +0x93-Freigabe fehlte, und OHNE sie bleibt jeder
         * per Flinch (0x501 @0x801050A4) niedergeschlagene Zombie fuer immer untreffbar, weil
         * der Flinch-Pfad keinen Reaktions-Handler-Endausgang durchlaeuft. */
        re2z_hit_latch_release(e);                                 /* +0x93 &= 0xfe @0x80105fa4 */
        /* Die beiden Aufraeum-Stores des Original-P8, die im Port fehlten (selbst
         * nachdisassembliert 2026-08-19, 0x801036F4..0x80103754):
         *   80103718: lbu v0,467(s2)   80103724: andi v0,v0,0x7f   80103728: sb v0,467(s2)
         *   8010373c: lhu v0,270(s2)   80103744: andi v0,v0,0xdfff 8010374c: sh v0,270(s2)
         * Ohne den ersten blieb +0x1D3 nach JEDEM Sturz auf 0x80 stehen — der Zombie waere mit
         * dem Original-Trefferfilter (@0x80047138-40) fuer immer untreffbar; der zweite nimmt
         * das in P0 gesetzte Bit 0x2000 (@0x80103308/3320) wieder zurueck. */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f  @0x80103718-28 */
        e->re2z_f10e    &= (uint16_t)~0x2000u;                     /* andi 0xdfff @0x8010373C-4C */
        e->grid_id &= (uint8_t)0x7Fu;                              /* PORT-MAPPING (Review #18):
                                                                    * Downed-Band-Clear beim
                                                                    * Aufstehen (RE1.5-Zwilling
                                                                    * @0x801022b8-bc) */
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
    e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;      /* `addiu v0,zero,13`
                                                                    * @0x801049B4, `sb v0,337/338/
                                                                    * 339` @0x801049B8-C0 */
    e->re2z_hitdir1d0 = 0;                                         /* sh zero,464 @0x8010498C */
    /* nicht modelliert (dokumentiert): Re-Bind jal 0x80028794 @0x80104984, +0x219-Clear
     * @0x801049E0, Schatten-Reset 500/-1500 @0x801049FC-0x80104A28 */
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
        re2z_blood_fx_at(e, 8, (int16_t)e->rot_y);                 /* FX 0x0A001000 @0x80104DE0-F4,
                                                                    * Anker `addiu a2,a2,1448`
                                                                    * @0x80104DF8 = Part 8 (Kopf) */
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

/* ============================================================================================
 * HURT @0x80104F40 — die TREFFERREAKTION (Nutzer-Report 2026-08-17: "Sie reagieren nicht auf
 * die Schuesse auf sie."). Vollstaendig neu disassembliert 2026-08-18; die alte Fassung war an
 * drei Stellen fehlkalibriert und sprang statt in die Reaktion sofort zurueck in den Gang.
 *
 * ---- Die Wurzel FUN_80104F40(a0=entity, a1, a2) ----
 *   @0x80104F68-FDC  Grab-Abbruch (Spieler-Kommando 5 && PL+0x1B4 == self)
 *   @0x80104FE0-500C `lhu a0,270`; `andi 0x1` -> eigene 1D-Tabelle @0x8010CBE8[+0x5] (Eintrag
 *                    0 = NULL, 1..18 = 0x80107888) — der KRIECHER-Zweig.
 *   @0x80105014-38   `lhu 538`; `andi 0x10` -> FUN_80107A78 (Kriecher-Umbau), return.
 *   @0x8010503C-58   Schwelle v1 = (+0x10E & 0x40) ? ((+0x5 != 1) ? 23 : 0) : 0
 *   @0x8010505C-64   `lbu v0,6(s1)` / `bne v0,zero,0x80105168` — Flinch NUR in +0x6 == 0
 *   @0x8010506C-78   `lb v0,547(s1)` / `slt v0,v1,v0` / `bne` — Flinch nur wenn +0x223 <= v1
 *   @0x80105080-9C   `+0x222 == 1` ODER `+0x5 == 1` -> Flinch, sonst nur +0x222 = 1 @0x80105164
 *   @0x801050A0-C8   +0x222 = 1 ; +0x4(WORT) = 0x501 ; +0x223 = 16 + (rand & 15)
 *   @0x801050B0-15C  Blut: (+0x1D2 % 3) == 0 -> generisch (id 6096, ofs {0,800,0}, Block +0x198
 *                    +244), sonst am Knochen (id 4096, Block +0x198 +72, Pos aus +0x58C/590/594)
 *   @0x80105168-284  +0x21A & 2 (liegend) -> +0x4 = 0x60501, +0x16B = 1, Zerleger je +0x5
 *   @0x80105288-3D8  +0x21A & 0x60 / (s8)+0x152 < 0 / +0x1D0 & 0xC0 -> Gore-Zweig (SE 9 …)
 *   @0x801053E0-410  DISPATCH  `tbl[+0x5][+0x1D2]`, Basis 0x8010C940, Zeilen-Stride 36:
 *                       lbu v1,5(a0) ; sll v0,v1,3 ; addu v0,v0,v1 ; sll v0,v0,2
 *                       lbu v1,466(a0) ; addu v0,v0,a2 ; sll v1,v1,2 ; addu v1,v1,v0
 *                       lw v0,0(v1) ; jalr v0
 *
 * ---- DREI belegte Fehlkalibrierungen, die hier gefixt werden ----
 * (1) +0x223 ist KEIN Schadens-Akkumulator. Die Wurzel VERGLEICHT nur (@0x8010506C-78); der
 *     ABZUG steht im Haupt-Handler und ist eine TABELLEN-Konstante je Zeile:
 *         801055c4: lbu a0,547(s1)
 *         801055d0: lui at,0x8011
 *         801055d4: addu at,at,v0            ; v0 = +0x5
 *         801055d8: lbu v1,-13261(at)        ; = 0x8010CC33 + (+0x5)
 *         801055e0: subu a0,a0,v1
 *         801055ec: sb   a0,547(s1)
 *     Der frueher behauptete "nirgends dekrementiert"-Scan hat genau diesen Store uebersehen.
 * (2) +0x6 ist die REAKTIONS-PHASE, kein Treffer-Winkel (Produzent-Fix in re15_damage.c).
 * (3) +0x1D2 = ZONE + 3*BRACKET; die Basis-Zone ist 1 (Produzent-Fix in re15_damage.c).
 *
 * ---- Die Dispatch-Tabelle, selbst gedumpt (`table 0x8010c940 162`) ----
 * Zeile 0 (0x8010C940..63) enthaelt KEINE Zeiger (0x8032008C, 0x803E0096, …) — sie ist der
 * Schwanz der davorliegenden (u16,u16)-Datentabelle; Zeile 0 wird also nie dispatcht. Gueltig
 * sind die Zeilen 1..17 (Zeile 17 komplett NULL; ab 0x8010CBE8 beginnt die 1D-Kriecher-Tabelle).
 *   Zeile |  c0    c1    c2  |  c3    c4    c5  |  c6    c7    c8
 *      1  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      2  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      3  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      4  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      5  |  -     -     -   |  -     -     -   |  -     -     -
 *      6  |  -     -     -   |  -     -     -   |  -     -     -
 *      7  | 7438  66FC   -   | 7438  5BC0   -   | 7438  5438   -
 *      8  | 7438  66FC   -   | 7438  5BC0   -   | 7438  5BC0   -
 *      9  | 7438  5BC0  5BC0 | 5438  5438  5438 | 5438  5438  5438
 *     10  | 5BC0  5BC0  5BC0 | 5438  5438  5438 | 5438  5438  5438
 *     11  | 5BC0  5BC0  5BC0 | 5438  5438  5438 | 5438  5438  5438
 *     12  | 7438  703C   -   | 7438  703C   -   | 7438  703C   -
 *     13  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *     14  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *     15  | 7EF0  7EF0   -   | 7EF0  7EF0   -   | 7EF0  7EF0   -
 *     16  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *     17  |  -     -     -   |  -     -     -   |  -     -     -
 * ("-" = NULL). Spalte = zone + 3*bracket, also c0..c2 = Bracket 0, c3..c5 = 1, c6..c8 = 2;
 * innerhalb einer Gruppe zone 0/1/2. Zone 2 (c2/c5/c8) ist ausserhalb der Zeilen 9..11 NULL —
 * eine Kombination, die das Original nie erzeugt (Zone 2 braucht word0 & 0x10000000).
 *
 * ---- ZEILEN-SEMANTIK (+0x5) = DIE RE2-ATTACKEN-ID — VOLLSTAENDIG AUFGELOEST 2026-08-18 ----
 * +0x5 ist NICHT der KI-Substate: beide RE2-Applier ueberschreiben ihn pro Treffer mit der
 * TREFFER-ART: `sb s5,5(s1)` @0x80047324 (s5 = Hitcode, Basis a3) bzw. `+0x5 = (a1>>16)+1`
 * @0x80041AA0-B4. Die frueher hier als "OPEN" gefuehrte Herkunft ist jetzt zu Ende
 * disassembliert; die Kette ist LUECKENLOS und macht die Zeile zu einer WAFFEN-Id:
 *
 *  (1) EQUIP FUN_8006B000 (info/re2leon/PSX.EXE):
 *        8006b004: lbu a3,23548(a3)      ; a3 = *(u8*)0x800D5BFC  = angewaehlter Inventar-Slot
 *        8006b028: addu a2,v0,a1         ; a2 = 0x800CC1E8 + slot*4
 *        8006b034: lbu v0,-30636(at)     ; at = a2+0x10000 -> ea = 0x800D4A3C + slot*4 = ITEM-ID
 *        8006b040: sltiu v0,v0,0x14      ; Item-Id < 20  == "das ist eine Waffe"
 *        8006b088: sb a3,23544(at)       ; 0x800D5BF8 = Slot   (128 = nichts ausgeruestet)
 *        8006b09c: sb v0,23546(at)       ; 0x800D5BFA = ITEM-ID der Waffe
 *      Dass 0x800D4A3C das Inventar-Array (Stride 4, Byte0 = Item-Id) ist, belegt FUN_800696CC
 *      @0x800696E4-0x80069704: es scannt `0x800D4A3C + i*4` nach dem Byte == a0 und liefert i.
 *  (2) UEBERNAHME in die Entity FUN_8003BAF0:
 *        8003bd30: lbu v1,23544(v1)      ; 0x800D5BF8
 *        8003bd38: beq v1,128,0x8003bd50 ; nichts ausgeruestet
 *        8003bd44: lbu v0,23546(v0)      ; 0x800D5BFA
 *        8003bd4c: sh v0,270(s2)         ; +0x10E = ITEM-ID      (8003bd50: sh zero,270 sonst)
 *  (3) ANGRIFF FUN_80047C6C @0x80047EB4-F8: `v0 = +0x10E & 0xFFF`, `a1 = ((v0-1)<<16)|band`,
 *      `a2 = 0x800A68E8 + v0*24 + 0x800A6F8C[band]*8`, dann `jal FUN_800410CC`. Zwilling
 *      FUN_80048314 @0x80048444-60. -> `+0x5 = (a1>>16)+1 = ITEM-ID` @0x80041AA0-B4.
 *  => **Die Zeile IST die Item-Id der gefuehrten Waffe.** 0 = unbewaffnet, 1..19 = Waffen.
 *
 * ---- WIE VIELE ATTACKEN-IDS? GENAU 19 (1..19) — drei unabhaengige Belege ----
 *  a) `sltiu ...,0x14` @0x8006B040 (Item-Id < 20 == Waffe).
 *  b) Die Post-Hit-Handler-Tabelle @0x800A6FDC hat exakt 20 Eintraege (0..19): 0x800A6FDC +
 *     20*4 = 0x800A702C, ab dort stehen andere Daten.
 *  c) Die per-Gegnertyp-Schadensrecords `0x800A6A88[typ] + (id-1)*20` (Stride belegt
 *     @0x800413A4-B8 `v1*5*4` bzw. @0x8004723C `addiu v0,v0,-20`) sind je 0x17C = 380 Byte
 *     = 19*20 lang (0x800A412C -> 0x800A42A8 -> 0x800A4424 -> 0x800A45A0 …).
 *
 * ---- ZOMBIE-SCHADEN JE ATTACKEN-ID (0x800A412C = Typen 0x10-0x14/0x18-0x1F, selbst gedumpt) --
 * HP-Abzug = `(word0 >> (bracket*10)) & 0x3FF` (@0x80047244-54 / FUN_800410CC):
 *   id 1: 3/0/0 | 2: 16/15/14 | 3: 16/15/14 | 4: 16/15/14 | 5: 900 | 6: 900 | 7: 200/60/40
 *   id 8: 300/80/60 | 9: 200/50/10 | 10: 200/50/5 | 11: 200/50/10 | 12: 30 | 13: 16/15/14
 *   id 14: 60 | 15: 4 | 16: 15 | 17: 900 | 18: 8 | 19: 16/15/14
 *
 * ---- WARUM DIE ZEILEN 5, 6, 17 NULL SIND: SIE SIND IM ORIGINAL UNERREICHBAR ----
 * Genau diese drei Ids tragen `0x384E1384` = 900/900/900 Schaden (@0x800A412C + 4*20 / 5*20 /
 * 16*20). Kein Zombie hat 900 HP (HP-Tabelle @0x8010C600 max 118), also setzt der Applier
 * IMMER `+0x4 = 3` (DEATH) statt 2 (`lh v1,342 / bgez / sw 2,4(s1)` @0x8004727C-90) und die
 * HURT-Wurzel FUN_80104F40 laeuft dort nie. Die NULL-Zellen sind also kein Loch, sondern der
 * Beweis, dass Magnum-/Raketen-Klasse den Zombie sofort toetet.
 *
 * ---- DIE GEOMETRIE-TABELLE @0x800A68E8 (id-Stride 24, 3 Hoehen-Gruppen a 8 Byte) ------------
 * Sie bestaetigt die Waffen-KLASSEN unabhaengig vom Schaden (selbst gedumpt): id 1 hat drei
 * EIGENE Boxen je Zielhoehe (0x800A657C/0x800A63A8/0x800A64E0 = Nahkampf-Schwung); die Ids
 * 2/3/4/13 teilen sich EINE identische Box (0x800A6618/34/50 + 0x800A666C = Pistolen-Klasse);
 * 5,6 / 7,8 / 14 / 15 haben je eigene Boxen; 9,10,11,12,16,17 zeigen auf den NULL-Record
 * 0x800A6350 — diese Waffen benutzen den Kontakt-Pfad FUN_800410CC gar nicht, sondern erzeugen
 * eigene Projektil-/Flammen-Entities (Granaten, Bolzen, Flammenstrahl, Rakete).
 * ==========================================================================================*/

/* ==========================================================================================
 * DIE RE1.5-WAFFE -> RE2-ATTACKEN-ID-ZUORDNUNG (Nutzer-Vorgabe 2026-08-18: "Einzelne staerkere
 * Waffen muessen auch in Resident Evil 1.5 reagieren, unabhaengig von der RE2-KI").
 *
 * Das Problem: der Port schiesst mit dem RE1.5-Hitscan FUN_80011F50 (`+0x5 = weapon_id`
 * @0x800124BC). RE1.5-Waffen-Id und RE2-Attacken-Id sind ZWEI VERSCHIEDENE Id-Raeume — RE1.5
 * hat 22 Zeilen (0..21, Schaden @0x8006E0D0, Tester-Dispatch @0x8006E548), RE2 hat 19 Waffen
 * (1..19). Roh durchgereicht landeten 7 von 22 RE1.5-Waffen auf einer NULL-Zelle bzw. ausserhalb
 * der Tabelle (gemessen: w 0,5,6,17,19,20,21 -> gar keine Reaktion).
 *
 * Die Zuordnung unten geht nach WAFFEN-KLASSE. Belegt ist die Klasse auf BEIDEN Seiten:
 *   RE1.5: Tester-Dispatch @0x8006E548 (0x80012574 = Schusswaffe / 0x800127FC = Nahkampf /
 *          0x800128A0 = Sprengstoff), Reach @0x8006E5A0, Schaden @0x8006E0D0, Munitions-Records
 *          @0x80074C88.. (Waffen-Props @0x80074DA8, Stride 0xC).
 *   RE2:   Geometrie-Tabelle @0x800A68E8 (s.o.), Schadensrecords @0x800A412C, Poise-Kosten
 *          @0x8010CC33 und die Reaktionszeile selbst @0x8010C940.
 * Wo RE1.5 mehr Waffen einer Klasse hat als RE2 Zeilen, wird die naechstliegende Zeile derselben
 * Klasse gewaehlt — das ist eine [PORT-ZUORDNUNG, kein Byte-Beleg], je Zeile unten begruendet.
 * INVARIANTE: KEINE Waffe darf auf eine stumme Zelle fallen (re2z_row_guard unten).
 * ========================================================================================== */
enum { RE2Z_ATK_MAX = 19 };   /* Ids 1..19 (Beleg (a)/(b)/(c) oben)                             */

/* ---- DIE RE2-WAFFEN-IDS 1..19, byte-belegt aus info/re2leon/PSX.EXE --------------------------
 * Item-Definitionstabelle `{u8 maxQty, u8 pad, u8 flags, u8 nCombine, Rec *list}` mit Stride 8
 * ab 0x800A9E1C — Beleg FUN_800695B0 @0x80069600 (`lbu a0,-25057(at)` = 0x800A9E1F + id*8) und
 * @0x80069618 (`lw v1,-25056(at)` = 0x800A9E20 + id*8). `flags` Bit 7 = DAUERFEUER, gesetzt genau
 * fuer 0x0E/0x0F/0x10/0x12 — deckungsgleich mit den Dauerfeuer-Zweigen in FUN_8006A0CC
 * (@0x8006A128 id 15, @0x8006A130 id 18, @0x8006A184 id 16, @0x8006A1D0 id 14).
 * Zweiter, unabhaengiger Beleg: die Waffen-Modelle `info/re2leon/PL0/PLD/PL00W%02X.PLW` (Leon)
 * bzw. `PL01W%02X.PLW` (Claire) — PLW-Index == Item-Id, Stub-Dateien = "hat diese Waffe nicht";
 * PL01W09/0A/0B sind byte-gleich (EIN Granatwerfer, 3 Munitionsarten), PL00W01 == PL01W01 (Messer).
 *   1 Messer | 2 Handgun (Leon, 18) | 3 Handgun Browning HP (Claire, 13) | 4 Custom Handgun (18)
 *   5 Magnum (8) | 6 Custom Magnum (8) | 7 Schrotflinte (5) | 8 Custom Schrotflinte (7)
 *   9 GL Explosiv | 10 GL Brand | 11 GL Saeure | 12 Bowgun (18) | 13 Colt S.A.A. (6)
 *   14 Spark Shot (100) | 15 SMG/Ingram (100) | 16 Flammenwerfer (100) | 17 Raketenwerfer (4)
 *   18 Gatling (100) | 19 Chris-Pistole (15, nur PL0BW13.PLW)
 * Gegenprobe gegen die Zombie-Schadensrecords oben: 1 -> 3 (Messer), 5/6 -> 900 (Magnum),
 * 7/8 -> 200/300 (Schrot), 12 -> 30 (Bowgun-Bolzen), 15/18 -> 4/8 pro Schuss (Dauerfeuer),
 * 17 -> 900 (Rakete). Beide Quellen stimmen ueberein.
 *
 * ---- RE1.5-Waffen-Id (0..21) -> RE2-Attacken-Id (Zeile @0x8010C940) --------------------------
 *  w  RE1.5-Waffe            -> id  RE2-Zeile          Kriterium
 *  -- ---------------------- --- ------------------    ------------------------------------------
 *  0  unbewaffnet (dmg 0)       1  MAIN                RE2-Id 0 hat KEINE Geometrie (@0x800A68E8
 *                                                      Zeile 0 = Datenwoerter) und feuert nie; der
 *                                                      Port kann mit w=0 aber "treffen" (Schaden 0).
 *                                                      Schwaechste gueltige Zeile. [PORT-ZUORDNUNG]
 *  1  Combat Knife              1  Messer   MAIN       IDENTITAET. Beide sind die Nahkampfwaffe mit
 *                                                      dem kleinsten Schaden; RE2-Id 1 ist die
 *                                                      EINZIGE Zeile mit eigener Nahkampf-Geometrie
 *                                                      je Zielhoehe (@0x800A6900).
 *  2  Pipe (Nahkampf, dmg 24)   1  Messer   MAIN       ebenfalls Nahkampf-Tester 0x800127FC
 *                                                      (@0x8006E550), aber RE2 hat nur EINE
 *                                                      Nahkampfzeile. [PORT-ZUORDNUNG]
 *  3  Browning HP               3  Browning HP MAIN    IDENTITAET — RE2-Id 3 IST die Browning HP
 *                                                      (Claires Startpistole, Magazin 13).
 *  4  SIG P228                  2  Handgun  MAIN       die andere Standard-Pistolenzeile (Leons
 *                                                      VP70). Gleiche Poise-Kosten 0x0F wie Zeile 3
 *                                                      (@0x8010CC35/36). [PORT-ZUORDNUNG]
 *  5  Beretta M93R (3-Schuss)   4  Custom HG MAIN      Pistolenzeile mit der GROESSTEN Poise-Kosten-
 *  6  Glock 18 (Vollauto)       4  Custom HG MAIN      Konstante 0x23 = 35 (@0x8010CC37) = das
 *                                                      RE2-Pendant zu "mehr Stopping Power pro
 *                                                      Trigger-Zug" (Burst/Vollauto, RE1.5-Schaden
 *                                                      15 statt 5). [PORT-ZUORDNUNG]
 *  7  Super Redhawk (.44)       5  Magnum   (NULL)     KLASSEN-IDENTITAET. RE2 5/6 = 900 Schaden =
 *                                                      immer toedlich; RE1.5 w7 toetet ebenfalls
 *                                                      IMMER (`hp = -1` @0x800124FC fuer type<0x20,
 *                                                      alle RE2-Zombietypen 0x10..0x18 sind <0x20).
 *                                                      Die NULL-Zeile bleibt damit genau so
 *                                                      unerreichbar wie im Original.
 *  8  Remington M870            7  Schrot   7438/RAGDOLL  KLASSEN-IDENTITAET (Standard-Schrotflinte,
 *                                                      eigene breitere Geometrie 0x800A6724).
 *  9  Hand Grenade (HE)         9  GL Explosiv STAGGER Sprengstoff-Tester 0x800128A0 @0x8006E56C-74.
 * 10  Acid Grenade             11  GL Saeure  STAGGER  Die Munitionsart ist auf BEIDEN Seiten
 * 11  Incend. Grenade          10  GL Brand   STAGGER  belegt: RE1.5 Subtyp-Byte @0x80074E14+8
 *                                                      (3 = HE, 1 = Saeure, 2 = Brand) gegen RE2
 *                                                      Ammo-Recs 0x800A9D10/1C/28 (0x18 Grenade,
 *                                                      0x19 Flame, 0x1A Acid). Zeilen 10/11 sind
 *                                                      ausserdem byte-identisch.
 * 12  Ingram M10 (MP)          15  SMG      7EF0/BURN  IDENTITAET — RE2-Id 15 IST die Ingram-MP
 *                                                      (Dauerfeuer, flags 0x80 @0x800A9E96,
 *                                                      8-Frame-Takt @0x8006A128). Die 7EF0-Zeile ist
 *                                                      genau die "leichte" Reaktion, die eine
 *                                                      Dauerfeuerwaffe braucht.
 * 13  SPAS-12 (dmg 100)         8  Custom Schrot RAGDOLL  die staerkere Schrotflintenzeile
 *                                                      (300/80/60). KLASSEN-IDENTITAET.
 * 14  Flammenwerfer            16  Flammenwerfer MAIN  IDENTITAET — RE2-Id 16 IST der Flammenwerfer
 *                                                      (Munition 0x17 Fuel, flags 0x81
 *                                                      @0x800A9E9E, Dauerfeuer @0x8006A184).
 * 15  GL Explosiv               9  GL Explosiv STAGGER  wie w9/w10/w11 — im RE1.5-Original tragen
 * 16  GL Saeure                11  GL Saeure  STAGGER   Handgranate und GL-Runde derselben Sorte
 * 17  GL Brand                 10  GL Brand   STAGGER   byte-gleiche Schadenszeilen.
 * 18  Rocket Launcher          17  Rakete   (NULL)     IDENTITAET. RE2 17 = 900 = immer toedlich;
 *                                                      RE1.5 w18 = 400 Schaden > jede Zombie-HP
 *                                                      (Tabelle @0x8011F034 max 111, RE2 max 118).
 * 19  H&K MC51 (Vollauto)      18  Gatling  7EF0/BURN  die zweite Dauerfeuerzeile; byte-identisch zu
 *                                                      Zeile 15, damit MP und Sturmgewehr dieselbe
 *                                                      leichte Reaktion tragen. [PORT-ZUORDNUNG]
 * 20  Colt Python (dmg 0)      13  Colt S.A.A. MAIN    KLASSEN-IDENTITAET (Single-Action-Revolver,
 *                                                      RE2-Magazin 6). Im RE1.5-Original unfertig
 *                                                      (Schaden 0 in jeder Gegnerzeile).
 * 21  keine Waffe (tote Zeile)  1  Messer   MAIN       nicht fuehrbar (kein PLW, ARMS-Bank 0);
 *                                                      defensiver Default. [PORT-ZUORDNUNG]
 * NICHT benutzt werden die RE2-Zeilen 6 (Custom Magnum, ebenfalls NULL/900), 12 (Bowgun — RE1.5
 * hat keine Armbrust), 14 (Spark Shot — RE1.5 hat keine Elektrowaffe) und 19 (existiert als
 * Attacken-Id, liegt physisch aber schon in der 1D-Kriecher-Tabelle, s. re2z_hit_tbl).
 */
static const uint8_t re2z_row_from_weapon[22] = {
    /* 0*/  1, /* 1*/  1, /* 2*/  1, /* 3*/  3, /* 4*/  2, /* 5*/  4,
    /* 6*/  4, /* 7*/  5, /* 8*/  7, /* 9*/  9, /*10*/ 11, /*11*/ 10,
    /*12*/ 15, /*13*/  8, /*14*/ 16, /*15*/  9, /*16*/ 11, /*17*/ 10,
    /*18*/ 17, /*19*/ 18, /*20*/ 13, /*21*/  1
};

/* Der zweite Port-Erzeuger von +0x5: re15_enemy_take_damage (FUN_80012D60-Gegner-Zweig) fuettert
 * `+0x5 = re15_react_table[attack_type]` — RE1.5-REAKTIONS-CLIP-Ids (0x03..0x14), wieder ein
 * dritter Id-Raum. Kriterium hier ist die SCHADENSKLASSE aus re15_damage_table @0x8006F418
 * {10,20,1000,1000,1000,50,100,200,300,1000,0}:
 *   Typ 0/1  (10/20, Nahkampf-Angriff eines Gegners) -> RE2 1  (Nahkampfzeile)
 *   Typ 2/3/4/9 (1000 = Instakill)                   -> RE2 17 (900er-Klasse; toetet ohnehin)
 *   Typ 5/6  (50/100, Sprengstoff)                   -> RE2 9
 *   Typ 7    (200)                                   -> RE2 10
 *   Typ 8    (300)                                   -> RE2 11 (RE2-Id 8 traegt genau 300/80/60)
 *   Typ 10   (0)                                     -> RE2 1
 * [PORT-ZUORDNUNG] — im Original gibt es diesen Pfad nicht, dort kommt jede Zeile aus +0x10E. */
static const uint8_t re2z_row_from_atktype[11] = { 1, 1, 17, 17, 17, 9, 9, 10, 11, 17, 1 };

/* INVARIANTE "kein stummer Treffer": faellt die gewaehlte Zeile in der TATSAECHLICH gestempelten
 * Spalte auf NULL, obwohl der Zombie den Treffer UEBERLEBT hat (also die HURT-Wurzel wirklich
 * laeuft), wird auf die schwerste nicht-NULL-Zeile derselben Wucht ausgewichen: 8 = 7438/RAGDOLL.
 * Das kann im Original nicht passieren (die drei NULL-Zeilen gehoeren zu 900-Schaden-Waffen, s.o.);
 * im Port ist es erreichbar, weil der RE1.5-Schaden ein anderer ist (z.B. Zombietyp 0x18 mit der
 * HP-Zeile 1058 aus @0x8011F034 ueberlebt die 400 des Raketenwerfers). [PORT-SICHERUNG] */
enum { RE2Z_ROW_FALLBACK = 8 };
/* (die drei Funktionen stehen direkt hinter re2z_hit_tbl — sie lesen die Tabelle) */

/* Kosten-Tabelle je Zeile, Bytes @0x8010CC33 + (+0x5) — gelesen `lbu v1,-13261(at)` @0x801055D8.
 * Selbst gedumpt 2026-08-18 (`bytes 0x8010cc30 40`):
 *   8010cc30: 88 78 10 80 | 09 0f 0f 23 | 00 00 00 00 | 00 00 00 00
 *   8010cc40: 14 00 00 00 | 00 00 00 00 | 30 85 10 80 …
 * Index 0 faellt auf das letzte Byte des davorstehenden Zeigers 0x80107888 (0x80) und wird nie
 * gelesen (Zeile 0 dispatcht nicht). 0x8010CC40 = Index 13 = 0x14 liegt hinter dem gueltigen
 * Zeilenbereich der Kosten (13..17 sind 0/…), wird aber der Vollstaendigkeit halber gefuehrt. */
static const uint8_t re2z_hit_cost[18] = {
    0,  9, 15, 15, 35,  0,  0,  0,  0,   /* 0..8   */
    0,  0,  0,  0, 20,  0,  0,  0,  0    /* 9..17  ([13] = 0x14 @0x8010CC40) */
};

/* Handler-Id je Tabellenzelle (0 = NULL). 1 = 0x80105438, 2 = 0x80105BC0, 3 = 0x801066FC,
 * 4 = 0x8010703C, 5 = 0x80107438, 6 = 0x80107EF0 — Werte 1:1 aus dem Dump oben. */
enum { RE2ZH_NULL = 0, RE2ZH_MAIN, RE2ZH_STAGGER, RE2ZH_66FC, RE2ZH_703C, RE2ZH_7438, RE2ZH_7EF0 };
static int s_re2z_last_handler = 0;   /* Port-Diagnose: zuletzt dispatchte Zelle (Tests) */
static const uint8_t re2z_hit_tbl[19][9] = {
/* 0*/ { 0,0,0, 0,0,0, 0,0,0 },   /* existiert nicht (Datenwoerter @0x8010C940) */
/* 1*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 2*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 3*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 4*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 5*/ { 0,0,0, 0,0,0, 0,0,0 },
/* 6*/ { 0,0,0, 0,0,0, 0,0,0 },
/* 7*/ { 5,3,0, 5,2,0, 5,1,0 },
/* 8*/ { 5,3,0, 5,2,0, 5,2,0 },
/* 9*/ { 5,2,2, 1,1,1, 1,1,1 },
/*10*/ { 2,2,2, 1,1,1, 1,1,1 },
/*11*/ { 2,2,2, 1,1,1, 1,1,1 },
/*12*/ { 5,4,0, 5,4,0, 5,4,0 },
/*13*/ { 1,1,0, 1,1,0, 1,1,0 },
/*14*/ { 1,1,0, 1,1,0, 1,1,0 },
/*15*/ { 6,6,0, 6,6,0, 6,6,0 },
/*16*/ { 1,1,0, 1,1,0, 1,1,0 },
/*17*/ { 0,0,0, 0,0,0, 0,0,0 },
/*18*/ { 6,6,0, 6,6,0, 6,6,0 }    /* @0x8010CBC8, byte-identisch zu Zeile 15; col8 faellt mit dem
                                   * NULL-Eintrag [0] der 1D-Tabelle @0x8010CBE8 zusammen */
    /* Zeile 19 existiert als Attacken-Id, liegt physisch aber schon in der 1D-Kriecher-Tabelle
     * @0x8010CBE8 (0x8010C940 + 19*36 = 0x8010CBEC = 1D[1] = 0x80107888 in allen 9 Spalten).
     * Der Port stempelt sie deshalb NIE (re2z_row_from_weapon enthaelt keine 19). */
};

/* ---- die Zuordnungs-Funktionen (Tabellen + Kriterien s. Block oben) ------------------------ */
static uint8_t re2z_row_guard(unsigned row, unsigned col, int survived)
{
    if (row == 0u || row > (unsigned)RE2Z_ATK_MAX) row = 1u;      /* nie ausserhalb 1..19 */
    if (!survived) return (uint8_t)row;                           /* DEATH-Wurzel, Zeile egal */
    if (col > 8u) col = 1u;
    if (row < 19u && re2z_hit_tbl[row][col] != RE2ZH_NULL) return (uint8_t)row;
    return (uint8_t)RE2Z_ROW_FALLBACK;                            /* [PORT-SICHERUNG] */
}

uint8_t re15_re2z_row_for_weapon(unsigned weapon_id, unsigned col, int survived)
{
    unsigned row = (weapon_id < 22u) ? re2z_row_from_weapon[weapon_id] : 1u;
    return re2z_row_guard(row, col, survived);
}

uint8_t re15_re2z_row_for_atktype(unsigned attack_type, unsigned col, int survived)
{
    unsigned row = (attack_type < 11u) ? re2z_row_from_atktype[attack_type] : 1u;
    return re2z_row_guard(row, col, survived);
}

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

/* Der gemeinsame Bewegungs-/Pose-Block der Reaktions-Phasen:
 *   FUN_80015558(self, +0x1C4, +0x1C6, 16)   @0x80105714-20 / @0x80105740-4C / @0x80105890-9C
 *   FUN_80015E7C(self, +0x108, +0x17C, 0)    @0x80105580-94 (im Onset zweimal, das erste Mal mit
 *                                             kurzzeitig genulltem +0x14D @0x80105594/@0x801055AC)
 *   FUN_800152C8(self, 0)                    @0x80105B98, GEGATED auf `+0x218 == (u8)+0x14C`
 *                                             (`lbu 536` / `lbu 332` / `bne` @0x80105B84-90).
 * Port-Zwillinge: re15_enemy_steer_point == FUN_80015558, re15_re2z_move_root == das Paar
 * FUN_80015E7C + FUN_800152C8 (so in Welle B etabliert). */
static void re2z_hit_move(re15_actor_t *e)
{
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, 16);         /* @0x80105714-20 */
    if ((uint8_t)e->motion == e->re2z_walkclip)                    /* @0x80105B84-90 */
        re15_re2z_move_root(e);                                    /* @0x80105590 + @0x80105B98 */
}

/* ---- DAS OBERKOERPER-ZUCKEN: die Bone-Matrix-Injektion ------------------------------------
 * Setzt den Lean-SVECTOR, den re15_skel_compute_pose (skeleton_common.c) in DIESEM Frame auf
 * Bone 0 (nach-multipliziert) und Bone 1 (vor-multipliziert, transponiert) legt. Der Aufrufer
 * setzt ihn pro Tick neu; re15_re2z_tick loescht ihn am Tick-Anfang, damit nichts akkumuliert
 * (das Original baut die Part-Matrizen im Advance jeden Tick neu).
 * Fundstellen: @0x801057A4-838 (P1), @0x801058D4-960 (P2), @0x80106A04-3C / @0x80106CA0-D4
 * (Ragdoll). Details + Ableitung stehen am Feld re2_lean in re15_actor.h. */
static void re2z_lean_pair(re15_actor_t *e, int vx, int vy, int vz)
{
    e->re2_lean[0] = (int16_t)vx;
    e->re2_lean[1] = (int16_t)vy;
    e->re2_lean[2] = (int16_t)vz;
    e->re2_lean_on = 1;
}

/* Der Zuck-Winkel der Reaktions-Phasen. `shift3` = der zusaetzliche <<3, den NUR P1 traegt:
 *   P1 @0x801057B0-C4 : mult +0x158,(s8)+0x16B -> `sll v0,t0,3` -> `subu v0,zero,v0` -> * +0x15A
 *   P2 @0x801058E0-F0 : dieselbe Kette OHNE das `sll` (selbst nachdisassembliert 2026-08-18)
 * Die zweite Matrix ist exakt der Gegenwinkel (@0x801057EC-838 / @0x80105918-960) = Transposition. */
static int re2z_lean_angle(const re15_actor_t *e, int shift3)
{
    int prod = (int)e->re2z_t158 * (int)(int8_t)e->re2z_gaitrow;   /* +0x158 * (s8)+0x16B */
    if (shift3) prod <<= 3;                                        /* `sll v0,t0,3` @0x801057BC */
    return -(prod * (int)e->re2z_t15a);                            /* * +0x15A, negiert */
}

/* ============================================================================================
 * WELLE E — DER ZERLEGER-/GORE-ZWEIG. Vollstaendig disassembliert 2026-08-18 aus
 * info/re2leon/COMMON/BIN/EMOVL10_S0.BIN (RAW @0x80100000) + info/re2leon/PSX.EXE.
 *
 * ---- DER MODELLBLOCK +0x198 ---------------------------------------------------------------
 * Ein Array von 172-Byte-Part-Records (Stride 0xAC). Beleg: die Offsetleiter der Tinten-Schreiber
 * 112/284/456/972/1488/1660/1832/2004/2176/2348/2520 in FUN_80106128 (@0x8010627C-F4) — alle
 * exakt `n*172 + 112`; die Emitter-Anker `n*172 + 72` (@0x80106178 s1+72, @0x801061AC +1448 = 8,
 * @0x801061E0 +588 = 3, @0x80106250 +1104 = 6); und `sw zero,2580` = Part 15 (@0x801010DC).
 * Gelesene/geschriebene Felder: +0x00 Flag-Wort (Bit 0 = Part vorhanden, `lw v0,516(a2)` /
 * `andi 1` @0x801061C0-CC), +0x48 Position (Emitter-Anker), +0x70 Farbwort.
 *
 * ---- DIE PART-INDIZES = die klassische RE-Knochenreihenfolge -------------------------------
 * Gemessen an der geladenen RE2-Bank EM010 (probe_re2_gore): 15 Knochen, Elternkette
 * 0<-(-1) 1<-0 2<-1 3<-2 4<-3 5<-1 6<-5 7<-6 8<-1 9<-0 10<-9 11<-10 12<-0 13<-12 14<-13.
 * Zusammen mit dem im Port bereits belegten "Kopf = Bone 8" (re15_skel_compute_pose /
 * Plc_neck, re15_actor.h) ergibt das:
 *   0 Huefte | 1 Brust | 2 R-Oberarm | 3 R-Unterarm | 4 R-Hand | 5 L-Oberarm | 6 L-Unterarm
 *   7 L-Hand | 8 Kopf | 9 R-Oberschenkel | 10 R-Schienbein | 11 R-Fuss | 12 L-Oberschenkel
 *   13 L-Schienbein | 14 L-Fuss     (15 = der Reserve-/Stumpf-Record, @0x801010DC genullt)
 *
 * ---- WER RUFT WAS (die Zeile +0x5 = die RE2-Waffen-Id) --------------------------------------
 * Liegend-Zweig @0x80105188-284 (im HURT, `+0x21A & 2`), Haupt-Handler P0 @0x80105520-78,
 * Haupt-Handler P3 @0x80105A58-64, Stagger-P0 @0x80105DC4-F18, DEATH @0x80108444-4D4:
 *   Zeile  9 (GL Explosiv)  -> FUN_8010640C  (Russ), Gate `!(+0x10E & 0x80)`
 *   Zeile 10 (GL Brand)     -> FUN_80106128  (Verkohlung), Gate `!(+0x10E & 0x80)`, danach
 *                              `+0x21A |= 0x800` (nur im Overlay-HURT, nicht im DEATH)
 *   Zeile 11 (GL Saeure)    -> FUN_80106310  (Aetzung), KEIN Gate
 *   Zeile 14 (Spark Shot)   -> FUN_80106510  (Elektro), KEIN Gate
 *   Zeile 16 (Flammenwerfer)-> +0x23A-Zaehler; ab dem 10. Treffer (`sltiu v0,v0,0x9`
 *                              @0x80105250/@0x80105544) FUN_80106128 + `+0x21A |= 0x800`
 *   Zeile 17 (Rakete)       -> FUN_8010640C  (nur im DEATH-Zweig @0x80108490-B8)
 *
 * ---- FUN_8001BF10 / FUN_8001CEFC ------------------------------------------------------------
 * FUN_8001BF10(a0 = gepackte Effekt-Id, a1 = +0x76, a2 = &Part+0x48, a3 = &Geschwindigkeit|0)
 * belegt einen der 96 Effekt-Slots @0x800D8CF0 (Stride 124, Suche @0x8001BF70-8C).
 * FUN_8001CEFC(gruppe, sub, anker) ist das Gegenstueck: es LOESCHT alle Slots, deren
 * (gruppe,sub,anker) passen (`lbu v0,28(v1)` / `lbu v0,30(v1)` / `lw v0,108(v1)` /
 * `sb zero,0(v1)` @0x8001CF14-4C) — "die am abgerissenen Teil haengenden Effekte abschalten".
 * Der Port hat weder gepackte RE2-Gruppen noch ankergebundene Effekte: [PORT-MAPPING] je
 * Original-Emitter EIN RE1.5-Raumbank-Spawn (wie schon im Ragdoll/Rutsch), FUN_8001CEFC = No-op.
 * Die RNG-WURFZAHL ist Verhalten und wird exakt nachgezogen.
 * ========================================================================================== */

/* [PORT-MAPPING] FUN_8001BF10 — Stand-in, Id nur dokumentiert (s. Block oben). `part` ist der
 * ANKER (a2 = +0x198 + part*172 + 72); jede Fundstelle traegt ihr `@0x…` am Aufruf. */
static void re2z_gore_fx(re15_actor_t *e, int part, uint32_t packed_id)
{
    (void)packed_id;
    re2z_blood_fx_at(e, part, (int16_t)e->rot_y);
}

/* Der Modellblock beim INIT: Bit 0 ("Part vorhanden") fuer die 15 echten Parts, Part 15 leer
 * (`sw zero,2580(v0)` @0x801010DC nullt ihn im Original in jedem ACTIVE-Frame). [PORT-MAPPING]
 * fuer den Anfangswert — im Original kommt er aus dem Modell-Loader. */
static void re15_re2z_part_reset(re15_actor_t *e)
{
    /* Die Flug-/Wurf-Felder des Records (+0x38..+0xA4) und die eingefrorene Matrix (+0x48):
     * im Original baut FUN_80028368 den Block komplett neu auf, im Port ist das dieser Reset. */
    memset(e->re2z_part_v,    0, sizeof e->re2z_part_v);
    memset(e->re2z_part_rot,  0, sizeof e->re2z_part_rot);
    memset(e->re2z_part_m,    0, sizeof e->re2z_part_m);
    memset(e->re2z_part_t,    0, sizeof e->re2z_part_t);
    memset(e->re2z_part_grav, 0, sizeof e->re2z_part_grav);
    memset(e->re2z_part_st86, 0, sizeof e->re2z_part_st86);
    memset(e->re2z_part_yaw98,0, sizeof e->re2z_part_yaw98);
    memset(e->re2z_part_w9a,  0, sizeof e->re2z_part_w9a);
    memset(e->re2z_part_w9c,  0, sizeof e->re2z_part_w9c);
    memset(e->re2z_part_w9e,  0, sizeof e->re2z_part_w9e);
    memset(e->re2z_part_life, 0, sizeof e->re2z_part_life);
    memset(e->re2z_part_wa4,  0, sizeof e->re2z_part_wa4);
    for (int i = 0; i < 16; i++) e->re2z_part_blend[i] = -1;   /* +0x7A: -1 = keine Setz-Blende
                                                                * (`bltz` @0x80028D2C) */
    e->re2z_part_seeded  = 0u;
    e->re2z_part_stepped = 0u;
    e->re2z_part_frame   = 0u;
    for (int i = 0; i < 16; i++) {
        e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
        /* Farbwort-Seed: der Modell-Aufbau schreibt in JEDEN Part `+0x70 = 0x808080`
         * (`puVar8[-9] = 0x808080;` FUN_80028368 in RE2_Quellcode_V2/FUN_80028368.c:55,
         * puVar8 laeuft mit Wort-Stride 0x2b = 0xAC). 0x80 ist der PSX-neutrale
         * Modulationswert (GPU: final = tex*prim/0x80) — die Tinte MULTIPLIZIERT also,
         * sie ersetzt nicht. Frueher stand hier 0 = "keine Tinte" (Port-Sentinel);
         * mit dem Renderer-Konsumenten ist der Original-Seed die richtige Zahl. */
        e->re2z_part_tint[i]  = 0x00808080u;
        e->re2z_part_mesh[i]  = (uint8_t)i;     /* Geometrie-Zeiger zeigen initial auf den
                                                 * eigenen Part (FUN_80028368-Aufbau) */
    }
}

/* ============================================================================================
 * DIE ANZEIGE-BRUECKE DES MODELLBLOCKS  (der bisher fehlende Konsument von +0x198)
 * --------------------------------------------------------------------------------------------
 * Original: FUN_80027160 @0x80027160 (RE2-EXE, ghidra_re2_Leon.txt Z.108178-108432) ist der
 * Part-Draw-Walk. Aufruf-Konvention aus dem Aufrufer @0x80026894 belegt:
 *     80026894: lw    a1,0x198(s0)     ; a1 = Part-Array (16 Records, Stride 0xAC)
 *     80026898: lw    a2,0x0(s0)       ; a2 = Entity-Flagwort
 *     8002689c: jal   FUN_80027160
 *     800268a0: _addiu a3,s0,0x38      ; a3 = &Entity-Position
 * Part-Zahl aus `lbu s3,0x107(s1)` @0x80027354.
 *
 * (A) SICHTBARKEIT — Bit 0 des Flagworts +0x00 ist DRAW ENABLE, der Test steht in BEIDEN
 *     Schleifen und ist ein FLACHES `continue`, KEIN Baum-Schnitt:
 *     einfache Schleife                       Hauptschleife
 *       80027374: lw   a2,0x0(s2)               800273b4: lw   a2,0x0(s2)
 *       8002737c: andi v0,a2,0x1                800273bc: andi v0,a2,0x9000
 *       80027380: beq  v0,zero,0x80027394       800273c0: beq  v0,zero,0x800273e4
 *       8002738c: jal  FUN_80027434             800273c4: _andi v0,a2,0x1
 *       80027390: _addiu a3,s2,0x48             800273c8: beq  v0,zero,0x800273f8
 *       800273a4: _addiu s2,s2,0xac             800273f8: addiu s0,s0,0xac
 *                                               8002740c: _addiu s2,s2,0xac
 *     Der Record-Zeiger wird UNBEDINGT im Delay-Slot weitergeschoben (0x800273A4 /
 *     0x800273F8 / 0x8010740C) — es gibt keinen Matrix-Stack, kein Push/Pop, keine Rekursion.
 *     Ein geloeschtes Bit 0 nimmt also NICHT automatisch die Knochenkette darunter mit;
 *     die Kinder wuerden mit der (eingefrorenen) Elternmatrix weiterzeichnen.
 *
 * (A2) DIE KETTE gibt es trotzdem — als FLAG-KASKADE im Kopf BEIDER Zeichner
 *     (FUN_80027434 @0x80027470-94, wortgleich in FUN_80027ff0 @0x80028010-34):
 *       80027470: lw   v0,0x94(s1)     ; +0x94 = Zeiger auf den ELTERN-Record
 *       80027478: lw   v0,0x0(v0)      ; Eltern-Flagwort
 *       80027480: andi v1,v0,0x21      ; Bit 0 und Bit 5 isolieren
 *       80027484: li   v0,0x20
 *       80027488: bne  v1,v0,0x80027498; Eltern != (0x20 gesetzt UND 0x01 klar) -> normal
 *       80027490: j    0x80027bb8      ; sonst: Epilog = NICHTS zeichnen
 *       80027494: _sw  v1,0x0(s1)      ; und das EIGENE Flagwort wird 0x20
 *     Weil FUN_80028368 die Records ELTERN-VOR-KIND ablegt (`child[+0x74] = &parent+0x48`,
 *     `child[+0x94] = &parent` FUN_80028368.c:106-109), pflanzt sich die 0x20 im selben
 *     flachen Durchlauf durch die ganze noch aktive Kette fort. Ein Kind, dessen Bit 0
 *     bereits klar ist, wird nie betreten und gibt die 0x20 auch nicht weiter.
 *     -> abgerissener Oberschenkel nimmt Unterschenkel+Fuss NUR mit, wenn er 0x20 traegt
 *        UND sein Bit 0 geloescht ist (das tut im Original die Flugphysik FUN_80028ad8 beim
 *        Aufschlag: `*param_1 = *param_1 & 0xfffffffe`, FUN_80028ad8.c:52).
 *
 * (B) TINTE — das Farbwort +0x70 geht als CVECTOR in das GTE-RGB-Register und wird von
 *     NCCT mit dem BELEUCHTUNGSERGEBNIS MULTIPLIZIERT (nicht ersetzt):
 *       80027900: lw   s8,0x70(s1)     ; Farbwort
 *       80027ae0: move a2,s8           ; -> FUN_80027bec (Tris)  / @0x80027AFC -> FUN_80027dbc (Quads)
 *       80027c08: sw   a2,0x10(sp)     ; CVECTOR r,g,b   (LOW BYTE = R)
 *       80027c18: sb   a3,0x13(sp)     ; CVECTOR.cd = GPU-Primitiv-Code -> Byte 3 ist NICHT Farbe
 *       80027c2c: ldrgb v0             ; GTE RGB-Register
 *       80027d10: NCCT                 ; out = RGB_reg * (BK + LCM*LLM*N)
 *     Neutral ist 0x808080 (FUN_80028368.c:55) — genau der PSX-Modulationsneutralwert.
 *     Der Port rendert die Beleuchtung in DEMSELBEN Raum (render_pc.c
 *     psx_prim_to_sdl_vert: "final = (tex x prim) / 0x80", Quelle psx-spx), die Tinte ist
 *     also `prim' = prim * tint / 0x80` mit Saettigung — genau ein NCCT-Modulationsschritt.
 *
 * ⛔ NUR RE2: die Bruecke wird ausschliesslich betreten, wenn der RE2-Flavor aktiv IST, der
 *    RE2-Zombie-Brain den Typ besitzt UND der INIT-Seed gelaufen ist. Im RE1.5-Pfad liefert
 *    re15_re2z_gore_resolve() 0 und der Renderer laeuft unveraendert weiter.
 *
 * (C) DAS FREIFLIEGENDE TEIL — seit Welle G verdrahtet. Parts mit 0x4A (Arm) bzw. 0x1062 (Bein)
 *    behalten Bit 0, zeichnen also weiter, bekommen aber ueber Bit 0x40 eine EIGENE Matrix
 *    (`andi v0,s3,0x40` @0x80027498) und eine eigene Physik (Bit 0x08 -> FUN_80028DAC
 *    @0x80027B98, Bit 0x20 -> FUN_80028AD8 @0x800276A0). Beide sind unten byte-true portiert;
 *    der Einstieg ist re15_re2z_gore_part_matrix(). Damit feuert auch der KASKADEN-AUSLOESER
 *    echt: FUN_80028AD8 loescht beim zweiten Bodenkontakt Bit 0 (`and v0,v0,-2` @0x80028CA0),
 *    das Flagwort des Schienbeins bleibt 0x1062 -> `(Eltern & 0x21) == 0x20` -> der Fuss
 *    verschwindet mit. */
int re15_re2z_gore_active(const re15_actor_t *e)
{
    if (!e) return 0;
    if (re15_ai_flavor() != RE15_AI_FLAVOR_RE2) return 0;      /* RE1.5 kann hier NIE landen */
    if (!re15_re2z_owns_type(e->type)) return 0;
    return (e->re2z_part_flags[0] & 1u) != 0;                  /* INIT-Seed (part_reset) gelaufen */
}

int re15_re2z_gore_resolve(const re15_actor_t *e, const int8_t *bone_parent, int n,
                           uint8_t *out_draw, uint32_t *out_tint, uint8_t *out_mesh)
{
    if (!re15_re2z_gore_active(e) || !out_draw || !out_tint || !out_mesh) return 0;
    if (n > 16) n = 16;                                        /* Modellblock hat 16 Records */

    uint32_t fl[16];
    for (int i = 0; i < n; i++) fl[i] = e->re2z_part_flags[i];

    for (int i = 0; i < n; i++) {                              /* flacher Walk, Stride 0xAC */
        out_tint[i] = e->re2z_part_tint[i];
        out_mesh[i] = e->re2z_part_mesh[i];                    /* Stumpf-Tausch @0x8010531C-50 */
        if (!(fl[i] & 1u)) { out_draw[i] = 0; continue; }      /* andi 0x1 @0x8002737C/@0x800273C4 */
        int p = bone_parent ? (int)bone_parent[i] : -1;
        if (p >= 0 && p < n && (fl[p] & 0x21u) == 0x20u) {     /* andi 0x21 / li 0x20 @0x80027480-88 */
            fl[i] = 0x20u;                                     /* sw v1,0(s1) @0x80027494 */
            out_draw[i] = 0;                                   /* j 0x80027bb8 = Epilog */
            continue;
        }
        out_draw[i] = 1;
    }
    return 1;
}

/* ============================================================================================
 * DAS FREIFLIEGENDE TEIL — die zwei Part-Physiken (Welle G)
 * Vollstaendig disassembliert 2026-08-18:
 *   FUN_80028AD8  0x80028AD8..0x80028DA8   Wurf mit Aufschlag   (Bein, Flagwort 0x1062)
 *   FUN_80028DAC  0x80028DAC..0x80028EA0   Drift mit Ablauf     (Arm,  Flagwort 0x4A)
 *   FUN_80028F48  0x80028F48..0x8002910C   die Setz-Blende der Matrix (nur im Bein-Zweig)
 *   MatrixNormal_0 @0x8008CE30 (OuterProduct0 @0x8008DF78, VectorNormal @0x8008D424)
 *
 * ---- WER RUFT SIE, UND MIT WELCHER MATRIX ---------------------------------------------------
 * Beide haengen im ZEICHNER FUN_80027434, nicht in der KI:
 *   80027498: andi v0,s3,0x40        ; Flagbit 0x40 = "eigene Matrix"
 *   8002749c: bne  v0,zero,0x800275e4; -> Eltern-Verkettung UEBERSPRINGEN
 *   80027694: andi v0,s3,0x20
 *   800276a0: jal  0x80028ad8        ; (a0 = Record, a1 = Matrix)
 *   80027b98: jal  0x80028dac        ; nur erreichbar, wenn `param_3 & 0x18` != 0 (sonst
 *                                    ; return @0x80027... im 0x18==0-Zweig) -> Bit 0x08
 * Die Matrix IST der Record selbst: der Walk uebergibt `_addiu a3,s2,0x48` @0x80027390 bzw.
 * fuehrt `addiu s0,s2,72` @0x80027370/@0x800273B0 mit `addiu s0,s0,172` @0x800273F8 im
 * Gleichtakt zu `addiu s2,s2,172` @0x8002740C. rec+0x48 = MATRIX{ short m[3][3]; long t[3]; },
 * also m @+0x48..+0x59 und t @+0x5C/+0x60/+0x64 — genau die Offsets, die FUN_80028AD8 ueber
 * `param_2+20/24/28` und FUN_80028DAC ueber `param_1[0x17]/[0x18]/[0x19]` anfasst.
 *
 * ---- [PORT-MAPPING], je einzeln benannt ------------------------------------------------------
 *  (1) BODEN. `FUN_8004FBA0(rec+0x5C,100,0x2000,1)` @0x80028B74 ist die Raum-Boden-Sonde AN DER
 *      POSITION DES TEILS. Der Port hat in diesem Modul keine Punkt-Sonde und nimmt die
 *      Boden-Y des Aktors (e->y) — dieselbe Ersetzung, die der Ragdoll/Rutsch-Zweig mit
 *      +0x1C2/+0x232 schon macht. Fuer ein Teil, das <= 100 Einheiten weit fliegt, ist das
 *      dieselbe Ebene; auf einer Treppe waere es eine Abweichung. OFFEN, mit Adresse.
 *  (2) WAND. `FUN_8004C1BC(rec+0x5C,100,1<<(-(Y/1800)&0x1f),0x2000)` @0x80028CDC prallt das Teil
 *      an der Raumgeometrie ab (`v>>2`, dann Vorzeichenwechsel @0x80028CEC-D20). Kein
 *      Port-Zwilling -> der Zweig ist NICHT modelliert. OFFEN, mit Adresse.
 *  (3) ROTATION. `RotMatrix` @0x8008E1F4 (PsyQ) wird durch den byte-true Port-Builder
 *      mat3_from_euler (skeleton_common.c, Zwilling von FUN_8001F3BC) ersetzt — dieselbe
 *      Trig-Tabelle, dieselbe Q12-Kette, die der ganze Port fuer Knochenmatrizen benutzt.
 *  (4) MatrixNormal_0 @0x8008CE30 ist strukturell nachgebaut (r2 = r0 x r1 @0x8008CE8C,
 *      r0 = r1 x r2 @0x8008CE9C, dann VectorNormal auf alle drei Zeilen @0x8008CEA8/B4/C0).
 *      Der Zeilen-Normalisierer ist re15_vector_normal — der bereits vorhandene byte-true
 *      libgte-VectorNormal-Zwilling (RE1.5-Link-Adresse 0x80066A30, RE2 0x8008D424, dieselbe
 *      Bibliotheksroutine). Die GTE-interne Rundung des OP-Befehls ist damit nicht bit-exakt
 *      reproduziert; das ist eine PRAEZISIONS-, keine Mechanismus-Abweichung.
 *  (5) SKALIERUNG. Der `flags & 0x10`-Zweig @0x80028C54-80 schreibt zusaetzlich +0x8C/+0x90 =
 *      7000 und +0x8E = 0 — die drei Skalierungs-Halbwoerter, die der Zeichner ueber
 *      `param_3 & 0x400` / `& 0x2000` in ScaleMatrix schiebt. Der Port hat keine Part-Skalierung;
 *      der Zweig setzt hier nur die belegten Flag-/Zustandsfelder. OFFEN, mit Adresse.
 * ========================================================================================== */

/* RotMatrix-Zwilling (mat3_from_euler in skeleton_common.c). */
extern void re15_skel_euler_matrix_for_test(int ax, int ay, int az, int32_t m[9]);

/* FUN_80028F48 @0x80028F48 + MatrixNormal_0 @0x8008CE30 — die Setz-Blende:
 *   80028f4c: addiu v1,zero,4096
 *   80028f50: subu  v1,v1,a3          ; w = 4096 - t   auf die ALTE Matrix
 *   80028f58: mtc2  v1,IR0 / ... / gpf12 (0x4b98003d)   ; IR = w * M
 *   80028f80: mtc2  a3,IR0 / ... / gpl12 (0x4ba8003e)   ; IR += t * R
 * elementweise ueber alle NEUN Kurzwoerter (Bloecke @0x80028F5C-FBC, @0x80028FC4-9030,
 * @0x80029034-90E4), danach `jal 0x8008ce30` @0x80029108. */
static void re2z_part_settle(re15_actor_t *e, int i)
{
    int32_t R[9];
    re15_skel_euler_matrix_for_test((int)e->re2z_part_rot[i][0],      /* +0x3E @0x80028D34 */
                                    (int)e->re2z_part_rot[i][1],      /* +0x40 @0x80028D40 */
                                    (int)e->re2z_part_rot[i][2], R);  /* +0x42 @0x80028D4C */
    int32_t t = 4096 - ((int32_t)e->re2z_part_blend[i] << 8);         /* 4096 - c*256
                                                                       * @0x80028D6C-78 */
    int32_t w = 4096 - t;                                             /* @0x80028F4C-50 */
    int16_t *m = e->re2z_part_m[i];
    for (int k = 0; k < 9; k++)
        m[k] = (int16_t)((((int32_t)m[k] * w) + (R[k] * t)) >> 12);   /* gpf12 + gpl12 */

    /* MatrixNormal_0 @0x8008CE30 */
    int32_t r0[3] = { m[0], m[1], m[2] };
    int32_t r1[3] = { m[3], m[4], m[5] };
    int32_t r2[3];
    r2[0] = (r0[1]*r1[2] - r0[2]*r1[1]) >> 12;      /* OuterProduct0(r0,r1,r2) @0x8008CE8C, */
    r2[1] = (r0[2]*r1[0] - r0[0]*r1[2]) >> 12;      /* GTE OP mit sf=1 (0x4b78000c) */
    r2[2] = (r0[0]*r1[1] - r0[1]*r1[0]) >> 12;
    r0[0] = (r1[1]*r2[2] - r1[2]*r2[1]) >> 12;      /* OuterProduct0(r1,r2,r0) @0x8008CE9C */
    r0[1] = (r1[2]*r2[0] - r1[0]*r2[2]) >> 12;
    r0[2] = (r1[0]*r2[1] - r1[1]*r2[0]) >> 12;
    re15_vector_normal(r0[0], r0[1], r0[2], r0);    /* @0x8008CEA8 */
    re15_vector_normal(r1[0], r1[1], r1[2], r1);    /* @0x8008CEB4 */
    re15_vector_normal(r2[0], r2[1], r2[2], r2);    /* @0x8008CEC0 */
    m[0] = (int16_t)r0[0]; m[1] = (int16_t)r0[1]; m[2] = (int16_t)r0[2];  /* @0x8008CED0-E8 */
    m[3] = (int16_t)r1[0]; m[4] = (int16_t)r1[1]; m[5] = (int16_t)r1[2];  /* @0x8008CEF4-F00 */
    m[6] = (int16_t)r2[0]; m[7] = (int16_t)r2[1]; m[8] = (int16_t)r2[2];  /* @0x8008CF18-30 */

    e->re2z_part_blend[i] = (int8_t)(e->re2z_part_blend[i] - 1);      /* @0x80028D7C-88 */
}

/* FUN_80028AD8 — Wurf mit Aufschlag (Flagbit 0x20). */
static void re2z_part_phys_ad8(re15_actor_t *e, int i)
{
    int16_t *v = e->re2z_part_v[i];
    int32_t *t = e->re2z_part_t[i];

    if (e->re2z_part_st86[i] == 0) {                           /* lh v0,134 @0x80028AF4 */
        /* RotMatrix((0, +0x38, 0)) @0x80028B10-24, dann ApplyMatrixSV((+0x3C, +0x3A, 0))
         * mit dem Ergebnis ZURUECK nach +0x38/+0x3A/+0x3C (`addiu a2,s1,56` @0x80028B3C). */
        int32_t M[9];
        re15_skel_euler_matrix_for_test(0, (int)v[0], 0, M);
        int32_t ix = v[2], iy = v[1];                          /* vz bleibt 0 @0x80028B18 */
        int32_t ox = (M[0]*ix + M[1]*iy) >> 12;
        int32_t oy = (M[3]*ix + M[4]*iy) >> 12;
        int32_t oz = (M[6]*ix + M[7]*iy) >> 12;
        v[0] = (int16_t)ox; v[1] = (int16_t)oy; v[2] = (int16_t)oz;
        e->re2z_part_st86[i]  = 3;                             /* sh 3,134  @0x80028B48-4C */
        e->re2z_part_blend[i] = 15;                            /* sb 15,122 @0x80028B50-54 */
    }

    if (e->re2z_part_st86[i] >= 2) {                           /* slti v0,v0,2 @0x80028B60 */
        int32_t floor_y = (int32_t)e->y;                       /* [PORT (1)] @0x80028B74 */
        t[0] += v[0];                                          /* @0x80028B80-8C */
        v[1]  = (int16_t)(v[1] + e->re2z_part_grav[i]);        /* +0x3A += +0x79 @0x80028B90-A4 */
        t[1] += v[1];                                          /* @0x80028BAC-B8 */
        t[2] += v[2];                                          /* @0x80028BBC-CC */
        if (floor_y < t[1]) {                                  /* slt v1,s0,v1 @0x80028BD8 */
            e->re2z_part_st86[i] = (int16_t)(e->re2z_part_st86[i] - 1); /* @0x80028BE4-F0 */
            t[1] = floor_y;                                    /* @0x80028BF4 */
            v[1] = (int16_t)(((int32_t)(int16_t)(-v[1])) >> 2);/* @0x80028BF8-C10 */
            v[0] = (int16_t)(((int32_t)v[0]) >> 2);            /* @0x80028BFC-C1C */
            v[2] = (int16_t)(((int32_t)v[2]) >> 2);            /* @0x80028C14-28 */
            (void)re2z_rand();                                 /* jal 0x80015FE8 @0x80028C28 */
            re2z_gore_fx(e, i, 2000u);                         /* Anker = das fliegende Teil selbst
                                                             * (`addiu a2,s1,72` @0x80028C34),
                                                             * FUN_8001BF10 @0x80028C4C */
            if (e->re2z_part_flags[i] & 0x10u) {               /* @0x80028C54-60 */
                e->re2z_part_flags[i] = (uint16_t)(e->re2z_part_flags[i] | 0x2000u); /* @0x80028C64-68 */
                /* +0x8C/+0x90 = 7000, +0x8E = 0 @0x80028C6C-7C: Skalierung, [PORT (5)] */
                e->re2z_part_st86[i] = 1;                      /* sh 1,134 @0x80028C80 */
            }
            if (e->re2z_part_st86[i] == 1) {                   /* lh 134 / beq 1 @0x80028C84-8C */
                t[1] = floor_y;                                /* @0x80028C94 */
                e->re2z_part_flags[i] =                        /* DER KASKADEN-AUSLOESER:
                                                                * `and v0,v0,-2` @0x80028CA0 */
                    (uint16_t)(e->re2z_part_flags[i] & (uint16_t)~1u);
            }
        }
        /* [PORT (2)] Wandtest @0x80028CAC-D20 nicht modelliert. */
    }

    if (e->re2z_part_blend[i] >= 0)                            /* lb 122 / bltz @0x80028D24-2C */
        re2z_part_settle(e, i);
}

/* FUN_80028DAC — Drift mit Ablauf (Flagbit 0x08). */
static void re2z_part_phys_dac(re15_actor_t *e, int i)
{
    int32_t *t = e->re2z_part_t[i];

    uint16_t life = e->re2z_part_life[i];                      /* lhu 160 @0x80028DC0 */
    if ((life & 0x7fffu) < 0x1du)                              /* andi/sltiu @0x80028DC8-CC */
        e->re2z_part_life[i] = (uint16_t)(life + 1u);          /* sh @0x80028DD4/E0 */
    else
        e->re2z_part_flags[i] = 0u;                            /* sw zero,0 @0x80028DDC — das
                                                                * Teil verschwindet nach 29 Frames */

    e->re2z_part_w9a[i] = (int16_t)(e->re2z_part_w9a[i] + e->re2z_part_w9e[i]); /* @0x80028DF0-E00 */
    t[1] += (int32_t)e->re2z_part_w9a[i];                                        /* @0x80028E04-14 */

    int32_t M[9];
    re15_skel_euler_matrix_for_test(0, (int)e->re2z_part_yaw98[i], 0, M);        /* @0x80028DF8-E24 */
    int16_t sp = (int16_t)(e->re2z_part_w9c[i] + e->re2z_part_wa4[i]);           /* @0x80028E28-38 */
    if (sp < 0) sp = 0;                                                          /* bgez @0x80028E3C-48 */
    e->re2z_part_w9c[i] = sp;
    t[0] += (int32_t)(int16_t)((M[0] * (int32_t)sp) >> 12);                      /* out.vx @0x80028E64-74 */
    t[2] += (int32_t)(int16_t)((M[6] * (int32_t)sp) >> 12);                      /* out.vz @0x80028E78-88 */
}

int re15_re2z_gore_part_matrix(re15_actor_t *e, int part, uint32_t frame,
                               int32_t rot[9], int32_t trans[3])
{
    if (!e || !rot || !trans || part < 0 || part >= 16) return 0;
    if (!re15_re2z_gore_active(e)) return 0;                   /* RE1.5 landet hier NIE */
    uint16_t fl = e->re2z_part_flags[part];
    if (!(fl & 0x40u)) return 0;                               /* andi v0,s3,0x40 @0x80027498 */
    if (!(fl & 0x01u)) return 0;                               /* Bit 0 weg -> der Walk betritt
                                                                * FUN_80027434 gar nicht erst
                                                                * (@0x8002737C/@0x800273C4) */

    uint16_t bit = (uint16_t)(1u << part);
    if (!(e->re2z_part_seeded & bit)) {
        /* [PORT] Das Original hat die Matrix schon im Record stehen (der Zeichner hat sie im
         * VORFRAME dort hineingeschrieben). Der Port berechnet die Pose jeden Frame neu und
         * friert sie hier beim ersten Frame mit Bit 0x40 ein — 1 Frame Unterschied in der
         * Ausgangspose, danach identisch. */
        for (int k = 0; k < 9; k++) e->re2z_part_m[part][k] = (int16_t)rot[k];
        for (int k = 0; k < 3; k++) e->re2z_part_t[part][k] = trans[k];
        e->re2z_part_seeded = (uint16_t)(e->re2z_part_seeded | bit);
    }

    if (e->re2z_part_frame != frame) {                         /* neuer Frame -> Schritt-Sperre auf */
        e->re2z_part_frame   = frame;
        e->re2z_part_stepped = 0u;
    }
    if (!(e->re2z_part_stepped & bit)) {
        e->re2z_part_stepped = (uint16_t)(e->re2z_part_stepped | bit);
        if (fl & 0x20u) re2z_part_phys_ad8(e, part);           /* @0x80027694-A0 */
        /* Der 0x08-Zweig haengt hinter dem `param_3 & 0x18`-Gate; 0x08 impliziert es. */
        if (fl & 0x08u) re2z_part_phys_dac(e, part);           /* @0x80027B98 */
    }

    for (int k = 0; k < 9; k++) rot[k]   = (int32_t)e->re2z_part_m[part][k];
    for (int k = 0; k < 3; k++) trans[k] = e->re2z_part_t[part][k];
    return 1;
}

/* ---- FUN_80106128 — VERKOHLUNG (+ das +0x10E-Bit 0x80) ------------------------------------ */
static void re2z_gore_burn(re15_actor_t *e)
{
    e->re2z_f10e |= 0x80u;                     /* lhu 270 / ori 0x80 / sh @0x8010613C-48 */
    if (re2z_rand() & 1u) {                    /* jal @0x80106160, andi/beq @0x80106168-6C */
        re2z_gore_fx(e, 0, 0x05032710u);                               /* Part 0 (a2 = s1+72
                                                                        * @0x80106178), v={0,200,0}
                                                                        * @0x80106174-84 */
        if (e->re2z_part_flags[8] & 1u) re2z_gore_fx(e, 8, 0x05031388u); /* Kopf (+1448 = 8*172+72
                                                                        * @0x801061AC) @0x80106190-B4 */
        if (e->re2z_part_flags[3] & 1u) re2z_gore_fx(e, 3, 0x050313E8u); /* R-Unterarm (+588
                                                                        * @0x801061E0), v={0,200,0}
                                                                        * @0x801061C0-E0/@0x80106254 */
    } else {
        re2z_gore_fx(e, 0, 0x05032710u);                               /* Part 0 , v=0
                                                                        * @0x801061E4-F4 */
        if (e->re2z_part_flags[3] & 1u) re2z_gore_fx(e, 3, 0x05031388u); /* R-Unterarm, v={0,200,0}
                                                                        * @0x80106200-24 */
        if (e->re2z_part_flags[6] & 1u) re2z_gore_fx(e, 6, 0x050313E8u); /* L-Unterarm (+1104
                                                                        * @0x80106250) @0x8010622C-58 */
    }
    if (e->sub_state_1 == 16u) return;         /* `lbu v1,5` / `beq v1,16` @0x8010625C-64 */
    e->re2z_part_tint[0]  = 0x00404040u;       /* sw a1,112  @0x8010627C */
    e->re2z_part_tint[1]  = 0x00484848u;       /* sw a2,284  @0x80106288 */
    e->re2z_part_tint[2]  = 0x00707070u;       /* sw a0,456  @0x80106294 */
    e->re2z_part_tint[5]  = 0x00707070u;       /* sw a0,972  @0x801062A0 */
    e->re2z_part_tint[8]  = 0x00505050u;       /* sw v1,1488 @0x801062AC */
    e->re2z_part_tint[9]  = 0x00505050u;       /* sw v1,1660 @0x801062B8 */
    e->re2z_part_tint[10] = 0x00484848u;       /* sw a2,1832 @0x801062C4 */
    e->re2z_part_tint[11] = 0x00404040u;       /* sw a1,2004 @0x801062D0 */
    e->re2z_part_tint[12] = 0x00505050u;       /* sw v1,2176 @0x801062DC */
    e->re2z_part_tint[13] = 0x00404040u;       /* sw a1,2348 @0x801062E8 */
    e->re2z_part_tint[14] = 0x00404040u;       /* sw a1,2520 @0x801062F4 */
}

/* ---- FUN_80106310 — SAEURE-AETZUNG -------------------------------------------------------- */
static void re2z_gore_acid(re15_actor_t *e)
{
    e->re2z_flags21a |= 0x1800u;               /* lhu 538 / ori 0x1800 / sh @0x8010632C-38 */
    e->re2z_part_tint[0]  = 0x00304040u;       /* sw v1,112  @0x8010633C */
    e->re2z_part_tint[1]  = 0x00304040u;       /* sw v1,284  @0x80106348 */
    e->re2z_part_tint[2]  = 0x00405050u;       /* sw a0,456  @0x80106354 */
    e->re2z_part_tint[5]  = 0x00405050u;       /* sw a0,972  @0x80106360 */
    e->re2z_part_tint[8]  = 0x00304040u;       /* sw v1,1488 @0x8010636C */
    e->re2z_part_tint[9]  = 0x00506060u;       /* sw a1,1660 @0x80106378 */
    e->re2z_part_tint[12] = 0x00506060u;       /* sw a1,2176 @0x80106384 */
    re2z_gore_fx(e, 0, 0x040F1770u);                                   /* Part 0  @0x80106390 */
    if (e->re2z_part_flags[12] & 1u) re2z_gore_fx(e, 12, 0x040F0FA0u); /* L-Oberschenkel
                                                                        * @0x801063A0-C0 */
    if (e->re2z_part_flags[3]  & 1u) re2z_gore_fx(e, 3, 0x040F0FA0u);  /* R-Unterarm @0x801063D0-F0 */
}

/* ---- FUN_8010640C — SPRENG-RUSS ----------------------------------------------------------- */
static void re2z_gore_soot(re15_actor_t *e)
{
    re2z_gore_fx(e, 0, 0x05032710u);                                   /* Part 0 , v={0,200,0}
                                                                        * @0x80106418-44 */
    if (e->re2z_part_flags[3] & 1u) re2z_gore_fx(e, 3, 0x050313E8u);   /* R-Unterarm @0x80106450-70 */
    e->re2z_part_tint[0]  = 0x00404040u;       /* sw a0,112  @0x8010648C */
    e->re2z_part_tint[1]  = 0x00484848u;       /* sw a2,284  @0x80106498 */
    e->re2z_part_tint[2]  = 0x00707070u;       /* sw v1,456  @0x801064A4 */
    e->re2z_part_tint[5]  = 0x00707070u;       /* sw v1,972  @0x801064B0 */
    e->re2z_part_tint[9]  = 0x00505050u;       /* sw a1,1660 @0x801064BC */
    e->re2z_part_tint[10] = 0x00484848u;       /* sw a2,1832 @0x801064C8 */
    e->re2z_part_tint[11] = 0x00404040u;       /* sw a0,2004 @0x801064D4 */
    e->re2z_part_tint[12] = 0x00505050u;       /* sw a1,2176 @0x801064E0 */
    e->re2z_part_tint[13] = 0x00404040u;       /* sw a0,2348 @0x801064EC */
    e->re2z_part_tint[14] = 0x00404040u;       /* sw a0,2520 @0x801064F8 */
}

/* ---- FUN_80106510 — SPARK-SHOT-ENTLADUNG --------------------------------------------------
 * Vier Emitter am Part 0 mit je einem eigenen Geschwindigkeitsvektor `128 - rand` je Achse
 * (`addiu s0,zero,128` @0x80106534, `subu v0,s0,v0` @0x80106538/44/50), Id = 0x06000000 |
 * ((rand << 3) + K) mit K = 5096/4096/4096/3096 (@0x80106560/@0x801065B0/@0x801065FC/@0x80106648);
 * danach drei Einzelwuerfe: 0x040C0000-Id am Part 0 (@0x80106670-84), 0x06000000-Id am Part 9
 * (`addiu a2,s0,1620` = 9*172+72 @0x801066B0) und am Part 3 (`addiu a2,s0,588` @0x801066D8).
 * MACHT ZUSAMMEN 19 RNG-WUERFE und 7 Emitter — beides Verhalten, beides exakt nachgezogen. */
static void re2z_gore_spark(re15_actor_t *e)
{
    for (int i = 0; i < 4; i++) {              /* @0x80106530-5C / 80-AC / CC-F8 / 618-44 */
        (void)re2z_rand(); (void)re2z_rand(); (void)re2z_rand();
        uint32_t r = re2z_rand();
        static const int k[4] = { 5096, 4096, 4096, 3096 };
        re2z_gore_fx(e, 0, 0x06000000u | (uint32_t)(((r & 0xffu) << 3) + (uint32_t)k[i]));
    }
    {   uint32_t r = re2z_rand();                                      /* @0x80106664 */
        re2z_gore_fx(e, 0, 0x040C0000u | (uint32_t)(((r & 0xffu) << 3) + 4096u)); /* Part 0
                                                                        * @0x80106670-84 */
    }
    {   uint32_t r = re2z_rand();                                      /* @0x80106690 */
        re2z_gore_fx(e, 9, 0x06000000u | (uint32_t)(((r & 0xffu) << 3) + 3096u)); /* Part 9 (+1620
                                                                        * @0x801066B0) @0x8010669C-B0 */
    }
    {   uint32_t r = re2z_rand();                                      /* @0x801066B8 */
        re2z_gore_fx(e, 3, 0x06000000u | (uint32_t)(((r & 0xffu) << 3) + 3096u)); /* Part 3 (+588
                                                                        * @0x801066D8) @0x801066C0-D8 */
    }
}

/* ---- die per-Zeile-Leiter (Liegend-Zweig @0x80105188-284; DEATH-Zwilling @0x80108444-4D4) --
 * `death` = 1 laesst den `+0x21A |= 0x800`-Nachtrag weg (den hat nur der HURT-Zweig) und nimmt
 * zusaetzlich Zeile 17 in den Russ-Zweig (`beq v1,9` / `bne v1,17` @0x80108490-98). */
static void re2z_dismember_row(re15_actor_t *e, int death)
{
    unsigned row = e->sub_state_1;
    if (row == 10u && !(e->re2z_f10e & 0x80u)) {                /* @0x80105190-A8 / @0x80108444-60 */
        re2z_gore_burn(e);                                     /* jal 0x80106128 @0x801051B0 */
        if (!death) e->re2z_flags21a |= 0x800u;                 /* ori 0x800 @0x801051C0-C4 */
    }
    if (row == 11u) re2z_gore_acid(e);                          /* @0x801051C8-DC / @0x80108478-84 */
    if ((row == 9u || (death && row == 17u)) && !(e->re2z_f10e & 0x80u))
        re2z_gore_soot(e);                                      /* @0x801051E8-208 / @0x801084A0-B8 */
    if (row == 14u) re2z_gore_spark(e);                         /* @0x8010520C-20 / @0x801084C4-D0 */
    if (row == 16u && !death) {                                 /* @0x80105228-84 */
        if (!(e->re2z_f10e & 0x80u) && e->re2z_burn23a >= 9u) { /* andi 0x80 @0x8010523C,
                                                                 * sltiu 0x9 @0x80105250 */
            re2z_gore_burn(e);                                  /* jal @0x8010525C */
            e->re2z_flags21a |= 0x800u;                         /* ori 0x800 @0x8010526C-70 */
        }
        e->re2z_burn23a = (uint8_t)(e->re2z_burn23a + 1u);      /* @0x80105274-84 */
    }
}

/* ---- DER ZERLEGER SELBST @0x80105288-3D8: EIN BEIN WEG ------------------------------------
 * Gate (alle drei muessen gelten):
 *   `+0x21A & 0x60` == 0   @0x80105288-8C  (Einweg — ist ein Bein weg, kommt nie ein zweites)
 *   `(s8)+0x152 < 0`       @0x80105294-9C  (die MITTLERE Zonen-Reserve ist aufgebraucht)
 *   `+0x1D0 & 0xC0` != 0   @0x801052A4-B0  (der Treffer kam von der Seite)
 * Danach: SE 9 (@0x801052B8); Seite ueber `+0x1D0 & 0x80` — gesetzt -> Part 12 (L-Oberschenkel)
 * und `+0x21A |= 0x40` (@0x801052D4-E4), sonst Part 9 (R-Oberschenkel) und `+0x21A |= 0x20`
 * (@0x801052E8-F4). Ein RNG-Wurf (@0x801052F8): bei (rand&1) != 0 UND Typ != 30
 * (`lbu v1,8(s1)` / `beq v1,30` @0x8010530C-14) werden vier Mesh-Woerter des Reserve-Parts 15
 * in den Oberschenkel kopiert (`lw v0,2588/2592/2596/2600(v1)` -> `sw ...,8/12/16/20(s0)`
 * @0x8010531C-50) = der STUMPF. Dann Blut-Emitter 8000 am Oberschenkel (1 Wurf, @0x80105354-6C),
 * das SCHIENBEIN (`addiu s0,s0,172` @0x80105370) bekommt `flags |= 0x1062` (@0x8010537C),
 * seine Wurf-Felder (+0x38 = +0x76+2048, +0x3A = -200, +0x3C = 10, +0x3E = 0, +0x40 = +0x76,
 * +0x42 = 1024, +0x79 = 50, +0x86 = 0 @0x80105384-B8) und einen zweiten Emitter 7000 (1 Wurf,
 * @0x801053BC-D4). Danach faellt der Zweig in den DISPATCH (kein return, @0x801053D8).
 * MACHT DREI RNG-WUERFE. [PORT-MAPPING]: der Mesh-Tausch und die Wurf-Felder haben keinen
 * Renderer-Zwilling — Flag-Wort und Tinte tragen den Zustand, der Wurf-Vektor bleibt OFFEN. */
static void re2z_leg_gore(re15_actor_t *e)
{
    if (e->re2z_flags21a & 0x60u) return;                       /* @0x80105288-8C */
    if (e->re2z_pool152 >= 0) return;                           /* lb 338 / bgez @0x80105294-9C */
    if (!(e->re2z_hitdir1d0 & 0xc0u)) return;                   /* lhu 464 / andi 0xC0 @0x801052A4-B0 */

    re2z_se(9);                                                 /* jal 0x8005bd6c, a0=9 @0x801052B4-B8 */

    int thigh;
    if (e->re2z_hitdir1d0 & 0x80u) {                            /* andi 0x80 @0x801052C8 */
        thigh = 12;                                             /* addiu s0,v1,2064 @0x801052DC */
        e->re2z_flags21a |= 0x40u;                              /* ori 0x40 @0x801052E4 */
    } else {
        thigh = 9;                                              /* addiu s0,v1,1548 @0x801052F0 */
        e->re2z_flags21a |= 0x20u;                              /* ori 0x20 @0x801052F4 */
    }
    if ((re2z_rand() & 1u) && e->type != 0x1eu) {               /* @0x801052F8-314 */
        /* STUMPF-MESH: die vier Geometrie-Woerter des RESERVE-Parts 15 wandern in den
         * Oberschenkel — der Oberschenkel zeigt danach das Stumpf-Modell:
         *   8010531c: lw v0,2588(v1)  ->  8010532c: sw v0,8(s0)
         *   80105330: lw v0,2596(v1)  ->  80105338: sw v0,16(s0)
         *   8010533c: lw v0,2592(v1)  ->  80105344: sw v0,12(s0)
         *   80105348: lw v0,2600(v1)  ->  80105350: sw v0,20(s0)
         * 2588..2600 = 15*172 + 8/12/16/20 (Stride 172), s0 = v1 + 2064 bzw. 1548 = Part 12/9.
         * Port-Zwilling: Objektindex statt Zeiger (Part i == MD1-Mesh i). Die frueher hier
         * eingesetzte Blut-TINTE war ein Stand-in ohne Original-Beleg — @0x8010531C-50
         * schreibt KEIN Farbwort — und ist damit entfallen. */
        e->re2z_part_mesh[thigh] = 15u;                         /* @0x8010531C-50 */
    }
    (void)re2z_rand();                                          /* @0x80105354, Emitter-Winkel */
    re2z_gore_fx(e, thigh, 8000u);                              /* Anker = der Oberschenkel
                                                                 * (`addiu a2,s0,72` @0x80105364),
                                                                 * Emitter @0x80105368 */

    int shin = thigh + 1;                                       /* addiu s0,s0,172 @0x80105370 */
    e->re2z_part_flags[shin] |= 0x1062u;                        /* ori 0x1062 @0x8010537C-80 */
    /* DIE WURF-FELDER (@0x80105384-C0) — jetzt verdrahtet (Welle G). Sie sind die Eingabe der
     * Flugphysik FUN_80028AD8; +0x86 = 0 loest deren INIT aus (`lh 134` @0x80028AF4). */
    e->re2z_part_v   [shin][2] = 10;                            /* +0x3C = 10    @0x80105388-8C */
    e->re2z_part_v   [shin][1] = -200;                          /* +0x3A = -200  @0x80105390-94 */
    e->re2z_part_st86[shin]    = 0;                             /* +0x86 = 0     @0x8010539C */
    e->re2z_part_grav[shin]    = 50;                            /* +0x79 = 50    @0x80105398/A0 */
    e->re2z_part_rot [shin][0] = 0;                             /* +0x3E = 0     @0x801053A4 */
    e->re2z_part_v   [shin][0] = (int16_t)(e->rot_y + 2048);    /* +0x38 = +0x76+2048
                                                                 * @0x80105384/A8-AC */
    e->re2z_part_rot [shin][2] = 1024;                          /* +0x42 = 1024  @0x801053B4-B8 */
    e->re2z_part_rot [shin][1] = (int16_t)e->rot_y;             /* +0x40 = +0x76 @0x801053B0/C0 */
    (void)re2z_rand();                                          /* @0x801053BC */
    re2z_gore_fx(e, shin, 7000u);                               /* Anker = das Schienbein
                                                                 * (`addiu s0,s0,172` @0x80105370,
                                                                 * `addiu a2,s0,72` @0x801053CC) */
}

/* ---- @0x80105E10-F10 (Stagger-P0, Zeile 11 = GL SAEURE): das UNTERSCHENKEL-WEGAETZEN -------
 * Gate `!(+0x21A & 0x1000)` (@0x80105E10-1C) — 0x1000 ist der Latch, den FUN_80106310 selbst
 * setzt (`ori 0x1800` @0x80106334); danach ein RNG-Wurf, und nur (rand & 1) == 0 macht weiter
 * (@0x80105E24-30). Zwei weitere Wuerfe waehlen die Seite: `s0 = (r1 >> (r2 & 3)) & 1`
 * (@0x80105E38-54, das `srav` @0x80105E4C). s0 == 0 -> `+0x21A |= 0x20` und Part 10/11
 * (R-Schienbein/-Fuss), s0 == 1 -> `+0x21A = (x & ~0x20) | 0x40` und Part 13/14
 * (`sll v0,s0,7 / addu v0,v0,s0 / sll v0,v0,2 / addiu v0,v0,1720` = 10*172 + s0*3*172,
 * @0x80105E58-90). Beide Parts bekommen `flags |= 0x10` (@0x80105EC4/@0x80105F00), die
 * Aetz-Tinte 0x00304040 (`lui 0x30 / ori 0x4040` @0x80105E70-74, `sw ...,112` @0x80105EC0/
 * @0x80105EFC), die Zerfalls-Felder (+0x98 = 0, +0x9A = 0, +0x9C = 5, +0x9D = 5, +0x9E =
 * -32718 @0x80105EAC-BC) und je einen Emitter (0x040F0BB8 @0x80105ED0 / 0x040F07D0
 * @0x80105F0C). MACHT DREI RNG-WUERFE (bzw. einen, wenn der erste abbricht). */
static void re2z_stagger_acid_leg(re15_actor_t *e)
{
    if (e->re2z_flags21a & 0x1000u) return;                     /* @0x80105E10-1C */
    if (re2z_rand() & 1u) return;                               /* @0x80105E24-30 */
    uint32_t r1 = re2z_rand();                                  /* @0x80105E38 */
    uint32_t r2 = re2z_rand();                                  /* @0x80105E40 */
    unsigned side = (unsigned)((r1 >> (r2 & 3u)) & 1u);         /* srav/andi @0x80105E4C-54 */
    if (side) e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x20u) | 0x40u); /* @0x80105E64-6C */
    else      e->re2z_flags21a |= 0x20u;                        /* ori 0x20 @0x80105E58 */
    int shin = 10 + 3 * (int)side;                              /* @0x80105E84-90 */
    e->re2z_part_flags[shin]     |= 0x10u;                      /* ori 0x10 @0x80105EC4 */
    e->re2z_part_tint [shin]      = 0x00304040u;                /* sw s2,112 @0x80105EC0 */
    re2z_gore_fx(e, shin, 0x040F0BB8u);                         /* Anker = das Schienbein
                                                                 * (`addiu a2,s3,72` @0x80105EB0,
                                                                 * Emitter @0x80105ED0) */
    e->re2z_part_flags[shin + 1] |= 0x10u;                      /* ori 0x10 @0x80105F00 */
    e->re2z_part_tint [shin + 1]  = 0x00304040u;                /* sw s2,112 @0x80105EFC */
    re2z_gore_fx(e, shin + 1, 0x040F07D0u);                     /* Anker = der Fuss (`addiu s3,
                                                                 * s3,172` @0x80105ED4, `addiu a2,
                                                                 * s3,72` @0x80105EE4) @0x80105F0C */
}

/* ---- Handler 0x80105BC0: der "schon angeschlagen"-Treffer (Zeilen 9-11 + jeder Treffer mit
 * +0x222 == 1). Phasen wieder in +0x6, hier aber ANIM-getrieben: `+0x6 += FUN_8002959C()`
 * @0x80105F48-58 (der Advancer liefert 1, wenn der Clip umlaeuft).
 *   P0 @0x80105C38: +0x6=1, +0x15A=10, +0x14C = Clip 4 (bzw. 3 wenn +0x1D0&0x20) mit Rate 3,
 *      +0x144 = -450 / 0, Drehen auf +0x1C4/+0x1C6 (Rate 16), SE 12/13 per rand&1 mit
 *      +0x239 = 150, +0x16A = 1 / -1, Blut je Zone; Zeile 10/11 zusaetzlich Gore (OPEN).
 *   P1 @0x80105F1C: Advance + Root-Motion, Blut solange +0x15A != 0 && +0x16A != 0, +0x15A--.
 *   P2 @0x80106010: +0x4 = 0x101 (`sw` im Delay-Slot @0x8010601C laeuft IMMER); zusaetzlich bei
 *      (s8)+0x223 <= 0: +0x223 = (rand&0x10)+15 @0x80106028-30, +0x222 = 0 @0x80106034; danach
 *      die Leiter 0xB01 / 0xC01 / 0x201. */
static void re2z_hit_stagger(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80105C48 */
        e->re2z_t15a   = 10;                                       /* +0x15A = 10 @0x80105C50 */
        /* RUECKEN-TREFFER (`+0x1D0 & 0x20`) — Welle F, jetzt verdrahtet. Das Clip-Wort steht
         * als 0x00030004 in a1 (`lui a1,0x3` @0x80105C18 / `ori a1,a1,0x4` @0x80105C38) und
         * wird um EINS DEKREMENTIERT, wenn das Bit steht -> Clip 3, Rate unveraendert 3:
         *   80105c40: lhu  v1,464(s4)     ; +0x1D0
         *   80105c58: andi v1,v1,0x20
         *   80105c5c: sltu v1,zero,v1     ; 1 wenn Ruecken
         *   80105c60: subu a1,a1,v1       ; 0x00030004 -> 0x00030003
         *   80105c74: sw   a1,332(s4)     ; +0x14C
         * Der Schub kommt aus einer Zwei-Wort-Tabelle auf dem Stack, indiziert mit dem
         * gleichen Bit (`sp+16 = -450` @0x80105BEC-F0, `sp+18 = 0` @0x80105BF4):
         *   80105c64: srl  v0,v0,4
         *   80105c68: andi v0,v0,0x2      ; 0 (Front) oder 2 (Ruecken) = Byte-Index
         *   80105c78: lhu  v0,0(v0)       ; sp+16 bzw. sp+18
         *   80105c88: sh   v0,324(s4)     ; +0x144
         * Ein Ruecken-Treffer schiebt den Zombie also NICHT zurueck (Schub 0). */
        {   int back = (e->re2z_hitdir1d0 & 0x20u) != 0;           /* @0x80105C40-60 */
            re2z_clip(e, back ? 3 : 4, 0, 3, 0x400, 0);            /* sw a1,332 @0x80105C74,
                                                                    * Advance-Blend 1024 @0x80105F3C */
            re2z_thrust(e, back ? 0 : -450);                       /* sh v0,324 @0x80105C88 +
                                                                    * FUN_800152C8 @0x80105C84 */
        }
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 16);     /* @0x80105C90-9C */
        if (e->re2z_cd239 == 0) {                                  /* @0x80105CA0-A8 */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* @0x80105CB0-C8 */
            e->re2z_cd239 = 150;                                   /* @0x80105CD0-D4 */
        }
        /* +0x16A = (+0x1D0 & 0x20) ? -1 : 1 — der `sb 1` steht im Delay-Slot und laeuft immer,
         * die -1 ueberschreibt ihn nur im Ruecken-Treffer (@0x80105CE4-F8). Frueher stand hier
         * fest 1, weil +0x1D0 keinen Produzenten hatte; den gibt es jetzt (re2z_stamp_hitdir). */
        e->re2z_dir16a = (e->re2z_hitdir1d0 & 0x20u) ? (uint8_t)0xffu : (uint8_t)1u;
        re2z_blood_fx(e);                                          /* @0x80105D80 / @0x80105DB8 */
        /* Zeile 10 = GL Brand -> Verkohlung (@0x80105DC4-E08), Zeile 11 = GL Saeure -> das
         * BEIN WEGAETZEN (@0x80105E08-F18) + FUN_80106310 (@0x80105F14). */
        if (e->sub_state_1 == 10u && !(e->re2z_f10e & 0x80u)) {    /* @0x80105DC4-E00 */
            re2z_gore_burn(e);                                     /* jal 0x80106128 @0x80105DE8 */
            e->re2z_flags21a |= 0x800u;                            /* ori 0x800 @0x80105DF8-FC */
        }
        if (e->sub_state_1 == 11u) {                               /* @0x80105E08 */
            re2z_stagger_acid_leg(e);                              /* @0x80105E10-F10 */
            re2z_gore_acid(e);                                     /* jal 0x80106310 @0x80105F14 */
        }
        /* fall through in denselben Frame (@0x80105E08 `bne … 0x80105F1C`) */
        /* FALLTHROUGH */
    case 1:
        re2z_hit_move(e);                                          /* @0x80105F28-58 */
        if (re2z_clip_done(e)) e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);  /* @0x80105F50-58 */
        if (e->re2z_t15a != 0 && e->re2z_dir16a != 0) {            /* @0x80105F5C-74 */
            re2z_blood_fx_at(e, 0, (int16_t)e->rot_y);             /* Anker in BEIDEN Zweigen
                                                                    * `addiu a2,s3,72` = Part 0
                                                                    * (@0x80105FA8 / @0x80105FDC) */
                                                                   /* @0x80105FF4 (Effekt-Ids
                                                                    * 8000/5096 je +0x222) */
            e->re2z_t15a = (int16_t)(e->re2z_t15a - 1);            /* @0x80105FFC-0C */
        }
        break;
    default:                                                       /* P2 @0x80106010 */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257 @0x8010601C (Delay-Slot,
                                                                    * laeuft immer) */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        if ((int)e->re2z_res223 <= 0) {                            /* bgtz @0x80106018 */
            e->re2z_res223 = (int8_t)((re2z_rand() & 0x10u) + 15u);/* @0x80106028-30 */
            e->re2z_flag222 = 0;                                   /* sb zero,546 @0x80106034 */
        }
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0            /* @0x8010604C-54 */
            && e->ai_dist < 0xfa0u                                 /* +0x1F0 < 4000 @0x8010605C-68 */
            && (re2z_rand() & 3u) == 0u) {                         /* @0x80106088-94 */
            re15_ai_set_state_word(e, 0xB01);                      /* 2817 @0x80106098/@0x801060F8 */
            break;
        }
        if (e->ai_dist < 0xbb8u                                    /* < 3000 @0x8010609C-A8 */
            && re15_ai_arc_test(e, pl->x, pl->z, 900) != 0         /* @0x801060C0-C8 */
            && (re2z_rand() & 1u) != 0u) {                         /* @0x801060D0-DC */
            re15_ai_set_state_word(e, 0xC01);                      /* 3073 @0x801060E0/@0x801060F8 */
            break;
        }
        if ((re2z_rand() & 1u) != 0u)                              /* @0x801060E4-F0 */
            re15_ai_set_state_word(e, 0x201);                      /* 513 @0x801060F4/@0x801060F8 */
        break;
    }
}

/* ---- Handler 0x80107EF0 (Zeilen 15 und 18): der leichte Taumel-Treffer ---------------------
 *   P0 @0x80107F80: Clip aus der 8-Byte-Tabelle @0x80100124 {1D,1C,1E,1C,1C,1D,1C,1E}[rand&7]
 *      mit Rate 7 (`sw 0x0007_00xx,332` @0x80107FAC), +0x6 = 1 @0x80107F98, +0x158 = Winkel zum
 *      Spieler @0x80107FD8, Blut je Zone, SE 12 + +0x239 = 150 @0x801080D8-E8, Rueckstoss
 *      +0x144 = (rand&0x3F)+20 @0x80108124 / +0x148 = 64-(rand&0x7F) @0x80108140, Bewegung
 *      um (Winkel+2048) = VOM Spieler WEG @0x80108148, Yaw-Wackler +-64 @0x8010816C, dann
 *      Drehen auf den Spieler mit Rate 16 @0x8010817C-80.
 *   P1 @0x80108184: Advance (Blend 512), +0x6 += clip_done.
 *   P2 @0x801081AC: +0x4 = 0x101, 50% 0x201; danach arc512==0 && dist<5000 && 1/8 -> 0xB01. */
static const uint8_t re2z_hit7ef0_clips[8] = { 0x1D,0x1C,0x1E,0x1C, 0x1C,0x1D,0x1C,0x1E };
static void re2z_hit_light(re15_actor_t *e, re15_actor_t *pl)
{
    int clip;
    switch (e->sub_state_2) {
    case 0:
        clip = (int)re2z_hit7ef0_clips[re2z_rand() & 7u];          /* @0x80107F80-90 */
        e->sub_state_2 = 1;                                        /* @0x80107F98 */
        re2z_clip(e, clip, 0, 7, 0x200, 0);                        /* @0x80107FAC (Blend 512
                                                                    * @0x80108194) */
        re2z_blood_fx_at(e, 1, (int16_t)e->rot_y);                 /* @0x801080C4; Anker
                                                                    * `addiu a2,s1,244` @0x801080C0
                                                                    * = Part 1 (Brust), OHNE
                                                                    * Zonen-Verzweigung */
        if (e->re2z_cd239 == 0) {                                  /* @0x801080CC-D4 */
            re2z_se(12);                                           /* @0x801080D8-E0 */
            e->re2z_cd239 = 150;                                   /* @0x801080E4-E8 */
        }
        re2z_thrust(e, -(int)((re2z_rand() & 0x3fu) + 20u));       /* +0x144 @0x80108124, Bewegung
                                                                    * um Winkel+2048 (=rueckwaerts)
                                                                    * @0x80108148 */
        e->rot_y = (int16_t)((e->rot_y + 64 - (int)(re2z_rand() & 0x7fu)) & 0xfff); /* @0x8010816C */
        re15_enemy_steer_point(e, pl->x, pl->z, 16);               /* @0x8010817C-80 */
        /* faellt in denselben Frame in den Advance-Block (@0x801081A4) */
        /* FALLTHROUGH */
    case 1:
        if (re2z_clip_done(e)) e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);  /* @0x80108198-A8 */
        break;
    default:                                                       /* P2 @0x801081AC */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257 @0x801081B0 */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        if ((re2z_rand() & 1u) == 0u)                              /* @0x801081B4-B8 */
            re15_ai_set_state_word(e, 0x201);                      /* 513 @0x801081BC-C0 */
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0            /* @0x801081D8-E0 */
            && e->ai_dist < 0x1388u                                /* < 5000 @0x801081E8-F4 */
            && (re2z_rand() & 7u) == 0u)                           /* @0x80108214-20 */
            re15_ai_set_state_word(e, 0xB01);                      /* 2817 @0x80108224-28 */
        break;
    }
}

/* ---- Haupt-Handler 0x80105438 -------------------------------------------------------------
 *   @0x80105458-70 : +0x222 == 1 -> `jal 0x80105BC0`, danach Epilog
 *   @0x80105478-B8 : Verzweigung auf +0x6 (0/1/2/3, sonst Epilog)
 *   P0 @0x801054BC : Clip auf den Walk-Clip zwingen (`+0x14C = +0x218 + 0xF0000`, nur wenn
 *                    `+0x218 != (u8)+0x14C` @0x801054C8), Treffer-SE (rand&1 ? 11 : 12) mit
 *                    +0x239 = 150, +0x16B = 24, Pose/Push, +0x6 = 1, +0x15A = 1, +0x158 = 0,
 *                    +0x223 -= cost[+0x5], Nachladen wenn (!+0x10E&0x40 && +0x223<=0 && HP>=81)
 *   P1 @0x8010573C : Push/Pose; +0x158++; war +0x158 >= 3 -> +0x6 = 2, +0x158 = 16
 *   P2 @0x8010588C : Push/Pose; +0x158--; war +0x158 == 0 -> +0x6 = 3
 *   P3 @0x80105A50 : Erholung, Leiter 0xB01 / 0xC01 / 0x201 / 0x101
 * ⛔ NICHT portiert (OPEN, reine Presentation): der Modell-Lean. Das Original dreht in jeder
 * Phase zwei Bone-Matrizen um `-(+0x158 * (s8)+0x16B * 8) * +0x15A` bzw. `+0x158 * +0x16B *
 * +0x15A` (@0x801057A4-E8 / @0x801058D4-960, RotMatrix 0x8008E1F4 + 0x8008D934/0x8008DA44 auf
 * +0x198+24 und +0x198+196). Der Port hat keine Bone-Matrix-Injektion; die Zustands-, Ton- und
 * Timing-Seite ist vollstaendig, das sichtbare Zucken des Oberkoerpers fehlt. */
static void re2z_hit_main(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2z_flag222 == 1) { re2z_hit_stagger(e, pl); return; } /* @0x80105458-70 */

    switch (e->sub_state_2) {
    case 0:
        if ((uint8_t)e->motion != e->re2z_walkclip)                /* @0x801054BC-C8 */
            re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 0);      /* sw +0x218+0xF0000 @0x801054D4 */
        if (e->re2z_cd239 == 0) {                                  /* @0x801054D8-E0 */
            re2z_se((re2z_rand() & 1u) ? 11 : 12);                 /* @0x801054E8-500 */
            e->re2z_cd239 = 150;                                   /* @0x80105508-0C */
        }
        e->re2z_gaitrow = 24;                                      /* +0x16B = 24 @0x80105514-18 */
        if (e->sub_state_1 == 16u) {                               /* Flammenwerfer @0x80105520 */
            if (!(e->re2z_f10e & 0x80u) && e->re2z_burn23a >= 9u) {/* andi 0x80 @0x80105530,
                                                                    * sltiu 0x9 @0x80105544 */
                re2z_gore_burn(e);                                 /* jal 0x80106128 @0x80105550 */
                e->re2z_flags21a |= 0x800u;                        /* ori 0x800 @0x80105560-64 */
            }
            e->re2z_gaitrow = 2;                                   /* sb 2,363 @0x8010556C-70 */
            e->re2z_burn23a = (uint8_t)(e->re2z_burn23a + 1u);     /* @0x80105568-78 */
        }
        re2z_hit_move(e);                                          /* @0x80105580-AC/@0x80105714-20 */
        e->sub_state_2 = 1;                                        /* @0x801055B0-B4 */
        e->re2z_t15a   = 1;                                        /* +0x15A = 1 @0x801055B8-BC */
        e->re2z_t158   = 0;                                        /* +0x158 = 0 @0x801055CC */
        {   unsigned row = e->sub_state_1;
            int cost = (row < 18u) ? (int)re2z_hit_cost[row] : 0;  /* tbl @0x8010CC33 @0x801055D8 */
            e->re2z_res223 = (int8_t)(e->re2z_res223 - cost);      /* subu/sb @0x801055E0/@0x801055EC */
        }
        if (!(e->re2z_f10e & 0x40u)                                /* andi 0x40 / bne @0x801055E4-E8 */
            && (int)e->re2z_res223 <= 0                            /* sll 24 / bgtz @0x801055F0-F4 */
            && (int)e->hp >= 81)                                   /* slti 81 / bne @0x801055FC-08 */
            e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));  /* @0x80105610-20 */
        /* RUECKEN-TREFFER kehrt die ZUCK-RICHTUNG um (Welle F, jetzt verdrahtet):
         *   80105624: lhu v0,464(s1)      ; +0x1D0
         *   8010562c: andi v0,v0,0x20     ; Ruecken-Bit
         *   80105630: beq  v0,zero,0x80105640
         *   80105634: addiu v0,zero,-1    ; Delay-Slot
         *   80105638: sh   v0,346(s1)     ; +0x15A = -1  (Default 1 @0x801055B8-BC)
         * re2z_lean_angle multipliziert mit +0x15A, die P1/P2-Rampe spiegelt sich also
         * am Vorzeichen (0/-192/-384/-576 -> 0/+192/+384/+576). Das ist byte-true: der
         * Zombie zuckt beim Ruecken-Treffer nach VORN statt nach hinten.
         * Der Zwilling `addiu s3,zero,2048` @0x8010563C (Default `addu s3,zero,zero`
         * @0x801055C8) dreht die Richtung des Blut-Emitters. */
        if (e->re2z_hitdir1d0 & 0x20u) e->re2z_t15a = -1;          /* @0x80105624-38 */
        /* ---- DER BLUT-EMITTER DES HAUPT-HANDLERS @0x80105640-710 (Welle G) ------------------
         * Er fehlte im Port komplett — ein normaler Treffer spritzte im RE2-Modus gar nicht.
         *   80105640: lbu  v1,5(s1)
         *   80105644: addiu v0,zero,16
         *   80105648: beq  v1,v0,0x80105714     ; Zeile 16 (Flammenwerfer) -> KEIN Blut
         *   80105650: lbu  v1,466(s1)           ; +0x1D2
         *   8010565c-74: v1 % 3                 ; Zonen-Anteil
         *   80105678: beq  v1,zero,0x801056dc   ; Zone 0 -> Anker Part 1 (+244), v = {0,800,0}
         *   8010567c: _addiu a0,zero,6096       ; Delay-Slot: BEIDE Zweige nehmen Id 6096
         *   80105680-d8: sonst Anker Part 0 (+72), v aus Part 8 (+0x2C..) mit x -= 100*+0x15A
         *                und y += 300
         *   801056e8: sll  a1,s3,16
         *   801056fc: sra  a1,a1,16
         *   80105700: subu a1,v0,a1             ; a1 = +0x76 - s3   (s3 = 2048 bei Ruecken)
         *   80105708: jal  0x8001bf10
         * [PORT-MAPPING] Der Stand-in ist positionslos, die beiden Anker-/Geschwindigkeits-
         * Varianten fallen also zusammen; die Id und der WINKEL sind die des Originals.
         * KEIN RNG-Wurf in diesem Block — die Wurfzahl bleibt unveraendert. */
        if (e->sub_state_1 != 16u) {                               /* @0x80105640-48 */
            int16_t bdir = (int16_t)((int)e->rot_y -
                                     ((e->re2z_hitdir1d0 & 0x20u) ? 2048 : 0));
            re2z_blood_fx_dir(e, bdir);                            /* Id 6096 @0x8010567C */
        }
        break;
    case 1:
        re2z_hit_move(e);                                          /* @0x80105740-90 */
        /* ZUCKEN: die Injektion steht NACH dem Advance und VOR dem +0x158-Inkrement
         * (@0x801057A4-838), rechnet also mit dem noch nicht erhoehten +0x158. */
        re2z_lean_pair(e, 0, 0, re2z_lean_angle(e, 1));            /* <<3 nur in P1 */
        {   int16_t old = e->re2z_t158;
            e->re2z_t158 = (int16_t)(old + 1);                     /* sh @0x80105858 (Delay-Slot) */
            if (!(old < 3)) {                                      /* slti 3 / bne @0x80105850-54 */
                e->sub_state_2 = 2;                                /* @0x80105860-64 */
                e->re2z_t158   = 16;                               /* @0x80105868-6C */
            }
        }
        break;
    case 2:
        re2z_hit_move(e);                                          /* @0x8010588C-C4 */
        re2z_lean_pair(e, 0, 0, re2z_lean_angle(e, 0));            /* @0x801058D4-960, KEIN <<3 */
        {   uint16_t old = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(old - 1u);                    /* sh @0x80105974 (Delay-Slot) */
            if (old == 0u) e->sub_state_2 = 3;                     /* bne/sb @0x80105970-7C */
        }
        /* OPEN — der P2-SCHWANZ @0x80105980-0x80105A4C, der GEGENSPIELER des Zerlegers:
         * `sltiu v0,s3,0x514` (+0x1F0 < 1300 @0x80105980), `0x800CFDCB & 0x80` == 0
         * (@0x8010598C-A0), `+0x106 == *(u8*)0x800CFCFE` (@0x801059A8-B8) und
         * `**(u32**)0x800CFE18 & 1` == 0 (@0x801059C0-D8) — danach ZWEI Sektor-Tests
         * FUN_80015758(&self+0x38, <global>, +0x76 ± 256, 288) @0x80105A00 / @0x80105A38, die
         * je `+0x4 = 0x301` (GRAB) committen. JEDER der beiden ist durch eine LEG-LATCH
         * GEGATET: `+0x21A & 0x20` -> @0x801059E8-EC ueberspringt den ersten, `+0x21A & 0x40`
         * -> @0x80105A1C-20 den zweiten. Das ist die spielbare Folge des Bein-Abrisses (der
         * Zombie kann von dieser Seite nicht mehr zugreifen). Nicht portiert, weil FUN_80015758
         * und die drei RE2-Globals keinen Port-Zwilling haben — die LATCHES erzeugt Welle E
         * jetzt aber korrekt. */
        break;
    case 3:
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (RE1.5-Zwilling
                                                                    * @0x80105f9c-fac); ALLE vier
                                                                    * Ausgaenge dieser Phase
                                                                    * (@0x80105ACC/B08/B24/B38)
                                                                    * verlassen die Reaktion */
        if (e->sub_state_1 == 14u) re2z_gore_spark(e);             /* Spark Shot @0x80105A50-64
                                                                    * (v0 = 14 aus dem Delay-Slot
                                                                    * des Phasen-`beq` @0x801054B0) */
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0            /* @0x80105A7C-84 */
            && e->ai_dist < 0xfa0u                                 /* +0x1F0 < 4000 @0x80105A8C-98 */
            && (re2z_rand() & 0xfu) == 0u) {                       /* @0x80105AB8-C4 */
            re15_ai_set_state_word(e, 0xB01);                      /* 2817 @0x80105ACC-D4 */
            break;
        }
        if (re15_ai_arc_test(e, pl->x, pl->z, 1024) != 0           /* beq-zero-skip @0x80105AE8-F0 */
            && (re2z_rand() & 1u) != 0u) {                         /* @0x80105AF8-04 */
            re15_ai_set_state_word(e, 0xC01);                      /* 3073 @0x80105B08-10 */
            break;
        }
        if ((re2z_rand() & 1u) != 0u) {                            /* @0x80105B14-20 */
            re15_ai_set_state_word(e, 0x201);                      /* 513 @0x80105B24-2C */
            break;
        }
        re15_ai_set_state_word(e, 0x101);                          /* 257 @0x80105B38-40 */
        if ((uint8_t)e->motion != e->re2z_walkclip) {              /* @0x80105B30-3C */
            re15_ai_set_state_word(e, 0x10101);                    /* 0x10101 @0x80105B44-50 */
            e->re2z_gaitrow = (uint8_t)((re2z_rand() & 0xfu) << 1);/* @0x80105B54-5C */
            e->re2z_t158    = 30;                                  /* @0x80105B60-68 */
            e->re2z_dir16a  = (uint8_t)((re2z_rand() & 0x1fu) + 30u); /* @0x80105B6C-78 */
            e->re2z_t15a    = (int16_t)(re2z_rand() + 300u);       /* @0x80105B7C-80 */
        }
        break;
    default: break;                                                /* @0x801054B4 j Epilog */
    }
}

/* ============================================================================================
 * Handler 0x801066FC (Tabellen-Zeilen 7 und 8, Spalte 1) — der RAGDOLL-STURZ mit Bounce-Physik.
 * Vollstaendig disassembliert 2026-08-18 (0x801066FC..0x80106F1C, EMOVL10_S0.BIN @0x80100000).
 *
 * ---- Der Kopf @0x80106738-98 ----
 *   `lbu v0,561(s2)` = +0x231: 1 -> `jal 0x80109610`, 2 -> `jal 0x801092C4`, danach return.
 *   Beide Untermaschinen sind im Port OHNE Zwilling (OPEN) — und im Port UNERREICHBAR, weil
 *   ihre einzigen Setzer (s.u.) alle drei an Bedingungen haengen, die der Port nicht erzeugt.
 * ---- Phase 0 @0x8010679C ----
 *   `lw 0x800CFBF8; bgez` -> < 0 setzt +0x231 = 2 (Global ohne Port-Produzent, OPEN)
 *   `lh v0,268(s2)` (+0x10C) != 0  ODER  `lw 0x800CFBD8 & 0x10000000` -> +0x231 = 1 (OPEN)
 *   sonst: `jal 0x80015FE8` @0x801067F4 (der Wurf laeuft IMMER), und NUR wenn (rand&3) != 0
 *   UND `lbu v1,4(s2)` == 3 -> +0x231 = 1. +0x4 ist in diesem Handler zwangslaeufig 2 (die
 *   Wurzel 0x80104F40 ist der HURT-Zustand), der Zweig ist also strukturell tot — der Wurf
 *   selbst aber Verhalten und wird nachgezogen.
 *   Danach @0x8010683C-89C: +0x6 = 1; +0x13C/+0x13E/+0x140 = 0; +0x158 = 0; +0x16B = 0;
 *   +0x14C = 0x30004 (Clip 4, Rate 3; `& 0x20` von +0x1D0 -> Clip 3, OPEN); +0x148 = 0;
 *   +0x144 = -250 (bzw. +100 bei +0x1D0 & 0x20 — die Tabelle sp+16/sp+18 @0x80106728-34);
 *   `jal 0x800152C8` @0x80106898 wendet den Schub an.
 *   @0x801068A0-4C Gore-Emitter (FUN_8001BF10) — 4 feste RNG-Wuerfe plus 2 je Durchlauf einer
 *   Schleife mit (rand&3)+2 Durchlaeufen. Kein FX-Zwilling im Port; die WURF-ZAHL ist Verhalten.
 *   @0x80106950-90 SE: (rand&1)==0 -> 13 sonst 12, nur bei +0x239 == 0, danach +0x239 = 150;
 *   dann UNBEDINGT SE 9. @0x80106994-A0 +0x232 = +0x1C2 (Boden-Y). @0x8010699C
 *   `jal 0x80018FB0` = Pad-Rumble (liest +0x1CE, ruft 0x8007730C) — reine Praesentation.
 *   @0x801069A4-B0 +0x1D3 |= 0x80 (Spieler-Claim). Danach FALLTHROUGH in Phase 1.
 * ---- Phase 1 @0x801069B4 ----
 *   FUN_80015E7C + FUN_8002959C(a3=1024) + FUN_800152C8; `+0x6 += Advance-Rueckgabe`
 *   (@0x801069E8-F0). Wird +0x6 dabei 2, folgt SOFORT die Bone-Injektion @0x80106A04-3C.
 *   @0x80106A40-84: +0x14D != 0 && (+0x14D & 3) == 0 -> Blut (2 RNG-Wuerfe).
 * ---- Phase 2 @0x80106A8C ---- der Aufschlag-Abschluss:
 *   +0x21A &= ~4, +0x1D3 &= 0x7F, Modell-Felder (Praesentation), +0x3C = +0x232,
 *   +0x21A |= 1, +0x10E = 0x2001 (Bit 0 = KRIECHER), Hitbox 200/200/200/200/-350/350,
 *   +0x4 = 1 (WORT -> Zustand 1, Sub 0), +0x14C = 0xF0005 (Clip 5), word0-Bits 0x0C000000 -> 0x04000000.
 * ---- Der gemeinsame Schwanz @0x80106B5C ----
 *   Nur bei +0x6 < 2: drei Frame-Fenster ueber +0x14D rampen den Lean-Vektor
 *     0..9   : +0x13E = 0 ; +0x13C -= (s8)+0x16B*16 ; +0x140 += (s8)+0x16B*24 ; +0x16B += 1
 *     11..29 : +0x13C += (s8)+0x16B*8 ; +0x13E -= +0x158*2 ; +0x140 -= (s8)+0x16B*2 ;
 *              +0x158 += 1 ; +0x16B -= 1                       (alle Faktoren = ALTE Werte)
 *     31..49 : +0x13C -= (s8)+0x16B*4 ; +0x13E += +0x158*2 ; +0x140 -= (s8)+0x16B*2 ;
 *              +0x158 -= 1 ; +0x16B += 1
 *   danach IMMER die Bone-Injektion @0x80106CA0-D4.
 *   @0x80106CD8-D8C: +0x14D == 20 && +0x6 == 1 -> Aufschlag: +0x15A = 10, +0x16A = 2,
 *     Staub-FX (1 RNG-Wurf) und SE 9.
 *   @0x80106D90-F10: nur +0x14D >= 20 && +0x6 == 1 — die BOUNCE-PHYSIK:
 *     +0x16A != 0 -> +0x3C += +0x15A ; +0x15A += (+0x14D < 35) ? 5 : 55
 *     (+0x232 - 200 - Wurzel-Bone-Y) < +0x3C  UND  +0x16A != 0 ->
 *         +0x15A = -((s16)+0x15A >> 3) ; +0x16A -= 1 ; +0x3C = +0x232 - 300 - Wurzel-Bone-Y ;
 *         SE 4.
 *     "Wurzel-Bone-Y" = `lw v1,48(s3)` = die Translation Y der Part-0-Matrix (MATRIX +24, t[1]
 *     bei +20 -> Part +48). Im Port ist genau das die gerenderte Wurzel-Translation:
 *     skeleton_common.c setzt poses[0].trans[1] = rt_y = die Keyframe-py des laufenden Clips.
 * ========================================================================================== */

/* Die Wurzel-Translation Y des laufenden Clips (== Original `lw 48(s3)`, s. oben). Liest
 * dieselbe Bank, aus der der Renderer posiert; ohne Bank 0 (dann ist der Boden-Test die reine
 * `+0x232 - 200 < y`-Ebene und die Physik laeuft trotzdem, statt einzufrieren). */
static int re2z_root_py(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    const re15_emd_animation_t *an = &b->anim;
    const re15_emd_skeleton_t  *sk = &b->skel;
    int clip = (int)e->motion;
    if (clip < 0 || clip >= an->clip_count) return 0;
    int fc = an->clips[clip].frame_count;
    if (fc <= 0) return 0;
    int slot = (int)(e->anim_frame % (uint32_t)fc);
    int fi   = an->clips[clip].first_frame + slot;
    if (fi < 0 || fi >= an->frame_count) return 0;
    int16_t px = 0, py = 0, pz = 0;
    re15_emd_get_keyframe_position(sk, (int)(an->frames[fi] & 0xfffu), &px, &py, &pz);
    (void)px; (void)pz;
    return (int)py;
}

static void re2z_hit_ragdoll(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    if (e->re2z_rag231 != 0) {                                     /* @0x80106738-60 */
        /* OPEN: FUN_80109610 / FUN_801092C4 ohne Port-Zwilling. Der Port kann +0x231 nie
         * setzen (alle drei Setzer haengen an nicht erzeugten Globals bzw. an +0x4 == 3),
         * also ist dieser Zweig hier tot; er bleibt als Marker stehen. */
        re15_ai_set_state_word(e, 0x101);
        return;
    }

    switch (e->sub_state_2) {
    case 0:
        /* 0x800CFBF8 < 0 / +0x10C / 0x800CFBD8 & 0x10000000: keine Port-Produzenten (OPEN) ->
         * der Original-Pfad faellt hier durch bis zum RNG-Wurf. */
        (void)re2z_rand();                                         /* jal @0x801067F4, IMMER */
        /* `+0x4 == 3` @0x8010680C-10 kann in HURT nicht auftreten (Wurzel = Zustand 2). */
        e->sub_state_2  = 1;                                       /* sb 1,6 @0x8010684C */
        e->re2_lean[0]  = 0;                                       /* sh zero,316 @0x80106858 */
        e->re2_lean[1]  = 0;                                       /* sh zero,318 @0x8010685C */
        e->re2_lean[2]  = 0;                                       /* sh zero,320 @0x80106860 */
        e->re2z_t158    = 0;                                       /* sh zero,344 @0x80106864 */
        e->re2z_gaitrow = 0;                                       /* sb zero,363 @0x80106868 */
        /* RUECKEN-TREFFER (`+0x1D0 & 0x20`) — Welle F, jetzt verdrahtet. Exakt dieselbe
         * Zwei-Instruktions-Mechanik wie im Stagger-P0, nur mit anderer Schub-Tabelle:
         *   80106728: addiu v0,zero,-250  /  8010672c: sh v0,16(sp)   ; Front
         *   80106730: addiu v0,zero,100   /  80106734: sh v0,18(sp)   ; Ruecken
         *   8010686c: andi v1,v1,0x20
         *   80106870: sltu v1,zero,v1
         *   80106874: subu a2,a2,v1       ; Clip-Wort 0x00030004 -> 0x00030003
         *   80106878: srl  v0,v0,4  /  8010687c: andi v0,v0,0x2      ; Byte-Index 0/2
         *   80106888: sw   a2,332(s2)     ; +0x14C
         *   8010688c: lhu  v0,0(v0)  /  8010689c: sh v0,324(s2)      ; +0x144
         * Ein Treffer in den Ruecken kippt den Zombie also NACH VORN (+100) statt nach
         * hinten (-250), mit dem gespiegelten Sturz-Clip 3. */
        {   int back = (e->re2z_hitdir1d0 & 0x20u) != 0;           /* @0x8010686C-74 */
            re2z_clip(e, back ? 3 : 4, 0, 3, 0x400, 0);            /* sw a2,332 @0x80106888,
                                                                    * Advance-Blend 1024 @0x801069D8 */
            re2z_thrust(e, back ? 100 : -250);                     /* sh v0,324 @0x8010689C +
                                                                    * FUN_800152C8 @0x80106898 */
        }
        /* Gore-Emitter @0x801068A0-4C: 4 feste Wuerfe, danach (rand&3)+2 Durchlaeufe zu je 2.
         * FX-Zwilling fehlt (Lane-I), die WURF-ZAHL ist Verhalten und wird exakt nachgezogen. */
        (void)re2z_rand();                                         /* @0x801068C0 */
        (void)re2z_rand();                                         /* @0x801068E0 */
        (void)re2z_rand();                                         /* @0x801068F4 */
        {   uint32_t n = (re2z_rand() & 3u) + 2u;                  /* @0x80106910-1C */
            for (uint32_t i = 0; i < n; i++) {
                (void)re2z_rand();                                 /* @0x80106920 */
                (void)re2z_rand();                                 /* @0x8010692C */
            }
        }
        re2z_blood_fx_at(e, 0, (int16_t)e->rot_y);                 /* Anker `addiu s1,s3,72` = Part 0
                                                                    * @0x801068D0, Emitter
                                                                    * @0x801068D8 / @0x80106908 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80106950-58 */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* @0x80106960-78 */
            e->re2z_cd239 = 150;                                   /* @0x80106980-84 */
        }
        re2z_se(9);                                                /* @0x80106988-90, unbedingt */
        e->re2z_gy232   = (int16_t)e->y;                           /* +0x232 = +0x1C2 @0x80106994-A0.
                                                                    * +0x1C2 ist das Boden-Y der
                                                                    * Entity: der EXE-Produzent
                                                                    * schreibt +0x3C und +0x1C2 aus
                                                                    * DEMSELBEN Boden-Query
                                                                    * (`sw v0,60(s0)` / `sh v0,450(s0)`
                                                                    * @0x8003EE04-18) und zieht beide
                                                                    * auf Treppen gemeinsam nach
                                                                    * (@0x8003EAA0-AC). */
        e->re2z_self1d3 |= 0x80u;                                  /* +0x1D3 |= 0x80 @0x801069A4-B0 */
        /* FALLTHROUGH @0x801069B4 — Phase 1 laeuft im selben Frame */
        /* FALLTHROUGH */
    case 1:
        re2z_hit_move(e);                                          /* FUN_80015E7C @0x801069C0 +
                                                                    * FUN_800152C8 @0x801069EC */
        if (re2z_clip_done(e))                                     /* +0x6 += Advance @0x801069E8-F0 */
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);
        break;
    default:                                                       /* Phase 2 @0x80106A8C */
        e->re2z_flags21a &= (uint16_t)~4u;                         /* andi 0xfffb @0x80106A98 */
        e->re2z_self1d3  &= 0x7fu;                                 /* andi 0x7f    @0x80106A9C */
        e->y              = (int32_t)e->re2z_gy232;                /* +0x3C = +0x232 @0x80106AC8 */
        e->re2z_flags21a |= 1u;                                    /* ori 1 @0x80106AD0-D4 */
        /* `+0x4 == 3 -> HP = 10` @0x80106ADC-E8 ist hier tot (HURT). */
        e->re2z_f10e      = 0x2001u;                               /* sh 8193,270 @0x80106B0C-10 —
                                                                    * Bit 0 schaltet die Trefferwurzel
                                                                    * auf die KRIECHER-Tabelle
                                                                    * @0x8010CBE8 (@0x80104FE0-500C) */
        re2z_clip(e, 5, 0, 0xF, 0x100, 0);                         /* sw 0xF0005,332 @0x80106B40 */
        re15_ai_set_state_word(e, 0x1);                            /* sw 1,4 @0x80106B3C (WORT) */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        /* Hitbox-/word0-Felder @0x80106B14-50 sind Modell-/Kollisions-Praesentation ohne
         * Port-Zwilling (OPEN). */
        return;                                                    /* j 0x80106F14 */
    }

    /* ---- der gemeinsame Schwanz @0x80106B5C ---- */
    int frame = re2z_frame_slot(e);                                /* +0x14D */
    if (e->sub_state_2 < 2) {                                      /* sltiu 2 @0x80106B64-68 */
        if (frame < 10) {                                          /* sltiu 0xa @0x80106B78 */
            int8_t k = (int8_t)e->re2z_gaitrow;                    /* lb 363 @0x80106B88/90 */
            e->re2_lean[1]  = 0;                                   /* sh zero,318 @0x80106B94 */
            e->re2_lean[0]  = (int16_t)(e->re2_lean[0] - k * 16);  /* sll 4 / subu @0x80106B9C-A4 */
            e->re2_lean[2]  = (int16_t)(e->re2_lean[2] + k * 24);  /* *3<<3 @0x80106BA8-C0 */
            e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow + 1u);     /* sb @0x80106BB8 */
        }
        if ((unsigned)(frame - 11) < 0x13u) {                      /* 11..29 @0x80106BCC-D4 */
            int8_t  k = (int8_t)e->re2z_gaitrow;                   /* ALTE Werte @0x80106BE4/C08 */
            int16_t t = e->re2z_t158;                              /* lh 344 @0x80106BF4 */
            e->re2_lean[0]  = (int16_t)(e->re2_lean[0] + k * 8);   /* sll 3 @0x80106BEC */
            e->re2_lean[1]  = (int16_t)(e->re2_lean[1] - t * 2);   /* sll 1 / subu @0x80106C00-04 */
            e->re2_lean[2]  = (int16_t)(e->re2_lean[2] - k * 2);   /* sll 1 / subu @0x80106C24-28 */
            e->re2z_t158    = (int16_t)(e->re2z_t158 + 1);         /* sh @0x80106C1C */
            e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow - 1u);     /* sb @0x80106C20 */
        }
        if ((unsigned)(frame - 31) < 0x13u) {                      /* 31..49 @0x80106C38-40 */
            int8_t  k = (int8_t)e->re2z_gaitrow;                   /* @0x80106C50/C74 */
            int16_t t = e->re2z_t158;                              /* lh 344 @0x80106C60 */
            e->re2_lean[0]  = (int16_t)(e->re2_lean[0] - k * 4);   /* sll 2 / subu @0x80106C58-5C */
            e->re2_lean[1]  = (int16_t)(e->re2_lean[1] + t * 2);   /* sll 1 / addu @0x80106C6C-70 */
            e->re2_lean[2]  = (int16_t)(e->re2_lean[2] - k * 2);   /* sll 1 / subu @0x80106C90-94 */
            e->re2z_t158    = (int16_t)(e->re2z_t158 - 1);         /* sh @0x80106C88 */
            e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow + 1u);     /* sb @0x80106C8C */
        }
    }
    /* Die Injektion laeuft in BEIDEN Faellen: fuer +0x6 < 2 hier (@0x80106CA0-D4), fuer den
     * Uebergangs-Tick nach +0x6 == 2 schon oben (@0x80106A04-3C) — beides dieselbe Operation. */
    re2z_lean_pair(e, e->re2_lean[0], e->re2_lean[1], e->re2_lean[2]);

    if (frame == 20 && e->sub_state_2 == 1) {                      /* @0x80106CD8-F0 */
        e->re2z_t15a   = 10;                                       /* +0x15A = 10 @0x80106D50-54 */
        e->re2z_dir16a = 2;                                        /* +0x16A = 2  @0x80106D58-5C */
        (void)re2z_rand();                                         /* Staub-FX-Wurf @0x80106D64 */
        re2z_blood_fx_at(e, 1, (int16_t)e->rot_y);                 /* FUN_8001BF10 @0x80106D7C,
                                                                    * Anker `addiu a2,s0,244`
                                                                    * @0x80106D78 = Part 1 */
        re2z_se(9);                                                /* @0x80106D84-8C */
    }
    if (frame < 20 || e->sub_state_2 != 1) return;                 /* @0x80106D98-AC */

    /* ---- BOUNCE-PHYSIK @0x80106E64-F10 ---- */
    int rooty = re2z_root_py(e);                                   /* `lw 48(s3)` @0x80106E64/EAC */
    if ((int8_t)e->re2z_dir16a != 0) {                             /* lb 362 / beq @0x80106E68-70 */
        e->y = e->y + (int)e->re2z_t15a;                           /* +0x3C += +0x15A @0x80106E84-88 */
        e->re2z_t15a = (int16_t)(e->re2z_t15a + (frame < 35 ? 5 : 55));
                                                                   /* +5 @0x80106E90/9C, +55 bei
                                                                    * +0x14D >= 35 @0x80106EA0-A4 */
    }
    if ((int)e->re2z_gy232 - 200 - rooty < e->y                    /* slt @0x80106EB4-C0 */
        && (int8_t)e->re2z_dir16a != 0) {                          /* lb 362 @0x80106EC8-D0 */
        e->re2z_t15a   = (int16_t)(-(int)(e->re2z_t15a >> 3));     /* sra 19 / negiert @0x80106EE4-F4 */
        e->re2z_dir16a = (uint8_t)(e->re2z_dir16a - 1u);           /* sb @0x80106F00-08 */
        e->y           = (int)e->re2z_gy232 - 300 - rooty;         /* sw 60 @0x80106EF0-F10 */
        re2z_se(4);                                                /* jal @0x80106F0C, a0 = 4 */
    }
}

/* ============================================================================================
 * Handler 0x8010703C (Tabellen-Zeile 12, Spalte 1) — der RUTSCH-RUECKSTOSS mit Bone-Blend.
 * Vollstaendig disassembliert 2026-08-18. Sprungtabelle @0x80100104 (7 Eintraege, selbst gedumpt):
 *   [0] 0x80107080  [1] 0x801071A8  [2] 0x80107248  [3] 0x80107274
 *   [4] 0x8010732C  [5] 0x8010733C  [6] 0x801073E8      (`sltiu v0,v1,0x7` @0x8010705C)
 *
 * P0 @0x80107080: liest den TREFFER-DATENSATZ `lw s2,512(s1)` (+0x200):
 *      rec+0x99 -> +0x16A (getroffener Part); ist er 3 ODER 6 -> +0x1D2 = 3 und SOFORT return
 *      (@0x8010709C-B4 — das ist der einzige BRACKET-Erzeuger, den ich im Overlay gefunden habe).
 *      sonst: +0x6 = 1; +0x158 = rec+0x9A (Rueckstoss-Winkel); +0x14C = 0xF0000 + +0x218
 *      (Walk-Clip, Rate 15); Modell-Flags &= ~0x40; SE (rand&1)==0 ? 13 : 12 bei +0x239 == 0,
 *      danach +0x239 = 150; +0x144 = 300, +0x146 = +0x148 = 0, +0x15A = 0, +0x16B = 50;
 *      FUN_800154AC (Peilung zum Spieler) und ein Blut-Emitter am Part +0x16A (1 RNG-Wurf).
 *      FALLTHROUGH in P1.
 * P1 @0x801071A8: RotMatrix(0,+0x158,0) -> ApplyMatrixLV 0x8008DBA4 auf (+0x144,+0x146,+0x148)
 *      -> +0x38 += out.x, +0x40 += out.z; FUN_8002959C(a3=256);
 *      +0x144 -= +0x15A ; +0x15A += (s8)+0x16B   (beide mit den ALTEN Werten, @0x801071F0-228)
 *      +0x144 < 0 -> +0x6 = 6.
 * P2 @0x80107248: +0x6 = 3; +0x16B = (rand&0x3F)+30; +0x158 = 4096; +0x15A = Wurzel-Bone-Y.
 * P3 @0x80107274: +0x16B--; bei 0 -> +0x6 = 4. Advance(256), dann FUN_80028F48(Identitaet
 *      @0x8009DB44, Kopie der Part-0-Matrix, Part 0, w = +0x158) und Wurzel-Y zurueckschreiben;
 *      +0x158 > 1024 -> +0x158 -= 512.
 * P4 @0x8010732C: +0x6 = 5; +0x16B = 20 (FALLTHROUGH in P5).
 * P5 @0x8010733C: +0x16B--; bei 0 -> +0x6 = 6. Advance(256), derselbe Blend, +0x158 += 153.
 * P6 @0x801073E8: Modell-Flags &= ~0x40; +0x4 = 0x101; (rand&1) != 0 -> +0x4 = 0x201.
 *
 * ⛔ OPEN: der Treffer-Datensatz +0x200 (rec+0x99 Part, rec+0x9A Winkel) hat im Port KEINEN
 * Produzenten — der Port-Hitscan FUN_80011F50 fuehrt weder Trefferpart noch Rueckstoss-Winkel.
 * Der Port setzt darum +0x16A = 0 (Part 0 = Wurzel, der auch die Bone-Blend-Stelle ist) und
 * +0x158 = die AKTUELLE Blickrichtung +0x6A, was den Schub nach der Original-Formel
 * R_y(+0x158) * (300,0,0) exakt nach VORNE legt; jede andere Zahl waere geraten. Der
 * `rec+0x99 in {3,6}` -> +0x1D2 = 3-Zweig bleibt damit unerreichbar und ist als Bracket-Beleg
 * nur dokumentiert (Bracket bleibt insgesamt OPEN, wie im Auftrag festgehalten).
 * ========================================================================================== */
static void re2z_hit_slide(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:
        /* +0x200 OPEN (s.o.): +0x16A = 0, +0x158 = eigene Blickrichtung. */
        e->re2z_dir16a = 0;                                        /* sb +0x99,362 @0x80107090 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x801070C0 */
        e->re2z_t158   = (int16_t)(e->rot_y & 0xfff);              /* sh +0x9A,344 @0x801070C8 */
        re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 0);          /* 0xF0000+ +0x218 @0x801070D4 */
        if (e->re2z_cd239 == 0) {                                  /* @0x801070E8-F0 */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* @0x801070F8-110 */
            e->re2z_cd239 = 150;                                   /* @0x80107118-1C */
        }
        e->speed_h      = 300;                                     /* +0x144 = 300 @0x8010712C */
        e->re2z_t15a    = 0;                                       /* +0x15A = 0   @0x8010713C */
        e->re2z_gaitrow = 50;                                      /* +0x16B = 50  @0x80107140 */
        (void)re2z_rand();                                         /* Blut-Wurf @0x80107184 */
        /* Anker ist HIER kein fester Part: `lb a0,362(s1)` (+0x16A) @0x8010715C, dann
         *   80107164-80: v1 = a0*172   (sll/addu/subu-Kette: 3a<<2 -a =11a, <<2 -a =43a, <<2)
         *   80107188: _addu s2,a0,v1   ; a0 = +0x198
         *   8010719c: addiu a2,s2,72   ; = &Part[+0x16A] + 0x48
         * -> der Anker ist der Part mit dem Index (s8)+0x16A. */
        re2z_blood_fx_at(e, (int)(int8_t)e->re2z_dir16a, (int16_t)e->rot_y); /* @0x801071A0 */
        /* FALLTHROUGH @0x801071A8 */
        /* FALLTHROUGH */
    case 1:
        {   /* R_y(+0x158) * (+0x144, 0, 0) -> +0x38/+0x40 (@0x801071B0-208).
             * Die Port-Trigonometrie ist dieselbe wie in re2z_thrust (dort mit +0x6A). */
            int spd = (int)e->speed_h;
            int yaw = (int)e->re2z_t158 & 0xfff;
            e->x += (int32_t)(((int32_t)re15_cos_q12(yaw) * spd) >> 12);
            e->z -= (int32_t)(((int32_t)re15_sin_q12(yaw) * spd) >> 12);
            int16_t old_v = (int16_t)e->re2z_t15a;                 /* ALTE Werte @0x801071F8/214 */
            e->speed_h    = (int16_t)(e->speed_h - old_v);         /* sh @0x8010721C */
            e->re2z_t15a  = (int16_t)(old_v + (int8_t)e->re2z_gaitrow); /* sh @0x80107228 */
            if ((int16_t)e->speed_h < 0)                           /* bgez @0x80107234 */
                e->sub_state_2 = 6;                                /* sb 6,6 @0x80107244 */
        }
        break;
    case 2:
        e->sub_state_2  = 3;                                       /* sb 3,6 @0x80107254 */
        e->re2z_gaitrow = (uint8_t)((re2z_rand() & 0x3fu) + 30u);  /* @0x80107258-60 */
        e->re2z_t158    = 4096;                                    /* +0x158 = 4096 @0x8010726C */
        e->re2z_t15a    = (int16_t)re2z_root_py(e);                /* +0x15A = `lw 48(s2)` @0x80107270 */
        /* FALLTHROUGH @0x80107274 */
        /* FALLTHROUGH */
    case 3:
        {   uint8_t old = e->re2z_gaitrow;
            e->re2z_gaitrow = (uint8_t)(old - 1u);                 /* sb @0x80107284 */
            if (old == 0u) e->sub_state_2 = 4;                     /* @0x80107288-8C */
        }
        e->re2_bone0_wgt = e->re2z_t158;                           /* FUN_80028F48 w @0x801072F4-F8 */
        if ((int16_t)e->re2z_t158 >= 1025)                         /* slti 1025 @0x80107318 */
            e->re2z_t158 = (int16_t)(e->re2z_t158 - 512);          /* @0x80107320-28 */
        break;
    case 4:
        e->sub_state_2  = 5;                                       /* sb 5,6  @0x80107330 */
        e->re2z_gaitrow = 20;                                      /* +0x16B  @0x80107338 */
        /* FALLTHROUGH @0x8010733C */
        /* FALLTHROUGH */
    case 5:
        {   uint8_t old = e->re2z_gaitrow;
            e->re2z_gaitrow = (uint8_t)(old - 1u);                 /* sb @0x8010734C */
            if (old == 0u) e->sub_state_2 = 6;                     /* @0x80107350-54 */
        }
        e->re2_bone0_wgt = e->re2z_t158;                           /* FUN_80028F48 w @0x801073BC-C0 */
        e->re2z_t158 = (int16_t)((uint16_t)e->re2z_t158 + 153u);   /* @0x801073D4-E4 */
        break;
    default:                                                       /* P6 @0x801073E8 */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257,4 @0x80107408 */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        if ((re2z_rand() & 1u) != 0u)                              /* @0x8010740C-10 */
            re15_ai_set_state_word(e, 0x201);                      /* sw 513,4 @0x80107418 */
        break;
    }
    (void)pl;
}

/* ============================================================================================
 * Handler 0x80107438 (Spalte 0 der Zeilen 7/8/9/12) — der KNOCKDOWN mit ARM-ABRISS.
 * Vollstaendig disassembliert 2026-08-18 (0x80107438..0x80107884).
 * Phasen-Dispatch auf +0x6: 0 -> 0x801074B0, 1 -> 0x80107734, 2 -> 0x8010777C, sonst Epilog
 * (`beq v1,1` @0x80107478, `slti 2`/`beq zero` @0x80107480-8C, `beq v1,2` @0x801074A0).
 *
 * P0 @0x801074B0: `+0x14C = 0x000F0001` (Clip 1, Rate 15; `lui v0,0xf / ori 1 / sw v0,332`
 *   @0x80107490-C8), zwei bare FUN_80015E7C (@0x801074C4/@0x801074E8 — sie fuellen nur den
 *   Wurzel-Delta-Vektor nach, angewendet wird er erst in P1 durch FUN_800152C8, also im Port
 *   keine Bewegung in P0), dazwischen ein RNG-Wurf fuer den ZUFALLS-STARTFRAME
 *   `+0x14D = rand & 7` (@0x801074CC-EC), `+0x1D3 |= 0x80` (@0x801074F0-504), `+0x6 = 1`
 *   (@0x801074F8). Zeile 12 (Bowgun) springt sofort zum SE-Block (@0x8010750C).
 *   Sonst `+0x16B = rand & 1` (@0x80107514-24): Bit 0 -> nur die Blutfontaene, Bit 1 -> ARM AB.
 * P1 @0x80107734: FUN_80015E7C + FUN_8002959C(a3=256) + FUN_800152C8, `+0x6 += Advance`
 *   (@0x80107740-6C).
 * P2 @0x8010777C: `+0x4 == 3` -> HP-Wiederbelebung (in HURT strukturell tot, s. Ragdoll).
 *   `+0x10C != 0` ODER `0x800CFBD8 & 0x10000000` -> `+0x4 = 7` (WORT) und `+0x21A &= ~4`
 *   (@0x801077B4-F0); beide Eingaben haben im Port keinen Produzenten (OPEN, wie im Ragdoll).
 *   Sonst: Hitbox 200/200/-350/200/200/350 (@0x801077F8-818, Praesentation ohne Port-Zwilling),
 *   `+0x10E = 0x2001` (KRIECHER-Bit 0, @0x80107820-24), word0 = (word0 & 0xF3FFFFFF) | 0x04000000
 *   (@0x801077D8/@0x80107828-38), ein RNG-Wurf -> `+0x4 = 1` bzw. `0x201` (@0x80107834-4C),
 *   `+0x1D3 &= 0x7F` (@0x80107850-5C).
 * ========================================================================================== */
/* Die Drift-Felder eines abgerissenen ARMS (Flagwort 0x4A). Die drei Vorkommen @0x80107554-74,
 * @0x801075C4-E4 und @0x80107640-60 sind wortgleich; die Offsets folgen dem Part-Stride 172
 * (Part 3: 676/672/670/674/680/668 - 516; Part 5: ... - 860; Part 6: ... - 1032).
 *   +0xA0 = 0    (Lebensdauer)      sh zero,676(v1)  @0x80107554
 *   +0x9C = 0    (Vortrieb)         sh zero,672(v1)  @0x80107558
 *   +0x9A = -10  (vy)               sh s3,670(v1)    @0x8010755C   (s3 = -10 @0x80107540)
 *   +0x9E = 0    (vy-Zuwachs)       sh zero,674(v1)  @0x80107560
 *   +0xA4 = 0    (Vortriebs-Zuwachs)sh zero,680(v1)  @0x80107564
 *   +0x98 = +0x76 + 2048 (Kurs)     sh v0,668(v1)    @0x8010756C-74
 * Der Arm steigt also mit 10 Einheiten je Frame und verschwindet nach 29 Frames
 * (`sltiu v0,v0,0x1d` @0x80028DCC / `sw zero,0(s0)` @0x80028DDC) — KEIN Vortrieb, keine
 * Gravitation, keine Kaskade (Flagwort wird 0, nicht 0x20). */
static void re2z_arm_throw(re15_actor_t *e, int p)
{
    e->re2z_part_life [p] = 0;
    e->re2z_part_w9c  [p] = 0;
    e->re2z_part_w9a  [p] = -10;
    e->re2z_part_w9e  [p] = 0;
    e->re2z_part_wa4  [p] = 0;
    e->re2z_part_yaw98[p] = (int16_t)(e->rot_y + 2048);
}

static void re2z_knockdown_gore(re15_actor_t *e)
{
    uint32_t r = re2z_rand();                                      /* @0x80107514 */
    e->re2z_gaitrow = (uint8_t)(r & 1u);                           /* sb v0,363 @0x80107524 */
    if (r & 1u) {
        /* RECHTER UNTERARM AB, RECHTE HAND WEG (@0x80107534-88) */
        e->re2z_part_flags[3] |= 0x4Au;                            /* ori 0x4A @0x80107544-48 */
        e->re2z_part_tint [3]  = 0x0010104Fu;                      /* sw s1,628 @0x80107568 */
        re2z_arm_throw(e, 3);                                      /* @0x80107554-74 */
        /* FUN_8001CEFC(5,3,Part 3) @0x80107570 = die am Teil haengenden Effekte abschalten
         * (Port: No-op, der Stand-in kennt keine Anker) */
        re2z_se(2);                                                /* jal 0x8005bd6c @0x80107584 */
        e->re2z_part_flags[4]  = 0u;                               /* sw zero,688 @0x80107588 */
        if (re2z_rand() & 1u) {                                    /* @0x8010758C-98 */
            /* LINKER OBERARM + UNTERARM AB, LINKE HAND WEG (@0x801075A4-7C) */
            e->re2z_part_flags[5] |= 0x4Au;                        /* ori 0x4A @0x801075B4-B8 */
            e->re2z_part_tint [5]  = 0x0010104Fu;                  /* sw s1,972 @0x801075D8 */
            re2z_arm_throw(e, 5);                                  /* @0x801075C4-E4 */
            re2z_gore_fx(e, 5, 6000u);                             /* Part 5 (`addiu s0,v1,932`
                                                                    * @0x801075B0) @0x801075E0 */
            (void)re2z_rand();                                     /* @0x801075E8 */
            re2z_gore_fx(e, 5, 0x08001000u);                       /* Part 5 (a2 = s0 @0x801075FC)
                                                                    * @0x80107600 */
            e->re2z_part_flags[6] |= 0x4Au;                        /* ori 0x4A @0x80107630-34 */
            e->re2z_part_tint [6]  = 0x0010104Fu;                  /* sw s1,1144 @0x80107654 */
            re2z_arm_throw(e, 6);                                  /* @0x80107640-60 */
            re2z_gore_fx(e, 6, 6000u);                             /* Part 6 (`addiu s0,v1,1104`
                                                                    * @0x8010762C) @0x8010765C */
            e->re2z_part_flags[7]  = 0u;                           /* sw zero,1204 @0x8010767C */
        }
    }
    /* Die Blutfontaene am Part 3 laeuft IMMER — der Abriss faellt hier durch (@0x80107680-708). */
    /* Alle vier Fontaenen haengen am PART 3 (`addiu s0,s0,588` @0x801076A0, a2 = s0). */
    re2z_gore_fx(e, 3, 5000u);                                     /* v={0,0,0}   @0x801076A4 */
    (void)re2z_rand();                                             /* @0x801076B0 */
    re2z_gore_fx(e, 3, 6000u);                                     /* v={0,400,0} @0x801076C4 */
    (void)re2z_rand();                                             /* @0x801076D0 */
    re2z_gore_fx(e, 3, 4000u);                                     /* v={0,800,0} @0x801076E4 */
    (void)re2z_rand();                                             /* @0x801076EC */
    re2z_gore_fx(e, 3, 0x08001000u);                               /* @0x80107704 */
}

static void re2z_hit_knockdown(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    switch (e->sub_state_2) {
    case 0:
        re2z_clip(e, 1, 0, 0xF, 0x100, 0);                         /* 0xF0001 @0x801074C8 */
        e->anim_frame = (uint16_t)(re2z_rand() & 7u);              /* +0x14D @0x801074CC-EC */
        e->re2z_self1d3 |= 0x80u;                                  /* @0x801074F0-504 */
        e->sub_state_2   = 1;                                      /* sb 1,6 @0x801074F8 */
        if (e->sub_state_1 != 12u)                                 /* `beq v1,12` @0x8010750C */
            re2z_knockdown_gore(e);
        if (e->re2z_cd239 == 0) {                                  /* @0x8010770C-14 */
            re2z_se(12);                                           /* @0x80107718-20 */
            e->re2z_cd239 = 150;                                   /* @0x80107724-28 */
        }
        /* FUN_80018FB0 @0x8010772C = Pad-Rumble (Praesentation, wie im Ragdoll dokumentiert) */
        /* FALLTHROUGH @0x80107734 — P1 laeuft im selben Frame */
        /* FALLTHROUGH */
    case 1:
        re15_re2z_move_root(e);                                    /* e7c @0x80107740 + 152c8
                                                                    * @0x8010776C */
        if (re2z_clip_done(e))                                     /* +0x6 += Advance @0x80107754-70 */
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);
        break;
    default:                                                       /* P2 @0x8010777C */
        /* `+0x4 == 3` (@0x80107784) ist in HURT unerreichbar; +0x10C und 0x800CFBD8 haben im
         * Port keinen Produzenten (OPEN) -> der Original-Pfad faellt in den KRIECHER-Ausgang. */
        e->re2z_f10e = 0x2001u;                                    /* sh 8193,270 @0x80107820-24 */
        re15_ai_set_state_word(e, (re2z_rand() & 1u) ? 1u : 0x201u);/* @0x80107834-4C */
        e->re2z_self1d3 &= 0x7fu;                                  /* andi 0x7f @0x80107850-5C */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        /* Hitbox-/word0-Felder @0x801077F8-838 = Modell-/Kollisions-Praesentation (OPEN). */
        return;
    }
}

/* ---- der Treffer-Stempel des Ports: +0x1D0 (Richtung) und die Zonen-Reserven ---------------
 * Im Original macht das der EXE-Applier (FUN_800410CC/FUN_800470C0), BEVOR die HURT-Wurzel
 * laeuft. Der Port hat in diesem Modul keinen Applier-Hook (re15_damage.c ist fuer diese Welle
 * gesperrt); die Flanke ist stattdessen `+0x6 == 0` in Zustand 2 — genau das setzt
 * re15_re2_stamp_hit je Treffer (`e->sub_state_2 = 0`, Beleg `sw v0,4(s1)` @0x80047288/90).
 * [PORT-MAPPING] nur fuer den AUFRUFZEITPUNKT; die Rechnung selbst ist byte-true:
 *   Richtung  d = FUN_800154AC(Angreifer.x/z, Ziel.x/z) - Ziel+0x76   (@0x800419D8-A08)
 *             FUN_800154AC liefert die Peilung Angreifer->Ziel mit 0 = +X (dx==0 -> 1024/3072
 *             @0x800154B0-E8, sonst `4096 - ratan(dz/dx)` bzw. `2048 - ratan(...)`
 *             @0x80015530-40). Der Port-Zwilling ist `re15_atan2_q12(dz,dx) - 1024`
 *             (re15_actor.h: "0 = +Z", minus 1024 = "0 = +X").
 *   Reserven  Region = +0x1D2 % 3 (`+0x1D2 = 3*Bracket + Region` @0x80041A88-9C),
 *             Kosten = (rec.w1 >> (Bracket*3)) & 7, Saettigung bei -1 (@0x80041954-88).
 * Die Kosten-Woerter sind selbst gedumpt (`read 0x800a412c 95 --w 4`), Zombie-Record-Tabelle
 * 0x800A412C, Stride 20, Zeile = Attacken-Id 1..19; hier steht word1 (+0x04) je Zeile. */
static const uint32_t re2z_pool_cost_w1[20] = {
    /* 0 unbenutzt */ 0u,
    0x078EFC14u, 0x02851014u, 0x02851014u, 0x00810214u, 0x078EFC0Au,   /*  1.. 5 */
    0x078EFC0Au, 0x078EFC0Au, 0x078EFC0Au, 0x078EFC0Au, 0x078EFC0Au,   /*  6..10 */
    0x078EFC0Au, 0x0000000Au, 0x02851014u, 0x078EFC0Au, 0x0081020Au,   /* 11..15 */
    0x0285100Au, 0x078EFC0Au, 0x0102040Au, 0x02851014u                 /* 16..19 */
};

static void re2z_stamp_hit(re15_actor_t *e, const re15_actor_t *pl)
{
    /* --- +0x1D0: Low-Byte je Treffer neu (`andi 0xff00` @0x80041384 / @0x80047178) --- */
    int bearing = ((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 1024) & 0xfff;
    int d       = bearing - ((int)e->rot_y & 0xfff);               /* subu v1,v0,v1 @0x80041A08 */
    uint16_t dir = (uint16_t)(e->re2z_hitdir1d0 & 0xff00u);
    if ((unsigned)((d + 1024) & 0xfff) < 2048u) dir |= 0x20u;      /* @0x80041A0C-2C */
    if ((unsigned)((d + 1536) & 0xfff) < 1024u) dir |= 0x40u;      /* @0x80041A30-58 */
    if ((unsigned)((d -  512) & 0xfff) < 1024u) dir |= 0x80u;      /* @0x80041A5C-84 */
    e->re2z_hitdir1d0 = dir;

    /* --- die Zonen-Reserve der getroffenen Region --- */
    unsigned region  = (unsigned)e->re2z_hits1d2 % 3u;             /* @0x80041A88-9C */
    unsigned bracket = (unsigned)e->re2z_hits1d2 / 3u;
    unsigned row     = e->sub_state_1;
    if (row >= 20u) return;
    int cost = (int)((re2z_pool_cost_w1[row] >> (bracket * 3u)) & 7u);  /* @0x80041954-70 */
    int8_t *pool = (region == 0u) ? &e->re2z_pool153                    /* @0x80041900-08 */
                 : (region == 1u) ? &e->re2z_pool152                    /* @0x80041950 */
                                  : &e->re2z_pool151;                   /* @0x80041994 */
    int v = (int)*pool - cost;                                          /* subu @0x80041974 */
    *pool = (int8_t)((v < 0) ? -1 : v);                                 /* Klemme @0x8004197C-88 */
}

static void re2z_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    /* Der Treffer-Stempel des Applier-Zwillings (s. Block oben) — genau EINMAL je Treffer.
     * `+0x6 == 0` in Zustand 2 tritt AUSSCHLIESSLICH in dem Tick auf, in dem ein Treffer
     * eingeschlagen ist: re15_re2_stamp_hit nullt +0x6 (Beleg `sw v0,4(s1)` @0x80047288/90),
     * und jede Reaktion verlaesst diesen Tick entweder mit +0x6 = 1 (alle sechs Handler-P0)
     * oder mit einem Zustandswort ungleich 2 (Flinch 0x501 @0x801050A4, Liegend 0x60501
     * @0x8010517C, NULL-Zelle 0x101). Eine zweite Stempelung im selben Treffer ist damit
     * ausgeschlossen — ohne Zusatz-Latch. */
    if (e->sub_state_2 == 0u) re2z_stamp_hit(e, pl);

    re2z_grab_abort(e, pl);                                        /* @0x80104F68-FDC */

    /* OPEN (Kriecher, Welle 5): +0x10E & 1 -> 1D-Tabelle @0x8010CBE8 -> FUN_80107888
     * (@0x80104FE0-500C); +0x21A & 0x10 -> Kriecher-Umbau FUN_80107A78 (@0x80105014-38).
     * Beide Zweige haben im Port keinen Handler; der Marker bleibt byte-true stehen und der
     * Treffer laeuft weiter durch die normale Reaktion (bisheriges Port-Verhalten). */

    /* Flinch-Schwelle @0x8010503C-58: `+0x10E & 0x40` ? (+0x5 != 1 ? 23 : 0) : 0 */
    int thr = 0;
    if (e->re2z_f10e & 0x40u) thr = (e->sub_state_1 != 1) ? 23 : 0;

    /* Flinch-Gate: NUR in Phase 0 (`bne v0,zero,0x80105168` @0x80105064) und nur wenn die
     * Resistenz aufgebraucht ist (`lb 547` / `slt v1,v0` / `bne` @0x8010506C-78). */
    if (e->sub_state_2 == 0 && (int)e->re2z_res223 <= thr) {
        if (e->re2z_flag222 == 1 || e->sub_state_1 == 1) {         /* @0x80105080-98 */
            e->re2z_flag222 = 1;                                   /* sb 1,546 @0x801050A0 */
            re15_ai_set_state_word(e, 0x501);                      /* sw 0x501 @0x801050A4-AC */
            e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));  /* @0x801050C0-C8 */
            /* Blut in BEIDEN Zonen-Zweigen (@0x801050E8-15C): Zone 0 generisch (id 6096,
             * ofs {0,800,0}), sonst am Knochen (id 4096). Der Port-FX ist positionslos, der
             * frueher hier stehende `%3 == 0`-Filter unterdrueckte den Knochen-Zweig faelschlich. */
            re2z_blood_fx(e);
            return;                                                /* j 0x80105418 */
        }
        e->re2z_flag222 = 1;                                       /* sb 1,546 @0x80105164 */
    }

    /* @0x80105168-284: liegender Zombie (+0x21A & 2) -> zurueck in den Knockdown-Executor,
     * Phase 6 (Liege-Clip). `lui 6 / ori 0x501 / sw` = 0x60501 @0x8010517C-84. */
    if (e->re2z_flags21a & 2u) {
        re15_ai_set_state_word(e, 0x60501);                        /* @0x8010517C-84 */
        e->re2z_gaitrow = 1;                                       /* sb 1,363 @0x8010518C */
        /* ⛔ TOTER ZWEIG IM ORIGINAL — NICHT "reparieren". Die Leiter @0x80105188-284 liest
         * `lbu v1,5(s1)` @0x80105188, also NACH dem Wort-Store `sw v1,4(s1)` @0x80105184 mit
         * v1 = 0x00060501 (`lui v1,0x6` @0x8010517C / `ori v1,v1,0x501` @0x80105180). Little
         * Endian: +0x4 = 0x01, **+0x5 = 0x05**, +0x6 = 0x06. Der Vergleich `bne v1,10`
         * @0x80105194 (und die Zwillinge 11/9/14/16) sieht damit IMMER 5 — keine der fuenf
         * Zeilen kann hier zuenden. Der Port bildet genau das ab: derselbe Aufruf NACH
         * demselben Zustandswort, also liest er ebenfalls die 5. Die LEBENDEN Einstiege in
         * dieselbe Leiter sind Stagger-P0 (@0x80105DC4, `lbu v1,5(s4)` ohne vorherigen
         * Wort-Store), Haupt-P0 (@0x80105510), Haupt-P3 (@0x80105A50) und DEATH (@0x80108444).
         * GEMESSEN (test_re2_gore PIN 8): dieser Aufruf kostet 0 RNG-Wuerfe, auch mit Zeile 14 —
         * der Beweis, dass er im Port genauso stumm ist wie im Original. */
        re2z_dismember_row(e, 0);                                  /* @0x80105188-284 */
        return;
    }

    /* Der ZERLEGER @0x80105288-3D8 — kein return, er faellt in den Dispatch (@0x801053D8). */
    re2z_leg_gore(e);

    /* DISPATCH @0x801053E0-410 */
    unsigned row = e->sub_state_1;
    unsigned col = e->re2z_hits1d2;
    uint8_t h = (row < 19u && col < 9u) ? re2z_hit_tbl[row][col] : (uint8_t)RE2ZH_NULL;
    s_re2z_last_handler = h;                                       /* Port-Diagnose (Tests) */
    switch (h) {
    case RE2ZH_MAIN:    re2z_hit_main(e, pl);    return;           /* 0x80105438 */
    case RE2ZH_STAGGER: re2z_hit_stagger(e, pl); return;           /* 0x80105BC0 */
    case RE2ZH_66FC:    re2z_hit_ragdoll(e, pl); return;           /* 0x801066FC */
    case RE2ZH_703C:    re2z_hit_slide(e, pl);   return;           /* 0x8010703C */
    case RE2ZH_7EF0:    re2z_hit_light(e, pl);   return;           /* 0x80107EF0 */
    case RE2ZH_7438:    re2z_hit_knockdown(e, pl); return;         /* 0x80107438 */
    default: break;
    }
    /* Jede NULL-Zelle: das Original wuerde dort `jalr 0` ausfuehren, die Kombination entsteht
     * dort also nie. Der Port haelt hier das bisherige Verhalten (zurueck in den Gang), damit
     * kein Zombie einfriert. */
    re15_ai_set_state_word(e, 0x101);
    re2z_hit_latch_release(e);                                     /* auch die Notbremse verlaesst
                                                                    * die Reaktion -> treffbar
                                                                    * (@0x80105f9c-fac) */
}

/* Port-Diagnose: welche Tabellenzelle zuletzt dispatcht wurde (0 = NULL/keine). Nur fuer die
 * Tests — die Engine liest das nicht. */
int re15_re2z_last_hit_handler(void) { return s_re2z_last_handler; }

/* ============================================================================================
 * DEATH @0x80108250 — DIE STURZ-KETTE
 *
 * ⛔ NUTZER-REPORT 2026-08-20 (v0.3.4, beide RE2-Modi): "Die Zombies fallen manchmal ganz komisch
 * nach vorne, nachdem sie frontal mit Kugel getroffen wurden, und liegen merkwuerdig abrupt am
 * Boden auf dem Bauch mit Sterbe-Animation, ohne richtigen Uebergang."
 *
 * GEMESSEN (probe_re2_zfall 4 3, ROOM1140, echter Weg, GELADENE RE2-Bank EM010) — der Port VOR
 * diesem Fix, Brustknochen 8 relativ zum Boden:
 *   f160 st=1 s1=1 clip=0  af=0  b8dy=-2766   <- STEHT
 *   f165 st=3 s1=3 s2=1 clip=7 af=0 b8dy=-548 <- EIN Frame spaeter LIEGT er (Sprung 2218 Einheiten)
 *   f234 st=7 s1=9 clip=7 af=69 b8dy=-165     <- Clip 7 laeuft am BODEN aus
 *   f235 clip=22 (0x16) b8dy=-129             <- Leiche schnappt in die BAUCH-Pose
 * Es gab also gar keinen Sturz: der Port sprang in EINEM Frame in eine Liege-Animation.
 *
 * URSACHE, selbst disassembliert (EMOVL10_S0.BIN): die DEATH-Wurzel FUN_80108250 hat VIER Zweige,
 * der Port kannte nur EINEN — und zwar den falschen.
 *   @0x80108260-6C  `lhu 538 / andi 0x4000 / bne 0x8010829c` — NUR Kill-Zaehler (0x800D46C0++
 *                   @0x8010827C-88) und Latch `+0x21A |= 0x4000` @0x80108294-98 stehen hinter
 *                   diesem Gate. ALLES danach laeuft in JEDEM DEATH-Frame. Der Port hatte die
 *                   ganze Wurzel in den Einmal-Zweig gepackt = keine Phasen-Fortschaltung.
 *   @0x8010829C-AC  `andi 0xfdff / sh 538` -> +0x21A &= ~0x200
 *   @0x801082B0-328 Grab-Abbruch
 *   @0x80108328-AC  `lbu 6 / bne` -> nur in Phase 0: fuenf Modell-Wortsetzer |= 0x8000
 *                   (Praesentation, PORT-OPEN)
 *   @0x801083B0-E0  ZWEIG 1 `lhu 270 / andi 0x1 / beq` -> 1D-Tabelle @0x8010CECC[+0x5]
 *                   ([0] = NULL, [1..18] = FUN_80108A14) `lw v0,-12596(at)` @0x801083D8,
 *                   `jalr v0` @0x80108514. **Genau dieser Handler setzt Clip 7 (@0x80108A88).**
 *                   Er gilt fuer den WIEDERBELEBTEN KRIECHER (+0x10E Bit 0 wird ausschliesslich
 *                   in der Wiederbelebung @0x801089B0 gesetzt, `sh 0x2001,270`), NICHT fuer den
 *                   normalen Stand-Tod. Der Port spielte ihn IMMER.
 *   @0x801083E4-404 ZWEIG 2 `andi v0,v1,0x10 / beq 0x80108408` -> FUN_801099E4 (Kriecher) + raus
 *   @0x80108408-DC  ZWEIG 3 `andi v0,v1,0x2 / beq 0x801084E0` -> Liegend: Blut 8000 an Part 0
 *                   (@0x8010840C-1C), SE (rand&1)?13:11 (@0x80108424-3C), Zerleger-Leiter
 *                   (@0x80108444-D0), dann `addiu v0,zero,2311 / sw v0,4` = 0x907 @0x801084D8-DC
 *   @0x801084E0-518 ZWEIG 4 (der NORMALE Stand-Tod) 2D-Dispatch
 *                   `lbu v1,5(a0)` @0x801084E4 (Zeile = Angriffs-Id, Stride 36 via
 *                   `sll 3 / addu / sll 2` @0x801084F0-F8), `lbu v1,466(a0)` @0x801084FC
 *                   (Spalte = +0x1D2, `sll 2` @0x80108504), Basis `addiu a2,a2,-13276`
 *                   @0x801084EC = 0x8010CC24, `lw v0,0(v1)` @0x8010850C, `jalr v0` @0x80108514.
 *
 * Die Vorwaerts-/Rueckwaerts-Kette steckt im Haupt-Handler FUN_80108530 (Zeile 3 = Browning HP,
 * Spalte 1 = Basis-Zone -> Zelle @0x8010CC94): Phase 0 zieht `+0x16A = rand&1` und spielt
 * Clip 1 (rueckwaerts, ab Frame 0) bzw. Clip 2 (VORWAERTS, ab Frame 10) — das sind DIESELBEN
 * Sturz-Clips, die der Knockdown-Executor benutzt (`re2z_param_clips[side]` @0x801032C8) und die
 * den Koerper sichtbar vom Stand zu Boden bringen. Zusaetzlich stellt Phase 0 `+0x21A` Bit 0x4
 * auf die gewaehlte Richtung — und GENAU dieses Bit waehlt spaeter die Leichen-Pose
 * (`+0x21A & 0x4 ? 22 : 23` @0x8010A490-BC). Der Port hat Bit 0x4 nie im Tod gesetzt, sondern
 * den Rest eines frueheren Knockdowns stehen lassen: Clip 7 (Boden) endete in einer Pose, die
 * Leichen-Pose kam aus einem fremden Latch -> der "abrupte Bauch ohne Uebergang".
 * ========================================================================================== */

/* Die DEATH-Dispatch-Tabelle @0x8010CC24, selbst gedumpt
 * (`re2_disasm.py table 0x8010cc24 170 --bin EMOVL10_S0.BIN`), 18 Zeilen a 9 Spalten.
 * Zeile 0 liegt physisch auf der 1D-Kriecher-Tabelle @0x8010CBE8 + den Kosten-Bytes @0x8010CC33
 * und wird nie dispatcht (Angriffs-Id 0 existiert nicht) — genau wie bei der HURT-Tabelle.
 * Zeile 18 Spalte 8 faellt auf 0x8010CECC = Index 0 der 1D-Tabelle = NULL, ebenfalls wie dort. */
enum {
    RE2ZD_NULL = 0,
    RE2ZD_MAIN,      /* 0x80108530 — der normale Sturz (Clip 1/2), unten portiert            */
    RE2ZD_7438,      /* 0x80107438 — = der HURT-Knockdown-Handler, DEATH-Zweig `+0x4==3` OPEN */
    RE2ZD_92C4,      /* 0x801092C4 — Phasentabelle @0x8010014C (5 Phasen), OPEN               */
    RE2ZD_66FC,      /* 0x801066FC — = der HURT-Ragdoll-Handler, DEATH-Zweig `+0x4==3` OPEN   */
    RE2ZD_8BEC,      /* 0x80108BEC — Phasentabelle @0x8010012C (7 Phasen), OPEN               */
    RE2ZD_9610       /* 0x80109610 — Zerplatzen (`+0x4 = 12` @0x801099B0), OPEN               */
};
static const uint8_t re2z_death_tbl[19][9] = {
/* 0*/ { 0,0,0, 0,0,0, 0,0,0 },   /* existiert nicht (1D-Tabelle + Kosten-Bytes @0x8010CC24) */
/* 1*/ { 1,1,0, 1,1,0, 1,1,0 },   /* @0x8010CC48 */
/* 2*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 3*/ { 1,1,0, 1,1,0, 1,1,0 },   /* @0x8010CC90 — Browning HP, die Zeile des Nutzer-Reports */
/* 4*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 5*/ { 2,3,0, 2,3,0, 2,3,0 },   /* @0x8010CCD8 Magnum */
/* 6*/ { 2,3,0, 2,3,0, 2,3,0 },
/* 7*/ { 2,4,0, 2,1,0, 2,1,0 },   /* @0x8010CD20 Schrot */
/* 8*/ { 2,5,0, 2,6,0, 2,1,0 },   /* @0x8010CD44 Custom Schrot */
/* 9*/ { 2,5,5, 1,1,1, 1,1,1 },   /* @0x8010CD68 GL Explosiv */
/*10*/ { 1,1,1, 1,1,1, 1,1,1 },   /* @0x8010CD8C */
/*11*/ { 1,1,1, 1,1,1, 1,1,1 },   /* @0x8010CDB0 */
/*12*/ { 1,6,0, 1,6,0, 1,6,0 },   /* @0x8010CDD4 Bowgun (im Port nie gestempelt) */
/*13*/ { 1,1,0, 1,1,0, 1,1,0 },
/*14*/ { 1,1,0, 1,1,0, 1,1,0 },
/*15*/ { 1,1,0, 1,1,0, 1,1,0 },
/*16*/ { 1,1,0, 1,1,0, 1,1,0 },
/*17*/ { 2,5,0, 2,6,0, 2,1,0 },   /* @0x8010CE88 Rakete */
/*18*/ { 1,1,0, 1,1,0, 1,1,0 }    /* @0x8010CEAC, Spalte 8 = 0x8010CECC[0] = NULL */
};
static int s_re2z_last_death_handler = 0;   /* Port-Diagnose (Tests) — 0 = keine Zelle gelaufen */
int re15_re2z_last_death_handler(void) { return s_re2z_last_death_handler; }
/* Port-Diagnose: EINE Zelle der Tabelle (Tests pinnen sie gegen den Dump). -1 = ausserhalb. */
int re15_re2z_death_cell(unsigned row, unsigned col)
{
    if (row >= 19u || col >= 9u) return -1;
    return (int)re2z_death_tbl[row][col];
}

/* ---- FUN_80108A14 — der Zweig `+0x10E & 1` (der wiederbelebte Kriecher stirbt ein ZWEITES Mal).
 * Drei Phasen ueber +0x6 (`lbu v1,6(s0)` @0x80108A40, `beq v1,1 -> 0x80108BA4` @0x80108A48,
 * `slti v0,v1,2` @0x80108A50, `beq v1,zero -> 0x80108A80` @0x80108A5C, `beq v1,2 -> 0x80108BCC`
 * @0x80108A70). Phase 0 FAELLT DURCH in den Advancer (kein Sprung ueber @0x80108BA4). */
static void re2z_death_crawler(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:
        re2z_clip(e, 7, 0, 0xF, 0x100, 0);                         /* `lui v0,0xf / ori v0,v0,0x7`
                                                                    * @0x80108A60/80 -> Wort
                                                                    * 0x000F0007, Blend = a3 = 256
                                                                    * des 959c @0x80108BB4 */
        e->sub_state_2 = 1;                                        /* sb a1(=1),6 @0x80108A9C */
        e->re2z_flags21a &= (uint16_t)~0x4u;                       /* andi 0xfffb @0x80108AA0-A4 —
                                                                    * Leichen-Pose 23 (Ruecken) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80108AB4-B8 */
        re2z_gore_fx(e, 0, 8096u);                                 /* Part 0 (`addiu a2,a2,72`
                                                                    * @0x80108AD4), ofs y+300
                                                                    * @0x80108ABC-C4 */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);                     /* @0x80108AD8-F4 */
        /* jal 0x80018FB0 @0x80108AF8 (Praesentation, OPEN) */
        if (e->sub_state_1 == 10u && !(e->re2z_f10e & 0x80u))
            re2z_gore_burn(e);                                     /* @0x80108B00-28 */
        if (e->sub_state_1 == 11u && !(e->re2z_flags21a & 0x1000u))
            re2z_gore_acid(e);                                     /* @0x80108B34-54 */
        if ((e->sub_state_1 == 9u || e->sub_state_1 == 17u) && !(e->re2z_f10e & 0x80u))
            re2z_gore_soot(e);                                     /* @0x80108B58-88 */
        if (e->sub_state_1 == 14u) re2z_gore_spark(e);             /* @0x80108B8C-A0 */
        /* FALLTHROUGH — @0x80108BA4 ist nur das Sprungziel von Phase 1 */
        /* fall through */
    case 1:
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* `+0x6 += 959c(...,256)`
                                                                    * @0x80108BB0-C8 */
        break;
    case 2:
        re15_ai_set_state_word(e, 7u);                             /* `sw v0,4(s0)` @0x80108BCC mit
                                                                    * v0 = 7 (`addiu v0,zero,7`
                                                                    * @0x80108A74) -> CORPSE Sub 0 */
        break;
    default: break;                                                /* j 0x80108BD0 */
    }
}

/* ---- die Zerleger-Leiter des DEATH-HANDLERS @0x801086E4-808 -------------------------------
 * NICHT identisch mit re2z_dismember_row(): sie traegt (a) den IMMER laufenden Delay-Slot
 * `sb zero,363(s1)` @0x8010871C, (b) den Zusatz-Spray der Saeure-Zeile @0x80108728-8C und
 * (c) eine EIGENE Zeile-16-Fassung ohne Brand-Zaehler @0x801087DC-808. Reihenfolge = Original. */
static void re2z_death_dismember(re15_actor_t *e)
{
    unsigned row = e->sub_state_1;
    if (row == 10u && !(e->re2z_f10e & 0x80u))
        re2z_gore_burn(e);                                         /* @0x801086E4-70C */
    e->re2z_gaitrow = 0;                                           /* +0x16B: `sb zero,363` im
                                                                    * DELAY-SLOT von `bne v1,v0`
                                                                    * @0x80108718/1C = IMMER */
    if (row == 11u) {
        re2z_gore_acid(e);                                         /* @0x80108720-24 */
        if (re2z_rand() & 1u) {                                    /* @0x80108728-34 */
            re2z_gore_fx(e, 8, 0x040F0FA0u);                       /* Anker `addiu a2,a2,1448`
                                                                    * @0x80108784 = Part 8, Id
                                                                    * 0x040F0FA0 @0x80108740-44 */
            e->re2z_gaitrow = 1;                                   /* sb 1,363 @0x80108788-8C */
        }
    }
    if ((row == 9u || row == 17u) && !(e->re2z_f10e & 0x80u))
        re2z_gore_soot(e);                                         /* @0x80108790-C0 */
    if (row == 14u) re2z_gore_spark(e);                            /* @0x801087CC-D8 */
    if (row == 16u && !(e->re2z_f10e & 0x80u))
        re2z_gore_burn(e);                                         /* @0x801087DC-808 — OHNE
                                                                    * +0x23A-Zaehler und ohne
                                                                    * `+0x21A |= 0x800` */
}

/* ---- FUN_80108530 — DER NORMALE STURZ (Zellen RE2ZD_MAIN) ---------------------------------
 * Drei Phasen ueber +0x6 (`lbu v1,6(s1)` @0x80108564, `beq v1,1 -> 0x80108810` @0x8010856C,
 * `slti v0,v1,2` @0x80108574, `beq v1,zero -> 0x801085A4` @0x80108580, `beq v1,2 -> 0x80108918`
 * @0x80108594). Phase 0 FAELLT DURCH nach Phase 1 (@0x8010880C ist die letzte Zeile von Phase 0,
 * @0x80108810 die erste von Phase 1 — kein Sprung dazwischen). */
static void re2z_death_main(re15_actor_t *e)
{
    static const uint8_t clipsel[2] = { 1u, 2u };                  /* `sb 1,16(sp)` @0x8010855C /
                                                                    * `sb 2,17(sp)` @0x80108560,
                                                                    * gelesen `lbu a2,16(v0)`
                                                                    * @0x801085F0 */
    switch (e->sub_state_2) {
    case 0: {
        /* ---- die RICHTUNGSWAHL: ein RNG-Bit, erzwungen 0 wenn +0x21A Bit 0x2000 steht ---- */
        uint32_t r = re2z_rand();                                  /* jal 0x80015FE8 @0x801085A4 */
        e->re2z_dir16a = (uint8_t)(r & 1u);                        /* `andi v0,v0,0x1` @0x801085B0 /
                                                                    * `sb v0,362(s1)` @0x801085BC
                                                                    * (DELAY-SLOT = immer) */
        if (e->re2z_flags21a & 0x2000u) e->re2z_dir16a = 0;        /* `andi v1,v1,0x2000` @0x801085B4
                                                                    * / `sb zero,362` @0x801085C0 */
        e->re2z_flags21a &= (uint16_t)~0x4u;                       /* `andi v0,v0,0xfffb` @0x801085CC
                                                                    * / `sh` @0x801085D4 */
        if (e->re2z_dir16a != 0)
            e->re2z_flags21a |= 0x4u;                              /* `ori v0,v0,0x4` @0x801085D8 /
                                                                    * `sh v0,538` @0x801085DC —
                                                                    * waehlt spaeter Leichen-Pose 22 */
        e->re2z_self1d3 |= 0x80u;                                  /* `ori v1,v1,0x80` @0x80108604 /
                                                                    * `sb v1,467` @0x80108608 */
        e->sub_state_2 = 1;                                        /* `sb s0(=1),6(s1)` @0x80108610 */
        /* `+0x1C0 |= 1` @0x80108614-18 — Modell-/Kollisions-Byte, im Port nicht gefuehrt (OPEN) */

        /* ---- der Clip: Wort = ((+0x16A*5) << 9) + 0xF0000 + clipsel[+0x16A] ----
         *   8010861c: sll v0,a1,2 / 80108620: addu v0,v0,a1   -> dir*5
         *   80108624: sll v0,v0,9                             -> dir*2560 (Frame-Byte = dir*10)
         *   80108628: addu a2,a2,v1  (v1 = `lui v1,0xf` @0x8010860C)
         *   8010862c: addu v0,v0,a2 / 80108630: sw v0,332(s1)
         * dir 0 -> 0x000F0001 = Clip 1 ab Frame 0   (Sturz nach HINTEN)
         * dir 1 -> 0x000F0A02 = Clip 2 ab Frame 10  (Sturz nach VORNE)
         * Blend = a3 = 256 des 959c-Aufrufs @0x801088E8. */
        {   int dir = (int)(e->re2z_dir16a & 1u);
            re2z_clip(e, (int)clipsel[dir], dir * 10, 0xF, 0x100, 0);
        }

        /* ---- Blut: Zone-0 generisch an Part 1, sonst am Part 0 (`+0x1D2 % 3`, magisches
         * multu 0xAAAAAAAB @0x801085F4-4C / @0x80108634-4C) ---- */
        if (((unsigned)e->re2z_hits1d2 % 3u) != 0u)
            re2z_gore_fx(e, 0, 8096u);                             /* Id 8096 @0x8010866C, Anker
                                                                    * `addiu a2,s2,72` @0x80108690,
                                                                    * ofs y+300 @0x80108670 */
        else
            re2z_gore_fx(e, 1, 6096u);                             /* Id 6096 @0x80108694, Anker
                                                                    * `addiu a2,a2,244` @0x801086B0,
                                                                    * ofs {0,800,0} @0x801086A0-A8 */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);                     /* EIN Wurf @0x801086BC-D8 */
        /* jal 0x80018FB0(self) @0x801086DC (Praesentation, OPEN) */
        re2z_death_dismember(e);                                   /* @0x801086E4-808 */
        /* `sh 11,324(s1)` @0x8010880C ist ein DEAD STORE — derselbe Fall wie die drei schon
         * dokumentierten `sh 11,324`-Seeds (@0x80104824/@0x80104DC8/@0x80103614): der naechste
         * 0x80015E7C ueberschreibt +0x144 mit dem Clip-Root-Delta, bevor 0x800152C8 ihn liest. */
        /* FALLTHROUGH nach Phase 1 (@0x8010880C -> @0x80108810 ohne Sprung) */
    }
    /* fall through */
    case 1:
        /* Tropfblut-Fenster @0x80108810-D4: nur wenn `+0x1D2 % 3 != 0`, und nur solange der
         * Frame-Zaehler +0x14D in [dir*10+4, dir*10+16] liegt und UNGERADE ist
         * (`addiu v0,v1,4 / slt` @0x80108858-60, `addiu v0,v1,16 / slt` @0x80108864-6C,
         * `andi v0,a1,0x1` @0x80108870). Zwei RNG-Wuerfe je Tropfen (@0x801088B0/BC). */
        if (((unsigned)e->re2z_hits1d2 % 3u) != 0u) {
            int d  = (int)(e->re2z_dir16a & 1u) * 10;
            int fr = re2z_frame_slot(e);
            if (fr >= d + 4 && fr <= d + 16 && (fr & 1)) {
                uint32_t r1 = re2z_rand();                         /* @0x801088B0 */
                uint32_t r2 = re2z_rand();                         /* @0x801088BC — Winkel
                                                                    * `sll a1,v0,4` @0x801088C8;
                                                                    * der Port-Stand-in nimmt
                                                                    * +0x76 als Winkel */
                (void)r2;
                re2z_gore_fx(e, 0, (uint32_t)((r1 << 3) + 4048u)); /* Id `sll s0,v0,3` @0x801088B8
                                                                    * `addiu s0,s0,4048` @0x801088C0,
                                                                    * Anker `addiu a2,s2,72`
                                                                    * @0x801088D4, ofs y+500
                                                                    * @0x80108898 */
            }
        }
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* `+0x6 += 959c(...,256)`
                                                                    * @0x801088E4-FC */
        /* `if (+0x14E == 0) FUN_80016200(self,0,1)` @0x801088F0/F8 + @0x80108900-0C —
         * Matrix-/Positions-Nachzieher (Praesentation), im Port OPEN. */
        break;
    case 2:
        /* @0x80108918: `sw v0,4(s1)` im DELAY-SLOT des rand()-Aufrufs, v0 = 7 aus dem
         * `addiu v0,zero,7` @0x80108598 -> +0x4 = 7 (CORPSE, Sub 0). PLAIN 7, NICHT 0x907. */
        re15_ai_set_state_word(e, 7u);
        (void)re2z_rand();                                         /* der Wurf @0x80108918 gehoert
                                                                    * zur Wurfzahl und wird gezogen */
        /* Die WIEDERBELEBUNG als Kriecher haengt hinter fuenf Gates und ist im Port NICHT
         * portiert (es gibt keinen Kriecher-Executor — dieselbe dokumentierte Luecke wie an der
         * HURT-Wurzel @0x80104FE0-500C / @0x80105014-38). Die Gates, damit sie beim Nachziehen
         * vollstaendig sind:
         *   `andi v0,v0,0x3 / bne`  @0x80108920-24  3/4 -> raus
         *   `andi v0,v0,0x4 / bne`  @0x8010892C-38  +0x21A Bit 0x4 (VORWAERTS-Toter) -> raus
         *   `lh v0,268(s1) / bne`   @0x80108940-48  +0x10C != 0 -> raus (+0x10C hat im Port kein
         *                                           Feld, s. Ragdoll-P2 @0x80107784)
         *   `lb v0,363(s1) / bne`   @0x80108950-58  +0x16B != 0 -> raus
         *   `0x800CFBD8 & 0x10000000` @0x80108960-70 -> raus
         * Rumpf @0x8010895C-B0: HP = 1 (`sh 1,342` @0x80108980), Hitbox-Felder @0x80108984-A4,
         * `sh 0x2001,270` @0x801089B0 (= +0x10E Bit 0, der Eingang der 1D-Tabelle oben), dann
         * +0x4 = 0x201 bzw. das Vorgabewort (@0x801089C8-DC) und `+0x1D3 &= 0x7F` @0x801089E0-EC.
         * OPEN, mit Adressen. Bis dahin endet die Kette hier in CORPSE. */
        break;
    default: break;                                                /* j 0x801089F0 */
    }
}

static void re2z_death(re15_actor_t *e, re15_actor_t *pl)
{
    if (!(e->re2z_flags21a & 0x4000u)) {                           /* @0x80108260-6C */
        e->re2z_flags21a |= 0x4000u;                               /* kill latch @0x80108294-98 */
        /* the global kill counter 0x800D46C0++ (@0x8010827C-88) has no port equivalent */
    }
    e->re2z_flags21a &= (uint16_t)~0x200u;                         /* andi 0xfdff @0x801082A8-AC */
    re2z_grab_abort(e, pl);                                        /* @0x801082B0-328, claim clear
                                                                    * @0x801082F4 */
    /* `lbu v0,6(s0) / bne -> 0x801083B0` @0x80108328-34: nur in Phase 0 werden fuenf
     * Modell-Flagwoerter |= 0x8000 gesetzt (@0x80108338-AC) — Praesentation, PORT-OPEN. */

    if (e->re2z_f10e & 1u) {                                       /* ZWEIG 1 @0x801083B0-BC */
        s_re2z_last_death_handler = -1;                            /* 1D-Tabelle @0x8010CECC */
        re2z_death_crawler(e);
        return;
    }
    if (e->re2z_flags21a & 0x10u) {                                /* ZWEIG 2 @0x801083EC-F0 */
        /* FUN_801099E4 (Kriecher-Tod) hat im Port keinen Handler — dieselbe dokumentierte
         * Luecke wie an der HURT-Wurzel. Stand-in: die Clip-7-Kette, damit die Leiche entsteht. */
        s_re2z_last_death_handler = -2;
        re2z_death_crawler(e);
        return;
    }
    if (e->re2z_flags21a & 0x2u) {                                 /* ZWEIG 3 @0x80108408 */
        s_re2z_last_death_handler = -3;
        re2z_gore_fx(e, 0, 8000u);                                 /* Part 0 (`addiu a2,a2,72`
                                                                    * @0x80108420) @0x8010841C */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);                     /* @0x80108424-3C */
        re2z_dismember_row(e, 1);                                  /* @0x80108444-D0 */
        re15_ai_set_state_word(e, 0x907);                          /* `addiu v0,zero,2311 / sw`
                                                                    * @0x801084D8-DC */
        return;
    }

    {   /* ZWEIG 4 — der 2D-Dispatch @0x801084E0-518 */
        unsigned row = e->sub_state_1;
        unsigned col = e->re2z_hits1d2;
        uint8_t h = (row < 19u && col < 9u) ? re2z_death_tbl[row][col] : (uint8_t)RE2ZD_NULL;
        s_re2z_last_death_handler = (int)h;
        switch (h) {
        case RE2ZD_MAIN:
            re2z_death_main(e);
            return;
        default:
            /* RE2ZD_7438/92C4/66FC/8BEC/9610 = die ZERREISS-Tode (Magnum, Schrot, Rakete,
             * Sprenggranate). Ihre Handler sind noch nicht portiert — Adressen stehen im enum
             * oben. Bis dahin bleibt fuer diese Zellen das bisherige Port-Verhalten (die
             * Clip-7-Kette), damit dieser Fix fuer sie KEINE Regression ist.
             * Ebenso die NULL-Zellen: im Original waere das `jalr 0`, also unerreichbar
             * (PORT-SICHERUNG, kein eingefrorener Gegner). */
            re2z_death_crawler(e);
            return;
        }
    }
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
    /* ---- WELLE E: die Zerleger-Seeds des INIT ---- */
    e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;      /* `addiu v0,zero,13`
                                                                    * @0x8010081C, `sb v0,337/338/
                                                                    * 339(s2)` @0x80100820/24/28 */
    e->re2z_burn23a   = 0;                                         /* sb zero,570 @0x801008B4 */
    e->re2z_hitdir1d0 = 0;                                         /* sh zero,464 @0x801006BC */
    re15_re2z_part_reset(e);                                       /* Modellblock (PORT-MAPPING) */
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
     *   801008cc: sh  v1,342(s2)               ; +0x156 = HP = 250  (portiert, s.u.)
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
     * ⚠ +0x156 = 250 (@0x801008C8-CC) LIEGT NICHT HIER, SONDERN IN re15_damage.c
     * (re15_re2_init_hp -> re15_re2_hp_sync, gestempelt aus game_step). Grund: +0x156 ist das
     * HP-Halbwort, und der RE2-INIT wuerfelt es sonst aus einer Tabelle (`lhu v0,-14736(at)` /
     * `sh v0,342(s2)` @0x80100708-10, Tabelle 0x8010C670, Index (rand>>(rand&3))&0xf, +15 bei
     * Raum-Gegnerzahl < 4 @0x801007FC-814) — das ganze Modell gehoert an EINE Stelle, sonst
     * driften zwei Kopien derselben Regel auseinander. Der Stempel laeuft NACH dem INIT-Tick
     * und ersetzt die RE1.5-Live-INIT-HP.
     * ⛔ WICHTIG: die 250 und das Bit 0x8000 zwei Zeilen weiter unten sind DIESELBE
     * Verzweigung (`bne v1,v0` @0x801008BC). Wer eine der beiden anfasst, muss die andere
     * mitfuehren. Die frueher hier begruendete Auslassung ("nicht portiert, weil unbelegt, dass
     * RE1.5-Typ 0x11 dasselbe Wesen ist") ist am 2026-08-20 WIDERLEGT worden: beide Spiele
     * lesen den Typ aus ENTITY+0x8 (RE1.5 @0x801007d8), der Port indiziert den RE2-Asset-TOC
     * @0x8009ADF4 mit genau diesem Typ (re2_ems.c `((kind-0x10)*4+rec)*2`) und laedt fuer 0x11
     * eine EIGENE, andere EM011-Textur (313/66592 Bytes gleich zu EM010) — RE2-EM011 IST Brad
     * Vickers (BioModels.h:186). Volle Herleitung im Block ueber re15_re2_init_hp.
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
    if (e->type == 0x11) e->re2z_flags21a |= 0x8000u;              /* @0x801008BC-D4 — ZWILLING
                                                                    * der 250-HP-Zeile
                                                                    * @0x801008C8-CC (dieselbe
                                                                    * bne-Verzweigung); die HP-
                                                                    * Haelfte steht in
                                                                    * re15_re2_init_hp
                                                                    * (re15_damage.c). Beide sind
                                                                    * scharf — nie nur eine
                                                                    * aendern. */
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
    /* Die Bone-Injektion des Vor-Ticks verfaellt: das Original baut die Part-Matrizen in JEDEM
     * Advance (FUN_8002959C) neu und multipliziert danach absolut auf — nichts akkumuliert.
     * Der Port muss dieselbe Ein-Tick-Lebensdauer haben, sonst bleibt der Oberkoerper nach der
     * Reaktion verdreht stehen. Der Lean-VEKTOR (+0x13C..+0x140) bleibt erhalten, nur das
     * Anwenden wird pro Tick neu bestellt. */
    e->re2_lean_on    = 0;
    e->re2_bone0_wgt  = 0;

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

    /* ============================================================================================
     * ⛔ DIE TREFFBARKEIT IM RE2-ZWEIG IST +0x1D3 — NICHT DER RE1.5-LATCH +0x93
     * --------------------------------------------------------------------------------------------
     * NUTZER-REPORT 2026-08-19: "Nach ein paar Treffern schwanken die Zombies nach hinten. Beim
     * Original wuerden sie dann umfallen, wenn man sie noch mal trifft. Bei uns nicht."
     *
     * GEMESSEN (probe_re2_stagger, ROOM1140, echter Weg game_step + R1/SQUARE, RE2-Bank geladen):
     *   f132 Treffer -> +0x222 = 1, Stagger-P0 (Clip 4, Schub -450 = "nach hinten schwanken")
     *   f132..f174   -> Stagger-P1, +0x93 = 0x01 die GANZE Zeit
     *   f175         -> P2: +0x223 neu geseedet, +0x222 = 0
     *   f187         -> erst JETZT wieder ein Treffer (11 Frames nach der Latch-Freigabe)
     * Der Flinch 0x501 (@0x801050A4) — der einzige TREFFER-Weg in den Sturz-Executor EXEC[5]
     * (der zweite 0x501-Produzent @0x80102D24-2C ist der Griff-Ausgang, kein Treffer) — wurde in
     * 900 Frames NIE erreicht: sein Gate ist `+0x222 == 1` (@0x80105080-98), und +0x222 ist
     * ausschliesslich WAEHREND des Staggers 1 (gesetzt @0x80105164, geloescht in Stagger-P2
     * @0x80106034 bzw. EXEC[5]-P0 @0x801032F4). Es braucht also einen Treffer MITTEN in der
     * Reaktion — und genau den hat der Port abgewiesen.
     *
     * ---- WAS DAS ORIGINAL TUT (selbst disassembliert, info/re2leon/PSX.EXE) --------------------
     * Die RE2-Trefferschleife FUN_800470C0 filtert ihre Kandidaten so:
     *   80047124: lw   v0,0(s0)          ; Entity-Flagwort
     *   8004712c: andi v0,v0,0x1
     *   80047130: beq  v0,zero,0x8004740c ; inaktiv -> naechster Kandidat
     *   80047138: lbu  v0,467(s0)        ; +0x1D3
     *   8004713c: nop
     *   80047140: bne  v0,zero,0x8004740c ; **+0x1D3 != 0 -> Kandidat UEBERSPRUNGEN**
     *   80047148: lh   v0,342(s0)        ; HP < 0 -> ueberspringen …
     * RE2 kennt kein +0x93: eigener Voll-Scan aller `sb/lbu rt,467(rs)` in EMOVL10_S0.BIN zeigt,
     * dass WEDER der Haupt-Handler 0x80105438 NOCH der Stagger 0x80105BC0 +0x1D3 je schreiben —
     * der Zombie bleibt in beiden Reaktionen TREFFBAR. Gesetzt wird +0x1D3 |= 0x80 nur dort, wo
     * der Port es auch schon setzt: Ragdoll-P0 @0x801069A4-B0, Knockdown-P0 @0x801074F0-504,
     * EXEC[5]-P0 @0x80103304, EXEC[6] @0x801039F8 — und passend wieder geloescht.
     *
     * ---- WARUM DER PORT ES ANDERS MACHTE ------------------------------------------------------
     * Der Port faehrt in BEIDEN Flavors den RE1.5-Resolver FUN_80011f50, dessen Ein-Treffer-Latch
     * +0x93 Bit 0 (@0x800124E8/F0) einen Kandidaten bis zur Freigabe abweist. Die Freigabe wurde
     * an den RE1.5-Ort gehaengt (Reaktions-ENDE, @0x80105f9c-fac) — damit war der RE2-Zombie
     * fuer die gesamte Reaktion unverwundbar und der Flinch-Zweig toter Code.
     * PORT-VERTRAG, kein geratener Wert: der Latch, den der geteilte Resolver liest, SPIEGELT
     * jetzt exakt den Original-Filter — treffbar genau dann, wenn +0x1D3 == 0. Die bestehenden
     * Freigaben an den Reaktions-Ausgaengen bleiben stehen (sie sind dieselbe Aussage) .
     *
     * ⛔ KORREKTUR 2026-08-20 — DER SPIEGEL WAR UNVOLLSTAENDIG UND EINSEITIG.
     * Nutzer-Report v0.3.3: "(a) manchmal fallen sie einfach so hin", "(b) stehen mitten in der
     * Hinfall-Animation wieder auf", "(e) beim Anschiessen und Sterben landen sie weitestgehend
     * animationslos am Boden".
     * GEMESSEN (probe_re2_zfall Modus 4, ROOM1140, echter Weg game_step + R1/SQUARE, RE2-Bank
     * EM010 geladen — Dauerfeuer Pistole):
     *   f165 Treffer -> st=3 DEATH, s1=3, s2=1, clip 7 (Laenge 70) — die Todes-Anim startet
     *   f176 TREFFER auf die LEICHE (hp -1 -> -17), Todes-Phase s2 faellt 1 -> 0
     *   f187/198/209/220/231 fuenf WEITERE Treffer, hp bis -97, s2 bleibt bei 0 haengen
     * Der Port beschiesst also die Leiche weiter und setzt dabei jedes Mal die Todes-Phase zurueck.
     *
     * ---- DER ORIGINAL-FILTER HAT VIER GATES, DER PORT SPIEGELTE EINS -------------------------
     * FUN_800470C0, Kandidaten-Schleife (info/re2leon/PSX.EXE, selbst disassembliert):
     *   80047124: lw   v0,0(s0)          ; Entity-Flagwort
     *   8004712c: andi v0,v0,0x1
     *   80047130: beq  v0,zero,0x8004740c ; (1) inaktiv                 -> UEBERSPRUNGEN
     *   80047138: lbu  v0,467(s0)         ; +0x1D3
     *   80047140: bne  v0,zero,0x8004740c ; (2) +0x1D3 != 0             -> UEBERSPRUNGEN
     *   80047148: lh   v0,342(s0)         ; +0x156 = HP
     *   80047150: bltz v0,0x8004740c      ; (3) HP < 0 (TOT)            -> UEBERSPRUNGEN
     *   80047158: lhu  v0,270(s0)         ; +0x10E
     *   80047160: andi v0,v0,0xc000
     *   80047164: bne  v0,zero,0x8004740c ; (4) +0x10E & 0xC000 gesetzt -> UEBERSPRUNGEN
     *   8004716c: lhu  v1,464(s0)         ; erst DANACH beginnt die Trefferpruefung
     * 717d13e0 hat NUR Gate (2) gespiegelt. Gate (3) fehlte -> Leichen bleiben beschiessbar;
     * Gate (4) fehlte -> der Limpet-/Liege-Latch 0x4000 und das Schon-gefallen-Bit schuetzen nicht.
     *
     * ---- UND DER SPIEGEL WAR EINSEITIG --------------------------------------------------------
     * Der alte Code konnte den Latch nur FREIGEBEN (`&= ~1`), nie SETZEN. Damit war jeder Zombie
     * ohne +0x1D3 in JEDEM Frame treffbar — auch tot, auch liegend, auch mitten in einer laufenden
     * Reaktion. Der geteilte RE1.5-Resolver blockiert einen Kandidaten aber ausschliesslich ueber
     * den gesetzten Latch (@0x800123fc-418 Zweitkontakt-Rekursion, Port re15_damage.c:1154-1156);
     * ein Filter, der nur freigibt, ist deshalb GAR KEIN Filter. Beide Richtungen sind noetig.
     *
     * ---- WARUM BIT 1 (+0x93 & 2) HIER MITGELOESCHT WIRD ---------------------------------------
     * Das Blockieren ueber Bit 0 zieht im RE1.5-Resolver zwangslaeufig `+0x93 |= 2` nach sich
     * (@0x8001240c / @0x80012fcc, Port re15_damage.c:1155/1373) — und Bit 1 ist der Ausloeser des
     * RE1.5-Gore-Spawns FUN_80106a44 (re15_enemy_gore_tick, re15_damage.c:1411), der an der
     * AKTOR-WURZEL e->x/y/z spawnt, also AM BODEN ZWISCHEN DEN FUESSEN. Das ist der Nutzer-Befund
     * (d) "manchmal taucht das Blut an der richtigen Stelle auf, und manchmal beim Bein unten".
     * RE2 hat das Feld +0x93 UEBERHAUPT NICHT: eigener Voll-Scan beider RE2-Binaries nach
     * `sb/lbu rt,147(rs)` liefert 0 Treffer in info/re2leon/PSX.EXE UND in EMOVL10_S0.BIN
     * (Beleg schon im Kopf von re2z_hit_latch_release). Ein +0x93-getriebener Wurzel-Gore ist im
     * RE2-Gehirn also definitionsgemaess FREMDCODE. Die RE2-Blutquellen sind die drei
     * anker-gebundenen Emitter (@0x801050B0-158 / @0x80105650-704 / @0x80105D14-DBC), und die
     * laufen bereits ueber re2z_blood_fx_at an der Knochen-Matrix. Deshalb: Bit 1 gehoert bei
     * einem RE2-eigenen Zombie in JEDEM Tick auf 0 — sonst erzeugt ausgerechnet der korrekte
     * Filter das Fuss-Blut. */
    {   int hittable = (e->active != 0)                            /* (1) +0x0 & 1   @0x80047124-30 */
                    && (e->re2z_self1d3 == 0u)                     /* (2) +0x1D3     @0x80047138-40 */
                    && (e->hp >= 0)                                /* (3) HP < 0     @0x80047148-50 */
                    && !(e->re2z_f10e & 0xC000u);                  /* (4) +0x10E     @0x80047158-64 */
        if (hittable) e->hit_react &= (uint8_t)~1u;                /* Kandidat freigegeben */
        else          e->hit_react |= (uint8_t)1u;                 /* Kandidat UEBERSPRUNGEN */
        e->hit_react &= (uint8_t)~2u;                              /* RE2 kennt +0x93 nicht ->
                                                                    * kein RE1.5-Wurzel-Gore */
    }
    return 1;
}
