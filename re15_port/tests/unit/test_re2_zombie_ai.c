/* test_re2_zombie_ai.c — pins the RE2 zombie brain (OPTIONS -> AI -> RE2) to the RE2 overlay bytes.
 *
 * Source of truth: info/re2leon/COMMON/BIN/EMZ0.BIN, loaded RAW @0x80100000. Disassemble with
 *     .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMZ0.BIN
 *
 * W1 — the walk TURN gate (@0x80101BAC..0x80101CAC), self-disassembled:
 *     80101bac: lw    v0,496(s1)        ; +0x1F0 = distance to the player
 *     80101bb4: sltiu v0,v0,0x1389      ; dist < 5001 ?
 *     80101bbc: addiu v1,zero,8         ;   YES -> rate +8, MONOTONE toward the steer point
 *     80101bc0: lb    v0,363(s1)        ;   NO  -> gait row +0x16B
 *     80101bd4: lhu   v0,-14044(at)     ;          tbl16[row] @0x8010C924
 *     80101bdc: srl   v0,v0,15          ;          bit15
 *     80101be8: subu  v1,v1,v0<<4       ;          rate = 8 - (bit15 ? 16 : 0) -> +8 / -8 = WEAVE
 *     80101c94: sltiu v0,v0,0xbb8       ; dist < 3000 ?
 *     80101cac: addiu a3,zero,16        ;   -> an EXTRA rate-16 turn toward the same point
 *
 * The RE1.5 default must stay untouched: flavor RE15 is the boot value and nothing here changes it
 * permanently (the 96 other byte-true tests all run on the RE1.5 brain).
 */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern const uint16_t re15_re2z_gait_tbl[32];
extern int re15_re2z_walk_turn(re15_actor_t *e, int32_t px, int32_t pz, uint32_t dist);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- WELLE B: SE capture hook (records what the brain would send to ENEMSE) -------------- */
static int  se_log[128]; static int se_n = 0;
static int  se_bank_sel = -1;
static void se_cap(int id, int flag2000) { (void)flag2000; if (se_n < 128) se_log[se_n++] = id; }
static void se_bank_cap(int bank) { se_bank_sel = bank; }
static int  se_last(void) { return se_n ? se_log[se_n - 1] : -1; }
static int  se_seen(int id) { for (int i = 0; i < se_n; i++) if (se_log[i] == id) return 1; return 0; }

/* One tick of the RE2 walk turn from rot_y=0 with the steer point far off-heading, so the residual
 * always exceeds the rate and the observed rot_y delta IS the rate. The gait machine is pre-seeded
 * (init done, timer still running) so this tick only exercises the TURN, not the row advance. */
static int turn_delta(unsigned gaitrow, unsigned dist)
{
    re15_actor_t e;
    for (unsigned i = 0; i < sizeof(e); i++) ((unsigned char *)&e)[i] = 0;
    e.x = 0; e.z = 0; e.rot_y = 0;
    e.re2z_gaitrow  = (uint8_t)gaitrow;
    e.re2z_gaitinit = (uint8_t)(0x80u | (e.sub_state_1 & 0x7fu));  /* "walk already entered" tag */
    e.re2z_gaittmr  = 999;                 /* far from expiry -> the row under test stays put */
    re15_re2z_walk_turn(&e, 0, -10000, dist);
    return (((int)e.rot_y + 0x800) & 0xfff) - 0x800;
}


/* ---- W2: the attack-decision ladder @0x80101714 ------------------------------------------ */
static re15_re2z_gates_t base_gates(void)
{
    re15_re2z_gates_t g;
    for (unsigned i = 0; i < sizeof(g); i++) ((unsigned char *)&g)[i] = 0;
    g.dist = 60000u;          /* far: below every distance gate's threshold is FALSE */
    g.arc1024 = 1; g.arc512 = 1;   /* player OUTSIDE both cones */
    g.pl_8 = 0;               /* take the G branch, not J */
    g.pl_156 = 0;             /* K off */
    return g;
}

