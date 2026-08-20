/* probe_1090_objects.c — MESSSONDE ROOM1090 (Nutzer-Report v0.3.5, 4 Befunde).
 *
 * Reine Diagnose (kein add_test): laedt STAGE1/ROOM1090.RDT, faehrt main00+sub00 hoch
 * und protokolliert, was der Port aus den Raumdaten macht:
 *   (D) "Kiste zu tief"   -> Obj_model_set-Props: id/typ/band/Position/Box
 *   (B) "Kisten schieben" -> gibt es im Port ueberhaupt einen Schiebe-Zaehler?
 *   (C) "Cutscene startet nicht" -> work_vars[0x0A] (= aktiver Kamera-Cut im Original)
 *   (A) "Feuer fehlt"     -> Effekt-Opcodes im SCD / ESP-ID-Liste der RDT
 *
 * SOLLSEITE (Roh-Scan der RDT, Datei-Offsets):
 *   main00 @0x211A: Door_aot_set x2, Aot_set slot3 sce3,
 *     @0x2170 2D 00 04 00 01 00 00 01 00 00 38E0 F8F8 9112 ... = obj0 Typ4 Band1 (-8136,-1800,4753)
 *     @0x2192 2D 01 04 00 05 00 00 01 00 00 B8E8 D8DC 86C3 ... = obj1 Typ4 Band5 (-5960,-9000,-15482)
 *   sub00 @0x21F2: 2D 03 00 00 02 ... obj3 Typ0; @0x22AA: 2D 02 00 00 02 ... obj2 Typ0
 *   sub01 @0x23F4: If Ck(3,0x80)==0 { If Cmp(work_vars[0x0A]==13) { GOSUB sub02; Set(3,0x80)=1 } }
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

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

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT";
    size_t sz = 0;
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("FEHLT: %s\n", path); return 77; }
    printf("ROOM1090.RDT  %u Bytes\n", (unsigned)sz);
    printf("  Header: nSprite=%u nCut=%u nOmodel=%u nItem=%u nDoor=%u\n",
           raw[0], raw[1], raw[2], raw[3], raw[4]);
    /* (A) ESP-ID-Liste der RDT: Adresstabellen-Eintrag @0x4C, erste 8 Bytes des Blocks. */
    uint32_t eff = (uint32_t)raw[0x4C] | ((uint32_t)raw[0x4D] << 8)
                 | ((uint32_t)raw[0x4E] << 16) | ((uint32_t)raw[0x4F] << 24);
    printf("  ESP-Block @0x%X ids:", eff);
    if (eff && eff + 8 <= sz) for (int i = 0; i < 8; i++) printf(" %02X", raw[eff + i]);
    printf("\n");

    re15_rdt_t rdt;
    int prc = re15_rdt_parse(raw, sz, &rdt);
    if (prc < 0) { printf("PARSE FEHLGESCHLAGEN rc=%d\n", prc); return 1; }
    printf("  rdt.prop_count=%d  sub_scd_count=%d\n", rdt.prop_count, rdt.sub_scd_count);

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;   /* re15_room_spawns.h */

    scd_room_reenter(&rdt, pl->x, pl->z, /*entry_scenario=*/0);

    printf("\n-- (D)/(B) Obj_model_set-Props nach main00+sub00 --\n");
    for (int i = 0; i < (int)g_scd.prop_count; i++) {
        printf("  prop[%d] aktiv=%u id=0x%02X typ=%u band=%u pos=(%ld,%ld,%ld) rot=(%d,%d,%d) "
               "box c=(%d,%d,%d) h=(%d,%d,%d)\n",
               i, g_scd.props[i].active, g_scd.props[i].obj_id, g_scd.props[i].obj_type,
               g_scd.props[i].band,
               (long)g_scd.props[i].x, (long)g_scd.props[i].y, (long)g_scd.props[i].z,
               g_scd.props[i].rot_x, g_scd.props[i].rot_y, g_scd.props[i].rot_z,
               g_scd.props[i].box_cx, g_scd.props[i].box_cy, g_scd.props[i].box_cz,
               g_scd.props[i].box_hx, g_scd.props[i].box_hy, g_scd.props[i].box_hz);
    }

    printf("\n-- (C) work_vars (0x0A = aktiver Kamera-Cut im Original) --\n");
    for (int i = 8; i <= 0x0C; i++)
        printf("  work_vars[0x%02X] = %d\n", i, (int)g_scd.work_vars[i]);
    printf("  g_scd.cam_id=%u cam_id_prev=%u cam_change_pending=%u cut_auto=%u\n",
           g_scd.cam_id, g_scd.cam_id_prev, g_scd.cam_change_pending, g_scd.cut_auto_enabled);

    printf("\n-- AOT-Slots --\n");
    for (int s = 0; s < 8; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->type) continue;
        printf("  slot%d type=%u band=0x%02X rect c=(%ld,%ld) half=(%ld,%ld)\n",
               s, a->type, a->band, (long)a->x, (long)a->z,
               (long)a->half_w, (long)a->half_h);
    }

    /* Kamera-Cut 13 simulieren: im Original setzt der Per-Frame-Kamera-Apply
     * (FUN_80021bc0 @0x80021bfc) work_vars[0x0A] auf den aktiven Cut. */
    printf("\n-- (C) Gegenprobe: work_vars[0x0A] = 13 setzen, dann sub01 ticken --\n");
    g_scd.work_vars[10] = 13;
    for (int f = 0; f < 4; f++) {
        scd_vm_tick();
        printf("   F%d: flag(3,0x80)=%d message_active=%u message_id=%u\n",
               f, re15_game_flag_get(3, 0x80), g_scd.message_active, g_scd.message_id);
    }

    free(raw);
    return 0;
}
