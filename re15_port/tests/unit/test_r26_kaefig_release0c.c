/* test_r26_kaefig_release0c.c — PIN fuer die RE1.5-Kaefig-Freigabe (ROOM11D0/11D1/3060/3061).
 *
 * BEFUND (Nutzer): "Bei Room 11D0, nach dem Aufheben der Beretta M93R gibt es nur komische
 * Bluteffekte beim Starten der Cutscene, wo die Hunde erscheinen."
 *
 * MECHANISMUS (alle Adressen in dieser Runde selbst disassembliert):
 *   - Die RE1.5-Kaefig-Freigabe schreibt das Zustandswort 0x0C01:
 *       STAGE1.BIN  8011170c: lbu v1,9(a0)      ; grid
 *                   80111710: ori v0,zero,0x42
 *                   80111714: bne v1,v0,0x80111754
 *                   80111718: ori v0,zero,0xc01 ; Delay-Slot
 *                   8011171c: sb  zero,9(a0)    ; grid verbraucht
 *                   8011172c: sw  v0,4(v1)      ; +0x4=0x01 +0x5=0x0C +0x6=+0x7=0
 *   - Sub 0x0C existiert NUR in der RE1.5-ACT-Tabelle @0x80120FD4 (Rohwort @0x80121004)
 *     = 0x801101E4: Clip 8 aufstehen -> Sub 2, EINZIGER jal = anim_set 0x8001f314 @0x80110288,
 *     null Effekte.
 *   - In RE2 ist @0x80105464[12] = 0x80102608 der Post-Latch; sein Schwanz zieht Blut
 *     (`jal 0x80105070` @0x80102804 / @0x80102844) und seine Phase 0 zieht Clip 14 auf
 *     (`re2d_clip(e,14,...)` @0x801026B4-E4).
 *   - Herkunftsmerkmal ist noetig, weil der ECHTE RE2-Abwurf denselben Zustand erzeugt:
 *       EMD0G_MOD0.BIN 80102218: addiu v0,zero,12
 *                      8010221c: sb    v0,5(s1)
 *                      80102220: sh    zero,6(s1)
 *
 * Der Pin faellt am alten Stand durch: dort lief der freigegebene Hund in
 * re2d_sub12_postlatch und bekam Clip 14 statt Clip 8.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_esp.h"

static int g_fail = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); \
                                       printf("  [%s:%d]\n", __FILE__, __LINE__); g_fail = 1; } } while (0)

static re15_actor_t *dog_setup(void)
{
    re15_actor_init();
    re15_esp_fx_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);          /* ausgelieferter Default seit 2026-08-22 */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0;

    re15_actor_t *d = &g_actors[1];
    memset(d, 0, sizeof *d);
    d->active = 1; d->type = 0x20; d->flags = 1; d->hp = 100;
    d->x = 2000; d->z = 0; d->em_flag_id = 0xFF;
    d->grid_id = 0x41;                               /* Fenster-/Zwinger-Hund, Sce_em_set pc[3] */
    re15_enemy_ai_live_init(1);
    d->hp = 100;
    return d;
}

