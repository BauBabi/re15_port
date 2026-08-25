/* probe_item_model_1020.c — MESSSONDE (Nutzer-Report 2026-08-26, FINDING 3):
 * "room 1020 u.a. Wenn ich das Item aufgenommen habe muss das Model fuer das Item verschwinden."
 *
 * Reine Diagnose (kein add_test). Misst am PORT:
 *   (1) Welche Obj_model_set-Props legt ROOM1020 an, und wie ist die Verknuepfung
 *       Item_aot_set.prop (pc[20]) -> Prop-Index?
 *   (2) Ist der Prop nach einer VOLLSTAENDIGEN Aufnahme (Item-Modal bis state 0) noch aktiv?
 *   (3) Ist er nach RAUMWECHSEL (Neu-Laden derselben RDT mit gesetztem taken-Bit) noch aktiv?
 * Gegenprobe in ROOM1000 (drei Items, props 255/0/1).
 *
 * SOLLSEITE (Original, disasm-belegt):
 *   - Sofort-Ausblenden: Message-Script-Routine LAB_80021f6c (Tabelle PTR 0x80072f34[0]),
 *     aufgerufen aus dem Yes-Zweig des "Will you take the ___?"-Prompts (Opcode 0x03 +
 *     0xF9 0x00, FUN_8002877c @0x80028820). Sie setzt rec[0]=0 (@0x80021f90),
 *     pool[payload[6]].flags = 0 (@0x80021fc8) und FUN_8004ef90(0x800b1078, payload[4]).
 *   - Persistenz: Item_aot_set-Installer @0x800406d4-0x80040718 vergiftet pool[prop].flags
 *     = 0x80000000, das nachfolgende Obj_model_set schaltet sich daraufhin selbst ab
 *     (@0x80040978 `lw v0,0(a1)` / @0x8004098c `sw zero,0(a1)`).
 *   - Gezeichnet wird nur bei flags&1 (FUN_8002c18c @0x8002c1fc).
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
#include "re15_item_modal.h"
#include "re15_inventory.h"

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

static void dump_props(const char *tag)
{
    printf("  [%s] prop_count=%u\n", tag, g_scd.prop_count);
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        printf("     prop[%d] obj_id=0x%02X active=%u flags=0x%04X pos=(%ld,%ld,%ld)\n",
               i, g_scd.props[i].obj_id, g_scd.props[i].active,
               g_scd.props[i].flags,
               (long)g_scd.props[i].x, (long)g_scd.props[i].y, (long)g_scd.props[i].z);
}

static void dump_item_aots(const char *tag)
{
    printf("  [%s] AOT-Item-Slots:\n", tag);
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        if (g_aot.slots[s].type != RE15_AOT_TYPE_ITEM) continue;
        printf("     slot%d type=%u active=%u item=0x%02X amt=%u tk=%u\n",
               s, g_aot.slots[s].type, g_aot.slots[s].active,
               g_aot.item_params[s].item_type, g_aot.item_params[s].amount,
               g_aot.item_params[s].taken_bit);
    }
}

static int load_room(const char *file, uint16_t room_id, re15_rdt_t *rdt,
                     uint8_t **raw_out, int32_t px, int32_t pz)
{
    size_t sz = 0;
    uint8_t *raw = read_file(file, &sz);
    if (!raw) { printf("FEHLT: %s\n", file); return 0; }
    if (re15_rdt_parse(raw, sz, rdt) < 0) { printf("PARSE FEHLER %s\n", file); free(raw); return 0; }
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = room_id;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = -1800; pl->z = pz; pl->rot_y = 0;
    scd_room_reenter(rdt, pl->x, pl->z, 0);
    *raw_out = raw;
    return 1;
}

/* Faehrt das Item-Modal komplett durch (Bestaetigen = virtuelles SQUARE 0x4000).
 * ⛔ Der Prop-Index wird NICHT von Hand uebergeben, sondern aus den AOT-Parametern des Slots
 * gezogen — genau so, wie es die beiden echten Aufrufer tun (aot_common.c Aot_on-fire-now und
 * der ACTION-Scan). Damit misst die Sonde die GANZE Kette (op_item_aot_set -> item_params ->
 * Modal -> props[].active) und nicht nur den letzten Handgriff. */
static void run_modal(uint8_t type, uint8_t amount, uint8_t tk, int slot)
{
    uint8_t prop = 0xFF;
    if (slot >= 0 && slot < RE15_AOT_MAX) prop = g_aot.item_params[slot].taken_prop;
    printf("  (Modal-Start: slot=%d prop=%u)\n", slot, prop);
    re15_item_modal_start(type, amount, tk, slot, prop);
    for (int f = 0; f < 600 && re15_item_modal_active(); f++) {
        uint16_t edge = 0;
        if (re15_item_modal_state() == 6 && re15_item_modal_prompt_ready())
            edge = 0x4000;
        re15_item_modal_tick(edge, 0);
    }
    printf("  Modal beendet: state=%u\n", re15_item_modal_state());
}

