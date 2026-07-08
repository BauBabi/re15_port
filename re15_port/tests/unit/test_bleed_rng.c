/* test_bleed_rng.c — the byte-true bleed/poison roll consumes TWO RNG draws (FUN_80012d60 @0x80012e78).
 *
 * On a type<2 hit the original draws the engine RNG TWICE, back-to-back and UNCONDITIONALLY (two
 * `jal FUN_8001af20` @0x80012e78/e80), then ANDs the two &1 results (@0x80012e88-90). The port had
 * `if ((dmg_rng()&1) && (dmg_rng()&1))` — C `&&` short-circuits, so a first draw with bit0==0 skipped
 * the second draw, consuming ONE value where the PSX consumes TWO. re15_engine_rand8 is the SHARED
 * engine stream (bleed AND all AI decisions), so a dropped draw desyncs everything downstream.
 * Asserts: re15_player_take_damage(type<2) advances the RNG by EXACTLY 2 for every seed — including a
 * seed whose first draw is even (the short-circuit case the bug hit).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_damage.h"

static void set_player(int16_t hp)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->x = 0; p->z = 0; p->rot_y = 0; p->floor = 0; p->hp = hp; p->hit_react = 0;
    re15_player_death_reset();
}

int main(void)
{
    int fail = 0, even_first_seen = 0;
    printf("=== bleed/poison roll consumes TWO RNG draws (byte-true FUN_80012d60 @0x80012e78) ===\n");

    /* A spread of seeds; for each, the RNG state after take_damage(type=0) must equal the state after
     * exactly TWO manual draws. */
    const uint32_t seeds[8] = { 0x2545f491u, 1u, 0xdeadbeefu, 0x12345678u, 2u, 0xcafef00du, 0x0badc0deu, 99u };
    for (int i = 0; i < 8; i++) {
        /* reference: two manual draws, then capture the 3rd value */
        re15_damage_seed_rng(seeds[i]);
        uint8_t d0 = re15_engine_rand8();
        (void)re15_engine_rand8();
        uint8_t after_two = re15_engine_rand8();
        if ((d0 & 1) == 0) even_first_seen = 1;   /* this seed exercises the short-circuit path */

        /* actual: take_damage(type<2) must consume exactly 2, so the next draw == after_two */
        re15_damage_seed_rng(seeds[i]);
        set_player(1000);
        re15_player_take_damage(&g_actors[RE15_ACTOR_SLOT_PLAYER], /*attack_type=*/0, 0, 0);
        uint8_t after_td = re15_engine_rand8();
        if (after_td != after_two) {
            fprintf(stderr, "FAIL: seed %#x first-draw=%#x -> take_damage consumed != 2 (next %#x, want %#x)\n",
                    seeds[i], d0, after_td, after_two);
            fail = 1;
        }
    }
    if (!even_first_seen) { fprintf(stderr, "FAIL: no tested seed had an even first draw (test wouldn't catch the short-circuit)\n"); fail = 1; }
    else if (!fail) printf("  take_damage(type<2) consumes exactly 2 RNG draws for all 8 seeds (incl. even-first)\n");

    /* type>=2 must consume ZERO (no bleed roll). */
    {
        re15_damage_seed_rng(0x2545f491u);
        uint8_t ref = re15_engine_rand8();
        re15_damage_seed_rng(0x2545f491u);
        set_player(1000);
        re15_player_take_damage(&g_actors[RE15_ACTOR_SLOT_PLAYER], /*attack_type=*/5, 0, 0);
        uint8_t after = re15_engine_rand8();
        if (after != ref) { fprintf(stderr, "FAIL: type>=2 must consume 0 RNG draws (next %#x, want %#x)\n", after, ref); fail = 1; }
        else printf("  take_damage(type>=2) consumes 0 RNG draws (no bleed roll)\n");
    }

    if (fail) { printf("BLEED-RNG: FAIL\n"); return 1; }
    printf("BLEED-RNG: all checks passed\n");
    return 0;
}
