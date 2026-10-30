/* test_tyrant_b11.c — RIEGEL: Tyrant/Spider-Man (Typ 0x2b) Wand-/Deckenwechsel B[11] 0x80113438 (STAGE4).
 *   phasen   P0/P1 dreht auf (+0x9f&0xf0)<<4 (Rate 0x10), P2/P3 Clip 0x16 + Box klein in Bild 0xa (@0x8011a0a0),
 *            Bild 0x23 -> P4; P4: grid 0 -> 8, y -= 2014, +0x6c -= 0x400; P5: +0x5 = 12 (@0x801002c4[8]).
 *   wand     A[12] grid 8: y faellt bis < -3600 -> +0x5 = 11, +0x6 = 2; Wechsel 8 -> 0x10, y = -5400, Box gross,
 *            P5 -> +0x5 = 13 (Decke).
 *   decke    B[13] laeuft rueckwaerts (FUN_800245d8(0x800)), lenkt nicht. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

int main(void)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 30000; pl->hp = 100; pl->hit_react = 1;   /* weit weg, reagiert: kein Angriff */
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x2b; e->state = 1; e->grid_id = 0; e->rot_y = 0; e->hp = 100;
    re15_enemy_apply_hitbox(e, 0x2b);
    e->sub_state_1 = 11; e->sub_state_2 = 0; e->sub_state_3 = 0; e->zg_9f = (int8_t)0x40;   /* Ziel 0x400 */
    int gedreht = 0;
    for (int f = 0; f < 200 && e->sub_state_2 < 2; f++) { pl->hit_react = 1; re15_enemy_ai_run_all(0); if (e->sub_state_2 >= 1) gedreht = 1; }
    CHECK(gedreht && e->sub_state_2 >= 2 && ((e->rot_y - 0x400) & 0xfff) < 0x20, "P1 dreht auf 0x400 (rot %d, P %d)", e->rot_y, e->sub_state_2);
    int box_klein = 0;
    for (int f = 0; f < 200 && e->sub_state_1 == 11 && e->sub_state_2 < 5; f++) {
        pl->hit_react = 1; re15_enemy_ai_run_all(0);
        if (e->hit_radius_min == 50 && e->hit_height == 900) box_klein = 1;
    }
    CHECK(box_klein, "Box klein @0x8011a0a0 in Bild 0xa");
    CHECK(e->grid_id == 8 && e->y == -2014, "P4: grid 0 -> 8, y -2014 (grid %d y %d)", e->grid_id, e->y);
    CHECK(e->rot_z == (int16_t)-0x400, "P4: +0x6c -= 0x400 (rot_z %d)", e->rot_z);
    pl->hit_react = 1; re15_enemy_ai_run_all(0);
    CHECK(e->sub_state_1 == 12, "P5: +0x5 = 12 (Wand), ist %d", e->sub_state_1);

    /* Wand hoch bis < -3600 */
    for (int f = 0; f < 200 && e->sub_state_1 == 12; f++) { pl->hit_react = 1; re15_enemy_ai_run_all(0); }
    CHECK(e->sub_state_1 == 11 && e->sub_state_2 >= 2, "A[12]: -> B[11] P2 (sub %d P %d y %d)", e->sub_state_1, e->sub_state_2, e->y);
    for (int f = 0; f < 200 && e->sub_state_1 == 11; f++) { pl->hit_react = 1; re15_enemy_ai_run_all(0); }
    CHECK(e->grid_id == 0x10 && e->y == -5400 && e->hit_radius_min == 800, "Decke: grid 0x10, y -5400, Box gross (grid %d y %d r %d)", e->grid_id, e->y, e->hit_radius_min);
    CHECK(e->sub_state_1 == 13, "P5: +0x5 = 13 (Decke), ist %d", e->sub_state_1);

    /* Decke: rueckwaerts, keine Lenkung */
    int16_t rot0 = e->rot_y; int32_t x0 = e->x, z0 = e->z;
    for (int f = 0; f < 20; f++) { pl->hit_react = 1; re15_enemy_ai_run_all(0); }
    CHECK(e->rot_y == rot0, "B[13] lenkt nicht (rot %d -> %d)", rot0, e->rot_y);
    {   /* rot0 = 0x400: vorwaerts (cos, -sin) = (0,-1) -> rueckwaerts heisst z waechst */
        CHECK(e->z > z0 && e->x == x0, "B[13] rueckwaerts (x %d->%d z %d->%d)", x0, e->x, z0, e->z); }

    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n"); return 0;
}
