/* test_enemy_shootable.c — the player auto-aim must target EVERY combat enemy, not just zombies.
 *
 * Byte-true: FUN_80011f50 iterates all entities and tests each one's DAMAGE HITBOX (+0x78) — there is
 * NO per-type whitelist. The port previously gated auto-aim on `type in {0x10,0x11,0x16,0x20}`, so the
 * whole non-zombie roster (alligator/tyrant/ivy/crow/spider/maggot/birkin/...) was UNSHOOTABLE despite
 * having HP + hitboxes. The fix gates on hit_radius_min>0 (the hitbox). Asserts:
 *   (1) an ALLIGATOR (0x23, radius 2200) in front takes pistol damage.
 *   (2) a TYRANT (0x2b, radius 800) in front takes pistol damage.
 *   (3) an IVY (0x2d) in front takes pistol damage.
 *   (4) a WRITHER (0x1a, no hitbox = unkillable) is NOT targetable (fire returns 0, no damage).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

static int fire_takes_damage(uint8_t type, int32_t dist)
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
    return (ret == 2 /* slot 1 + 1 */) && (e->hp < hp0);
}

int main(void)
{
    int fail = 0;
    printf("=== player auto-aim targets EVERY combat enemy (hitbox-gated, not type-whitelist) ===\n");

    if (!fire_takes_damage(0x23, 2000)) { fprintf(stderr, "FAIL(1): ALLIGATOR (0x23) must be shootable\n"); fail = 1; }
    else printf("  (1) ALLIGATOR (0x23): shootable, takes pistol damage\n");

    if (!fire_takes_damage(0x2b, 1500)) { fprintf(stderr, "FAIL(2): TYRANT (0x2b) must be shootable\n"); fail = 1; }
    else printf("  (2) TYRANT (0x2b): shootable, takes pistol damage\n");

    if (!fire_takes_damage(0x2d, 1200)) { fprintf(stderr, "FAIL(3): IVY (0x2d) must be shootable\n"); fail = 1; }
    else printf("  (3) IVY (0x2d): shootable, takes pistol damage\n");

    /* (4) the writher (0x1a) is unkillable — apply_hitbox gives it NO box -> not targetable */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 0; pl->rot_y = 0; pl->floor = 0; pl->hp = 100;
        re15_player_death_reset();
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x1a; e->state = 1; e->x = 0; e->z = 800; e->rot_y = 0;
        re15_enemy_apply_hitbox(e, 0x1a);   /* default: no box -> hit_radius_min stays 0 */
        int ret = re15_player_weapon_fire(2);
        if (ret != 0 || e->hit_radius_min != 0) {
            fprintf(stderr, "FAIL(4): WRITHER (0x1a, no hitbox) must NOT be targetable, ret=%d radius=%u\n",
                    ret, e->hit_radius_min); fail = 1; }
        else printf("  (4) WRITHER (0x1a): no hitbox -> correctly NOT targetable\n");
    }

    if (fail) { printf("ENEMY-SHOOTABLE: FAIL\n"); return 1; }
    printf("ENEMY-SHOOTABLE: all checks passed\n");
    return 0;
}
