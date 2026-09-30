/*
 * RE1.5 Rebuilt — VIER NEUE DOKUMENTE (Runde 34 Nacht, Spur E).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_dokumente.h und
 * analysis/befunde_runde34_nacht/E_dokumente.md. Modelle + Texturen:
 * tools/r34n_e/dokumente_engine_export.py -> gen/dokumente_props.inc (eingebackene RE2-Bytes,
 * kein Asset-Patch). Vorbild: engine/src/irons_tisch_1150.c (anlegen()).
 */
#include "re15_dokumente.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_pri.h"

#include "gen/dokumente_props.inc"

typedef struct {
    uint16_t raum;                 /* John-/Leon-Variante; die Elza-Variante ist raum | 1   */
    uint8_t  nr, item, bit, slot, obj;
    int32_t  x, y, z;
    int16_t  rot_y;
    int32_t  rect_x, rect_z, rect_w, rect_d;
    const unsigned char *md1; int md1_n;
    const unsigned char *tim; int tim_n;
} dokument_t;

static const dokument_t k_dokumente[] = {
    { 0x1050, 1, RE15_DOK1_ITEM, RE15_DOK1_BIT, RE15_DOK1_SLOT, RE15_DOK1_OBJ,
      RE15_DOK1_X, RE15_DOK1_Y, RE15_DOK1_Z, RE15_DOK1_ROT_Y,
      RE15_DOK1_RECT_X, RE15_DOK1_RECT_Z, RE15_DOK1_RECT_W, RE15_DOK1_RECT_D,
      re15_dokument1_md1, (int)sizeof re15_dokument1_md1,
      re15_dokument1_tim, (int)sizeof re15_dokument1_tim },
    { 0x1000, 2, RE15_DOK2_ITEM, RE15_DOK2_BIT, RE15_DOK2_SLOT, RE15_DOK2_OBJ,
      RE15_DOK2_X, RE15_DOK2_Y, RE15_DOK2_Z, RE15_DOK2_ROT_Y,
      RE15_DOK2_RECT_X, RE15_DOK2_RECT_Z, RE15_DOK2_RECT_W, RE15_DOK2_RECT_D,
      re15_dokument2_md1, (int)sizeof re15_dokument2_md1,
      re15_dokument2_tim, (int)sizeof re15_dokument2_tim },
    { 0x1020, 3, RE15_DOK3_ITEM, RE15_DOK3_BIT, RE15_DOK3_SLOT, RE15_DOK3_OBJ,
      RE15_DOK3_X, RE15_DOK3_Y, RE15_DOK3_Z, RE15_DOK3_ROT_Y,
      RE15_DOK3_RECT_X, RE15_DOK3_RECT_Z, RE15_DOK3_RECT_W, RE15_DOK3_RECT_D,
      re15_dokument3_md1, (int)sizeof re15_dokument3_md1,
      re15_dokument3_tim, (int)sizeof re15_dokument3_tim },
    { 0x1010, 4, RE15_DOK4_ITEM, RE15_DOK4_BIT, RE15_DOK4_SLOT, RE15_DOK4_OBJ,
      RE15_DOK4_X, RE15_DOK4_Y, RE15_DOK4_Z, RE15_DOK4_ROT_Y,
      RE15_DOK4_RECT_X, RE15_DOK4_RECT_Z, RE15_DOK4_RECT_W, RE15_DOK4_RECT_D,
      re15_dokument4_md1, (int)sizeof re15_dokument4_md1,
      re15_dokument4_tim, (int)sizeof re15_dokument4_tim },
};
#define DOK_ANZAHL ((int)(sizeof k_dokumente / sizeof k_dokumente[0]))

static const dokument_t *dokument_des_raums(uint16_t room_id)
{
    for (int k = 0; k < DOK_ANZAHL; k++)
        if ((uint16_t)(room_id & 0xFFFEu) == k_dokumente[k].raum) return &k_dokumente[k];
    return NULL;
}

/* Pool-Index eines Props ueber seine obj_id (dieselbe Suche wie irons_tisch_1150.c). */
static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

static void anlegen(const dokument_t *d)
{
    /* Schon genommen -> weder Prop noch Zone (Item_aot_set @0x800406dc-718 legt den Satz
     * still und blendet das Modell aus; hier entsteht beides gar nicht erst). */
    if (re15_game_flag_get(9, d->bit)) return;

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914), Werte aus re15_dokumente.h. */
    if (slot_von_obj_id(d->obj) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = d->obj;
        g_scd.props[i].obj_type   = 0;                     /* pc[2] -> pool+8 @0x8004095c   */
        g_scd.props[i].band       = RE15_DOK_PROP_BAND;    /* pc[4] -> pool+130 @0x80040974 */
        g_scd.props[i].parent_obj = -1;                    /* pc[5] = 0x00 = Weltraum        */
        g_scd.props[i].x = d->x;
        g_scd.props[i].y = d->y;
        g_scd.props[i].z = d->z;
        g_scd.props[i].rot_x = 0;
        g_scd.props[i].rot_y = d->rot_y;
        g_scd.props[i].rot_z = 0;
        g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
        g_scd.props[i].vel_ry = 0;
        g_scd.props[i].flags  = RE15_DOK_PROP_FLAGS;       /* 0x000A | 1 @0x80040998         */
        g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
        g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
        g_scd.props[i].member_0b = 0;
    }

    /* ZONE — wie op_item_aot_set (scd_vm.c): Rechteck als Mitte + halbe Ausdehnung, danach
     * sat (Item_aot_set +3) und floor (+4). Nur in einen FREIEN Slot. */
    if (d->slot < RE15_AOT_MAX && !g_aot.slots[d->slot].active) {
        re15_aot_set_item_tk_prop((int)d->slot,
                                  d->rect_x + d->rect_w / 2, d->rect_z + d->rect_d / 2,
                                  d->rect_w / 2, d->rect_d / 2,
                                  d->item, RE15_DOK_MENGE, d->bit, d->obj);
        g_aot.slots[d->slot].sce_flags = RE15_DOK_AOT_SAT;
        g_aot.slots[d->slot].band      = RE15_DOK_AOT_FLOOR;
    }
}