static void test_ladder(void)
{
    re15_re2z_gates_t g; re15_re2z_decision_t d;

    /* nothing fires -> +0x4 untouched (the original simply does not store) */
    g = base_gates();
    CHECK(re15_re2z_decide_walk(&g, &d) == 0 && d.wrote == 0 && d.rng_draws == 0,
          "no block fires -> no store, no RNG draw");

    /* --- A is the ONLY early exit (j 0x80101a1c @0x801017E0) -------------------------------- */
    g = base_gates();
    g.self_1f4 = 0x40000000u | 1999u; g.a_sector_hit = 1;
    g.pl_156 = (int16_t)-32768; g.arc512 = 0; g.dist = 999u;   /* K would fire and would WIN... */
    CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00000E01u && d.early_out == 1,
          "block A must return BEFORE K can overwrite (@0x801017e0), got word 0x%08x early=%d",
          d.word, d.early_out);
    /* the 2000 bound is on the MASKED payload, not the raw word (@0x801017c0 after andi 0x3fffffff) */
    g.self_1f4 = 0x40000000u | 2000u;
    CHECK(!(re15_re2z_decide_walk(&g, &d) && d.early_out),
          "block A payload 2000 must FAIL the sltiu 0x7d0 bound");

    /* --- LAST WRITER WINS: C fires, then K overwrites it ------------------------------------ */
    g = base_gates();
    g.self_1d4 = (int16_t)0x4000; g.self_110 = 1u;             /* C -> 0x0A01 @0x80101858 */
    g.pl_156 = (int16_t)-32768; g.arc512 = 0; g.dist = 999u;   /* K  -> 0x00060801 @0x80101a10 */
    CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00060801u && d.set_10e_4000 == 1,
          "K is the last writer and must beat C (got 0x%08x)", d.word);

    /* C alone */
    g = base_gates(); g.self_1d4 = (int16_t)0x8000; g.self_110 = 3u;
    CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00000A01u, "C alone -> 0x0A01");
    g.self_110 = 2u;   /* bit0 clear */
    CHECK(!re15_re2z_decide_walk(&g, &d), "C needs self+0x110 bit0 (@0x8010184c)");

    /* --- D and E are SEQUENTIAL: under 2500 BOTH gates run and BOTH draw -------------------- */
    g = base_gates(); g.dist = 2499u; g.global_cfbf6 = 0x04u;  /* bit in BOTH masks */
    re15_re2z_rng_reset();
    re15_re2z_decide_walk(&g, &d);
    CHECK(d.rng_draws == 2, "under 2500 both D and E must draw (got %d)", d.rng_draws);
    /* between 2500 and 3500 only D draws */
    g.dist = 2500u;
    re15_re2z_rng_reset();
    re15_re2z_decide_walk(&g, &d);
    CHECK(d.rng_draws == 1, "at 2500 only D draws (sltiu 0x9c4 is STRICT), got %d", d.rng_draws);
    /* at/above 3500 neither draws */
    g.dist = 3500u;
    re15_re2z_rng_reset();
    re15_re2z_decide_walk(&g, &d);
    CHECK(d.rng_draws == 0, "at 3500 neither draws (sltiu 0xdac is STRICT), got %d", d.rng_draws);
    /* the mask difference is REAL: bit 0x2 is in 0x17 (E) but NOT in 0x15 (D) */
    g = base_gates(); g.dist = 2000u; g.global_cfbf6 = 0x02u;
    re15_re2z_rng_reset();
    re15_re2z_decide_walk(&g, &d);
    CHECK(d.rng_draws == 1, "global bit 0x2 must gate E only, not D (masks 0x15 vs 0x17), got %d",
          d.rng_draws);
    /* the cone gate: D/E need the player OUTSIDE +-1024 (arc != 0) */
    g = base_gates(); g.dist = 2000u; g.global_cfbf6 = 0x04u; g.arc1024 = 0;
    re15_re2z_rng_reset();
    re15_re2z_decide_walk(&g, &d);
    CHECK(d.rng_draws == 0, "inside the +-1024 cone D/E must not even draw (@0x80101868)");

    /* --- G: two INDEPENDENT halves, and it claims the player -------------------------------- */
    g = base_gates(); g.dist = 1199u; g.g1_sector_hit = 1;
    CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00000301u && d.claim_player == 1,
          "G1 -> 0x0301 + claims the player");
    g.g1_sector_hit = 0; g.g2_sector_hit = 1;
    CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00000301u,
          "G2 fires independently of G1 (it is a fall-through, not an else)");
    g.self_21a = 0x40u;                       /* G2 masked off */
    CHECK(!re15_re2z_decide_walk(&g, &d), "self+0x21A bit 0x40 masks G2 (@0x80101974)");
    g.pl_1d3 = 0x80u; g.self_21a = 0; g.g2_sector_hit = 1;
    CHECK(!re15_re2z_decide_walk(&g, &d), "an already-claimed player blocks G (@0x80101908)");

    /* --- the G/J fork: pl+0x8 == 15 takes J, and J skips the +0x106 test -------------------- */
    g = base_gates(); g.pl_8 = 15; g.arc512 = 0; g.dist = 1999u;
    CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00000E01u,
          "pl+0x8==15 -> J fires with self+0x106 == pl+0x106 (B could not)");
    g.pl_8 = 0;
    CHECK(!re15_re2z_decide_walk(&g, &d),
          "with pl+0x8 != 15 the same state must NOT fire (B needs 0x106 to DIFFER)");
}

