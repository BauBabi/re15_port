/* test_grid3_lauerer.c — RIEGEL: Zombie-Nibble 3 (Deskriptor 0x83, ROOM3010/3011) = Kriech-Maschine von Grid 1 mit
 * DECIDE[5] = 0x80104760 (nur Griff, kein Loskriechen bei < 3000 wie Grid 1 @0x80104704-48). RE1.5-Gehirn. */
#include <stdio.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static re15_actor_t *setup(uint8_t grid, int32_t pz)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = pz;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x11; e->state = 1; e->grid_id = grid; e->sub_state_1 = 5; e->rot_y = 0xc00;   /* blickt nach +z */
    e->ai_dist = (uint32_t)(pz < 0 ? -pz : pz); e->hp = 50;
    return e;
}
int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_t *e = setup(0x83, 2000);
    re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 5, "Nibble 3 bei 2000: bleibt lauernd (+0x5 = %d)", e->sub_state_1);
    e = setup(0x81, 2000);
    re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 0, "Nibble 1 bei 2000: kriecht los (+0x5 = %d)", e->sub_state_1);
    e = setup(0x83, 1000);
    re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 1 || e->sub_state_1 == 2, "Nibble 3 bei 1000 vorn: Griff (+0x5 = %d)", e->sub_state_1);
    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n"); return 0;
}
