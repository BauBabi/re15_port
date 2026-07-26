/* test_ivy_ai.c — IVY plant-grappler (type 0x2d, EM02D, STAGE4 lab) AI.
 *
 * Byte-true rebuild from audit wf_efd92a2c (root 0x801168c4, state table @0x8011a2c0 = exactly two code
 * pointers INIT 0x80116920 / ACTIVE 0x801169b8; word[2]=0xfa060000 is DATA). The shipped ivy is a
 * DORMANT SCRIPTED PROP, not a combat enemy: the prior port INVENTED a nav-chase + grab-instakill +
 * hurt/death/corpse machine by mis-reading the neighbouring type-0x40 NPC root as the ivy brain. This
 * test now pins the byte-true behavior:
 *   (1) INIT: state -> 1, HP 100 unconditional (@0x80116954), clip 0, +0x93=3 immunity guard (@0x8011693c-44).
 *   (2) NO chase: a distant player does NOT make the ivy move (root makes zero walker/pathfind calls,
 *       ivy #73/#78) — it stays exactly where it spawned.
 *   (3) NO box / weapon-immune: apply_hitbox(0x2d) installs no +0x78 box (the {450,1530} box belongs to
 *       the 0x40 NPC, ivy #77); a hit runs the +0x93=3 hit-once guard -> take_damage returns 0, hp
 *       unchanged, state never promoted past 1 (no HURT/DEATH/CORPSE, ivy #75/#76).
 *   (4) Dormant-until-flag: with game-flag z5:31 set, ACTIVE sub 0 consumes the flag and advances
 *       (@0x80116a14-34) into the sub-1/sub-2 sway — still no player read, no movement, no attack.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_scd.h"        /* re15_game_flag_set/get */

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);
    extern int re15_player_is_grabbed(void);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 5000; pl->hp = 100;

    const int IS = 1;
    re15_actor_t *e = &g_actors[IS];
    e->active = 1; e->type = 0x2d; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x2d);

    printf("=== IVY plant-grappler (type 0x2d, EM02D) AI — dormant prop ===\n");

    /* (1) INIT: state->1, HP 100 unconditional, +0x93=3 guard. */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)      { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp != 100)       { fprintf(stderr, "FAIL(1): HP must be 100, got %d\n", e->hp); fail = 1; }
    if (e->hit_react != 3)  { fprintf(stderr, "FAIL(1): INIT must set +0x93=3 (immunity guard), got %d\n", e->hit_react); fail = 1; }
    printf("  (1) INIT: state->1 sub=%d, hp=%d, hit_react=%d\n", e->sub_state_1, e->hp, e->hit_react);

    /* (2) NO chase: a distant player must NOT move the ivy. */
    int32_t ix = e->x, iz = e->z;
    for (int f = 0; f < 100; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    if (e->x != ix || e->z != iz) { fprintf(stderr, "FAIL(2): ivy must stay put (dormant), moved (%d,%d)->(%d,%d)\n", ix, iz, e->x, e->z); fail = 1; }
    if (re15_player_is_grabbed()) { fprintf(stderr, "FAIL(2): ivy must never grab (dead grab VM)\n"); fail = 1; }
    if (pl->hp != 100)            { fprintf(stderr, "FAIL(2): ivy must never damage the player, hp=%d\n", pl->hp); fail = 1; }
    printf("  (2) NO chase: pos held at (%d,%d), player hp=%d, grabbed=%d\n", e->x, e->z, pl->hp, re15_player_is_grabbed());

    /* (3) NO box + weapon-immune: apply_hitbox installs nothing; a hit is a no-op via the +0x93 guard. */
    if (e->hit_radius_min != 0) { fprintf(stderr, "FAIL(3): ivy must have NO damage box, got radius %d\n", e->hit_radius_min); fail = 1; }
    e->hp = 100; e->hit_react = 3;                 /* dormant state after INIT */
    int applied = re15_enemy_take_damage(e, 2);    /* attack_type 2 */
    if (applied != 0)     { fprintf(stderr, "FAIL(3): weapon hit must be rejected by the +0x93 guard, got %d\n", applied); fail = 1; }
    if (e->hp != 100)     { fprintf(stderr, "FAIL(3): immune ivy hp must be unchanged, got %d\n", e->hp); fail = 1; }
    if (e->state != 1)    { fprintf(stderr, "FAIL(3): ivy must never enter HURT/DEATH, state=%d\n", e->state); fail = 1; }
    for (int f = 0; f < 60; f++) { re15_enemy_ai_run_all(0); if (e->state == 7) { fprintf(stderr, "FAIL(3): ivy reached CORPSE (impossible)\n"); fail = 1; break; } }
    printf("  (3) IMMUNE: box radius=%d, take_damage applied=%d, hp=%d, state=%d\n", e->hit_radius_min, applied, e->hp, e->state);

    /* (4) Dormant-until-flag: setting z5:31 advances ACTIVE sub 0 (consumes the flag), still inert. */
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    int32_t bx = e->x, bz = e->z;
    re15_game_flag_set(5, 31, 1);
    re15_enemy_ai_run_all(0);
    if (e->sub_state_1 == 0) { fprintf(stderr, "FAIL(4): flag z5:31 must advance ACTIVE sub 0, still 0\n"); fail = 1; }
    if (re15_game_flag_get(5, 31)) { fprintf(stderr, "FAIL(4): ACTIVE sub 0 must CONSUME the z5:31 flag\n"); fail = 1; }
    for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
    if (e->x != bx || e->z != bz) { fprintf(stderr, "FAIL(4): ivy must stay put even after flag, moved\n"); fail = 1; }
    printf("  (4) FLAG: z5:31 -> sub advanced to %d, flag consumed=%d, pos held\n", e->sub_state_1, !re15_game_flag_get(5, 31));

    if (fail) { printf("IVY GRAPPLER: FAIL\n"); return 1; }
    printf("IVY GRAPPLER: all checks passed\n");
    return 0;
}