int main(void)
{
    /* ---- the flavor switch: RE1.5 is and stays the default ------------------------------ */
    CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE15, "boot flavor must be RE15 (byte-true default)");

    /* ---- only the zombie family ever leaves the RE1.5 brain ------------------------------ */
    CHECK(re15_re2z_owns_type(0x10) && re15_re2z_owns_type(0x11) && re15_re2z_owns_type(0x12) &&
          re15_re2z_owns_type(0x13) && re15_re2z_owns_type(0x16) && re15_re2z_owns_type(0x18),
          "the six RE1.5 zombie types must be owned by the RE2 brain");
    CHECK(!re15_re2z_owns_type(0x20) && !re15_re2z_owns_type(0x21) && !re15_re2z_owns_type(0x26) &&
          !re15_re2z_owns_type(0x27) && !re15_re2z_owns_type(0x40),
          "dog/crow/spider/gorilla/NPC must NEVER leave the RE1.5 brain");

    /* ---- gait table @0x8010C924, byte-verified against EMZ0.BIN -------------------------- */
    static const uint16_t expect[32] = {
          190, 32818,   150, 32832,   110, 32832,   180, 32820,
          140, 32820,   120, 32826,   150, 32846,   140, 32818,
          150, 32830,   150, 32840,   110, 32838,   120, 32842,
          180, 32802,   110, 32820,   200, 32834,   180, 32834,
    };
    for (int i = 0; i < 32; i++)
        CHECK(re15_re2z_gait_tbl[i] == expect[i],
              "gait tbl[%d] = %u, expected %u (@0x8010C924)", i, re15_re2z_gait_tbl[i], expect[i]);

    /* ---- W1a: dist >= 5001 -> WEAVE, sign taken from the gait row's bit15 (@0x80101BDC) --- */
    for (int row = 0; row < 32; row++) {
        int bit15 = (expect[row] >> 15) & 1;
        int want  = bit15 ? -8 : 8;                       /* 8 - (bit15 ? 16 : 0) @0x80101BE8 */
        int got   = turn_delta((unsigned)row, 5001);
        CHECK(got == want, "far turn row %d: got %d, expected %d (bit15=%d)", row, got, want, bit15);
    }

    /* ---- W1b: dist < 5001 -> MONOTONE +8, the gait row must NOT matter (@0x80101BEC) ------ */
    for (int row = 0; row < 32; row += 3)
        CHECK(turn_delta((unsigned)row, 5000) == 8,
              "lock-on turn row %d: got %d, expected +8", row, turn_delta((unsigned)row, 5000));

    /* the 5001 boundary itself: sltiu is STRICT, so 5001 is already the far branch. Row 1's bit15
     * IS set (32818 = 0x8032), so there the far branch steers AWAY -> the boundary shows up as a
     * SIGN FLIP, which is the sharpest possible probe of the gate. */
    CHECK(turn_delta(1, 5000) == 8 && turn_delta(1, 5001) == -8,
          "sltiu 0x1389 boundary: 5000 -> +8 (lock-on), 5001 -> -8 (weave away); got %d / %d",
          turn_delta(1, 5000), turn_delta(1, 5001));

    /* ---- the gait machine: row advance + segment timer (@0x80101B2C-90) ------------------- */
    {
        re15_actor_t e;
        for (unsigned i = 0; i < sizeof(e); i++) ((unsigned char *)&e)[i] = 0;
        re15_re2z_gait_init(&e);                                     /* @0x80101A7C-AC */
        CHECK((e.re2z_gaitrow & 1) == 0 && e.re2z_gaitrow < 32,
              "init row must be EVEN and < 32 ((rand & 0xf) << 1 @0x80101A90/94), got %u",
              e.re2z_gaitrow);
        CHECK(e.re2z_gaittmr >= (expect[e.re2z_gaitrow] & 0x7fff) &&
              e.re2z_gaittmr <= (expect[e.re2z_gaitrow] & 0x7fff) + 255,
              "init timer = duration + a FULL rand byte (@0x80101AC0/C8), got %u for row %u",
              e.re2z_gaittmr, e.re2z_gaitrow);

        /* run the timer out: the row must step by exactly 1 and reload from the NEW row */
        unsigned row0 = e.re2z_gaitrow;
        e.re2z_gaittmr = 1;
        re15_re2z_walk_turn(&e, 0, -10000, 9999);   /* 1 -> 0 */
        CHECK(e.re2z_gaitrow == row0, "row must not advance while the timer is still running");
        re15_re2z_walk_turn(&e, 0, -10000, 9999);   /* 0 -> reroll */
        CHECK(e.re2z_gaitrow == ((row0 + 1) % 32),
              "row must step by 1 with a wrap at 32 (@0x80101B40/50/5C), got %u after %u",
              e.re2z_gaitrow, row0);
        unsigned dur = expect[e.re2z_gaitrow] & 0x7fff;
        CHECK(e.re2z_gaittmr >= dur && e.re2z_gaittmr <= dur + 31,
              "reload timer = duration + (rand & 0x1F) (@0x80101B80/84/90), got %u for row %u",
              e.re2z_gaittmr, e.re2z_gaitrow);

        /* the wrap itself */
        e.re2z_gaitrow = 31; e.re2z_gaittmr = 0;
        re15_re2z_walk_turn(&e, 0, -10000, 9999);
        CHECK(e.re2z_gaitrow == 0, "row 31 must wrap to 0 (@0x80101B5C), got %u", e.re2z_gaitrow);
    }

    /* ---- the RE2 PRNG @0x80015FE8, seeded 0xD2706CA4 (@0x8002B908-1C / @0x8003BCB0-C4) ----- */
    {
        /* hand-computed first draw: s=0xD2706CA4 -> h=(s>>7)&0xff=0xD9 -> lo=(0xD9+0xA4)&0xff=0x7D
         *                           -> v=0xD97D -> RETURN 0x7D, STATE 0xD97D                    */
        re15_re2z_rng_reset();
        re15_actor_t e;
        for (unsigned i = 0; i < sizeof(e); i++) ((unsigned char *)&e)[i] = 0;
        re15_re2z_gait_init(&e);
        CHECK(e.re2z_gaitrow == (uint8_t)((0x7Du & 0x0f) << 1),
              "first PRNG draw must be 0x7D -> row %u, got %u", (0x7Du & 0x0f) << 1, e.re2z_gaitrow);
        /* the generator must NOT be a fixed point (state 0 would freeze it forever) */
        re15_re2z_rng_reset();
        re15_actor_t f; for (unsigned i = 0; i < sizeof(f); i++) ((unsigned char *)&f)[i] = 0;
        re15_re2z_gait_init(&f);
        CHECK(f.re2z_gaitrow == e.re2z_gaitrow && f.re2z_gaittmr == e.re2z_gaittmr,
              "re15_re2z_rng_reset must make the sequence reproducible");
    }

    /* ---- W1c: dist < 3000 -> an ADDITIONAL rate-16 turn on top (@0x80101C94/CAC) ---------- */
    CHECK(turn_delta(0, 2999) == 8 + 16,
          "close turn: got %d, expected 24 (8 @0x80101BBC + 16 @0x80101CAC)", turn_delta(0, 2999));
    CHECK(turn_delta(0, 3000) == 8,
          "sltiu 0xbb8 boundary: 3000 must NOT get the extra turn (got %d)", turn_delta(0, 3000));

    /* =======================================================================================
     * WELLE B — the full brain (re15_re2z_tick): gates wired to the closed producers, the
     * attack executors, the grab phases, HURT/DEATH/CORPSE/state 8. All thresholds cited in
     * enemy_ai_re2_zombie.c; the checks below pin the PORT to those citations.
     * ===================================================================================== */
    re15_re2z_audio_hook(se_cap, se_bank_cap);
    CHECK(se_bank_sel == 0, "default ENEMSE bank must be 0 (probe-selected; RE2Z_ENEMSE_BANK)");

    /* ---- fill_gates: the Welle-B producers ------------------------------------------------ */
    {
        re15_actor_t e, p; memset(&e, 0, sizeof e); memset(&p, 0, sizeof p);
        e.re2z_cd23e = 5; e.re2z_flags21a = 0x4030; e.contact_flags = 2;
        re15_re2z_gates_t g;
        re15_re2z_fill_gates(&e, &p, 0, &g);
        CHECK(g.self_23e == 5,      "gate +0x23E must read re2z_cd23e (@0x80101790)");
        CHECK(g.self_21a == 0x4030, "gate +0x21A must read re2z_flags21a (@0x80101928)");
        CHECK(g.self_110 == 1,      "gate +0x110 bit0 must read the contact result (@0x80101844)");
        e.contact_flags = 0;
        re15_re2z_fill_gates(&e, &p, 0, &g);
        CHECK(g.self_110 == 0, "no contact -> +0x110 bit0 clear");
    }

    /* ---- the +0x23E cooldown gates blocks B (and A/J) in the ladder ----------------------- */
    {
        re15_re2z_gates_t g = base_gates(); re15_re2z_decision_t d;
        g.arc512 = 0; g.dist = 1999u; g.self_106 = 1; g.pl_106 = 0; g.pl_1d3 = 0;
        g.self_23e = 1;                                /* cooling down -> bne @0x8010179c */
        CHECK(!re15_re2z_decide_walk(&g, &d), "B must be gated while +0x23E != 0");
        g.self_23e = 0;
        CHECK(re15_re2z_decide_walk(&g, &d) && d.word == 0x00000E01u,
              "B fires once the cooldown expired (-> EXEC[14])");
    }

    /* ---- full-brain fixtures over g_actors ------------------------------------------------ */
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->x = 200; pl->z = 0; pl->floor = 0;

    /* ---- root prolog: the cooldown bank decrements (@0x8010045C-98) ----------------------- */
    {
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->state = 1; e->sub_state_1 = 4;              /* EXEC[4] = jr ra -> a pure prolog tick */
        e->re2z_cd239 = 3; e->re2z_cd23e = 2; e->re2z_self1d3 = 0x8F;   /* bit7 + low7 = 15 */
        re15_re2z_tick(s);
        CHECK(e->re2z_cd239 == 2 && e->re2z_cd23e == 1,
              "root prolog must decrement +0x239/+0x23E (@0x8010045C-6C/@0x80100470-80)");
        CHECK(e->re2z_self1d3 == 0x8E,
              "self+0x1D3 low-7 decrement must PRESERVE bit 7 (@0x80100484-98), got 0x%02X",
              e->re2z_self1d3);
        re15_actor_free(s);
    }

    /* ---- GRAB phases P0..P4 + bite damage (EXEC[3] @0x801025EC) --------------------------- */
    {
        re15_re2z_rng_reset(); se_n = 0;
        pl->hp = 100; pl->hit_react = 0; pl->state = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->x = 0; e->z = 0; e->rot_y = 0; e->hp = 60;
        e->state = 1; e->sub_state_1 = 3; e->sub_state_2 = 0; e->ai_dist = 200;
        re15_re2z_tick(s);                                       /* P0 @0x801026C0 */
        CHECK(e->sub_state_2 == 1, "grab P0 -> phase 1 (@0x801026D4)");
        CHECK(e->motion == 0x0B, "upright grab clip must be 0x0B (param[0x0C], @0x801026C4-CC)");
        CHECK(e->re2z_bitefr == 16 && e->re2z_bitedmg == 20,
              "upright bite pair must be (16,20) (param @0x80100014)");
        CHECK(e->re2z_self1d3 == 15, "self+0x1D3 = 15 at P0 (@0x8010276C-70)");
        CHECK((e->ai_flags & 1u) && e->grab_choreo, "grab latches (word0|=0x1000 @0x80102768-88)");
        CHECK(re15_player_is_grabbed(), "P0 must claim/pin the player (@0x8010275C-60)");
        CHECK(pl->hit_react & 1, "PL hit-guard set with the cmd-5 latch");
        re15_re2z_tick(s);                                       /* P1: no bank -> clip done */
        CHECK(e->sub_state_2 == 2, "grab P1 -> phase 2 on clip end (@0x80102800-10)");
        re15_re2z_tick(s);                                       /* P2 @0x80102814 */
        CHECK(e->sub_state_2 == 3 && e->motion == 0x0C,
              "P2: bite clip = grab+1 (@0x80102830-34), phase 3");
        CHECK(e->re2z_t158 == 148, "P2 struggle budget = 148 (@0x80102828-2C)");
        e->anim_frame = 16;                                      /* the byte-cited bite frame */
        int16_t hp0 = pl->hp;
        re15_re2z_tick(s);                                       /* P3 bite @0x801028A0-FC */
        CHECK(pl->hp == (int16_t)(hp0 - 20), "bite damage 20 (param @0x80100015), HP %d->%d",
              hp0, pl->hp);
        CHECK(se_last() == 3, "bite SE 3 (@0x801028E8-F0), got %d", se_last());
        CHECK(e->re2z_t158 == 146, "budget -2 per un-mashed tick (@0x80102868-7C)");
        e->re2z_t158 = 1; e->anim_frame = 0;
        re15_re2z_tick(s);                                       /* budget 1-2 = -1 -> throw */
        CHECK(e->sub_state_2 == 4, "budget < 0 -> phase 4 (@0x80102884-94)");
        re15_re2z_tick(s);                                       /* P4 @0x80102968 */
        CHECK(e->sub_state_2 == 5 && e->motion == 0x0D,
              "P4: throw clip = grab+2 (@0x8010297C-A0), phase 5");
        /* the expiry tick FALLS THROUGH into the bite check (@0x80102884 skips only the
         * phase-4 writes — Review #6): budget out ON the bite frame still bites */
        e->sub_state_2 = 3; e->re2z_t158 = 1; e->anim_frame = 16;
        {
            int16_t hpx = pl->hp;
            re15_re2z_tick(s);
            CHECK(e->sub_state_2 == 4 && pl->hp == (int16_t)(hpx - 20),
                  "expiry tick still bites (@0x80102898 fall-through), phase %d hp %d->%d",
                  e->sub_state_2, hpx, pl->hp);
        }
        /* P9 exits with word 0x1 = STAND, NO SE (`addiu v0,1; sw v0,4` @0x80102EB4-B8) */
        se_n = 0;
        e->state = 1; e->sub_state_1 = 3; e->sub_state_2 = 9;
        re15_re2z_tick(s);
        CHECK(e->state == 1 && e->sub_state_1 == 0,
              "P9 -> word 0x1 (STAND), got %d/%d", e->state, e->sub_state_1);
        CHECK(!se_seen(4), "P9 plays NO SE (the SE 4 lives in the P8 partner block)");
        re15_actor_free(s);
    }

    /* ---- P8 partner-domino (@0x80102DF0-EB0): frames 7..24 wake the bumped partner --------- */
    {
        se_n = 0;
        int s1 = re15_actor_alloc(0x10);
        int s2 = re15_actor_alloc(0x10);
        re15_actor_t *ez = &g_actors[s1], *pz = &g_actors[s2];
        ez->state = 1; ez->sub_state_1 = 3; ez->sub_state_2 = 8;
        ez->anim_frame = 10;                                     /* in [7..24] @0x80102DF8-FC */
        ez->contact_slot = (int8_t)s2;                           /* +0xD-MAPPING: body-push contact */
        pz->state = 1; pz->sub_state_1 = 1; pz->hp = 50;
        pz->re2z_self1d3 = 0; pz->re2z_flags21a = 0; pz->re2z_f10e = 0;
        re15_re2z_tick(s1);
        CHECK(pz->state == 1 && pz->sub_state_1 == 9,
              "partner gets 0x901 (@0x80102E90-98), got %d/%d", pz->state, pz->sub_state_1);
        CHECK(se_seen(4), "partner wake plays SE 4 (@0x80102E9C-A4)");
        /* gate: an already-fallen partner (+0x10E&0x2000) is skipped (@0x80102E80-84) */
        pz->state = 1; pz->sub_state_1 = 1; pz->sub_state_2 = 0; pz->re2z_f10e = 0x2000;
        ez->sub_state_2 = 8; ez->anim_frame = 10;
        re15_re2z_tick(s1);
        CHECK(pz->sub_state_1 == 1, "fallen partner (0x2000) must NOT be dominoed");
        re15_actor_free(s1); re15_actor_free(s2);
    }

    /* ---- grab kill: one-save then death (FUN_800401d4) ------------------------------------ */
    {
        re15_re2z_rng_reset();                                   /* also clears the one-save latch */
        pl->hp = 10; pl->hit_react = 0; pl->state = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->state = 1; e->sub_state_1 = 3; e->sub_state_2 = 2; e->ai_dist = 200;
        e->re2z_grabclip = 0x0B; e->re2z_bitefr = 16; e->re2z_bitedmg = 20;
        re15_re2z_tick(s);                                       /* P2 -> phase 3 */
        e->anim_frame = 16;
        re15_re2z_tick(s);                                       /* bite: 10-20 = -10 > -15 */
        CHECK(pl->hp == 0 && e->sub_state_2 == 4,
              "first lethal bite = ONE SAVE: HP clamped 0 + throw-off (FUN_800401d4)");
        /* second grab, the save latch is set -> the next lethal bite kills */
        e->sub_state_2 = 2; re15_re2z_tick(s);
        e->anim_frame = 16;
        re15_re2z_tick(s);
        CHECK(pl->hp < 0 && pl->state == 7,
              "latched one-save -> death (HP %d, PL state %d)", pl->hp, pl->state);
        CHECK(e->state == 1 && e->sub_state_1 == 6,
              "player death commits self 0x601 (@0x80102924-38), got state %d sub %d",
              e->state, e->sub_state_1);
        re15_actor_free(s);
        re15_re2z_rng_reset();
    }

    /* ---- SNAP BITE (EXEC[14]) + the 60-frame cooldown ------------------------------------- */
    {
        se_n = 0;
        pl->hp = 100; pl->state = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->state = 1; e->sub_state_1 = 14; e->sub_state_2 = 0; e->ai_dist = 1500;
        re15_re2z_tick(s);                                       /* P0 @0x80104DB4 */
        CHECK(e->motion == 0x11 && e->sub_state_2 == 1,
              "snap bite clip 0x11 rate 0xF (sw 0xF0011 @0x80104DB4-BC)");
        e->anim_frame = 10;
        re15_re2z_tick(s);                                       /* frame 10 + clip end (no bank) */
        CHECK(se_seen(5), "snap-bite SE 5 at frame 10 (@0x80104DFC-04)");
        CHECK(e->state == 1 && e->sub_state_1 == 0,
              "snap bite ends with word 1 -> ACTIVE sub 0 (sw 1 @0x80104E24)");
        CHECK(e->re2z_cd23e == 60, "+0x23E = 60 at the end (@0x80104E28-2C), got %u", e->re2z_cd23e);
        re15_re2z_tick(s);
        CHECK(e->re2z_cd23e == 59, "the root prolog counts the bite cooldown down (@0x80100470-80)");
        re15_actor_free(s);
    }

    /* ---- LUNGE BITE (EXEC[12]): clips 0x19 -> 0x1B, ends in WALK -------------------------- */
    {
        pl->x = 2000; pl->z = 0;                                 /* in front (mesh yaw 0 = +X) */
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->x = 0; e->z = 0; e->rot_y = 0;
        e->state = 1; e->sub_state_1 = 12; e->sub_state_2 = 0; e->ai_dist = 2000;
        re15_re2z_tick(s);                                       /* P0 @0x801047B8 */
        CHECK(e->motion == 0x19 && e->sub_state_2 == 1, "lunge P0 clip 0x19 (sw 0x70019)");
        re15_re2z_tick(s);                                       /* P1: player inside +-320 -> P2 */
        CHECK(e->sub_state_2 == 2, "P1 arc(PL,320)==0 -> phase 2 (@0x80104840-50)");
        re15_re2z_tick(s);                                       /* P2 sets 0x1B, falls into P3 */
        CHECK(e->motion == 0x1B, "P2 clip 0x1B (sw 0x7001B @0x80104884-8C)");
        CHECK(e->state == 1 && e->sub_state_1 == 1,
              "P3 clip end -> 0x101 WALK (@0x801048D8-DC), got sub %d", e->sub_state_1);
        re15_actor_free(s);
    }

    /* ---- HURT: resist / eligibility mark / knockdown-flinch (@0x80104F40, Review #2) ------- */
    {
        re15_re2z_rng_reset(); se_n = 0;
        pl->x = 200; pl->z = 0; pl->hp = 100; pl->state = 0; pl->motion = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        /* ⛔ NEU-KALIBRIERT 2026-08-18 (Nutzer-Report "reagieren nicht auf die Schuesse").
         * Der Treffer-Zustand, den beide RE2-Applier hinterlassen: `sw 2,4(s1)` @0x80047288
         * nullt +0x5/+0x6/+0x7, danach `sb <Trefferart>,5(s1)` @0x80047324 — also state=2,
         * +0x5 = ZEILE (Trefferart), +0x6 = 0. Der Port stempelt genau das in
         * re15_re2_stamp_1d2 (re15_damage.c). Zeile 2 -> Kosten 15 (@0x8010CC33[2]). */
#define RE2_HIT(row_) do { e->hp = (int16_t)(e->hp - 10); e->state = 2;                       \
                           e->sub_state_1 = (uint8_t)(row_); e->sub_state_2 = 0;              \
                           e->re2z_hits1d2 = 1; } while (0)
        e->state = 1; e->sub_state_1 = 4; e->hp = 60; e->re2z_res223 = 20;
        re15_re2z_tick(s);
        /* Treffer 1: Resistenz noch da -> KEIN Flinch, sondern der Haupt-Handler 0x80105438
         * (Zeile 2, Spalte 1 = @0x8010C98C). Der zieht die Kosten ab und macht Krach. */
        RE2_HIT(2); se_n = 0; e->re2z_cd239 = 0;
        re15_re2z_tick(s);
        CHECK(e->state == 2 && e->sub_state_2 == 1,
              "Treffer 1 -> Reaktions-Phase 1 (sb 1,6 @0x801055B4), got %d/%d",
              e->state, e->sub_state_2);
        CHECK(se_seen(11) || se_seen(12),
              "Treffer-Grunzer 11/12 (rand&1 @0x801054E8-500)");
        CHECK(e->re2z_cd239 == 150, "Grunzer-Cooldown 150 (@0x80105508-0C), got %d", e->re2z_cd239);
        CHECK(e->re2z_res223 == 5,
              "+0x223 -= cost[Zeile 2] = 15 (@0x801055D8-EC), got %d", e->re2z_res223);
        CHECK(e->re2z_gaitrow == 24, "+0x16B = 24 (@0x80105514-18), got %d", e->re2z_gaitrow);
        CHECK(e->re2z_flag222 == 0,
              "Treffer mit Restresistenz schreibt KEIN +0x222 (@0x80105078 springt an 0x80105164 vorbei)");
        /* die Phasen laufen ohne weitere Treffer weiter: P1 3 Frames -> P2 (t158 = 16) */
        for (int f = 0; f < 4; f++) re15_re2z_tick(s);
        CHECK(e->state == 2 && e->sub_state_2 == 2 && e->re2z_t158 <= 16,
              "P1 (3 Frames, @0x80105850-6C) -> P2 mit +0x158 = 16, got %d/%d/%d",
              e->state, e->sub_state_2, e->re2z_t158);
        /* Treffer 2: Resistenz 5 > 0 -> wieder Haupt-Handler, 5-15 = -10 */
        RE2_HIT(2);
        re15_re2z_tick(s);
        CHECK(e->re2z_res223 == -10, "+0x223 zweiter Abzug -> -10, got %d", e->re2z_res223);
        CHECK(e->hp < 81, "Nachlade-Gate: HP < 81 (slti 81 @0x80105604), hp=%d", e->hp);
        /* Treffer 3: Resistenz <= 0, aber +0x222 == 0 und Zeile != 1 -> NUR markieren
         * (@0x80105164); der Dispatch laeuft danach in 0x80105BC0, weil +0x222 jetzt 1 ist. */
        RE2_HIT(2);
        re15_re2z_tick(s);
        CHECK(e->re2z_flag222 == 1 && e->state == 2 && e->sub_state_1 != 5,
              "Eligibility verfehlt -> Markieren @0x80105164, KEIN 0x501 (got %d/%d)",
              e->state, e->sub_state_1);
        /* Treffer 4: jetzt markiert -> Flinch 0x501 */
        RE2_HIT(2);
        re15_re2z_tick(s);
        CHECK(e->state == 1 && e->sub_state_1 == 5,
              "+0x222==1 -> knockdown-flinch 0x501 (@0x801050A4-AC), got sub %d", e->sub_state_1);
        CHECK(e->re2z_res223 >= 16 && e->re2z_res223 <= 31,
              "flinch reseeds res223 = 16+(rand&15) (@0x801050C0-C8), got %d", e->re2z_res223);
        CHECK(!se_seen(9), "NO SE on the flinch path (both 0x501 arms end via j 0x80105418; "
              "SE 9 lives behind the un-ported +0x1D0&0xC0 gates — Review #14)");
