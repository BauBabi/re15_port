/* test_crow_fixes.c — pins the ROOM1170 crow-AI fixes from the RE dossier
 * analysis/crow_1170.md (adversarially CONFIRMED 2026-08-02):
 *
 *  D1: INIT @0x801123cc-e8 sets +0x1db = (grid & 0x10) ? 0 : 1 — ROOM1170 spawns its 7 crows
 *      with grid 2x0x90 / 3x0x10 / 2x0x00, so 5 of 7 are PERMANENTLY disarmed (they launch and
 *      circle but never commit an attack; re-arm only via the aca50 death-lane broadcast).
 *      The port used to hardcode crow_armed = 1 -> all 7 attacked (the user-visible divergence).
 *
 *  D5: move[0] patrol timer @0x80112824-2c decrements UNCONDITIONALLY (0 wraps to 255, store in
 *      the branch delay slot); at t==0 the completion flag +0x1d3 increments additionally. */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include <stdio.h>
#include <string.h>

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); fails++; } } while (0)

static re15_actor_t *mkcrow(int slot, uint8_t grid)
{
    re15_actor_t *c = &g_actors[slot];
    memset(c, 0, sizeof(*c));
    c->active = 1; c->type = 0x21; c->flags = 1; c->hp = 10;
    c->state = 0;                       /* INIT */
    c->grid_id = grid;
    c->x = 0; c->z = 0; c->y = -7200;   /* the helipad perch height */
    c->em_flag_id = 0xFF;
    return c;
}

int main(void)
{
    re15_actor_init();
    { extern void re15_crow_flock_reset(void); re15_crow_flock_reset(); }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 60000; pl->z = 60000; pl->hp = 100; pl->hit_react = 0;

    /* ---- D1: armed flag follows grid & 0x10 ---- */
    re15_actor_t *c10 = mkcrow(1, 0x10);
    re15_actor_t *c00 = mkcrow(2, 0x00);
    re15_actor_t *c90 = mkcrow(3, 0x90);
    re15_enemy_ai_run_all(1);                       /* one tick: all three run INIT */
    CHECK(c10->crow_armed == 0, "D1: grid 0x10 -> disarmed (+0x1db=0)");
    CHECK(c00->crow_armed == 1, "D1: grid 0x00 -> armed (+0x1db=1)");
    CHECK(c90->crow_armed == 0, "D1: grid 0x90 -> disarmed (0x90 & 0x10)");
    CHECK(c00->state == 1, "INIT muss nach ACTIVE (state 1) uebergehen");

    /* ---- D5: move[0] timer wraps 0 -> 255 with the completion flag ---- */
    c00->sub_state_1 = 0; c00->sub_state_2 = 1;     /* patrol step1 (timer already seeded) */
    c00->crow_timer = 0; c00->crow_pturn = 0;
    c10->active = 0; c90->active = 0;               /* isolate */
    re15_enemy_ai_run_all(1);
    CHECK(c00->crow_timer == 255, "D5: Timer muss unconditional dekrementieren (0 -> 255)");
    CHECK(c00->crow_pturn >= 1, "D5: bei t==0 muss der Completion-Flag inkrementieren");

    if (fails == 0) { printf("test_crow_fixes: OK\n"); return 0; }
    printf("test_crow_fixes: %d FAILURES\n", fails);
    return 1;
}
