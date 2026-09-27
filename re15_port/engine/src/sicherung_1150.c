/*
 * RE1.5 Rebuilt — die SICHERUNG im Hebetisch von Irons' Buero.
 *
 * Herleitung, Belege und alle Konstanten: include/re15_sicherung.h.
 * Modell + Textur: tools/sicherung_modell.py -> tools/sicherung_engine_export.py
 * -> gen/sicherung_prop.inc (eingebackene MD1/TIM-Bytes, kein Asset-Patch).
 */
#include "re15_sicherung.h"

#include "re15_scd.h"
#include "re15_item_modal.h"

#include "gen/sicherung_prop.inc"

/* Der Hebetisch selbst ist Prop obj_id 0 des Raums (main00 @0x0E00). */
#define PLATTFORM_OBJ_ID   0

/* ⛔ FENSTER, NICHT EINSEITIGE SCHRANKE. Die Plattform muss IM RAUM *und* OBEN sein.
 * Eine blosse Schranke `y <= -1100` reicht NICHT: die Parkposition y=-20324
 * (main00 @0x0E00) unterschreitet sie ebenfalls, und das Modal ginge dann schon beim
 * Betreten des Raums auf, bevor der Tisch ueberhaupt erscheint. Genau das hat die Sonde
 * probe_sicherung_1150 gemessen — "Modal aufgemacht in Bild 0".
 * Gemessene Fahrt (dieselbe Sonde): geparkt -20324; Pos_set holt sie auf -305; der
 * Hochpunkt ist -1205; danach zurueck auf -305. Das Fenster faengt nur den Hochpunkt. */
#define IM_RAUM_AB         (-5000)    /* alles darunter ist die Parkposition */
#define OBEN_BIS           (-1100)    /* Hochpunkt -1205, Startlage -305     */

static uint8_t s_raum_aktiv;        /* Prop in diesem Raum angelegt?              */
static uint8_t s_modal_ausgeloest;  /* in diesem Raumaufenthalt schon aufgemacht? */


const uint8_t *re15_sicherung_md1_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_sicherung_md1);
    return re15_sicherung_md1;
}

const uint8_t *re15_sicherung_tim_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_sicherung_tim);
    return re15_sicherung_tim;
}

static int ist_irons_buero(uint16_t room_id)
{
    return room_id == 0x1150 || room_id == 0x1151;   /* Elza- und John-Variante */
}

/* Pool-Index eines Props ueber seine obj_id (dieselbe Suche wie pc_prop_world). */
static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

void re15_sicherung_install(uint16_t room_id)
{
    s_raum_aktiv = 0;
    s_modal_ausgeloest = 0;

    if (!ist_irons_buero(room_id)) return;
    /* Schon genommen -> gar nicht erst anlegen (dieselbe Regel, die auch das Original
     * fuer aufgenommene Welt-Modelle fuehrt, s. s_prop_taken_hidden in scd_vm.c). */
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return;
    /* Ohne die Plattform gibt es nichts, woran sich die Sicherung haengen koennte. */
    if (slot_von_obj_id(PLATTFORM_OBJ_ID) < 0) return;
    if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return;
    if (slot_von_obj_id(RE15_SICHERUNG_OBJ_ID) >= 0) return;   /* schon da */

    int i = (int)g_scd.prop_count++;
    g_scd.props[i].active   = 1;
    g_scd.props[i].obj_id   = RE15_SICHERUNG_OBJ_ID;
    g_scd.props[i].obj_type = 0;      /* Mesh-Zweig von FUN_8002c18c (kein Sprite-Gitter) */
    g_scd.props[i].band     = 1;      /* Etagenband wie die vier Props des Raums          */
    /* ⛔ DAS IST DER KERN: die Sicherung haengt an der Elternmatrix der Plattform —
     * genau die Anhaenge-Form (Obj_model_set pc[5] = 0xC0), die im ganzen Spiel nur
     * die beiden Deckelhaelften dieses Tisches benutzen. Dadurch faehrt sie mit,
     * statt in der Luft stehen zu bleiben. */
    g_scd.props[i].parent_obj = (int8_t)PLATTFORM_OBJ_ID;
    g_scd.props[i].x = RE15_SICHERUNG_POS_X;
    g_scd.props[i].y = RE15_SICHERUNG_POS_Y;
    g_scd.props[i].z = RE15_SICHERUNG_POS_Z;
    g_scd.props[i].rot_x = 0;         /* liegend ist schon im Modell (Laengsachse X) */
    g_scd.props[i].rot_y = 0;
    g_scd.props[i].rot_z = 0;
    g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
    g_scd.props[i].vel_ry = 0;
    g_scd.props[i].flags = 0x0001;    /* aktiv, NICHT kletterbar (kein 0x100)        */
    g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
    g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
                                      /* Nullbox = nicht kollidierbar: der Gegenstand
                                       * liegt auf der Plattform, er ist kein Hindernis */
    g_scd.props[i].member_0b = 0;
    s_raum_aktiv = 1;
}

int re15_sicherung_tick(void)
{
    if (!s_raum_aktiv || s_modal_ausgeloest) return 0;
    if (re15_item_modal_active()) return 0;
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return 0;

    int p = slot_von_obj_id(PLATTFORM_OBJ_ID);
    if (p < 0 || !g_scd.props[p].active) return 0;
    /* +Y zeigt nach unten: OBEN heisst KLEINERES y. Beide Seiten pruefen (s.o.). */
    int32_t py = g_scd.props[p].y;
    if (py <= IM_RAUM_AB || py > OBEN_BIS) return 0;

    /* aot_slot -1 = es gibt keine AOT-Zone, die abgeschaltet werden muesste (die
     * Uebergabe kommt aus der Szene, nicht aus einem Aufsammel-Rechteck).
     * taken_bit setzt das Zone-9-Flag, taken_prop blendet das Welt-Modell aus —
     * beides erledigt das Modal beim Bestaetigen (item_modal_common.c:340/360). */
    re15_item_modal_start(RE15_SICHERUNG_ITEM, 1, RE15_SICHERUNG_TAKEN_BIT,
                          -1, RE15_SICHERUNG_OBJ_ID);
    s_modal_ausgeloest = 1;
    return 1;
}
