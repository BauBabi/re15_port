/* probe_1190_props.c — MESSUNG: wie viele Prop-Objekte legt ROOM1190 wirklich an?
 *
 * NUTZER-BEFUND (2026-08-26): "Im Room 1190 ist statt der Weste von Leon ein gelbes
 * viereck zu sehen."
 *
 * Diese Sonde MISST, statt zu modellieren:
 *   M1  nOmodel aus dem RDT-Kopf (Byte RDT+0x02) gegen rdt.prop_count nach dem Parser.
 *   M2  wie viele Obj_model_set-Records der laufende SCD-Lauf wirklich ausfuehrt,
 *       und welche obj_ids dabei vorkommen (der statische Zensus zaehlt 276 Vorkommen
 *       ueber alle Sub-Skripte — davon liegen die meisten in toten Zweigen).
 *   M3  ob Installations-Reihenfolge == obj_id ist (die Annahme in scd_vm.c:2934).
 * KEIN add_test — reine Messsonde.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"

extern void scd_vm_init(void);
extern void scd_vm_tick(void);
extern int  scd_thread_start(int slot, const uint8_t *pc);
extern void scd_register_room_events(const re15_rdt_t *rdt);
extern void scd_vm_set_room_init(int on);

static re15_rdt_t s_rdt;

int main(void)
{
    char path[512];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1190.RDT", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(path, "rb");
    if (!f) { printf("FAIL: %s nicht gefunden\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { printf("FAIL: read\n"); return 1; }
    fclose(f);

    printf("== M1: Kopf gegen Parser ==\n");
    printf("  RDT+0x02 nOmodel        = %u\n", buf[2]);
    if (re15_rdt_parse(buf, (size_t)sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    printf("  rdt.prop_count (Parser) = %d   (RE15_RDT_MAX_PROPS = %d)\n",
           s_rdt.prop_count, RE15_RDT_MAX_PROPS);
    for (int i = 0; i < s_rdt.prop_count; i++)
        printf("    prop[%2d] MD1=%7d B  TIM=%7d B\n",
               i, s_rdt.prop_md1_size[i], s_rdt.prop_tim_size[i]);

    printf("== M2/M3: Laufzeit-Installationen ==\n");
    scd_vm_init();
    scd_register_room_events(&s_rdt);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    scd_vm_set_room_init(1);
    for (int t = 0; t < 4; t++) scd_vm_tick();
    scd_vm_set_room_init(0);
    for (int t = 0; t < 300; t++) scd_vm_tick();

    printf("  g_scd.prop_count        = %u\n", (unsigned)g_scd.prop_count);
    int order_is_objid = 1;
    for (int i = 0; i < (int)g_scd.prop_count; i++) {
        printf("    pool[%2d] obj_id=%2u type=%u aktiv=%u flags=0x%04X\n",
               i, (unsigned)g_scd.props[i].obj_id, (unsigned)g_scd.props[i].obj_type,
               (unsigned)g_scd.props[i].active, (unsigned)g_scd.props[i].flags);
        if (g_scd.props[i].obj_id != (uint8_t)i) order_is_objid = 0;
    }
    printf("  Reihenfolge == obj_id?  %s\n", order_is_objid ? "JA" : "NEIN");
    free(buf);
    return 0;
}
