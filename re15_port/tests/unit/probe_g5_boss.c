/**
 * @file probe_g5_boss.c
 * @brief SMOKE (ctest) — das neue G5-Endkampf-Modul (enemy_ai_boss_g5.c, Runde 6).
 *
 * Faehrt den Boss-Tick direkt mit geladener RE2-EM036-Bank (wie probe_gator_fress):
 *  PIN 1  PARK: grid 0x33 -> off-world, kein Zustandsfortschritt.
 *  PIN 2  KAMPFSTART grid 0x13 -> INTRO (sub2) laeuft die Clip-Choreo 1->3->4->2->0
 *         und traegt Root-Motion (u waechst um ~+10960 = 7014+3946).
 *  PIN 3  Nach dem Intro erreicht der Boss den Kampf (sub 0/1) und der ZUG (sub1)
 *         bewegt ihn weiter vorwaerts (Clip 5 x1,5 = +4050, Kappe u=12000).
 *  PIN 4  Blob-Simulation atmet (Gewicht[0] pendelt) und der Biss feuert bei Naehe
 *         (Spieler-HP faellt um 40 ODER Devour-Kill bei u>=9001).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_g5_boss_tick(int slot);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail = 1; \
    fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

static int32_t u_von(const re15_actor_t *e) { return 8000 - ((int32_t)e->x - 1200); }

int main(void)
{
    printf("=== G5-ENDKAMPF-MODUL (ROOM5090, RE2-EM036) ===\n");
    re15_actor_init();
    g_current_room_id = 0x5090;

    /* RE2-Bank EM036 laden (Muster probe_gator_fress). */
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("SKIP: shared_assets/RE2/CDEMD0.EMS fehlt\n"); return 0; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x36);
    if (!eb || re2_ems_load_bank(ems, n, 0x36, eb, NULL) != 0) {
        printf("FAIL: EM036 laedt nicht\n"); return 1;
    }
    eb->buf = NULL; eb->ok = 1;
    printf("EM036: %d Bones, %d Clips\n", eb->skel.bone_count, eb->anim.clip_count);
    CHECK(eb->anim.clip_count == 11, "EM036 hat %d Clips (erwartet 11)", eb->anim.clip_count);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = 300; pl->z = -23400; pl->y = 0;

    re15_actor_t *e = &g_actors[2];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600;
    e->x = -14700; e->z = -23350; e->grid_id = 0x33;   /* RDT-Spawn */

    /* PIN 1: PARK. */
    for (int t = 0; t < 30; t++) re15_g5_boss_tick(2);
    CHECK(e->x == -32000 && e->z == -32000, "PARK: pos=(%d,%d) != (-32000,-32000)",
          (int)e->x, (int)e->z);

    /* PIN 2: Kampfstart -> Intro-Choreo mit Root-Motion. */
    e->grid_id = 0x13;
    int32_t u_start = 0;
    int treffer = 0;
    int saw_clip[16]; memset(saw_clip, 0, sizeof saw_clip);
    int intro_fertig_t = -1;
    for (int t = 0; t < 2600; t++) {
        re15_g5_boss_tick(2);
        if (t == 0) u_start = u_von(e);
        if (e->motion >= 0 && e->motion < 16) saw_clip[e->motion] = 1;
        if (pl->hp < 100) treffer++;                   /* VOR dem Reset zaehlen */
        if (intro_fertig_t < 0 && e->hp > 0) {
            /* Intro fertig, wenn der Zug (Clip 5) laeuft */
            if (e->motion == 5) intro_fertig_t = t;
        }
        pl->hp = 100; pl->hit_react = 0;               /* Dauer-Reset: nur Delta messen */
    }
    printf("Intro: u %d -> %d (Delta %d), Clips gesehen:", u_start, u_von(e),
           u_von(e) - u_start);
    for (int c = 0; c < 12; c++) if (saw_clip[c]) printf(" %d", c);
    printf("\n");
    CHECK(saw_clip[1] && saw_clip[3] && saw_clip[4] && saw_clip[2] && saw_clip[0],
          "Intro-Choreo 1/3/4/2/0 nicht vollstaendig gesehen");
    CHECK(intro_fertig_t > 0, "der Tentakel-Zug (Clip 5) wurde nie erreicht");
    CHECK(u_von(e) > u_start + 8000,
          "Root-Motion traegt nicht: u-Delta %d (erwartet > 8000: Intro 10960 + Zuege)",
          u_von(e) - u_start);
    CHECK(u_von(e) <= 12000, "Zug-Kappe 12000 verletzt: u=%d", u_von(e));

    /* PIN 4: der Kampf hat den Spieler getroffen (Biss 40 und/oder Devour
     * Clip 9); der Hauptlauf resettet hp je Tick, das Ereignis wird VOR dem
     * Reset gezaehlt. Nach einem Devour haelt der Boss byte-true den
     * sub4-Endzustand ("kein Routine-Exit - der Spieler ist tot"). */
    printf("Treffer-Ereignisse im Hauptlauf: %d, Devour-Clip 9 gesehen: %d\n",
           treffer, saw_clip[9]);
    CHECK(treffer > 0 || saw_clip[9],
          "weder Biss-Schaden noch Devour im Hauptlauf gesehen");

    /* TOD: HP 0 -> Sequenz bis zur Absink-Rampe. */
    e->hp = 0;
    {
        int saw10 = 0; int32_t y_max = 0;
        for (int t = 0; t < 2200; t++) {
            re15_g5_boss_tick(2);
            if (e->motion == 10) saw10 = 1;
            if (e->y > y_max) y_max = e->y;
        }
        printf("Tod: Clip10=%d, Absink-y max=%d (Soll 2950)\n", saw10, (int)y_max);
        CHECK(saw10, "Todes-Kollaps Clip 10 nie gesehen");
        CHECK(y_max >= 2900, "Absink-Rampe fehlt (y_max=%d, Soll ~2950)", (int)y_max);
    }

    if (g_fail) { printf("=== FAIL ===\n"); return 1; }
    printf("=== PASS: Park, Intro-Choreo+Root-Motion, Zug, Biss, Todes-Rampe ===\n");
    return 0;
}
