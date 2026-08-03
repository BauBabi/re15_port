/* probe_stair_10a0.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-03:
 * ROOM10A0 (Treppenhaus) — vordere Treppe absteigen OK, danach die HINTERE
 * Treppe (fuehrt nach UNTEN) ausloesen -> Leon steigt HOCH.
 *
 * ORIGINAL (RE'ed, siehe analysis/stair_10a0.md):
 *   sce-13-Handler LAB_800435cc: delta = player_z - rec[+6]; half = rec[+0xA]>>1;
 *   low_half = delta < half (@0x800435dc-f0);  side = rec[+0xC] (u16, pc[14..15]);
 *   aca5a = ((low_half && side==0) || (!low_half && side==1)) ? 1 : 2
 *   1 = ASCEND (Mode bleibt 0xb), 2 = DESCEND (Mode-0xb Phase1 -> aca59=0xc
 *   @0x800389e0-f4). Stufenzahl = rec[+0xE] = pc[16] (& 7, -1) — KEIN Paar-Zonen-Ziel.
 *
 * PORT-HYPOTHESE: stair_common.c paart Zonen ueber event_id — aber event_id
 * traegt data0/side (nur 0|1). ROOM10A0 hat 4 Treppen (8 Zonen, side 0,0,1,1,0,0,1,1):
 * Treppe C (slots 7/8, side 0) paart faelschlich mit Treppe A slot 3 (band 8),
 * Treppe D (slots 9/10, side 1) mit Treppe B slot 5 (band 6) -> Richtung/Ziel falsch. */
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

/* Original-Referenz: sce-13-Entscheidung (LAB_800435cc) fuer eine gegebene
 * Zone (RDT-Bytes) + Spieler-Z. Liefert 1=ASCEND, 2=DESCEND. */
static int original_dir_sce13(int32_t player_z, int16_t rect_z, uint16_t rect_d, uint16_t side)
{
    int low_half = (player_z - rect_z) < (int32_t)(rect_d >> 1);  /* @0x800435dc-f0 */
    if (low_half)  return (side == 0) ? 1 : 2;                    /* @0x800435fc-04 */
    return (side == 1) ? 1 : 2;                                   /* @0x80043624-2c */
}

typedef struct {
    const char *name;
    int   band;        /* Spieler-Band vor dem Ausloesen */
    int32_t x, z;      /* Spieler-Position (im Zonen-Rect) */
    int   rot_y;       /* Blick: 1024 = -z, 3072 = +z */
    /* Zone (RDT-Bytes) fuer die Original-Referenz: */
    int16_t rect_z; uint16_t rect_d; uint16_t side;
    int   expect_target;   /* Original-Zielband */
} scenario_t;

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

    int odir = original_dir_sce13(pl->z, s->rect_z, s->rect_d, s->side);
    printf("--- %s: band=%d pos=(%ld,%ld,%ld) rot=%d\n", s->name, s->band,
           (long)pl->x, (long)pl->y, (long)pl->z, s->rot_y);
    printf("    ORIGINAL LAB_800435cc: delta=%ld half=%d side=%u -> aca5a=%d (%s), Ziel=%d\n",
           (long)(pl->z - s->rect_z), s->rect_d >> 1, s->side, odir,
           odir == 1 ? "ASCEND" : "DESCEND", s->expect_target);

    int started = re15_stair_try_start(rdt, 1);
    if (!started) { printf("    PORT: KEIN Start (try_start=0)\n\n"); return; }
    int dir_down = (pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN);
    printf("    PORT: START motion=%d (%s)\n", pl->motion, dir_down ? "DOWN" : "UP");

    for (int f = 0; f < 900 && re15_stair_active(); f++)
        re15_stair_tick(rdt, NULL, NULL);
    int end_band = re15_collision_band_from_y(pl->y);
    int match = (end_band == s->expect_target);
    printf("    PORT: Ende y=%ld -> band=%d  [%s Original-Ziel %d]\n\n",
           (long)pl->y, end_band, match ? "==" : "!=", s->expect_target);
}

int main(void)
{
    putenv("RE15_STAIR_DBG=1");
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
    pl->active = 1; pl->type = 0; pl->x = 21450; pl->y = -8 * 0x708; pl->z = 23000;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    printf("== installierte STAIR-AOTs ==\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (a->active && a->type == RE15_AOT_TYPE_STAIR)
            printf("  slot=%2d band=%d event(side)=%d centre=(%ld,%ld) half=(%ld,%ld)\n",
                   i, (int)a->band, (int)a->event_id,
                   (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
    }
    printf("\n");

    /* RDT-Bytes (ROOM10A0.RDT, main_scd @0xdb0):
     * A slots3/4 @0xe40/0xe54: rect x=20100 z=19500 w=2700 d=4300 side=0 cnt=2 (8<->6)
     * B slots5/6 @0xe68/0xe7c: rect x=24000 z=20100 w=2700 d=4500 side=1 cnt=2 (6<->4)
     * C slots7/8 @0xe90/0xea4: rect x=20100 z=19500 w=2700 d=2300 side=0 cnt=1 (4<->3)
     * D slots9/10@0xeb8/0xecc: rect x=24000 z=20100 w=2700 d=4500 side=1 cnt=2 (3<->1) */
    scenario_t sc[] = {
        /* Nutzer-Flow: erst vordere Treppe(n) runter — funktioniert laut Report */
        { "A DESCEND 8->6 (oben, hohe z-Haelfte)", 8, 21450, 23000, 1024, 19500, 4300, 0, 6 },
        { "B DESCEND 6->4 (oben, niedrige z-Haelfte)", 6, 25350, 21000, 3072, 20100, 4500, 1, 4 },
        /* die 'hintere' Treppe, fuehrt nach UNTEN: */
        { "C DESCEND 4->3 (oben, hohe z-Haelfte)", 4, 21450, 21400, 1024, 19500, 2300, 0, 3 },
        { "D DESCEND 3->1 (oben, niedrige z-Haelfte)", 3, 25350, 21000, 3072, 20100, 4500, 1, 1 },
        /* Gegenrichtungen: */
        { "C ASCEND 3->4 (unten, niedrige z-Haelfte)", 3, 21450, 20000, 3072, 19500, 2300, 0, 4 },
        { "D ASCEND 1->3 (unten, hohe z-Haelfte)", 1, 25350, 23900, 1024, 20100, 4500, 1, 3 },
        { "A ASCEND 6->8 (unten, niedrige z-Haelfte)", 6, 21450, 20200, 3072, 19500, 4300, 0, 8 },
        { "B ASCEND 4->6 (unten, hohe z-Haelfte)", 4, 25350, 23900, 1024, 20100, 4500, 1, 6 },
    };
    for (size_t i = 0; i < sizeof sc / sizeof sc[0]; i++)
        run_scenario(&rdt, &sc[i]);

    free(data);
    return 0;
}
