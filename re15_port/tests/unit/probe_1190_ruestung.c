/* probe_1190_ruestung.c — MESSUNG: laeuft die R.P.D.-Ruestungs-Kette im Port?
 *
 * BELEGTE ORIGINAL-KETTE (alles selbst gemessen, s. Commit-Text):
 *   1. Prop 16 = die Weste auf einem Staender, Obj_model_set @Datei 0x002D4C (sub14),
 *      pos (20480,-1500,-21168).
 *   2. Aot_set @Datei 0x002D90 (sub14): slot 16, sce 3, Rechteck (19700,-22000)+(1000,1500)
 *      — enthaelt die Weste. Nutzlast pc[14..19] = ff 00 18 0f 00 00 -> p1>>8 = sub 15.
 *   3. sub15: Flag(3,0x75) -> Meldung 0x04 "There is one R.P.D. armor that should fit you.
 *      Will you equip it?" bzw. 0x05 "Will you unequip the armor?"; danach Flag umschalten,
 *      work_vars[0x10] = 1 bzw. 0, dann Opcode 0x47 mit Id 15.
 *   4. AOT-Slot 15 = Door_aot_set @Datei 0x002D70, Rechteck (0,0,0,0) = reiner Skript-
 *      Ausloeser, next_pos (19100,0,-21250), Raum 0x19 = ROOM1190 selbst, Cut 13.
 *   5. Raumlader FUN_800396fc @0x80039760-8c: untere Nibble von DAT_800aca5c = work_vars[0x10];
 *      weicht sie ab -> FUN_800314b0 laedt das Spielermodell neu. Datei-Id-Tabelle
 *      @0x80073f70 = 0x3C..0x4B fuer Index 0..15, also PL0<index>.PLD.
 *      Modell 1 = PL01 = Leon mit roter "POLICE"-Panzerweste (Textur selbst dekodiert).
 * KEIN add_test. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"

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
    if (!f) { printf("FAIL: %s\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) return 1;
    fclose(f);
    if (re15_rdt_parse(b, (size_t)sz, &s_rdt) != 0) { printf("FAIL: Parse\n"); return 1; }

    scd_vm_init();
    scd_register_room_events(&s_rdt);
    if (s_rdt.main_scd)    scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[14]) scd_thread_start(1, s_rdt.sub_scd[14]);
    scd_vm_set_room_init(1);
    for (int t = 0; t < 4; t++) scd_vm_tick();
    scd_vm_set_room_init(0);
    for (int t = 0; t < 60; t++) scd_vm_tick();

    printf("== nach sub14 ==\n");
    printf("  Props installiert: %u\n", (unsigned)g_scd.prop_count);
    int seen16 = 0;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].obj_id == 16) { seen16 = 1;
            printf("  obj16 (Weste): pos=(%d,%d,%d) aktiv=%u flags=0x%04X\n",
                   g_scd.props[i].x, g_scd.props[i].y, g_scd.props[i].z,
                   (unsigned)g_scd.props[i].active, (unsigned)g_scd.props[i].flags); }
    if (!seen16) printf("  obj16 (Weste): NICHT installiert\n");

    for (int s = 14; s <= 16; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        printf("  AOT %2d: typ=%d aktiv=%d event_id=%u  x=%d z=%d hw=%d hh=%d\n",
               s, (int)a->type, (int)a->active, (unsigned)a->event_id,
               a->x, a->z, a->half_w, a->half_h);
    }

    printf("== AOT 16 ausloesen (= Weste untersuchen) ==\n");
    printf("  work_vars[0x10] vorher = %d\n", (int)g_scd.work_vars[0x10]);
    re15_aot_fire_slot(16);
    for (int t = 0; t < 90; t++) scd_vm_tick();
    printf("  work_vars[0x10] nachher = %d   (1 = Ruestung an)\n",
           (int)g_scd.work_vars[0x10]);
    free(b);
    return 0;
}
