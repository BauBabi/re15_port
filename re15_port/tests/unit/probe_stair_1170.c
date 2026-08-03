/* probe_stair_1170.c — REGRESSION zur per-Record-Richtungswahl (stair_10a0.md Fix):
 * ROOM1170 (Helipad) funktionierte VOR dem Fix nur, weil die event_id-Paarung dort
 * zufaellig die richtigen Partner fand. Nach dem Umbau auf die byte-true
 * per-Record-Entscheidung (LAB_80043500/LAB_800435cc: Rect-Haelfte x side, Ziel =
 * band +/- count&7) muessen die vier 1170-Zonen weiterhin die bekannten Ergebnisse
 * liefern: X-Treppe Courtyard(4)<->Plattform(2) (slots 9/10, sce 12, side 0),
 * Z-Treppe Plattform(2)<->Pit(0) (slots 7/8, sce 13, side 1), counts alle 2.
 *
 * Spieler-Positionen = die BEGEHBARE Haelfte der jeweiligen Zone (oberes Ende =
 * bergseitige/Nicht-side-Haelfte -> DESCEND, unteres Ende = talseitige/side-Haelfte
 * -> ASCEND) — exakt die Haelften, die der reale Anlauf erzwingt. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_aot.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

typedef struct {
    const char *name;
    int   band;
    int32_t x, z;
    int   rot_y;
    int   expect_target;
    const char *expect_dir;   /* "DOWN" / "UP" */
} scenario_t;

static int s_fail = 0;

static void run_scenario(const re15_rdt_t *rdt, const scenario_t *s)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = s->x; pl->z = s->z;
    pl->y = -(int32_t)s->band * 0x708;
    pl->rot_y = (int16_t)s->rot_y;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_stair_reset();
    re15_collision_set_band(s->band);

    printf("--- %s: band=%d pos=(%ld,%ld) rot=%d  erwarte %s -> Band %d\n",
           s->name, s->band, (long)pl->x, (long)pl->z, s->rot_y,
           s->expect_dir, s->expect_target);
    int started = re15_stair_try_start(rdt, 1);
    if (!started) { printf("    FAIL: KEIN Start\n\n"); s_fail = 1; return; }
    const char *dir = (pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN) ? "DOWN" : "UP";
    for (int f = 0; f < 900 && re15_stair_active(); f++)
        re15_stair_tick(rdt, NULL, NULL);
    int end_band = re15_collision_band_from_y(pl->y);
    int ok = (end_band == s->expect_target) && (strcmp(dir, s->expect_dir) == 0);
    if (!ok) s_fail = 1;
    printf("    PORT: %s -> Ende band=%d  [%s]\n\n", dir, end_band, ok ? "OK" : "FAIL");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = -20505; pl->y = -4 * 0x708; pl->z = -25595;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    /* [STAIRREG] 1170: slot7 t13 chain0 side1 cnt2 corner(z)=-21090 ext=890  c=(-25670,-20645)
     *             slot8 t13 chain2 side1 cnt2 corner(z)=-24130 ext=1410 c=(-25640,-23425)
     *             slot9 t12 chain2 side0 cnt2 corner(x)=-24030 ext=1470 c=(-23295,-26155)
     *             slot10 t12 chain4 side0 cnt2 corner(x)=-21060 ext=740 c=(-20690,-25595) */
    scenario_t sc[] = {
        { "X-Treppe 4->2 (slot10, high-X-Haelfte)",  4, -20505, -25595, 2048, 2, "DOWN" },
        { "X-Treppe 2->4 (slot9, low-X-Haelfte)",    2, -23662, -26155,    0, 4, "UP"   },
        { "Z-Treppe 2->0 (slot8, low-Z-Haelfte)",    2, -25640, -23777, 3072, 0, "DOWN" },
        { "Z-Treppe 0->2 (slot7, high-Z-Haelfte)",   0, -25670, -20423, 1024, 2, "UP"   },
    };
    for (size_t i = 0; i < sizeof sc / sizeof sc[0]; i++)
        run_scenario(&rdt, &sc[i]);

    printf(s_fail ? "REGRESSION: FAIL\n" : "REGRESSION: alle 4 Szenarien OK\n");
    free(data);
    return s_fail;
}
