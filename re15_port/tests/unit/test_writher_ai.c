/* test_writher_ai.c — WRITHE-HAZARD (type 0x1a, EM01A, STAGE1 ROOM1210/1211) AI.
 *
 * BYTE-TRUE from audit wf_efd92a2c writher (root 0x8010c1ec, state table @0x8012093c, raw STAGE1.BIN).
 * This enemy is a ROOTED ground-writhing contact-hazard that is a SOLID 300-radius obstacle and is
 * KILLABLE in one hit (INIT installs the +0x78 box @0x8012091c={0,-1440,0,300,1440,300} but writes NO
 * HP -> spawn HP=0; type 0x1a<0x20 -> any damaging shot -> hp<0 -> DEATH -> CORPSE). It never
 * walks/chases and deals no direct player damage. Asserts:
 *   (1) INIT: state -> 1 (ACTIVE writhe), HP stays 0 (INIT never sets +0x9a).
 *   (2) ROOTED: with the player adjacent it NEVER advances X/Z (no chase) over many frames.
 *   (3) HARMLESS: the writher itself writes no player.hp (contact push is game_step, not this tick).
 *   (4) IDLE = clip 0 ONLY: an unhit writher (grid 0) loops clip 0 forever — no 0->1->2 cycling
 *       (the old invented behavior, audit #3).
 *   (5) SOLID + SHOOTABLE: re15_enemy_apply_hitbox(0x1a) installs the 300-radius box (audit #1).
 *   (6) KILLABLE: a damaging hit on the HP-0 spawn -> DEATH (state 3) -> CORPSE (state 7) (audit #5).
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
    re15_enemy_apply_hitbox(e, 0x1a);   /* installs the 300-radius +0x78 box @0x8012091c */

    printf("=== WRITHE-HAZARD (type 0x1a, EM01A) AI ===\n");

    /* (5) SOLID + SHOOTABLE: the byte-true box is a real 300-radius damage hitbox */
    if (e->hit_radius_min != 300) { fprintf(stderr, "FAIL(5): 0x1a must have the 300-radius +0x78 box, got %d\n", e->hit_radius_min); fail = 1; }
    printf("  (5) SOLID+SHOOTABLE: hit_radius_min=%d (box @0x8012091c)\n", e->hit_radius_min);

    /* (1) INIT: state -> 1, HP stays 0 (INIT writes no +0x9a) */
    re15_enemy_ai_run_all(0);
    if (e->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp != 0)    { fprintf(stderr, "FAIL(1): INIT must not set HP (byte-true spawn 0), got %d\n", e->hp); fail = 1; }
    printf("  (1) INIT: state->%d (writhe), hp=%d (unset), motion=%d\n", e->state, e->hp, e->motion);

    /* (2) ROOTED + (3) HARMLESS: player adjacent, run 200 frames, enemy must not move / not hurt */
    int32_t ex0 = e->x, ez0 = e->z; int16_t hp0 = pl->hp;
    for (int f = 0; f < 200; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    if (e->x != ex0 || e->z != ez0) { fprintf(stderr, "FAIL(2): rooted hazard must NOT move, (%d,%d)->(%d,%d)\n", ex0, ez0, e->x, e->z); fail = 1; }
    if (pl->hp != hp0)              { fprintf(stderr, "FAIL(3): tick must write no player hp, hp %d->%d\n", hp0, pl->hp); fail = 1; }
    printf("  (2) ROOTED: stayed at (%d,%d) over 200 frames\n", e->x, e->z);
    printf("  (3) HARMLESS: player hp %d (unchanged)\n", pl->hp);

    /* (4) IDLE = clip 0 only: an unhit writher never changes clip (byte-true A[0]/B[0], grid 0) */
    if (e->motion != 0) { fprintf(stderr, "FAIL(4): unhit writher must loop clip 0, got clip %d\n", e->motion); fail = 1; }
    printf("  (4) IDLE: clip stayed 0 (no invented 0->1->2 cycling)\n");

    /* (6) KILLABLE: the HP-0 spawn dies in one damaging hit -> DEATH(3) -> CORPSE(7) */
    e->hit_react = 0;
    int landed = re15_enemy_take_damage(e, 2);   /* type-2 = instakill class (dmg 1000); hp 0 -> -1000 */
    if (!landed)        { fprintf(stderr, "FAIL(6): a hit on the writher must land (it is targetable)\n"); fail = 1; }
    if (e->state != 3)  { fprintf(stderr, "FAIL(6): one damaging hit on HP-0 spawn must -> DEATH state 3, got %d\n", e->state); fail = 1; }
    printf("  (6a) KILLABLE: after 1 hit, hp=%d state=%d (DEATH)\n", e->hp, e->state);
    for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
    if (e->state != 7)  { fprintf(stderr, "FAIL(6): DEATH must settle into CORPSE state 7, got %d\n", e->state); fail = 1; }
    printf("  (6b) CORPSE: death anim settled to state=%d (inert corpse)\n", e->state);

    if (fail) { printf("WRITHE-HAZARD: FAIL\n"); return 1; }
    printf("WRITHE-HAZARD: all checks passed\n");
    return 0;
}