/* TIEFEN-KLEMMEN je (Raum, Cut) — Herleitung je Eintrag in re15_dokumente.h. */
typedef struct { uint16_t raum; uint8_t cut; uint16_t tiefe; } klemme_t;
static const klemme_t k_klemmen[] = {
    { 0x1020, RE15_DOK3_KLEMME_CUT, RE15_DOK3_KLEMME_TIEFE },
    { 0x1021, RE15_DOK3_KLEMME_CUT, RE15_DOK3_KLEMME_TIEFE },
    { 0x1010, 0, RE15_DOK4_KLEMME_CUT0 },       /* nur ROOM1010: ROOM1011 hat keine MSK-Datei */
    { 0x1010, 1, RE15_DOK4_KLEMME_CUT1 },
    { 0x1010, 6, RE15_DOK4_KLEMME_CUT6 },
    { 0x1010, 8, RE15_DOK4_KLEMME_CUT8 },
};
#define KLEMMEN_ANZAHL ((int)(sizeof k_klemmen / sizeof k_klemmen[0]))

/* VORRANG ROOM1020/1021: die Tisch-Nachricht in den Slot RE15_DOK3_NACHRICHT_ZIEL umziehen
 * (Herleitung und Satz-Waechter: re15_dokumente.h, "VORRANG"). Nur der unveraenderte Original-
 * Satz zieht um, nur in einen freien Ziel-Slot; alle Parallel-Felder des Slots ziehen mit. */
static void tisch_nachricht_umziehen(uint16_t room_id)
{
    const int q   = (room_id == 0x1020) ? RE15_DOK3_NACHRICHT_1020 : RE15_DOK3_NACHRICHT_1021;
    const int msg = (room_id == 0x1020) ? RE15_DOK3_MSG_1020 : RE15_DOK3_MSG_1021;
    const int z   = RE15_DOK3_NACHRICHT_ZIEL;
    const re15_aot_t *a = &g_aot.slots[q];
    if (!a->active || a->type != RE15_AOT_TYPE_MESSAGE || a->event_id != msg ||
        a->sce_flags != 0x31 || a->pause_mask16 != 0xFFFFu ||
        a->x != RE15_DOK3_NACHRICHT_RECT_X + RE15_DOK3_NACHRICHT_RECT_W / 2 ||
        a->z != RE15_DOK3_NACHRICHT_RECT_Z + RE15_DOK3_NACHRICHT_RECT_D / 2 ||
        a->half_w != RE15_DOK3_NACHRICHT_RECT_W / 2 ||
        a->half_h != RE15_DOK3_NACHRICHT_RECT_D / 2) return;
    if (g_aot.slots[z].active) return;
    g_aot.slots[z]        = g_aot.slots[q];
    g_aot.door_params[z]  = g_aot.door_params[q];
    g_aot.item_params[z]  = g_aot.item_params[q];
    g_aot.flag_params[z]  = g_aot.flag_params[q];
    g_aot.env_params[z]   = g_aot.env_params[q];
    g_aot.stair_params[z] = g_aot.stair_params[q];
    memset(&g_aot.slots[q],        0, sizeof g_aot.slots[q]);
    memset(&g_aot.door_params[q],  0, sizeof g_aot.door_params[q]);
    memset(&g_aot.item_params[q],  0, sizeof g_aot.item_params[q]);
    memset(&g_aot.flag_params[q],  0, sizeof g_aot.flag_params[q]);
    memset(&g_aot.env_params[q],   0, sizeof g_aot.env_params[q]);
    memset(&g_aot.stair_params[q], 0, sizeof g_aot.stair_params[q]);
}

void re15_dokumente_install(uint16_t room_id)
{
    const dokument_t *d = dokument_des_raums(room_id);
    if (!d) return;
    if (d->nr == 3 && !re15_game_flag_get(9, d->bit))
        tisch_nachricht_umziehen(room_id);
    anlegen(d);
}

int re15_dokumente_obj_id(uint16_t room_id)
{
    const dokument_t *d = dokument_des_raums(room_id);
    return d ? (int)d->obj : -1;
}

int re15_dokumente_nr(uint16_t room_id)
{
    const dokument_t *d = dokument_des_raums(room_id);
    return d ? (int)d->nr : 0;
}

int re15_dokumente_sort_max_mit(int irons, uint16_t room_id, int cut, int obj_id)
{
    const dokument_t *d = dokument_des_raums(room_id);
    if (!d || obj_id != (int)d->obj) return irons;
    for (int k = 0; k < KLEMMEN_ANZAHL; k++)
        if (k_klemmen[k].raum == room_id && (int)k_klemmen[k].cut == cut)
            return (int)re15_pri_mask_camera_z((int)k_klemmen[k].tiefe) - 1;
    return irons;
}

const uint8_t *re15_dokumente_md1_bytes(uint16_t room_id, int *out_size)
{
    const dokument_t *d = dokument_des_raums(room_id);
    if (out_size) *out_size = d ? d->md1_n : 0;
    return d ? d->md1 : NULL;
}

const uint8_t *re15_dokumente_tim_bytes(uint16_t room_id, int *out_size)
{
    const dokument_t *d = dokument_des_raums(room_id);
    if (out_size) *out_size = d ? d->tim_n : 0;
    return d ? d->tim : NULL;
}
