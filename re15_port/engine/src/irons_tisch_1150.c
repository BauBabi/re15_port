/*
 * RE1.5 Rebuilt — IRONS DIARY und MEMORY CARD auf dem Schreibtisch in Irons' Buero.
 *
 * Herleitung, Belege und alle Konstanten: include/re15_irons_tisch.h.
 * Modelle + Texturen: tools/irons_tisch_engine_export.py -> gen/irons_tisch_props.inc
 * (eingebackene MD1/TIM-Bytes, kein Asset-Patch).
 */
#include "re15_irons_tisch.h"

#include <stddef.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_pri.h"

#include "gen/irons_tisch_props.inc"

static int ist_irons_buero(uint16_t room_id)
{
    return room_id == 0x1150 || room_id == 0x1151;   /* John- und Elza-Variante */
}

/* Pool-Index eines Props ueber seine obj_id (dieselbe Suche wie sicherung_1150.c). */
static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

typedef struct {
    uint8_t  obj_id, aot_slot, taken_bit, item, menge;
    int32_t  x, y, z;
    int16_t  rot_y;
    int32_t  rect_x, rect_z, rect_w, rect_d;
} gegenstand_t;

static const gegenstand_t k_gegenstaende[2] = {
    { RE15_IRONS_DIARY_OBJ_ID, RE15_IRONS_DIARY_AOT_SLOT, RE15_IRONS_DIARY_TAKEN_BIT,
      RE15_IRONS_DIARY_ITEM, RE15_IRONS_DIARY_MENGE,
      RE15_IRONS_DIARY_X, RE15_IRONS_DIARY_Y, RE15_IRONS_DIARY_Z, RE15_IRONS_DIARY_ROT_Y,
      RE15_IRONS_DIARY_RECT_X, RE15_IRONS_DIARY_RECT_Z,
      RE15_IRONS_DIARY_RECT_W, RE15_IRONS_DIARY_RECT_D },
    { RE15_IRONS_KARTE_OBJ_ID, RE15_IRONS_KARTE_AOT_SLOT, RE15_IRONS_KARTE_TAKEN_BIT,
      RE15_IRONS_KARTE_ITEM, RE15_IRONS_KARTE_MENGE,
      RE15_IRONS_KARTE_X, RE15_IRONS_KARTE_Y, RE15_IRONS_KARTE_Z, RE15_IRONS_KARTE_ROT_Y,
      RE15_IRONS_KARTE_RECT_X, RE15_IRONS_KARTE_RECT_Z,
      RE15_IRONS_KARTE_RECT_W, RE15_IRONS_KARTE_RECT_D },
};

static void anlegen(const gegenstand_t *g)
{
    /* Schon genommen -> weder Prop noch Zone (Item_aot_set @0x800406dc-718 legt den Satz
     * still und blendet das Modell aus; hier entsteht beides gar nicht erst). */
    if (re15_game_flag_get(9, g->taken_bit)) return;

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914) mit den Werten aus dem Kopf. */
    if (slot_von_obj_id(g->obj_id) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = g->obj_id;
        g_scd.props[i].obj_type   = 0;                     /* pc[2] -> pool+8 @0x8004095c   */
        g_scd.props[i].band       = RE15_IRONS_PROP_BAND;  /* pc[4] -> pool+130 @0x80040974 */
        g_scd.props[i].parent_obj = -1;                    /* pc[5] = 0x00 = Weltraum        */
        g_scd.props[i].x = g->x;
        g_scd.props[i].y = g->y;
        g_scd.props[i].z = g->z;
        g_scd.props[i].rot_x = 0;
        g_scd.props[i].rot_y = g->rot_y;
        g_scd.props[i].rot_z = 0;
        g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
        g_scd.props[i].vel_ry = 0;
        g_scd.props[i].flags  = RE15_IRONS_PROP_FLAGS;     /* 0x000A | 1 @0x80040998         */
        g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
        g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
        g_scd.props[i].member_0b = 0;
    }

    /* ZONE — wie op_item_aot_set (scd_vm.c): Rechteck als Mitte + halbe Ausdehnung,
     * danach sat (Item_aot_set +3) und floor (+4). */
    if (g->aot_slot < RE15_AOT_MAX && !g_aot.slots[g->aot_slot].active) {
        re15_aot_set_item_tk_prop((int)g->aot_slot,
                                  g->rect_x + g->rect_w / 2, g->rect_z + g->rect_d / 2,
                                  g->rect_w / 2, g->rect_d / 2,
                                  g->item, g->menge, g->taken_bit, g->obj_id);
        g_aot.slots[g->aot_slot].sce_flags = RE15_IRONS_AOT_SAT;
        g_aot.slots[g->aot_slot].band      = RE15_IRONS_AOT_FLOOR;
    }
}

void re15_irons_tisch_install(uint16_t room_id)
{
    if (!ist_irons_buero(room_id)) return;
    for (int k = 0; k < 2; k++) anlegen(&k_gegenstaende[k]);
}

int re15_irons_tisch_sort_max(uint16_t room_id, int cut, int obj_id)
{
    if (!ist_irons_buero(room_id)) return -1;
    if (cut != RE15_IRONS_KLEMME_CUT) return -1;
    if (obj_id != RE15_IRONS_DIARY_OBJ_ID && obj_id != RE15_IRONS_KARTE_OBJ_ID) return -1;
    return (int)re15_pri_mask_camera_z(RE15_IRONS_KLEMME_TIEFE) - 1;
}

const uint8_t *re15_irons_tisch_md1_bytes(int obj_id, int *out_size)
{
    if (obj_id == RE15_IRONS_DIARY_OBJ_ID) {
        if (out_size) *out_size = (int)sizeof(re15_irons_diary_md1);
        return re15_irons_diary_md1;
    }
    if (obj_id == RE15_IRONS_KARTE_OBJ_ID) {
        if (out_size) *out_size = (int)sizeof(re15_irons_karte_md1);
        return re15_irons_karte_md1;
    }
    if (out_size) *out_size = 0;
    return NULL;
}

const uint8_t *re15_irons_tisch_tim_bytes(int obj_id, int *out_size)
{
    if (obj_id == RE15_IRONS_DIARY_OBJ_ID) {
        if (out_size) *out_size = (int)sizeof(re15_irons_diary_tim);
        return re15_irons_diary_tim;
    }
    if (obj_id == RE15_IRONS_KARTE_OBJ_ID) {
        if (out_size) *out_size = (int)sizeof(re15_irons_karte_tim);
        return re15_irons_karte_tim;
    }
    if (out_size) *out_size = 0;
    return NULL;
}
