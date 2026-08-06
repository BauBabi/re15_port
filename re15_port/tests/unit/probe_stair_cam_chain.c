/* probe_stair_cam_chain.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-06
 * "Im Treppenhaus schwenkt nach dem Runterlaufen der LETZTEN Treppe die Kamera
 *  teilweise nicht richtig um."
 *
 * Faehrt die KOMPLETTE Abstiegs-Kette eines Treppenhauses (ROOM10A0 bzw. ROOM1060)
 * ab: Treppe runter -> zur naechsten Treppe laufen -> runter -> ... und protokolliert
 * die RVD-Kamera-Umschaltungen (re15_aot_scan) PRO FRAME.
 *
 * Der Lauf zwischen den Treppen ist ein gerader Marsch (Probe-Parameter, KEINE
 * Original-Behauptung) — fuer die Kamera-Zonen zaehlt nur, dass der Punkt die
 * Streifen ueberstreicht. Die TREPPEN selbst laufen ueber den echten Port-Code
 * (re15_stair_try_start/tick) mit geladenem PL00-Skelett (FK-Foot-Lock).
 */
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
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;
static int g_cut = 0;
static unsigned g_frame = 0;

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

/* Ein Frame RVD-Scan wie game_step_common.c (Stair-Zweig + Normal-Zweig). */
static void scan_frame(const char *what)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) {
            printf("      F%-4u CUT %d -> %-2d  @(%ld,%ld) y=%ld  [%s]\n",
                   g_frame, g_cut, nc, (long)pl->x, (long)pl->z, (long)pl->y, what);
            g_cut = nc;
        }
    }
    g_frame++;
}

/* Gerader Marsch zum Ziel (Probe-Hilfsmittel, kein Original-Mechanismus). */
static void march_to(int32_t tx, int32_t tz, int rot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)rot;
    for (int i = 0; i < 4000; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (ad <= 40) break;
        /* 40 Einheiten/Frame entlang der Manhattan-Richtung */
        int32_t sx = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int32_t sz = (dz > 0) ? 1 : (dz < 0 ? -1 : 0);
        int32_t stepx = (dx < 0 ? -dx : dx) > 40 ? sx * 40 : dx;
        int32_t stepz = (dz < 0 ? -dz : dz) > 40 ? sz * 40 : dz;
        pl->x += stepx; pl->z += stepz;
        scan_frame("walk");
    }
    pl->x = tx; pl->z = tz;
    scan_frame("walk-arrive");
}

static void descend(const char *label, int32_t sx, int32_t sz, int rot, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    printf("   -- %s: Trigger @(%ld,%ld) band=%d cut=%d\n",
           label, (long)sx, (long)sz, band, g_cut);
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("      KEIN Stair-Start!\n"); return; }
    printf("      Start %s\n", pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "DOWN" : "UP");
    int32_t z0 = pl->z, x0 = pl->x;
    int n = 0;
    while (re15_stair_active() && n < 900) {
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        scan_frame("stair");
        n++;
    }
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld band=%d cut=%d   dz=%+ld dx=%+ld\n",
           n, (long)pl->x, (long)pl->z, (long)pl->y,
           re15_collision_band_from_y(pl->y), g_cut,
           (long)(pl->z - z0), (long)(pl->x - x0));
}

typedef struct { int32_t x, z; int rot, band; const char *label; } step_t;

static void run_room(const char *base, const char *room, int room_id,
                     const step_t *steps, int nsteps, int start_cut)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, room);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse %s\n", room); return; }

    printf("\n############ %s ############\n", room);
    printf("RVD (%d Zonen; Anker = erste Zone je cam_from, wird vom Original NIE getestet):\n",
           g_rdt.zone_count);
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != z->cam_from);
        printf("  [%2d]%s from=%-2d to=%-2d floor=0x%02x x[%d..%d] z[%d..%d]\n",
               i, anchor ? " ANKER" : "      ", (int)z->cam_from, (int)z->cam_to, (int)z->floor,
               (int)(z->cx - z->half_w), (int)(z->cx + z->half_w),
               (int)(z->cz - z->half_h), (int)(z->cz + z->half_h));
    }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = (uint16_t)room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = steps[0].x; pl->z = steps[0].z; pl->y = -(int32_t)steps[0].band * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    printf("\nSTAIR-AOTs:\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (a->active && a->type == RE15_AOT_TYPE_STAIR)
            printf("  slot=%2d band=%d centre=(%ld,%ld) half=(%ld,%ld)\n",
                   i, (int)a->band, (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
    }

    g_cut = start_cut; g_frame = 0;
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    printf("\nKette (Start-Cut=%d):\n", start_cut);
    for (int i = 0; i < nsteps; i++) {
        if (steps[i].label) {
            /* Treppe: erst hinlaufen, dann ausloesen */
            if (i > 0) march_to(steps[i].x, steps[i].z, steps[i].rot);
            descend(steps[i].label, steps[i].x, steps[i].z, steps[i].rot, steps[i].band);
        } else {
            march_to(steps[i].x, steps[i].z, steps[i].rot);
            printf("   -- Wegpunkt (%ld,%ld) cut=%d\n", (long)steps[i].x, (long)steps[i].z, g_cut);
        }
    }
    printf("\nEND-CUT = %d @(%ld,%ld) band=%d\n", g_cut, (long)pl->x, (long)pl->z,
           re15_collision_band_from_y(pl->y));
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT (FK-Foot-Lock inaktiv!)");
    }

    /* ---- ROOM10A0: A(8->6,west) B(6->4,ost) C(4->3,west) D(3->1,ost) ---- */
    static const step_t s10a0[] = {
        { 21450, 23000, 1024, 8, "Treppe A 8->6 (west)"  },
        { 25350, 21000, 3072, 6, "Treppe B 6->4 (ost)"   },
        { 21450, 21400, 1024, 4, "Treppe C 4->3 (west)"  },
        { 25350, 21000, 3072, 3, "Treppe D 3->1 (ost) = LETZTE" },
        { 23200, 25000,    0, 1, NULL },   /* zum Ausgang: x nach West durch 7->8 */
        { 21000, 23400,    0, 1, NULL },   /* weiter: 8->9 Streifen */
    };
    run_room(base, "ROOM10A0", 0x10A0, s10a0, 6, 0);

    /* ---- ROOM1060: ost(8->6) west(6->4) ost(4->2) west(2->0) ---- */
    static const step_t s1060[] = {
        { 25150, 23500, 3072, 8, "Treppe 1 8->6 (ost)"  },
        { 21400, 21500, 3072, 6, "Treppe 2 6->4 (west)" },
        { 25150, 23500, 1024, 4, "Treppe 3 4->2 (ost)"  },
        { 21400, 21500, 3072, 2, "Treppe 4 2->0 (west) = LETZTE" },
        { 23000, 25500,    0, 0, NULL },
    };
    run_room(base, "ROOM1060", 0x1060, s1060, 5, 0);
    return 0;
}