#undef RE2_HIT
        /* the knockdown chain: P0 fall clip 1/2 AT FRAME 10/15 + SE 12/13 + downed marker */
        se_n = 0; e->re2z_cd239 = 0;
        re15_re2z_tick(s);                                       /* EXEC[5] P0 */
        CHECK(e->motion == 1 || e->motion == 2,
              "knockdown fall clip 1/2 (param @0x80100044, @0x801032C8-CC), got %d", e->motion);
        CHECK(e->anim_frame == (uint32_t)((e->motion - 1) * 5 + 10),
              "fall clip starts at frame side*5+10 (@0x80103310-1C), got %u", (unsigned)e->anim_frame);
        CHECK(se_seen(12) || se_seen(13), "P0 SE 12/13 (@0x8010332C-60)");
        CHECK(e->re2z_cd239 == 150, "P0 arms the moan cooldown 150 (@0x8010335C-60)");
        CHECK((e->grid_id & 0x80u) != 0, "downed marker set (PORT-MAPPING Review #18)");
        CHECK((e->re2z_f10e & 0x2000u) != 0, "+0x10E|=0x2000 (@0x80103308/3320)");
        re15_re2z_tick(s);                                       /* P1: clip done -> 6 */
        re15_re2z_tick(s);                                       /* P6 */
        CHECK(e->re2z_flags21a & 0x10u, "P6 sets the crawl marker +0x21A|=0x10 (@0x8010358C)");
        CHECK(e->motion == 8 || e->motion == 9,
              "P6 ground-lie clip 8/9 (@0x80103574-A8), got %d", e->motion);
        CHECK(e->sub_state_2 == 7, "P6 -> phase 7 (@0x8010357C-80)");
        re15_re2z_tick(s);                                       /* P7: lie clip done (no bank) */
        CHECK(!(e->re2z_flags21a & 0x10u),
              "P7 exit CLEARS the crawl marker (@0x801036B8-BC, Review #6)");
        CHECK(e->sub_state_2 == 8, "P7 -> phase 8 on clip end (@0x801036C4), got %d", e->sub_state_2);
        re15_re2z_tick(s);                                       /* P8 */
        CHECK(e->state == 1 && e->sub_state_1 == 1,
              "knockdown ends in 0x101 (@0x801036F4-F8), got %d/%d", e->state, e->sub_state_1);
        CHECK((e->grid_id & 0x80u) == 0, "downed marker cleared on rise (PORT-MAPPING)");
        re15_actor_free(s);
    }

    /* ---- DEATH -> CORPSE (claim-clear + kill latch + lie clip by orientation) ------------- */
    {
        se_n = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->state = 3; e->hp = -5; e->re2z_flags21a = 0;
        re15_re2z_tick(s);
        CHECK(e->re2z_flags21a & 0x4000u, "DEATH sets the kill latch +0x21A|=0x4000 (@0x80108294-98)");
        CHECK(e->motion == 7, "death clip 7 (@0x80108A88 family)");
        re15_re2z_tick(s);                                       /* clip done -> corpse */
        CHECK(e->state == 7, "DEATH commits 0x907 (@0x801084DC), got state %d", e->state);
        re15_re2z_tick(s);                                       /* corpse init */
        CHECK(e->hp == -1, "CORPSE HP = -1 (@0x8010A4D4)");
        CHECK(e->motion == 0x17, "corpse lie clip 23 with orientation bit clear (@0x8010A490-98)");
        re15_actor_free(s);
        /* the 22-variant: orientation bit 0x4 set */
        s = re15_actor_alloc(0x10); e = &g_actors[s];
        e->state = 7; e->re2z_flags21a = 0x4; e->hp = -5; e->sub_state_1 = 9;
        re15_re2z_tick(s);
        CHECK(e->motion == 0x16, "corpse lie clip 22 when +0x21A&0x4 (@0x8010A4A8-BC)");
        re15_actor_free(s);
    }

    /* ---- STATE 8 resolves through the get-up commit 0x901 (@0x8010AE9C) ------------------- */
    {
        se_n = 0;
        re15_re2z_rng_reset();                                   /* first draw 0x7D (odd) -> the
                                                                  * rand&1 SE gate passes */
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->state = 8; e->hp = 30; e->re2z_cd239 = 0;
        re15_re2z_tick(s);
        CHECK(e->state == 1 && e->sub_state_1 == 9,
              "state 8 -> 0x901 get-up (@0x8010AE9C), got %d/%d", e->state, e->sub_state_1);
        re15_re2z_tick(s);                                       /* EXEC[9] P0 */
        CHECK(e->motion == 3 || e->motion == 4,
              "get-up clip 3/4 from the param list @0x801000D8, got %d", e->motion);
        CHECK((e->re2z_flags21a & 0x8u) != 0 && !(e->re2z_flags21a & 0x10u),
              "get-up sets +0x21A|=0x8 and clears 0x10 (@0x80103F7C-90)");
        CHECK(se_seen(12), "get-up SE 12 (rand&1 + cd gate @0x80103F84-B4)");
        CHECK(e->re2z_cd239 == 150, "get-up SE arms the cooldown (@0x80103FB0-B4)");
        re15_actor_free(s);
    }

    /* ---- the ladder LIVE: D/E commit 0x0C01 and EXEC[12] runs the same tick --------------- */
    {
        re15_re2z_rng_reset();
        pl->x = -1500; pl->z = 0; pl->motion = 100;              /* RUNNING, behind the zombie */
        pl->hp = 100; pl->state = 0; pl->hit_react = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->x = 0; e->z = 0; e->rot_y = 0;
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 1;
        e->re2z_gaitinit = (uint8_t)(0x80u | 1u); e->re2z_gaittmr = 30000;
        int committed_tick = -1;
        for (int t = 0; t < 300 && committed_tick < 0; t++) {
            e->ai_dist = 1400;                                   /* inside E(2500)+D(3500), outside G(1200) */
            e->x = 0; e->z = 0; e->rot_y = 0;                    /* facing +X, the player BEHIND (-X)
                                                                  * -> OUTSIDE both cones (D/E need
                                                                  * arc1024 != 0, beq @0x80101868) */
            re15_re2z_tick(s);
            if (e->sub_state_1 == 12) committed_tick = t;
        }
        CHECK(committed_tick >= 0, "D/E must commit 0x0C01 against a running player (@0x801018a0/e4)");
        CHECK(e->motion == 0x19, "the commit runs EXEC[12] the SAME tick (@0x801011A8-EC)");
        pl->motion = 0;
        re15_actor_free(s);
    }

    /* ---- INIT remap: RE1.5 spawn nibbles land in the RE2 spawn subs ----------------------- */
    {
        re15_re2z_rng_reset();
        pl->x = 20000; pl->z = 0;                                /* far away: no wake */
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->x = 0; e->z = 0; e->grid_id = 6; e->state = 0;        /* the ROOM1140 feeder nibble */
        re15_re2z_tick(s);
        CHECK(e->state == 1 && e->sub_state_1 == 8,
              "feeder spawn -> ACTIVE sub 8 (INIT 0x801 @0x80100AD4), got %d/%d",
              e->state, e->sub_state_1);
        CHECK(e->re2z_f10e == 0x4004,
              "feeder spawn writes +0x10E = 0x4004 (@0x80100A88-8C), got 0x%04x", e->re2z_f10e);
        CHECK(e->re2z_walkclip == 0 || e->re2z_walkclip == 2,
              "walk clip from the param style row (values 0/2, @0x80100860-8C), got %u",
              e->re2z_walkclip);
        CHECK(e->re2z_res223 >= 16 && e->re2z_res223 <= 31,
              "INIT seeds res223 = 16+(rand&15) (@0x80100888-9C)");
        e->ai_dist = 60000;                                      /* far: the limpet latch holds */
        re15_re2z_tick(s);
        CHECK(e->motion >= 0x12 && e->motion <= 0x14,
              "feeding ROTATES clips 18/19/20 (rand&7 @0x801000A8, @0x80103BF4-C10), got %d",
              e->motion);
        for (int t = 0; t < 6; t++) { e->ai_dist = 60000; re15_re2z_tick(s); }
        CHECK(e->state == 1 && e->sub_state_1 == 8,
              "far feeder stays feeding (limpet +0x10E&0x4000 @0x80103C94-A0)");
        /* wake (MAPPED gate clears the limpet) -> byte-true chain P3 clip 0x15 -> P5 0x101 */
        int woke = 0;
        for (int t = 0; t < 12 && !woke; t++) {
            e->ai_dist = 100;
            re15_re2z_tick(s);
            if (e->sub_state_1 == 1 || e->sub_state_1 == 0) woke = 1;
        }
        CHECK(woke, "near feeder wakes through P3(clip 0x15 @0x80103CD8)->P5(0x101 @0x80103D90)");
        CHECK(e->grid_id == 0, "wake clears the spawn nibble (PORT-MAPPING Review #16)");
        re15_actor_free(s);
    }

    /* ---- LYING chain: P0 hold -> limpet -> idle 8/9 -> 0x101 + grid clear ------------------ */
    {
        re15_re2z_rng_reset();
        int s = re15_actor_alloc(0x16);
        re15_actor_t *e = &g_actors[s];
        e->state = 1; e->sub_state_1 = 7; e->sub_state_2 = 0;
        e->grid_id = 0x88; e->re2z_f10e = 0x4002; e->ai_dist = 60000;
        re15_re2z_tick(s);
        CHECK(e->motion == 0x17 && e->sub_state_2 == 1,
              "lying P0: clip 23 PLAIN (rate 0, @0x801037CC-E4), phase 1");
        CHECK((e->re2z_self1d3 & 0x80u) != 0, "lying P0 sets self+0x1D3|=0x80 (@0x80103804-14)");
        re15_re2z_tick(s);
        CHECK(e->sub_state_2 == 1, "far lyer HOLDS in P1 (limpet +0x10E&0x4000 @0x8010381C-28)");
        e->ai_dist = 100;                                        /* MAPPED wake clears the latch */
        re15_re2z_tick(s);                                       /* P1 -> 2 */
        re15_re2z_tick(s);                                       /* P2: idle clip */
        CHECK(e->motion == 8 || e->motion == 9,
              "lying P2 idle clip 8/9 rate 0xF (@0x80103838-8C), got %d", e->motion);
        re15_re2z_tick(s);                                       /* P3: clip done -> 4 */
        re15_re2z_tick(s);                                       /* P4: 0x101 */
        CHECK(e->state == 1 && e->sub_state_1 == 1,
              "lying P4 exits 0x101 (@0x80103900-0C, NOT 0x901 — Review #15), got %d/%d",
              e->state, e->sub_state_1);
        CHECK(e->grid_id == 0,
              "the 0x88 spawn nibble is cleared on wake (PORT-MAPPING Review #16/#3)");
        re15_actor_free(s);
    }

    /* ---- P5 escape side latch: the 1/16 two-draw roll sets +0x21A 0x20/0x40 --------------- */
    {
        re15_re2z_rng_reset();
        pl->x = 200; pl->z = 0;
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->state = 1; e->sub_state_1 = 3; e->re2z_grabclip = 0x0B;
        int latched = 0;
        for (int t = 0; t < 400 && !latched; t++) {
            e->sub_state_2 = 5; e->anim_frame = 3; e->re2z_flags21a = 0;
            re15_re2z_tick(s);                                   /* P5 @0x801029A4 */
            if (e->re2z_flags21a & 0x60u) latched = 1;
        }
        CHECK(latched, "the 1/16 escape roll must eventually latch +0x21A 0x20/0x40 (@0x80102A34-40)");
        re15_actor_free(s);
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);                     /* leave the byte-true default */

    if (fails == 0) printf("test_re2_zombie_ai: OK\n");
    else            printf("test_re2_zombie_ai: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
