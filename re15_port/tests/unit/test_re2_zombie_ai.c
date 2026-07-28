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
#include <stdio.h>
#include <stdint.h>

extern const uint16_t re15_re2z_gait_tbl[32];
extern int re15_re2z_walk_turn(re15_actor_t *e, int32_t px, int32_t pz, uint32_t dist);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

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

    if (fails == 0) printf("test_re2_zombie_ai: OK\n");
    else            printf("test_re2_zombie_ai: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
