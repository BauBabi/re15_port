/*
 * RE1.5 Rebuilt — Memory Card im Regal ROOM1010/1011 und Shotgun Shells auf dem Aussenluefter
 * ROOM1090/1091 (Runde 35 Spur F, Punkte 4/5). Herleitung, Belege und alle Konstanten:
 * include/re15_inhalt_r35.h und analysis/befunde_runde35/F_inhalt.md. Vorbild: dokumente_r34.c.
 */
#include "re15_inhalt_r35.h"

#include <stddef.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_irons_tisch.h"      /* Karten-Modell (gen/irons_tisch_props.inc), RE15_IRONS_KARTE_MENGE */

#include "gen/r35_schrot_prop.inc"

typedef struct {
    uint16_t raum;                 /* Leon-Variante; Elza = raum | 1 */
    uint8_t  item, menge, bit, slot, obj, floor;
    int32_t  x, y, z;
    int16_t  rot_x, rot_y, rot_z;   /* Pool +0x68/+0x6A/+0x6C (scd_vm.c Member 3..5) */
    int32_t  rect_x, rect_z, rect_w, rect_d;
} gegenstand_t;

static const gegenstand_t k_gegenstaende[] = {
    { 0x1010, RE15_R35I_KARTE_ITEM, RE15_IRONS_KARTE_MENGE, RE15_R35I_KARTE_BIT,
      RE15_R35I_KARTE_SLOT, RE15_R35I_KARTE_OBJ, 0,
      RE15_R35I_KARTE_X, RE15_R35I_KARTE_Y, RE15_R35I_KARTE_Z,
      RE15_R35I_KARTE_ROT_X, RE15_R35I_KARTE_ROT_Y, RE15_R35I_KARTE_ROT_Z,
      RE15_R35I_KARTE_RECT_X, RE15_R35I_KARTE_RECT_Z, RE15_R35I_KARTE_RECT_W, RE15_R35I_KARTE_RECT_D },
    { 0x1090, RE15_R35I_SCHROT_ITEM, RE15_R35I_SCHROT_MENGE, RE15_R35I_SCHROT_BIT,
      RE15_R35I_SCHROT_SLOT, RE15_R35I_SCHROT_OBJ, RE15_R35I_SCHROT_FLOOR,
      RE15_R35I_SCHROT_X, RE15_R35I_SCHROT_Y, RE15_R35I_SCHROT_Z, 0, RE15_R35I_SCHROT_ROT_Y, 0,
      RE15_R35I_SCHROT_RECT_X, RE15_R35I_SCHROT_RECT_Z, RE15_R35I_SCHROT_RECT_W, RE15_R35I_SCHROT_RECT_D },
};
#define N_GEGENSTAENDE ((int)(sizeof k_gegenstaende / sizeof k_gegenstaende[0]))

static const gegenstand_t *des_raums(uint16_t room_id)
{
    for (int k = 0; k < N_GEGENSTAENDE; k++)
        if ((uint16_t)(room_id & 0xFFFEu) == k_gegenstaende[k].raum) return &k_gegenstaende[k];
    return NULL;
}

static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

void re15_inhalt_r35_install(uint16_t room_id)
{
    const gegenstand_t *g = des_raums(room_id);
    if (!g) return;
    /* schon genommen -> weder Prop noch Zone (Item_aot_set @0x800406dc-718) */
    if (re15_game_flag_get(9, g->bit)) return;

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914) */
    if (slot_von_obj_id(g->obj) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = g->obj;
        g_scd.props[i].obj_type   = 0;                     /* pc[2] -> pool+8 @0x8004095c   */
        g_scd.props[i].band       = RE15_R35I_PROP_BAND;   /* pc[4] -> pool+130 @0x80040974 */
        g_scd.props[i].parent_obj = -1;                    /* pc[5] = 0x00 = Weltraum        */
        g_scd.props[i].x = g->x;
        g_scd.props[i].y = g->y;
        g_scd.props[i].z = g->z;
        g_scd.props[i].rot_x = g->rot_x;
        g_scd.props[i].rot_y = g->rot_y;
        g_scd.props[i].rot_z = g->rot_z;
        g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
        g_scd.props[i].vel_ry = 0;
        g_scd.props[i].flags  = RE15_R35I_PROP_FLAGS;      /* 0x000A | 1 @0x80040998         */
        g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
        g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
        g_scd.props[i].member_0b = 0;
    }

    /* ZONE — wie op_item_aot_set: Mitte + halbe Ausdehnung, sat (+3), Etage (+4). Nur frei. */
    if (g->slot < RE15_AOT_MAX && !g_aot.slots[g->slot].active) {
        re15_aot_set_item_tk_prop((int)g->slot,
                                  g->rect_x + g->rect_w / 2, g->rect_z + g->rect_d / 2,
                                  g->rect_w / 2, g->rect_d / 2,
                                  g->item, g->menge, g->bit, g->obj);
        g_aot.slots[g->slot].sce_flags = RE15_R35I_AOT_SAT;
        g_aot.slots[g->slot].band      = g->floor;
    }
}

int re15_inhalt_r35_obj_id(uint16_t room_id)
{
    const gegenstand_t *g = des_raums(room_id);
    return g ? (int)g->obj : -1;
}

const uint8_t *re15_inhalt_r35_md1_bytes(uint16_t room_id, int *out_size)
{
    const gegenstand_t *g = des_raums(room_id);
    if (out_size) *out_size = 0;
    if (!g) return NULL;
    if (g->item == RE15_R35I_KARTE_ITEM) return re15_irons_tisch_md1_bytes(RE15_IRONS_KARTE_OBJ_ID, out_size);
    if (out_size) *out_size = (int)sizeof re15_r35_schrot_md1;
    return re15_r35_schrot_md1;
}

const uint8_t *re15_inhalt_r35_tim_bytes(uint16_t room_id, int *out_size)
{
    const gegenstand_t *g = des_raums(room_id);
    if (out_size) *out_size = 0;
    if (!g) return NULL;
    if (g->item == RE15_R35I_KARTE_ITEM) return re15_irons_tisch_tim_bytes(RE15_IRONS_KARTE_OBJ_ID, out_size);
    if (out_size) *out_size = (int)sizeof re15_r35_schrot_tim;
    return re15_r35_schrot_tim;
}
