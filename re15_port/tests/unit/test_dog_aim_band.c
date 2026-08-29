/* test_dog_aim_band.c — PIN (Nutzer-Report 2026-08-29 "erster Hund-Treffer stark
 * verzoegert"): der RE1.5-Hund (Typ 0x20) traegt das byte-true Ziel-Hoehenband aus
 * seinem ACTIVE-Tail, statt pauschal LEVEL.
 *
 * Original (selbst nachdisassembliert, STAGE1.BIN / PSX.EXE):
 *   Tail FUN_8010dbcc:  @0x8010dd00-10  word0 &= 0x1fffffff
 *                       @0x8010dd20-30  Clip +0x94 == 1 (IDLE) oder 0x13 (Low-HP-Hop)
 *                                       -> LEVEL ueberspringen
 *                       @0x8010dd38-44  word0 |= 0x40000000 (LEVEL)
 *                       @0x8010dd48-4c  jal 0x80012974, a0 = 0xfa0 (R = 4000)
 *   FUN_80012974:       volle 32-bit-Deltas, SquareRoot0; dist < R ->
 *                       word0 |= 0x20000000 (DOWN) — @0x800129cc-f0, kein Clear
 *   Resolver-Gate:      (Gegner-word0 & Spieler-word0 & 0xe0000000) == 0 ->
 *                       Kandidat verworfen — @0x800120d0-ec
 *   Spawn:              Sce_em_set schreibt word0 = 1 (@0x8004228c/98) bzw. 0x2001
 *                       (@0x800422ac/b0) — Band-Bits 29-31 am Spawn 0.
 *
 * Wirkung, die hier gepinnt wird:
 *   - Hund in Reichweite (< 4000) ist AUCH mit Dpad-DOWN-Zielen treffbar (vorher: nie
 *     -> stiller Whiff = das gemeldete "verzoegerte" Feedback).
 *   - IDLE-Hund (Clip 1) ist NUR mit DOWN treffbar (vorher: LEVEL traf).
 *   - IDLE-Hund jenseits 4000: gar nicht treffbar (Band 0).
 */
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_inventory.h"
#include "re15_ai_flavor.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern void re15_player_set_aim_elevation_for_test(int elev);

#define DS 1   /* dog slot */

/* Frischer Aufbau: Spieler @Ursprung, Hund frontal in +X bei Distanz d,
 * state 1 mit vorgegebenem Sub (0 = IDLE haelt Clip 1 via ai_timer = rng+300;
 * 1 = TURN setzt Clip 2). */
static re15_actor_t *setup(int32_t d, uint8_t sub)
{
    re15_actor_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = 2000; pl->y = 0; pl->z = -3000; pl->rot_y = 0;

    re15_actor_t *e = &g_actors[DS];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->hp = 100;
    e->x = pl->x + d; e->y = 0; e->z = pl->z;
    e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x20);
    e->state = 1; e->sub_state_1 = sub; e->sub_state_2 = 0; e->sub_state_3 = 0;

    /* Spieler exakt auf den Hund ausrichten (Gun-Wedge): Mesh-rot = heading - 0x400. */
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);

    re15_player_aim_reset();
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    return e;
}

static int fire(int elev)
{
    re15_player_set_aim_elevation_for_test(elev);
    return re15_player_weapon_fire(3);               /* Handfeuerwaffe */
}

int main(void)
{
    printf("=== Hund-Hoehenband (Tail @0x8010dd00-4c + FUN_80012974(0xfa0), Resolver @0x800120d0-ec) ===\n");

    /* --- A: IDLE nah (2000 < 4000): Clip 1 haelt -> Band = NUR DOWN --- */
    {
        re15_actor_t *e = setup(2000, 0);
        for (int t = 0; t < 3; t++) re15_enemy_ai_run_all(1);
        printf("  [A] motion=%d aim_band=%u\n", (int)e->motion, (unsigned)e->aim_band);
        CHECK("A: IDLE haelt Clip 1", e->motion == 1);
        CHECK("A: Band == DOWN (1) — @0x8010dd28 skip-LEVEL + dist<0xfa0", e->aim_band == 1);
        CHECK("A: LEVEL-Schuss trifft NICHT (Original: IDLE nur mit DOWN)", fire(0) == 0);
        CHECK("A: DOWN-Schuss trifft", fire(-1) == DS + 1);
    }

    /* --- B: IDLE fern (6000 > 4000): Band = 0 -> gar nicht treffbar --- */
    {
        re15_actor_t *e = setup(6000, 0);
        for (int t = 0; t < 3; t++) re15_enemy_ai_run_all(1);
        printf("  [B] motion=%d aim_band=%u\n", (int)e->motion, (unsigned)e->aim_band);
        CHECK("B: Band == 0 (kein LEVEL wegen Clip 1, kein DOWN wegen dist)", e->aim_band == 0);
        CHECK("B: LEVEL-Schuss Whiff", fire(0) == 0);
        CHECK("B: DOWN-Schuss Whiff", fire(-1) == 0);
    }

    /* --- C: TURN nah (2000): Clip 2 -> Band = LEVEL|DOWN (3) --- */
    {
        re15_actor_t *e = setup(2000, 1);
        re15_enemy_ai_run_all(1);
        printf("  [C] motion=%d aim_band=%u\n", (int)e->motion, (unsigned)e->aim_band);
        CHECK("C: Clip != 1/0x13", e->motion != 1 && e->motion != 0x13);
        CHECK("C: Band == LEVEL|DOWN (3)", e->aim_band == 3);
        CHECK("C: LEVEL-Schuss trifft", fire(0) == DS + 1);
    }
    {   /* getrennter Aufbau: der +0x93-Latch aus C wuerde den 2. Schuss verschlucken */
        re15_actor_t *e = setup(2000, 1);
        re15_enemy_ai_run_all(1);
        (void)e;
        CHECK("C2: DOWN-Schuss trifft (Nutzer-Fall: nach unten zielen auf nahen Hund)",
              fire(-1) == DS + 1);
    }

    /* --- D: TURN fern (6000): Band = NUR LEVEL (2) --- */
    {
        re15_actor_t *e = setup(6000, 1);
        re15_enemy_ai_run_all(1);
        printf("  [D] motion=%d aim_band=%u\n", (int)e->motion, (unsigned)e->aim_band);
        CHECK("D: Band == LEVEL (2)", e->aim_band == 2);
        CHECK("D: DOWN-Schuss Whiff", fire(-1) == 0);
        CHECK("D: LEVEL-Schuss trifft", fire(0) == DS + 1);
    }

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
