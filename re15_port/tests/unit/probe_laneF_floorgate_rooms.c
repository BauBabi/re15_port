/* probe_laneF_floorgate_rooms.c — LANE F DIAGNOSE (kein ctest).
 *
 * Frage: In welchen Raeumen ist das RVD-FLOOR-Gate (@0x80014274 `beq v1,0xff` /
 * @0x80014280 `lbu 0x800acad6` / @0x80014288 `bne v1,v0`) tatsaechlich SCHARF —
 * und haben genau diese Raeume Treppen (sce 12/13)?  Denn waehrend einer Treppe
 * schreibt das Original das Band auf die KONSTANTE 7:
 *     LAB_80038c60 (Abstieg) @0x80038cd4 `ori v1,zero,0x7` / @0x80038ce4 `sb v1,player+0x82`
 *     LAB_800388ac (Aufstieg)@0x80038a18 `ori v0,zero,0x7` / @0x80038a28 `sb v0,player+0x82`
 * und erst im FINALIZE @0x80038edc (bzw. @0x80038c40) wieder auf (-Y)/1800.
 * Der Port laesst das Band waehrend der Treppe auf dem QUELL-Band stehen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_aot.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;

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

static void one_room(const char *base, const char *stage, const char *room, int room_id)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", base, stage, room);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse %s\n", room); return; }

    int gated = 0, gated7 = 0;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != g_rdt.zones[i].cam_from);
        if (anchor) continue;
        if (g_rdt.zones[i].floor != 0xFF) { gated++; if (g_rdt.zones[i].floor == 7) gated7++; }
    }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = (uint16_t)room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->z = 0; pl->y = 0;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    int nstair = 0;
    char sbuf[512]; sbuf[0] = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
        const re15_aot_stair_params_t *sp = &g_aot.stair_params[i];
        char t[64];
        snprintf(t, sizeof t, " [slot%d band%d axis%d side%d cnt%d]",
                 i, (int)a->band, sp->axis, sp->side, sp->count & 7);
        if (strlen(sbuf) + strlen(t) < sizeof sbuf - 1) strcat(sbuf, t);
        nstair++;
    }
    printf("%-9s zonen=%3d  floor-gated(nicht-Anker)=%2d (davon floor==7: %d)  TREPPEN=%d%s\n",
           room, g_rdt.zone_count, gated, gated7, nstair, sbuf);
    free(data);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    printf("=== LANE F: RVD-Floor-Gate vs. Treppen ===\n");
    printf("(Original: Floor-Byte rec[1] wird @0x80014274 gegen 0xFF und sonst\n"
           " @0x80014280 gegen player+0x82 (DAT_800acad6) geprueft.)\n\n");
    one_room(base, "STAGE1", "ROOM1060", 0x1060);
    one_room(base, "STAGE1", "ROOM10A0", 0x10A0);
    one_room(base, "STAGE1", "ROOM11B0", 0x11B0);
    one_room(base, "STAGE1", "ROOM11C0", 0x11C0);
    one_room(base, "STAGE1", "ROOM11E0", 0x11E0);
    one_room(base, "STAGE2", "ROOM2000", 0x2000);
    one_room(base, "STAGE2", "ROOM20B0", 0x20B0);
    one_room(base, "STAGE3", "ROOM3080", 0x3080);
    one_room(base, "STAGE5", "ROOM50A0", 0x50A0);
    one_room(base, "STAGE5", "ROOM5100", 0x5100);
    one_room(base, "STAGE5", "ROOM5140", 0x5140);
    one_room(base, "STAGE6", "ROOM6030", 0x6030);
    return 0;
}
