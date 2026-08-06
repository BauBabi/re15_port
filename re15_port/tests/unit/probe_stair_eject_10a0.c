/* probe_stair_eject_10a0.c — MESSUNG (kein Fix): Nutzer-Report 2026-08-06
 *   "Im Treppenhaus schwenkt nach dem Runterlaufen der letzten Treppe die
 *    Kamera teilweise nicht richtig um."
 *
 * TICK-GENAU: fuer die LETZTE Treppe (D, band 3 -> 1, ROOM10A0 main00 @0xdb0
 * Aot_set slot9 sce=13 band=3 rect(24000,20100,2700,4500) side=1 count=2) wird
 * JEDER Tick des Abstiegs mit Position, Schrittweite und aktivem Cut geloggt,
 * inklusive des FINALIZE-Ticks.
 *
 * Warum die Schrittweite zaehlt: der RVD-Scan des Originals (FUN_80014230
 * @0x80014230, gerufen @0x8001ccec) ist ein LEVEL-Test — er sieht pro Frame
 * genau EINEN Punkt. Das Original bewegt den Spieler im Finalize NICHT
 * (LAB_80038e50 setzt Clip/Kontrolle/Band, keine Position); der Port dagegen
 * fuehrt in stair_common.c:150-185 einen PORT-EIGENEN "FORWARD eject" aus,
 * der bis zu 48*100 = 4800 Units in EINEM Tick zurueklegt. Jede
 * Transitions-Zone, die dabei uebersprungen wird, feuert nie.
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

/* Welche RVD-Transitionen wuerde ein Punkt treffen (unabhaengig vom cam_from)? */
static void zones_at(const re15_rdt_t *rdt, int32_t x, int32_t z, char *out, size_t n)
{
    out[0] = 0;
    for (int i = 0; i < rdt->zone_count; i++) {
        if (i == 0 || rdt->zones[i - 1].cam_from != rdt->zones[i].cam_from) continue; /* Anker */
        if (re15_aot_point_in_quad(x, z, rdt->zones[i].xs, rdt->zones[i].zs)) {
            char b[24];
            snprintf(b, sizeof b, "[%d]%u>%u ", i, rdt->zones[i].cam_from, rdt->zones[i].cam_to);
            if (strlen(out) + strlen(b) < n) strcat(out, b);
        }
    }
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10A0.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    int start_cut = (argc > 1) ? atoi(argv[1]) : 6;
    int32_t sx = (argc > 2) ? (int32_t)atoi(argv[2]) : 25350;
    int32_t sz = (argc > 3) ? (int32_t)atoi(argv[3]) : 21000;

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10A0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = sx; pl->z = sz; pl->y = -3 * 0x708; pl->rot_y = 3072;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    re15_collision_set_band(3);
    s_cut = start_cut;
    g_scd.cam_change_pending = 0;

    char zb[256];
    zones_at(&rdt, pl->x, pl->z, zb, sizeof zb);
    printf("Treppe D (LETZTE, band 3->1): Start pos=(%ld,%ld) cut=%d  zonen: %s\n",
           (long)pl->x, (long)pl->z, s_cut, zb);
    if (!re15_stair_try_start(&rdt, 1)) { printf("KEIN Start\n"); return 1; }

    int32_t px = pl->x, pz = pl->z;
    int32_t maxstep = 0; int maxtick = -1;
    for (int f = 0; f < 900 && re15_stair_active(); f++) {
        re15_stair_tick(&rdt, NULL, NULL);
        int32_t dx = pl->x - px, dz = pl->z - pz;
        int32_t st = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (st > maxstep) { maxstep = st; maxtick = f; }
        int prev = s_cut;
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)s_cut);
        if (g_scd.cam_change_pending) { s_cut = (int)g_scd.cam_id; g_scd.cam_change_pending = 0; }
        zones_at(&rdt, pl->x, pl->z, zb, sizeof zb);
        printf("  F%-3d step=%-5ld pos=(%ld,%ld) y=%ld band=%d cut %d->%d  zonen: %s\n",
               f, (long)st, (long)pl->x, (long)pl->z, (long)pl->y,
               re15_collision_debug_band(), prev, s_cut, zb);
        px = pl->x; pz = pl->z;
    }
    printf("\nGROESSTER EINZELSCHRITT: %ld Units in Tick F%d "
           "(Original bewegt im Finalize LAB_80038e50 gar nicht)\n", (long)maxstep, maxtick);
    printf("ENDE: pos=(%ld,%ld,%ld) band=%d CUT=%d "
           "(Original-Tuer aus ROOM11E0 rahmt Band 1 mit CUT 8)\n",
           (long)pl->x, (long)pl->y, (long)pl->z,
           re15_collision_band_from_y(pl->y), s_cut);
    free(data);
    return 0;
}