int main(void)
{
    re15_rdt_t rdt; uint8_t *raw = NULL;

    printf("=== ROOM1020: Item_aot_set slot3 (typ 0x15 x15, tk=2, prop=4)\n");
    printf("             Item_aot_set slot4 (typ 0x15 x15, tk=227, prop=5)\n");
    printf("             Obj_model_set id=0..6 (Reihenfolge: 0,1,2,3, ITEMS, 4,5, AOT, 6)\n\n");
    re15_inv_init();
    for (int z = 0; z < 32; z++) for (int b = 0; b < 8; b++) re15_game_flag_set((uint8_t)z, (uint8_t)(1u<<b), 0);

    if (!load_room(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1020.RDT", 0x1020, &rdt, &raw, -23700, -9998))
        return 77;
    printf("-- A) frisch geladen, nichts genommen --\n");
    dump_props("A"); dump_item_aots("A");
    printf("     flag zone9 bit2=%d bit227=%d\n",
           re15_game_flag_get(9, 2), re15_game_flag_get(9, 227));

    printf("\n-- B) Item in AOT-Slot 3 aufnehmen (Modal komplett, Yes) --\n");
    run_modal(g_aot.item_params[3].item_type, g_aot.item_params[3].amount,
              g_aot.item_params[3].taken_bit, 3);
    dump_props("B"); dump_item_aots("B");
    printf("     flag zone9 bit2=%d  (Soll: 1)\n", re15_game_flag_get(9, 2));
    printf("     >>> ERWARTUNG Original: der zum Item gehoerende Prop (obj_id 4) ist AUS.\n");

    printf("\n-- C) Raumwechsel simulieren: ROOM1020 neu laden (taken-Bit gesetzt) --\n");
    free(raw); raw = NULL;
    if (!load_room(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1020.RDT", 0x1020, &rdt, &raw, -23700, -9998))
        return 77;
    printf("     flag zone9 bit2 NACH Neuladen = %d  (0 = Sonden-Artefakt: scd_vm_init "
           "loescht die Flag-Baenke)\n", re15_game_flag_get(9, 2));
    if (!re15_game_flag_get(9, 2)) {
        printf("     -> setze taken-Bit von Hand und lade nochmal, um den INSTALLER zu messen\n");
        free(raw); raw = NULL;
        size_t sz2 = 0; uint8_t *raw2 = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1020.RDT", &sz2);
        re15_rdt_parse(raw2, sz2, &rdt);
        re15_actor_init(); scd_vm_init();
        re15_game_flag_set(9, 2, 1);            /* taken-Bit VOR dem Install setzen */
        g_current_room_id = 0x1020; g_room_change.pending = 0;
        re15_actor_t *pl2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl2->active = 1; pl2->type = 0; pl2->hp = 100;
        pl2->x = -23700; pl2->y = -1800; pl2->z = -9998;
        scd_room_reenter(&rdt, pl2->x, pl2->z, 0);
        raw = raw2;
        printf("     flag zone9 bit2 = %d\n", re15_game_flag_get(9, 2));
    }
    dump_props("C"); dump_item_aots("C");
    printf("     >>> ERWARTUNG Original: AOT-Slot 3 tot UND Prop obj_id 4 AUS.\n");
    free(raw); raw = NULL;

    printf("\n=== ROOM1000 (Gegenprobe: 3 Items, props 255 / 0 / 1)\n");
    re15_inv_init();
    for (int z = 0; z < 32; z++) for (int b = 0; b < 8; b++) re15_game_flag_set((uint8_t)z, (uint8_t)(1u<<b), 0);
    if (!load_room(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1000.RDT", 0x1000, &rdt, &raw, 0, 0))
        return 77;
    dump_props("A"); dump_item_aots("A");
    printf("\n-- Item in Slot 5 aufnehmen (typ 0x25, tk=167, prop=1) --\n");
    run_modal(g_aot.item_params[5].item_type, g_aot.item_params[5].amount,
              g_aot.item_params[5].taken_bit, 5);
    dump_props("B"); dump_item_aots("B");
    printf("     flag zone9 bit167=%d\n", re15_game_flag_get(9, 167));
    free(raw); raw = NULL;
    if (!load_room(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1000.RDT", 0x1000, &rdt, &raw, 0, 0))
        return 77;
    printf("-- nach Neuladen --\n");
    dump_props("C"); dump_item_aots("C");
    free(raw);
    return 0;
}
