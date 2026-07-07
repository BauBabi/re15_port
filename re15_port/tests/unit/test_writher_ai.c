/* test_writher_ai.c — ROOTED WRITHE-HAZARD (type 0x1a, EM01A, STAGE1) AI.
 *
 * Byte-true from workflow wf_5c34ffe7 (root 0x8010c1ec). This enemy is a rooted, ground-emergent,
 * UNKILLABLE contact-hazard: it never walks/chases, sets no HP, has no death state, and deals no
 * player damage. Asserts:
 *   (1) INIT: state -> 1 (ACTIVE writhe).
 *   (2) ROOTED: with the player adjacent it NEVER advances X/Z (no chase) over many frames.
 *   (3) HARMLESS: the player takes no damage from it.
 *   (4) UNKILLABLE: a "lethal" hit never puts it into a death/corpse state (stays ACTIVE).
 *   (5) EMERGE: a buried spawn (grid bit 0x80) starts on the emerge clip 0x1f.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 900; pl->hp = 100;

    const int WS = 1;
    re15_actor_t *e = &g_actors[WS];
    e->active = 1; e->type = 0x1a; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x1a);   /* unkillable -> default: no hitbox */

    printf("=== ROOTED WRITHE-HAZARD (type 0x1a, EM01A) AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    printf("  (1) INIT: state->%d (writhe), motion=%d\n", e->state, e->motion);

    /* (2) ROOTED + (3) HARMLESS: player adjacent, run 200 frames, enemy must not move and not hurt */
    int32_t ex0 = e->x, ez0 = e->z; int16_t hp0 = pl->hp;
    for (int f = 0; f < 200; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    if (e->x != ex0 || e->z != ez0) { fprintf(stderr, "FAIL(2): rooted hazard must NOT move, (%d,%d)->(%d,%d)\n", ex0, ez0, e->x, e->z); fail = 1; }
    if (pl->hp != hp0)              { fprintf(stderr, "FAIL(3): hazard must deal NO damage, hp %d->%d\n", hp0, pl->hp); fail = 1; }
    printf("  (2) ROOTED: stayed at (%d,%d) over 200 frames\n", e->x, e->z);
    printf("  (3) HARMLESS: player hp %d (unchanged)\n", pl->hp);

    /* (4) UNKILLABLE: a lethal hit must not produce a death/corpse state */
    e->hp = 1; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);
    for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
    if (e->state == 3 || e->state == 7) { fprintf(stderr, "FAIL(4): 0x1a is UNKILLABLE but reached death/corpse state %d\n", e->state); fail = 1; }
    printf("  (4) UNKILLABLE: after a lethal hit, state=%d (still active, no death)\n", e->state);

    /* (5) EMERGE: a buried spawn (grid & 0x80) starts on the emerge clip 0x1f */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->x = 0; pl->z = 900; pl->hp = 100;
    re15_actor_t *eb = &g_actors[WS];
    eb->active = 1; eb->type = 0x1a; eb->state = 0; eb->grid_id = 0x80; eb->x = 0; eb->z = 0;
    re15_enemy_ai_run_all(0);
    if (eb->motion != 0x1f) { fprintf(stderr, "FAIL(5): buried spawn must start on emerge clip 0x1f, got %d\n", eb->motion); fail = 1; }
    printf("  (5) EMERGE: buried spawn starts clip=0x%x (ground-emergence)\n", eb->motion);

    if (fail) { printf("WRITHE-HAZARD: FAIL\n"); return 1; }
    printf("WRITHE-HAZARD: all checks passed\n");
    return 0;
}