int main(void)
{
    /* ---------------- (1) Der Nutzerfall: Kaefig-Freigabe -> Clip 8, kein Blut ------------- */
    re15_actor_t *d = dog_setup();

    /* state 4 / sub 1 = Fenster-Halten (0x80111658), Phase 0 zieht Clip 8 auf. */
    d->state = 4; d->sub_state_1 = 1; d->sub_state_2 = 0; d->sub_state_3 = 0;
    int armed = 0;
    for (int f = 0; f < 10; f++) {
        re15_enemy_ai_run_all(1);
        if (d->sub_state_2 == 1) { armed = 1; break; }
    }
    CHECK(armed, "Fenster-Halten hat Phase 1 nicht erreicht (ss2=%d state=%d)",
          (int)d->sub_state_2, (int)d->state);

    /* Member_set(0x0C, 66) aus sub02 der Cutscene: EXE-Tabelle @0x80010c8c[12] = 0x800411f4,
     * dort `j 0x80041230` mit Delay-Slot `sb a2,9(a0)` -> grid := 66 = 0x42. */
    d->grid_id = 0x42;
    re15_enemy_ai_run_all(1);
    CHECK(d->state == 1 && d->sub_state_1 == 0x0c,
          "Freigabe schrieb nicht 0x0C01: state=%d sub=%d", (int)d->state, (int)d->sub_state_1);
    CHECK(d->grid_id == 0, "grid wurde nicht verbraucht (%d)", (int)d->grid_id);

    const int fx_before = re15_esp_fx_count();
    int saw_clip8 = 0, saw_clip14 = 0, exited_to2 = 0, ticks = 0;
    for (int f = 0; f < 240; f++) {
        re15_enemy_ai_run_all(1);
        ticks++;
        if (d->state != 1) break;
        if (d->sub_state_1 == 0x0c) {
            if (d->motion == 8)  saw_clip8  = 1;
            if (d->motion == 14) saw_clip14 = 1;
        } else { exited_to2 = (d->sub_state_1 == 2); break; }
    }
    printf("Abdeckung (1): %d Ticks, motion=%d sub=%d fx %d->%d\n",
           ticks, (int)d->motion, (int)d->sub_state_1, fx_before, re15_esp_fx_count());
    CHECK(saw_clip8,  "Sub 0x0C spielte NICHT Clip 8 (@0x8011023c-40)");
    CHECK(!saw_clip14, "Sub 0x0C lief in den RE2-Post-Latch (Clip 14 @0x801026B4-E4)");
    CHECK(re15_esp_fx_count() == fx_before,
          "Sub 0x0C hat Effekte gespawnt (%d -> %d) — 0x801101E4 ruft NUR anim_set @0x80110288",
          fx_before, re15_esp_fx_count());
    CHECK(exited_to2, "Sub 0x0C ist nicht nach Sub 2 ausgestiegen (@0x801102b0), sub=%d",
          (int)d->sub_state_1);
    CHECK(d->dog_cage_rel0c == 0, "Herkunftsmarke wurde am Ausgang nicht verbraucht");

    /* ---------------- (2) Regression: der ECHTE RE2-Abwurf bleibt im Post-Latch ------------ */
    d = dog_setup();
    d->state = 1; d->sub_state_1 = 0x0c; d->sub_state_2 = 0; d->sub_state_3 = 0;
    d->dog_cage_rel0c = 0;                       /* Herkunft: @0x8010221c, NICHT die Kaefig-Freigabe */
    re15_enemy_ai_run_all(1);
    printf("Abdeckung (2): 1 Tick, motion=%d sub=%d ss3=%d\n",
           (int)d->motion, (int)d->sub_state_1, (int)d->sub_state_3);
    CHECK(d->motion == 14,
          "RE2-Post-Latch @0x80102608 wurde mit abgeschnitten (motion=%d statt 14)",
          (int)d->motion);

    /* ---------------- (3) Fehlstelle C1: Idle-Wander Phase 0 raeumt den Pose-Riegel ab ----- */
    /* EMD0G_MOD0.BIN 801005f4: lbu v0,467(s0) / 80100600: andi v0,v0,0x7f /
     *                80100608: sb  v0,467(s0)   — s0 = dieser Hund (`addu s0,a0,zero` @0x80100550) */
    d = dog_setup();
    d->state = 1; d->sub_state_1 = 0; d->sub_state_2 = 0; d->sub_state_3 = 0;
    d->re2z_self1d3 = (uint8_t)(0x80u | 0x05u);      /* Pose-Riegel + 5 Bilder Trefferpause */
    re15_enemy_ai_run_all(1);
    printf("Abdeckung (3): 1 Tick, +0x1D3 = 0x%02X\n", (unsigned)d->re2z_self1d3);
    CHECK((d->re2z_self1d3 & 0x80u) == 0u,
          "C1 fehlt: +0x1D3 Bit 0x80 nach Idle-Wander Phase 0 noch gesetzt (0x%02X)",
          (unsigned)d->re2z_self1d3);

    printf(g_fail ? "test_r26_kaefig_release0c: FAIL\n" : "test_r26_kaefig_release0c: OK\n");
    return g_fail;
}
