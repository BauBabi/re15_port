/* ============================================================================
 *  Zombie Girl (type 0x13, EM013 = ZOMBIE_GIRL) AI — Wave 1 port probe.
 *
 *  Byte-true 0x8010a8c8 family (RE15_ZOMBIEGIRL_AI.md). A nav-pathing zombie
 *  variant that reuses the standard zombie combat (grab -10, shared corpse).
 *  Seeds a type-0x13 actor + a player and drives re15_enemy_ai_run_all:
 *   (1) INIT (0x8010ab2c): state 0 -> 1 (ACTIVE), HP 50..81, nav-walk clip 26.
 *   (2) CHASE + GRAB: a nearby player -> nav-chase -> grab (state 3), player
 *       PINNED (re15_player_is_grabbed) and takes -10.
 *   (3) KILLABLE: a lethal hit -> CORPSE (state 7, shared with the zombie).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 1500; pl->hp = 100;

    const int GS = 1;
    re15_actor_t *g = &g_actors[GS];
    g->active = 1; g->type = 0x13; g->state = 0; g->x = 0; g->y = 0; g->z = 0; g->rot_y = 0;
    re15_enemy_apply_hitbox(g, 0x13);

    printf("=== ZOMBIE GIRL (type 0x13, EM013) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (g->state != 1)        { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", g->state); fail = 1; }
    if (g->hp < 50 || g->hp > 81) { fprintf(stderr, "FAIL(1): HP must seed 50..81, got %d\n", g->hp); fail = 1; }
    if (g->motion != 0x1a)    { fprintf(stderr, "FAIL(1): nav-walk clip 26 expected, got %d\n", g->motion); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d, clip=%d\n", g->hp, g->motion);

    /* (2) CHASE + GRAB: face the player, close in, grab (state 3) + -10 */
    extern int re15_player_is_grabbed(void);
    extern int16_t re15_atan2_q12(int32_t, int32_t);
    g->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - g->z, pl->x - g->x) - 0x400) & 0xfff);  /* face player */
    int16_t php0 = pl->hp; int grabbed = 0, hurt10 = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (re15_player_is_grabbed()) grabbed = 1;
        if (pl->hp <= php0 - 10) hurt10 = 1;
        if (grabbed && hurt10) break;
        pl->hit_react = 0;
    }
    if (!grabbed) { fprintf(stderr, "FAIL(2): zombie girl must grab + pin the player (state=%d)\n", g->state); fail = 1; }
    if (!hurt10)  { fprintf(stderr, "FAIL(2): the grab must deal -10, player hp %d->%d\n", php0, pl->hp); fail = 1; }
    printf("  (2) GRAB: state=%d, pinned=%d, player hp %d->%d\n", g->state, grabbed, php0, pl->hp);

    /* (2b) HELD BITE-LOOP: byte-true FUN_80102548 [3] deals a repeated -5 per grab-clip cycle (NOT a
     * single -10 then release), and the kill window (+0x9e=100) < escape window (+0x9c=110, unmashed)
     * devours the player -> death (state 7). */
    int16_t hp_after_impact = pl->hp;   /* ~90 after the -10 impact */
    int extra_bite = 0, devoured = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if (pl->hp <= hp_after_impact - 5) extra_bite = 1;   /* a held -5 landed beyond the impact */
        if (pl->state == 7) { devoured = 1; break; }         /* kill window ran out -> devoured */
        pl->hit_react = 0;
    }
    if (!extra_bite) { fprintf(stderr, "FAIL(2b): the held grab must deal repeated -5 bites (hp stayed %d)\n", pl->hp); fail = 1; }
    if (!devoured)   { fprintf(stderr, "FAIL(2b): an unmashed held grab must devour the player (state 7)\n"); fail = 1; }
    printf("  (2b) HELD BITE: repeated -5, unmashed grab devoured the player (state %d, hp=%d)\n", pl->state, pl->hp);

    /* (3) KILLABLE: a lethal hit -> CORPSE (state 7) */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 8000; pl->hp = 100;
    g = &g_actors[GS]; g->active = 1; g->type = 0x13; g->state = 1; g->x = 0; g->z = 0; g->hp = 4;
    re15_enemy_apply_hitbox(g, 0x13);
    re15_enemy_take_damage(g, 2);              /* hp 4-24 < 0 -> state 3, hp<0 -> corpse */
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (g->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(3): a killed zombie girl must reach CORPSE (state 7), state=%d hp=%d\n", g->state, g->hp); fail = 1; }
    printf("  (3) DEATH: lethal hit -> CORPSE (state %d, hp=%d)\n", g->state, g->hp);

    if (fail) { printf("ZOMBIE GIRL WAVE-1: FAIL\n"); return 1; }
    printf("ZOMBIE GIRL WAVE-1: all checks passed\n");
    return 0;
}
