/**
 * @file test_r35_raeume_hundeschatten.c
 * @brief Runde 35 Spur H, Punkt 1 — Hunde-Schatten beim Sprung durch die Luke (ROOM1190).
 *
 * Nutzer: "In ROOM 1190 haben die Hunde einen Schatten waehrend sie durch die Luke springen in der
 * Luft. Das ist quatsch und muss waehrend des Springens raus."
 *
 * Laedt die ECHTE ROOM1190.RDT, spawnt die drei Hunde ueber das raum-eigene SCD (sub13), gibt sie
 * wie sub10 frei (grid 0x43, `Member_set 0x0C=0x43` @RDT 0x027E6) und faehrt die Sprungmaschine
 * FUN_80111398 Bild fuer Bild. Gemessen wird die Quad-Hoehe, die der Zeichner bekommt
 * (re15_hundeschatten_y — dieselbe Funktion, die main.c im NPC-Schatten-Pfad ruft):
 *   (1) jeder Hund ist waehrend des Sprungs mindestens 10 Bilder in der Luft (y < Boden) — sonst
 *       misst der Test nichts;
 *   (2) in JEDEM Luft-Bild liegt der Schatten auf der Boden-Referenz +0x1ba (FUN_8010d7f8
 *       `lh a1,442(v0)` @0x8010d91c -> `jal 0x8001b064` @0x8010d920) und NICHT auf dem Koerper;
 *   (3) die Boden-Referenz ist ab dem Absprung 0 (`sh zero,442(v0)` @0x801114f0) = Raumboden;
 *   (4) nach der Landung (Zustand 1) liegen Schatten und Koerper gleich (y == Boden);
 *   (5) fuer andere Typen bleibt der Port unveraendert (Schatten = e->y).
 * Beide KI-Geschmaecker: die Skript-Zustaende 4/5/6 laufen in beiden ueber die RE1.5-Maschine
 * (enemy_ai_common.c re15_dog_ai_tick).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_hundeschatten.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

static void lauf(const re15_rdt_t *rdt, int flavor, const char *name)
{
    re15_ai_flavor_set(flavor);
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x35u);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    /* sub13 = die drei Sce_em_set 0x44/Typ 0x20/grid 0x40 (@RDT 0x2900/0x2914/0x2928) */
    if (rdt->sub_scd_count > 13 && rdt->sub_scd[13]) scd_thread_start(2, rdt->sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 900; pl->y = 0; pl->z = -19300; pl->hp = 100; pl->floor = 0;

    int ds[RE15_ACTOR_MAX], nd = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_HUNDESCHATTEN_TYP) ds[nd++] = s;
    CHECK(nd == 3, "[%s] ROOM1190 sub13 muss 3 Hunde spawnen, %d", name, nd);
    if (nd < 1) return;

    frame();                                       /* INIT: grid 0x40 -> Zustand 4 */
    /* sub10 @RDT 0x027E6: Member_set 0x0C=0x43, Hund 1/2 zusaetzlich 0x01 = -3600 */
    for (int i = 0; i < nd; i++) {
        re15_actor_t *e = &g_actors[ds[i]];
        CHECK(e->state == 4, "[%s] Hund %d nach INIT in Zustand 4 (grid 0x40 @0x8010db88), state=%d",
              name, ds[i], e->state);
        e->grid_id = 0x43;
        e->y = -3600;
    }

    int luft[RE15_ACTOR_MAX] = {0}, gelandet[RE15_ACTOR_MAX] = {0}, im_koerper = 0;
    int min_y[RE15_ACTOR_MAX];
    for (int i = 0; i < nd; i++) min_y[i] = 0;
    for (int f = 0; f < 400; f++) {
        frame();
        for (int i = 0; i < nd; i++) {
            re15_actor_t *e = &g_actors[ds[i]];
            const int32_t sy = re15_hundeschatten_y(e);
            CHECK(sy == (int32_t)e->dog_floor_y,
                  "[%s] F%d Hund %d: Schatten-Y %d != Boden-Referenz %d (@0x8010d91c)",
                  name, f, ds[i], sy, (int)e->dog_floor_y);
            if (e->y < (int32_t)e->dog_floor_y) {           /* in der Luft */
                luft[i]++;
                if (e->y < min_y[i]) min_y[i] = e->y;
                if (sy == e->y) im_koerper++;
                if (e->state == 4 && e->sub_state_1 == 0)
                    CHECK(e->dog_floor_y == 0,
                          "[%s] F%d Hund %d: Boden-Referenz im Sprung %d, erwartet 0 (@0x801114f0)",
                          name, f, ds[i], (int)e->dog_floor_y);
            }
            if (e->state == 1 && !gelandet[i]) {
                gelandet[i] = 1;
                CHECK(e->y == (int32_t)e->dog_floor_y && sy == e->y,
                      "[%s] Hund %d gelandet: y=%d Boden=%d Schatten=%d — muessen gleich sein",
                      name, ds[i], e->y, (int)e->dog_floor_y, sy);
            }
        }
        int alle = 1;
        for (int i = 0; i < nd; i++) if (!gelandet[i]) alle = 0;
        if (alle) break;
    }
    for (int i = 0; i < nd; i++) {
        printf("  [%s] Hund %d: %d Luft-Bilder, hoechster Punkt y=%d, gelandet=%d\n",
               name, ds[i], luft[i], min_y[i], gelandet[i]);
        CHECK(luft[i] >= 10, "[%s] Hund %d war nur %d Bilder in der Luft — Sprung nicht gemessen",
              name, ds[i], luft[i]);
        CHECK(gelandet[i], "[%s] Hund %d ist nicht gelandet (Exit 0x201 @0x8011162c)", name, ds[i]);
    }
    CHECK(im_koerper == 0, "[%s] %d Luft-Bilder mit Schatten auf Koerperhoehe", name, im_koerper);
}

int main(void)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1190.RDT fehlt\n"); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        printf("FAIL: RDT parse\n"); free(buf); return 1;
    }
    lauf(&rdt, RE15_AI_FLAVOR_RE2,  "RE2");
    lauf(&rdt, RE15_AI_FLAVOR_RE15, "RE1.5");

    /* (5) andere Typen unveraendert */
    {
        re15_actor_t z = {0};
        z.type = 0x10; z.y = -777; z.dog_floor_y = 0;
        CHECK(re15_hundeschatten_y(&z) == -777, "Zombie-Schatten muss e->y bleiben");
        re15_actor_t h = {0};
        h.type = RE15_HUNDESCHATTEN_TYP; h.y = -1200; h.dog_floor_y = 0;
        CHECK(re15_hundeschatten_y(&h) == 0, "Hund in der Luft: Schatten am Boden");
    }
    free(buf);
    if (fails) { printf("test_r35_raeume_hundeschatten: %d FAIL\n", fails); return 1; }
    printf("test_r35_raeume_hundeschatten: OK\n");
    return 0;
}
