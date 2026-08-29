/* test_writher_hit_feedback.c — PIN (Nutzer-Report 2026-08-29 "Es gibt keinerlei
 * Treffer-Feedback wenn ich auf die Arme schiesse. Kein Zurueckziehen, kein Bluteffekt,
 * kein nichts."):
 *
 * (1) ZIELBAND: die Gitterhand (0x1A) ist LEVEL- UND DOWN-treffbar (re15_damage.c;
 *     Nachruestungs-Ziel — das Original liefert die Arme mit HP 0 aus, ROOM1210
 *     Sce_em_set @0x1d86..0x1e4e Bytes 4-5 = 00 00). Vorher generischer LEVEL-Zweig:
 *     DOWN-Schuesse auf die bodennahen Arme waren stille Whiffs.
 * (2) FLINCH-FEEDBACK: der Treffer betritt den byte-true Original-Flinch 0x8010d188
 *     (im Auslieferungsstand mangels HP unerreichbarer Code): Clip 2 neu (+0x94=2
 *     @0x8010d200, +0x95=0 @0x8010d210), Blend 3 (+0x8f @0x8010d220), BLUT am Bone 1
 *     (FUN_80019700(0x2000,...) @0x8010d268-8c).
 * (3) BUDGET: ist das Trefferbudget (+0x1D2) negativ, taucht der Arm am Flinch-Ende ab
 *     (`bgez`-Weiche @0x8010d14c -> Abtauch-Sub, Port-Sub 6).
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
extern int  re15_esp_fx_count(void);
extern void re15_esp_fx_reset(void);

#define WS 1   /* writher slot */

static re15_actor_t *setup(void)
{
    re15_actor_init();
    re15_esp_fx_reset();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = 0; pl->y = 0; pl->z = 0;

    re15_actor_t *e = &g_actors[WS];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x1a;
    e->x = 1500; e->y = 0; e->z = 0;
    e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x1a);
    re15_enemy_ai_run_all(1);                     /* INIT: state->1, HP/Budget-Seed */

    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
    re15_player_aim_reset();
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    return e;
}

static int fire(int elev)
{
    re15_player_set_aim_elevation_for_test(elev);
    return re15_player_weapon_fire(3);            /* Pistole (Budget-Kosten -1) */
}

int main(void)
{
    printf("=== Gitterhand-Treffer-Feedback (Band + Original-Flinch 0x8010d188) ===\n");

    /* --- A: DOWN-Schuss registriert + Flinch mit Blut --- */
    {
        re15_actor_t *e = setup();
        CHECK("Arm hat HP/Budget nach INIT (Nachruestung v0.3.34)",
              e->hp > 0 && e->writher_hits >= 1);
        int fx0 = re15_esp_fx_count();
        CHECK("A: DOWN-Schuss trifft den bodennahen Arm", fire(-1) == WS + 1);
        CHECK("A: Treffer -> HURT (state 2)", e->state == 2);
        re15_enemy_ai_run_all(1);                 /* Flinch-Eintritt */
        CHECK("A: Flinch-Clip 2 neu gestartet (+0x94=2/+0x95 klein, @0x8010d200/210)",
              e->motion == 2 && e->anim_frame <= 1);
        CHECK("A: Blend 3 (+0x8f @0x8010d220)", e->anim_frac == 3);
        CHECK("A: genau 1 Blut-FX am Flinch-Eintritt (@0x8010d268-8c)",
              re15_esp_fx_count() == fx0 + 1);
    }

    /* --- B: LEVEL-Schuss registriert ebenfalls --- */
    {
        re15_actor_t *e = setup();
        CHECK("B: LEVEL-Schuss trifft", fire(0) == WS + 1);
        CHECK("B: -> HURT", e->state == 2);
    }

    /* --- C: Budget verbraucht -> Abtauchen (Sub 6) am Flinch-Ende --- */
    {
        re15_actor_t *e = setup();
        e->writher_hits = 1;                      /* Pistole kostet -1 -> nach 2 Treffern < 0 */
        CHECK("C: Treffer 1", fire(-1) == WS + 1);
        for (int t = 0; t < 80 && e->state != 1; t++) re15_enemy_ai_run_all(1);
        CHECK("C: Flinch 1 endet in Ruhe (Budget 0 >= 0)", e->state == 1 && e->sub_state_1 == 0);
        CHECK("C: Treffer 2", fire(-1) == WS + 1);
        for (int t = 0; t < 80 && e->state != 1; t++) re15_enemy_ai_run_all(1);
        CHECK("C: Budget < 0 -> ABTAUCHEN (Weiche @0x8010d14c, Port-Sub 6)",
              e->state == 1 && e->sub_state_1 == 6);
    }

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
