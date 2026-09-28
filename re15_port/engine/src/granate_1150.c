/*
 * RE1.5 Rebuilt — die HANDGRANATE im Hebetisch von Irons' Buero (Runde 30, Nachtrag K).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_granate.h.
 * Modell + Textur: tools/granate_engine_export.py -> gen/granate_prop.inc
 * (die Waffen-Flaechen aus PLD/PL00W09.PLW, kein Asset-Patch).
 * Vorbild und gemeinsame Regeln (Fenster, Sperre je Fahrt): engine/src/sicherung_1150.c.
 */
#include "re15_granate.h"

#include "re15_scd.h"
#include "re15_item_modal.h"
#include "re15_sicherung.h"
#include "re15_hebetisch.h"   /* Runde 31: Ruhe oben von sub04 = Zeitpunkt des Modals */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include "re15_inventory.h"   /* nur fuer die Logzeile: Inventar-Platz nach "Yes" */
#endif

#include "gen/granate_prop.inc"

/* Der Hebetisch ist Prop obj_id 0 des Raums (main00 @0x0E00), wie bei der Sicherung. */
#define PLATTFORM_OBJ_ID   0

/* Zeitpunkt wie die Sicherung (Runde 31): erst in der RUHE OBEN von sub04, Fenster
 * [Sleep 30 @0x101A, For-Abfahrt @0x1042) (re15_hebetisch_ruht_oben, include/re15_hebetisch.h);
 * die fruehere y-Schranke OBEN_BIS -1100 (mitten im Hub) ist entfernt.
 * IM_RAUM_AB nur fuer das Wiederscharfmachen in der Parklage: -20324 (main00 @0x0E00) / -20224
 * (Pos_set @0x109E) liegen darunter, die Fahrt bleibt in [-1215, -301].
 * ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE (Herleitung sicherung_1150.c). */
#define IM_RAUM_AB         (-5000)

static uint8_t s_raum_aktiv;        /* Prop in diesem Raum angelegt?                 */
/* SPERRE JE FAHRT wie die Sicherung (sicherung_1150.c, s_modal_ausgeloest): hoechstens ein
 * Modal je Fahrt, nach "No" bietet die NAECHSTE Fahrt die Granate wieder an; wieder scharf in
 * der Parklage. ⛔ Port-Wahl, keine Original-Adresse (Herleitung dort). */
static uint8_t s_modal_ausgeloest;
#ifdef RE15_PLATFORM_PC
static uint8_t s_modal_offen;       /* nur Logzeile: unser Modal laeuft gerade        */
#endif

const uint8_t *re15_granate_md1_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_granate_md1);
    return re15_granate_md1;
}

const uint8_t *re15_granate_tim_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_granate_tim);
    return re15_granate_tim;
}

static int ist_irons_buero(uint16_t room_id)
{
    return room_id == 0x1150 || room_id == 0x1151;   /* Elza- und John-Variante */
}

static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

void re15_granate_install(uint16_t room_id)
{
    s_raum_aktiv = 0;
    s_modal_ausgeloest = 0;
#ifdef RE15_PLATFORM_PC
    s_modal_offen = 0;
#endif

    if (!ist_irons_buero(room_id)) return;
    if (re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT)) return;   /* schon genommen */
    if (slot_von_obj_id(PLATTFORM_OBJ_ID) < 0) return;           /* kein Hebetisch */
    if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return;
    if (slot_von_obj_id(RE15_GRANATE_OBJ_ID) >= 0) return;       /* schon da */

    int i = (int)g_scd.prop_count++;
    g_scd.props[i].active   = 1;
    g_scd.props[i].obj_id   = RE15_GRANATE_OBJ_ID;
    g_scd.props[i].obj_type = 0;      /* Mesh-Zweig von FUN_8002c18c                        */
    g_scd.props[i].band     = 1;      /* Etagenband wie die Props des Raums                 */
    /* An der Elternmatrix der Plattform (Obj_model_set pc[5] = 0xC0, wie die Deckelhaelften
     * und die Sicherung): dadurch faehrt die Granate mit hoch. */
    g_scd.props[i].parent_obj = (int8_t)PLATTFORM_OBJ_ID;
    g_scd.props[i].x = RE15_GRANATE_POS_X;
    g_scd.props[i].y = RE15_GRANATE_POS_Y;
    g_scd.props[i].z = RE15_GRANATE_POS_Z;
    g_scd.props[i].rot_x = 0;         /* liegend ist schon im Modell (Laengsachse X)        */
    g_scd.props[i].rot_y = RE15_GRANATE_ROT_Y;   /* Port-Wahl, Herleitung re15_granate.h    */
    g_scd.props[i].rot_z = 0;
    g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
    g_scd.props[i].vel_ry = 0;
    g_scd.props[i].flags = 0x0001;    /* aktiv, nicht kletterbar                            */
    g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
    g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
                                      /* Nullbox: liegt auf der Plattform, kein Hindernis
                                       * (121 von 121 Item-Weltmodellen des Spiels tragen eine
                                       * Null-Kollisionsbox, irons-diary-welt.md §3.4) */
    g_scd.props[i].member_0b = 0;
    s_raum_aktiv = 1;
}

