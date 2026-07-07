/* test_enemy_shootable.c — the player auto-aim + PER-TYPE damage across the roster.
 *
 * Byte-true: FUN_80011f50 iterates all entities and tests each one's DAMAGE HITBOX (+0x78) — no per-type
 * whitelist. And the damage is per-type (@0x8006e0d0: dmg = u16[type*0x58 + weapon*4]) — a crow takes
 * 2/shot, a tyrant 16, an alligator/birkin 30, while the spider-baby/ivy/birkin-5 rows are all zero =
 * WEAPON-IMMUNE (targetable, but no gun damage). Asserts:
 *   (1) ALLIGATOR (0x23): shootable, pistol -30.
 *   (2) TYRANT (0x2b): shootable, pistol -16.
 *   (3) IVY (0x2d): TARGETABLE but WEAPON-IMMUNE (hit registers, 0 damage).
 *   (4) WRITHER (0x1a, no hitbox): NOT targetable at all.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

/* returns the fire result (slot+1 or 0) and reports the damage dealt via *dmg_out */
static int fire_at(uint8_t type, int32_t dist, int16_t *dmg_out)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0; pl->floor = 0; pl->hp = 100;
    re15_player_death_reset();
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = type; e->state = 1; e->x = 0; e->z = dist; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    int16_t hp0 = e->hp = 300;
    int ret = re15_player_weapon_fire(2);   /* pistol (weapon 2) */
    *dmg_out = (int16_t)(hp0 - e->hp);
    return ret;
}

int main(void)
{
    int fail = 0;
    int16_t dmg;
    printf("=== player auto-aim (hitbox-gated) + PER-TYPE weapon damage ===\n");

    /* (1) ALLIGATOR: shootable, pistol damage = 30 (@0x8006e0d0 + 0x23*0x58) */
    if (fire_at(0x23, 2000, &dmg) != 2 || dmg != 30) {
        fprintf(stderr, "FAIL(1): ALLIGATOR (0x23) pistol must deal 30, dealt %d\n", dmg); fail = 1; }
    else printf("  (1) ALLIGATOR (0x23): shootable, pistol -%d\n", dmg);

    /* (2) TYRANT: shootable, pistol damage = 16 */
    if (fire_at(0x2b, 1500, &dmg) != 2 || dmg != 16) {
        fprintf(stderr, "FAIL(2): TYRANT (0x2b) pistol must deal 16, dealt %d\n", dmg); fail = 1; }
    else printf("  (2) TYRANT (0x2b): shootable, pistol -%d\n", dmg);

    /* (3) IVY: TARGETABLE (auto-aim selects it) but WEAPON-IMMUNE (row all-zero -> 0 damage) */
    {
        int ret = fire_at(0x2d, 1200, &dmg);
        if (ret != 2 || dmg != 0) {
            fprintf(stderr, "FAIL(3): IVY (0x2d) must be targetable but weapon-immune, ret=%d dmg=%d\n", ret, dmg); fail = 1; }
        else printf("  (3) IVY (0x2d): targetable but WEAPON-IMMUNE (hit registers, 0 damage)\n");
    }

    /* (4) WRITHER (0x1a): no hitbox -> NOT targetable */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 0; pl->rot_y = 0; pl->floor = 0; pl->hp = 100;
        re15_player_death_reset();
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x1a; e->state = 1; e->x = 0; e->z = 800; e->rot_y = 0;
        re15_enemy_apply_hitbox(e, 0x1a);
        if (re15_player_weapon_fire(2) != 0 || e->hit_radius_min != 0) {
            fprintf(stderr, "FAIL(4): WRITHER (0x1a, no hitbox) must NOT be targetable\n"); fail = 1; }
        else printf("  (4) WRITHER (0x1a): no hitbox -> correctly NOT targetable\n");
    }

    if (fail) { printf("ENEMY-SHOOTABLE: FAIL\n"); return 1; }
    printf("ENEMY-SHOOTABLE: all checks passed\n");
    return 0;
}
