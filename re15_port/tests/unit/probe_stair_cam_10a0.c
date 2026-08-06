/* probe_stair_cam_10a0.c — MESSUNG (kein Fix): Nutzer-Report 2026-08-06
 *   "Im Treppenhaus schwenkt nach dem Runterlaufen der letzten Treppe die
 *    Kamera teilweise nicht richtig um."
 *
 * Faehrt den kompletten Abstieg ROOM10A0 (Band 8 -> 6 -> 4 -> 3 -> 1) und
 * protokolliert nach JEDEM Schritt den aktiven Cut — dabei laeuft genau der
 * Block aus game_step_common.c:446-457:
 *      re15_stair_tick(); g_scd.cut_auto_enabled = 1; re15_aot_scan();
 * und die Cut-Uebernahme (aot_common.c:1065 g_scd.cam_id -> active_cut).
 *
 * ORIGINAL-Referenz:
 *   RVD-Scan  FUN_80014230 @0x80014230 (Anker @0x80014324, Cut-Setzer
 *             FUN_800142f4 @0x800142f4 -> DAT_800afbb5 / DAT_800ac794).
 *   Treppen   ROOM10A0.RDT main00 @0xdb0: Aot_set sce=13
 *             slot3 band8 / slot4 band6  rect(20100,19500,2700,4300) side=0 cnt=2
 *             slot5 band6 / slot6 band4  rect(24000,20100,2700,4500) side=1 cnt=2
 *             slot7 band4 / slot8 band3  rect(20100,19500,2700,2300) side=0 cnt=1
 *             slot9 band3 / slot10 band1 rect(24000,20100,2700,4500) side=1 cnt=2
 *   Tueren nach ROOM10A0 (Door_aot_set pc[24] = Eintritts-Cut):
 *             aus ROOM1050 @0xb5a  spawn(26200,-14400,24800) yaw2048 -> cut 0  (Band 8)
 *             aus ROOM1180 @0xa34  spawn(21200, -7200,25500) yaw1024 -> cut 2  (Band 4)
 *             aus ROOM11E0 @0x1552 spawn(21200, -1800,25500) yaw1024 -> cut 8  (Band 1)
 *   => Die Band-1-Plattform (Ziel der LETZTEN Treppe) wird im Original von
 *      CUT 8 gerahmt.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;

static int s_cut = 0;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* Die Cut-Uebernahme des Main-Loops (aot_common.c:1065-1068 + main.c cut block) */
static int apply_scan(void)
{
    if (g_scd.cam_change_pending) {
        int old = s_cut;
        s_cut = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        return old != s_cut;
    }
    return 0;
}

static void step_scan(const char *tag)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)s_cut);
    if (apply_scan())
        printf("      CUT -> %d   @(%ld,%ld) band=%d   [%s]\n",
               s_cut, (long)pl->x, (long)pl->z,
               re15_collision_debug_band(), tag);
}

/* Gerade Linie in 40er-Schritten, Scan pro Schritt (Ersatz fuer den Walker;
 * die Zonenauswahl selbst ist per probe_rvd_diff_10a0 als byte-true belegt). */
static void walk_to(int32_t tx, int32_t tz)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    while (pl->x != tx || pl->z != tz) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t ax = dx < 0 ? -dx : dx, az = dz < 0 ? -dz : dz;
        int32_t len = ax > az ? ax : az;
        if (len > 40) {
            pl->x += (int32_t)((int64_t)dx * 40 / len);
            pl->z += (int32_t)((int64_t)dz * 40 / len);
        } else { pl->x = tx; pl->z = tz; }
        step_scan("walk");
    }
}

typedef struct { const char *name; int band; int32_t x, z; int rot; int target; } stair_t;

static void descend(const re15_rdt_t *rdt, const stair_t *s)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)s->rot;
    pl->y     = -(int32_t)s->band * 0x708;
    re15_collision_set_band(s->band);
    printf("  -- %s: Start band=%d pos=(%ld,%ld) cut=%d\n",
           s->name, s->band, (long)pl->x, (long)pl->z, s_cut);
    if (!re15_stair_try_start(rdt, 1)) {
        printf("     KEIN Treppenstart (try_start=0)\n");
        return;
    }
    int f = 0;
    while (re15_stair_active() && f < 900) {
        re15_stair_tick(rdt, NULL, NULL);
        step_scan("stair");
        f++;
    }
    printf("     Ende nach %d Ticks: pos=(%ld,%ld,%ld) band=%d CUT=%d (Ziel-Band %d)\n",
           f, (long)pl->x, (long)pl->y, (long)pl->z,
           re15_collision_band_from_y(pl->y), s_cut, s->target);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10A0.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10A0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Tuer aus ROOM1050: spawn + Eintritts-Cut 0, Band 8 */
    pl->x = 26200; pl->y = -14400; pl->z = 24800; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    re15_collision_set_band(8);
    s_cut = 0;
    g_scd.cam_change_pending = 0;
    printf("START: Tuer aus ROOM1050 -> pos=(%ld,%ld,%ld) band=8 cut=%d\n\n",
           (long)pl->x, (long)pl->y, (long)pl->z, s_cut);

    static const stair_t st[4] = {
        { "Treppe A  band8->6", 8, 21450, 23000, 1024, 6 },
        { "Treppe B  band6->4", 6, 25350, 21000, 3072, 4 },
        { "Treppe C  band4->3", 4, 21450, 21400, 1024, 3 },
        { "Treppe D  band3->1 (LETZTE)", 3, 25350, 21000, 3072, 1 },
    };
    for (int i = 0; i < 4; i++) {
        printf("=== Weg zur %s ===\n", st[i].name);
        walk_to(st[i].x, st[i].z);
        descend(&rdt, &st[i]);
        printf("\n");
    }

    printf("=== Weg zur Tuer ROOM11E0 (21200,25500) — dort rahmt das Original CUT 8 ===\n");
    walk_to(21200, 25500);
    printf("ENDE: pos=(%ld,%ld,%ld) band=%d CUT=%d  (Original-Tuer-Cut fuer Band 1 = 8)\n",
           (long)pl->x, (long)pl->y, (long)pl->z,
           re15_collision_band_from_y(pl->y), s_cut);

    free(data);
    return 0;
}