int re15_granate_tick(void)
{
    if (!s_raum_aktiv) return 0;
    int p = slot_von_obj_id(PLATTFORM_OBJ_ID);
    if (p < 0) return 0;
    int32_t py = g_scd.props[p].y;    /* +Y nach unten: OBEN heisst KLEINERES y */

#ifdef RE15_PLATFORM_PC
    if (s_modal_offen && !re15_item_modal_active()) {
        s_modal_offen = 0;
        if (re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT))
            fprintf(stderr, "[granate] Yes: genommen, Flag (9,%d) gesetzt, Item 0x%02X x%d in "
                            "Inventar-Platz %d\n", RE15_GRANATE_TAKEN_BIT, RE15_GRANATE_ITEM,
                    RE15_GRANATE_MENGE, re15_inv_find_item(RE15_GRANATE_ITEM));
        else
            fprintf(stderr, "[granate] No/voll: nicht genommen, Granate bleibt liegen "
                            "(Hebetisch y=%d), die naechste Fahrt bietet sie wieder an\n",
                    (int)py);
    }
#endif

    /* WIEDER SCHARF JE FAHRT in der Parklage (Pos_set @0x109E) — Port-Wahl wie die Sicherung. */
    if (py <= IM_RAUM_AB) {
        if (s_modal_ausgeloest) {
            s_modal_ausgeloest = 0;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[granate] Fahrt zu Ende, Hebetisch in der Parklage y=%d: "
                            "Sperre geloest\n", (int)py);
#endif
        }
        return 0;
    }
    if (s_modal_ausgeloest) return 0;          /* hoechstens EIN Modal je Fahrt */
    if (re15_item_modal_active()) return 0;    /* auch nicht, solange das der Sicherung laeuft */
    if (re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT)) return 0;
    if (!g_scd.props[p].active) return 0;
    /* Runde 31: erst in der RUHE OBEN (sub04-PC in [@0x101A, @0x1042), include/re15_hebetisch.h).
     * Die Aufnahme der Sicherung haelt das Skript an (FUN_8001db28 @0x8001dbc8 g_pauseflags |=
     * 0xFF000000, SCD-Laeufer @0x8003f04c) — die Ruhe (30 + 10 Skriptbilder) dauert also noch,
     * wenn deren Dialog zu ist, und beide Dialoge liegen an der ruhenden Plattform. */
    if (!re15_hebetisch_ruht_oben()) return 0;
    /* ERST DIE SICHERUNG (Auftrag): solange sie in dieser Fahrt noch ein Modal aufmachen kann,
     * wartet die Granate. ⛔ Port-Wahl, keine Original-Adresse. */
    if (re15_sicherung_fahrt_offen()) return 0;

    /* aot_slot -1: keine AOT-Zone (die Uebergabe kommt aus der Szene). taken_bit setzt das
     * Zone-9-Flag, taken_prop blendet das Weltmodell aus — beides beim Bestaetigen
     * (item_modal_common.c, Zustand 7). */
    re15_item_modal_start(RE15_GRANATE_ITEM, RE15_GRANATE_MENGE, RE15_GRANATE_TAKEN_BIT,
                          -1, RE15_GRANATE_OBJ_ID);
    s_modal_ausgeloest = 1;
#ifdef RE15_PLATFORM_PC
    s_modal_offen = 1;
    fprintf(stderr, "[granate] Modal auf (Hebetisch y=%d, Ruhe oben: sub04-PC @0x%04lX), "
                    "Sperre bis zum Ende dieser Fahrt\n", (int)py, re15_hebetisch_ruhe_pc_off());
#endif
    return 1;
}
